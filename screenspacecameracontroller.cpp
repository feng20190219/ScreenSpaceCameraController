#include "screenspacecameracontroller.h"
#include "cameraeventaggregator.h"
#include "liviewer.h"
#include "liscene.h"
#include "globe.h"
#include "licamera.h"
#include "liwidget.h"
#include "cameracontroller.h"
#include "tweencollection.h"
#include "liinputsystem.h"
#include "timestamp.h"
#include "limath.h"
#include "matrix4.h"
#include "litransform.h"
#include "intersectiontests.h"
#include "transforms.h"
#include "liutils.h"
#include "cesiummath.h"
#include "cesiumcartesian3.h"
#include "liraycasthit.h"
#include "quadtreeprimitive.h"
#include "ellipsoid.h"

ScreenSpaceCameraController::ScreenSpaceCameraController(LiNode *parent)
    : LiCameraController(parent)
    , _scene(GlobalViewer()->scene())
    , _ellipsoid(Ellipsoid::WGS84())
{
    _sphereEllipsoid = new Ellipsoid(1.0, 1.0, 1.0);

    _tweens = new TweenCollection();
    _canvas = _scene->canvas();
    _globe = _scene->globe();
    m_globe = _globe;
    _camera = _scene->mainCamera();
    _cameraTrans = _camera->transform();

    _aggregator = new CameraEventAggregator(_canvas, this);
    _cameraController = new CameraController(_scene, _camera, _tweens);

    _input = _aggregator->inputSystem;

    translateEventTypes.append(EventType(CameraEventType::LEFT_DRAG, 0));

    zoomEventTypes.append(EventType(CameraEventType::RIGHT_DRAG, 0));
    zoomEventTypes.append(EventType(CameraEventType::WHEEL, 0));
    zoomEventTypes.append(EventType(CameraEventType::PINCH, 0));

    rotateEventTypes.append(EventType(CameraEventType::LEFT_DRAG, 0));

    tiltEventTypes.append(EventType(CameraEventType::MIDDLE_DRAG, 0));
    tiltEventTypes.append(EventType(CameraEventType::PINCH, 0));
    tiltEventTypes.append(EventType(CameraEventType::LEFT_DRAG, (int)Qt::Key_Control));
    tiltEventTypes.append(EventType(CameraEventType::RIGHT_DRAG, (int)Qt::Key_Control));

    lookEventTypes.append(EventType(CameraEventType::LEFT_DRAG, (int)Qt::Key_Shift));
}

ScreenSpaceCameraController::~ScreenSpaceCameraController()
{
    qDeleteAll(_movementState);
    delete _tweens;
    delete _aggregator;
    delete _cameraController;
    delete _sphereEllipsoid;
}

void ScreenSpaceCameraController::update()
{
    _tweens->update();

    if (_cameraController->_transform != Matrix4()) {
        _globe = nullptr;
        _ellipsoid = _sphereEllipsoid;
    } else {
        _globe = _scene->globe();
        _ellipsoid = Ellipsoid::WGS84();
    }

    double radius = _ellipsoid->maximumRadius();
    _rotateFactor = 1.0 / radius;
    _rotateRateRangeAdjustment = radius;

    update3D();
    _aggregator->reset();

    handleKeyDown();
}

bool ScreenSpaceCameraController::enableInputs() const
{
    return _enableInputs;
}

void ScreenSpaceCameraController::setEnableInputs(bool enable)
{
    _enableInputs = enable;
}

bool ScreenSpaceCameraController::enableZoom() const
{
    return _enableZoom;
}

void ScreenSpaceCameraController::setEnableZoom(bool enable)
{
    _enableZoom = enable;
}

bool ScreenSpaceCameraController::enableRotate() const
{
    return _enableRotate;
}

void ScreenSpaceCameraController::setEnableRotate(bool enable)
{
    _enableRotate = enable;
}

bool ScreenSpaceCameraController::enableTilt() const
{
    return _enableTilt;
}

void ScreenSpaceCameraController::setEnableTilt(bool enable)
{
    _enableTilt = enable;
}

bool ScreenSpaceCameraController::enableLook() const
{
    return _enableLook;
}

void ScreenSpaceCameraController::setEnableLook(bool enable)
{
    _enableLook = enable;
}

bool ScreenSpaceCameraController::enableUnderGround() const
{
    return _enableUnderGround;
}

void ScreenSpaceCameraController::setEnableUnderGround(bool enable)
{
    _enableUnderGround = enable;
}

bool ScreenSpaceCameraController::enablePan() const
{
    return _enablePan;
}

void ScreenSpaceCameraController::setEnablePan(bool enable)
{
    _enablePan = enable;
}

double ScreenSpaceCameraController::minimumCollisionTerrainHeight() const
{
    return _minimumCollisionTerrainHeight;
}

void ScreenSpaceCameraController::setMinimumCollisionTerrainHeight(double minimumCollisionTerrainHeight)
{
    _minimumCollisionTerrainHeight = minimumCollisionTerrainHeight;
    _cameraController->_minimumCollisionTerrainHeight = minimumCollisionTerrainHeight;
}

Vector3 ScreenSpaceCameraController::positionWC()
{
    return _cameraController->positionWC();
}

Vector3 ScreenSpaceCameraController::directionWC()
{
    return _cameraController->directionWC();
}

Vector3 ScreenSpaceCameraController::upWC()
{
    return _cameraController->upWC();
}

Vector3 ScreenSpaceCameraController::rightWC()
{
    return _cameraController->rightWC();
}

Cartographic ScreenSpaceCameraController::positionCartographic()
{
    return _cameraController->positionCartographic();
}

double ScreenSpaceCameraController::heading()
{
    return _cameraController->heading();
}

double ScreenSpaceCameraController::pitch()
{
    return _cameraController->pitch();
}

double ScreenSpaceCameraController::roll()
{
    return _cameraController->roll();
}

Ray ScreenSpaceCameraController::getPickRay(double x, double y)
{
    return _cameraController->getPickRay(x, y);
}

void ScreenSpaceCameraController::setView(const Cartesian3 &destination, double heading, double pitch, double roll)
{
    return _cameraController->setView(destination, heading, pitch, roll);
}

void ScreenSpaceCameraController::rotate(const Vector3 &axis, double angle)
{
    _cameraController->rotate(axis, angle);
}

void ScreenSpaceCameraController::rotateUp(double angle)
{
    _cameraController->rotateUp(angle);
}

void ScreenSpaceCameraController::rotateDown(double angle)
{
    _cameraController->rotateDown(angle);
}

void ScreenSpaceCameraController::rotateRight(double angle)
{
    _cameraController->rotateRight(angle);
}

void ScreenSpaceCameraController::rotateLeft(double angle)
{
    _cameraController->rotateLeft(angle);
}

void ScreenSpaceCameraController::move(const Vector3 &dir, double amount)
{
    _cameraController->move(dir, amount);
}

void ScreenSpaceCameraController::moveForward(double amount)
{
    _cameraController->moveForward(amount);
}

void ScreenSpaceCameraController::moveBackward(double amount)
{
    _cameraController->moveBackward(amount);
}

void ScreenSpaceCameraController::moveUp(double amount)
{
    _cameraController->moveUp(amount);
}

void ScreenSpaceCameraController::moveDown(double amount)
{
    _cameraController->moveDown(amount);
}

void ScreenSpaceCameraController::moveRight(double amount)
{
    _cameraController->moveRight(amount);
}

void ScreenSpaceCameraController::moveLeft(double amount)
{
    _cameraController->moveLeft(amount);
}

void ScreenSpaceCameraController::look(const Vector3 &axis, double angle)
{
    _cameraController->look(axis, angle);
}

