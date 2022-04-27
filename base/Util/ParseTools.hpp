/* -----------------------------------------------------------------
                             Parse Tools

     This module contains tools and utilities for in-place parsing
   of buffers in various formats.

   ----------------------------------------------------------------- */

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

#ifndef PARSETOOLS_HPP
#define PARSETOOLS_HPP


#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"

//#include "Containers/BaseArray.hpp"
#include "Containers/IntArray.hpp"
#include "Containers/FloatArray.hpp"
#include "Containers/DoubleArray.hpp"
#include "Containers/RStrArray.hpp"
#include "Containers/KVPArray.hpp"

/**
  @addtogroup base
  @{
*/
//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------



//------------------------------------------------------------------------
class ParseTools
  {
  public:


  public:

                    ParseTools     (VOID);

    static BOOL     IsKVPColon     (RStrParser &    parserBufferIn,
                                    RStr &          strKeyWordOut,
                                    RStrParser &    parserValueOut);

    static BOOL     IsKVPBracket   (RStrParser &    parserBufferIn,
                                    RStr &          strKeyOut,
                                    RStrParser &    parserValueOut);

    static BOOL     IsKVPEquals    (RStrParser &    parserBufferIn,
                                    RStr &          strKeyOut,
                                    RStrParser &    parserValueOut);

    static EStatus  GetKey         (RStrParser &  parserIn,
                                    UINT32        uSeparatorIn,
                                    RStr &        strKeyOut);

    static VOID     GetIntArray    (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    IntArray &    arrayOut);

    static VOID     GetFloatArray  (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    FloatArray &  arrayOut);

    static VOID     GetDoubleArray (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    DoubleArray & arrayOut);

    static VOID     GetStringArray (RStrParser &  parserIn,
                                    UINT32        uBeginningCharIn,
                                    UINT32        uEndingCharIn,
                                    RStrArray &   arrayOut);



                                   // returns True if char was skipped, false if not present.
    static BOOL     SkipChar       (RStrParser &  parserIn,
                                    UINT32        uCharIn);


  };
/** @} */ // end of base group


#endif // PARSETOOLS_HPP











