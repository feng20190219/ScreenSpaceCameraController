#include "cameracontroller.h"
#include "intersectiontests.h"
#include "litransform.h"
#include "screenspaceeventutils.h"
#include "liraycasthit.h"
#include "liutils.h"
#include "cameraflightpath.h"
#include "cesiummath.h"
#include "cesiumcartesian3.h"
#include "ellipsoidgeodesic.h"
#include "liscene.h"
#include "licamera.h"
#include "ellipsoid.h"
#include "globe.h"
#include "tweencollection.h"
#include "liwidget.h"

CameraController::CameraController(QObject *parent) :QObject(parent)
{
}

CameraController::~CameraController()
{
}

CameraController::CameraController(LiScene *scene, LiCamera *camera, TweenCollection *tweens, QObject *parent)
    : QObject(parent)
{
    m_scene = scene;
    m_camera = camera;
    m_tweens = tweens;
    m_globe = scene->globe();
    m_cameraTrans = m_camera->transform();

    _position = Cartesian3(2033992.677662228, -15449708.24660572, 10948396.652844096);
    _positionWC = Cartesian3(2033992.677662228, -15449708.24660572, 10948396.652844096);

    _positionCartographic = Cartographic(-1.439896632895322, 0.613515261557082, 12673564.865952782);

    _direction = Cartesian3(-0.10668226241650887, 0.8103322335050215, -0.5761775474872814);
    _directionWC = Cartesian3(-0.10668226241650887, 0.8103322335050215, -0.5761775474872814);

    _up = Cartesian3(-0.07520626131620274, 0.5712482686952298, 0.8173245584047634);
    _upWC = Cartesian3(-0.07520626131620274, 0.5712482686952298, 0.8173245584047634);

    _right = Cartesian3(0.9914448613738105, 0.13052619222005152, 0);
    _rightWC = Cartesian3(0.9914448613738105, 0.13052619222005152, 0);

    m_cameraTrans->setWorldPosition(_position);
    m_cameraTrans->setAxes(_right, _direction, _up);
}

void CameraController::setView(const Cartesian3 &destination, double heading, double pitch, double roll)
{    
   _suspendTerrainAdjustment = true;

   setView3D(destination, heading, pitch, roll);
}

void CameraController::_setTransform(const Matrix4 &transform)
{
    Cartesian3 positionCarte = positionWC();
    Cartesian3 upCarte = upWC();
    Cartesian3 directionCarte = directionWC();

    _transform = transform;
    _transformChanged = true;
    updateMembers();

    Cartesian3 direction = multiplyByPointAsVector(_actualInvTransform, directionCarte).normalize();
    Cartesian3 up = multiplyByPointAsVector(_actualInvTransform, upCarte).normalize();
    Cartesian3 right = Cartesian3::cross(direction, up).normalize();

    m_cameraTrans->setWorldPosition(multiplyByPoint(_actualInvTransform, positionCarte));
    m_cameraTrans->setAxes(right, direction, up);

    updateMembers();
}

Cartesian3 CameraController::pickPoint(double x, double y/*, bool includeTerrainSurface*/)
{
    Ray ray = getPickRay(x, y);

    LiRaycastHit raycastHit;
    if (m_scene->raycast(ray, &raycastHit)) {
        return raycastHit.point();
    }
    else {
        Cartesian3 o;
//        m_globe->pick(ray, &o);
//        if (includeTerrainSurface) {
//            Cartographic cart = cartesianToCartographic(o);

//            cart.height = 0;
//            double h = m_globe->getHeight(cart,includeTerrainSurface);

//            cart.height = h;
//            o = cartographicToCartesian(cart);
//        }
        return o;
    }
}

Cartesian3 CameraController::pickEllipsoid(const Cartesian2 &windowPosition, Ellipsoid *ellipsoid, Cartesian3 &result)
{
    pickEllipsoid3D(windowPosition, ellipsoid, result);
    return result;
}

void CameraController::pickEllipsoid3D(const Cartesian2 &windowPosition, Ellipsoid *ellipsoid, Cartesian3 &result)
{
    Ray ray = getPickRay(windowPosition.x, windowPosition.y);
    Interval intersection = IntersectionTests::rayEllipsoid(ray, ellipsoid);
    if (!defined(intersection)) {
        result = Cartesian3(Math::EPSILON20, 0, 0);
        return;
    }
    double t = intersection.start > 0.0 ? intersection.start : intersection.stop;
    result = ray.getPoint(t);
}