void ScreenSpaceCameraController::lookUp(double amount)
{
    _cameraController->lookUp(amount);
}

void ScreenSpaceCameraController::lookDown(double amount)
{
    _cameraController->lookDown(amount);
}

void ScreenSpaceCameraController::lookRight(double amount)
{
    _cameraController->lookRight(amount);
}

void ScreenSpaceCameraController::lookLeft(double amount)
{
    _cameraController->lookLeft(amount);
}

void ScreenSpaceCameraController::zoomIn(double amount)
{
    _cameraController->zoomIn(amount);
}

void ScreenSpaceCameraController::zoomOut(double amount)
{
    _cameraController->zoomOut(amount);
}

void ScreenSpaceCameraController::flyTo(const Vector3 &destination, double duration, double heading, double pitch, double roll)
{
    _cameraController->flyTo(destination, duration, heading, pitch, roll);
}

void ScreenSpaceCameraController::flyTo(const Cartographic &destination, double duration, double heading, double pitch, double roll)
{
    _cameraController->flyTo(destination, duration, heading, pitch, roll);
}

void ScreenSpaceCameraController::flyTo(const LiRectangle &destination, double duration, double heading, double pitch, double roll)
{
    _cameraController->flyTo(destination, duration, heading, pitch, roll);
}

Cartesian3 ScreenSpaceCameraController::pickGlobe(const Vector2 &mousePosition) const
{
    if (!_globe) {
        return Cartesian3();
    }

    Ray ray = _cameraController->getPickRay(mousePosition.x(), mousePosition.y());
    Cartesian3 rayIntersection;
    _globe->pick(ray, &rayIntersection);

    if (rayIntersection.isNull()) {
        double height = _cameraController->positionCartographic().height;
        if (_enableUnderGround && height < 0) { //UnderGround
            //globalview
            Cartesian3 neworig = ray.getPoint(10000000);
            ray.origin = neworig;
            ray.direction = -ray.direction;
            _globe->pick(ray, &rayIntersection);
        }
    }

    return rayIntersection;
}

void ScreenSpaceCameraController::spin3DByKey(double startX, double startY, double endX, double endY, bool touring, bool mouseUp)
{
    if (mouseUp) {
        m_touring = false;
        m_looking = false;
        return;
    }

    if (m_looking) {
        look3DByKey(startX, startY, endX, endY);
        return;
    }

    m_touring = touring;
    Cartesian3 spin3DPick;
    Cartesian3 cameraCarte = _cameraTrans->worldPosition();
    double height = _ellipsoid->cartesianToCartographic(cameraCarte).height;
    Cartesian3 mousePos;
    if (height < _minimumPickingTerrainHeight) {
        mousePos = pickGlobe(Vector2(startX, startY));
        if (!mousePos.isNull()) {
            double magnitude = mousePos.magnitude();
            Ellipsoid ellipsoid(magnitude, magnitude, magnitude);
            Cartesian3 p0 = _cameraController->pickEllipsoid(Cartesian2(startX, startY), &ellipsoid, p0);
            pan3DByKey(startX, startY, endX, endY, &ellipsoid);
        } else {
            m_looking = true;
            look3DByKey(startX, startY, endX, endY);
        }
    } else if (defined(_cameraController->pickEllipsoid(Cartesian2(startX, startY), _ellipsoid, spin3DPick))) {
        pan3DByKey(startX, startY, endX, endY, _ellipsoid);
    } else if (height > _minimumTrackBallHeight) {
        rotate3DByKey(startX, startY, endX, endY);
    } else {
        m_looking = true;
        look3DByKey(startX, startY, endX, endY);
    }
}

void ScreenSpaceCameraController::pan3DByKey(double startX, double startY, double endX, double endY, Ellipsoid *ellipsoid)
{
    Cartesian2 startMousePosition(startX, startY);
    Cartesian2 endMousePosition(endX, endY);

    Cartesian3 p0, p1;
    _cameraController->pickEllipsoid(startMousePosition, ellipsoid, p0);
    _cameraController->pickEllipsoid(endMousePosition, ellipsoid, p1);

    _cameraController->worldToCameraCoordinates(p0);
    _cameraController->worldToCameraCoordinates(p1);
    p0.normalize();
    p1.normalize();
    double dot = Cartesian3::dot(p0, p1);
    Cartesian3 axis = Cartesian3::cross(p0, p1).normalize();

    Cartesian3 ZERO;
    if (dot <= 1.0 && !CesiumCartesian3::equalsEpsilon(axis, ZERO, Math::EPSILON14)) { // dot is in [0, 1]
        double angle = acos(dot);
        if (angle < 0.00000002)
            angle = 0.00000002;

        Cartesian3 oldPos = _cameraTrans->worldPosition();
        _cameraController->rotate(axis, angle);

        // add by feng
        if (cartesianToCartographic(_cameraTrans->worldPosition()).height < 1) {
            _cameraTrans->setWorldPosition(oldPos);
            return;
        }
    }
}

void ScreenSpaceCameraController::rotate3DByKey(double startX, double startY, double endX, double endY)
{
    double phiWindow = startX - endX;
    double thetaWindow = startY - endY;

    double phiWindowRatio = phiWindow / _canvas->width();
    double thetaWindowRatio = thetaWindow / _canvas->height();
    phiWindowRatio = std::min(phiWindowRatio, maximumMovementRatio);
    thetaWindowRatio = std::min(thetaWindowRatio, maximumMovementRatio);

    double deltaPhi = _maximumRotateRate * phiWindowRatio * M_PI * 2.0;
    double deltaTheta = _maximumRotateRate * thetaWindowRatio * M_PI;

    _cameraController->rotateRight(deltaPhi);
    _cameraController->rotateUp(deltaTheta);
}

void ScreenSpaceCameraController::look3DByKey(double startX, double startY, double endX, double endY)
{
    Cartesian3 start = _cameraController->getPickRay(startX, 0).direction;
    Cartesian3 end = _cameraController->getPickRay(endX, 0).direction;
    double angle = 0.0;

    double dot = Cartesian3::dot(start, end);
    if (dot < 1.0) { // dot is in [0, 1]
        angle = acos(dot);
    }
    angle = startX > endX ? -angle : angle;

    Cartesian3 rotationAxis = _ellipsoid->geodeticSurfaceNormal(_cameraTrans->worldPosition());
    _cameraController->look(rotationAxis, -angle);

    start = _cameraController->getPickRay(0, startY).direction;
    end = _cameraController->getPickRay(0, endY).direction;
    angle = 0.0;

    dot = Cartesian3::dot(start, end);
    if (dot < 1.0) { // dot is in [0, 1]
        angle = acos(dot);
    }
    angle = startY > endY ? -angle : angle;

    Cartesian3 negativeRotationAxis = -rotationAxis;
    bool northParallel = CesiumCartesian3::equalsEpsilon(_cameraTrans->yaxis(), rotationAxis, Math::EPSILON2);
    bool southParallel = CesiumCartesian3::equalsEpsilon(_cameraTrans->yaxis(), negativeRotationAxis, Math::EPSILON2);
    if ((!northParallel && !southParallel)) {
        dot = Cartesian3::dot(_cameraTrans->yaxis(), rotationAxis);
        double angleToAxis = CesiumMath::acosClamped(dot); // CesiumMath.acosClamped(dot);
        if (angle > 0 && angle > angleToAxis) {
            angle = angleToAxis - Math::EPSILON4;
        }

        dot = Cartesian3::dot(_cameraTrans->yaxis(), negativeRotationAxis);
        angleToAxis = CesiumMath::acosClamped(dot); // CesiumMath.acosClamped(dot);
        if (angle < 0 && -angle > angleToAxis) {
            angle = -angleToAxis + Math::EPSILON4;
        }

        Cartesian3 tangent = Cartesian3::cross(rotationAxis, _cameraTrans->yaxis());
        _cameraController->look(tangent, angle);
    } else if ((northParallel && angle < 0) || (southParallel && angle > 0)) {
        _cameraController->look(_cameraTrans->xaxis(), -angle);
    }
}

