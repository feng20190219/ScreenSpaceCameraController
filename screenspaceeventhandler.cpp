#include "screenspaceeventhandler.h"
#include "timestamp.h"
#include "liinputsystem.h"

ScreenSpaceEventHandler::ScreenSpaceEventHandler(QObject *parent)
    : QObject(parent)
{
}

void ScreenSpaceEventHandler::setInputSystem(LiInputSystem *inputSystem)
{
    _inputSystem = inputSystem;

    std::function<ScreenSpaceMouseEventPtr(int)> getScreenSpaceMouseEvent = [this](int button) {
        ScreenSpaceMouseEvent *event = new ScreenSpaceMouseEvent;
        ScreenSpaceMouseEventPtr eventPtr = ScreenSpaceMouseEventPtr(event);
        eventPtr->button = button;
        eventPtr->modifier = getModifier(_inputSystem);
        QPoint mousePosition = _inputSystem->mousePosition();
        eventPtr->position = Cartesian2(mousePosition.x(),
                                        mousePosition.y());
        return eventPtr;
    };

    connect(_inputSystem, &LiInputSystem::leftButtonDown, [=]() {
//        qDebug() << "leftButtonDown";
        handleMouseDown(getScreenSpaceMouseEvent(Qt::LeftButton));
    });
    connect(_inputSystem, &LiInputSystem::leftButtonUp, [=]() {
//        qDebug() << "leftButtonUp";
        handleMouseUp(getScreenSpaceMouseEvent(Qt::LeftButton));
    });
//    connect(_inputSystem, &LiInputSystem::leftButtonDoubleClick, [=]() {
//        handleDblClick(getScreenSpaceMouseEvent(Qt::LeftButton));
//    });
    connect(_inputSystem, &LiInputSystem::rightButtonDown, [=]() {
//        qDebug() << "rightButtonDown";
        handleMouseDown(getScreenSpaceMouseEvent(Qt::RightButton));
    });
    connect(_inputSystem, &LiInputSystem::rightButtonUp, [=]() {
//        qDebug() << "rightButtonUp";
        handleMouseUp(getScreenSpaceMouseEvent(Qt::RightButton));
    });
    connect(_inputSystem, &LiInputSystem::middleButtonDown, [=]() {
//        qDebug() << "middleButtonDown";
        handleMouseDown(getScreenSpaceMouseEvent(Qt::MiddleButton));
    });
    connect(_inputSystem, &LiInputSystem::middleButtonUp, [=]() {
//        qDebug() << "middleButtonUp";
        handleMouseUp(getScreenSpaceMouseEvent(Qt::MiddleButton));
    });
    connect(_inputSystem, &LiInputSystem::mouseMoving, [=]() {
//        qDebug() << "mouseMoving";
        handleMouseMove(getScreenSpaceMouseEvent(0));
    });
    connect(_inputSystem, &LiInputSystem::mouseWheeling, [=](int deltaX, int deltaY) {
//        qDebug() << "mouseWheeling";
        ScreenSpaceMouseEventPtr event = getScreenSpaceMouseEvent(0);
        event->deltaY = deltaY;
        handleWheel(event);
    });
//    connect(_inputSystem, &LiInputSystem::keyDown, [=](int k) {
//        handleKeyDown(getScreenSpaceMouseEvent(k));
//    });
//    connect(_inputSystem, &LiInputSystem::keyUp, [=]() {

    //    });
}

void ScreenSpaceEventHandler::setInputAction(InputAction action, ScreenSpaceEventType::Type type, int modifier)
{
    quint64 key = getInputEventKey((int)type, modifier);
    _inputEvents[key] = action;
}

ScreenSpaceEventHandler::InputAction ScreenSpaceEventHandler::getInputAction(ScreenSpaceEventType::Type type, int modifier) const
{
    quint64 key = getInputEventKey((int)type, modifier);
    return _inputEvents.value(key, nullptr);
}

void ScreenSpaceEventHandler::removeInputAction(ScreenSpaceEventType::Type type, int modifier)
{
    quint64 key = getInputEventKey((int)type, modifier);
    _inputEvents.remove(key);
}

int ScreenSpaceEventHandler::getModifier(LiInputSystem *inputSystem) const
{
    if(inputSystem->getKey(Qt::Key_Shift))
        return Qt::Key_Shift;
    else if(inputSystem->getKey(Qt::Key_Control))
        return Qt::Key_Control;
//    else if(inputSystem->getKey(Qt::Key_Alt))
//        return Qt::Key_Alt;
    else
        return 0;
}

quint64 ScreenSpaceEventHandler::getInputEventKey(int type, int modifier) const
{
    return quint64(type) | (quint64(modifier) << 32);
}

void ScreenSpaceEventHandler::gotTouchEvent()
{
    _lastSeenTouchEvent = getTimestamp();
}

bool ScreenSpaceEventHandler::canProcessMouseEvent() const
{
    return (getTimestamp() - _lastSeenTouchEvent) > mouseEmulationIgnoreMilliseconds;
}