Ray CameraController::getPickRay(double x, double y)
{
    return getPickRayPerspective(x, y);
}

void CameraController::rotate(const Vector3 &axis, double angle)
{
    angle = Math::toDegrees(angle);
    Quaternion quaternion = Quaternion::fromAxisAndAngle(axis, -angle);
    Matrix3 rotation = quaternion.toRotationMatrix();

    m_cameraTrans->setWorldPosition(rotation * m_cameraTrans->worldPosition());

    Cartesian3 direction = m_cameraTrans->yaxis();
    Cartesian3 up = m_cameraTrans->zaxis();
    Cartesian3 right;

    direction = (rotation * direction).normalize();
    up = (rotation * up).normalize();
    right = Cartesian3::cross(direction, up).normalize();
    up = Cartesian3::cross(right, direction).normalize();

    m_cameraTrans->setAxes(right, direction, up);
}

void CameraController::rotateUp(double angle)
{
    rotateVertical(-angle);
}

void CameraController::rotateDown(double angle)
{
    rotateVertical(angle);
}

void CameraController::rotateRight(double angle)
{
    rotateHorizontal(-angle);
}

void CameraController::rotateLeft(double angle)
{
    rotateHorizontal(angle);
}

void CameraController::move(const Vector3 &dir, double amount)
{
    Cartesian3 moveScratch = dir * amount;
    m_cameraTrans->setWorldPosition(m_cameraTrans->worldPosition() + moveScratch);
}

void CameraController::moveForward(double amount)
{
    move(m_cameraTrans->yaxis(), amount);
}

void CameraController::moveBackward(double amount)
{
    move(m_cameraTrans->yaxis(), -amount);
}

void CameraController::moveUp(double amount)
{
    move(m_cameraTrans->zaxis(), amount);
}

void CameraController::moveDown(double amount)
{
    move(m_cameraTrans->zaxis(), -amount);
}

void CameraController::moveRight(double amount)
{
    move(m_cameraTrans->xaxis(), amount);
}

void CameraController::moveLeft(double amount)
{
    move(m_cameraTrans->xaxis(), -amount);
}

void CameraController::look(const Vector3 &axis, double angle)
{
    angle = Math::toDegrees(angle);
    Quaternion quaternion = Quaternion::fromAxisAndAngle(axis, -angle);
    Matrix3 rotation = quaternion.toRotationMatrix();

    Cartesian3 direction = m_cameraTrans->yaxis();
    Cartesian3 up = m_cameraTrans->zaxis();
    Cartesian3 right = m_cameraTrans->xaxis();

    direction = rotation * direction;
    up = rotation * up;
    right = rotation * right;

    m_cameraTrans->setAxes(right.normalize(), direction.normalize(), up.normalize());
}

void CameraController::lookUp(double amount)
{
    // only want view of map to change in 3D mode, 2D visual is incorrect when look changes
    look(m_cameraTrans->xaxis(), -amount);
}

void CameraController::lookDown(double amount)
{
    // only want view of map to change in 3D mode, 2D visual is incorrect when look changes
    look(m_cameraTrans->xaxis(), amount);
}

void CameraController::lookRight(double amount)
{
    // only want view of map to change in 3D mode, 2D visual is incorrect when look changes
    look(m_cameraTrans->zaxis(), amount);
}

void CameraController::lookLeft(double amount)
{
    // only want view of map to change in 3D mode, 2D visual is incorrect when look changes
    look(m_cameraTrans->zaxis(), -amount);
}

void CameraController::zoomIn(double amount)
{
    zoom3D(amount);
}

void CameraController::zoomOut(double amount)
{
    zoom3D(-amount);
}

Cartesian3 CameraController::getRectangleCameraCoordinates(const LiRectangle &rectangle)
{
    return rectangleCameraPosition3D(rectangle);
}

void CameraController::cancelFlight()
{
    if(_currentFlight) {
        _currentFlight->cancelTween(m_tweens);
        delete _currentFlight;
        _currentFlight = nullptr;
    }
}