void ScreenSpaceCameraController::update3D()
{
    Action spin3DAction = [this](const Cartesian2 &startPosition, const CameraMovement &movement) {
        spin3D(startPosition, movement);
    };
    Action zoom3DAction = [this](const Cartesian2 &startPosition, const CameraMovement &movement) {
        zoom3D(startPosition, movement);
    };
    Action tilt3DAction = [this](const Cartesian2 &startPosition, const CameraMovement &movement) {
        tilt3D(startPosition, movement);
    };
    Action look3DAction = [this](const Cartesian2 &startPosition, const CameraMovement &movement) {
        look3D(startPosition, movement);
    };

    reactToInput(_enableRotate, rotateEventTypes, spin3DAction, inertiaSpin, QStringLiteral("_lastInertiaSpinMovement"));
    reactToInput(_enableZoom, zoomEventTypes, zoom3DAction, inertiaZoom, QStringLiteral("_lastInertiaZoomMovement"));
    reactToInput(_enableTilt, tiltEventTypes, tilt3DAction, inertiaSpin, QStringLiteral("_lastInertiaTiltMovement"));
    reactToInput(_enableLook, lookEventTypes, look3DAction);
}

void ScreenSpaceCameraController::reactToInput(bool enabled, const QVector<EventType> &eventTypes,  Action action,
                                               double inertiaConstant, const QString &inertiaStateName)
{
    if (!_enableInputs || !enabled)
        return;

    for (EventType eventType : eventTypes) {
        CameraEventType ::Type type = eventType.eventType;
        int modifier = eventType.modifier;

        bool isMoving = _aggregator->isMoving(type, modifier);
        if (isMoving) {
            CameraMovement movement = _aggregator->getMovement(type, modifier);
            Cartesian2 startPosition = _aggregator->getStartMousePosition(type, modifier);
            action(startPosition, movement);
        }
        else if (inertiaConstant < 1.0) {
            maintainInertia(type, modifier, inertiaConstant, action, inertiaStateName);
        }
    }
}

void ScreenSpaceCameraController::maintainInertia(CameraEventType::Type type, int modifier, double decayCoef,
                                                  Action action, const QString &lastMovementName)
{
    MovementState *movementState = _movementState.value(lastMovementName, nullptr);
    if (!movementState) {
        movementState = new MovementState;
        _movementState[lastMovementName] = movementState;
    }

    quint64 ts = _aggregator->getButtonPressTime(type, modifier);
    quint64 tr = _aggregator->getButtonReleaseTime(type, modifier);

    double threshold = 0.0;
    if (ts != 0 && tr != 0)
        threshold = (tr - ts) / 1000.0;

    quint64 now = getTimestamp();
    double fromNow = (now - tr) / 1000.0;

    double inertiaMaxClickTimeThreshold = 0.4;

    if (ts != 0 && tr != 0 && threshold < inertiaMaxClickTimeThreshold) {
        double d = decay(fromNow, decayCoef);

        if (!movementState->active) {
            CameraMovement lastMovement = _aggregator->getLastMovement(type, modifier);
            if (!lastMovement.valid || CesiumCartesian3::equalsEpsilon(Cartesian3(lastMovement.startPosition),
                                                 Cartesian3(lastMovement.endPosition),
                                                 Math::EPSILON14)) {
                return;
            }

            movementState->motion.x = (lastMovement.endPosition.x - lastMovement.startPosition.x) * 0.5;
            movementState->motion.y = (lastMovement.endPosition.y - lastMovement.startPosition.y) * 0.5;

            movementState->startPosition = lastMovement.startPosition;

            movementState->endPosition = movementState->motion * d;
            movementState->endPosition += movementState->startPosition;

            movementState->active = true;
        } else {
            movementState->startPosition = movementState->endPosition;

            movementState->endPosition = movementState->motion * d;
            movementState->endPosition += movementState->startPosition;

            movementState->motion = Cartesian2(0, 0);
        }

        // If value from the decreasing exponential function is close to zero,
        // the end coordinates may be NaN.
        if ((movementState->startPosition - movementState->endPosition).magnitude() < 0.3) {
            movementState->active = false;
            return;
        }

        if (!_aggregator->isButtonDown(type, modifier)) {
            Cartesian2 startPosition = _aggregator->getStartMousePosition(type, modifier);

            CameraMovement movement;
            movement.startPosition = movementState->startPosition;
            movement.endPosition = movementState->endPosition;

            action(startPosition, movement);
        }
    } else {
        movementState->active = false;
    }
}

double ScreenSpaceCameraController::decay(double time, double coefficient) const
{
    if (time < 0) {
        return 0.0;
    }

    double tau = (1.0 - coefficient) * 25.0;
    return exp(-tau * time);
}

void ScreenSpaceCameraController::spin3D(const Cartesian2 &startPosition, const CameraMovement &movement)
{
    Cartesian3 spin3DPick;

    if (_cameraController->_transform != Matrix4()) {
        rotate3D(movement);
        return;
    }

    double height = _ellipsoid->cartesianToCartographic(_cameraController->positionWC()).height;

    Cartesian3 mousePos;
    bool tangentPick = false;
    if (_globe && height < _minimumPickingTerrainHeight) {
        mousePos = pickGlobe(Vector2(movement.startPosition.x, movement.startPosition.y));
        if (!mousePos.isNull()) {
            Ray ray = _cameraController->getPickRay(movement.startPosition.x, movement.startPosition.y);
            Cartesian3 normal = _ellipsoid->geodeticSurfaceNormal(mousePos);
            tangentPick = abs(Cartesian3::dot(ray.direction, normal)) < 0.05;

            // 注释的地方
//            if (tangentPick && !enableUnderGround) {
//                return;
//            }

            if (tangentPick && !_looking) {
                _rotating = false;
                _strafing = true;
            }
        }
    }

    _rotationAxis = _ellipsoid->geodeticSurfaceNormal(_cameraTrans->worldPosition());

    if (startPosition == _rotateMousePosition) {
        if (_looking) {
            look3D(startPosition, movement);
        } else if (_rotating) {
            rotate3D(movement);
        } else if (_strafing) {
            _strafeStartPosition =  mousePos;
            strafe(movement);
        } else {
            double magnitude = _rotateStartPosition.magnitude();
            Ellipsoid ellipsoid(magnitude, magnitude, magnitude);
            pan3D(movement, &ellipsoid);
        }
        _rotationAxis = Cartesian3();
        return;
    }
    _looking = false;
    _rotating = false;
    _strafing = false;

    if (_globe && height < _minimumPickingTerrainHeight) {
        if (!mousePos.isNull()) {
            if (_cameraTrans->worldPosition().magnitude() < mousePos.magnitude()) {
                mousePos = _strafeStartPosition;

                _strafing = true;
                strafe(movement);
            } else {
                double magnitude = mousePos.magnitude();
                Ellipsoid ellipsoid(magnitude, magnitude, magnitude);
                pan3D(movement, &ellipsoid);

                _rotateStartPosition = mousePos;
            }
        } else {
            _looking = true;
            look3D(startPosition, movement);
        }
    } else if (defined(_cameraController->pickEllipsoid(movement.startPosition, _ellipsoid, spin3DPick))) {
        pan3D(movement, _ellipsoid);
        _rotateStartPosition = spin3DPick;
    } else if (height > _minimumTrackBallHeight) {
        _rotating = true;
        rotate3D(movement);
    } else {
        _looking = true;
        look3D(startPosition, movement);
    }

    _rotateMousePosition = startPosition;
    _rotationAxis = Cartesian3();
}

