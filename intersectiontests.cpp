#include "intersectiontests.h"
#include "matrix4.h"
#include "screenspaceeventutils.h"
#include "quadraticrealpolynomial.h"
#include "quarticrealpolynomial.h"
#include "plane.h"
#include "ellipsoid.h"

IntersectionTests::IntersectionTests()
{
}

Interval IntersectionTests::rayEllipsoid(const Ray &ray, Ellipsoid *ellipsoid)
{
    Cartesian3 inverseRadii = ellipsoid->oneOverRadii();
    Cartesian3 q = inverseRadii * Cartesian3(ray.origin);
    Cartesian3 w = inverseRadii * Cartesian3(ray.direction);

    double q2 = q.magnitudeSquared();
    double qw = Cartesian3::dot(q, w);

    double difference, w2, product, discriminant, temp;

    if (q2 > 1.0) {
        // Outside ellipsoid.
        if (qw >= 0.0) {
            // Looking outward or tangent (0 intersections).
            return Interval(Math::EPSILON20, 0);
        }

        // qw < 0.0.
        double qw2 = qw * qw;
        difference = q2 - 1.0; // Positively valued.
        w2 = w.magnitudeSquared();
        product = w2 * difference;

        if (qw2 < product) {
            // Imaginary roots (0 intersections).
            return Interval(Math::EPSILON20, 0);
        } else if (qw2 > product) {
            // Distinct roots (2 intersections).
            discriminant = qw * qw - product;
            temp = -qw + sqrt(discriminant); // Avoid cancellation.
            double root0 = temp / w2;
            double root1 = difference / temp;

            return root0 < root1 ? Interval(root0, root1) : Interval(root1, root0);
        }
            // qw2 == product.  Repeated roots (2 intersections).
            double root = sqrt(difference / w2);
            return Interval(root, root);
        } else if (q2 < 1.0) {
            // Inside ellipsoid (2 intersections).
            difference = q2 - 1.0; // Negatively valued.
            w2 = w.magnitudeSquared();
            product = w2 * difference; // Negatively valued.

            discriminant = qw * qw - product;
            temp = -qw + sqrt(discriminant); // Positively valued.
            return Interval(0.0, temp / w2);
        }
        // q2 == 1.0. On ellipsoid.
        if (qw < 0.0) {
            // Looking inward.
            w2 = w.magnitudeSquared();
            return Interval(0.0, -qw / w2);
        }

        // qw >= 0.0.  Looking outward or tangent.
        return Interval(Math::EPSILON20, 0);
}

Cartesian3 IntersectionTests::rayPlane(const Ray &ray, Plane *plane)
{
    Cartesian3 origin = ray.origin;
    Cartesian3 direction = ray.direction;
    Cartesian3 normal = plane->normal;
    double denominator = Cartesian3::dot(normal, direction);

    if (abs(denominator) < Math::EPSILON15) {
        // Ray is parallel to plane.  The ray may be in the polygon's plane.
        return Cartesian3(Math::EPSILON20, 0, 0);
    }

    double t = (-plane->distance - Cartesian3::dot(normal, origin)) / denominator;

    if (t < 0) {
        return Cartesian3(Math::EPSILON20, 0, 0);
    }

    Cartesian3 result;
    result = direction * t;
    result += origin;
    return result;
}

