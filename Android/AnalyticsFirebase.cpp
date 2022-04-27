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

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Android/AnalyticsFirebase.hpp"

const INT FIREBASE_MAX_PARAMETERS = 25;


//-----------------------------------------------------------------------------
AnalyticsFirebase::AnalyticsFirebase  ()
  {
  pApp = NULL;
  iParameterCount = 0;
  };

//-----------------------------------------------------------------------------
AnalyticsFirebase::~AnalyticsFirebase ()
  {
  Uninit ();
  };

//-----------------------------------------------------------------------------
VOID  AnalyticsFirebase::Init   (JNIEnv*  jniEnvIn,
                                 jobject  activityIn)
  {
  #if defined(__ANDROID__)
    pApp = ::firebase::App::Create (::firebase::AppOptions(), jniEnvIn, activityIn);
  #else
    pApp = ::firebase::App::Create();
  #endif  // defined(__ANDROID__)
  analytics::Initialize(*pApp);

  // Enable data collection.
  //analytics::SetAnalyticsCollectionEnabled (JNI_TRUE);

  // App needs to be open at least 1s before logging a valid session.
  //analytics::SetMinimumSessionDuration (1000); // deprecated in latest version of firebase

  // App session times out after 30 minutes
  analytics::SetSessionTimeoutDuration (1800000);
  };

//-----------------------------------------------------------------------------
VOID  AnalyticsFirebase::Uninit (VOID)
  {
  if (pApp != NULL)
    {
    analytics::Terminate();
    delete pApp;
    pApp = NULL;
    };
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::SetUserProperty  (const char *  szKeyIn,
                                          const char *  szValueIn)
  {
  // NOTE:  Up to 25 properties can be associated with a user.

  analytics::SetUserProperty (szKeyIn, szValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::SetUserId  (const char *  szIDIn)
  {
  // NOTE:  ID is a string up to 36 chars long.

  analytics::SetUserId (szIDIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::SetCurrentScreen (const char *  szScreenNameIn,
                                          const char *  szScreenClassIn)
  {
  // NOTE:  Set Screen Name to nullptr to clear it.  Set classname
  //   to nullptr to use the default value (name of activity).  Both
  //   are limited to 100 chars.

  analytics::SetCurrentScreen (szScreenNameIn, szScreenClassIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::LogEvent (const char *  szEventNameIn)
  {
  // NOTE:  Event name is 1-32 alphanum characters or underscores

  // NOTE:  Firebase allows up to 500 different types of events per app,
  //         and 25 unique parameters per event.

  analytics::LogEvent (szEventNameIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::LogEventString (const char *  szEventNameIn,
                                        const char *  szParamNameIn,
                                        const char *  szValueIn)
  {
  analytics::LogEvent (szEventNameIn, szParamNameIn, szValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::LogEventInt (const char *   szEventNameIn,
                                     const char *   szParamNameIn,
                                     const int64_t  iValueIn)
  {
  analytics::LogEvent (szEventNameIn, szParamNameIn, iValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::LogEventFloat (const char *    szEventNameIn,
                                       const char *    szParamNameIn,
                                       const double    dValueIn)
  {
  analytics::LogEvent (szEventNameIn, szParamNameIn, dValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::LogEventWithParams (const char *  szEventNameIn)
  {
  analytics::LogEvent (szEventNameIn, arrayParameters, iParameterCount);
  ClearParams ();
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::ClearParams (VOID)
  {
  iParameterCount = 0;
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::AddParam (const char *  szParamNameIn,
                                  const char *  szValueIn)
  {
  // NOTE:  When szParamName is passed in, the pointer is stored until ClearParams
  //  or LogEventWithParams is called.  Ideally only const strings should be passed
  //  as szParamNameIn

  ASSERT (iParameterCount < FIREBASE_MAX_PARAMETERS);

  if (iParameterCount < FIREBASE_MAX_PARAMETERS)
    {
    arrayParameters[iParameterCount].name  = szParamNameIn;
    arrayParameters[iParameterCount].value = szValueIn;
    ++iParameterCount;
    };
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::AddParam (const char *   szParamNameIn,
                                  const int64_t  iValueIn)
  {
  ASSERT (iParameterCount < FIREBASE_MAX_PARAMETERS);

  if (iParameterCount < FIREBASE_MAX_PARAMETERS)
    {
    arrayParameters[iParameterCount].name  = szParamNameIn;
    arrayParameters[iParameterCount].value = iValueIn;
    ++iParameterCount;
    };
  };

//-----------------------------------------------------------------------------
VOID AnalyticsFirebase::AddParam (const char *    szParamNameIn,
                                  const double    dValueIn)
  {
  ASSERT (iParameterCount < FIREBASE_MAX_PARAMETERS);

  if (iParameterCount < FIREBASE_MAX_PARAMETERS)
    {
    arrayParameters[iParameterCount].name  = szParamNameIn;
    arrayParameters[iParameterCount].value = dValueIn;
    ++iParameterCount;
    };
  };


