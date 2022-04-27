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

#include "Containers/IntArray.hpp"


//-----------------------------------------------------------------------------
//  IntArray
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
IntArray::IntArray  ()
  {
  Init ();
  };


//-----------------------------------------------------------------------------
IntArray::IntArray  (const IntArray &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };


//-----------------------------------------------------------------------------
IntArray::IntArray  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };


//-----------------------------------------------------------------------------
IntArray::~IntArray  ()
  {
  DeleteArrayLocal (&pArray);
  };


//-----------------------------------------------------------------------------
VOID *  IntArray::AllocArray  (INT  iSizeIn)
  {
  return new INT32 [iSizeIn];
  };

//-----------------------------------------------------------------------------
VOID  IntArray::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  IntArray::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  delete [] ((INT32 *) *pvArrayIn);
  *pvArrayIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID  IntArray::ZeroValues  (INT    iStartOffset,
                             INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero, 0);
  };

//-----------------------------------------------------------------------------
VOID  IntArray::InitValues  (INT    iStartOffset,
                             INT    iNumToInit,
                             INT    iValue)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    ((INT32 *) pArray) [iIndex] = iValue;
    };
  };


//-----------------------------------------------------------------------------
VOID  IntArray::MoveValues  (PVOID  pvSourceDataIn,
                              INT    iSourceOffsetIn,
                              INT    iStartOffsetIn,
                              INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    ((INT32 *) pArray) [iIndex + iStartOffsetIn] = ((INT32 *) pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  IntArray::MoveValuesRev  (PVOID  pvSourceDataIn,
                                INT    iSourceOffsetIn,
                                INT    iStartOffsetIn,
                                INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    ((INT32 *) pArray) [iStartOffsetIn + iIndex] = ((INT32 *) pvSourceDataIn) [iIndex + iSourceOffsetIn];
    };
  };


//-----------------------------------------------------------------------------
VOID  IntArray::AverageValues  (INT    iIndexAverage,
                                INT    iIndexValueOne,
                                INT    iIndexValueTwo,
                                FLOAT  fBias)
  {


  ((INT32 *) pArray) [iIndexAverage] = INT32 ( (((INT32 *) pArray) [iIndexValueOne] * fBias) +
                                               (((INT32 *) pArray) [iIndexValueTwo] * (1.0f - fBias)) );
  };


//-----------------------------------------------------------------------------
VOID  IntArray::SwapIndexes  (INT  iIndexOne,
                              INT  iIndexTwo)
  {
  INT32                    iTemp = ((INT32 *) pArray) [iIndexOne];
  ((INT32 *) pArray) [iIndexOne] = ((INT32 *) pArray) [iIndexTwo];
  ((INT32 *) pArray) [iIndexTwo] = iTemp;
  };


//-----------------------------------------------------------------------------
INT  IntArray::CompareIndexes  (INT  iIndexOne,
                                INT  iIndexTwo)
  {
  if (((INT32 *) pArray) [iIndexOne] == ((INT32 *) pArray) [iIndexTwo]) return (0);
  if (((INT32 *) pArray) [iIndexOne] <  ((INT32 *) pArray) [iIndexTwo]) return (-1);
  return (1);
  };


//-----------------------------------------------------------------------------
EStatus IntArray::SetAt (INT     iIndex,
                         INT32   iElement)
  {
  if (iIndex >= iLength) return (EStatus::kFailure);
  ((INT32 *) pArray) [iIndex] = iElement;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus IntArray::Append (INT32  iElement)
  {
  INT  iOldLength = iLength;

  if (SetLength (iOldLength + 1) == EStatus::kFailure) {return EStatus::kFailure;};

  ((INT32 *) pArray) [iOldLength] = iElement;
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus IntArray::GetArray (INT32  aOut []) const
  {
  if (iLength == 0) return (EStatus::kFailure);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    aOut [iIndex] = ((INT32 *) pArray) [iIndex];
    };
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus IntArray::Copy (const IntArray &  arraySource)
  {
  if (SetLength (arraySource.iLength) == EStatus::kFailure) {return EStatus::kFailure;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
INT32 IntArray::operator[]  (INT  iIndex) const
  {
  if (! BoundsCheck (iIndex)) return (0);
  return ((INT32 *) pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
INT32 &  IntArray::operator[]  (INT  iIndex)
  {
  if (! BoundsCheck (iIndex))
    {
    static INT32  iZero = 0;

    iZero = 0;
    return (iZero);
    };
  return ((INT32 *) pArray) [iIndex];
  };


//-----------------------------------------------------------------------------
IntArray &  IntArray::operator=  (const IntArray &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };


//-----------------------------------------------------------------------------
BOOL  IntArray::operator==  (const IntArray &  arrayIn)
  {
  if (iLength == 0) return (false);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (arrayIn [iIndex] != ((INT32 *) pArray) [iIndex])
      {
      return (false);
      };
    };
  return (true);
  };


//-----------------------------------------------------------------------------
INT  IntArray::Find  (INT32  iSearchIn)
  {
  for (INT iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (((INT32 *) pArray) [iIndex] == iSearchIn)
      {
      return (iIndex);
      };
    };
  return (-1);
  };

//-----------------------------------------------------------------------------
VOID  IntArray::DebugPrint  (UINT  uCount)
  {
  INT32     iIndex = 0;

  while (iIndex < (INT32) uCount)
    {
    if (iIndex < iLength) {printf ("%d ", (INT)(((INT32 *) pArray)[iIndex])); ++iIndex;};
    if (iIndex < iLength) {printf ("%d ", (INT)(((INT32 *) pArray)[iIndex])); ++iIndex;};
    if (iIndex < iLength) {printf ("%d ", (INT)(((INT32 *) pArray)[iIndex])); ++iIndex;};
    if (iIndex < iLength) {printf ("%d ", (INT)(((INT32 *) pArray)[iIndex])); ++iIndex;};
    printf ("\n");
    };
  };


//-----------------------------------------------------------------------------
VOID  IntArray::CalcRange (INT &  iMinOut,
                           INT &  iMaxOut)
  {
  if (iLength == 0)
    {
    iMinOut = iMaxOut = 0;
    return;
    };

  iMinOut = iMaxOut = (INT)(((INT32 *) pArray)[0]);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    INT  iCurr = (INT)(((INT32 *) pArray)[iIndex]);

    iMinOut = RMin (iMinOut, iCurr);
    iMaxOut = RMax (iMaxOut, iCurr);
    };
  };


//-----------------------------------------------------------------------------
VOID  SortIndexes (BaseArray &  arrayToSortIn,
                   IntArray &   arrayOut)
  {
  // returns an array of indexes that access the elements of arrayToSortIn in
  // sorted order.

  // Perform a quicksort on the indexes of the array.


  INT32  iLeft;
  INT32  iRight;
  INT32  iIndex;


  // initialize the index array to be returned

  INT  iLength = arrayToSortIn.Length ();

  arrayOut.SetLength (iLength);
  for (iIndex = 0; iIndex < iLength; ++iIndex)
    {
    arrayOut [iIndex] = iIndex;
    };

  iLeft = 0;
  iRight = iLength - 1;

  IntArray  arrayStack;

  arrayStack.Append (iLeft);
  arrayStack.Append (iRight);

  do
    {
    if (iRight > iLeft)
      {
      // partition
      INT  iScanLeft = iLeft;
      INT  iScanRight = iRight;

      //printf ("partitioning between %d (%s) and %d (%s)\n", iScanLeft, (*this) [arrayOut [iScanLeft]].AsChar (),iScanRight, (*this) [arrayOut [iScanRight]].AsChar ());

      for (;;)
        {
        iScanLeft = iLeft;
        iScanRight = iRight - 1;

        while (iScanLeft < iRight)
          {
          //  scan left greater than right
          if (arrayToSortIn.CompareIndexes (arrayOut [iScanLeft], arrayOut [iRight]) == 1)
            {
            break;
            };
          ++iScanLeft;
          };
        while (iScanRight > iLeft)
          {
          // scan right less than right
          if (arrayToSortIn.CompareIndexes (arrayOut [iScanRight], arrayOut [iRight]) == -1)
            {
            break;
            };
          --iScanRight;
          };

        if (iScanLeft < iScanRight)
          {
          // swap indexes
          arrayOut.SwapIndexes (iScanLeft, iScanRight);
          }
        else
          {
          break;
          };
        }; // while scan left < scan right


      arrayOut.SwapIndexes (iScanLeft, iRight);

      iIndex = iScanLeft;
      // partition done

      if ((iIndex - iLeft) > (iRight - iIndex))
        {
        arrayStack.Append (iLeft);
        arrayStack.Append (iIndex - 1);
        iLeft = iIndex + 1;
        }
      else
        {
        arrayStack.Append (iIndex + 1);
        arrayStack.Append (iRight);
        iRight = iIndex - 1;
        };

      }
    else
      {
      iRight = arrayStack [arrayStack.Length () - 1];
      iLeft  = arrayStack [arrayStack.Length () - 2];
      arrayStack.Remove (arrayStack.Length () - 2, 2);
      };
    } while (arrayStack.Length () > 0);
  };