Cartesian3 IntersectionTests::grazingAltitudeLocation(const Ray &ray, Ellipsoid *ellipsoid)
{
    Cartesian3 position = ray.origin;
    Cartesian3 direction = ray.direction;

    if (!position.isNull()) {
        Cartesian3 normal = ellipsoid->geodeticSurfaceNormal(position);
        if (Cartesian3::dot(direction, normal) >= 0.0) { // The location provided is the closest point in altitude
            return position;
        }
    }

    Interval interval = rayEllipsoid(ray, ellipsoid);
    bool intersects = defined(interval);

    // Compute the scaled direction vector.
    Cartesian3 f = ellipsoid->transformPositionToScaledSpace(direction);

    // Constructs a basis from the unit scaled direction vector. Construct its rotation and transpose.
    Cartesian3 firstAxis = f.normalized();
    Cartesian3 reference = mostOrthogonalAxis(f);
    Cartesian3 secondAxis = Cartesian3::cross(reference, firstAxis).normalized();
    Cartesian3 thirdAxis  = Cartesian3::cross(firstAxis, secondAxis).normalized();

//    var B = bScratch;
//    B[0] = firstAxis.x;
//    B[1] = firstAxis.y;
//    B[2] = firstAxis.z;
//    B[3] = secondAxis.x;
//    B[4] = secondAxis.y;
//    B[5] = secondAxis.z;
//    B[6] = thirdAxis.x;
//    B[7] = thirdAxis.y;
//    B[8] = thirdAxis.z;

//    var B_T = Matrix3.transpose(B, btScratch);
    Matrix3 B(firstAxis.x, secondAxis.x, thirdAxis.x,
              firstAxis.y, secondAxis.y, thirdAxis.y,
              firstAxis.z, secondAxis.z, thirdAxis.z);


    Matrix3 B_T(firstAxis.x, firstAxis.y, firstAxis.z,
                secondAxis.x, secondAxis.y, secondAxis.z,
                thirdAxis.x, thirdAxis.y, thirdAxis.z);
    // Get the scaling matrix and its inverse.
    Matrix3 D_I = Matrix4::fromScale(ellipsoid->radii()).toMatrix3();
    Matrix3 D = Matrix4::fromScale(ellipsoid->oneOverRadii()).toMatrix3();

//    var C = cScratch;
//    C[0] = 0.0;
//    C[1] = -direction.z;
//    C[2] = direction.y;
//    C[3] = direction.z;
//    C[4] = 0.0;
//    C[5] = -direction.x;
//    C[6] = -direction.y;
//    C[7] = direction.x;
//    C[8] = 0.0;

    Matrix3 C(0.0, direction.z, -direction.y,
              -direction.z, 0.0, direction.x,
              direction.y, -direction.x, 0.0);

    Matrix3 temp = B_T * D * C;
    Matrix3 A = temp * D_I * B;

    Vector3 posVec3(position.x, position.y, position.z);
    Vector3 b = temp * posVec3;

    // Solve for the solutions to the expression in standard form:
    QVector<Vector3> solutions = quadraticVectorExpression(A, -b, 0.0, 0.0, 1.0);

    Vector3 s;
    double altitude;
    int length = solutions.length();
    if (length > 0) {
        Cartesian3 closest;
        double maximumValue = DBL_MIN;

        for ( int i = 0; i < length; ++i) {
            s = D_I * B * solutions[i];
            Cartesian3 cs(s.x(), s.y(), s.z());
            Cartesian3 v = (cs - position).normalize();
            double dotProduct = Cartesian3::dot(v, direction);

            if (dotProduct > maximumValue) {
                maximumValue = dotProduct;
                closest = cs;
            }
        }

        Cartographic surfacePoint = ellipsoid->cartesianToCartographic(closest);
        maximumValue = Math::clamp(maximumValue, 0.0, 1.0);
        altitude = (closest - position).magnitude() * sqrt(1.0 - maximumValue * maximumValue);
        altitude = intersects ? -altitude : altitude;
        surfacePoint.height = altitude;
        return ellipsoid->cartographicToCartesian(surfacePoint);
    }

    return Cartesian3();
}

