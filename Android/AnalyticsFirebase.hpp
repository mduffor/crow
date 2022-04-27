/* -----------------------------------------------------------------
                          Analytics - Firebase

     This module is the Firebase version of the analytics system.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2017, Michael T. Duffy II.  All rights reserved.
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

#ifndef ANALYTICSFIREBASE_HPP
#define ANALYTICSFIREBASE_HPP

#include "firebase/analytics.h"
#include "firebase/analytics/event_names.h"
#include "firebase/analytics/parameter_names.h"
#include "firebase/analytics/user_property_names.h"
#include "firebase/app.h"

#include "Sys/Types.hpp"

#include "Net/AnalyticsAdapter.hpp"

namespace analytics = ::firebase::analytics;

//------------------------------------------------------------------------------
class AnalyticsFirebase : public AnalyticsAdapter
  {
  private:

    ::firebase::App*      pApp;

    INT                   iParameterCount;
    analytics::Parameter  arrayParameters[25];

  public:

            AnalyticsFirebase  ();

    virtual ~AnalyticsFirebase ();

    VOID    Init               (JNIEnv*         jniEnvIn,
                                jobject         activityIn);

    VOID    Uninit             (VOID);

    VOID    SetUserProperty    (const char *    szKeyIn,
                                const char *    szValueIn);

    VOID    SetUserId          (const char *    szIDIn);

    VOID    SetCurrentScreen   (const char *    szScreenNameIn,
                                const char *    szScreenClassIn);

    VOID    LogEvent           (const char *    szEventNameIn);

    VOID    LogEventString     (const char *    szEventNameIn,
                                const char *    szParamNameIn,
                                const char *    szValueIn);

    VOID    LogEventInt        (const char *    szEventNameIn,
                                const char *    szParamNameIn,
                                const int64_t   iValueIn);

    VOID    LogEventFloat      (const char *    szEventNameIn,
                                const char *    szParamNameIn,
                                const double    dValueIn);

    VOID    LogEventWithParams (const char *    szEventNameIn);

    VOID    ClearParams        (VOID);

    INT     GetNumParams       (VOID)           {return iParameterCount;};

    VOID    AddParam           (const char *    szParamNameIn,
                                const char *    szValueIn);

    VOID    AddParam           (const char *    szParamNameIn,
                                const int64_t   iValueIn);

    VOID    AddParam           (const char *    szParamNameIn,
                                const double    dValueIn);



  };



#endif // ANALYTICSFIREBASE_HPP