void CameraController::flyTo(const Vector3 &destination, double duration, double heading, double pitch, double roll)
{
    heading = Math::toRadians(heading);
    pitch = Math::toRadians(pitch);
    roll = Math::toRadians(roll);

    cancelFlight();

    Tween *flightTween = nullptr;

    CameraNewOptions newOptions;

    newOptions.destination = destination;
    newOptions.heading = heading;
    newOptions.pitch = pitch;
    newOptions.roll = roll;
    newOptions.duration = duration;
    newOptions.complete = [=]() {
        emit m_camera->completeFlight();
        if(_currentFlight)
            _currentFlight = nullptr;
    };

    flightTween = m_tweens->add(CameraFlightPath::createTween(m_camera, this, newOptions));
    _currentFlight = flightTween;
}

void CameraController::flyTo(const Cartographic &destination, double duration, double heading, double pitch, double roll)
{
    Cartesian3 cartesian = cartographicToCartesian(destination);
    flyTo(cartesian, duration, heading, pitch, roll);
}

void CameraController::flyTo(const LiRectangle &destination, double duration, double heading, double pitch, double roll)
{
    Cartesian3 cartesian = getRectangleCameraCoordinates(destination);
    flyTo(cartesian, duration, heading, pitch, roll);
}

Matrix4 CameraController::invTransform()
{
    updateMembers();
    return _invTransform;
}

Cartesian3 CameraController::positionWC()
{
    updateMembers();
    return _positionWC;
}

Cartesian3 CameraController::directionWC()
{
    updateMembers();
    return _directionWC;
}

Cartesian3 CameraController::upWC()
{
    updateMembers();
    return _upWC;
}

Cartesian3 CameraController::rightWC()
{
    updateMembers();
    return _rightWC;
}

Cartographic CameraController::positionCartographic()
{
    updateMembers();
    return _positionCartographic;
}

void CameraController::worldToCameraCoordinates(Cartesian3 &cartesian)
{
    updateMembers();
    cartesian = _actualInvTransform * cartesian;
}

double CameraController::heading()
{
    Matrix4 oldTransform = _transform;
    Matrix4 transform = eastNorthUpToFixedFrame(positionWC());
    _setTransform(transform);

    double heading = getHeading(m_cameraTrans->yaxis(), m_cameraTrans->zaxis());

    _setTransform(oldTransform);

    return heading;
}

double CameraController::pitch()
{
    Matrix4 oldTransform = _transform;
    Matrix4 transform = eastNorthUpToFixedFrame(positionWC());
    _setTransform(transform);

    double pitch = getPitch(m_cameraTrans->yaxis());

    _setTransform(oldTransform);

    return pitch;
}

double CameraController::roll()
{
    Matrix4 oldTransform = _transform;
    Matrix4 transform = eastNorthUpToFixedFrame(positionWC());
    _setTransform(transform);

    double roll = getRoll(m_cameraTrans->yaxis(), m_cameraTrans->zaxis(), m_cameraTrans->xaxis());

    _setTransform(oldTransform);

    return roll;
}

Ray CameraController::getPickRayPerspective(double wx, double wy)
{
    Ray ray;

    LiWidget *canvas = m_scene->canvas();
    int width = canvas->width();
    int height = canvas->height();

    double tanPhi = tan(Math::toRadians(m_camera->fovy()) * 0.5);
    double tanTheta = m_camera->aspectRatio() * tanPhi;
    double near1 = m_camera->nearPlane();

    double x = (2.0 / width) * wx - 1.0;
    double y = (2.0 / height) * (height - wy) - 1.0;

    Cartesian3 position = positionWC();
    ray.origin = position;

    Cartesian3 nearCenter = directionWC() * near1;
    nearCenter += position;
    Cartesian3 xDir = rightWC() * (x * near1 * tanTheta);
    Cartesian3 yDir = upWC() * (y * near1 * tanPhi);
    ray.direction = nearCenter + xDir;
    ray.direction += yDir;
    ray.direction -= position;
    ray.direction.normalize();

    return ray;
}

