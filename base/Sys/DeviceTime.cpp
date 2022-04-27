/* -----------------------------------------------------------------
                           Time Class

    This module implements a class to represent real-world time.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2021, Michael T. Duffy II.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
// Redistributions of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <ctime>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Sys/DeviceTime.hpp"
#include "Util/NTP.hpp"
#include "Util/ParseTools.hpp"

// Reference: https://www.cplusplus.com/reference/ctime/

//-----------------------------------------------------------------------------
//  Time Delta
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TimeDelta::TimeDelta  ()
  {
  iDelta = 0;
  };

//-----------------------------------------------------------------------------
TimeDelta::TimeDelta  (const TimeDelta &  deltaIn)
  {
  iDelta = deltaIn.iDelta;
  };

//-----------------------------------------------------------------------------
TimeDelta::TimeDelta  (const char *  szTimeDeltaIn)
  {
  FromString (szTimeDeltaIn);
  };

//-----------------------------------------------------------------------------
TimeDelta::TimeDelta  (INT           iSecIn)
  {
  iDelta = iSecIn;
  };

//-----------------------------------------------------------------------------
TimeDelta::~TimeDelta  ()
  {
  };

//-----------------------------------------------------------------------------
VOID  TimeDelta::FromString  (const char *  szTimeIn)
  {
  iDelta = 0;

  RStrParser  parser (szTimeIn);

  // First try to parse from standard format like "D:HH:MM:SS"
  INT  iNumColons = parser.CountChar (':');
  // TODO: handle negatives in standard format

  if (iNumColons >= 3)
    {
    INT  iDays = parser.GetInt ();
    if (! ParseTools::SkipChar (parser, ':'))  return;
    iDelta += DAY_TO_SEC (iDays);
    }
  if (iNumColons >= 2)
    {
    INT  iHours = parser.GetInt ();
    if (! ParseTools::SkipChar (parser, ':'))  return;
    iDelta += HOUR_TO_SEC (iHours);
    }
  if (iNumColons >= 1)
    {
    INT  iMinutes = parser.GetInt ();
    if (! ParseTools::SkipChar (parser, ':'))  return;
    iDelta += MINUTE_TO_SEC (iMinutes);
    }
  if (iNumColons > 0)
    {
    INT  iSeconds = parser.GetInt ();
    iDelta += iSeconds;
    }

  // Now try to parse out non-colon formatted times, like 3d 4h, etc.
  while (!parser.IsEOL ())
    {
    INT     iValue = parser.GetInt ();
    UINT32  uUnit  = parser.GetChar ();

    if ((uUnit == 'd') || (uUnit == 'D'))
      {
      iDelta += DAY_TO_SEC (iValue);
      }
    if ((uUnit == 'h') || (uUnit == 'H'))
      {
      iDelta += HOUR_TO_SEC (iValue);
      }
    if ((uUnit == 'm') || (uUnit == 'M'))
      {
      iDelta += MINUTE_TO_SEC (iValue);
      }
    if ((uUnit == 's') || (uUnit == 'S') || (uUnit == 0))
      {
      iDelta += iValue;
      }
    };
  };


//-----------------------------------------------------------------------------
INT  TimeDelta::Days  (VOID) const
  {
  INT  iPosDelta = iDelta < 0 ? -iDelta : iDelta;

  return (SEC_TO_DAY (iPosDelta));
  };

//-----------------------------------------------------------------------------
INT  TimeDelta::Hours  (VOID) const
  {
  INT  iPosDelta = iDelta < 0 ? -iDelta : iDelta;

  INT  iDays    = SEC_TO_DAY    (iPosDelta);
  return (SEC_TO_HOUR (iPosDelta - DAY_TO_SEC(iDays)));
  };

//-----------------------------------------------------------------------------
INT  TimeDelta::Minutes  (VOID) const
  {
  INT  iPosDelta = iDelta < 0 ? -iDelta : iDelta;

  INT  iHours   = SEC_TO_HOUR   (iPosDelta);
  return (SEC_TO_MINUTE (iPosDelta - HOUR_TO_SEC (iHours)));
  };

//-----------------------------------------------------------------------------
INT  TimeDelta::Seconds  (VOID) const
  {
  INT  iPosDelta = iDelta < 0 ? -iDelta : iDelta;

  INT  iMinutes = SEC_TO_MINUTE (iPosDelta);
  return (iPosDelta - MINUTE_TO_SEC (iMinutes));
  };

//-----------------------------------------------------------------------------
BOOL  TimeDelta::operator==  (const TimeDelta &  deltaIn) const
  {
  return (deltaIn.iDelta == iDelta);
  };

//-----------------------------------------------------------------------------
const char *  TimeDelta::ToStopwatchString  (RStr *  pstrOut) const
  {
  static RStr  strOut;

  // if no RStr is passed, use a static one.  If the static one is used, then
  //  only one conversion can be active at a time.

  if (pstrOut == NULL)
    {
    strOut.Empty ();
    pstrOut = &strOut;
    };

  // Converts to a format of "D:HH:MM:SS"

  INT  iPosDelta = iDelta < 0 ? -iDelta : iDelta;

  INT  iDays    = SEC_TO_DAY    (iPosDelta);
  INT  iHours   = SEC_TO_HOUR   (iPosDelta - DAY_TO_SEC(iDays));
  INT  iMinutes = SEC_TO_MINUTE (iPosDelta - DAY_TO_SEC(iDays) - HOUR_TO_SEC (iHours));
  INT  iSeconds = iPosDelta - DAY_TO_SEC(iDays) - HOUR_TO_SEC (iHours) - MINUTE_TO_SEC (iMinutes);

  if (iDelta < 0)
    {
    pstrOut->Set ("-");
    }
  if (iDays > 0)
    {
    pstrOut->AppendFormat ("%d:", iDays);
    };
  if (iHours > 0)
    {
    pstrOut->AppendFormat ("%02d:", iHours);
    };
  pstrOut->AppendFormat ("%02d:%02d", iMinutes, iSeconds);

  return (pstrOut->AsChar());
  };


//-----------------------------------------------------------------------------
const char *  TimeDelta::ToLetterString  (RStr *  pstrOut,
                                          BOOL    bHideSeconds) const
  {
  static RStr  strOut;

  // if no RStr is passed, use a static one.  If the static one is used, then
  //  only one conversion can be active at a time.

  if (pstrOut == NULL)
    {
    strOut.Empty ();
    pstrOut = &strOut;
    };

  // Converts to a format of "D d H h M m S s" where the inclusion of seconds
  //  is driven by the bHideSeconds parameter.

  INT  iPosDelta = iDelta < 0 ? -iDelta : iDelta;

  INT  iDays    = SEC_TO_DAY    (iPosDelta);
  INT  iHours   = SEC_TO_HOUR   (iPosDelta - DAY_TO_SEC(iDays));
  INT  iMinutes = SEC_TO_MINUTE (iPosDelta - DAY_TO_SEC(iDays) - HOUR_TO_SEC (iHours));
  INT  iSeconds = iPosDelta - DAY_TO_SEC(iDays) - HOUR_TO_SEC (iHours) - MINUTE_TO_SEC (iMinutes);

  if (iDelta < 0)
    {
    pstrOut->Set ("-");
    }
  if (iDays > 0)
    {
    pstrOut->AppendFormat ("%dd ", iDays);
    };
  if (iHours > 0)
    {
    pstrOut->AppendFormat ("%02dh ", iHours);
    };
  pstrOut->AppendFormat ("%02dm", iMinutes);
  if (!bHideSeconds)
    {
    pstrOut->AppendFormat (" %02ds", iSeconds);
    };

  return (pstrOut->AsChar());
  };



/*

#define SEC_TO_MS(sec)     ((sec)*1000)
#define MS_TO_SEC(ms)      ((ms)/1000)
#define USEC_TO_MS(usec)   ((usec)/1000)
#define SEC_TO_MINUTE(sec) ((sec)/60)
#define SEC_TO_HOUR(sec)   ((sec)/3600)
#define SEC_TO_DAY(sec)    ((sec)/(3600*24))
#define MINUTE_TO_SEC(min) ((min)*60)
#define HOUR_TO_SEC(hour)  ((hour)*3600)
#define DAY_TO_SEC(day)    ((day)*3600*24)
#define DAY_TO_HOUR(day)   ((day)*24)

#define MINUTE_TO_HOUR(min) ((min)/60)
#define HOUR_TO_MINUTE(hour) ((hour)*60)

*/


