/* -----------------------------------------------------------------
                          Analytics - Log

     This module is the run log version of the analytics system.

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

#include "Net/AnalyticsLog.hpp"

//-----------------------------------------------------------------------------
AnalyticsLog::AnalyticsLog  ()
  {
  };

//-----------------------------------------------------------------------------
AnalyticsLog::~AnalyticsLog ()
  {
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::SetUserProperty  (const char *  szKeyIn,
                                     const char *  szValueIn)
  {
  DBG_INFO ("Analytics: UserProperty = %s:%s", szKeyIn, szValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::SetUserId  (const char *  szIDIn)
  {
  DBG_INFO ("Analytics: UserId = %s", szIDIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::SetCurrentScreen (const char *  szScreenNameIn,
                                     const char *  szScreenClassIn)
  {
  DBG_INFO ("Analytics: CurrentScreen = %s:%s", szScreenNameIn, (szScreenClassIn == nullptr) ? "undefined" : szScreenClassIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::LogEvent (const char *  szEventNameIn)
  {
  DBG_INFO ("Analytics: Event %s", szEventNameIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::LogEventString (const char *  szEventNameIn,
                                   const char *  szParamNameIn,
                                   const char *  szValueIn)
  {
  DBG_INFO ("Analytics: Event %s", szEventNameIn);
  DBG_INFO ("           %s : %s", szParamNameIn, szValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::LogEventInt (const char *   szEventNameIn,
                                const char *   szParamNameIn,
                                const int64_t  iValueIn)
  {
  DBG_INFO ("Analytics: Event %s", szEventNameIn);
  DBG_INFO ("           %s : %d", szParamNameIn, (int)iValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::LogEventFloat (const char *    szEventNameIn,
                                  const char *    szParamNameIn,
                                  const double    dValueIn)
  {
  DBG_INFO ("Analytics: Event %s", szEventNameIn);
  DBG_INFO ("           %s : %f", szParamNameIn, dValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::LogEventWithParams (const char *  szEventNameIn)
  {
  DBG_INFO ("Analytics: Event %s", szEventNameIn);

  INT  iNumParams = regParameters.Size ();
  for (INT  iIndex = 0; iIndex < iNumParams; ++iIndex)
    {
    ValueElem *  pelem = regParameters.FindByIndex (iIndex);
    DBG_INFO ("           %s : %s", pelem->GetName (), pelem->GetString ());
    };
  ClearParams ();
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::ClearParams (VOID)
  {
  regParameters.Clear ();
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::AddParam (const char *  szParamNameIn,
                             const char *  szValueIn)
  {
  regParameters.SetString (szParamNameIn, szValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::AddParam (const char *   szParamNameIn,
                             const int64_t  iValueIn)
  {
  regParameters.SetInt (szParamNameIn, (INT) iValueIn);
  };

//-----------------------------------------------------------------------------
VOID AnalyticsLog::AddParam (const char *    szParamNameIn,
                             const double    dValueIn)
  {
  regParameters.SetFloat (szParamNameIn, (FLOAT) dValueIn);
  };


