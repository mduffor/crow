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


#include "Debug.hpp"
ASSERTFILE (__FILE__)


#include "Containers/RVecArray.hpp"

//-----------------------------------------------------------------------------
//  RVec3Array
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
RVec3Array::RVec3Array  ()
  {
  Init ();
  };


//-----------------------------------------------------------------------------
RVec3Array::RVec3Array  (const RVec3Array &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };


//-----------------------------------------------------------------------------
RVec3Array::RVec3Array  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };


//-----------------------------------------------------------------------------
RVec3Array::~RVec3Array  ()
  {
  DeleteArrayLocal (&pArray);
  };


//-----------------------------------------------------------------------------
VOID *  RVec3Array::AllocArray  (INT  iSizeIn)
  {
  return new RVec3 [iSizeIn];
  };


//-----------------------------------------------------------------------------
VOID  RVec3Array::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  RVec3Array::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  delete [] ((RVec3 *) *pvArrayIn);
  *pvArrayIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID  RVec3Array::ZeroValues  (INT    iStartOffset,
                               INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero);
  };

//-----------------------------------------------------------------------------
VOID  RVec3Array::InitValues  (INT    iStartOffset,
                               INT    iNumToInit)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    static_cast<RVec3 *>(pArray) [iIndex].Zero ();
    };
  };