QVector<Vector3> IntersectionTests::quadraticVectorExpression(const Matrix3 &matrix, const Cartesian3 &cartesian, double c, double x, double w)
{
    double xSquared = x * x;
    double wSquared = w * w;

    double l2 = (matrix[4] - matrix[8]) * wSquared;
    double l1 = w * (x * addWithCancellationCheck(matrix[3], matrix[1], Math::EPSILON15) + cartesian.y);
    double l0 = (matrix[0] * xSquared + matrix[8] * wSquared) + x * cartesian.x + c;

    double r1 = wSquared * addWithCancellationCheck(matrix[7], matrix[5], Math::EPSILON15);
    double r0 = w * (x * addWithCancellationCheck(matrix[6], matrix[2], Math::EPSILON15) + cartesian.z);

    QVector<double> cosines;
    QVector<Vector3> solutions;
    if (r0 == 0.0 && r1 == 0.0) {
        cosines = QuadraticRealPolynomial::computeRealRoots(l2, l1, l0);
        int cosinesLength = cosines.length();
        if (cosinesLength == 0) {
            return solutions;
        }

        double cosine0 = cosines[0];
        double sine0 = sqrt(std::max(1.0 - cosine0 * cosine0, 0.0));
        solutions.append(Vector3(x, w * cosine0, w * -sine0));
        solutions.append(Vector3(x, w * cosine0, w * sine0));

        if (cosinesLength == 2) {
            double cosine1 = cosines[1];
            double sine1 = sqrt(std::max(1.0 - cosine1 * cosine1, 0.0));
            solutions.append(Vector3(x, w * cosine1, w * -sine1));
            solutions.append(Vector3(x, w * cosine1, w * sine1));
        }

        return solutions;
    }

    double r0Squared = r0 * r0;
    double r1Squared = r1 * r1;
    double l2Squared = l2 * l2;
    double r0r1 = r0 * r1;

    double c4 = l2Squared + r1Squared;
    double c3 = 2.0 * (l1 * l2 + r0r1);
    double c2 = 2.0 * l0 * l2 + l1 * l1 - r1Squared + r0Squared;
    double c1 = 2.0 * (l0 * l1 - r0r1);
    double c0 = l0 * l0 - r0Squared;

    if (c4 == 0.0 && c3 == 0.0 && c2 == 0.0 && c1 == 0.0) {
        return solutions;
    }

    cosines = QuarticRealPolynomial::computeRealRoots(c4, c3, c2, c1, c0);
    int length = cosines.length();
    if (length == 0) {
        return solutions;
    }

    for ( int i = 0; i < length; ++i) {
        double cosine = cosines[i];
        double cosineSquared = cosine * cosine;
        double sineSquared = std::max(1.0 - cosineSquared, 0.0);
        double sine = sqrt(sineSquared);

        //var left = l2 * cosineSquared + l1 * cosine + l0;
        double left;
        if (sign(l2) == sign(l0)) {
            left = addWithCancellationCheck(l2 * cosineSquared + l0, l1 * cosine, Math::EPSILON12);
        } else if (sign(l0) == sign(l1 * cosine)) {
            left = addWithCancellationCheck(l2 * cosineSquared, l1 * cosine + l0, Math::EPSILON12);
        } else {
            left = addWithCancellationCheck(l2 * cosineSquared + l1 * cosine, l0, Math::EPSILON12);
        }

        double right = addWithCancellationCheck(r1 * cosine, r0, Math::EPSILON15);
        double product = left * right;

        if (product < 0.0) {
            solutions.append(Vector3(x, w * cosine, w * sine));
        } else if (product > 0.0) {
            solutions.append(Vector3(x, w * cosine, w * -sine));
        } else if (sine != 0.0) {
            solutions.append(Vector3(x, w * cosine, w * -sine));
            solutions.append(Vector3(x, w * cosine, w * sine));
            ++i;
        } else {
            solutions.append(Vector3(x, w * cosine, w * sine));
        }
    }

    return solutions;
}

Cartesian3 IntersectionTests::mostOrthogonalAxis(const Cartesian3 &cartesian)
{
    Cartesian3 result;
    Cartesian3 f = cartesian.normalized();
    f = f.abs();

    if (f.x <= f.y) {
        if (f.x <= f.z) {
            result = Cartesian3::UNIT_X;
        } else {
            result = Cartesian3::UNIT_Z;
        }
    } else if (f.y <= f.z) {
        result = Cartesian3::UNIT_Y;
    } else {
        result = Cartesian3::UNIT_Z;
    }

    return result;
}

double IntersectionTests::addWithCancellationCheck(double left, double right, double tolerance)
{
    double difference = left + right;
    if (sign(left) != sign(right) &&
            abs(difference / std::max(abs(left), abs(right))) < tolerance) {
        return 0.0;
    }

    return difference;
}

int IntersectionTests::sign(double value)
{
    if(value == 0.0)
        return 0;
    return value > 0 ? 1 : -1;
}

