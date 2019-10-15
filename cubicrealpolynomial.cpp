#include "cubicrealpolynomial.h"
#include "quadraticrealpolynomial.h"

CubicRealPolynomial::CubicRealPolynomial()
{
}

QVector<double> CubicRealPolynomial::computeRealRoots(double a, double b, double c, double d)
{
    double ratio;
    if (a == 0.0) {
        // Quadratic function: b * x^2 + c * x + d = 0.
        return QuadraticRealPolynomial::computeRealRoots(b, c, d);
    } else if (b == 0.0) {
        if (c == 0.0) {
            if (d == 0.0) {
                // 3rd order monomial: a * x^3 = 0.
                QVector<double> result;
                result.append(0);
                result.append(0);
                result.append(0);
                return result;
            }

            // a * x^3 + d = 0
            ratio = -d / a;
            double root = (ratio < 0.0) ? -pow(-ratio, 1.0 / 3.0) : pow(ratio, 1.0 / 3.0);
            QVector<double> result;
            result.append(root);
            result.append(root);
            result.append(root);
            return result;
        } else if (d == 0.0) {
            // x * (a * x^2 + c) = 0.
            QVector<double> roots = QuadraticRealPolynomial::computeRealRoots(a, 0, c);

            // Return the roots in ascending order.
            if (roots.length() == 0) {

                QVector<double> result;
                result.append(0);
                return roots;
            }
            QVector<double> result;
            result.append(roots[0]);
            result.append(0);
            result.append(roots[1]);
            return result;
        }

        // Deflated cubic polynomial: a * x^3 + c * x + d= 0.
        return computeRealRootsPrivate(a, 0, c, d);
    } else if (c == 0.0) {
        if (d == 0.0) {
            // x^2 * (a * x + b) = 0.
            ratio = -b / a;
            if (ratio < 0.0) {
                QVector<double> result;
                result.append(ratio);
                result.append(0);
                result.append(0);
                return result;
            }
            QVector<double> result;
            result.append(0);
            result.append(0);
            result.append(ratio);
            return result;
        }
        // a * x^3 + b * x^2 + d = 0.
        return computeRealRootsPrivate(a, b, 0, d);
    } else if (d == 0.0) {
        // x * (a * x^2 + b * x + c) = 0
        QVector<double> roots = QuadraticRealPolynomial::computeRealRoots(a, b, c);

        // Return the roots in ascending order.
        if (roots.length() == 0) {
            QVector<double> result;
            result.append(0);
            return result;
        } else if (roots[1] <= 0.0) {
            QVector<double> result;
            result.append(roots[0]);
            result.append(roots[1]);
            result.append(0);
            return result;
        } else if (roots[0] >= 0.0) {
            QVector<double> result;
            result.append(0);
            result.append(roots[0]);
            result.append(roots[1]);
            return result;
        }

        QVector<double> result;
        result.append(roots[0]);
        result.append(0);
        result.append(roots[1]);
        return result;
    }

    return computeRealRootsPrivate(a, b, c, d);
}

QVector<double> CubicRealPolynomial::computeRealRootsPrivate(double a, double b, double c, double d)
{
    double A = a;
    double B = b / 3.0;
    double C = c / 3.0;
    double D = d;

    double AC = A * C;
    double BD = B * D;
    double B2 = B * B;
    double C2 = C * C;
    double delta1 = A * C - B2;
    double delta2 = A * D - B * C;
    double delta3 = B * D - C2;

    double discriminant = 4.0 * delta1 * delta3 - delta2 * delta2;
    double temp;
    double temp1;

    if (discriminant < 0.0) {
        double ABar;
        double CBar;
        double DBar;

        if (B2 * BD >= AC * C2) {
            ABar = A;
            CBar = delta1;
            DBar = -2.0 * B * delta1 + A * delta2;
        } else {
            ABar = D;
            CBar = delta3;
            DBar = -D * delta2 + 2.0 * C * delta3;
        }

        double s = (DBar < 0.0) ? -1.0 : 1.0; // This is not Math.Sign()!
        double temp0 = -s * abs(ABar) * sqrt(-discriminant);
        temp1 = -DBar + temp0;

        double x = temp1 / 2.0;
        double p = x < 0.0 ? -pow(-x, 1.0 / 3.0) : pow(x, 1.0 / 3.0);
        double q = (temp1 == temp0) ? -p : -CBar / p;

        temp = (CBar <= 0.0) ? p + q : -DBar / (p * p + q * q + CBar);

        if (B2 * BD >= AC * C2) {
            QVector<double> result;
            result.append((temp - B) / A);
            return result;
        }

        QVector<double> result;
        result.append(-D / (temp + C));
        return result;
    }

    double CBarA = delta1;
    double DBarA = -2.0 * B * delta1 + A * delta2;

    double CBarD = delta3;
    double DBarD = -D * delta2 + 2.0 * C * delta3;

    double squareRootOfDiscriminant = sqrt(discriminant);
    double halfSquareRootOf3 = sqrt(3.0) / 2.0;

    double theta = abs(atan2(A * squareRootOfDiscriminant, -DBarA) / 3.0);
    temp = 2.0 * sqrt(-CBarA);
    double cosine = cos(theta);
    temp1 = temp * cosine;
    double temp3 = temp * (-cosine / 2.0 - halfSquareRootOf3 * sin(theta));

    double numeratorLarge = (temp1 + temp3 > 2.0 * B) ? temp1 - B : temp3 - B;
    double denominatorLarge = A;

    double root1 = numeratorLarge / denominatorLarge;

    theta = abs(atan2(D * squareRootOfDiscriminant, -DBarD) / 3.0);
    temp = 2.0 * sqrt(-CBarD);
    cosine = cos(theta);
    temp1 = temp * cosine;
    temp3 = temp * (-cosine / 2.0 - halfSquareRootOf3 * sin(theta));

    double numeratorSmall = -D;
    double denominatorSmall = (temp1 + temp3 < 2.0 * C) ? temp1 + C : temp3 + C;

    double root3 = numeratorSmall / denominatorSmall;

    double E = denominatorLarge * denominatorSmall;
    double F = -numeratorLarge * denominatorSmall - denominatorLarge * numeratorSmall;
    double G = numeratorLarge * numeratorSmall;

    double root2 = (C * F - B * G) / (-B * F + C * E);

    if (root1 <= root2) {
        if (root1 <= root3) {
            if (root2 <= root3) {
                QVector<double> result;
                result.append(root1);
                result.append(root2);
                result.append(root3);
                return result;
            }
            QVector<double> result;
            result.append(root1);
            result.append(root3);
            result.append(root2);
            return result;
        }
        QVector<double> result;
        result.append(root3);
        result.append(root1);
        result.append(root2);
        return result;
    }
    if (root1 <= root3) {
        QVector<double> result;
        result.append(root2);
        result.append(root1);
        result.append(root3);
        return result;
    }
    if (root2 <= root3) {
        QVector<double> result;
        result.append(root2);
        result.append(root3);
        result.append(root1);
        return result;
    }
    QVector<double> result;
    result.append(root3);
    result.append(root2);
    result.append(root1);
    return result;
}
