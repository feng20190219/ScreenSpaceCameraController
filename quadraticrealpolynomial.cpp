#include "quadraticrealpolynomial.h"
#include "limath.h"

QuadraticRealPolynomial::QuadraticRealPolynomial()
{
}

QVector<double> QuadraticRealPolynomial::computeRealRoots(double a, double b, double c)
{
    double ratio;
    if (a == 0.0) {
        if (b == 0.0) {
            // Constant function: c = 0.
            return QVector<double>();
        }

        // Linear function: b * x + c = 0.
        QVector<double> result;
        result.append(-c / b);
        return result;
    } else if (b == 0.0) {
        if (c == 0.0) {
            // 2nd order monomial: a * x^2 = 0.
            QVector<double> result;
            result.append(0);
            result.append(0);
            return result;
        }

        double cMagnitude = abs(c);
        double aMagnitude = abs(a);

        if ((cMagnitude < aMagnitude) && (cMagnitude / aMagnitude < Math::EPSILON14)) { // c ~= 0.0.
            // 2nd order monomial: a * x^2 = 0.
            QVector<double> result;
            result.append(0);
            result.append(0);
            return result;
        } else if ((cMagnitude > aMagnitude) && (aMagnitude / cMagnitude < Math::EPSILON14)) { // a ~= 0.0.
            // Constant function: c = 0.
            return QVector<double>();
        }

        // a * x^2 + c = 0
        ratio = -c / a;

        if (ratio < 0.0) {
            // Both roots are complex.
            return QVector<double>();
        }

        // Both roots are real.
        double root = sqrt(ratio);

        QVector<double> result;
        result.append(-root);
        result.append(root);
        return result;
    } else if (c == 0.0) {
        // a * x^2 + b * x = 0
        ratio = -b / a;
        if (ratio < 0.0) {
            QVector<double> result;
            result.append(ratio);
            result.append(0);
            return result;
        }

        QVector<double> result;
        result.append(0);
        result.append(ratio);
        return result;
    }

    // a * x^2 + b * x + c = 0
    double b2 = b * b;
    double four_ac = 4.0 * a * c;
    double radicand = addWithCancellationCheck(b2, -four_ac, Math::EPSILON14);

    if (radicand < 0.0) {
        // Both roots are complex.
        return QVector<double>();
    }

    double q = -0.5 * addWithCancellationCheck(b, sign(b) * sqrt(radicand), Math::EPSILON14);
    if (b > 0.0) {
        QVector<double> result;
        result.append(q / a);
        result.append(c / q);
        return result;
    }

    QVector<double> result;
    result.append(c / q);
    result.append(q / a);
    return result;
}

double QuadraticRealPolynomial::addWithCancellationCheck(double left, double right, double tolerance)
{
    double difference = left + right;
    if (sign(left) != sign(right) &&
            abs(difference / std::max(abs(left), abs(right))) < tolerance) {
        return 0.0;
    }

    return difference;
}

int QuadraticRealPolynomial::sign(double value)
{
    if(value == 0)
        return 0;
    return value > 0 ? 1 : -1;
}
