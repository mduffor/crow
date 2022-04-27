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

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Containers/PtrArray.hpp"


//-----------------------------------------------------------------------------
//  PtrArray
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
PtrArray::PtrArray  ()
  {
  Init ();
  };


//-----------------------------------------------------------------------------
PtrArray::PtrArray  (const PtrArray &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };


//-----------------------------------------------------------------------------
PtrArray::PtrArray  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };


//-----------------------------------------------------------------------------
PtrArray::~PtrArray  ()
  {
  DeleteArrayLocal (&pArray);
  };


//-----------------------------------------------------------------------------
VOID *  PtrArray::AllocArray  (INT  iSizeIn)
  {
  return new PVOID [iSizeIn];
  };


//-----------------------------------------------------------------------------
VOID  PtrArray::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  PtrArray::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  delete [] ((PVOID *) *pvArrayIn);
  *pvArrayIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID  PtrArray::ZeroValues  (INT    iStartOffset,
                             INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero, NULL);
  };

//-----------------------------------------------------------------------------
VOID  PtrArray::InitValues  (INT    iStartOffset,
                             INT    iNumToInit,
                             PVOID  pValue)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    ((PVOID *) pArray) [iIndex] = pValue;
    };
  };


//-----------------------------------------------------------------------------
VOID  PtrArray::MoveValues  (PVOID  pvSourceDataIn,
                             INT    iSourceOffsetIn,
                             INT    iStartOffsetIn,
                             INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    ((PVOID *) pArray) [iIndex + iStartOffsetIn] = ((PVOID *) pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  PtrArray::MoveValuesRev  (PVOID  pvSourceDataIn,
                                INT    iSourceOffsetIn,
                                INT    iStartOffsetIn,
                                INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    ((PVOID *) pArray) [iStartOffsetIn + iIndex] = ((PVOID *) pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  PtrArray::AverageValues  (INT    iIndexAverage,
                                 INT    iIndexValueOne,
                                 INT    iIndexValueTwo,
                                 FLOAT  fBias)
  {
  DBG_ERROR ("PtrArray::AverageValues () not supported.");
  };


//-----------------------------------------------------------------------------
VOID  PtrArray::SwapIndexes  (INT  iIndexOne,
                              INT  iIndexTwo)
  {
  PVOID                    pTemp = ((PVOID *) pArray) [iIndexOne];
  ((PVOID *) pArray) [iIndexOne] = ((PVOID *) pArray) [iIndexTwo];
  ((PVOID *) pArray) [iIndexTwo] = pTemp;
  };


//-----------------------------------------------------------------------------
INT  PtrArray::CompareIndexes  (INT  iIndexOne,
                                INT  iIndexTwo)
  {
  if (((PVOID *) pArray) [iIndexOne] == ((PVOID *) pArray) [iIndexTwo]) return (0);
  if (((PVOID *) pArray) [iIndexOne] <  ((PVOID *) pArray) [iIndexTwo]) return (-1);
  return (1);
  };


//-----------------------------------------------------------------------------
EStatus PtrArray::SetAt (INT     iIndex,
                         PVOID   pElement)
  {
  if (iIndex >= iLength) return (EStatus::kFailure);
  ((PVOID *) pArray) [iIndex] = pElement;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus PtrArray::Append (PVOID  pElement)
  {
  INT  iOldLength = iLength;

  if (SetMinLength (iOldLength + 1) == EStatus::kFailure) {return EStatus::kFailure;};

  ((PVOID *) pArray) [iOldLength] = pElement;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus PtrArray::GetArray (PVOID  aOut []) const
  {
  if (iLength == 0) return (EStatus::kFailure);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    aOut [iIndex] = ((PVOID *) pArray) [iIndex];
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus PtrArray::Copy (const PtrArray &  arraySource)
  {
  if (SetMinLength (arraySource.iLength) == EStatus::kFailure) {return EStatus::kFailure;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
PVOID PtrArray::operator[]  (INT  iIndex) const
  {
  if (! BoundsCheck (iIndex)) return (0);
  return ((PVOID *) pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
PVOID &  PtrArray::operator[]  (INT  iIndex)
  {


  if (! BoundsCheck (iIndex)) {static PVOID  pNull = NULL; pNull = NULL; return (pNull);};
  return ((PVOID *) pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
PtrArray &  PtrArray::operator=  (const PtrArray &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };


//-----------------------------------------------------------------------------
BOOL  PtrArray::operator==  (const PtrArray &  arrayIn)
  {
  if (iLength == 0) return (false);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (arrayIn [iIndex] != ((PVOID *) pArray) [iIndex])
      {
      return (false);
      };
    };
  return (true);
  };


//-----------------------------------------------------------------------------
INT  PtrArray::Find  (PVOID  pSearchIn)
  {
  for (INT iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (((PVOID *) pArray) [iIndex] == pSearchIn)
      {
      return (iIndex);
      };
    };
  return (-1);
  };

//-----------------------------------------------------------------------------
VOID  PtrArray::DebugPrint  (UINT  uCount)
  {
  printf ("DebugPrint not supported for PtrArray\n");
  printf ("\n");
  };

