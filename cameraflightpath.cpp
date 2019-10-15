#include "cameraflightpath.h"
#include "liutils.h"
#include "lientity.h"
#include "cesiummath.h"
#include "cesiumcartesian3.h"
#include "tweenjs.h"
#include "litransform.h"
#include "tweencollection.h"
#include "cameracontroller.h"
#include "screenspacecameracontroller.h"
#include "licamera.h"

CameraFlightPath::CameraFlightPath()
{
}

Tween *CameraFlightPath::createTween(LiCamera *camera, CameraController *controller, const CameraNewOptions &options)
{
    LiTransform *cameraTrans = camera->transform();

    Tween *tween;
    Cartesian3 destination = options.destination;
    TweenAction1Double easingFunction = options.easingFunction;

    double duration = options.duration;
    if(!defined(duration)) {
        duration = ceil((cameraTrans->worldPosition() - destination).magnitude() / 1000000.0) + 2.0;
        duration = std::min(duration, 3.0);
    }
    double heading = options.heading;
    double pitch = options.pitch;
    double roll = options.roll;

    ScreenSpaceCameraController *screenSpaceCameraController = qobject_cast<ScreenSpaceCameraController*>(camera->cameraController());
    screenSpaceCameraController->_enableInputs = false;

    TweenAction complete = wrapCallback(screenSpaceCameraController, options.complete);
    TweenAction cancel = wrapCallback(screenSpaceCameraController, options.cancel);

    bool empty = false;
    empty = empty || (CesiumCartesian3::equalsEpsilon(destination, cameraTrans->worldPosition(), Math::EPSILON10));

    empty = empty &&
            CesiumMath::equalsEpsilon(CesiumMath::negativePiToPi(heading), CesiumMath::negativePiToPi(controller->heading()), Math::EPSILON10) &&
            CesiumMath::equalsEpsilon(CesiumMath::negativePiToPi(pitch), CesiumMath::negativePiToPi(controller->pitch()), Math::EPSILON10) &&
            CesiumMath::equalsEpsilon(CesiumMath::negativePiToPi(roll), CesiumMath::negativePiToPi(controller->roll()), Math::EPSILON10);

    if (empty) {
        tween = new Tween();
        tween->_complete = complete;
        tween->_cancle = cancel;
        return tween;
    }

    if (duration <= 0.0) {
        TweenAction newOnComplete = [=]() {
            TweenAction1 update = createUpdate3D(camera, controller, 1.0, destination, heading, pitch, roll);
            update(1.0);

            if(complete)
                complete();
        };
        tween = new Tween();
        tween->_complete = newOnComplete;
        tween->_cancle = cancel;
        return tween;
    }

    TweenAction1 update = createUpdate3D(camera, controller, duration, destination, heading, pitch, roll);

    double startHeight = controller->positionCartographic().height;
    double endHeight = cartesianToCartographic(destination).height;

    if (startHeight > endHeight && startHeight > 11500.0) {
        easingFunction = [=](double k) {
            return TweenJS::EasingCubicOut(k);
        };
    } else {
        easingFunction = [=](double k) {
            return TweenJS::EasingQuinticInOut(k);
        };
    }

    tween = new Tween();
    tween->_duration = duration;
    tween->_easingFunction = easingFunction;
    tween->_startObject = 0.0;
    tween->_stopObject = duration;
    tween->_update = update;
    tween->_complete = complete;
    tween->_cancle = cancel;

    return tween;
}

TweenAction CameraFlightPath::wrapCallback(ScreenSpaceCameraController *controller, const TweenAction &action)
{
    TweenAction result = [=]() {
        controller->_enableInputs = true;
        if(action)
            action();
    };
    return result;
}