//-----------------------------------------------------------------------------
// DeviceTime
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DeviceTime::DeviceTime  ()
  {
  };

//-----------------------------------------------------------------------------
DeviceTime::DeviceTime  (const DeviceTime &        timeIn) : _timeUTC (timeIn._timeUTC)
  {
  };

//-----------------------------------------------------------------------------
DeviceTime::DeviceTime  (const char *  szDeviceTimeIn)
  {
  FromString  (szDeviceTimeIn);
  };

//-----------------------------------------------------------------------------
DeviceTime::DeviceTime        (time_t              timeIn) : _timeUTC (timeIn)
  {
  };

 /*
//-----------------------------------------------------------------------------
DeviceTime::DeviceTime        (INT64  iSecondsSinceEpochIn,
                               BOOL   bUnusedIn)
  {
  struct tm  _tm;

  memset (&_tm, 0, sizeof (_tm));
  _tm.tm_sec = iSecondsSinceEpochIn; // NOTE: Can go out of bounds INT64->INT

  _timeUTC = DeviceTime::MakeUTC (&_tm);
  };
*/

//-----------------------------------------------------------------------------
DeviceTime::~DeviceTime  ()
  {
  };

//-----------------------------------------------------------------------------
time_t DeviceTime::MakeUTC (struct tm *  ptmUTCIn)
  {
  // NOTE:  Because mktime only works in the local time zone, we need to
  //   subtract the timezone difference to UTC, which we find by sending
  //   zero-time (epoch) through localtime and mktime.
  // TODO: Make sure this is accurate in daylight savings time.

  // use the passed tm before calculating zero time, otherwise you'll likely
  //  overwrite the internal structure.
  time_t      timeIn = mktime (ptmUTCIn);

  time_t      timeZero = 0;
  time_t      timeOffsetUTC = mktime(gmtime(&timeZero));

  //DBG_INFO ("MakeUTC returns %d - %d", timeIn, timeOffsetUTC);

  return (timeIn - timeOffsetUTC);
  };

