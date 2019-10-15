#include "ellipsoidgeodesic.h"
#include "liutils.h"
#include "ellipsoid.h"

EllipsoidGeodesic::EllipsoidGeodesic()
{
    _ellipsoid = Ellipsoid::WGS84();

    _startHeading = Math::EPSILON20;
    _endHeading = Math::EPSILON20;
    _distance = Math::EPSILON20;
    _uSquared = Math::EPSILON20;
}

void EllipsoidGeodesic::setEndPoints(const Cartographic &start, const Cartographic &end)
{
    computeProperties(start, end);
}

Cartographic EllipsoidGeodesic::interpolateUsingFraction(double fraction)
{
    return interpolateUsingSurfaceDistance(_distance * fraction);
}

Cartographic EllipsoidGeodesic::interpolateUsingSurfaceDistance(double distance)
{
    double s = _constants.distanceRatio + distance / _constants.b;

    double cosine2S = cos(2.0 * s);
    double cosine4S = cos(4.0 * s);
    double cosine6S = cos(6.0 * s);
    double sine2S = sin(2.0 * s);
    double sine4S = sin(4.0 * s);
    double sine6S = sin(6.0 * s);
    double sine8S = sin(8.0 * s);

    double s2 = s * s;
    double s3 = s * s2;

    double u8Over256 = _constants.u8Over256;
    double u2Over4 = _constants.u2Over4;
    double u6Over64 = _constants.u6Over64;
    double u4Over16 = _constants.u4Over16;
    double sigma = 2.0 * s3 * u8Over256 * cosine2S / 3.0 +
            s * (1.0 - u2Over4 + 7.0 * u4Over16 / 4.0 - 15.0 * u6Over64 / 4.0 + 579.0 * u8Over256 / 64.0 -
                 (u4Over16 - 15.0 * u6Over64 / 4.0 + 187.0 * u8Over256 / 16.0) * cosine2S -
                 (5.0 * u6Over64 / 4.0 - 115.0 * u8Over256 / 16.0) * cosine4S -
                 29.0 * u8Over256 * cosine6S / 16.0) +
            (u2Over4 / 2.0 - u4Over16 + 71.0 * u6Over64 / 32.0 - 85.0 * u8Over256 / 16.0) * sine2S +
            (5.0 * u4Over16 / 16.0 - 5.0 * u6Over64 / 4.0 + 383.0 * u8Over256 / 96.0) * sine4S -
            s2 * ((u6Over64 - 11.0 * u8Over256 / 2.0) * sine2S + 5.0 * u8Over256 * sine4S / 2.0) +
            (29.0 * u6Over64 / 96.0 - 29.0 * u8Over256 / 16.0) * sine6S +
            539.0 * u8Over256 * sine8S / 1536.0;

    double theta = asin(sin(sigma) * _constants.cosineAlpha);
    double latitude = atan(_constants.a / _constants.b * tan(theta));

    // Redefine in terms of relative argument of latitude.
    sigma = sigma - _constants.sigma;

    double cosineTwiceSigmaMidpoint = cos(2.0 * _constants.sigma + sigma);

    double sineSigma = sin(sigma);
    double cosineSigma = cos(sigma);

    double cc = _constants.cosineU * cosineSigma;
    double ss = _constants.sineU * sineSigma;

    double lambda = atan2(sineSigma * _constants.sineHeading, cc - ss * _constants.cosineHeading);

    double l = lambda - computeDeltaLambda(_constants.f, _constants.sineAlpha, _constants.cosineSquaredAlpha,
                                        sigma, sineSigma, cosineSigma, cosineTwiceSigmaMidpoint);

    return Cartographic(_start.longitude + l, latitude, 0.0);
}

