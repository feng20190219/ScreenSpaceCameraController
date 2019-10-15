#include "math.h"
#include "cesiummath.h"

CesiumMath::CesiumMath()
{
}

bool CesiumMath::equalsEpsilon(double left, double right, double relativeEpsilon, double absoluteEpsilon)
{
    if(absoluteEpsilon == 0.0)
        absoluteEpsilon = relativeEpsilon;
    double absDiff = abs(left - right);
    return (absDiff <= absoluteEpsilon) || (absDiff <= relativeEpsilon * std::max(abs(left), abs(right)));
}

double CesiumMath::zeroToTwoPi(double angle)
{
    double TwoPI = 2.0 * M_PI;
    double m = mod(angle, TwoPI);
    if (abs(m) < Math::EPSILON14 && abs(angle) > Math::EPSILON14) {
        return TwoPI;
    }
    return m;
}

double CesiumMath::mod(double m, double n)
{
    return fmod(fmod(m, n) + n, n);
}

double CesiumMath::acosClamped(double value)
{
    return acos(Math::clamp(value, -1.0, 1.0));
}

double CesiumMath::negativePiToPi(double angle)
{
    return zeroToTwoPi(angle + M_PI) - M_PI;
}

double CesiumMath::lerp(double p, double q, double time)
{
    return ((1.0 - time) * p) + (time * q);
}

Quaternion CesiumMath::fromHeadingPitchRoll(double heading, double pitch, double roll)
{
    heading = Math::toDegrees(heading);
    pitch = Math::toDegrees(pitch);
    roll = Math::toDegrees(roll);
    Quaternion scratchRollQuaternion = Quaternion::fromAxisAndAngle(Cartesian3::UNIT_X, roll);
    Quaternion scratchPitchQuaternion = Quaternion::fromAxisAndAngle(Cartesian3::UNIT_Y, -pitch);
    Quaternion result = scratchPitchQuaternion * scratchRollQuaternion;
    Quaternion scratchHeadingQuaternion = Quaternion::fromAxisAndAngle(Cartesian3::UNIT_Z, -heading);
    return scratchHeadingQuaternion * result;
}
