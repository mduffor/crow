/* -----------------------------------------------------------------
                              Base Array

     This module implements the base class from which all array objects
       should be derived.  This allows us to build arrays that are aware
       of each other and can have operations applied across multiple
       related arrays at once.

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
ASSERTFILE (__FILE__);
#include "Containers/BaseArray.hpp"

//-----------------------------------------------------------------------------
//  BaseArray
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BaseArray::BaseArray ()
  {
  Init ();
  };

/*
//-----------------------------------------------------------------------------
BaseArray::BaseArray (const BaseArray &  arrayIn)
  {
  Init ();
//  SetLength (arrayIn.Length ());
//  MoveValues (arrayIn.pArray, 0, 0, arrayIn.Length ());
  };

//-----------------------------------------------------------------------------
BaseArray::BaseArray (INT  iInitialSize)
  {
  Init ();
//  SetLength (iInitialSize);
//  InitValues (0, iAllocSize);
  };
*/


//-----------------------------------------------------------------------------
BaseArray::~BaseArray ()
  {
  //SetSparseArray (FALSE);
  };


//-----------------------------------------------------------------------------
VOID  BaseArray::Init  (VOID)
  {
  pArray     = NULL;
  iLength    = 0;
  iAllocSize = 0;
  iAllocInc  = 10;
  iCurrIndex = -1;

  pPrev     = this;
  pNext     = this;
  };


