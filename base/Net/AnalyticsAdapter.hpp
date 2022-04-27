/* -----------------------------------------------------------------
                         Analytics Adapter

     This module is the base class for the adapters in the analytics
   system.

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

#ifndef ANALYTICSADAPTER_HPP
#define ANALYTICSADAPTER_HPP

#include "Sys/Types.hpp"


//------------------------------------------------------------------------------
class AnalyticsAdapter
  {

  public:

                    AnalyticsAdapter   () {};

    virtual         ~AnalyticsAdapter  () {};

    virtual VOID    SetUserProperty    (const char *    szKeyIn,
                                        const char *  szValueIn) = 0;

    virtual VOID    SetUserId          (const char *    szIDIn) = 0;

    virtual VOID    SetCurrentScreen   (const char *    szScreenNameIn,
                                        const char *    szScreenClassIn) = 0;

    virtual VOID    LogEvent           (const char *    szEventNameIn) = 0;

    virtual VOID    LogEventString     (const char *    szEventNameIn,
                                        const char *    szParamNameIn,
                                        const char *    szValueIn) = 0;

    virtual VOID    LogEventInt        (const char *    szEventNameIn,
                                        const char *    szParamNameIn,
                                        const int64_t   iValueIn) = 0;

    virtual VOID    LogEventFloat      (const char *    szEventNameIn,
                                        const char *    szParamNameIn,
                                        const double    dValueIn) = 0;

    virtual VOID    LogEventWithParams (const char *    szEventNameIn) = 0;

    virtual VOID    ClearParams        (VOID) = 0;

    virtual INT     GetNumParams       (VOID) = 0;

    virtual VOID    AddParam           (const char *    szParamNameIn,
                                        const char *    szValueIn) = 0;

    virtual VOID    AddParam           (const char *    szParamNameIn,
                                        const int64_t   iValueIn) = 0;

    virtual VOID    AddParam           (const char *    szParamNameIn,
                                        const double    dValueIn) = 0;


  };



#endif // ANALYTICSADAPTER_HPP