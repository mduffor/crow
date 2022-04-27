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

#ifndef TIME_HPP
#define TIME_HPP

#include "Sys/Types.hpp"
#include "Util/Signal.h"
#include "ValueRegistry/ValueRegistry.hpp"
/**
  @addtogroup base
  @{
*/
// NOTE:  These classes are meant to represent real-world time, down to the
//  second.  They are not suitable for sub-second timing, or for representing
//  game-world time.

// NOTE: Sys/Types.hpp has a number of macros to convert between different
//  units of time (seconds to minutes or hours, etc).

//------------------------------------------------------------------------------
class TimeDelta
  {
  public:
    INT64         iDelta;

  public:
                  TimeDelta  ();
                  TimeDelta  (const TimeDelta &  deltaIn);
    explicit      TimeDelta  (INT                iSecIn);
    explicit      TimeDelta  (const char *       szTimeDeltaIn);
    virtual       ~TimeDelta ();

    INT           Days         (VOID) const;
    INT           Hours        (VOID) const;
    INT           Minutes      (VOID) const;
    INT           Seconds      (VOID) const;
    INT64         TotalSeconds (VOID) const     {return (iDelta);};

    BOOL          operator==  (const TimeDelta &  deltaIn) const;

    VOID          FromString        (const char *  szTimeIn);
    const char *  ToStopwatchString (RStr *        pstrOut = NULL) const;
    const char *  ToLetterString    (RStr *        pstrOut = NULL,
                                     BOOL          bHideSeconds = FALSE) const;


  };

//------------------------------------------------------------------------------
class DeviceTime
  {
  public:

    time_t        _timeUTC;


    enum EDateTimeFormat  {
                          YearMonthDayDash = 0, // "YYYY-MM-DD hh:mm::ss"
                          MonthDayYearDash = 1, // "MM-DD-YYYY hh:mm::ss"
                          DayMonthYearDash = 2 // "DD-MM-YYYY hh:mm::ss"
                          };

  public:
                  DeviceTime        ();
                  DeviceTime        (const DeviceTime &  timeIn);
    explicit      DeviceTime        (const char *        szTimeIn);
    explicit      DeviceTime        (time_t              timeIn);

//                  DeviceTime        (INT64  iSecondsSinceEpochIn,
//                                     BOOL   bUnusedIn); // to differentiate wiht time_t

    virtual       ~DeviceTime       ();

    static DeviceTime    Now               (VOID);

    static time_t MakeUTC     (struct tm *  ptmUTCIn);

    DeviceTime    operator+   (const TimeDelta &  timeDeltaIn) const;
    DeviceTime &  operator+=  (const TimeDelta &  timeDeltaIn);
    DeviceTime    operator-   (const TimeDelta &  timeDeltaIn) const;
    DeviceTime &  operator-=  (const TimeDelta &  timeDeltaIn);

    TimeDelta     operator-   (const DeviceTime &  timeIn) const;
    BOOL          operator<   (const DeviceTime &  timeIn) const;
    BOOL          operator<=  (const DeviceTime &  timeIn) const;
    BOOL          operator>   (const DeviceTime &  timeIn) const;
    BOOL          operator>=  (const DeviceTime &  timeIn) const;
    BOOL          operator==  (const DeviceTime &  timeIn) const;

    VOID          FromString  (const char *  szTimeIn);
    const char *  ToString    (RStr *        pstrOut = NULL) const;

    static time_t UnixFromString (const char *     szDateIn,
                                  EDateTimeFormat  formatIn,
                                  BOOL             bUseLocalTimezoneIn);

  };
/** @} */ // end of base group

#endif // TIME_HPP