void ScreenSpaceCameraController::zoom3D(const Cartesian2 &startPosition, const CameraMovement &movement)
{
//    if (defined(movement.distance)) {
//        movement = movement.distance;
//    }

    Cartesian2 windowPosition(_canvas->width() / 2, _canvas->height() / 2);
    Ray ray = _cameraController->getPickRay(windowPosition.x, windowPosition.y);

    Cartesian3 intersection;
    double height = _ellipsoid->cartesianToCartographic(_cameraTrans->worldPosition()).height;
    if (height < _minimumPickingTerrainHeight) {
        intersection = pickGlobe(Vector2(windowPosition.x, windowPosition.y));
    }

    double distance;
    if (!intersection.isNull()) {
        distance = (ray.origin - intersection).magnitude();
    } else {
        distance = height;
    }

    Cartesian3 unitPosition = _cameraTrans->worldPosition().normalized();
    handleZoom(startPosition, movement, _zoomFactor, distance, Cartesian3::dot(unitPosition, _cameraTrans->yaxis()));
}

void ScreenSpaceCameraController::tilt3D(const Cartesian2 &startPosition, const CameraMovement &movement)
{
    if (_cameraController->_transform != Matrix4()) {
        return;
    }

//    if (defined(movement.angleAndHeight)) {
//        movement = movement.angleAndHeight;
//    }

    if (startPosition != _tiltCenterMousePosition) {
        Cartographic cartographic = _ellipsoid->cartesianToCartographic(_cameraTrans->worldPosition());
        if (cartographic.height > _minimumCollisionTerrainHeight) {
            _tiltOnEllipsoid = true;
        }
        else {
            _tiltOnEllipsoid = false;
        }
        _looking = false;
    }

    if (_looking) {
        _rotationAxis = _ellipsoid->geodeticSurfaceNormal(_cameraTrans->worldPosition());
        look3D(startPosition, movement);
        _rotationAxis = Cartesian3(0, 0, 0);
        return;
    }

    if (_tiltOnEllipsoid){
        tilt3DOnEllipsoid(startPosition, movement);
    } else {
        tilt3DOnTerrain(startPosition, movement);

        Cartesian3 normal = _ellipsoid->geodeticSurfaceNormal(_cameraController->positionWC());
        double angle = CesiumCartesian3::angleBetween(normal, _cameraTrans->xaxis()); // radian

        if (abs(M_PI_2 - angle) >= 0.0001) {
            double angle2 = angle - M_PI_2;
//            _cameraController->look(_cameraTrans->yaxis(), angle2);

            Cartesian3 lookAxis;
            double dotZ = Cartesian3::dot(normal, _cameraTrans->zaxis());
            if (abs(dotZ) > 0.707) // 相机z轴 与 normal夹角小于45度, 表示相机绕x轴向上旋转超过45度
                lookAxis = _cameraTrans->yaxis();
            else
                lookAxis = _cameraTrans->zaxis();

            _cameraController->look(lookAxis, angle2);
        }
    }
}

void ScreenSpaceCameraController::look3D(const Cartesian2 &startPosition, const CameraMovement &movement)
{
    Cartesian2 startPos;
    Cartesian2 endPos;
    if (movement.pinch) {
        startPos = Cartesian2(movement.angleAndHeight.startPosition.x, 0);
        endPos = Cartesian2(movement.angleAndHeight.endPosition.x, 0);
    }
    else {
        startPos = Cartesian2(movement.startPosition.x, 0);
        endPos = Cartesian2(movement.endPosition.x, 0);
    }

    Cartesian3 start = _cameraController->getPickRay(startPos.x, startPos.y).direction;
    Cartesian3 end = _cameraController->getPickRay(endPos.x, endPos.y).direction;
    double angle = 0.0;

    double dot = Cartesian3::dot(start, end);
    if (dot < 1.0) { // dot is in [0, 1]
        angle = acos(dot);
    }

    if (movement.pinch)
        angle = (movement.angleAndHeight.startPosition.x > movement.angleAndHeight.endPosition.x) ? -angle : angle;
    else
        angle = (movement.startPosition.x > movement.endPosition.x) ? -angle : angle;

    Cartesian3 rotationAxis = _rotationAxis;
    if (!rotationAxis.isNull()) {
        _cameraController->look(rotationAxis, -angle);
    }
    else {
        _cameraController->lookLeft(angle);
    }

    startPos.x = 0.0;
    if (movement.pinch)
        startPos.y = movement.angleAndHeight.startPosition.y;
    else
        startPos.y = movement.startPosition.y;


    endPos.x = 0.0;
    if (movement.pinch)
        endPos.y = movement.angleAndHeight.endPosition.y;
    else
        endPos.y = movement.endPosition.y;


    start = _cameraController->getPickRay(startPos.x, startPos.y).direction;
    end = _cameraController->getPickRay(endPos.x, endPos.y).direction;
    angle = 0.0;

    dot = Cartesian3::dot(start, end);
    if (dot < 1.0) { // dot is in [0, 1]
        angle = acos(dot);
    }
    if (movement.pinch)
        angle = (movement.angleAndHeight.startPosition.y > movement.angleAndHeight.endPosition.y) ? -angle : angle;
    else
        angle = (movement.startPosition.y > movement.endPosition.y) ? -angle : angle;


    if (!rotationAxis.isNull()) {
        Cartesian3 negativeRotationAxis = -rotationAxis;
        bool northParallel = CesiumCartesian3::equalsEpsilon(_cameraTrans->yaxis(), rotationAxis, Math::EPSILON2);
        bool southParallel = CesiumCartesian3::equalsEpsilon(_cameraTrans->yaxis(), negativeRotationAxis, Math::EPSILON2);
        if ((!northParallel && !southParallel)) {
            dot = Cartesian3::dot(_cameraTrans->yaxis(), rotationAxis);
            double angleToAxis = CesiumMath::acosClamped(dot); // CesiumMath.acosClamped(dot);
            if (angle > 0 && angle > angleToAxis) {
                angle = angleToAxis - Math::EPSILON4;
            }

            dot = Cartesian3::dot(_cameraTrans->yaxis(), negativeRotationAxis);
            angleToAxis = CesiumMath::acosClamped(dot); // CesiumMath.acosClamped(dot);
            if (angle < 0 && -angle > angleToAxis) {
                angle = -angleToAxis + Math::EPSILON4;
            }

            Cartesian3 tangent = Cartesian3::cross(rotationAxis, _cameraTrans->yaxis());
            _cameraController->look(tangent, angle);
        } else if ((northParallel && angle < 0) || (southParallel && angle > 0)) {
            _cameraController->look(_cameraTrans->xaxis(), -angle);
        }
    } else {
        _cameraController->lookUp(angle);
    }
}

void ScreenSpaceCameraController::strafe(const CameraMovement &movement)
{
    Cartesian3 mouseStartPosition = pickGlobe(Vector2(movement.startPosition.x, movement.startPosition.y));
    if (mouseStartPosition.isNull()) {
        return;
    }

    Cartesian3 mousePosition = movement.endPosition;
    Ray ray = _cameraController->getPickRay(mousePosition.x, mousePosition.y);

    Cartesian3 direction = _cameraTrans->yaxis();

    Plane plane(mouseStartPosition, direction);
    Cartesian3 intersection = IntersectionTests::rayPlane(ray, &plane);
    if (!defined(intersection)) {
        return;
    }

    direction = mouseStartPosition - intersection;

    _cameraTrans->setWorldPosition(_cameraTrans->worldPosition() + direction);

    Cartographic cameraCartographic = _ellipsoid->cartesianToCartographic(_cameraController->positionWC());
    double cameraHeight = cameraCartographic.height;
    if (!_enableUnderGround) {
        double height = m_globe->getHeight(cameraCartographic);
        if (height < 0)
            height = 0;
        height = height + 0.9;
        if (cameraHeight < height) {
           _cameraTrans->setWorldPosition(_cameraTrans->worldPosition() - direction);
        }
    }
    else {
        if (cameraHeight < -980)
            _cameraTrans->setWorldPosition(_cameraTrans->worldPosition() - direction);
    }
}

