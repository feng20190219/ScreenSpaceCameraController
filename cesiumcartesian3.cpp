#include "cesiumcartesian3.h"
#include "cesiummath.h"

CesiumCartesian3::CesiumCartesian3()
{   
}

bool CesiumCartesian3::equalsEpsilon(const Cartesian3 &left, const Cartesian3 &right, double epsilon)
{
    return (left == right) ||
            (CesiumMath::equalsEpsilon(left.x, right.x, epsilon) &&
             CesiumMath::equalsEpsilon(left.y, right.y, epsilon) &&
             CesiumMath::equalsEpsilon(left.z, right.z, epsilon));
}

double CesiumCartesian3::angleBetween(const Cartesian3 &left, const Cartesian3 &right)
{
    Cartesian3 angleBetweenScratch = left.normalized();
    Cartesian3 angleBetweenScratch2 = right.normalized();
    double cosine = Cartesian3::dot(angleBetweenScratch, angleBetweenScratch2);
    double sine = Cartesian3::cross(angleBetweenScratch, angleBetweenScratch2).magnitude();
    return atan2(sine, cosine);
}

Cartesian3 CesiumCartesian3::mostOrthogonalAxis(const Cartesian3 &cartesian)
{
    Cartesian3 result;
    Cartesian3 f = cartesian.normalized();
    f.abs();

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

Cartesian3 CesiumCartesian3::fromRadians(double longitude, double latitude, double height)
{
    Cartesian3 scratchN, scratchK;
    Cartesian3 radiiSquared(6378137.0 * 6378137.0, 6378137.0 * 6378137.0, 6356752.3142451793 * 6356752.3142451793);

    double cosLatitude = cos(latitude);
    scratchN.x = cosLatitude * cos(longitude);
    scratchN.y = cosLatitude * sin(longitude);
    scratchN.z = sin(latitude);
    scratchN.normalize();

    scratchK = radiiSquared * scratchN;
    double gamma = sqrt(Cartesian3::dot(scratchN, scratchK));
    scratchK = scratchK / gamma;
    scratchN = scratchN * height;

    return (scratchK + scratchN);
}

