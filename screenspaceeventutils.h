#ifndef SCREENSPACEEVENTUTILS_H
#define SCREENSPACEEVENTUTILS_H

#include "cartesian2.h"
#include "cartesian3.h"
#include "interval.h"
#include "matrix4.h"

/**
 * @brief 定义一个没有参数且没有返回值的函数
 *
 */
typedef std::function<void()> TweenAction;

/**
 * @brief 定义一个仅以一个double类型为参数且没有返回值的函数
 *
 */
typedef std::function<void(double k)> TweenAction1;

/**
 * @brief 定义一个仅以一个double类型为参数且返回值类型为double的函数
 *
 */
typedef std::function<double(double k)> TweenAction1Double;

/**
 * @brief 相机信息结构体
 *
 */
struct CameraNewOptions {
    Cartesian3 destination; ///< 目标位置
    double heading = 0.0; ///< 相机的heading
    double pitch = 0.0; ///< 相机的pitch
    double roll = 0.0; ///< 相机的roll
    double duration = Math::EPSILON20; ///< 持续的时间段
    TweenAction complete = nullptr; ///< 完成函数
    TweenAction cancel = nullptr; ///< 取消函数
    TweenAction1Double easingFunction = nullptr; ///< 插值函数
};

/**
 * @brief 相机移动结构体
 *
 */
struct CameraMovement {
    /**
     * @brief 包含起始位置和结束位置的结构体
     *
     */
    struct Pair {
        Cartesian2 startPosition; ///< 起始位置
        Cartesian2 endPosition; ///< 结束位置
    };

    Cartesian2 startPosition; ///< 起始位置
    Cartesian2 endPosition; ///< 结束位置
    Pair distance; ///< 距离
    Pair angleAndHeight; ///< 角度和高度
    double prevAngle = 0.0; ///< 前一次的角度 (仅用于触摸状态下)
    bool valid = false; ///< 是否有效, 默认为false
    bool pinch = false; ///< 是否为触摸状态, 默认为false
};

/**
 * @brief 鼠标事件结构体
 *
 */
struct ScreenSpaceMouseEvent {
    int button; ///< 鼠标按下的键
    int modifier; ///< 键盘按下的键
    int deltaX = 0; ///< 鼠标中键在x方向的滚动量
    int deltaY = 0; ///< 鼠标中键在y方向的滚动量
    Cartesian2 position; ///< 位置1
    Cartesian2 position1; ///< 位置2
    Cartesian2 position2; ///< 位置3

    CameraMovement movement; ///< 相机移动信息
};

/**
 * @brief 定义共享指针
 *
 */
typedef QSharedPointer<ScreenSpaceMouseEvent> ScreenSpaceMouseEventPtr;

/**
 * @brief 窗口事件类型
 *
 */
struct ScreenSpaceEventType {
    /**
     * @brief 类型集合
     *
     */
    enum Type {
        LEFT_DOWN = 0,
        LEFT_UP = 1,
        LEFT_CLICK = 2,
        LEFT_DOUBLE_CLICK = 3,
        RIGHT_DOWN = 5,
        RIGHT_UP = 6,
        RIGHT_CLICK = 7,
        MIDDLE_DOWN = 10,
        MIDDLE_UP = 11,
        MIDDLE_CLICK = 12,
        MOUSE_MOVE = 15,
        WHEEL = 16,
        PINCH_START = 17,
        PINCH_END = 18,
        PINCH_MOVE = 19
    };
};

/**
 * @brief 相机事件类型
 *
 */
struct CameraEventType {
    /**
     * @brief 类型集合
     *
     */
    enum Type {
        LEFT_DRAG = 0,
        RIGHT_DRAG = 1,
        MIDDLE_DRAG = 2,
        WHEEL = 3,
        PINCH = 4,
        TypeCount = 5
    };
};

/**
 * @brief 判定指定的double值是否合法
 *
 * @param factor 指定的值, double类型
 * @return bool true: 合法, false: 不合法
 */
inline bool defined(double factor)
{
    return (factor - Math::EPSILON20) != 0.0;
}

/**
 * @brief 判定指定的Cartesian2值是否合法
 *
 * @param cartesian  指定的值, Cartesian2类型
 * @return bool true: 合法, false: 不合法
 */
inline bool defined(const Cartesian2 &cartesian)
{
    return (cartesian.x - Math::EPSILON20) != 0.0;
}

/**
 * @brief 判定指定的Cartesian3值是否合法
 *
 * @param cartesian 指定的值, Cartesian3类型
 * @return bool true: 合法, false: 不合法
 */
inline bool defined(const Cartesian3 &cartesian)
{
    return (cartesian.x - Math::EPSILON20) != 0.0;
}

/**
 * @brief 判定指定的Interval值是否合法
 *
 * @param interval 指定的值, Interval类型
 * @return bool true: 合法, false: 不合法
 */
inline bool defined(const Interval &interval)
{
    return (interval.start - Math::EPSILON20) != 0.0;
}

#endif // SCREENSPACEEVENTUTILS_H