void ScreenSpaceEventHandler::handleMouseDown(ScreenSpaceMouseEventPtr event)
{
    if (!canProcessMouseEvent()) {
        return;
    }
    int button = event->button;
    _buttonDown = button;

    ScreenSpaceEventType::Type type;
    if (button == Qt::LeftButton) {
        type = ScreenSpaceEventType::LEFT_DOWN;
    } else if (button == Qt::MiddleButton) {
        type = ScreenSpaceEventType::MIDDLE_DOWN;
    } else if (button == Qt::RightButton) {
        type = ScreenSpaceEventType::RIGHT_DOWN;
    } else {
        return;
    }

    Cartesian2 position = event->position;
    _primaryStartPosition = position;
    _primaryPreviousPosition = position;

    int modifier = event->modifier;

    InputAction action = getInputAction(type, modifier);

    if (action) {
        action(event);
    }
}

void ScreenSpaceEventHandler::handleMouseUp(ScreenSpaceMouseEventPtr event)
{
    if (!canProcessMouseEvent()) {
        return;
    }
    int button = event->button;
    _buttonDown = 0;

    ScreenSpaceEventType::Type type;
    ScreenSpaceEventType::Type clickType;
    if (button == Qt::LeftButton) {
        type = ScreenSpaceEventType::LEFT_UP;
        clickType = ScreenSpaceEventType::LEFT_CLICK;
    } else if (button == Qt::MiddleButton) {
        type = ScreenSpaceEventType::MIDDLE_UP;
        clickType = ScreenSpaceEventType::MIDDLE_CLICK;
    } else if (button == Qt::RightButton) {
        type = ScreenSpaceEventType::RIGHT_UP;
        clickType = ScreenSpaceEventType::RIGHT_CLICK;
    } else {
        return;
    }

    int modifier = event->modifier;

    InputAction action = getInputAction(type, modifier);
    InputAction clickAction = getInputAction(clickType, modifier);

    if (action || clickAction) {

        if (action) {
            action(event);
        }

        if (clickAction) {
            Cartesian2 startPosition = _primaryStartPosition;
            double xDiff = startPosition.x - event->position.x;
            double yDiff = startPosition.y - event->position.y;
            double totalPixels = sqrt(xDiff * xDiff + yDiff * yDiff);

            if (totalPixels < _clickPixelTolerance) {
                clickAction(event);
            }
        }
    }
}

void ScreenSpaceEventHandler::handleMouseMove(ScreenSpaceMouseEventPtr event)
{
    if (!canProcessMouseEvent()) {
        return;
    }

    int modifier = event->modifier;

    Cartesian2 position = event->position;
    Cartesian2 previousPosition = _primaryPreviousPosition;

    InputAction action = getInputAction(ScreenSpaceEventType::MOUSE_MOVE, modifier);

    if (action) {
        event->movement.startPosition = previousPosition;
        event->movement.endPosition = position;

        action(event);
    }

    _primaryPreviousPosition = position;
}

void ScreenSpaceEventHandler::handleDblClick(ScreenSpaceMouseEventPtr event)
{
    int button = event->button;

    ScreenSpaceEventType::Type type;
    if (button == Qt::LeftButton) {
        type = ScreenSpaceEventType::LEFT_DOUBLE_CLICK;
    } else {
        return;
    }

    int modifier = event->modifier;

    InputAction action = getInputAction(type, modifier);

    if (action) {
        action(event);
    }
}

void ScreenSpaceEventHandler::handleWheel(ScreenSpaceMouseEventPtr event)
{
    if (event->deltaY) {
        int modifier = event->modifier;

        InputAction action = getInputAction(ScreenSpaceEventType::WHEEL, modifier);

        if (action) {
            action(event);
        }
    }
}

void ScreenSpaceEventHandler::handleTouchStart(ScreenSpaceMouseEventPtr event)
{
    gotTouchEvent();

//    var changedTouches = event.changedTouches;

//    var i;
//    var length = changedTouches.length;
//    var touch;
//    var identifier;
//    var positions = screenSpaceEventHandler._positions;

//    for (i = 0; i < length; ++i) {
//        touch = changedTouches[i];
//        identifier = touch.identifier;
//        positions.set(identifier, getPosition(screenSpaceEventHandler, touch, new Cartesian2()));
//    }

//    fireTouchEvents(screenSpaceEventHandler, event);

//    var previousPositions = screenSpaceEventHandler._previousPositions;

//    for (i = 0; i < length; ++i) {
//        touch = changedTouches[i];
//        identifier = touch.identifier;
//        previousPositions.set(identifier, Cartesian2.clone(positions.get(identifier)));
//    }
}

void ScreenSpaceEventHandler::handleTouchEnd(ScreenSpaceMouseEventPtr event)
{
    gotTouchEvent();

//    var changedTouches = event.changedTouches;

//    var i;
//    var length = changedTouches.length;
//    var touch;
//    var identifier;
//    var positions = screenSpaceEventHandler._positions;

//    for (i = 0; i < length; ++i) {
//        touch = changedTouches[i];
//        identifier = touch.identifier;
//        positions.remove(identifier);
//    }

//    fireTouchEvents(screenSpaceEventHandler, event);

//    var previousPositions = screenSpaceEventHandler._previousPositions;

//    for (i = 0; i < length; ++i) {
//        touch = changedTouches[i];
//        identifier = touch.identifier;
//        previousPositions.remove(identifier);
//    }
}

