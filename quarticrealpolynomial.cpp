#include "quarticrealpolynomial.h"
#include "limath.h"
#include "cubicrealpolynomial.h"
#include "quadraticrealpolynomial.h"

QuarticRealPolynomial::QuarticRealPolynomial()
{
}

QVector<double> QuarticRealPolynomial::computeRealRoots(double a, double b, double c, double d, double e)
{
    if (abs(a) < Math::EPSILON15) {
        return CubicRealPolynomial::computeRealRoots(b, c, d, e);
    }
    double a3 = b / a;
    double a2 = c / a;
    double a1 = d / a;
    double a0 = e / a;

    double k = (a3 < 0.0) ? 1 : 0;
    k += (a2 < 0.0) ? k + 1 : k;
    k += (a1 < 0.0) ? k + 1 : k;
    k += (a0 < 0.0) ? k + 1 : k;

    if(k == 0)
        return original(a3, a2, a1, a0);
    else if(k == 1)
        return neumark(a3, a2, a1, a0);
    else if(k == 2)
        return neumark(a3, a2, a1, a0);
    else if(k == 3)
        return original(a3, a2, a1, a0);
    else if(k == 4)
        return original(a3, a2, a1, a0);
    else if(k == 5)
        return neumark(a3, a2, a1, a0);
    else if(k == 6)
        return original(a3, a2, a1, a0);
    else if(k == 7)
        return original(a3, a2, a1, a0);
    else if(k == 8)
        return neumark(a3, a2, a1, a0);
    else if(k == 9)
        return original(a3, a2, a1, a0);
    else if(k == 10)
        return original(a3, a2, a1, a0);
    else if(k == 11)
        return neumark(a3, a2, a1, a0);
    else if(k == 12)
        return original(a3, a2, a1, a0);
    else if(k == 13)
        return original(a3, a2, a1, a0);
    else if(k == 14)
        return original(a3, a2, a1, a0);
    else if(k == 15)
        return original(a3, a2, a1, a0);
    else
        return QVector<double>();
}

QVector<double> QuarticRealPolynomial::original(double a3, double a2, double a1, double a0)
{
    double a3Squared = a3 * a3;

    double p = a2 - 3.0 * a3Squared / 8.0;
    double q = a1 - a2 * a3 / 2.0 + a3Squared * a3 / 8.0;
    double r = a0 - a1 * a3 / 4.0 + a2 * a3Squared / 16.0 - 3.0 * a3Squared * a3Squared / 256.0;

    // Find the roots of the cubic equations:  h^6 + 2 p h^4 + (p^2 - 4 r) h^2 - q^2 = 0.
    QVector<double> cubicRoots = CubicRealPolynomial::computeRealRoots(1.0, 2.0 * p, p * p - 4.0 * r, -q * q);

    if (cubicRoots.length() > 0) {
        double temp = -a3 / 4.0;

        // Use the largest positive root.
        double hSquared = cubicRoots.last();

        if (abs(hSquared) < Math::EPSILON14) {
            // y^4 + p y^2 + r = 0.
            QVector<double> roots = QuadraticRealPolynomial::computeRealRoots(1.0, p, r);

            if (roots.length() == 2) {
                double root0 = roots[0];
                double root1 = roots[1];

                double y;
                if (root0 >= 0.0 && root1 >= 0.0) {
                    double y0 = sqrt(root0);
                    double y1 = sqrt(root1);

                    QVector<double> result;
                    result.append(temp - y1);
                    result.append(temp - y0);
                    result.append(temp + y0);
                    result.append(temp + y1);
                    return result;
                } else if (root0 >= 0.0 && root1 < 0.0) {
                    y = sqrt(root0);
                    QVector<double> result;
                    result.append(temp - y);
                    result.append(temp + y);
                    return result;
                } else if (root0 < 0.0 && root1 >= 0.0) {
                    y = sqrt(root1);
                    QVector<double> result;
                    result.append(temp - y);
                    result.append(temp + y);
                    return result;
                }
            }
            return QVector<double>();
        } else if (hSquared > 0.0) {
            double h = sqrt(hSquared);

            double m = (p + hSquared - q / h) / 2.0;
            double n = (p + hSquared + q / h) / 2.0;

            // Now solve the two quadratic factors:  (y^2 + h y + m)(y^2 - h y + n);
            QVector<double> roots1 = QuadraticRealPolynomial::computeRealRoots(1.0, h, m);
            QVector<double> roots2 = QuadraticRealPolynomial::computeRealRoots(1.0, -h, n);

            if (roots1.length() != 0) {
                roots1[0] += temp;
                roots1[1] += temp;

                if (roots2.length() != 0) {
                    roots2[0] += temp;
                    roots2[1] += temp;

                    if (roots1[1] <= roots2[0]) {
                        QVector<double> result;
                        result.append(roots1[0]);
                        result.append(roots1[1]);
                        result.append(roots2[0]);
                        result.append(roots2[1]);
                        return result;
                    } else if (roots2[1] <= roots1[0]) {
                        QVector<double> result;
                        result.append(roots2[0]);
                        result.append(roots2[1]);
                        result.append(roots1[0]);
                        result.append(roots1[1]);
                        return result;
                    } else if (roots1[0] >= roots2[0] && roots1[1] <= roots2[1]) {
                        QVector<double> result;
                        result.append(roots2[0]);
                        result.append(roots1[0]);
                        result.append(roots1[1]);
                        result.append(roots2[1]);
                        return result;
                    } else if (roots2[0] >= roots1[0] && roots2[1] <= roots1[1]) {
                        QVector<double> result;
                        result.append(roots1[0]);
                        result.append(roots2[0]);
                        result.append(roots2[1]);
                        result.append(roots1[1]);
                        return result;
                    } else if (roots1[0] > roots2[0] && roots1[0] < roots2[1]) {
                        QVector<double> result;
                        result.append(roots2[0]);
                        result.append(roots1[0]);
                        result.append(roots2[1]);
                        result.append(roots1[1]);
                        return result;
                    }
                    QVector<double> result;
                    result.append(roots1[0]);
                    result.append(roots2[0]);
                    result.append(roots1[1]);
                    result.append(roots2[1]);
                    return result;
                }
                return roots1;
            }

            if (roots2.length() != 0) {
                roots2[0] += temp;
                roots2[1] += temp;

                return roots2;
            }
            return QVector<double>();
        }
    }
    return QVector<double>();
}

