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

#ifndef ANALYTICS_HPP
#define ANALYTICS_HPP

#include "Sys/Types.hpp"
#include "Net/AnalyticsAdapter.hpp"
#include "Containers/TList.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"

//------------------------------------------------------------------------------
class Analytics
  {
  private:

    static TList<AnalyticsAdapter *>  listAdapters;

    DBG(static ValueRegistrySimple    registryParams);


  public:

            Analytics          ();

            ~Analytics         ();

                   // NOTE:  Registered adapters are the property of Analytics and will
                   //         be deleted by it.
    static VOID    RegisterAdapter    (AnalyticsAdapter *  pAdapterIn);

    static VOID    DeleteAllAdapters  (VOID);

    static VOID    SetUserProperty    (const char *    szKeyIn,
                                       const char *    szValueIn);

    static VOID    SetUserId          (const char *    szIDn);

    static VOID    SetCurrentScreen   (const char *    szScreenNameIn,
                                       const char *    szScreenClassIn);

    static VOID    LogEvent           (const char *    szEventNameIn);

    static VOID    LogEventString     (const char *    szEventNameIn,
                                       const char *    szParamNameIn,
                                       const char *    szValueIn);

    static VOID    LogEventInt        (const char *    szEventNameIn,
                                       const char *    szParamNameIn,
                                       const int64_t   iValueIn);

    static VOID    LogEventFloat      (const char *    szEventNameIn,
                                       const char *    szParamNameIn,
                                       const double    dValueIn);

    static VOID    LogEventWithParams (const char *    szEventNameIn);

    static VOID    ClearParams        (VOID);

    static INT     GetNumParams       (VOID);

    static VOID    AddParamString     (const char *    szParamNameIn,
                                       const char *    szValueIn);

    static VOID    AddParamInt        (const char *    szParamNameIn,
                                       const int64_t   iValueIn);

    static VOID    AddParamDouble     (const char *    szParamNameIn,
                                       const double    dValueIn);


    };



#endif // ANALYTICS_HPP