void CameraController::rotateVertical(double angle)
{
    Cartesian3 p = m_cameraTrans->worldPosition().normalize();
    if (defined(constrainedAxis)) {
        bool northParallel = CesiumCartesian3::equalsEpsilon(p, constrainedAxis, Math::EPSILON2);
        bool southParallel = CesiumCartesian3::equalsEpsilon(p, -constrainedAxis, Math::EPSILON2);
        if ((!northParallel && !southParallel)) {
            Cartesian3 constrainedAxis1 = constrainedAxis.normalized();

            double dot = Cartesian3::dot(p, constrainedAxis1);
            double angleToAxis = CesiumMath::acosClamped(dot);  //  CesiumMath.acosClamped(dot);
            if (angle > 0 && angle > angleToAxis) {
                angle = angleToAxis - Math::EPSILON4;
            }

            dot = Cartesian3::dot(p, -constrainedAxis1);
            angleToAxis = CesiumMath::acosClamped(dot);  //  CesiumMath.acosClamped(dot);
            if (angle < 0 && -angle > angleToAxis) {
                angle = -angleToAxis + Math::EPSILON4;
            }

            Cartesian3 tangent = Cartesian3::cross(constrainedAxis1, p);
            rotate(tangent, angle);
        } else if ((northParallel && angle < 0) || (southParallel && angle > 0)) {
            rotate(m_cameraTrans->xaxis(), angle);
        }
    } else {
        rotate(m_cameraTrans->xaxis(), angle);
    }
}

void CameraController::rotateHorizontal(double angle)
{
    if (defined(constrainedAxis)) {
        rotate(constrainedAxis, angle);
    } else {
        rotate(m_cameraTrans->zaxis(), angle);
    }
}

void CameraController::zoom3D(double amount)
{
    move(m_cameraTrans->yaxis(), amount);
}

void CameraController::updateMembers()
{
    Cartesian3 position = m_cameraTrans->worldPosition();
    Cartesian3 direction = m_cameraTrans->yaxis();
    Cartesian3 up = m_cameraTrans->zaxis();
    Cartesian3 right = m_cameraTrans->xaxis();

    bool positionChanged = _position != position;
    if (positionChanged) {
        _position = position;
    }

    bool directionChanged = _direction != direction;
    if (directionChanged) {
//        direction.normalize();
//        m_cameraTrans->setAxes(right, direction, up);
        _direction = direction;
    }

    bool upChanged = _up != up;
    if (upChanged) {
//        up.normalize();
//        m_cameraTrans->setAxes(right, direction, up);
        _up = up;
    }

    bool rightChanged = _right != right;
    if (rightChanged) {
//        right.normalize();
//        m_cameraTrans->setAxes(right, direction, up);
        _right = right;
    }

    bool transformChanged = _transformChanged;
    _transformChanged = false;

    if (transformChanged) {
        _invTransform = _transform.inverseTransformation();
        _actualTransform = _transform;
        _actualInvTransform = _actualTransform.inverseTransformation();
    }

    if (positionChanged || transformChanged) {
        _positionWC = multiplyByPoint(_actualTransform, _position);

        // Compute the Cartographic position of the camera.
        _positionCartographic = cartesianToCartographic(_positionWC);
    }

    if (directionChanged || upChanged || rightChanged) {
        double det = Cartesian3::dot(_direction, Cartesian3::cross(_up, _right));
        if (abs(1.0 - det) > Math::EPSILON2) {
            //orthonormalize axes
            double invUpMag = 1.0 / _up.magnitudeSquared();
            double scalar = Cartesian3::dot(_up, _direction) * invUpMag;
            Cartesian3 w0 = _direction * scalar;
            _up = (_up - w0).normalize();
            _right = Cartesian3::cross(_direction, _up).normalize();

            m_cameraTrans->setAxes(_right, direction, _up);
        }
    }

    if (directionChanged || transformChanged) {
        _directionWC = multiplyByPointAsVector(_actualTransform, _direction);
        _directionWC.normalize();
    }

    if (upChanged || transformChanged) {
        _upWC = multiplyByPointAsVector(_actualTransform, _up);
        _upWC.normalize();
    }

    if (rightChanged || transformChanged) {
        _rightWC = multiplyByPointAsVector(_actualTransform, _right);
        _rightWC.normalize();
    }
}

double CameraController::getHeading(const Cartesian3 &direction, const Cartesian3 &up)
{
    double heading = 0.0;
    if (!CesiumMath::equalsEpsilon(abs(direction.z), 1.0, Math::EPSILON3)) {
        heading = atan2(direction.y, direction.x) - M_PI_2;
    } else {
        heading = atan2(up.y, up.x) - M_PI_2;
    }

    return 2.0 * M_PI - CesiumMath::zeroToTwoPi(heading);
}

