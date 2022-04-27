/* -----------------------------------------------------------------
                            Int Array

     This module implements an array of integers.

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

#ifndef INTARRAY_HPP
#define INTARRAY_HPP

#include "Sys/Types.hpp"
#include "Containers/BaseArray.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


//-----------------------------------------------------------------------------
class IntArray : public BaseArray
  {
  private:

  public:

                   IntArray      ();
                   IntArray      (const IntArray &  arrayIn);
   explicit        IntArray      (INT   iInitialSize);
                   ~IntArray     ();

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
                                     INT      iValue = 0);
    EStatus        Copy             (const  IntArray &  arraySource);
    EStatus        SetAt            (INT  iIndex, INT32  iElement);
    VOID           SetFast          (INT32   iElement,
                                     INT     iIndex)      {((INT32 *) pArray) [iIndex] = iElement;}; ///< Set with no bounds checking
    EStatus        Append           (INT32  iElement);
    EStatus        GetArray         (INT32  []) const;
    INT32 *        GetRawArray      (VOID) const          {return (INT32 *)pArray;};

    INT32          operator[]       (INT  iIndex) const;
    INT32 &        operator[]       (INT  iIndex);
    INT32          GetAt            (INT  iIndex) const {return (*this)[iIndex];};
    IntArray &     operator=        (const  IntArray & arrayIn);
    VOID           Set              (const  IntArray & arrayIn)  {*this = arrayIn;};
    BOOL           operator==       (const  IntArray & arrayIn);
    BOOL           operator!=       (const  IntArray & arrayIn)  {return (!(*this == arrayIn));};

    INT            Find             (INT32  iSearchIn);
    VOID           DebugPrint       (UINT  uCount);

    VOID           CalcRange        (INT &  iMinOut,
                                     INT &  iMaxOut);

  };

// NOTE:  SortIndexes () will work on any of the BaseArray derivatives that
//         implement CompareIndexes () and SwapIndexes ()
VOID  SortIndexes (BaseArray &  arrayToSortIn,
                   IntArray &   arrayOut);



#endif // INTARRAY_HPP


