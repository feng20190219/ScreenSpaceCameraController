#ifndef QUADRATICREALPOLYNOMIAL_H
#define QUADRATICREALPOLYNOMIAL_H

#include <QVector>

/**
 * @brief 定义仅具有实数系数的一个变量的二阶多项式函数的函数
 *
 */
class QuadraticRealPolynomial
{
public:
    QuadraticRealPolynomial();

    /**
     * @brief 用提供的系数提供二次多项式的实数 (静态函数)
     *
     * @param a 第二阶单项式系数
     * @param b 第一阶单项式系数
     * @param c 第零阶单项式系数
     * @return QVector<double> 返回符合条件的实数集合
     */
    static QVector<double> computeRealRoots(double a, double b, double c);

private:
    static double addWithCancellationCheck(double left, double right, double tolerance);
    static int sign(double value);
};

#endif // QUADRATICREALPOLYNOMIAL_H
