/* -----------------------------------------------------------------
                                Analytics

     This module is the main interface to the analytics system.

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

#include "Net/Analytics.hpp"

TList<AnalyticsAdapter *>  Analytics::listAdapters;
DBG(ValueRegistrySimple    Analytics::registryParams);


//-----------------------------------------------------------------------------
Analytics::Analytics  ()
  {

  };

//-----------------------------------------------------------------------------
Analytics::~Analytics  ()
  {
  DeleteAllAdapters ();
  };

//-----------------------------------------------------------------------------
VOID Analytics::RegisterAdapter (AnalyticsAdapter *  pAdapterIn)
  {
  listAdapters.PushBack (pAdapterIn);
  };

//-----------------------------------------------------------------------------
VOID Analytics::DeleteAllAdapters (VOID)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listAdapters.Empty ();
  };

//-----------------------------------------------------------------------------
VOID  Analytics::SetUserProperty  (const char *    szKeyIn,
                                   const char *    szValueIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->SetUserProperty (szKeyIn, szValueIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::SetUserId  (const char *    szIDn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->SetUserId (szIDn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::SetCurrentScreen  (const char *    szScreenNameIn,
                                    const char *    szScreenClassIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->SetCurrentScreen (szScreenNameIn, szScreenClassIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::LogEvent  (const char *    szEventNameIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->LogEvent (szEventNameIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::LogEventString  (const char *    szEventNameIn,
                                  const char *    szParamNameIn,
                                  const char *    szValueIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->LogEventString  (szEventNameIn, szParamNameIn, szValueIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::LogEventInt  (const char *    szEventNameIn,
                               const char *    szParamNameIn,
                               const int64_t   iValueIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->LogEventInt  (szEventNameIn, szParamNameIn, iValueIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::LogEventFloat  (const char *    szEventNameIn,
                                 const char *    szParamNameIn,
                                 const double    dValueIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->LogEventFloat (szEventNameIn, szParamNameIn, dValueIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::LogEventWithParams  (const char *    szEventNameIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->LogEventWithParams (szEventNameIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::ClearParams  (VOID)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->ClearParams ();
    };
  };

//-----------------------------------------------------------------------------
INT  Analytics::GetNumParams  (VOID)
  {
  INT  iMaxParams = 0;
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    iMaxParams = RMax ((*itrCurr)->GetNumParams (), iMaxParams);
    };
  return (iMaxParams);
  };

//-----------------------------------------------------------------------------
VOID  Analytics::AddParamString  (const char *    szParamNameIn,
                                  const char *    szValueIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->AddParam (szParamNameIn, szValueIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::AddParamInt  (const char *    szParamNameIn,
                               const int64_t   iValueIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->AddParam (szParamNameIn, iValueIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  Analytics::AddParamDouble  (const char *    szParamNameIn,
                                  const double    dValueIn)
  {
  for (TListItr<AnalyticsAdapter *>  itrCurr = listAdapters.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->AddParam (szParamNameIn, dValueIn);
    };
  };