void ScreenSpaceCameraController::pan3D(const CameraMovement &movement, Ellipsoid *ellipsoid)
{
    if (!_enablePan) {
        return;
    }

    Cartesian2 startMousePosition = movement.startPosition;
    Cartesian2 endMousePosition = movement.endPosition;

    Cartesian3 p0, p1;
    _cameraController->pickEllipsoid(startMousePosition, ellipsoid, p0);
    _cameraController->pickEllipsoid(endMousePosition, ellipsoid, p1);

    if (!defined(p0) || !defined(p1)) {
        _rotating = true;
        rotate3D(movement);
        return;
    }

    _cameraController->worldToCameraCoordinates(p0);
    _cameraController->worldToCameraCoordinates(p1);

    if (!defined(_cameraController->constrainedAxis)) {
        p0.normalize();
        p1.normalize();
        double dot = Cartesian3::dot(p0, p1);
        Cartesian3 axis = Cartesian3::cross(p0, p1).normalize();

        Cartesian3 ZERO;
        if (dot <= 1.0 && !CesiumCartesian3::equalsEpsilon(axis, ZERO, Math::EPSILON14)) { // dot is in [0, 1]
            double angle = acos(dot);
            if (angle < 0.00000002)
                angle = 0.00000002;

            _cameraController->rotate(axis, angle);
        }
    } else {
        Cartesian3 basis0 = _cameraController->constrainedAxis;
        Cartesian3 basis1 = CesiumCartesian3::mostOrthogonalAxis(basis0);
        basis1 = Cartesian3::cross(basis1, basis0);
        basis1.normalize();
        Cartesian3 basis2 = Cartesian3::cross(basis0, basis1);

        double startRho = p0.magnitude();
        double startDot = Cartesian3::dot(basis0, p0);
        double startTheta = acos(startDot / startRho);
        Cartesian3 startRej = basis0 * startDot;
        startRej = p0 - startRej;
        startRej.normalize();

        double endRho = p1.magnitude();
        double endDot = Cartesian3::dot(basis0, p1);
        double endTheta = acos(endDot / endRho);
        Cartesian3 endRej = basis0 * endDot;
        endRej = p1 - endRej;
        endRej.normalize();

        double startPhi = acos(Cartesian3::dot(startRej, basis1));
        if (Cartesian3::dot(startRej, basis2) < 0) {
            startPhi = M_PI *2 - startPhi;
        }

        double endPhi = acos(Cartesian3::dot(endRej, basis1));
        if (Cartesian3::dot(endRej, basis2) < 0) {
            endPhi = M_PI *2 - endPhi;
        }

        double deltaPhi = startPhi - endPhi;

        Cartesian3 east;
        if (CesiumCartesian3::equalsEpsilon(basis0, _cameraTrans->worldPosition(), Math::EPSILON2)) {
            east = _cameraTrans->xaxis();
        } else {
            east = Cartesian3::cross(basis0, _cameraTrans->worldPosition());
        }

        Cartesian3 planeNormal = Cartesian3::cross(basis0, east);
        double side0 = Cartesian3::dot(planeNormal, p0 - basis0);
        double side1 = Cartesian3::dot(planeNormal, p1 - basis0);

        double deltaTheta;
        if (side0 > 0 && side1 > 0) {
            deltaTheta = endTheta - startTheta;
        } else if (side0 > 0 && side1 <= 0) {
            if (Cartesian3::dot(_cameraTrans->worldPosition(), basis0) > 0) {
                deltaTheta = -startTheta - endTheta;
            } else {
                deltaTheta = startTheta + endTheta;
            }
        } else {
            deltaTheta = startTheta - endTheta;
        }

        _cameraController->rotateRight(deltaPhi);
        _cameraController->rotateUp(deltaTheta);
    }
}

void ScreenSpaceCameraController::rotate3D(const CameraMovement &movement, const Cartesian3 &constrainedAxis,
                                           bool rotateOnlyVertical, bool rotateOnlyHorizontal)
{
    Ellipsoid *ellipsoid = Ellipsoid::WGS84();

    Cartesian3 oldAxis = _cameraController->constrainedAxis;
    if (defined(constrainedAxis)) {
        _cameraController->constrainedAxis = constrainedAxis;
    }

    double rho = _cameraTrans->worldPosition().magnitude();
    double rotateRate = _rotateFactor * (rho - _rotateRateRangeAdjustment);

    if (rotateRate > _maximumRotateRate) {
        rotateRate = _maximumRotateRate;
    }

    if (rotateRate < _minimumRotateRate) {
        rotateRate = _minimumRotateRate;
    }
    double phiWindow, thetaWindow;
    if (movement.pinch) {
        phiWindow = movement.angleAndHeight.startPosition.x - movement.angleAndHeight.endPosition.x;
        thetaWindow = movement.angleAndHeight.startPosition.y - movement.angleAndHeight.endPosition.y;
    }
    else {
        phiWindow = movement.startPosition.x - movement.endPosition.x;
        thetaWindow = movement.startPosition.y - movement.endPosition.y;
    }

    double phiWindowRatio = phiWindow / _canvas->width();
    double thetaWindowRatio = thetaWindow / _canvas->height();
    phiWindowRatio = std::min(phiWindowRatio, maximumMovementRatio);
    thetaWindowRatio = std::min(thetaWindowRatio, maximumMovementRatio);

    double deltaPhi = rotateRate * phiWindowRatio * M_PI * 2.0;
    double deltaTheta = rotateRate * thetaWindowRatio * M_PI;

    if (!rotateOnlyVertical) {
        _cameraController->rotateRight(deltaPhi);

        Cartographic cameraCartographic = ellipsoid->cartesianToCartographic(_cameraController->positionWC());
        double cameraHeight = cameraCartographic.height;
        if (!_enableUnderGround){
            double height = m_globe->getHeight(cameraCartographic);
            if (height < 0)
                height = 0;
            height = height + 0.9;
            if (cameraHeight < height) {
                _cameraController->rotateRight(-deltaPhi);
            }
        }
        else {
            if (cameraHeight < -980) {
                _cameraController->rotateRight(-deltaPhi);
            }
        }
    }

    if (!rotateOnlyHorizontal) {
        _cameraController->rotateUp(deltaTheta);

        Cartographic cameraCartographic = ellipsoid->cartesianToCartographic(_cameraController->positionWC());
        double cameraHeight = cameraCartographic.height;
        if (!_enableUnderGround){
            double height = m_globe->getHeight(cameraCartographic);
            if (height < 0)
                height = 0;
            height = height + 0.9;
            if (cameraHeight < height) {
                if (deltaTheta > 0)
                    _cameraController->rotateUp(-deltaTheta);
            }
        }
        else {
            if (cameraHeight < -980)
                _cameraController->rotateUp(-deltaTheta);
        }
    }
    _cameraController->constrainedAxis = oldAxis;
}