//-----------------------------------------------------------------------------
DeviceTime  DeviceTime::Now   (VOID)
  {
  return (DeviceTime (time (NULL))); // time() should return UTC
  };

//-----------------------------------------------------------------------------
DeviceTime  DeviceTime::operator+   (const TimeDelta &  timeDeltaIn) const
  {
  time_t      timeOut = _timeUTC;
  struct tm * ptm;

  ptm = gmtime (&timeOut);

  ptm->tm_sec += timeDeltaIn.Seconds ();
  ptm->tm_min += timeDeltaIn.Minutes ();
  ptm->tm_hour += timeDeltaIn.Hours () + DAY_TO_HOUR (timeDeltaIn.Days());

  timeOut = DeviceTime::MakeUTC (ptm);

  return (DeviceTime (timeOut));
  };

//-----------------------------------------------------------------------------
DeviceTime &  DeviceTime::operator+=  (const TimeDelta &  timeDeltaIn)
  {
  *this = *this + timeDeltaIn;
  return (*this);
  };

//-----------------------------------------------------------------------------
DeviceTime  DeviceTime::operator-   (const TimeDelta &  timeDeltaIn) const
  {
  time_t      timeOut = _timeUTC;
  struct tm * ptm;

  ptm = gmtime (&timeOut);

  ptm->tm_sec -= timeDeltaIn.Seconds ();
  ptm->tm_min -= timeDeltaIn.Minutes ();
  ptm->tm_hour -= timeDeltaIn.Hours () - DAY_TO_HOUR (timeDeltaIn.Days());

  timeOut = DeviceTime::MakeUTC (ptm);

  return (DeviceTime (timeOut));
  };

//-----------------------------------------------------------------------------
DeviceTime &  DeviceTime::operator-=  (const TimeDelta &  timeDeltaIn)
  {
  *this = *this - timeDeltaIn;
  return (*this);
  };

