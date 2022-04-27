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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Util/RStr.hpp"
#include "Containers/KVPIntArray.hpp"


//-----------------------------------------------------------------------------
//  KVPIntArray
//-----------------------------------------------------------------------------

// Note: This class should really be optimized by implementing it as an array of
//        pointers to RStr classes, instead of a straight array of RStrs.
//        Currently insert and remove calls perform copying entire strings,
//        instead of just moving pointers around.


//-----------------------------------------------------------------------------
KVPIntArray::KVPIntArray  ()
  {
  Init ();
  };

//-----------------------------------------------------------------------------
KVPIntArray::KVPIntArray  (const KVPIntArray &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };

//-----------------------------------------------------------------------------
KVPIntArray::KVPIntArray  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };

//-----------------------------------------------------------------------------
KVPIntArray::~KVPIntArray  ()
  {
  DeleteArrayLocal (&pArray);
  };

//-----------------------------------------------------------------------------
VOID *  KVPIntArray::AllocArray  (INT  iSizeIn)
  {
  return new KVPIntEntry [iSizeIn];
  };

//-----------------------------------------------------------------------------
VOID  KVPIntArray::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  KVPIntArray::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  delete [] ((KVPIntEntry *) *pvArrayIn);
  *pvArrayIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID  KVPIntArray::ZeroValues  (INT    iStartOffset,
                                INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero);
  };

//-----------------------------------------------------------------------------
VOID  KVPIntArray::InitValues  (INT    iStartOffset,
                                INT    iNumToInit)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    static_cast<KVPIntEntry*>(pArray) [iIndex].strKey.Empty ();
    static_cast<KVPIntEntry*>(pArray) [iIndex].iValue = 0;
    };
  };


//-----------------------------------------------------------------------------
VOID  KVPIntArray::MoveValues  (PVOID  pvSourceDataIn,
                                INT    iSourceOffsetIn,
                                INT    iStartOffsetIn,
                                INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    static_cast<KVPIntEntry*>(pArray) [iIndex + iStartOffsetIn].strKey = static_cast<KVPIntEntry*>(pvSourceDataIn) [iIndex + iSourceOffsetIn].strKey;
    static_cast<KVPIntEntry*>(pArray) [iIndex + iStartOffsetIn].iValue = static_cast<KVPIntEntry*>(pvSourceDataIn) [iIndex + iSourceOffsetIn].iValue;
    };
  };

//-----------------------------------------------------------------------------
VOID  KVPIntArray::MoveValuesRev  (PVOID  pvSourceDataIn,
                                   INT    iSourceOffsetIn,
                                   INT    iStartOffsetIn,
                                   INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    static_cast<KVPIntEntry*>(pArray) [iStartOffsetIn + iIndex].strKey = static_cast<KVPIntEntry*>(pvSourceDataIn) [iIndex + iSourceOffsetIn].strKey;
    static_cast<KVPIntEntry*>(pArray) [iStartOffsetIn + iIndex].iValue = static_cast<KVPIntEntry*>(pvSourceDataIn) [iIndex + iSourceOffsetIn].iValue;
    };
  };

//-----------------------------------------------------------------------------
VOID  KVPIntArray::AverageValues  (INT    iIndexAverage,
                                   INT    iIndexValueOne,
                                   INT    iIndexValueTwo,
                                   FLOAT  fBias)
  {
  static_cast<KVPIntEntry*>(pArray) [iIndexAverage].iValue = ( (static_cast<KVPIntEntry*>(pArray) [iIndexValueOne].iValue * fBias) +
                                                      (static_cast<KVPIntEntry*>(pArray) [iIndexValueTwo].iValue * (1.0f - fBias)) );
  };

//-----------------------------------------------------------------------------
VOID  KVPIntArray::SwapIndexes  (INT  iIndexOne,
                                 INT  iIndexTwo)
  {
  RStr                  strTemp = static_cast<KVPIntEntry*>(pArray) [iIndexOne].strKey;
  static_cast<KVPIntEntry*>(pArray) [iIndexOne].strKey = static_cast<KVPIntEntry*>(pArray) [iIndexTwo].strKey;
  static_cast<KVPIntEntry*>(pArray) [iIndexTwo].strKey = strTemp;

  INT                     iTemp = static_cast<KVPIntEntry*>(pArray) [iIndexOne].iValue;
  static_cast<KVPIntEntry*>(pArray) [iIndexOne].iValue = static_cast<KVPIntEntry*>(pArray) [iIndexTwo].iValue;
  static_cast<KVPIntEntry*>(pArray) [iIndexTwo].iValue = iTemp;
  };