void ScreenSpaceCameraController::tilt3DOnEllipsoid(const Cartesian2 &startPosition, const CameraMovement &movement)
{
    double minHeight = minimumZoomDistance * 0.25;
    double height = _ellipsoid->cartesianToCartographic(_cameraController->positionWC()).height;
    bool flags;
    if (movement.pinch)
        flags = movement.angleAndHeight.endPosition.y - movement.angleAndHeight.startPosition.y < 0;
    else
        flags = movement.endPosition.y - movement.startPosition.y < 0;
    if (height - minHeight - 1.0 < Math::EPSILON3 && flags) {
        return;
    }

    Cartesian2 windowPosition(_canvas->width() / 2.0, _canvas->height() / 2.0);
    Ray ray = _cameraController->getPickRay(windowPosition.x, windowPosition.y);

    Cartesian3 center;
    Interval intersection = IntersectionTests::rayEllipsoid(ray, _ellipsoid);
    if (defined(intersection)) {
        center = ray.getPoint(intersection.start);
    } else if (height > _minimumTrackBallHeight) {
        Cartesian3 grazingAltitudeLocation = IntersectionTests::grazingAltitudeLocation(ray, _ellipsoid);
//        if (!defined(grazingAltitudeLocation)) {
//            return;
//        }
        Cartographic grazingAltitudeCart = _ellipsoid->cartesianToCartographic(grazingAltitudeLocation);
        grazingAltitudeCart.height = 0.0;
        center = _ellipsoid->cartographicToCartesian(grazingAltitudeCart);
    } else {
        _looking = true;
        _rotationAxis = _ellipsoid->geodeticSurfaceNormal(_cameraTrans->worldPosition());
        look3D(startPosition, movement);
        _rotationAxis = Cartesian3(0, 0, 0);
        _tiltCenterMousePosition = startPosition;
        return;
    }

    Matrix4 transform = Transforms::eastNorthUpToFixedFrame(center, _ellipsoid);

    Globe *oldGlobe = _globe;
    Ellipsoid *oldEllipsoid = _ellipsoid;
    _globe = nullptr;
    _ellipsoid = _sphereEllipsoid;
    _rotateFactor = 1.0;
    _rotateRateRangeAdjustment = 1.0;

    Matrix4 oldTransform = _cameraController->_transform;
    _cameraController->_setTransform(transform);

    rotate3D(movement, Cartesian3::UNIT_Z);

    _cameraController->_setTransform(oldTransform);

    _globe = oldGlobe;
    _ellipsoid = oldEllipsoid;

    double radius = oldEllipsoid->maximumRadius();
    _rotateFactor = 1.0 / radius;
    _rotateRateRangeAdjustment = radius;
}

void ScreenSpaceCameraController::tilt3DOnTerrain(const Cartesian2 &startPosition, const CameraMovement &movement)
{
    Cartesian3 center;
    Ray ray;
    Interval intersection;

    if (startPosition == _tiltCenterMousePosition) {
        center = _tiltCenter;
    } else {
        double height = cartesianToCartographic(_cameraTrans->worldPosition()).height;
        if (height < 0)
            center = pickGlobe(Vector2(startPosition.x, startPosition.y));
        else {
            center = _cameraController->pickPoint(startPosition.x, startPosition.y/*, true*/);
            if (center.isNull())
                center = pickGlobe(Vector2(startPosition.x, startPosition.y));
        }

//        center = _cameraController->pickPoint(startPosition.x, startPosition.y/*, true*/);
//        if (center.isNull()) {
//            center = pickGlobe(Vector2(startPosition.x, startPosition.y));
//        }

        if (center.isNull()) {
            ray = _cameraController->getPickRay(startPosition.x, startPosition.y);
            intersection = IntersectionTests::rayEllipsoid(ray, _ellipsoid);
            if (!defined(intersection)) {
                Cartographic cartographic = _ellipsoid->cartesianToCartographic(_cameraTrans->worldPosition());
                if (cartographic.height <= _minimumTrackBallHeight) {
                    _looking = true;
                    _rotationAxis = _ellipsoid->geodeticSurfaceNormal(_cameraTrans->worldPosition());
                    look3D(startPosition, movement);
                    _rotationAxis = Cartesian3(0, 0, 0);
                    _tiltCenterMousePosition = startPosition;
                }
                return;
            }
            center = ray.getPoint(intersection.start);
        }

        _tiltCenterMousePosition = startPosition;
        _tiltCenter = center;
    }

    Cartesian2 windowPosition(_canvas->width() / 2, _tiltCenterMousePosition.y);
    ray = _cameraController->getPickRay(windowPosition.x, windowPosition.y);

    double mag = center.magnitude();
    Ellipsoid newEllipsoid(mag, mag, mag);

    intersection = IntersectionTests::rayEllipsoid(ray, &newEllipsoid);
    if (!defined(intersection)) {
        return;
    }

    double t = ray.origin.magnitude() > mag ? intersection.start : intersection.stop;
    Vector3 verticalCenter = ray.getPoint(t);

    Matrix4 transform = Transforms::eastNorthUpToFixedFrame(center, _ellipsoid);
    Matrix4 verticalTransform = Transforms::eastNorthUpToFixedFrame(verticalCenter, &newEllipsoid);

    Globe *oldGlobe = _globe;
    Ellipsoid *oldEllipsoid = _ellipsoid;
    _globe = nullptr;
    _ellipsoid = _sphereEllipsoid;
    _rotateFactor = 1.0;
    _rotateRateRangeAdjustment = 1.0;

    Cartesian3 constrainedAxis = Cartesian3::UNIT_Z;

    Matrix4 oldTransform = _cameraController->_transform;
    _cameraController->_setTransform(transform);

    Cartesian3 tangent = Cartesian3::cross(verticalCenter, _cameraController->positionWC());
    double dot = Cartesian3::dot(_cameraController->rightWC(), tangent);

    rotate3D(movement, constrainedAxis, false, true);

    _cameraController->_setTransform(verticalTransform); // try to move above, but not

    if (dot < 0.0) {
        bool flags;
        if (movement.pinch)
            flags = movement.angleAndHeight.startPosition.y > movement.angleAndHeight.endPosition.y;
        else {
            flags = movement.startPosition.y > movement.endPosition.y;
        }
        if (flags) {
            constrainedAxis = Cartesian3(Math::EPSILON20, 0, 0);
        }

        Cartesian3 oldConstrainedAxis = _cameraController->constrainedAxis;
        _cameraController->constrainedAxis = Cartesian3(Math::EPSILON20, 0, 0);

        rotate3D(movement, constrainedAxis, true, false);

        _cameraController->constrainedAxis = oldConstrainedAxis;
    } else {
        rotate3D(movement, constrainedAxis, true, false);
    }

    if (defined(_cameraController->constrainedAxis)) {
        Cartesian3 ZERO;
        Cartesian3 right = Cartesian3::cross(_cameraTrans->yaxis(), _cameraController->constrainedAxis).normalize();
        if (!CesiumCartesian3::equalsEpsilon(right, ZERO, Math::EPSILON6)) {
            if (Cartesian3::dot(right, _cameraTrans->xaxis()) < 0.0) {
                right = -right;
            }

            Cartesian3 rightCarte;
            Cartesian3 directionCarte = _cameraTrans->yaxis();
            Cartesian3 upCarte;

            upCarte = Cartesian3::cross(right, directionCarte);
            rightCarte = Cartesian3::cross(directionCarte, upCarte);
            upCarte.normalize();
            rightCarte.normalize();

            _cameraTrans->setAxes(rightCarte, directionCarte, upCarte);
        }
    }

    _cameraController->_setTransform(oldTransform);
    _globe = oldGlobe;
    _ellipsoid = oldEllipsoid;

    double radius = oldEllipsoid->maximumRadius();
    _rotateFactor = 1.0 / radius;
    _rotateRateRangeAdjustment = radius;

    Cartesian3 originalPosition = _cameraController->positionWC();

    if (_cameraController->positionWC() != originalPosition) {
        _cameraController->_setTransform(verticalTransform);
        _cameraController->worldToCameraCoordinates(originalPosition);

        double magSqrd = originalPosition.magnitudeSquared();
        if (_cameraTrans->worldPosition().magnitudeSquared() > magSqrd) {
            _cameraTrans->setWorldPosition(_cameraTrans->worldPosition().normalize() * sqrt(magSqrd));
        }

        double angle = CesiumCartesian3::angleBetween(originalPosition, _cameraTrans->worldPosition());
        Cartesian3 axis = Cartesian3::cross(originalPosition, _cameraTrans->worldPosition());
        axis.normalize();

        angle = Math::toDegrees(angle);
        Quaternion quaternion = Quaternion::fromAxisAndAngle(axis, angle);
        Matrix3 rotation = quaternion.toRotationMatrix();

        Cartesian3 direction = _cameraTrans->yaxis();
        Cartesian3 right;
        Cartesian3 up = _cameraTrans->zaxis();

        direction = rotation * direction;
        up = rotation * up;
        right = Cartesian3::cross(direction, up);
        up = Cartesian3::cross(right, direction);

        _cameraTrans->setAxes(right.normalize(), direction.normalize(), up.normalize());

        _cameraController->_setTransform(oldTransform);
    }
}

