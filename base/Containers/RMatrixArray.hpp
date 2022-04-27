/* -----------------------------------------------------------------
                          Array of 3D Matrices

     This module implements an array of matrices.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2004-2014, Michael T. Duffy II.  All rights reserved.
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


#ifndef RMATRIXARRAY_HPP
#define RMATRIXARRAY_HPP

#include "Sys/Types.hpp"
#include "Math/RMatrix.hpp"
#include "Containers/BaseArray.hpp"

//-----------------------------------------------------------------------------
class RMatrixArray : public BaseArray
  {
  private:

  public:

                    RMatrixArray      ();
                    RMatrixArray      (const RMatrixArray &  arrayIn);
   explicit         RMatrixArray      (INT   iInitialSize);
                    ~RMatrixArray     ();

  protected:
    VOID *          AllocArray       (INT      iSizeIn) override;
    VOID            DeleteArray      (PVOID *  pvArrayIn) override;
    VOID            DeleteArrayLocal (PVOID *  pvArrayIn);
    VOID            ZeroValues       (INT      iStartOffset,
                                      INT      iNumToZero) override;
    VOID            MoveValues       (PVOID    pvSourceData,
                                      INT      iSourceOffsetIn,
                                      INT      iStartOffset,
                                      INT      iNumToCopy) override;
    VOID            MoveValuesRev    (PVOID    pvData,
                                      INT      iSourceOffsetIn,
                                      INT      iStartOffset,
                                      INT      iNumToCopy) override;
    VOID            AverageValues    (INT      iIndexAverage,
                                      INT      iIndexValueOne,
                                      INT      iIndexValueTwo,
                                      FLOAT    fBias = 0.5f) override;
  public:
    VOID            SwapIndexes      (INT  iIndexOne,
                                      INT  iIndexTwo) override;
    INT             CompareIndexes   (INT  iIndexOne,
                                      INT  iIndexTwo) override;

    VOID            InitValues       (INT      iStartOffset,
                                      INT      iNumToInit);
    EStatus         Copy             (const RMatrixArray &  arraySource);
    EStatus         SetAt            (INT  iIndex, const RMatrix &  matElement);
    EStatus         Append           (const RMatrix &  matElement);
    EStatus         GetArray         (RMatrix  []) const;

    RMatrix         operator[]       (INT  iIndex) const;
    RMatrix &       operator[]       (INT  iIndex);
    RMatrixArray &  operator=        (const  RMatrixArray & arrayIn);
    BOOL            operator==       (const  RMatrixArray & arrayIn);
    BOOL            operator!=       (const  RMatrixArray & arrayIn)  {return (!(*this == arrayIn));};
  };

#endif // RMATRIXARRAY_HPP