double CameraController::getPitch(const Cartesian3 &direction)
{
    return M_PI_2 - CesiumMath::acosClamped(direction.z);
}

double CameraController::getRoll(const Cartesian3 &direction, const Cartesian3 &up, const Cartesian3 &right)
{
    double roll = 0.0;
    if (!CesiumMath::equalsEpsilon(abs(direction.z), 1.0, Math::EPSILON3)) {
        roll = atan2(-right.z, up.z);
        roll = CesiumMath::zeroToTwoPi(roll + 2.0 * M_PI);
    }

    return roll;
}

void CameraController::setView3D(const Cartesian3 &destination, double heading, double pitch, double roll)
{
    Matrix4 currentTransform = _transform;
    Matrix4 localTransform = eastNorthUpToFixedFrame(destination);
    _setTransform(localTransform);

    heading = heading - M_PI_2;

    Quaternion rotQuat = CesiumMath::fromHeadingPitchRoll(heading, pitch , roll);
    Matrix3 rotMat = rotQuat.toRotationMatrix();

    Cartesian3 direction = rotMat.column(0).normalize();
    Cartesian3 up = rotMat.column(2).normalize();
    Cartesian3 right = Cartesian3::cross(direction, up).normalize();

    m_cameraTrans->setWorldPosition(Vector3(0, 0, 0));
    m_cameraTrans->setAxes(right, direction, up);

    _setTransform(currentTransform);
}

