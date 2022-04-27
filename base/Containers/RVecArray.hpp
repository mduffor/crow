/* -----------------------------------------------------------------
                          Array of 3D Vectors

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


#ifndef RVECTORARRAY_HPP
#define RVECTORARRAY_HPP

#include "Sys/Types.hpp"
#include "Math/RVec.hpp"
#include "Containers/BaseArray.hpp"


//-----------------------------------------------------------------------------
class RVec3Array : public BaseArray
  {
  private:

  public:

                   RVec3Array      ();
                   RVec3Array      (const RVec3Array &  arrayIn);
   explicit        RVec3Array      (INT   iInitialSize);
                   ~RVec3Array     ();

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
                                     INT      iNumToInit);
    EStatus        Copy             (const RVec3Array &  arraySource);
    EStatus        Set              (const RVec3 &  vecElement, INT  iIndex);
    EStatus        Append           (const RVec3 &  vecElement);
    EStatus        GetArray         (RVec3  []) const;

    RVec3          operator[]       (INT  iIndex) const;
    RVec3 &        operator[]       (INT  iIndex);
    RVec3Array &   operator=        (const  RVec3Array & arrayIn);
    BOOL           operator==       (const  RVec3Array & arrayIn);
    BOOL           operator!=       (const  RVec3Array & arrayIn)  {return (!(*this == arrayIn));};

    VOID           CalcRange        (RVec3 &  vecMinOut,
                                     RVec3 &  vecMaxOut);

  };


//-----------------------------------------------------------------------------
class RVec4Array : public BaseArray
  {
  private:

  public:

                   RVec4Array      ();
                   RVec4Array      (const RVec4Array &  arrayIn);
   explicit        RVec4Array      (INT   iInitialSize);
                   ~RVec4Array     ();

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
    VOID           SwapIndexes      (INT  iIndexOne,
                                     INT  iIndexTwo) override;
    INT            CompareIndexes   (INT  iIndexOne,
                                     INT  iIndexTwo) override;

  public:
    VOID           InitValues       (INT      iStartOffset,
                                     INT      iNumToInit);
    EStatus        Copy             (const RVec4Array &  arraySource);
    EStatus        SetAt            (INT  iIndex, const RVec4 &  vecElement);
    EStatus        Append           (const RVec4 &  vecElement);
    EStatus        GetArray         (RVec4  []) const;

    RVec4 *        GetAt            (INT  iIndex) const;
    RVec4          operator[]       (INT  iIndex) const;
    RVec4 &        operator[]       (INT  iIndex);
    RVec4Array &   operator=        (const  RVec4Array & arrayIn);
    BOOL           operator==       (const  RVec4Array & arrayIn);
    BOOL           operator!=       (const  RVec4Array & arrayIn)  {return (!(*this == arrayIn));};
    VOID           CalcRange        (RVec4 &  vecMinOut,
                                     RVec4 &  vecMaxOut);
  };





#endif // RVECTORARRAY_HPP