//-----------------------------------------------------------------------------
VOID  RVec3Array::MoveValues  (PVOID  pvSourceDataIn,
                               INT    iSourceOffsetIn,
                               INT    iStartOffsetIn,
                               INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    static_cast<RVec3 *>(pArray) [iIndex + iStartOffsetIn] = static_cast<RVec3 *>(pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  RVec3Array::MoveValuesRev  (PVOID  pvSourceDataIn,
                                  INT    iSourceOffsetIn,
                                  INT    iStartOffsetIn,
                                  INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    static_cast<RVec3 *>(pArray) [iStartOffsetIn + iIndex] = static_cast<RVec3 *>(pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  RVec3Array::AverageValues  (INT    iIndexAverage,
                                  INT    iIndexValueOne,
                                  INT    iIndexValueTwo,
                                  FLOAT  fBias)
  {


  static_cast<RVec3 *>(pArray) [iIndexAverage] = ( (static_cast<RVec3 *>(pArray) [iIndexValueOne] * fBias) +
                                         (static_cast<RVec3 *>(pArray) [iIndexValueTwo] * (1.0f - fBias)) );
  };


//-----------------------------------------------------------------------------
VOID  RVec3Array::SwapIndexes  (INT  iIndexOne,
                                INT  iIndexTwo)
  {
  RVec3                  vecTemp = static_cast<RVec3 *>(pArray) [iIndexOne];
  static_cast<RVec3 *>(pArray) [iIndexOne] = static_cast<RVec3 *>(pArray) [iIndexTwo];
  static_cast<RVec3 *>(pArray) [iIndexTwo] = vecTemp;
  };


//-----------------------------------------------------------------------------
INT  RVec3Array::CompareIndexes  (INT  iIndexOne,
                                  INT  iIndexTwo)
  {
  if (static_cast<RVec3 *>(pArray) [iIndexOne] == static_cast<RVec3 *>(pArray) [iIndexTwo]) return (0);
  if (static_cast<RVec3 *>(pArray) [iIndexOne].LengthSquared () <  static_cast<RVec3 *>(pArray) [iIndexTwo].LengthSquared ()) return (-1);
  return (1);
  };


//-----------------------------------------------------------------------------
EStatus RVec3Array::Set (const RVec3 &  vecElement,
                         INT            iIndex)
  {
  if (iIndex >= iLength) return (EStatus::kFailure);
  static_cast<RVec3 *>(pArray) [iIndex] = vecElement;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus RVec3Array::Append (const RVec3 &  vecElement)
  {
  INT  iOldLength = iLength;

  if (SetLength (iOldLength + 1) == EStatus::kFailure) {return EStatus::kFailure;};

  static_cast<RVec3 *>(pArray) [iOldLength] = vecElement;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus RVec3Array::GetArray (RVec3  aOut []) const
  {
  if (iLength == 0) return (EStatus::kFailure);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    aOut [iIndex] = static_cast<RVec3 *>(pArray) [iIndex];
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus RVec3Array::Copy (const RVec3Array &  arraySource)
  {
  if (SetLength (arraySource.iLength) == EStatus::kFailure) {return EStatus::kFailure;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
RVec3 RVec3Array::operator[]  (INT  iIndex) const
  {
  return static_cast<RVec3 *>(pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
RVec3 &  RVec3Array::operator[]  (INT  iIndex)
  {
  return static_cast<RVec3 *>(pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
RVec3Array &  RVec3Array::operator=  (const RVec3Array &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };

//-----------------------------------------------------------------------------
BOOL  RVec3Array::operator==  (const RVec3Array &  arrayIn)
  {
  if (iLength == 0) return (false);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (arrayIn [iIndex] != static_cast<RVec3 *>(pArray) [iIndex])
      {
      return (false);
      };
    };
  return (true);
  };


//-----------------------------------------------------------------------------
VOID  RVec3Array::CalcRange (RVec3 &  vecMinOut,
                             RVec3 &  vecMaxOut)
  {
  if (iLength == 0)
    {
    vecMinOut = vecMaxOut = RVec3::kZero;
    return;
    };

  vecMinOut = vecMaxOut = static_cast<RVec3 *>(pArray)[0];

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    vecMinOut.fX = RMin (vecMinOut.fX, static_cast<RVec3 *>(pArray)[iIndex].fX);
    vecMinOut.fY = RMin (vecMinOut.fY, static_cast<RVec3 *>(pArray)[iIndex].fY);
    vecMinOut.fZ = RMin (vecMinOut.fZ, static_cast<RVec3 *>(pArray)[iIndex].fZ);

    vecMaxOut.fX = RMax (vecMaxOut.fX, static_cast<RVec3 *>(pArray)[iIndex].fX);
    vecMaxOut.fY = RMax (vecMaxOut.fY, static_cast<RVec3 *>(pArray)[iIndex].fY);
    vecMaxOut.fZ = RMax (vecMaxOut.fZ, static_cast<RVec3 *>(pArray)[iIndex].fZ);
    };
  };



//-----------------------------------------------------------------------------
//  RVec4Array
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
RVec4Array::RVec4Array  ()
  {
  Init ();
  };


//-----------------------------------------------------------------------------
RVec4Array::RVec4Array  (const RVec4Array &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };


//-----------------------------------------------------------------------------
RVec4Array::RVec4Array  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };


//-----------------------------------------------------------------------------
RVec4Array::~RVec4Array  ()
  {
  DeleteArrayLocal (&pArray);
  };


//-----------------------------------------------------------------------------
VOID *  RVec4Array::AllocArray  (INT  iSizeIn)
  {
  return new RVec4 [iSizeIn];
  };


//-----------------------------------------------------------------------------
VOID  RVec4Array::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  RVec4Array::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  delete [] ((RVec4 *) *pvArrayIn);
  *pvArrayIn = NULL;
  };


//-----------------------------------------------------------------------------
VOID  RVec4Array::ZeroValues  (INT    iStartOffset,
                               INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero);
  };

//-----------------------------------------------------------------------------
VOID  RVec4Array::InitValues  (INT    iStartOffset,
                               INT    iNumToInit)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    static_cast<RVec4 *>(pArray) [iIndex].Zero ();
    };
  };


//-----------------------------------------------------------------------------
VOID  RVec4Array::MoveValues  (PVOID  pvSourceDataIn,
                               INT    iSourceOffsetIn,
                               INT    iStartOffsetIn,
                               INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    static_cast<RVec4 *>(pArray) [iIndex + iStartOffsetIn] = static_cast<RVec4 *>(pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  RVec4Array::MoveValuesRev  (PVOID  pvSourceDataIn,
                                  INT    iSourceOffsetIn,
                                  INT    iStartOffsetIn,
                                  INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    printf ("Vec4:CVR %d = %d\n",  iStartOffsetIn + iIndex, iIndex + iSourceOffsetIn);
    static_cast<RVec4 *>(pArray) [iStartOffsetIn + iIndex].Set (static_cast<RVec4 *>(pvSourceDataIn) [iIndex + iSourceOffsetIn]);
    };
  };


//-----------------------------------------------------------------------------
VOID  RVec4Array::AverageValues  (INT    iIndexAverage,
                                  INT    iIndexValueOne,
                                  INT    iIndexValueTwo,
                                  FLOAT  fBias)
  {


  static_cast<RVec4 *>(pArray) [iIndexAverage] = ( (static_cast<RVec4 *>(pArray) [iIndexValueOne] * fBias) +
                                         (static_cast<RVec4 *>(pArray) [iIndexValueTwo] * (1.0f - fBias)) );
  };


//-----------------------------------------------------------------------------
VOID  RVec4Array::SwapIndexes  (INT  iIndexOne,
                                INT  iIndexTwo)
  {
  RVec4                  vecTemp = static_cast<RVec4 *>(pArray) [iIndexOne];
  static_cast<RVec4 *>(pArray) [iIndexOne] = static_cast<RVec4 *>(pArray) [iIndexTwo];
  static_cast<RVec4 *>(pArray) [iIndexTwo] = vecTemp;
  };


//-----------------------------------------------------------------------------
INT  RVec4Array::CompareIndexes  (INT  iIndexOne,
                                  INT  iIndexTwo)
  {
  if (static_cast<RVec4 *>(pArray) [iIndexOne] == static_cast<RVec4 *>(pArray) [iIndexTwo]) return (0);
  if (static_cast<RVec4 *>(pArray) [iIndexOne].LengthSquared () <  static_cast<RVec4 *>(pArray) [iIndexTwo].LengthSquared ()) return (-1);
  return (1);
  };


//-----------------------------------------------------------------------------
EStatus RVec4Array::SetAt (INT            iIndex,
                           const RVec4 &  vecElement)
  {
  if (iIndex >= iLength) return (EStatus::kFailure);
  static_cast<RVec4 *>(pArray) [iIndex] = vecElement;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus RVec4Array::Append (const RVec4 &  vecElement)
  {
  INT  iOldLength = iLength;

  if (SetLength (iOldLength + 1) == EStatus::kFailure) {return EStatus::kFailure;};

  static_cast<RVec4 *>(pArray) [iOldLength] = vecElement;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus RVec4Array::GetArray (RVec4  aOut []) const
  {
  if (iLength == 0) return (EStatus::kFailure);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    aOut [iIndex] = static_cast<RVec4 *>(pArray) [iIndex];
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus RVec4Array::Copy (const RVec4Array &  arraySource)
  {
  if (SetLength (arraySource.iLength) == EStatus::kFailure) {return EStatus::kFailure;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
RVec4 *  RVec4Array::GetAt (INT  iIndex) const
  {
  return (&static_cast<RVec4 *>(pArray) [iIndex]);
  };


//-----------------------------------------------------------------------------
RVec4 RVec4Array::operator[]  (INT  iIndex) const
  {
  return static_cast<RVec4 *>(pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
RVec4 &  RVec4Array::operator[]  (INT  iIndex)
  {
  return static_cast<RVec4 *>(pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
RVec4Array &  RVec4Array::operator=  (const RVec4Array &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };


//-----------------------------------------------------------------------------
BOOL  RVec4Array::operator==  (const RVec4Array &  arrayIn)
  {
  if (iLength == 0) return (false);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (arrayIn [iIndex] != static_cast<RVec4 *>(pArray) [iIndex])
      {
      return (false);
      };
    };
  return (true);
  };

//-----------------------------------------------------------------------------
VOID  RVec4Array::CalcRange (RVec4 &  vecMinOut,
                             RVec4 &  vecMaxOut)
  {
  if (iLength == 0)
    {
    vecMinOut = vecMaxOut = RVec4::kZero;
    return;
    };

  vecMinOut = vecMaxOut = static_cast<RVec3 *>(pArray)[0];

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    vecMinOut.fX = RMin (vecMinOut.fX, static_cast<RVec4 *>(pArray)[iIndex].fX);
    vecMinOut.fY = RMin (vecMinOut.fY, static_cast<RVec4 *>(pArray)[iIndex].fY);
    vecMinOut.fZ = RMin (vecMinOut.fZ, static_cast<RVec4 *>(pArray)[iIndex].fZ);
    vecMinOut.fW = RMin (vecMinOut.fW, static_cast<RVec4 *>(pArray)[iIndex].fW);

    vecMaxOut.fX = RMax (vecMaxOut.fX, static_cast<RVec4 *>(pArray)[iIndex].fX);
    vecMaxOut.fY = RMax (vecMaxOut.fY, static_cast<RVec4 *>(pArray)[iIndex].fY);
    vecMaxOut.fZ = RMax (vecMaxOut.fZ, static_cast<RVec4 *>(pArray)[iIndex].fZ);
    vecMaxOut.fW = RMax (vecMaxOut.fW, static_cast<RVec4 *>(pArray)[iIndex].fW);
    };
  };