//-----------------------------------------------------------------------------
EStatus BaseArray::SetMinLength (INT  iLengthIn)
  {
  // Make sure the length of the array is at least iLengthIn
  if (iLengthIn > Length ())
    {
    return (SetLength (iLengthIn));
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus BaseArray::SetLength (INT  iLengthIn)
  {
  INT    iNewAllocSize = iAllocSize;

  // calculate how much to grow the array.
  while (iLengthIn > iNewAllocSize)
    {
    iNewAllocSize += iAllocInc;
    };

  // if more memory is to be allocated, allocate it, copy over old values, and delete old memory.
  if (iNewAllocSize > iAllocSize)
    {
    VOID *  pNewArray = AllocArray (iNewAllocSize);

    // Note:  Ideally you need to detect an allocation failure here and return a failure code.
    if (pNewArray == NULL)  return (EStatus::kFailure);

    INT     iOldAllocSize = iAllocSize;
    VOID *  pOldArray     = pArray;

    pArray = pNewArray;
    iAllocSize = iNewAllocSize;

    if (iOldAllocSize > 0)
      {
      MoveValues (pOldArray, 0, 0, iOldAllocSize);
      DeleteArray (&pOldArray);
      };
    ZeroValues (iOldAllocSize, iNewAllocSize - iOldAllocSize);
    };
  iLength = iLengthIn;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
VOID BaseArray::Clear ()
  {
  iLength = 0;
  };


//-----------------------------------------------------------------------------
EStatus BaseArray::Remove (INT  iStartIndex,
                           INT  iNumToRemove)
  {
  if (iLength == 0) return (EStatus::kFailure);
  if (iStartIndex < 0) iStartIndex += iLength;

  RemoveSequentialValues (iStartIndex, iNumToRemove);

  // update siblings
  BaseArray *  pCurr = pNext;
  while (pCurr != this)
    {
    pCurr->Remove (iStartIndex, iNumToRemove);
    pCurr = pCurr->pNext;
    };

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus  BaseArray::RemoveSequentialValues (INT  iStartIndex,
                                            INT  iNumToRemove)
  {
  ASSERT (iNumToRemove > 0);

  if (iStartIndex < 0) iStartIndex += iLength;

  // perform the removal on the actual sequential data.  This is the internal
  //  Remove function since Remove() is primarily responsible for chaining the
  //  Remove() call to siblings.
  if ((iStartIndex + iNumToRemove) > iLength) return (EStatus::kFailure);

  if ((iStartIndex + iNumToRemove) != iLength)
    {
    FreeValues (iStartIndex, iNumToRemove);
    MoveValues (pArray, iStartIndex + iNumToRemove, iStartIndex, iLength - iStartIndex - iNumToRemove);
    ZeroValues (iLength - iNumToRemove, iNumToRemove);
    };
  iLength -= iNumToRemove;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus BaseArray::Insert (INT  iStartIndex,
                           INT  iNumToInsert,
                           BOOL bDebug)
  {
  // This routine inserts new entries before the iStartIndex.
  INT  iStartIndexActual = iStartIndex;

  // perform the insertion on the actual sequential data
  INT  iOldLength = iLength;

  if (iStartIndexActual > iLength) return (EStatus::kFailure);

  if (SetLength (iOldLength + iNumToInsert) == EStatus::kFailure) {return EStatus::kFailure;};
  if (Length () < (iOldLength + iNumToInsert) )  {return EStatus::kFailure;};

  if (bDebug)
    {
    DBG_INFO ("BA:Ins %d old:%d cur:%d\n", iStartIndexActual, iOldLength, Length ());
    };
  //printf ("start index %d old length %d\n", iStartIndexActual,iOldLength);
  if ((iStartIndexActual != iLength) && (iOldLength - iStartIndexActual > 0))
    {
    //DBG_INFO ("Copy values rev %d %d %d\n", iStartIndexActual, iStartIndexActual + iNumToInsert, iOldLength - iStartIndexActual);
    FreeValues (iLength - iNumToInsert, iNumToInsert);
    MoveValuesRev (pArray, iStartIndexActual, iStartIndexActual + iNumToInsert, iOldLength - iStartIndexActual);
    ZeroValues (iStartIndexActual, iNumToInsert);
    };

  // update siblings
  BaseArray *  pCurr = pNext;
  while (pCurr != this)
    {
    pCurr->Insert (iStartIndex, iNumToInsert);
    pCurr = pCurr->pNext;
    };

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
VOID BaseArray::SetSizeIncrement (INT iNewIncrement)
  {
  iAllocInc = iNewIncrement;
  if (iAllocInc <= 0)  iAllocInc = 1;
  };


//-----------------------------------------------------------------------------
INT BaseArray::SizeIncrement () const
  {
  return (iAllocInc);
  };


//-----------------------------------------------------------------------------
VOID  BaseArray::AddAsSibling  (BaseArray *  parrayIn)
  {
  // first, remove ourself from any existing lists.
  RemoveFromSiblings ();

  // add ourselves to the linked list
  pNext           = parrayIn;
  pPrev           = parrayIn->pPrev;
  parrayIn->pPrev = this;
  pPrev->pNext    = this;
  };


//-----------------------------------------------------------------------------
VOID  BaseArray::RemoveFromSiblings  (VOID)
  {
  if (pPrev != this)
    {
    pPrev->pNext = pNext;
    pNext->pPrev = pPrev;

    pNext = this;
    pPrev = this;
    };
  };

//-----------------------------------------------------------------------------
VOID  BaseArray::RemoveIndexes (INT  iStartIndex,
                                INT  iNumToRemove)
  {
  if (iStartIndex < 0) iStartIndex += iLength;

  // indexes are always stored as INT32
  INT32  iEndIndex = iStartIndex + iNumToRemove;

  // Remove any matches, and collapse any followers.

  for (INT32  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (((INT32 *) pArray) [iIndex] >= iStartIndex)
      {
      // found an index that is changing
      if (((INT32 *) pArray) [iIndex] <  iEndIndex)
        {
        // found an index that is going away
        Remove (iIndex, 1);

        // since this index is gone now, we want to check it again next
        //  time through the loop since it now contains a new value that
        //  was collapsed down into it.
        --iIndex;
        }
      else
        {
        // decrement this index

        ((INT32 *) pArray) [iIndex] -= iNumToRemove;
        };
      };
    };
  };


//-----------------------------------------------------------------------------
VOID  BaseArray::InsertIndexes (INT  iStartIndex,
                                INT  iNumToInsert)
  {
  // indexes are always stored as INT32
  for (INT32  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (((INT32 *) pArray) [iIndex] >= iStartIndex)
      {
      // found an index that is changing

      ((INT32 *) pArray) [iIndex] += iNumToInsert;
      };
    };
  };
