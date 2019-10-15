#ifndef CESIUMMATH_H
#define CESIUMMATH_H

#include "cartesian3.h"
#include "quaternion.h"

/**
 * @brief  此类提供部分算术操作的相关函数
 *
 */
class CesiumMath
{
public:
    CesiumMath();

    /**
     * @brief 比较两个数的数量差是否在给定的范围之内, 满足返回true, 否则返回false (静态函数)
     *
     * @param left 第一个参数
     * @param right 第二个参数
     * @param relativeEpsilon 相对范围
     * @param absoluteEpsilon 绝对范围
     * @return bool true: 满足, false: 不满足
     */
    static bool equalsEpsilon(double left = 0, double right = 0, double relativeEpsilon = 0, double absoluteEpsilon = 0);

    /**
     * @brief 根据提供的一个角度(0到2π)生成另一个角度, 用弧度表示 (静态函数)
     *
     * @param angle 提供的角度 (弧度)
     * @return double 返回的角度 (弧度)
     */
    static double zeroToTwoPi(double angle);

    /**
     * @brief 适用于负数的模运算 (静态函数)
     *
     * @param m 被除数
     * @param n 除数
     * @return double 余数
     */
    static double mod(double m, double n);

    /**
     * @brief 计算math.acos(value), 但首先将值限制到范围[-1.0, 1.0], 这样函数就永远不会返回NaN (静态函数)
     *
     * @param value 需要计算math.acos的数
     * @return double 返回math.acos(value), 在范围[-1.0, 1.0]之间
     */
    static double acosClamped(double value);

    /**
     * @brief 根据提供的一个角度(-π到π)生成另一个角度, 用弧度表示 (静态函数)
     *
     * @param angle 提供的角度 (弧度)
     * @return double 返回的角度 (弧度)
     */
    static double negativePiToPi(double angle);

    /**
     * @brief 计算两个值的线性插值 (静态函数)
     *
     * @param p 线性插值的起始值
     * @param q 线性插值的结束值
     * @param time 线性插值需要的时间
     * @return double 线性插值的结果
     */
    static double lerp(double p, double q, double time);

    /**
     * @brief 根据给定的heading(弧度), pitch(弧度), roll(弧度)生成一个四元数 (静态函数)
     *
     * @param heading  heading(弧度)
     * @param pitch pitch(弧度)
     * @param roll roll(弧度)
     * @return Quaternion 返回四元数
     */
    static Quaternion fromHeadingPitchRoll(double heading, double pitch, double roll);
};

#endif // CESIUMMATH_H
