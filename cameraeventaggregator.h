#ifndef CAMERAEVENTAGGREGATOR_H
#define CAMERAEVENTAGGREGATOR_H

#include <QObject>
#include "screenspaceeventutils.h"
#include "cartesian2.h"

class LiWidget;
class LiEngine;
class LiInputSystem;
class ScreenSpaceEventHandler;

/**
 * @brief 相机输入事件结构体
 *
 */
struct CameraEventData {
    bool update = true; ///< 鼠标是否拖拽
    bool isDown = false; ///< 鼠标是否按下
    CameraMovement movement; ///< 鼠标移动信息
    CameraMovement lastMovement; ///< 鼠标最后的移动信息
    Cartesian2 eventStartPosition; ///< 鼠标事件触发的初始位置 (屏幕坐标)
    quint64 pressTime = 0; ///< 鼠标按下的时间戳
    quint64 releaseTime = 0; ///< 鼠标释放的时间戳
};

/**
 * @brief 相机事件合集
 *
 */
class CameraEventAggregator : public QObject
{
    Q_OBJECT
public:   
    /**
     * @brief 构造函数
     *
     * @param canvas LiWidget窗口
     * @param parent 父类指针
     */
    explicit CameraEventAggregator(LiWidget *canvas, QObject *parent = nullptr);

    /**
     * @brief 析构函数
     *
     */
    ~CameraEventAggregator();

    /**
     * @brief 鼠标当前位置 (屏幕坐标)
     *
     * @return Cartesian2 返回Cartesian2类型
     */
    Cartesian2 currentMousePosition() const { return _currentMousePosition; }

    /**
     * @brief 检测是否点击了鼠标或按下了指定的按键
     *
     * @return bool true: 是, false: 否
     */
    bool anyButtonDown() const;

    /**
     * @brief 检测鼠标是否在按下的情况下拖拽
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     * @return bool true: 是, false: 否
     */
    bool isMoving(CameraEventType::Type type, int modifier) const;

    /**
     * @brief 获取鼠标的移动信息
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     * @return CameraMovement 结构体, 包含鼠标的移动信息
     */
    CameraMovement getMovement(CameraEventType::Type type, int modifier) const;

    /**
     * @brief 获取鼠标最后的移动信息
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     * @return CameraMovement 结构体, 包含鼠标的移动信息
     */
    CameraMovement getLastMovement(CameraEventType::Type type, int modifier) const;

    /**
     * @brief 检测鼠标是否按下
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     * @return bool true: 是, false: 否
     */
    bool isButtonDown(CameraEventType::Type type, int modifier) const;

    /**
     * @brief 获取鼠标按下的初始位置
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     * @return Cartesian2 返回Cartesian2类型
     */
    Cartesian2 getStartMousePosition(CameraEventType::Type type, int modifier) const;

    /**
     * @brief 获取鼠标按下的时间戳
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     * @return quint64 时间戳
     */
    quint64 getButtonPressTime(CameraEventType::Type type, int modifier) const;

    /**
     * @brief 获取鼠标释放的时间戳
     *
     * @param type 鼠标事件的类型 (左键拖拽, 右键拖拽, 中键拖拽...)
     * @param modifier 键盘按下的键 (shitf, ctrl...默认为0, 表示不按下任何键)
     * @return quint64 时间戳
     */
    quint64 getButtonReleaseTime(CameraEventType::Type type, int modifier) const;

    /**
     * @brief 执行完一帧后, 重置结构体CameraEventData里面的update变量为true
     *
     */
    void reset();

    LiInputSystem *inputSystem; ///< 输入系统

private:
    quint64 getKey(int type, int modifier = 0) const;

    void listenToPinch(int modifier, LiWidget *canvas);
    void listenToWheel(int modifier);
    void listenMouseButtonDownUp(int modifier, CameraEventType::Type type);
    void listenMouseMove(int modifier);
    void clonePinchMovement(CameraMovement pinchMovement, CameraMovement &result);
    void cloneMouseMovement(CameraMovement mouseMovement, CameraMovement &result);

    CameraEventData *getOrCreateEventData(quint64 key);

    ScreenSpaceEventHandler *_eventHandler = nullptr;
    QHash<quint64, CameraEventData*> _eventData;
    LiWidget *_canvas;

    Cartesian2 _currentMousePosition;
    int _buttonsDown = 0;
};

#endif // CAMERAEVENTAGGREGATOR_H
