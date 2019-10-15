#ifndef SCREENSPACEEVENTHANDLER_H
#define SCREENSPACEEVENTHANDLER_H

#include <QObject>
#include <functional>
#include "screenspaceeventutils.h"

class LiInputSystem;
class CameraEventAggregator;

/**
 * @brief 处理用户输入事件
 *
 */
class ScreenSpaceEventHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief  默认构造
     *
     * @param parent 父类指针
     */
    explicit ScreenSpaceEventHandler(QObject *parent = nullptr);

    /**
     * @brief 设置输入系统
     *
     * @param inputSystem 输入系统
     */
    void setInputSystem(LiInputSystem *inputSystem);

    /**
     * @brief 定义一个仅以一个ScreenSpaceMouseEventPtr类型为参数且没有返回值的函数
     *
     */
    typedef std::function<void(ScreenSpaceMouseEventPtr)> InputAction;

    /**
     * @brief 设置要在输入事件上执行的函数
     *
     * @param action 要在输入事件上执行的函数, InputAction类型
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     */
    void setInputAction(InputAction action, ScreenSpaceEventType::Type type, int modifier);

    /**
     * @brief 获取在输入事件上执行的函数
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     * @return InputAction 在输入事件上执行的函数
     */
    InputAction getInputAction(ScreenSpaceEventType::Type type, int modifier) const;

    /**
     * @brief 移除在输入事件上执行的函数
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     */
    void removeInputAction(ScreenSpaceEventType::Type type, int modifier);

private:
    int getModifier(LiInputSystem *inputSystem) const;
    int getModifier(QKeyEvent *event) const;
    quint64 getInputEventKey(int type, int modifier) const;
    void gotTouchEvent();
    bool canProcessMouseEvent() const;
    void handleMouseDown(ScreenSpaceMouseEventPtr event);
    void handleMouseUp(ScreenSpaceMouseEventPtr event);
    void handleMouseMove(ScreenSpaceMouseEventPtr event);
    void handleDblClick(ScreenSpaceMouseEventPtr event);
    void handleWheel(ScreenSpaceMouseEventPtr event);
    void handleTouchStart(ScreenSpaceMouseEventPtr event);
    void handleTouchEnd(ScreenSpaceMouseEventPtr event);
    void handleTouchMove(ScreenSpaceMouseEventPtr event);
    void fireTouchEvents(ScreenSpaceMouseEventPtr event);

    LiInputSystem *_inputSystem;

    QHash<quint64, InputAction> _inputEvents;
    int _buttonDown = 0;
    bool _isPinching = false;
    quint64 mouseEmulationIgnoreMilliseconds = 800;
    quint64 _lastSeenTouchEvent = -800;
    Cartesian2 _primaryStartPosition;
    Cartesian2 _primaryPosition;
    Cartesian2 _primaryPreviousPosition;
    QHash<QString, Cartesian2> _positions;
    QHash<QString, Cartesian2> _previousPositions;
    QHash<QString, Cartesian2> _removalFunctions;
    int _clickPixelTolerance = 5;
};

#endif // SCREENSPACEEVENTHANDLER_H
