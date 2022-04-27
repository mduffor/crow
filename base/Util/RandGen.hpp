/* -----------------------------------------------------------------
                             RandGen

     The RandGen class is the base interface for random number generator
   algorithms to implement to interface with Random.

   ----------------------------------------------------------------- */

// Modified BSD License:
//
// Copyright (c) 2020, Michael T. Duffy II.  All rights reserved.
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

#ifndef RANDGEN_HPP
#define RANDGEN_HPP

#include "Sys/Types.hpp"
#include "Util/RStrParser.hpp"



//------------------------------------------------------------------------
class RandGen
  {
  public:

    virtual INT32   Int32         (VOID) = 0;

    virtual UINT32  UInt32        (VOID) = 0;

    virtual FLOAT   Float01       (VOID) = 0;

    virtual FLOAT   FloatOC       (VOID) = 0;

    virtual FLOAT   FloatOO       (VOID) = 0;

    virtual DOUBLE  Double        (VOID) = 0;

    virtual INT     IntRange      (INT  iMinInclusive,
                                   INT  iMaxInclusive) = 0;

    virtual FLOAT   FloatRange    (FLOAT  fMinInclusive,
                                   FLOAT  fMaxExclusive) = 0;

    virtual VOID    Serialize     (RStrParser &  parserOut) = 0;

    virtual VOID    Deserialize   (RStrParser &  parserIn) = 0;

  };


#endif // RANDGEN_HPP
