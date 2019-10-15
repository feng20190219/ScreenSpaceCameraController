#include "cameraeventaggregator.h"
#include "screenspaceeventhandler.h"
#include "timestamp.h"
#include "limath.h"
#include "liwidget.h"
#include "liviewer.h"
#include "liengine.h"
#include "liinputsystem.h"

CameraEventAggregator::CameraEventAggregator(LiWidget *canvas, QObject *parent) :  QObject(parent)
{
    _canvas = canvas;

    inputSystem = GlobalViewer()->engine()->inputSystem();

    _eventHandler = new ScreenSpaceEventHandler();
    _eventHandler->setInputSystem(inputSystem);

    const int modifiers[] = {
        0,
        Qt::Key_Shift,
        Qt::Key_Control,
//        Qt::Key_Alt
    };

    for (int i = 0; i < 3; ++i) {
        int modifier = modifiers[i];
        listenToWheel(modifier);
        listenToPinch(modifier, canvas);
        listenMouseButtonDownUp(modifier, CameraEventType::LEFT_DRAG);
        listenMouseButtonDownUp(modifier, CameraEventType::RIGHT_DRAG);
        listenMouseButtonDownUp(modifier, CameraEventType::MIDDLE_DRAG);
        listenMouseMove(modifier);
    }
}

CameraEventAggregator::~CameraEventAggregator()
{
    delete _eventHandler;
    qDeleteAll(_eventData);
}

bool CameraEventAggregator::anyButtonDown() const
{
    bool wheelMoved = !_eventData[getKey((int)CameraEventType::WHEEL)]->update ||
            !_eventData[getKey((int)CameraEventType::WHEEL, Qt::Key_Shift)]->update ||
            !_eventData[getKey((int)CameraEventType::WHEEL, Qt::Key_Control)]->update;
//            !_eventData[getKey((int)CameraEventType::WHEEL, Qt::Key_Alt)]->update;
    return _buttonsDown > 0 || wheelMoved;
}

bool CameraEventAggregator::isMoving(CameraEventType::Type type, int modifier) const
{
    return !_eventData[getKey((int)type, modifier)]->update;
}

CameraMovement CameraEventAggregator::getMovement(CameraEventType::Type type, int modifier) const
{
    return _eventData[getKey((int)type, modifier)]->movement;
}

CameraMovement CameraEventAggregator::getLastMovement(CameraEventType::Type type, int modifier) const
{
   return _eventData[getKey((int)type, modifier)]->lastMovement;
}

bool CameraEventAggregator::isButtonDown(CameraEventType::Type type, int modifier) const
{
    return _eventData[getKey((int)type, modifier)]->isDown;
}

Cartesian2 CameraEventAggregator::getStartMousePosition(CameraEventType::Type type, int modifier) const
{
    if (type == CameraEventType::WHEEL) {
        return _currentMousePosition;
    }

    return _eventData[getKey((int)type, modifier)]->eventStartPosition;
}

quint64 CameraEventAggregator::getButtonPressTime(CameraEventType::Type type, int modifier) const
{
    return _eventData[getKey((int)type, modifier)]->pressTime;
}

quint64 CameraEventAggregator::getButtonReleaseTime(CameraEventType::Type type, int modifier) const
{
    return _eventData[getKey((int)type, modifier)]->releaseTime;
}

void CameraEventAggregator::reset()
{
    for(CameraEventData *data : _eventData) {
        data->update = true;
    }
}

quint64 CameraEventAggregator::getKey(int type, int modifier) const
{
    return quint64(type) | (quint64(modifier) << 32);
}

void CameraEventAggregator::listenToPinch(int modifier, LiWidget *canvas)
{
    quint64 key = getKey((int)CameraEventType::PINCH, modifier);

    CameraEventData *data = getOrCreateEventData(key);

    data->update = true;
    data->isDown = false;
    data->eventStartPosition = Cartesian2();

    data->movement.distance.startPosition = Cartesian2();
    data->movement.distance.endPosition = Cartesian2();
    data->movement.angleAndHeight.startPosition = Cartesian2();
    data->movement.angleAndHeight.endPosition = Cartesian2();
    data->movement.prevAngle = 0.0;
    data->movement.pinch = true;

    _eventHandler->setInputAction([=](ScreenSpaceMouseEventPtr event) {
        _buttonsDown++;
        data->isDown = true;
        data->pressTime = getTimestamp();

        // Compute center position and store as start point.
//        Cartesian2.lerp(event->position1, event->position2, 0.5, data->eventStartPosition);
        Cartesian2 lerpScratch = event->position2 * 0.5;
        Cartesian2 result = event->position1 * 0.5;
        data->eventStartPosition = lerpScratch + result;
    }, ScreenSpaceEventType::PINCH_START, modifier);

    _eventHandler->setInputAction([=](ScreenSpaceMouseEventPtr event) {
        _buttonsDown = std::max(_buttonsDown - 1, 0);
        data->isDown = false;
        data->releaseTime = getTimestamp();
    }, ScreenSpaceEventType::PINCH_END, modifier);

    _eventHandler->setInputAction([=](ScreenSpaceMouseEventPtr event) {
        if (data->isDown) {
            // Aggregate several input events into a single animation frame.
            if (!data->update) {
                data->movement.distance.endPosition = event->movement.distance.endPosition;
                data->movement.angleAndHeight.endPosition = event->movement.angleAndHeight.endPosition;
            } else {
                clonePinchMovement(event->movement, data->movement);
                data->update = false;
                data->movement.prevAngle = data->movement.angleAndHeight.startPosition.x;
            }
            // Make sure our aggregation of angles does not "flip" over 360 degrees.
            double angle = data->movement.angleAndHeight.endPosition.x;
            double prevAngle = data->movement.prevAngle;
            double TwoPI = M_PI * 2;
            while (angle >= (prevAngle + M_PI)) {
                angle -= TwoPI;
            }
            while (angle < (prevAngle - M_PI)) {
                angle += TwoPI;
            }
            data->movement.angleAndHeight.endPosition.x = -angle * canvas->width() / 12;
            data->movement.angleAndHeight.startPosition.x = -prevAngle * canvas->width() / 12;
        }
    }, ScreenSpaceEventType::PINCH_MOVE, modifier);
}

