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

#include "Containers/FloatArray.hpp"


//-----------------------------------------------------------------------------
//  FloatArray
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FloatArray::FloatArray  ()
  {
  Init ();
  };


//-----------------------------------------------------------------------------
FloatArray::FloatArray  (const FloatArray &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };


//-----------------------------------------------------------------------------
FloatArray::FloatArray  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };


//-----------------------------------------------------------------------------
FloatArray::~FloatArray  ()
  {
  DeleteArrayLocal (&pArray);
  };


//-----------------------------------------------------------------------------
VOID *  FloatArray::AllocArray  (INT  iSizeIn)
  {
  return new FLOAT [iSizeIn];
  };


//-----------------------------------------------------------------------------
VOID  FloatArray::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  FloatArray::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  delete [] ((FLOAT *) *pvArrayIn);
  *pvArrayIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID  FloatArray::ZeroValues  (INT    iStartOffset,
                               INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero, 0.0f);
  };

//-----------------------------------------------------------------------------
VOID  FloatArray::InitValues  (INT    iStartOffset,
                               INT    iNumToInit,
                               FLOAT  fValue)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    ((FLOAT *) pArray) [iIndex] = fValue;
    };
  };


//-----------------------------------------------------------------------------
VOID  FloatArray::MoveValues  (PVOID  pvSourceDataIn,
                               INT    iSourceOffsetIn,
                               INT    iStartOffsetIn,
                               INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    ((FLOAT *) pArray) [iIndex + iStartOffsetIn] = ((FLOAT *) pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  FloatArray::MoveValuesRev  (PVOID  pvSourceDataIn,
                                  INT    iSourceOffsetIn,
                                  INT    iStartOffsetIn,
                                  INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    ((FLOAT *) pArray) [iStartOffsetIn + iIndex] = ((FLOAT *) pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  FloatArray::AverageValues  (INT    iIndexAverage,
                                  INT    iIndexValueOne,
                                  INT    iIndexValueTwo,
                                  FLOAT  fBias)
  {


  ((FLOAT *) pArray) [iIndexAverage] = ( (((FLOAT *) pArray) [iIndexValueOne] * fBias) +
                                         (((FLOAT *) pArray) [iIndexValueTwo] * (1.0f - fBias)) );
  };


//-----------------------------------------------------------------------------
VOID  FloatArray::SwapIndexes  (INT  iIndexOne,
                                INT  iIndexTwo)
  {
  FLOAT                    iTemp = ((FLOAT *) pArray) [iIndexOne];
  ((FLOAT *) pArray) [iIndexOne] = ((FLOAT *) pArray) [iIndexTwo];
  ((FLOAT *) pArray) [iIndexTwo] = iTemp;
  };


//-----------------------------------------------------------------------------
INT  FloatArray::CompareIndexes  (INT  iIndexOne,
                                  INT  iIndexTwo)
  {
  if (((FLOAT *) pArray) [iIndexOne] == ((FLOAT *) pArray) [iIndexTwo]) return (0);
  if (((FLOAT *) pArray) [iIndexOne] <  ((FLOAT *) pArray) [iIndexTwo]) return (-1);
  return (1);
  };


//-----------------------------------------------------------------------------
EStatus FloatArray::SetAt (INT     iIndex,
                           FLOAT   fElement)
  {
  if (iIndex >= iLength) return (EStatus::kFailure);
  ((FLOAT *) pArray) [iIndex] = fElement;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus FloatArray::Append (FLOAT  fElement)
  {
  INT  iOldLength = iLength;

  if (SetLength (iOldLength + 1) == EStatus::kFailure) {return EStatus::kFailure;};

  ((FLOAT *) pArray) [iOldLength] = fElement;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus FloatArray::GetArray (FLOAT  aOut []) const
  {
  if (iLength == 0) return (EStatus::kFailure);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    aOut [iIndex] = ((FLOAT *) pArray) [iIndex];
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus FloatArray::Copy (const FloatArray &  arraySource)
  {
  if (SetLength (arraySource.iLength) == EStatus::kFailure) {return EStatus::kFailure;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
FLOAT FloatArray::operator[]  (INT  iIndex) const
  {
  if (! BoundsCheck (iIndex)) return (0);
  return ((FLOAT *) pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
FLOAT &  FloatArray::operator[]  (INT  iIndex)
  {

  if (! BoundsCheck (iIndex))
    {
    static FLOAT  iZero = 0;
    iZero = 0.0f;
    return (iZero);
    };
  return ((FLOAT *) pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
FloatArray &  FloatArray::operator=  (const FloatArray &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };


//-----------------------------------------------------------------------------
VOID  FloatArray::Set  (const  FLOAT * pfIn,
                        INT            iCountIn)
  {
  SetMinLength (iCountIn);
  for (INT iIndex = 0; iIndex < iCountIn; ++iIndex)
    {
    ((FLOAT *) pArray)[iIndex] = pfIn[iIndex];
    };
  };


//-----------------------------------------------------------------------------
BOOL  FloatArray::operator==  (const FloatArray &  arrayIn)
  {
  if (iLength == 0) return (false);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (arrayIn [iIndex] != ((FLOAT *) pArray) [iIndex])
      {
      return (false);
      };
    };
  return (true);
  };


//-----------------------------------------------------------------------------
VOID  FloatArray::DebugPrint  (UINT  uCount)
  {
  INT     iIndex = 0;

  while (iIndex < (INT) uCount)
    {
    if (iIndex < iLength) {printf ("%f ", ((FLOAT *) pArray)[iIndex]); ++iIndex;};
    if (iIndex < iLength) {printf ("%f ", ((FLOAT *) pArray)[iIndex]); ++iIndex;};
    if (iIndex < iLength) {printf ("%f ", ((FLOAT *) pArray)[iIndex]); ++iIndex;};
    if (iIndex < iLength) {printf ("%f ", ((FLOAT *) pArray)[iIndex]); ++iIndex;};
    printf ("\n");
    };
  };


//-----------------------------------------------------------------------------
VOID  FloatArray::CalcRange (FLOAT &  fMinOut,
                             FLOAT &  fMaxOut)
  {
  if (iLength == 0)
    {
    fMinOut = fMaxOut = 0.0f;
    return;
    };

  fMinOut = fMaxOut = ((FLOAT *) pArray)[0];

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    FLOAT  fCurr = ((FLOAT *) pArray)[iIndex];

    fMinOut = RMin (fMaxOut, fCurr);
    fMaxOut = RMax (fMaxOut, fCurr);
    };
  };



