#ifndef CUBICREALPOLYNOMIAL_H
#define CUBICREALPOLYNOMIAL_H

#include <QVector>

/**
 * @brief 定义仅具有实数系数的一个变量的三阶多项式函数的函数
 *
 */
class CubicRealPolynomial
{
public:
    CubicRealPolynomial();

    /**
     * @brief 提供具有所提供系数的三次多项式的实数 (静态函数)
     *
     * @param a 第三阶单项式系数
     * @param b 第二阶单项式系数
     * @param c 第一阶单项式系数
     * @param d 第零阶单项式系数
     * @return QVector<double> 返回符合条件的实数集合
     */
    static QVector<double> computeRealRoots(double a, double b, double c, double d);

private:
    static QVector<double> computeRealRootsPrivate(double a, double b, double c, double d);
};

#endif // CUBICREALPOLYNOMIAL_H