Cartesian3 CameraController::rectangleCameraPosition3D(const LiRectangle &rectangle)
{
    CameraRF cameraRF;

    double north = rectangle.north;
    double south = rectangle.south;
    double east = rectangle.east;
    double west = rectangle.west;

    // If we go across the International Date Line
    if (west > east) {
        east += 2.0 * M_PI;
    }

    // Find the midpoint latitude.
    //
    // EllipsoidGeodesic will fail if the north and south edges are very close to being on opposite sides of the ellipsoid.
    // Ideally we'd just call EllipsoidGeodesic.setEndPoints and let it throw when it detects this case, but sadly it doesn't
    // even look for this case in optimized builds, so we have to test for it here instead.
    //
    // Fortunately, this case can only happen (here) when north is very close to the north pole and south is very close to the south pole,
    // so handle it just by using 0 latitude as the center.  It's certainliy possible to use a smaller tolerance
    // than one degree here, but one degree is safe and putting the center at 0 latitude should be good enough for any
    // rectangle that spans 178+ of the 180 degrees of latitude.
    double longitude = (west + east) * 0.5;
    double latitude;
    Cartographic viewRectangle3DCartographic1;
    if (south < -M_PI_2 + Math::RADIANS_PER_DEGREE && north > M_PI_2 - Math::RADIANS_PER_DEGREE) {
        latitude = 0.0;
    } else {
        Cartographic northCartographic;
        northCartographic.longitude = longitude;
        northCartographic.latitude = north;
        northCartographic.height = 0.0;
        viewRectangle3DCartographic1 = northCartographic;

        Cartographic southCartographic;
        southCartographic.longitude = longitude;
        southCartographic.latitude = south;
        southCartographic.height = 0.0;

        EllipsoidGeodesic *ellipsoidGeodesic = new EllipsoidGeodesic();

        ellipsoidGeodesic->setEndPoints(northCartographic, southCartographic);
        viewRectangle3DCartographic1 = ellipsoidGeodesic->interpolateUsingFraction(0.5);
        delete ellipsoidGeodesic;
        latitude = viewRectangle3DCartographic1.latitude;
    }

    Cartographic centerCartographic = viewRectangle3DCartographic1;
    centerCartographic.longitude = longitude;
    centerCartographic.latitude = latitude;
    centerCartographic.height = 0.0;
    viewRectangle3DCartographic1 = centerCartographic;

    Cartesian3 center = cartographicToCartesian(centerCartographic);

    Cartographic cart = viewRectangle3DCartographic1;
    cart.longitude = east;
    cart.latitude = north;
    Cartesian3 northEast = cartographicToCartesian(cart);
    cart.longitude = west;
    Cartesian3 northWest = cartographicToCartesian(cart);
    cart.longitude = longitude;
    Cartesian3 northCenter = cartographicToCartesian(cart);
    cart.latitude = south;
    Cartesian3 southCenter = cartographicToCartesian(cart);
    cart.longitude = east;
    Cartesian3 southEast = cartographicToCartesian(cart);
    cart.longitude = west;
    Cartesian3 southWest = cartographicToCartesian(cart);
    viewRectangle3DCartographic1 = cart;

    northWest -= center;
    southEast -= center;
    northEast -= center;
    southWest -= center;
    northCenter -= center;
    southCenter -= center;

    cameraRF.direction = Ellipsoid::WGS84()->geodeticSurfaceNormal(center);
    cameraRF.direction = -cameraRF.direction;
    cameraRF.right = Cartesian3::cross(cameraRF.direction, Cartesian3::UNIT_Z);
    cameraRF.right.normalize();
    cameraRF.up = Cartesian3::cross(cameraRF.right, cameraRF.direction);

    double d;
    double tanPhi = tan(Math::toRadians(m_camera->fovy()) * 0.5);
    double tanTheta = m_camera->aspectRatio() * tanPhi;

    d = std::max(computeD(cameraRF.direction, cameraRF.up, northWest, tanPhi),
                 computeD(cameraRF.direction, cameraRF.up, southEast, tanPhi));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.up, northEast, tanPhi));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.up, southWest, tanPhi));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.up, northCenter, tanPhi));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.up, southCenter, tanPhi));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.right, northWest, tanTheta));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.right, southEast, tanTheta));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.right, northEast, tanTheta));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.right, southWest, tanTheta));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.right, northCenter, tanTheta));
    d = std::max(d,
                 computeD(cameraRF.direction, cameraRF.right, southCenter, tanTheta));

    // If the rectangle crosses the equator, compute D at the equator, too, because that's the
    // widest part of the rectangle when projected onto the globe.
    if (south < 0 && north > 0) {
        Cartographic equatorCartographic(west, 0.0, 0.0);

        Cartesian3 equatorPosition = cartographicToCartesian(equatorCartographic);
        equatorPosition -= center;
        d = std::max(d, computeD(cameraRF.direction, cameraRF.up, equatorPosition, tanPhi));
        d = std::max(d, computeD(cameraRF.direction, cameraRF.right, equatorPosition, tanTheta));

        equatorCartographic.longitude = east;
        equatorPosition = cartographicToCartesian(equatorCartographic);
        equatorPosition -= center;

        d = std::max(d, computeD(cameraRF.direction, cameraRF.up, equatorPosition, tanPhi));
        d = std::max(d, computeD(cameraRF.direction, cameraRF.right, equatorPosition, tanTheta));
    }

    return center + cameraRF.direction * -d;
}

double CameraController::computeD(const Cartesian3 &direction, const Cartesian3 &upOrRight, const Cartesian3 &corner, double tanThetaOrPhi)
{
    double opposite = abs(Cartesian3::dot(upOrRight, corner));
    return opposite / tanThetaOrPhi - Cartesian3::dot(direction, corner);
}

Cartesian3 CameraController::multiplyByPoint(const Matrix4 &matrix, const Cartesian3 &cartesian)
{
    double vX = cartesian.x;
    double vY = cartesian.y;
    double vZ = cartesian.z;

    double x = matrix[0] * vX + matrix[4] * vY + matrix[8] * vZ + matrix[12];
    double y = matrix[1] * vX + matrix[5] * vY + matrix[9] * vZ + matrix[13];
    double z = matrix[2] * vX + matrix[6] * vY + matrix[10] * vZ + matrix[14];

    return Cartesian3(x, y, z);
}

Cartesian3 CameraController::multiplyByPointAsVector(const Matrix4 &matrix, const Cartesian3 &cartesian)
{
    double vX = cartesian.x;
    double vY = cartesian.y;
    double vZ = cartesian.z;

    double x = matrix[0] * vX + matrix[4] * vY + matrix[8] * vZ;
    double y = matrix[1] * vX + matrix[5] * vY + matrix[9] * vZ;
    double z = matrix[2] * vX + matrix[6] * vY + matrix[10] * vZ;

    return Cartesian3(x, y, z);
}
