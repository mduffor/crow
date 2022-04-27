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
#include "Containers/KVPArray.hpp"


//-----------------------------------------------------------------------------
//  KVPArray
//-----------------------------------------------------------------------------

// Note: This class should really be optimized by implementing it as an array of
//        pointers to RStr classes, instead of a straight array of RStrs.
//        Currently insert and remove calls perform copying entire strings,
//        instead of just moving pointers around.


//-----------------------------------------------------------------------------
KVPArray::KVPArray  ()
  {
  Init ();
  };


//-----------------------------------------------------------------------------
KVPArray::KVPArray  (const KVPArray &  arrayIn) : BaseArray ()
  {
  Init ();
  Copy (arrayIn);
  };


//-----------------------------------------------------------------------------
KVPArray::KVPArray  (INT   iInitialSize)
  {
  Init ();
  SetLength (iInitialSize);
  InitValues (0, iAllocSize);
  };


//-----------------------------------------------------------------------------
KVPArray::~KVPArray  ()
  {
  DeleteArrayLocal (&pArray);
  };


//-----------------------------------------------------------------------------
VOID *  KVPArray::AllocArray  (INT  iSizeIn)
  {
  return new KVPEntry [iSizeIn];
  };


//-----------------------------------------------------------------------------
VOID  KVPArray::DeleteArray (PVOID *  pvArrayIn)
  {
  DeleteArrayLocal (pvArrayIn);
  };

//-----------------------------------------------------------------------------
VOID  KVPArray::DeleteArrayLocal  (PVOID *  pvArrayIn)
  {
  delete [] ((KVPEntry *) *pvArrayIn);
  *pvArrayIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID  KVPArray::ZeroValues  (INT    iStartOffset,
                             INT    iNumToZero)
  {
  InitValues (iStartOffset, iNumToZero);
  };

//-----------------------------------------------------------------------------
VOID  KVPArray::InitValues  (INT    iStartOffset,
                             INT    iNumToInit)
  {
  // zero out the requested range of the array

  INT iEndOffset = iStartOffset + iNumToInit;

  for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
    {
    static_cast<KVPEntry *>(pArray) [iIndex].strKey.Empty ();
    static_cast<KVPEntry *>(pArray) [iIndex].strValue.Empty ();
    };
  };


//-----------------------------------------------------------------------------
VOID  KVPArray::MoveValues  (PVOID  pvSourceDataIn,
                              INT    iSourceOffsetIn,
                              INT    iStartOffsetIn,
                              INT    iNumToCopyIn)
  {
  for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
    {
    static_cast<KVPEntry *>(pArray) [iIndex + iStartOffsetIn].strKey   = static_cast<KVPEntry *>(pvSourceDataIn) [iIndex + iSourceOffsetIn].strKey;
    static_cast<KVPEntry *>(pArray) [iIndex + iStartOffsetIn].strValue = static_cast<KVPEntry *>(pvSourceDataIn) [iIndex + iSourceOffsetIn].strValue;
    };
  };


//-----------------------------------------------------------------------------
VOID  KVPArray::MoveValuesRev  (PVOID  pvSourceDataIn,
                                 INT    iSourceOffsetIn,
                                 INT    iStartOffsetIn,
                                 INT    iNumToCopyIn)
  {
  // this routine is the opposite of MoveValues, and is used for
  //  properly shifting values to the right in the array

  for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
    {
    static_cast<KVPEntry *>(pArray) [iStartOffsetIn + iIndex].strKey   = static_cast<KVPEntry *>(pvSourceDataIn) [iIndex + iSourceOffsetIn].strKey;
    static_cast<KVPEntry *>(pArray) [iStartOffsetIn + iIndex].strValue = static_cast<KVPEntry *>(pvSourceDataIn) [iIndex + iSourceOffsetIn].strValue;
    };
  };


//-----------------------------------------------------------------------------
VOID  KVPArray::AverageValues  (INT    iIndexAverage,
                                INT    iIndexValueOne,
                                INT    iIndexValueTwo,
                                FLOAT  fBias)
  {
  // note:  You can't average strings.

  //((RStr *) pArray) [iIndexAverage] = ( (((RStr *) pArray) [iIndexValueOne] * fBias) +
  //                                       (((RStr *) pArray) [iIndexValueTwo] * (1.0f - fBias)) );
  };


//-----------------------------------------------------------------------------
VOID  KVPArray::SwapIndexes  (INT  iIndexOne,
                              INT  iIndexTwo)
  {
  RStr                  strTemp = static_cast<KVPEntry *>(pArray) [iIndexOne].strKey;
  static_cast<KVPEntry *>(pArray) [iIndexOne].strKey = static_cast<KVPEntry *>(pArray) [iIndexTwo].strKey;
  static_cast<KVPEntry *>(pArray) [iIndexTwo].strKey = strTemp;

                        strTemp = static_cast<KVPEntry *>(pArray) [iIndexOne].strValue;
  static_cast<KVPEntry *>(pArray) [iIndexOne].strValue = static_cast<KVPEntry *>(pArray) [iIndexTwo].strValue;
  static_cast<KVPEntry *>(pArray) [iIndexTwo].strValue = strTemp;
  };


//-----------------------------------------------------------------------------
INT  KVPArray::CompareIndexes  (INT  iIndexOne,
                                INT  iIndexTwo)
  {
  // This compares values only.
  return (  (static_cast<KVPEntry *>(pArray) [iIndexOne].strValue).Compare (static_cast<KVPEntry *>(pArray) [iIndexTwo].strValue) );
  };


//-----------------------------------------------------------------------------
INT  KVPArray::FindKey (const char *  szKeyIn) const
  {
  // NOTE:  This could be sped up with a hash compare
  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    if (static_cast<KVPEntry *>(pArray) [iIndex].strKey.Compare (szKeyIn) == 0)
      {
      return (iIndex);
      };
    };
  return (-1);
  };


