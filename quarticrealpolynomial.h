#ifndef QUARTICREALPOLYNOMIAL_H
#define QUARTICREALPOLYNOMIAL_H

#include <QVector>

/**
 * @brief 定义只有实数系数的一个变量的四阶多项式函数的函数
 *
 */
class QuarticRealPolynomial
{
public:
    QuarticRealPolynomial();

    /**
     * @brief 用提供的系数提供四次多项式的实数
     *
     * @param a 第四阶单项式的系数
     * @param b 第三阶单项式的系数
     * @param c 第二阶单项式的系数
     * @param d 第一阶单项式的系数
     * @param e 第零阶单项式的系数
     * @return QVector<double> 返回符合条件的实数集合
     */
    static QVector<double> computeRealRoots(double a, double b, double c, double d, double e);

private:
    static QVector<double> original(double a3, double a2, double a1, double a0);
    static QVector<double> neumark(double a3, double a2, double a1, double a0);
    static int sign(double value);
};

#endif // QUARTICREALPOLYNOMIAL_H