TweenAction1 CameraFlightPath::createUpdate3D(LiCamera *camera, CameraController *controller, double duration, const Cartesian3 &destination, double heading, double pitch, double roll)
{
    Cartographic startCart = controller->positionCartographic();
    double startPitch = controller->pitch();
    double startHeading = adjustAngleForLERP(controller->heading(), heading);
    double startRoll = adjustAngleForLERP(controller->roll(), roll);

    Cartographic destCart = cartesianToCartographic(destination);

    startCart.longitude = CesiumMath::zeroToTwoPi(startCart.longitude);
    destCart.longitude = CesiumMath::zeroToTwoPi(destCart.longitude);

    double twoPI = 2.0 * M_PI;
    double diff = startCart.longitude - destCart.longitude;
    if (diff < -M_PI) {
        startCart.longitude += twoPI;
    } else if (diff > M_PI) {
        destCart.longitude += twoPI;
    }

    TweenAction1Double heightFunction = createHeightFunction(camera, destination, startCart.height, destCart.height);

    // Isolate scope for update function.
    // to have local copies of vars used in lerp
    // Othervise, if you call nex
    // createUpdate3D (createAnimationTween)
    // before you played animation, variables will be overwriten.

    double startLongitude = startCart.longitude;
    double destLongitude = destCart.longitude;
    double startLatitude = startCart.latitude;
    double destLatitude = destCart.latitude;

    TweenAction1 update = [=](double value) {
        double time = value / duration;
        Cartesian3 position = CesiumCartesian3::fromRadians(
                    CesiumMath::lerp(startLongitude, destLongitude, time),
                    CesiumMath::lerp(startLatitude, destLatitude, time),
                    heightFunction(time)
                    );

        controller->setView(position,
                            CesiumMath::lerp(startHeading, heading, time),
                            CesiumMath::lerp(startPitch, pitch, time),
                            CesiumMath::lerp(startRoll, roll, time));
    };
    return update;
}

double CameraFlightPath::adjustAngleForLERP(double startAngle, double endAngle)
{
    double twoPI = 2.0 * M_PI;
    if (CesiumMath::equalsEpsilon(startAngle, twoPI, Math::EPSILON11)) {
        startAngle = 0.0;
    }

    if (endAngle > startAngle + M_PI) {
        startAngle += twoPI;
    } else if (endAngle < startAngle - M_PI) {
        startAngle -= twoPI;
    }

    return startAngle;
}

TweenAction1Double CameraFlightPath::createHeightFunction(LiCamera *camera, const Cartesian3 &destination, double startHeight, double endHeight)
{
    LiTransform *cameraTrans = camera->transform();

    double maxHeight = std::max(startHeight, endHeight);

    Cartesian3 start = cameraTrans->worldPosition();
    Cartesian3 end = destination;
    Cartesian3 up = cameraTrans->zaxis();
    Cartesian3 right = cameraTrans->xaxis();

    Cartesian3 diff = start - end;
    double verticalDistance = (up * Cartesian3::dot(diff, up)).magnitude();
    double horizontalDistance = (right * Cartesian3::dot(diff, right)).magnitude();

    double altitude = std::min(getAltitude(camera, verticalDistance, horizontalDistance) * 0.20, 1000000000.0);

    if (maxHeight < altitude) {
        double power = 8.0;
        double factor = 1000000.0;

        double s = -pow((altitude - startHeight) * factor, 1.0 / power);
        double e = pow((altitude - endHeight) * factor, 1.0 / power);

        TweenAction1Double result = [=](double t) {
            double x = t * (e - s) + s;
            return -pow(x, power) / factor + altitude;
        };
        return result;
    }

    TweenAction1Double result = [=](double t) {
        return CesiumMath::lerp(startHeight, endHeight, t);
    };
    return result;
}

double CameraFlightPath::getAltitude(LiCamera *camera, double dx, double dy)
{
    double near1;
    double top;
    double right;

    double tanTheta = tan(0.5 * Math::toRadians(camera->fovy()));
    near1 = camera->nearPlane();
    top = near1 * tanTheta;
    right = camera->aspectRatio() * top;
    return std::max(dx * near1 / right, dy * near1 / top);
}