QVector<double> QuarticRealPolynomial::neumark(double a3, double a2, double a1, double a0)
{
    double a1Squared = a1 * a1;
    double a2Squared = a2 * a2;
    double a3Squared = a3 * a3;

    double p = -2.0 * a2;
    double q = a1 * a3 + a2Squared - 4.0 * a0;
    double r = a3Squared * a0 - a1 * a2 * a3 + a1Squared;

    QVector<double> cubicRoots = CubicRealPolynomial::computeRealRoots(1.0, p, q, r);

    if (cubicRoots.length() > 0) {
        // Use the most positive root
        double y = cubicRoots[0];

        double temp = (a2 - y);
        double tempSquared = temp * temp;

        double g1 = a3 / 2.0;
        double h1 = temp / 2.0;

        double m = tempSquared - 4.0 * a0;
        double mError = tempSquared + 4.0 * abs(a0);

        double n = a3Squared - 4.0 * y;
        double nError = a3Squared + 4.0 * abs(y);

        double g2;
        double h2;

        if (y < 0.0 || (m * nError < n * mError)) {
            double squareRootOfN = sqrt(n);
            g2 = squareRootOfN / 2.0;
            h2 = squareRootOfN == 0.0 ? 0.0 : (a3 * h1 - a1) / squareRootOfN;
        } else {
            double squareRootOfM = sqrt(m);
            g2 = squareRootOfM == 0.0 ? 0.0 : (a3 * h1 - a1) / squareRootOfM;
            h2 = squareRootOfM / 2.0;
        }

        double G;
        double g;
        if (g1 == 0.0 && g2 == 0.0) {
            G = 0.0;
            g = 0.0;
        } else if (sign(g1) == sign(g2)) {
            G = g1 + g2;
            g = y / G;
        } else {
            g = g1 - g2;
            G = y / g;
        }

        double H;
        double h;
        if (h1 == 0.0 && h2 == 0.0) {
            H = 0.0;
            h = 0.0;
        } else if (sign(h1) == sign(h2)) {
            H = h1 + h2;
            h = a0 / H;
        } else {
            h = h1 - h2;
            H = a0 / h;
        }

        // Now solve the two quadratic factors:  (y^2 + G y + H)(y^2 + g y + h);
        QVector<double> roots1 = QuadraticRealPolynomial::computeRealRoots(1.0, G, H);
        QVector<double> roots2 = QuadraticRealPolynomial::computeRealRoots(1.0, g, h);

        if (roots1.length() != 0) {
            if (roots2.length() != 0) {
                if (roots1[1] <= roots2[0]) {
                    QVector<double> result;
                    result.append(roots1[0]);
                    result.append(roots1[1]);
                    result.append(roots2[0]);
                    result.append(roots2[1]);
                    return result;
                } else if (roots2[1] <= roots1[0]) {
                    QVector<double> result;
                    result.append(roots2[0]);
                    result.append(roots2[1]);
                    result.append(roots1[0]);
                    result.append(roots1[1]);
                    return result;
                } else if (roots1[0] >= roots2[0] && roots1[1] <= roots2[1]) {
                    QVector<double> result;
                    result.append(roots2[0]);
                    result.append(roots1[0]);
                    result.append(roots1[1]);
                    result.append(roots2[1]);
                    return result;
                } else if (roots2[0] >= roots1[0] && roots2[1] <= roots1[1]) {
                    QVector<double> result;
                    result.append(roots1[0]);
                    result.append(roots2[0]);
                    result.append(roots2[1]);
                    result.append(roots1[1]);
                    return result;
                } else if (roots1[0] > roots2[0] && roots1[0] < roots2[1]) {
                    QVector<double> result;
                    result.append(roots2[0]);
                    result.append(roots1[0]);
                    result.append(roots2[1]);
                    result.append(roots1[1]);
                    return result;
                }
                QVector<double> result;
                result.append(roots1[0]);
                result.append(roots2[0]);
                result.append(roots1[1]);
                result.append(roots2[1]);
                return result;
            }
            return roots1;
        }
        if (roots2.length() != 0) {
            return roots2;
        }
    }
    return QVector<double>();
}

int QuarticRealPolynomial::sign(double value)
{
    if(value == 0)
        return 0;
    return value > 0 ? 1 : -1;
}
