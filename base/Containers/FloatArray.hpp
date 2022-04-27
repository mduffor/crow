/* -----------------------------------------------------------------
                            Float Array

     This module implements an array of floats.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2002-2014, Michael T. Duffy II.  All rights reserved.
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

#ifndef FLOATARRAY_HPP
#define FLOATARRAY_HPP

#include "Sys/Types.hpp"
#include "Containers/BaseArray.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


//-----------------------------------------------------------------------------
class FloatArray : public BaseArray
  {
  private:

  public:

                   FloatArray       ();
                   FloatArray       (const FloatArray &  arrayIn);
   explicit        FloatArray       (INT   iInitialSize);
                   ~FloatArray      ();

  protected:
    VOID *         AllocArray       (INT      iSizeIn) override;
    VOID           DeleteArray      (PVOID *  pvArrayIn) override;
    VOID           DeleteArrayLocal (PVOID *  pvArrayIn);
    VOID           ZeroValues       (INT      iStartOffset,
                                     INT      iNumToZero) override;
    VOID           MoveValues       (PVOID    pvSourceData,
                                     INT      iSourceOffsetIn,
                                     INT      iStartOffset,
                                     INT      iNumToCopy) override;
    VOID           MoveValuesRev    (PVOID    pvData,
                                     INT      iSourceOffsetIn,
                                     INT      iStartOffset,
                                     INT      iNumToCopy) override;
    VOID           AverageValues    (INT      iIndexAverage,
                                     INT      iIndexValueOne,
                                     INT      iIndexValueTwo,
                                     FLOAT    fBias = 0.5f) override;
  public:
    VOID           SwapIndexes      (INT  iIndexOne,
                                     INT  iIndexTwo) override;
    INT            CompareIndexes   (INT  iIndexOne,
                                     INT  iIndexTwo) override;

    VOID           InitValues       (INT      iStartOffset,
                                     INT      iNumToInit,
                                     FLOAT    fValue = 0.0f);


    EStatus        Copy             (const  FloatArray &  arraySource);
    EStatus        SetAt            (INT  iIndex, FLOAT  fElement);
    EStatus        Append           (FLOAT  fElement);
    EStatus        GetArray         (FLOAT  []) const;
    FLOAT *        GetRawArray      (VOID) const  {return (FLOAT *)pArray;};

    FLOAT          operator[]       (INT  iIndex) const;
    FLOAT &        operator[]       (INT  iIndex);
    FLOAT          GetAt            (INT  iIndex) const {return (*this)[iIndex];};
    FloatArray &   operator=        (const  FloatArray & arrayIn);
    VOID           Set              (const  FloatArray & arrayIn)  {*this = arrayIn;};
    VOID           Set              (const  FLOAT * pfIn,
                                     INT            iCountIn);
    BOOL           operator==       (const  FloatArray & arrayIn);
    BOOL           operator!=       (const  FloatArray & arrayIn)  {return (!(*this == arrayIn));};

    VOID           DebugPrint       (UINT  uCount);

    VOID           CalcRange        (FLOAT &  fMinOut,
                                     FLOAT &  fMaxOut);

  };



#endif // FloatARRAY_HPP