void ScreenSpaceEventHandler::handleTouchMove(ScreenSpaceMouseEventPtr event)
{
    gotTouchEvent();

//    var changedTouches = event.changedTouches;

//    var i;
//    var length = changedTouches.length;
//    var touch;
//    var identifier;
//    var positions = screenSpaceEventHandler._positions;

//    for (i = 0; i < length; ++i) {
//        touch = changedTouches[i];
//        identifier = touch.identifier;
//        var position = positions.get(identifier);
//        if (defined(position)) {

//            getPosition(screenSpaceEventHandler, touch, position);
//        }
//    }

//    fireTouchMoveEvents(screenSpaceEventHandler, event);

//    var previousPositions = screenSpaceEventHandler._previousPositions;

//    for (i = 0; i < length; ++i) {
//        touch = changedTouches[i];
//        identifier = touch.identifier;
//        Cartesian2.clone(positions.get(identifier), previousPositions.get(identifier));
//    }
}

void ScreenSpaceEventHandler::fireTouchEvents(ScreenSpaceMouseEventPtr event)
{
//    var modifier = getModifier(event);
//    var positions = screenSpaceEventHandler._positions;
//    var previousPositions = screenSpaceEventHandler._previousPositions;
//    var numberOfTouches = positions.length;
//    var action;
//    var clickAction;
//    var pinching = screenSpaceEventHandler._isPinching;

//    if (numberOfTouches !== 1 && screenSpaceEventHandler._buttonDown === MouseButton.LEFT) {
//        // transitioning from single touch, trigger UP and might trigger CLICK
//        screenSpaceEventHandler._buttonDown = undefined;
//        action = screenSpaceEventHandler.getInputAction(ScreenSpaceEventType.LEFT_UP, modifier);

//        if (defined(action)) {
//            Cartesian2.clone(screenSpaceEventHandler._primaryPosition, touchEndEvent.position);

//            action(touchEndEvent);
//        }

//        if (numberOfTouches === 0) {
//            // releasing single touch, check for CLICK
//            clickAction = screenSpaceEventHandler.getInputAction(ScreenSpaceEventType.LEFT_CLICK, modifier);

//            if (defined(clickAction)) {
//                var startPosition = screenSpaceEventHandler._primaryStartPosition;
//                var endPosition = previousPositions.values[0];
//                var xDiff = startPosition.x - endPosition.x;
//                var yDiff = startPosition.y - endPosition.y;
//                var totalPixels = Math.sqrt(xDiff * xDiff + yDiff * yDiff);

//                if (totalPixels < screenSpaceEventHandler._clickPixelTolerance) {
//                    Cartesian2.clone(screenSpaceEventHandler._primaryPosition, touchClickEvent.position);

//                    clickAction(touchClickEvent);
//                }
//            }
//        }

//        // Otherwise don't trigger CLICK, because we are adding more touches.
//    }

//    if (numberOfTouches === 0 && pinching) {
//        // transitioning from pinch, trigger PINCH_END
//        screenSpaceEventHandler._isPinching = false;

//        action = screenSpaceEventHandler.getInputAction(ScreenSpaceEventType.PINCH_END, modifier);

//        if (defined(action)) {
//            action();
//        }
//    }

//    if (numberOfTouches === 1 && !pinching) {
//        // transitioning to single touch, trigger DOWN
//        var position = positions.values[0];
//        Cartesian2.clone(position, screenSpaceEventHandler._primaryPosition);
//        Cartesian2.clone(position, screenSpaceEventHandler._primaryStartPosition);
//        Cartesian2.clone(position, screenSpaceEventHandler._primaryPreviousPosition);

//        screenSpaceEventHandler._buttonDown = MouseButton.LEFT;

//        action = screenSpaceEventHandler.getInputAction(ScreenSpaceEventType.LEFT_DOWN, modifier);

//        if (defined(action)) {
//            Cartesian2.clone(position, touchStartEvent.position);

//            action(touchStartEvent);
//        }

//        event.preventDefault();
//    }

//    if (numberOfTouches === 2 && !pinching) {
//        // transitioning to pinch, trigger PINCH_START
//        screenSpaceEventHandler._isPinching = true;

//        action = screenSpaceEventHandler.getInputAction(ScreenSpaceEventType.PINCH_START, modifier);

//        if (defined(action)) {
//            Cartesian2.clone(positions.values[0], touch2StartEvent.position1);
//            Cartesian2.clone(positions.values[1], touch2StartEvent.position2);

//            action(touch2StartEvent);

//            // Touch-enabled devices, in particular iOS can have many default behaviours for
//            // "pinch" events, which can still be executed unless we prevent them here.
//            event.preventDefault();
//        }
//    }
}