//-----------------------------------------------------------------------------
INT  KVPIntArray::CompareIndexes  (INT  iIndexOne,
                                   INT  iIndexTwo)
  {
  // This compares values only.
  INT  iValueOne =  static_cast<KVPIntEntry*>(pArray) [iIndexOne].iValue;
  INT  iValueTwo =  static_cast<KVPIntEntry*>(pArray) [iIndexTwo].iValue;

  return ((iValueOne == iValueTwo) ? 0 : (iValueOne < iValueTwo) ? -1 : 1);
  };

//-----------------------------------------------------------------------------
INT  KVPIntArray::FindIndexByKey (const char *  szKeyIn) const
  {
  // NOTE:  This could be sped up with a tree
  RStr  strKey (szKeyIn, TRUE);
  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (static_cast<KVPIntEntry*>(pArray) [iIndex].strKey == strKey)
      {
      return (iIndex);
      };
    };
  return (-1);
  };

//-----------------------------------------------------------------------------
INT  KVPIntArray::FindIndexByValue (INT           iValueIn) const
  {
  // NOTE:  This could be sped up with a tree
  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (static_cast<KVPIntEntry*>(pArray) [iIndex].iValue == iValueIn)
      {
      return (iIndex);
      };
    };
  return (-1);
  };

//-----------------------------------------------------------------------------
EStatus KVPIntArray::Set (const char *   szKeyIn,
                          INT            iValueIn)
  {
  INT  iIndex = FindIndexByKey (szKeyIn);

  if (iIndex == -1)
    {
    // not found.  Add it.
    return (Append (szKeyIn, iValueIn));
    };

  // just update the value
  static_cast<KVPIntEntry*>(pArray) [iIndex].iValue = iValueIn;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus KVPIntArray::Append (const char *  szKeyIn,
                             INT           iValueIn)
  {
  INT  iOldLength = iLength;

  EStatus  status = SetLength (iOldLength + 1);
  if (status == EStatus::kFailure) {return status;};

  static_cast<KVPIntEntry*>(pArray) [iOldLength].strKey = szKeyIn;
  static_cast<KVPIntEntry*>(pArray) [iOldLength].iValue = iValueIn;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus KVPIntArray::Copy (const KVPIntArray &  arraySource)
  {
  EStatus status = SetLength (arraySource.iLength);
  if (status == EStatus::kFailure) {return status;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
INT KVPIntArray::operator[]  (const char *  szKeyIn) const
  {
  INT  iIndex = FindIndexByKey (szKeyIn);

  if (iIndex == -1) {return (0);};
  return static_cast<KVPIntEntry*>(pArray) [iIndex].iValue;
  };

//-----------------------------------------------------------------------------
KVPIntArray &  KVPIntArray::operator=  (const KVPIntArray &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };

//-----------------------------------------------------------------------------
BOOL  KVPIntArray::operator==  (const  KVPIntArray & arrayIn)
  {
  if ((iLength == 0) || (iLength != arrayIn.iLength)) return (FALSE);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    INT  iSearchIndex = arrayIn.FindIndexByKey (static_cast<KVPIntEntry*>(pArray) [iIndex].strKey.AsChar ());

    if (iSearchIndex == -1) return (false);

    if ((static_cast<KVPIntEntry*>(arrayIn.pArray) [iSearchIndex].iValue) != static_cast<KVPIntEntry*>(pArray) [iIndex].iValue) return (FALSE);
    };
  return (TRUE);
  };

//-----------------------------------------------------------------------------
const RStr &  KVPIntArray::KeyAtIndex     (INT  iIndexIn)
  {
  if (iIndexIn >= iLength) return (RStr::kEmpty);
  return (static_cast<KVPIntEntry*>(pArray) [iIndexIn].strKey);
  };

//-----------------------------------------------------------------------------
INT  KVPIntArray::ValueAtIndex   (INT  iIndexIn)
  {
  if (iIndexIn >= iLength) return (0);
  return (static_cast<KVPIntEntry*>(pArray) [iIndexIn].iValue);
  };

//-----------------------------------------------------------------------------
VOID KVPIntArray::DebugPrint (VOID)
  {
  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    DBG_INFO ("%s:%d", static_cast<KVPIntEntry*>(pArray) [iIndex].strKey.AsChar (), static_cast<KVPIntEntry*>(pArray) [iIndex].iValue);
    };
  };


