#include "licameracontroller.h"

LiCameraController::LiCameraController(LiNode *parent) : LiBehavior(parent)
{
}

bool LiCameraController::enableInputs() const
{
    return true;
}

void LiCameraController::setEnableInputs(bool enable)
{

}

bool LiCameraController::enableZoom() const
{
    return true;
}

void LiCameraController::setEnableZoom(bool enable)
{

}

bool LiCameraController::enableRotate() const
{
    return true;
}

void LiCameraController::setEnableRotate(bool enable)
{

}

bool LiCameraController::enableTilt() const
{
    return true;
}

void LiCameraController::setEnableTilt(bool enable)
{

}

bool LiCameraController::enableLook() const
{
    return true;
}

void LiCameraController::setEnableLook(bool enable)
{

}

bool LiCameraController::enableUnderGround() const
{
    return true;
}

void LiCameraController::setEnableUnderGround(bool enable)
{

}

bool LiCameraController::enablePan() const
{
    return true;
}

void LiCameraController::setEnablePan(bool enable)
{

}

double LiCameraController::minimumCollisionTerrainHeight() const
{
    return 0;
}

void LiCameraController::setMinimumCollisionTerrainHeight(double minimumCollisionTerrainHeight)
{

}

Vector3 LiCameraController::positionWC()
{
    return Vector3();
}

Vector3 LiCameraController::directionWC()
{
    return Vector3();
}

Vector3 LiCameraController::upWC()
{
    return Vector3();
}

Vector3 LiCameraController::rightWC()
{
    return Vector3();
}

Cartographic LiCameraController::positionCartographic()
{
    return Cartographic();
}

double LiCameraController::heading()
{
    return 0;
}

double LiCameraController::pitch()
{
    return 0;
}

double LiCameraController::roll()
{
    return 0;
}

Ray LiCameraController::getPickRay(double x, double y)
{
    return Ray();
}

void LiCameraController::setView(const Cartesian3 &destination, double heading, double pitch, double roll)
{

}

void LiCameraController::rotate(const Vector3 &axis, double angle)
{

}

void LiCameraController::rotateUp(double angle)
{

}

void LiCameraController::rotateDown(double angle)
{

}

void LiCameraController::rotateRight(double angle)
{

}

void LiCameraController::rotateLeft(double angle)
{

}

void LiCameraController::move(const Vector3 &dir, double amount)
{

}

void LiCameraController::moveForward(double amount)
{

}

void LiCameraController::moveBackward(double amount)
{

}

void LiCameraController::moveUp(double amount)
{

}

void LiCameraController::moveDown(double amount)
{

}

void LiCameraController::moveRight(double amount)
{

}

void LiCameraController::moveLeft(double amount)
{

}

void LiCameraController::look(const Vector3 &axis, double angle)
{

}

void LiCameraController::lookUp(double amount)
{

}

void LiCameraController::lookDown(double amount)
{

}

void LiCameraController::lookRight(double amount)
{

}

void LiCameraController::lookLeft(double amount)
{

}

void LiCameraController::zoomIn(double amount)
{

}

void LiCameraController::zoomOut(double amount)
{

}

void LiCameraController::flyTo(const Vector3 &destination, double duration, double heading, double pitch, double roll)
{

}

void LiCameraController::flyTo(const Cartographic &destination, double duration, double heading, double pitch, double roll)
{

}

void LiCameraController::flyTo(const LiRectangle &destination, double duration, double heading, double pitch, double roll)
{

}

Cartesian3 LiCameraController::pickGlobe(const Vector2 &mousePosition) const
{
    return Cartesian3();
}
