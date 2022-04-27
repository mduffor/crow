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


#include "Debug.hpp"
ASSERTFILE (__FILE__)


#include "Containers/RMatrixArray.hpp"


//-----------------------------------------------------------------------------
//  RMatrixArray
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
RMatrixArray::RMatrixArray  ()
  {
  Init ();
  };


//-----------------------------------------------------------------------------
RMatrixArray::RMatrixArray  (const RMatrixArray &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };


//-----------------------------------------------------------------------------
RMatrixArray::RMatrixArray  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };


//-----------------------------------------------------------------------------
RMatrixArray::~RMatrixArray  ()
  {
  DeleteArrayLocal (&pArray);
  };


//-----------------------------------------------------------------------------
VOID *  RMatrixArray::AllocArray  (INT  iSizeIn)
  {
  return new RMatrix [iSizeIn];
  };


//-----------------------------------------------------------------------------
VOID  RMatrixArray::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  RMatrixArray::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  delete [] ((RMatrix *) *pvArrayIn);
  *pvArrayIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID  RMatrixArray::ZeroValues  (INT    iStartOffset,
                                 INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero);
  };

//-----------------------------------------------------------------------------
VOID  RMatrixArray::InitValues  (INT    iStartOffset,
                                 INT    iNumToInit)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    static_cast<RMatrix *>(pArray) [iIndex].Identity ();
    };
  };


//-----------------------------------------------------------------------------
VOID  RMatrixArray::MoveValues  (PVOID  pvSourceDataIn,
                                 INT    iSourceOffsetIn,
                                 INT    iStartOffsetIn,
                                 INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    static_cast<RMatrix *>(pArray) [iIndex + iStartOffsetIn] = static_cast<RMatrix *>(pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  RMatrixArray::MoveValuesRev  (PVOID  pvSourceDataIn,
                                    INT    iSourceOffsetIn,
                                    INT    iStartOffsetIn,
                                    INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    static_cast<RMatrix *>(pArray) [iStartOffsetIn + iIndex] = static_cast<RMatrix *>(pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  RMatrixArray::AverageValues  (INT    iIndexAverage,
                                    INT    iIndexValueOne,
                                    INT    iIndexValueTwo,
                                    FLOAT  fBias)
  {

  // note:  You can't average matrixes.
  //static_cast<RMatrix *>(pArray) [iIndexAverage] = ( (static_cast<RMatrix *>(pArray) [iIndexValueOne] * fBias) +
  //                                         (static_cast<RMatrix *>(pArray) [iIndexValueTwo] * (1.0f - fBias)) );
  };

//-----------------------------------------------------------------------------
VOID  RMatrixArray::SwapIndexes  (INT  iIndexOne,
                                  INT  iIndexTwo)
  {
  RMatrix                  matTemp = static_cast<RMatrix *>(pArray) [iIndexOne];
  static_cast<RMatrix *>(pArray) [iIndexOne] = static_cast<RMatrix *>(pArray) [iIndexTwo];
  static_cast<RMatrix *>(pArray) [iIndexTwo] = matTemp;
  };


//-----------------------------------------------------------------------------
INT  RMatrixArray::CompareIndexes  (INT  iIndexOne,
                                    INT  iIndexTwo)
  {
  // cannot compare matrixes other than equal
  if (static_cast<RMatrix *>(pArray) [iIndexOne] == static_cast<RMatrix *>(pArray) [iIndexTwo]) return (0);
  return (-1);
  };





//-----------------------------------------------------------------------------
EStatus RMatrixArray::SetAt (INT               iIndex,
                             const RMatrix &   matElement)
  {
  if (iIndex >= iLength) return (EStatus::kFailure);
  static_cast<RMatrix *>(pArray) [iIndex] = matElement;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus RMatrixArray::Append (const RMatrix &  matElement)
  {
  INT  iOldLength = iLength;

  if (SetLength (iOldLength + 1) == EStatus::kFailure) {return EStatus::kFailure;};

  static_cast<RMatrix *>(pArray) [iOldLength] = matElement;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus RMatrixArray::GetArray (RMatrix  aOut []) const
  {
  if (iLength == 0) return (EStatus::kFailure);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    aOut [iIndex] = static_cast<RMatrix *>(pArray) [iIndex];
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus RMatrixArray::Copy (const RMatrixArray &  arraySource)
  {
  if (SetLength (arraySource.iLength) == EStatus::kFailure) {return EStatus::kFailure;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
RMatrix RMatrixArray::operator[]  (INT  iIndex) const
  {
  return static_cast<RMatrix *>(pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
RMatrix &  RMatrixArray::operator[]  (INT  iIndex)
  {
  return static_cast<RMatrix *>(pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
RMatrixArray &  RMatrixArray::operator=  (const RMatrixArray &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };


//-----------------------------------------------------------------------------
BOOL  RMatrixArray::operator==  (const RMatrixArray &  arrayIn)
  {
  if (iLength == 0) return (false);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (arrayIn [iIndex] != static_cast<RMatrix *>(pArray) [iIndex])
      {
      return (false);
      };
    };
  return (true);
  };