void EllipsoidGeodesic::computeProperties(const Cartographic &start, const Cartographic &end)
{
    Cartesian3 firstCartesian = cartographicToCartesian(start).normalize();
    Cartesian3 lastCartesian = cartographicToCartesian(end).normalize();

//    //>>includeStart('debug', pragmas.debug);
//    Check.typeOf.number.greaterThanOrEquals('value', Math.abs(Math.abs(Cartesian3.angleBetween(firstCartesian, lastCartesian)) - Math.PI), 0.0125);
//    //>>includeEnd('debug');

    vincentyInverseFormula(_ellipsoid->maximumRadius(), _ellipsoid->minimumRadius(),
                           start.longitude, start.latitude, end.longitude, end.latitude);

    _start = start;
    _end = end;
    _start.height = 0;
    _end.height = 0;

    setConstants();
}

void EllipsoidGeodesic::setConstants()
{
    double a = _ellipsoid->maximumRadius();
    double b = _ellipsoid->minimumRadius();
    double f = (a - b) / a;

    double cosineHeading = cos(_startHeading);
    double sineHeading = sin(_startHeading);

    double tanU = (1 - f) * tan(_start.latitude);

    double cosineU = 1.0 / sqrt(1.0 + tanU * tanU);
    double sineU = cosineU * tanU;

    double sigma = atan2(tanU, cosineHeading);

    double sineAlpha = cosineU * sineHeading;
    double sineSquaredAlpha = sineAlpha * sineAlpha;

    double cosineSquaredAlpha = 1.0 - sineSquaredAlpha;
    double cosineAlpha = sqrt(cosineSquaredAlpha);

    double u2Over4 = _uSquared / 4.0;
    double u4Over16 = u2Over4 * u2Over4;
    double u6Over64 = u4Over16 * u2Over4;
    double u8Over256 = u4Over16 * u4Over16;

    double a0 = (1.0 + u2Over4 - 3.0 * u4Over16 / 4.0 + 5.0 * u6Over64 / 4.0 - 175.0 * u8Over256 / 64.0);
    double a1 = (1.0 - u2Over4 + 15.0 * u4Over16 / 8.0 - 35.0 * u6Over64 / 8.0);
    double a2 = (1.0 - 3.0 * u2Over4 + 35.0 * u4Over16 / 4.0);
    double a3 = (1.0 - 5.0 * u2Over4);

    double distanceRatio = a0 * sigma - a1 * sin(2.0 * sigma) * u2Over4 / 2.0 - a2 * sin(4.0 * sigma) * u4Over16 / 16.0 -
                        a3 * sin(6.0 * sigma) * u6Over64 / 48.0 - sin(8.0 * sigma) * 5.0 * u8Over256 / 512;

    _constants.a = a;
    _constants.b = b;
    _constants.f = f;
    _constants.cosineHeading = cosineHeading;
    _constants.sineHeading = sineHeading;
    _constants.tanU = tanU;
    _constants.cosineU = cosineU;
    _constants.sineU = sineU;
    _constants.sigma = sigma;
    _constants.sineAlpha = sineAlpha;
    _constants.sineSquaredAlpha = sineSquaredAlpha;
    _constants.cosineSquaredAlpha = cosineSquaredAlpha;
    _constants.cosineAlpha = cosineAlpha;
    _constants.u2Over4 = u2Over4;
    _constants.u4Over16 = u4Over16;
    _constants.u6Over64 = u6Over64;
    _constants.u8Over256 = u8Over256;
    _constants.a0 = a0;
    _constants.a1 = a1;
    _constants.a2 = a2;
    _constants.a3 = a3;
    _constants.distanceRatio = distanceRatio;
}

