/* -----------------------------------------------------------------
                        Key Value Pair Array

    This module implements an array of Key-Value pairs.

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2016, Michael T. Duffy II.  All rights reserved.
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


#ifndef KVPARRAY_HPP
#define KVPARRAY_HPP

// NOTE:  It might make more sense to implement this as a list instead of an array.

#include <ctype.h>
#include <stdarg.h>   // for variable argument functions (va_*)
#include "Sys/Types.hpp"
#include "Containers/BaseArray.hpp"
#include "Containers/IntArray.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"

//-----------------------------------------------------------------------------
struct KVPEntry
  {
  RStr  strKey;
  RStr  strValue;
  };

/// An array of KVP instances
//-----------------------------------------------------------------------------
class KVPArray : public BaseArray
  {
  private:

  public:

                 KVPArray      ();
                 KVPArray      (const KVPArray &  arrayIn);
   explicit      KVPArray      (INT   iInitialSize);
                 ~KVPArray     ();

  protected:
    VOID *       AllocArray     (INT      iSizeIn) override;
    VOID         DeleteArray    (PVOID *  pvArrayIn) override;
    VOID         DeleteArrayLocal (PVOID *  pvArrayIn);
    VOID         ZeroValues     (INT      iStartOffset,
                                 INT      iNumToZero) override;
    VOID         MoveValues     (PVOID    pvSourceData,
                                 INT      iSourceOffsetIn,
                                 INT      iStartOffset,
                                 INT      iNumToCopy) override;
    VOID         MoveValuesRev  (PVOID    pvSourceData,
                                 INT      iSourceOffsetIn,
                                 INT      iStartOffset,
                                 INT      iNumToCopy) override;
    VOID         AverageValues  (INT      iIndexAverage,
                                 INT      iIndexValueOne,
                                 INT      iIndexValueTwo,
                                 FLOAT    fBias = 0.5f) override;
  public:
    VOID         SwapIndexes    (INT  iIndexOne,
                                 INT  iIndexTwo) override;
    INT          CompareIndexes (INT  iIndexOne,
                                 INT  iIndexTwo) override;
    VOID         InitValues     (INT      iStartOffset,
                                 INT      iNumToInit);
    INT          FindKey        (const char *  szKeyIn) const;

    EStatus      Copy           (const KVPArray &  arraySource);
    EStatus      SetAt          (const char *   szKey,
                                 const char *   szValue);
    EStatus      Append         (const char *   szKey,
                                 const char *   szValue);

    RStr         operator[]     (const char *  szKeyIn) const;
    RStr &       operator[]     (const char *  szKeyIn);
    KVPArray &   operator=      (const  KVPArray & arrayIn);
    BOOL         operator==     (const  KVPArray & arrayIn);
    BOOL         operator!=     (const  KVPArray & arrayIn)   {return (!(*this == arrayIn));};

    const RStr &       KeyAtIndex     (INT  iIndexIn) const;
    const RStr &       ValueAtIndex   (INT  iIndexIn) const;

    const char *       ExpandVars     (RStrParser &  parserIn) const;

    const char *       ExpandMacros   (const char *  szSourceIn,
                                       RStrParser &  parserOut) const;

    const char *       FindReplace    (RStrParser &  parserIn) const;

    VOID               DebugPrint     (VOID);
  };




#endif // KVPARRAY_HPP
