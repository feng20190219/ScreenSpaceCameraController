#ifndef INTERSECTIONTESTS_H
#define INTERSECTIONTESTS_H

#include "matrix3.h"
#include "ray.h"
#include "interval.h"

class Plane;
class Ellipsoid;

/**
 * @brief 用于计算射, 平面, 三角形和椭圆体等几何体之间的交点的函数
 *
 */
class IntersectionTests
{
public:
    IntersectionTests();

    /**
     * @brief 计算射线与椭球体的交点 (静态函数)
     *
     * @param ray 射线
     * @param ellipsoid 椭球
     * @return Interval 交点
     */
    static Interval rayEllipsoid(const Ray &ray, Ellipsoid *ellipsoid);

    /**
     * @brief 计算射线与平面的交点 (静态函数)
     *
     * @param ray 射线
     * @param plane 平面
     * @return Cartesian3 返回Cartesian3类型
     */
    static Cartesian3 rayPlane(const Ray &ray, Plane *plane);

    /**
     * @brief 提供沿射线最靠近椭球的点 (静态函数)
     *
     * @param ray 射线
     * @param ellipsoid 椭球
     * @return Cartesian3 返回Cartesian3类型
     */
    static Cartesian3 grazingAltitudeLocation(const Ray &ray, Ellipsoid *ellipsoid);

private:
    static QVector<Vector3> quadraticVectorExpression(const Matrix3 &matrix,
                                          const Cartesian3 &cartesian,
                                          double c,
                                          double x,
                                          double w);
    static Cartesian3 mostOrthogonalAxis(const Cartesian3 &cartesian);
    static double addWithCancellationCheck(double left, double right, double tolerance);
    static int sign(double value);
};

#endif // INTERSECTIONTESTS_H