//-----------------------------------------------------------------------------
EStatus KVPArray::SetAt (const char *   szKeyIn,
                         const char *   szValueIn)
  {
  INT  iIndex = FindKey (szKeyIn);

  if (iIndex == -1)
    {
    // not found.  Add it.
    return (Append (szKeyIn, szValueIn));
    };

  // just update the value
  static_cast<KVPEntry *>(pArray) [iIndex].strValue = szValueIn;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus KVPArray::Append (const char *  szKeyIn,
                          const char *  szValueIn)
  {
  INT  iOldLength = iLength;

  EStatus  status = SetLength (iOldLength + 1);
  if (status == EStatus::kFailure) {return status;};

  static_cast<KVPEntry *>(pArray) [iOldLength].strKey   = szKeyIn;
  static_cast<KVPEntry *>(pArray) [iOldLength].strValue = szValueIn;
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus KVPArray::Copy (const KVPArray &  arraySource)
  {
  EStatus status = SetLength (arraySource.iLength);
  if (status == EStatus::kFailure) {return status;};

  MoveValues (arraySource.pArray, 0, 0, arraySource.Length ());

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
RStr KVPArray::operator[]  (const char *  szKeyIn) const
  {
  INT  iIndex = FindKey (szKeyIn);

  if (iIndex == -1) {return (RStr::kEmpty);};

  return static_cast<KVPEntry *>(pArray) [iIndex].strValue;
  };


//-----------------------------------------------------------------------------
RStr &  KVPArray::operator[]  (const char *  szKeyIn)
  {
  INT  iIndex = FindKey (szKeyIn);

  if (iIndex == -1) {
    // Since we don't want to chance modifying the empty string, we create a new, empty entry

    if (SetLength (iLength + 1) == EStatus::kSuccess)
      {
      iIndex = iLength - 1; // iLength was updated in SetLength
      static_cast<KVPEntry *>(pArray) [iIndex].strKey = szKeyIn;
      }
    else
      {
      return (RStr::kEmpty);
      };
    };

  return static_cast<KVPEntry *>(pArray) [iIndex].strValue;
  };


//-----------------------------------------------------------------------------
KVPArray &  KVPArray::operator=  (const KVPArray &  arrayIn)
  {
  Copy (arrayIn);
  return (*this);
  };


//-----------------------------------------------------------------------------
BOOL  KVPArray::operator==  (const  KVPArray & arrayIn)
  {
  if ((iLength == 0) || (iLength != arrayIn.iLength)) return (FALSE);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    INT  iSearchIndex = arrayIn.FindKey (static_cast<KVPEntry *>(pArray) [iIndex].strKey.AsChar ());

    if (iSearchIndex == -1) return (false);

    if ((static_cast<KVPEntry *>(arrayIn.pArray) [iSearchIndex].strValue) != static_cast<KVPEntry *>(pArray) [iIndex].strValue) return (FALSE);
    };
  return (TRUE);
  };

//-----------------------------------------------------------------------------
const RStr &  KVPArray::KeyAtIndex     (INT  iIndexIn) const
  {
  if (iIndexIn >= iLength) return (RStr::kEmpty);
  return (static_cast<KVPEntry *>(pArray) [iIndexIn].strKey);
  };

//-----------------------------------------------------------------------------
const RStr &  KVPArray::ValueAtIndex   (INT  iIndexIn) const
  {
  if (iIndexIn >= iLength) return (RStr::kEmpty);
  return (static_cast<KVPEntry *>(pArray) [iIndexIn].strValue);
  };

//------------------------------------------------------------------------------
const char *  KVPArray::ExpandVars (RStrParser &  parserIn) const
  {
  // scan for environment-style variables, and expand as you find them.
  // An environment variable name starts with a dollar sign ($), followed by
  //  a string of alpha_numeric and/or underscore characters.  This string may
  //  optionally be enclosed in either curly brackets {} or parenthesis ()

  // Note that if a match cannot be found, the env_var will be replaced
  //  with an empty string.

  // REFACTOR : Since this is now used by the Expression engine, we should
  //  go in and optimize this routine to be faster.

  INT32  iSavedCursorPos = parserIn.GetCursorStart ();
  RStr  strKey;
  RStr  strValue;
  RStr  strFullTag;
  while (parserIn.GetNextEnvVar (strKey, &strFullTag))
    {
    strValue = (*this)[strKey.AsChar ()];
    //DBG_INFO ("KVPArray::ExpandVars found KVP \"%s\" : \"%s\" at index %d", strKey.AsChar(), strValue.AsChar(), parserIn.GetCursorStart ());

    if (strValue.IsEmpty ())
      {
      // If we don't find the key, keep the tag in.  This will allow multi-pass replacements.
      parserIn.InsertString (strFullTag, parserIn.GetCursorStart ());
      // increment the cursor so we don't keep finding the same tag.
      parserIn.SkipChars (2);
      }
    else
      {
      parserIn.InsertString (strValue, parserIn.GetCursorStart ());
      };
    strFullTag.Empty();
    }
  parserIn.SetCursorStart (iSavedCursorPos);
  return (parserIn.AsChar ());
  };


//------------------------------------------------------------------------------
const char *  KVPArray::ExpandMacros (const char *  szSourceIn,
                                      RStrParser &  parserOut) const
  {
  // This is for a faster ExpandVars replace on a large buffer.  It replaces the
  //  text alone, without requiring the Env Var brackets "${}".

  if (iLength == 0) return (szSourceIn);

  // So our goal here is to build a new string with what is effectively an
  // insertion sort.
  RStr  strOrig;
  strOrig.AttachBuffer (szSourceIn);

  IntArray  aiNext      (iLength);
  IntArray  aiKeyLength (iLength);

  INT  iIndex;
  for (iIndex = 0; iIndex < iLength; ++iIndex)
    {
    RStr  strKey = KeyAtIndex(iIndex);

    aiNext      [iIndex] = strOrig.FindIdentifier (strKey);
    aiKeyLength [iIndex] = strKey.Length();
    };

  INT  iCopyLoc = 0;
  INT  iReplaceIndex = -1;

  // NOTE: Maintaining a sorted list might be faster if you have a large dictionary
  //  of replacement strings.  It adds complexity and a possibly slowdown from sorting,
  //  so I'm approaching the problem with a more straightfoward search solution for now.

  do
    {
    // find the lowest index that is not -1
    INT  iReplaceOffset = INT_MAX;
    iReplaceIndex = -1;
    for (iIndex = 0; iIndex < iLength; ++iIndex)
      {
      INT  iNextOccurrance = aiNext [iIndex];
      if ((iNextOccurrance != -1) && (iNextOccurrance < iReplaceOffset))
        {
        iReplaceOffset = iNextOccurrance;
        iReplaceIndex  = iIndex;
        };
      };
    // if no index, copy the remainder of the string and exit
    if (iReplaceIndex == -1)
      {
      parserOut.AppendChars (strOrig.GetBufferPtr (iCopyLoc), strOrig.Length() - iCopyLoc);
      }
    else
      {
      // copy up to the starting point of the replace
      parserOut.AppendChars (strOrig.GetBufferPtr (iCopyLoc), iReplaceOffset - iCopyLoc);
      iCopyLoc = iReplaceOffset + aiKeyLength [iReplaceIndex];
      // Copy in the replacement text
      parserOut += ValueAtIndex (iReplaceIndex);
      // Look up the next occurance of the replacement
      aiNext [iReplaceIndex] = strOrig.FindIdentifier (KeyAtIndex(iReplaceIndex), iReplaceOffset + 1);
      };
    } while (iReplaceIndex != -1);

  strOrig.DetachBuffer ();
  return (parserOut.AsChar ());
  };

//------------------------------------------------------------------------------
const char *  KVPArray::FindReplace (RStrParser &  parserIn) const
  {
  INT32  iSavedCursorPos = parserIn.GetCursorStart ();

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    parserIn.Replace (static_cast<KVPEntry *>(pArray) [iIndex].strKey,
                      static_cast<KVPEntry *>(pArray) [iIndex].strValue,
                      TRUE);
    //DBG_INFO ("KVPArray::FindReplace found KVP \"%s\" : \"%s\"", strKey.AsChar(), strValue.AsChar());
    };
  parserIn.SetCursorStart (iSavedCursorPos);
  return (parserIn.AsChar ());
  };


//-----------------------------------------------------------------------------
VOID KVPArray::DebugPrint (VOID)
  {
  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    DBG_INFO ("%s:%s", static_cast<KVPEntry *>(pArray) [iIndex].strKey.AsChar (), static_cast<KVPEntry *>(pArray) [iIndex].strValue.AsChar ());
    };
  };


