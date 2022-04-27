/* -----------------------------------------------------------------
                            Double Array

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

#include "Containers/DoubleArray.hpp"


//-----------------------------------------------------------------------------
//  DoubleArray
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
DoubleArray::DoubleArray  ()
  {
  Init ();
  };


//-----------------------------------------------------------------------------
DoubleArray::DoubleArray  (const DoubleArray &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };


//-----------------------------------------------------------------------------
DoubleArray::DoubleArray  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };


//-----------------------------------------------------------------------------
DoubleArray::~DoubleArray  ()
  {
  DeleteArrayLocal (&pArray);
  };


//-----------------------------------------------------------------------------
VOID *  DoubleArray::AllocArray  (INT  iSizeIn)
  {
  return new DOUBLE [iSizeIn];
  };

//-----------------------------------------------------------------------------
VOID  DoubleArray::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  DoubleArray::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  // This function is callable by the destructor
  delete [] ((DOUBLE *) *pvArrayIn);
  *pvArrayIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID  DoubleArray::ZeroValues  (INT    iStartOffset,
                                INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero, 0.0f);
  };

//-----------------------------------------------------------------------------
VOID  DoubleArray::InitValues  (INT    iStartOffset,
                                INT    iNumToInit,
                                DOUBLE  fValue)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    ((DOUBLE *) pArray) [iIndex] = fValue;
    };
  };


//-----------------------------------------------------------------------------
VOID  DoubleArray::MoveValues  (PVOID  pvSourceDataIn,
                                INT    iSourceOffsetIn,
                                INT    iStartOffsetIn,
                                INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    ((DOUBLE *) pArray) [iIndex + iStartOffsetIn] = ((DOUBLE *) pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  DoubleArray::MoveValuesRev  (PVOID  pvSourceDataIn,
                                   INT    iSourceOffsetIn,
                                   INT    iStartOffsetIn,
                                   INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    ((DOUBLE *) pArray) [iStartOffsetIn + iIndex] = ((DOUBLE *) pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  DoubleArray::AverageValues  (INT    iIndexAverage,
                                   INT    iIndexValueOne,
                                   INT    iIndexValueTwo,
                                   FLOAT  fBias)
  {
  DOUBLE dBias = fBias;

  ((DOUBLE *) pArray) [iIndexAverage] = ( (((DOUBLE *) pArray) [iIndexValueOne] * dBias) +
                                          (((DOUBLE *) pArray) [iIndexValueTwo] * (1.0 - dBias)) );
  };


//-----------------------------------------------------------------------------
VOID  DoubleArray::SwapIndexes  (INT  iIndexOne,
                                 INT  iIndexTwo)
  {
  DOUBLE                    iTemp = ((DOUBLE *) pArray) [iIndexOne];
  ((DOUBLE *) pArray) [iIndexOne] = ((DOUBLE *) pArray) [iIndexTwo];
  ((DOUBLE *) pArray) [iIndexTwo] = iTemp;
  };


//-----------------------------------------------------------------------------
INT  DoubleArray::CompareIndexes  (INT  iIndexOne,
                                   INT  iIndexTwo)
  {
  if (((DOUBLE *) pArray) [iIndexOne] == ((DOUBLE *) pArray) [iIndexTwo]) return (0);
  if (((DOUBLE *) pArray) [iIndexOne] <  ((DOUBLE *) pArray) [iIndexTwo]) return (-1);
  return (1);
  };


//-----------------------------------------------------------------------------
EStatus DoubleArray::SetAt (INT     iIndex,
                            DOUBLE  dElement)
  {
  if (iIndex >= iLength) return (EStatus::kFailure);
  ((DOUBLE *) pArray) [iIndex] = dElement;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus DoubleArray::Append (DOUBLE  dElement)
  {
  INT  iOldLength = iLength;

  if (SetLength (iOldLength + 1) == EStatus::kFailure) {return EStatus::kFailure;};

  ((DOUBLE *) pArray) [iOldLength] = dElement;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus DoubleArray::GetArray (DOUBLE  aOut []) const
  {
  if (iLength == 0) return (EStatus::kFailure);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    aOut [iIndex] = ((DOUBLE *) pArray) [iIndex];
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus DoubleArray::Copy (const DoubleArray &  arraySource)
  {
  if (SetLength (arraySource.iLength) == EStatus::kFailure) {return EStatus::kFailure;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
DOUBLE DoubleArray::operator[]  (INT  iIndex) const
  {
  if (! BoundsCheck (iIndex)) return (0);
  return ((DOUBLE *) pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
DOUBLE &  DoubleArray::operator[]  (INT  iIndex)
  {
  if (! BoundsCheck (iIndex))
    {
    static DOUBLE  iZero = 0.0;
    iZero = 0.0;
    return (iZero);
    };
  return ((DOUBLE *) pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
DoubleArray &  DoubleArray::operator=  (const DoubleArray &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };


//-----------------------------------------------------------------------------
VOID  DoubleArray::Set  (const  DOUBLE * pdIn,
                         INT             iCountIn)
  {
  SetMinLength (iCountIn);
  for (INT iIndex = 0; iIndex < iCountIn; ++iIndex)
    {
    ((DOUBLE *) pArray)[iIndex] = pdIn[iIndex];
    };
  };


//-----------------------------------------------------------------------------
BOOL  DoubleArray::operator==  (const DoubleArray &  arrayIn)
  {
  if (iLength == 0) return (false);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (arrayIn [iIndex] != ((DOUBLE *) pArray) [iIndex])
      {
      return (false);
      };
    };
  return (true);
  };


//-----------------------------------------------------------------------------
VOID  DoubleArray::DebugPrint  (UINT  uCount)
  {
  INT     iIndex = 0;

  while (iIndex < (INT) uCount)
    {
    if (iIndex < iLength) {printf ("%f ", ((DOUBLE *) pArray)[iIndex]); ++iIndex;};
    if (iIndex < iLength) {printf ("%f ", ((DOUBLE *) pArray)[iIndex]); ++iIndex;};
    if (iIndex < iLength) {printf ("%f ", ((DOUBLE *) pArray)[iIndex]); ++iIndex;};
    if (iIndex < iLength) {printf ("%f ", ((DOUBLE *) pArray)[iIndex]); ++iIndex;};
    printf ("\n");
    };
  };


//-----------------------------------------------------------------------------
VOID  DoubleArray::CalcRange (DOUBLE &  dMinOut,
                              DOUBLE &  dMaxOut)
  {
  if (iLength == 0)
    {
    dMinOut = dMaxOut = 0.0;
    return;
    };

  dMinOut = dMaxOut = ((DOUBLE *) pArray)[0];


  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    DOUBLE  dCurr = ((DOUBLE *) pArray)[iIndex];

    dMinOut = RMin (dMaxOut, dCurr);
    dMaxOut = RMax (dMaxOut, dCurr);
    };
  };