void EllipsoidGeodesic::vincentyInverseFormula(double major, double minor, double firstLongitude, double firstLatitude, double secondLongitude, double secondLatitude)
{
    double eff = (major - minor) / major;
    double l = secondLongitude - firstLongitude;

    double u1 = atan((1 - eff) * tan(firstLatitude));
    double u2 = atan((1 - eff) * tan(secondLatitude));

    double cosineU1 = cos(u1);
    double sineU1 = sin(u1);
    double cosineU2 = cos(u2);
    double sineU2 = sin(u2);

    double cc = cosineU1 * cosineU2;
    double cs = cosineU1 * sineU2;
    double ss = sineU1 * sineU2;
    double sc = sineU1 * cosineU2;

    double lambda = l;
    double lambdaDot = 2.0 * M_PI;

    double cosineLambda = cos(lambda);
    double sineLambda = sin(lambda);

    double sigma;
    double cosineSigma;
    double sineSigma;
    double cosineSquaredAlpha;
    double cosineTwiceSigmaMidpoint;

    do {
        cosineLambda = cos(lambda);
        sineLambda = sin(lambda);

        double temp = cs - sc * cosineLambda;
        sineSigma = sqrt(cosineU2 * cosineU2 * sineLambda * sineLambda + temp * temp);
        cosineSigma = ss + cc * cosineLambda;

        sigma = atan2(sineSigma, cosineSigma);

        double sineAlpha;

        if (sineSigma == 0.0) {
            sineAlpha = 0.0;
            cosineSquaredAlpha = 1.0;
        } else {
            sineAlpha = cc * sineLambda / sineSigma;
            cosineSquaredAlpha = 1.0 - sineAlpha * sineAlpha;
        }

        lambdaDot = lambda;

        cosineTwiceSigmaMidpoint = cosineSigma - 2.0 * ss / cosineSquaredAlpha;

        if (isnan(cosineTwiceSigmaMidpoint)) {
            cosineTwiceSigmaMidpoint = 0.0;
        }

        lambda = l + computeDeltaLambda(eff, sineAlpha, cosineSquaredAlpha,
                                        sigma, sineSigma, cosineSigma, cosineTwiceSigmaMidpoint);
    } while (abs(lambda - lambdaDot) > Math::EPSILON12);

    double uSquared = cosineSquaredAlpha * (major * major - minor * minor) / (minor * minor);
    double A = 1.0 + uSquared * (4096.0 + uSquared * (uSquared * (320.0 - 175.0 * uSquared) - 768.0)) / 16384.0;
    double B = uSquared * (256.0 + uSquared * (uSquared * (74.0 - 47.0 * uSquared) - 128.0)) / 1024.0;

    double cosineSquaredTwiceSigmaMidpoint = cosineTwiceSigmaMidpoint * cosineTwiceSigmaMidpoint;
    double deltaSigma = B * sineSigma * (cosineTwiceSigmaMidpoint + B * (cosineSigma *
            (2.0 * cosineSquaredTwiceSigmaMidpoint - 1.0) - B * cosineTwiceSigmaMidpoint *
            (4.0 * sineSigma * sineSigma - 3.0) * (4.0 * cosineSquaredTwiceSigmaMidpoint - 3.0) / 6.0) / 4.0);

    double distance = minor * A * (sigma - deltaSigma);

    double startHeading = atan2(cosineU2 * sineLambda, cs - sc * cosineLambda);
    double endHeading = atan2(cosineU1 * sineLambda, cs * cosineLambda - sc);

    _distance = distance;
    _startHeading = startHeading;
    _endHeading = endHeading;
    _uSquared = uSquared;
}

double EllipsoidGeodesic::computeDeltaLambda(double f, double sineAlpha, double cosineSquaredAlpha, double sigma, double sineSigma, double cosineSigma, double cosineTwiceSigmaMidpoint)
{
    double C = computeC(f, cosineSquaredAlpha);

    return (1.0 - C) * f * sineAlpha * (sigma + C * sineSigma * (cosineTwiceSigmaMidpoint +
                                                                 C * cosineSigma * (2.0 * cosineTwiceSigmaMidpoint * cosineTwiceSigmaMidpoint - 1.0)));
}

double EllipsoidGeodesic::computeC(double f, double cosineSquaredAlpha)
{
    return f * cosineSquaredAlpha * (4.0 + f * (4.0 - 3.0 * cosineSquaredAlpha)) / 16.0;
}