void CameraEventAggregator::listenToWheel(int modifier)
{
    quint64 key = getKey((int)CameraEventType::WHEEL, modifier);

    CameraEventData *data = getOrCreateEventData(key);
    data->update = true;

    data->movement.startPosition = Cartesian2();
    data->movement.endPosition = Cartesian2();

    _eventHandler->setInputAction([=](ScreenSpaceMouseEventPtr event) {
        // TODO: magic numbers
        double arcLength = 60.0 * Math::toRadians(event->deltaY);
        if (!data->update) {
            data->movement.endPosition.y += arcLength;
        } else {
            data->movement.startPosition = Cartesian2();
            data->movement.endPosition.x = 0;
            data->movement.endPosition.y = arcLength;
            data->update = false;
        }
    }, ScreenSpaceEventType::WHEEL, modifier);
}

void CameraEventAggregator::listenMouseButtonDownUp(int modifier, CameraEventType::Type type)
{
    quint64 key = getKey((int)type, modifier);

    CameraEventData *data = getOrCreateEventData(key);

    data->isDown = false;
    data->eventStartPosition = Cartesian2();

    ScreenSpaceEventType::Type down;
    ScreenSpaceEventType::Type up;
    if (type == CameraEventType::LEFT_DRAG) {
        down = ScreenSpaceEventType::LEFT_DOWN;
        up = ScreenSpaceEventType::LEFT_UP;
    } else if (type == CameraEventType::RIGHT_DRAG) {
        down = ScreenSpaceEventType::RIGHT_DOWN;
        up = ScreenSpaceEventType::RIGHT_UP;
    } else if (type == CameraEventType::MIDDLE_DRAG) {
        down = ScreenSpaceEventType::MIDDLE_DOWN;
        up = ScreenSpaceEventType::MIDDLE_UP;
    } else {
        return;
    }

    _eventHandler->setInputAction([=](ScreenSpaceMouseEventPtr event) {
        _buttonsDown++;
        data->lastMovement.valid = false;
        data->isDown = true;
        data->pressTime = getTimestamp();
        data->eventStartPosition = event->position;
    }, down, modifier);

    _eventHandler->setInputAction([=](ScreenSpaceMouseEventPtr event) {
        _buttonsDown = std::max(_buttonsDown - 1, 0);
        data->isDown = false;
        data->releaseTime = getTimestamp();
    }, up, modifier);
}

void CameraEventAggregator::listenMouseMove(int modifier)
{
    for (int i = 0; i < 4; ++i) {
        quint64 key = getKey(i, modifier);
        CameraEventData *data = getOrCreateEventData(key);
        data->update = true;
    }

    _eventHandler->setInputAction([=](ScreenSpaceMouseEventPtr event) {
        for (int i = 0; i < 4; ++i) {
            quint64 key = getKey(i, modifier);
            CameraEventData *data = _eventData[key];

            if (data->isDown) {
                if (!data->update) {
                    data->movement.endPosition = event->movement.endPosition;
                } else {
                    cloneMouseMovement(data->movement, data->lastMovement);
                    data->lastMovement.valid = true;
                    cloneMouseMovement(event->movement, data->movement);
                    data->update = false;
                }
            }
        }

        _currentMousePosition = event->movement.endPosition;
    }, ScreenSpaceEventType::MOUSE_MOVE, modifier);
}

CameraEventData *CameraEventAggregator::getOrCreateEventData(quint64 key)
{
    CameraEventData *data = _eventData.value(key, nullptr);
    if(!data) {
        data = new CameraEventData;
        _eventData[key] = data;
    }

    return data;
}

void CameraEventAggregator::clonePinchMovement(CameraMovement pinchMovement, CameraMovement &result)
{
    result.distance.startPosition = pinchMovement.distance.startPosition;
    result.distance.endPosition = pinchMovement.distance.endPosition;

    result.angleAndHeight.startPosition = pinchMovement.angleAndHeight.startPosition;
    result.angleAndHeight.endPosition = pinchMovement.angleAndHeight.endPosition;
}

void CameraEventAggregator::cloneMouseMovement(CameraMovement mouseMovement, CameraMovement &result)
{
    result.startPosition = mouseMovement.startPosition;
    result.endPosition = mouseMovement.endPosition;
}
