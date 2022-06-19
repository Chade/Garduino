#ifndef _SUNCALENDER_H_
#define _SUNCALENDER_H_

#include <Arduino.h>
#include <TimeLib.h>

// *****************************************************************************
// Defines
// *****************************************************************************

#define M_2PI     (M_PI * 2.0)
#define M_DEG2RAD (M_PI / 180.0)


// *****************************************************************************
// Functions
// *****************************************************************************

double julianDate (const time_t& unixtime)
{
  return  elapsedDays(unixtime) + 2440587.5;
}

double julianDay (const time_t& unixtime)
{
  return  julianDate(unixtime) - 2451545.0 + 0.0008;
}

double julianCentury (const time_t& unixtime)
{
  return julianDay(unixtime) / 36525.0;
}

double normalizeRad(double angle)
{
  int32_t n = (int32_t)(angle / M_2PI);
	angle = angle - n * M_2PI;
	if (angle < 0)
    angle += M_2PI;
	return angle;
}

double normalize24(double hours)
{
  if (hours < 0.0)
    hours += 24.0;
  else if (hours >= 24.0)
    hours -= 24.0;

  return hours;
}

void sunCalender(time_t& sunrise_unix, time_t& sunset_unix, const time_t& time_unix, const double& latitude_rad, const double& longitude_rad, const double& timezone_offset)
{
  double T = julianCentury(time_unix);  // [julian century]
  
  // Height over horizon
  double h = -50.0 / 60.0 * M_DEG2RAD; // [radians]

  // Rectaszension of the mean sun
  double RA_mean = 18.71506921 + 2400.0513369 * T + 2.5862e-5 * T * T - 1.72e-9 * T * T * T; // [hours]
  // Normalize to 24h
  RA_mean = 24.0 * normalizeRad(M_2PI * RA_mean / 24.0) / M_2PI; // [hours]
  
  double M  = normalizeRad(M_2PI * (0.993133 + 99.997361 * T)); // [radians]
  
  // Traveled distance of the sun on the ecliptic since spring
  double L  = normalizeRad(M_2PI * (0.7859453 + M / M_2PI + (6893.0 * sin(M) + 72.0 * sin(2.0 * M) + 6191.2 * T) / 1296.0e3)); // [radians]

  // Calculate inclination of earth axias
  double e = M_DEG2RAD * (23.43929111 + (-46.8150 * T - 0.00059 * T * T + 0.001813 * T * T * T) / 3600.0); // [radians]

  // Declination of the sun
  double DK = asin(sin(e) * sin(L)); // [radians]

  // Rectaszension of the real sun
  double RA_real = atan(tan(L) * cos(e)); // [radians]
  
  // Account for atan inconsistency
  if (RA_real < 0.0)
    RA_real += M_PI;
  if (L > M_PI)
    RA_real += M_PI;

  // Convert to hours
  RA_real = 24.0 * RA_real / M_2PI; // [hours]

  // Time equation
  double dRA =  1.0027379*(RA_mean - RA_real);
  if (dRA < -12.0)
    dRA += 24.0;
  if (dRA > 12.0)
    dRA -= 24.0;

  // Time difference from true noon till sun reaches height h over horizon
  // If value is greater 1 the sun never reaches demanded height h
  double timeDifference = 12.0 * acos((sin(h) - sin(latitude_rad) * sin(DK)) / (cos(latitude_rad) * cos(DK))) / M_PI; // [hours]

  double sunrise_hour = 12.0 - timeDifference - dRA - (24.0 * longitude_rad / M_2PI) + timezone_offset; // [hours]
  double sunset_hour  = 12.0 + timeDifference - dRA - (24.0 * longitude_rad / M_2PI) + timezone_offset; // [hours]

  sunrise_unix = previousMidnight(time_unix) + sunrise_hour * SECS_PER_HOUR;
  sunset_unix =  previousMidnight(time_unix) + sunset_hour  * SECS_PER_HOUR;
}

#endif