void ScreenSpaceCameraController::handleZoom(const Cartesian2 &startPosition, const CameraMovement &movement, double zoomFactor, double distanceMeasure, double unitPositionDotDirection)
{
    double percentage = Math::clamp(abs(unitPositionDotDirection), 0.25, 1.0);

    // distanceMeasure should be the height above the ellipsoid.
    // The zoomRate slows as it approaches the surface and stops minimumZoomDistance above it.
    double minHeight = minimumZoomDistance * percentage;
    double maxHeight = maximumZoomDistance;

    double minDistance = distanceMeasure - minHeight;
    double zoomRate = zoomFactor * minDistance;
    zoomRate = Math::clamp(zoomRate, _minimumZoomRate, _maximumZoomRate);

    double diff;
    if (movement.pinch)
        diff = movement.distance.endPosition.y - movement.distance.startPosition.y;
    else
        diff = movement.endPosition.y - movement.startPosition.y;
    double rangeWindowRatio = diff / _canvas->height();
    rangeWindowRatio = std::min(rangeWindowRatio, maximumMovementRatio);
    double distance = zoomRate * rangeWindowRatio;

    if (distance > 0.0 && abs(distanceMeasure - minHeight) < 1.0) {
        return;
    }

    if (distance < 0.0 && abs(distanceMeasure - maxHeight) < 1.0) {
        return;
    }

    if (distanceMeasure - distance < minHeight) {
        distance = distanceMeasure - minHeight - 1.0;
    } else if (distanceMeasure - distance > maxHeight) {
        distance = distanceMeasure - maxHeight;
    }

    bool sameStartPosition = startPosition == _zoomMouseStart;
    bool zoomingOnVector = _zoomingOnVector;
    bool rotatingZoom = _rotatingZoom;
    Cartesian3 pickedPosition;

    if (!sameStartPosition) {
        _zoomMouseStart = startPosition;
        pickedPosition = pickGlobe(Vector2(startPosition.x, startPosition.y));

        if (!pickedPosition.isNull()) {
            _useZoomWorldPosition = true;
            _zoomWorldPosition = pickedPosition;
        } else {
            _useZoomWorldPosition = false;
        }

        zoomingOnVector = _zoomingOnVector = false;
        rotatingZoom = _rotatingZoom = false;
    }

    if (!_useZoomWorldPosition) {
        Cartesian3 oldPos = _cameraTrans->worldPosition();
        Cartographic carto = cartesianToCartographic(oldPos);
        if (carto.height < 0) {
            carto.height = 0.8;
            _cameraTrans->setCartographic(carto);
        }
        else {
            _cameraController->zoomIn(distance);
        }
        return;
    }

    bool zoomOnVector = false;

    if (_cameraController->positionCartographic().height < 2000000) {
        rotatingZoom = true;
    }

    if (!sameStartPosition || rotatingZoom) {
        Cartesian3 cameraPositionNormal = _cameraTrans->worldPosition().normalized();

        if (_cameraController->positionCartographic().height < 3000.0 &&
                abs(Cartesian3::dot(_cameraTrans->yaxis(), cameraPositionNormal)) < 0.6) {
            zoomOnVector = true;
        } else {
            Cartesian2 centerPixel(_canvas->width() / 2, _canvas->height() / 2);
            Cartesian3 centerPosition = pickGlobe(Vector2(centerPixel.x, centerPixel.y));
            // If centerPosition is not defined, it means the globe does not cover the center position of screen

            if (!centerPosition.isNull() && _cameraController->positionCartographic().height < 1000000) {
                Cartesian3 cameraPosition = _cameraTrans->worldPosition();
                Cartesian3 target = _zoomWorldPosition;
                Cartesian3 targetNormal = target.normalized();

                if (Cartesian3::dot(targetNormal, cameraPositionNormal) < 0.0) {
                    return;
                }

                Cartesian3 forward = _cameraTrans->yaxis();
                Cartesian3 center = cameraPosition +  forward * 1000;

                Cartesian3 positionToTarget = target - cameraPosition;
                Cartesian3 positionToTargetNormal = positionToTarget.normalized();

                double alphaDot = Cartesian3::dot(cameraPositionNormal, positionToTargetNormal);
                double alpha = acos(-alphaDot);
                bool mbz = false;
                if (alphaDot >= 0.0) {
                    // We zoomed past the target, and this zoom is not valid anymore.
                    // This line causes the next zoom movement to pick a new starting point.
                    if (!_enableUnderGround) {
                       _zoomMouseStart.x = -1;
                       return;
                    }
                    else {
                        alpha = acos(alphaDot);
                        distance = -distance;
                        mbz = true;

                    }
                }
//                double alpha = acos(-alphaDot);
                double cameraDistance = cameraPosition.magnitude();
                double targetDistance = target.magnitude();
                double remainingDistance = cameraDistance - distance;
                double positionToTargetDistance = positionToTarget.magnitude();

                double gamma = asin(Math::clamp(positionToTargetDistance / targetDistance * sin(alpha), -1.0, 1.0));
                double delta = asin(Math::clamp(remainingDistance / targetDistance * sin(alpha), -1.0, 1.0));
                double beta = gamma - delta + alpha;

                Cartesian3 up = cameraPosition.normalized();
                Cartesian3 right = Cartesian3::cross(positionToTargetNormal, up);
                right.normalize();

                forward = Cartesian3::cross(up, right).normalize();

                // Calculate new position to move to
                center = center.normalized() * (center.magnitude() - distance);
                cameraPosition.normalize();
                cameraPosition = cameraPosition * remainingDistance;

                if (!mbz) {
                    // Pan
                    Cartesian3 pMid = (up * (cos(beta) - 1) + forward * sin(beta)) * remainingDistance;
                    cameraPosition += pMid;

                    up = center.normalized();
                    forward = Cartesian3::cross(up, right).normalize();

                    Cartesian3 cMid = (up * (cos(beta) - 1) + forward * sin(beta)) * center.magnitude();
                    center += cMid;
                }

                Cartesian3 oldPos = _cameraTrans->worldPosition();

                // Set new position
                _cameraTrans->setWorldPosition(cameraPosition);

                // add by feng
                if (cartesianToCartographic(_cameraTrans->worldPosition()).height < 1) {
                    _cameraTrans->setWorldPosition(oldPos);
                    return;
                }

                // Set new direction
                Cartesian3 directionCarte = (center - cameraPosition).normalize();
                Cartesian3 rightCarte;
                Cartesian3 upCarte = _cameraTrans->zaxis();

                // Set new right & up vectors
                rightCarte = Cartesian3::cross(directionCarte, upCarte);
                upCarte = Cartesian3::cross(rightCarte, directionCarte);

                _cameraTrans->setAxes(rightCarte.normalize(), directionCarte, upCarte.normalize());

                return;
            }

            if (!centerPosition.isNull()) {
                Cartesian3 positionNormal = centerPosition.normalized();
                Cartesian3 pickedNormal = _zoomWorldPosition.normalized();
                double dotProduct = Cartesian3::dot(pickedNormal, positionNormal);

                if (dotProduct > 0.0 && dotProduct < 1.0) {
                    double angle = CesiumMath::acosClamped(dotProduct);
                    Cartesian3 axis = Cartesian3::cross(pickedNormal, positionNormal);

                    double height = _cameraController->positionCartographic().height;
                    double denom = abs(angle) > Math::toRadians(20.0) ? height * 0.75 : height - distance;
                    double scalar = distance / denom;
                    _cameraController->rotate(axis, angle * scalar);
                }
            } else {
                zoomOnVector = true;
            }
        }

        _rotatingZoom = !zoomOnVector;
    }

    if ((!sameStartPosition && zoomOnVector) || zoomingOnVector) {
        Ray ray;
        Vector3 zoomMouseStartVec = _camera->worldToScreenPoint(_zoomWorldPosition);
        Cartesian2 zoomMouseStart(zoomMouseStartVec.x(), zoomMouseStartVec.y());
        if (!zoomMouseStart.isNull() && startPosition == _zoomMouseStart) {
            ray = _cameraController->getPickRay(zoomMouseStart.x, zoomMouseStart.y);
        } else {
            ray = _cameraController->getPickRay(startPosition.x, startPosition.y);
        }

        Cartesian3 rayDirection = ray.direction;
        _cameraController->move(rayDirection, distance);

        //相机位置低于地表0.9米
        Cartographic cameraCartographic = _ellipsoid->cartesianToCartographic(_cameraController->positionWC());
        double cameraHeight = cameraCartographic.height;
        if (!_enableUnderGround) {
            double height = m_globe->getHeight(cameraCartographic);
            if (height < 0)
                height = 0;
            height = height + 0.9;
            if (cameraHeight < height) {
               _cameraController->move(rayDirection, -distance);
            }
        }
        else {
            if (cameraHeight < -980) {
                _cameraController->move(rayDirection, -distance);
            }
        }

        _zoomingOnVector = true;
    } else {
        _cameraController->zoomIn(distance);
    }
}