//-----------------------------------------------------------------------------
TimeDelta  DeviceTime::operator-   (const DeviceTime &  timeIn) const
  {
  DOUBLE  dDiffTime = difftime (_timeUTC, timeIn._timeUTC);

  return (TimeDelta (INT64 (dDiffTime)));
  };

//-----------------------------------------------------------------------------
BOOL  DeviceTime::operator<   (const DeviceTime &  timeIn) const
  {
  return (difftime (_timeUTC, timeIn._timeUTC) < 0.0);
  };

//-----------------------------------------------------------------------------
BOOL  DeviceTime::operator<=  (const DeviceTime &  timeIn) const
  {
  return (difftime (_timeUTC, timeIn._timeUTC) <= 0.0);
  };

//-----------------------------------------------------------------------------
BOOL  DeviceTime::operator>   (const DeviceTime &  timeIn) const
  {
  return (difftime (_timeUTC, timeIn._timeUTC) > 0.0);
  };

//-----------------------------------------------------------------------------
BOOL  DeviceTime::operator>=  (const DeviceTime &  timeIn) const
  {
  return (difftime (_timeUTC, timeIn._timeUTC) >= 0.0);
  };

//-----------------------------------------------------------------------------
BOOL  DeviceTime::operator==  (const DeviceTime &  timeIn) const
  {
  return (timeIn._timeUTC == _timeUTC);
  };

//-----------------------------------------------------------------------------
VOID  DeviceTime::FromString  (const char *  szDeviceTimeIn)
  {
  _timeUTC = UnixFromString (szDeviceTimeIn, YearMonthDayDash, FALSE);
  };

//-----------------------------------------------------------------------------
const char *  DeviceTime::ToString  (RStr *  pstrOut) const
  {
  static RStr  strOut;

  // if no RStr is passed, use a static one.  If the static one is used, then
  //  only one conversion can be active at a time.

  if (pstrOut == NULL)
    {
    strOut.Empty ();
    pstrOut = &strOut;
    };

  struct tm * ptm = gmtime (&_timeUTC);

  pstrOut->Format ("%d-%d-%d %02d:%02d:%02d",
                 ptm->tm_year + 1900,
                 ptm->tm_mon + 1,
                 ptm->tm_mday,
                 ptm->tm_hour,
                 ptm->tm_min,
                 ptm->tm_sec);
  return (pstrOut->AsChar ());
  };

//-----------------------------------------------------------------------------
time_t DeviceTime::UnixFromString (const char *     szDateIn,
                                   EDateTimeFormat  formatIn,
                                   BOOL             bUseLocalTimezoneIn)
  {
  // This code based off of:
  // https://stackoverflow.com/questions/11213326/how-to-convert-a-string-variable-containing-time-to-time-t-type-in-c

  int YY, MM, DD, hh, mm, ss;
  struct tm when = { 0 };
  int iResult = 0;

  if (formatIn == YearMonthDayDash)
    {
    iResult = sscanf (szDateIn, "%d-%d-%d %d:%d:%d", &YY, &MM, &DD, &hh, &mm, &ss);
    }
  else if (formatIn == MonthDayYearDash)
    {
    iResult = sscanf (szDateIn, "%d-%d-%d %d:%d:%d", &MM, &DD, &YY, &hh, &mm, &ss);
    }
  else if (formatIn == DayMonthYearDash)
    {
    iResult = sscanf (szDateIn, "%d-%d-%d %d:%d:%d", &DD, &MM, &YY, &hh, &mm, &ss);
    }
  // In case datetime was in bad format, returns 0.
  if (iResult == EOF || iResult == 0) {
      return 0;
  }

  when.tm_year = YY - 1900; // Years from 1900
  when.tm_mon  = MM - 1;    // 0-based
  when.tm_mday = DD;        // 1 based

  when.tm_hour = hh;
  when.tm_min  = mm;
  when.tm_sec  = ss;

  // Make sure the daylight savings is same as current timezone.
  if (bUseLocalTimezoneIn)
    {
    time_t now = time(0);
    when.tm_isdst = localtime(&now)->tm_isdst;
    };

  // Convert the tm struct to the Linux epoch
  time_t converted;
  converted = DeviceTime::MakeUTC (&when);

  return converted;
  }
