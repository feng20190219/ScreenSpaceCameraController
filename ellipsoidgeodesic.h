#ifndef ELLIPSOIDGEODESIC_H
#define ELLIPSOIDGEODESIC_H

#include "cartographic.h"

class Ellipsoid;

/**
 * @brief 初始化椭球体上测地线, 连接所提供的两个平面点
 *
 */
class EllipsoidGeodesic
{
public:
    /**
     * @brief 默认构造
     *
     */
    EllipsoidGeodesic();

    /**
     * @brief 设置测地线的起点和终点
     *
     * @param start 测地线的起点
     * @param end 测地线的终点
     */
    void setEndPoints(const Cartographic &start, const Cartographic &end);

    /**
     * @brief 提供测地线上指示部分的点位置
     *
     * @param fraction 初始点和最终点之间的距离
     * @return Cartographic 测地线上点的位置
     */
    Cartographic interpolateUsingFraction(double fraction);

    /**
     * @brief 提供沿测地线指示距离处的点的位置
     *
     * @param distance 沿测地线从初始点到指示点的距离
     * @return Cartographic 测地线上点的位置
     */
    Cartographic interpolateUsingSurfaceDistance(double distance);

private:
    void computeProperties(const Cartographic &start, const Cartographic &end);
    void setConstants();
    void vincentyInverseFormula(double major, double minor, double firstLongitude, double firstLatitude, double secondLongitude, double secondLatitude);
    double computeDeltaLambda(double f, double sineAlpha, double cosineSquaredAlpha, double sigma, double sineSigma, double cosineSigma, double cosineTwiceSigmaMidpoint);
    double computeC(double f, double cosineSquaredAlpha);

    Ellipsoid *_ellipsoid;
    Cartographic _start;
    Cartographic _end;

    double _startHeading;
    double _endHeading;
    double _distance;
    double _uSquared;

    struct Constants {
        double a;
        double b;
        double f;
        double cosineHeading;
        double sineHeading;
        double tanU;
        double cosineU;
        double sineU;
        double sigma;
        double sineAlpha;
        double sineSquaredAlpha;
        double cosineSquaredAlpha;
        double cosineAlpha;
        double u2Over4;
        double u4Over16;
        double u6Over64;
        double u8Over256;
        double a0;
        double a1;
        double a2;
        double a3;
        double distanceRatio;
    };

    Constants _constants;
};

#endif // ELLIPSOIDGEODESIC_H