void ScreenSpaceCameraController::handleKeyDown()
{
    if (lastTime == 0)
        lastTime = getTimestamp();
    quint64 time = getTimestamp();
    quint64 gap = time - lastTime;
    lastTime = time;

    Cartesian3 cameraCarte =  _cameraTrans->worldPosition();
    Cartographic cameraCarto = _ellipsoid->cartesianToCartographic(cameraCarte);
    double cameraHeight = cameraCarto.height;

    double moveRate = cameraHeight / 300.0 * (gap/k1);
    double pageMoveRate = cameraHeight / 100.0 * (gap/k1);

//    double frustumWidth = 2.0 * cameraHeight * tan(fovy * 0.5) * 1.6;
//    double rotateRate = (frustumWidth / (2 * M_PI * (cameraHeight + earthRadius)) / k2) * (1 + cameraHeight/30000000) * (gap/k1);

    bool getAOrLeft = _input->getKey(Qt::Key_A ) || _input->getKey(Qt::Key_Left);
    if (getAOrLeft && _enableInputs) {
        double startX = _canvas->width()/2.0;
        double startY = _canvas->height()/2.0;
        spin3DByKey(startX, startY, startX + 5, startY);
    }

    bool getDOrRight = _input->getKey(Qt::Key_D) || _input->getKey(Qt::Key_Right);
    if (getDOrRight && _enableInputs) {
        double startX = _canvas->width()/2.0;
        double startY = _canvas->height()/2.0;
        spin3DByKey(startX, startY, startX - 5, startY);
    }

    bool getWOrUp = _input->getKey(Qt::Key_W) || _input->getKey(Qt::Key_Up);
    if (getWOrUp && _enableInputs) {
        double startX = _canvas->width()/2.0;
        double startY = _canvas->height()/2.0;
        spin3DByKey(startX, startY, startX, startY + 5);
    }

    bool getSOrDown = _input->getKey(Qt::Key_S) || _input->getKey(Qt::Key_Down);
    if (getSOrDown && _enableInputs) {
        double startX = _canvas->width()/2.0;
        double startY = _canvas->height()/2.0;
        spin3DByKey(startX, startY, startX, startY - 5);
    }

    if ((_input->getKey(Qt::Key_Plus) || _input->getKey(Qt::Key_Equal)) && _enableInputs) {
        if (!_enableUnderGround) {
            double height = m_globe->getHeight(cameraCarto);
            if (height < 0)
                height = 0;
            height = height + 0.9;
            if (cameraHeight > height) {
                if ((cameraHeight - moveRate) <= height)
                     _cameraController->moveForward(cameraHeight - height);
                else
                     _cameraController->moveForward(moveRate);
            }
        }
        else {
            if (cameraHeight > -980) {
                if ((cameraHeight - moveRate) <= -980)
                     _cameraController->moveForward(cameraHeight + 980);
                else
                     _cameraController->moveForward(moveRate);
            }
        }
    }

    if (_input->getKey(Qt::Key_Minus) && _enableInputs) {
        if (cameraHeight < maxCameraHeight) {
            if ((cameraHeight + moveRate) >= maxCameraHeight)
                 _cameraController->moveBackward(maxCameraHeight - cameraHeight);
            else
                 _cameraController->moveBackward(moveRate);
        }
    }

    if (_input->getKey(Qt::Key_PageUp) && _enableInputs) {
        if (!_enableUnderGround) {
            double height = m_globe->getHeight(cameraCarto);
            if (height < 0)
                height = 0;
            height = height + 0.9;
            if (cameraHeight > height) {
                if ((cameraHeight - pageMoveRate) <= height)
                     _cameraController->moveForward(cameraHeight - height);
                else
                     _cameraController->moveForward(pageMoveRate);
            }
        }
        else {
            if (cameraHeight > -980) {
                if ((cameraHeight - pageMoveRate) <= -980)
                     _cameraController->moveForward(cameraHeight + 980);
                else
                     _cameraController->moveForward(pageMoveRate);
            }
        }
    }

    if (_input->getKey(Qt::Key_PageDown) && _enableInputs) {
        if (cameraHeight < maxCameraHeight) {
            if ((cameraHeight + pageMoveRate) >= maxCameraHeight)
                 _cameraController->moveBackward(maxCameraHeight - cameraHeight);
            else
                 _cameraController->moveBackward(pageMoveRate);
        }
    }

    if (m_looking) {
        if (!getAOrLeft && !getDOrRight && !getWOrUp && !getSOrDown && !m_touring)
            m_looking = false;
    }

    if (cameraHeight > maxCameraHeight) {
        Cartesian3 normal = _ellipsoid->geodeticSurfaceNormal(_cameraController->positionWC());
        _cameraController->move(-normal, cameraHeight - maxCameraHeight);
    }
}

void loadPlugin()
{
    ScreenSpaceCameraController *sscc = new ScreenSpaceCameraController();

    LiCamera::main()->addComponent(sscc);
    LiCamera::main()->setCameraController(sscc);
}

//void spin3DByKeyFunc(void *s, double startX, double startY, double endX, double endY, bool mouseUp)
//{
//    ((ScreenSpaceCameraController*)s)->spin3DByKey(startX, startY, endX, endY, mouseUp);
//}
