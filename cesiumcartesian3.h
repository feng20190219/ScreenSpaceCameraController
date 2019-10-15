#ifndef CESIUMCARTESIAN3_H
#define CESIUMCARTESIAN3_H

#include "cartesian3.h"

/**
 * @brief 此类提供对Cartesian3类型操作的相关函数
 *
 */
class CesiumCartesian3
{
public:
    CesiumCartesian3();

    /**
     * @brief 分别比较两个Cartesian3类型的x, y, z值的数量差是否在给定的范围之内, 只有同时满足才返回true, 否则返回false (静态函数)
     *
     * @param left 第一个参数
     * @param right 第二个参数
     * @param epsilon 给定的范围
     * @return bool true: 满足, false: 不满足
     */
    static bool equalsEpsilon(const Cartesian3 &left, const Cartesian3 &right, double epsilon);

    /**
     * @brief 返回两个Cartesian3向量的夹角大小, 用弧度表示 (静态函数)
     *
     * @param left 第一个参数
     * @param right 第二个参数
     * @return double 返回夹角大小 (弧度)
     */
    static double angleBetween(const Cartesian3 &left, const Cartesian3 &right);

    /**
     * @brief 返回一个Cartesian3向量的垂直向量 (静态函数)
     *
     * @param cartesian 参照向量
     * @return Cartesian3 参照向量的垂直向量
     */
    static Cartesian3 mostOrthogonalAxis(const Cartesian3 &cartesian) ;

    /**
     * @brief 根据给定的longitude(弧度), latitude(弧度), height返回一个Cartesian3类型的世界坐标 (静态函数)
     *
     * @param longitude 经度, 用弧度表示
     * @param latitude 纬度, 用弧度表示
     * @param height 高度
     * @return Cartesian3 返回一个Cartesian3类型的世界坐标
     */
    static Cartesian3 fromRadians(double longitude, double latitude, double height);
};

#endif // CESIUMCARTESIAN3_H
