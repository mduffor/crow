/* -----------------------------------------------------------------
                           String Class

    This module implements a string class.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
#include "Util/RStr.hpp"

ASSERTFILE (__FILE__);

const UINT32   RStr::kESCAPE_PREFIX       = 0x5c;

RStr   RStr::kEmpty ("");
const char  RStr::szEmpty [1] = {'\0'};


//------------------------------------------------------------------------
RStr::RStr  ()
  {
  // strings should always have a valid pszBuffer value, so we initialize it
  // to a string that contains a null
  Init (0);
  };

//------------------------------------------------------------------------
RStr::RStr  (const RStr &  strIn,
             BOOL          bCalcHash)
  {
  // strings should always have a valid pszBuffer value, so we initialize it
  // to a string that contains a null
  Init (0);

  this->AppendChars (strIn.pszBuffer, strIn.Length ());
  if (bCalcHash || (strIn.uHash != 0)) {CalcHash ();};
  };

//------------------------------------------------------------------------
RStr::RStr  (RStr &        strIn,
             BOOL          bCalcHash,
             BOOL          bStripBuffer)
  {
  if (bStripBuffer)
    {
    pszBuffer      = strIn.pszBuffer;
    uStringLength  = strIn.uStringLength;
    uBufferSize    = strIn.uBufferSize;
    uGrowIncrement = strIn.uGrowIncrement;
    uHash          = strIn.uHash;

    strIn.Init (0);
    }
  else
    {
    Init (0);
    this->AppendChars (strIn.pszBuffer, strIn.Length ());
    if (bCalcHash || (strIn.uHash != 0)) {CalcHash ();};
    };
  };

//------------------------------------------------------------------------
RStr::RStr           (const char *  pszIn)
  {
  // strings should always have a valid pszBuffer value, so we initialize it
  // to a string that contains a null
  Init (0);

  if (pszIn != NULL)
    {
    this->AppendChars (pszIn, strlen (pszIn));
    };
  };

//------------------------------------------------------------------------
RStr::RStr           (const char *  pszIn,
                      BOOL          bCalcHash)
  {
  // strings should always have a valid pszBuffer value, so we initialize it
  // to a string that contains a null
  Init (0);

  if (pszIn != NULL)
    {
    this->AppendChars (pszIn, strlen (pszIn));
    };
  if (bCalcHash) {CalcHash ();};
  };


//------------------------------------------------------------------------
//RStr::RStr (const char * szFormatString, ...)
//  {
//  Init (0);
//
//  // Step through the args, and process the string as you go through each
//  va_list    vaArgList;
//
//  va_start         (vaArgList, szFormatString);
//  FormatProcessArg (szFormatString, vaArgList);
//  va_end           (vaArgList);
//  };


//------------------------------------------------------------------------
RStr::RStr  (UINT32  uSizeIn)
  {
  Init (uSizeIn);
  };


//------------------------------------------------------------------------
VOID  RStr::Init  (UINT32  uInitialSizeIn)
  {
  pszBuffer      = const_cast <char *> (RStr::szEmpty);
  uStringLength  = 0;
  uBufferSize    = 0;
  uGrowIncrement = 128; // grow 128 chars at a time as a default
  uHash          = 0;

  // REFACTOR: Strings should be initialized without any memory so their
  //  initial creation is fast.  I do not know what removing the below
  //  line will break.
  //Grow (uInitialSizeIn);
  };




//------------------------------------------------------------------------
RStr::~RStr          ()
  {
  FreeBuffer ();
  };


//------------------------------------------------------------------------
VOID RStr::FreeBuffer (VOID)
  {
  if (pszBuffer != const_cast <char *> (RStr::szEmpty))
    {
    //printf ("Free buffer at (%x)\n", pszBuffer);
    free (pszBuffer);
    pszBuffer = const_cast <char *> (RStr::szEmpty);
    uBufferSize = 0;
    uStringLength = 0;
    uHash = 0;
    };
  };

//------------------------------------------------------------------------
VOID RStr::AttachBuffer   (const char *  szBufferIn)
  {
  FreeBuffer ();
  pszBuffer = const_cast<char*>(szBufferIn);
  uStringLength = uBufferSize = strlen (pszBuffer);
  uHash          = 0;
  };

//------------------------------------------------------------------------
VOID RStr::DetachBuffer   (VOID)
  {
  pszBuffer = const_cast <char *> (RStr::szEmpty);
  uBufferSize = 0;
  uStringLength = 0;
  uHash = 0;
  };

//------------------------------------------------------------------------
VOID  RStr::SetAt  (UINT32  uPosIn,
                    UINT32  uIn)
  {
  if ((uPosIn + 1) >= uBufferSize)
    {
    // past the end of the buffer.
    GrowAbsolute (uPosIn + 1);
    };

  if (uPosIn >= uStringLength)
    {
    // extend the string to include the requested position
    uStringLength = uPosIn + 1;
    pszBuffer [uStringLength] = '\0';
    };

  // set the character
  pszBuffer [uPosIn] = static_cast <char> (uIn);
  uHash         = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::GetLeft  (UINT32  uCount,
                      RStr &  strResult) const
  {

  // Determine the final string length, grow the buffer, copy the characters, and cap the string with a null.

  if (uStringLength < uCount) {uCount = uStringLength;};

  strResult.GrowAbsolute (uCount);

  if (uCount > 0)
    {
    strncpy (strResult.pszBuffer, pszBuffer, uCount);
    strResult.pszBuffer [uCount] = 0;
    };
  strResult.uStringLength = uCount;
  strResult.uHash         = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::GetRight  (UINT32  uCount,
                       RStr &  strResult) const
  {

  // Determine the final string length, grow the buffer, copy the characters, and cap the string with a null.

  if (uStringLength < uCount) {uCount = uStringLength;};

  strResult.GrowAbsolute (uCount);

  if (uCount > 0)
    {
    strncpy (strResult.pszBuffer, pszBuffer + uStringLength - uCount, uCount);
    strResult.pszBuffer [uCount] = 0;
    };
  strResult.uStringLength = uCount;
  strResult.uHash         = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::GetMiddle  (UINT32  uStart,
                        UINT32  uCount,
                        RStr &  strResult) const
  {
  UINT32           uRemainingLength;


  // Determine the final string length, grow the buffer, copy the characters, and cap the string with a null.

  uRemainingLength = uStringLength - uStart;

  if (uRemainingLength < uCount) {uCount = uRemainingLength;};

  strResult.GrowAbsolute (uCount);

  if (uCount > 0)
    {
    strncpy (strResult.pszBuffer, pszBuffer + uStart, uCount);
    strResult.pszBuffer [uCount] = 0;
    };
  strResult.uStringLength = uCount;
  strResult.uHash         = 0;
  };


//------------------------------------------------------------------------
INT32  RStr::FindChar (UINT32  uIn,
                       UINT32  uStartAt) const
  {

  if (uStartAt >= uStringLength) return -1;


  char *  pszPos = strchr (pszBuffer + uStartAt, uIn);

  if (pszPos == NULL)  return -1;

  return (pszPos - pszBuffer);
  };


//------------------------------------------------------------------------
INT32  RStr::ReverseFindChar  (UINT32  uIn,
                               UINT32  uStartAt) const
  {
  INT32  iIndex;

  if (uStringLength == 0) return -1;

  if (uStartAt == (UINT32) -1)  {uStartAt = uStringLength - 1;};

  for (iIndex = (INT32) uStartAt; iIndex >= 0; --iIndex)
    {
    if (pszBuffer [iIndex] == static_cast <char> (uIn)) break;
    };
  return (iIndex);
  };


//------------------------------------------------------------------------
INT32  RStr::Find (const RStr &  strIn,
                   UINT32        uStartAt,
                   INT           iEndAt) const
  {
  return (Find (strIn.AsChar (), uStartAt, iEndAt));
  };


//------------------------------------------------------------------------
INT32  RStr::Find (const char *  szIn,
                   UINT32        uStartAt,
                   INT           iEndAt) const
  {

  if (uStartAt >= uStringLength) return -1;

  // NOTE: The following requires that pszBuffer be null terminated.
  //  This SHOULD be the case, but be careful in case it is not.
  char *  pszPos = strstr (pszBuffer + uStartAt, szIn);

  if (pszPos == NULL)  return -1;

  if ((iEndAt != -1) && ((pszPos - pszBuffer) >= iEndAt))
    {
    //DBG_INFO ("Find %d beyond end %d", (pszPos - pszBuffer), iEndAt);
    return -1;
    };

  return (pszPos - pszBuffer);
  };


//------------------------------------------------------------------------
BOOL  RStr::IsIdentChar (UINT32  uCharIn) const
  {
  return  ((uCharIn >= 'A' && uCharIn <= 'Z') ||
           (uCharIn >= 'a' && uCharIn <= 'z') ||
           (uCharIn >= '0' && uCharIn <= '9') ||
           uCharIn == '.' ||
           uCharIn == '_');
  };

//------------------------------------------------------------------------
INT32  RStr::FindIdentifier (const char *  szIn,
                             UINT32        uStartAt,
                             INT           iEndAt) const
  {
  if (uStartAt >= uStringLength) return -1;

  // NOTE: The following requires that pszBuffer be null terminated.
  //  This SHOULD be the case, but be careful in case it is not.
  char *  pszPos = strstr (pszBuffer + uStartAt, szIn);

  if (pszPos == NULL)  return -1;

  if ((iEndAt != -1) && ((pszPos - pszBuffer) >= iEndAt))
    {
    //DBG_INFO ("Find %d beyond end %d", (pszPos - pszBuffer), iEndAt);
    return -1;
    };

  // make sure we are surrounded by non-identifier characters, or ends.
  BOOL   bIsCapped  = TRUE;
  INT32  iFindStart = pszPos - pszBuffer;

  if ((iFindStart > 0) && (IsIdentChar (pszBuffer [iFindStart - 1])))
    {
    bIsCapped = FALSE;
    };

  INT  iExclusiveEnd = iFindStart + strlen (szIn);
  if ((UINT32 (iExclusiveEnd + 1) < uStringLength) && (IsIdentChar (pszBuffer [iExclusiveEnd])))
    {
    bIsCapped = FALSE;
    };

  if (!bIsCapped)
    {
    // try finding another match after this one that meets the requirements.
    return (FindIdentifier (szIn, iFindStart + 1, iEndAt));
    };

  return (iFindStart);
  };


//------------------------------------------------------------------------------
INT  RStr::Replace  (const RStr &  strSearchString,
                     const RStr &  strReplaceString,
                     BOOL          bReplaceAll)
  {
  INT  iFound = 0;
  INT  iDifference = strReplaceString.Length () - strSearchString.Length ();
  INT  iNumReplaced = 0;

  while ((iFound = Find (strSearchString, iFound)) != -1)
    {
    if (iDifference > 0)
      {
      // replace is longer than search.  Need to insert characters

      InsertChars (' ', UINT32 (iDifference), UINT32 (iFound));
      }
    else if (iDifference < 0)
      {
      // replace is shorter than search.  Need to remove characters.

      ClipMiddle (UINT32 (iFound), UINT32 (-iDifference));
      };
    // now replace the string
    strncpy (GetBufferPtr (iFound), strReplaceString.AsChar (), strReplaceString.Length ());
    ++iNumReplaced;
    if (!bReplaceAll) break;
    };
  uHash = 0;
  return (iNumReplaced);
  };


//------------------------------------------------------------------------
INT32  RStr::Compare        (const char *  pszMatchIn,
                             INT32         iNumCharsToMatch,
                             INT32         iStartPosition) const
  {
  if (iNumCharsToMatch == -1)
    {
    return (strcmp (&pszBuffer [iStartPosition], pszMatchIn));
    };
  return (strncmp (&pszBuffer [iStartPosition], pszMatchIn, iNumCharsToMatch));
  };


//------------------------------------------------------------------------
INT32  RStr::Compare        (RStr          strMatchIn,
                             INT32         iNumCharsToMatch,
                             INT32         iStartPosition) const
  {
  if (iNumCharsToMatch == -1)
    {
    return (strcmp (&pszBuffer [iStartPosition], strMatchIn.pszBuffer));
    };
  return (strncmp (&pszBuffer [iStartPosition], strMatchIn.pszBuffer, iNumCharsToMatch));
  };


//------------------------------------------------------------------------
INT32  RStr::CompareNoCase  (const char *  pszMatchIn,
                             INT32         iNumCharsToMatch,
                             INT32         iStartPosition) const
  {
  if (iNumCharsToMatch == -1)
    {
    #ifndef LINUX
      return (stricmp (&pszBuffer [iStartPosition], pszMatchIn));
    #else
      return (strcasecmp (&pszBuffer [iStartPosition], pszMatchIn));
    #endif
    }
  #ifndef LINUX
    return (strnicmp (&pszBuffer [iStartPosition], pszMatchIn, iNumCharsToMatch));
  #else
    return (strncasecmp (&pszBuffer [iStartPosition], pszMatchIn, iNumCharsToMatch));
  #endif
  };


//------------------------------------------------------------------------
INT32  RStr::CompareNoCase  (RStr          strMatchIn,
                             INT32         iNumCharsToMatch,
                             INT32         iStartPosition) const
  {
  if (iNumCharsToMatch == -1)
    {
    #ifndef LINUX
      return (stricmp (&pszBuffer [iStartPosition], strMatchIn.pszBuffer));
    #else
      return (strcasecmp (&pszBuffer [iStartPosition], strMatchIn.pszBuffer));
    #endif
    }
  #ifndef LINUX
    return (strnicmp (&pszBuffer [iStartPosition], strMatchIn.pszBuffer, iNumCharsToMatch));
  #else
    return (strncasecmp (&pszBuffer [iStartPosition], strMatchIn.pszBuffer, iNumCharsToMatch));
  #endif
  };


//------------------------------------------------------------------------
INT32  RStr::ReplaceChar  (UINT32        uSearchIn,
                           UINT32        uReplaceIn,
                           UINT32        uStartIn,
                           UINT32        uEndIn)
  {
  INT32            iNumReplaced = 0;
  UINT32           uEnd = (uEndIn != 0) ? uEndIn : uStringLength;



  UINT32           uCurr;

  for (uCurr = uStartIn; uCurr < uEnd; ++uCurr)
    {
    if (UINT32 (((UCHAR *) pszBuffer) [uCurr]) == uSearchIn)
      {
      pszBuffer [uCurr] = CHAR (uReplaceIn);
      ++iNumReplaced;
      };
    };
  uHash = 0;
  return (iNumReplaced);
  };


//------------------------------------------------------------------------
INT32  RStr::CountChar  (UINT32        uSearchIn,
                         UINT32        uStartIn,
                         UINT32        uEndsAt)
  {
  UINT32  uCountOut = 0;
  UINT32  uSearchEnd = (uEndsAt == 0) ? uStringLength : uEndsAt;

  for (UINT32  uIndex = uStartIn; uIndex < uSearchEnd; ++uIndex)
    {
    if (UINT32 (((UCHAR *) pszBuffer) [uIndex]) == uSearchIn)
      {
      ++uCountOut;
      };
    };
  return (uCountOut);
  };


//------------------------------------------------------------------------
INT32  RStr::TranslateEscapedChars  (UINT32        uStartIn,
                                     UINT32        uEndIn)
  {
  UINT32           uNumReplaced = 0;
  UINT32           uEnd = (uEndIn != 0) ? uEndIn : uStringLength;

  UINT32           uCurrRead;
  UINT32           uCurrWrite;

  if (uEnd == 0) return 0;

  for (uCurrRead = uCurrWrite = uStartIn; uCurrRead < uEnd; ++uCurrRead, ++uCurrWrite)
    {
    UINT32  uCharCurr = UINT32 (((UCHAR *) pszBuffer) [uCurrRead]);

    if (uCharCurr == kESCAPE_PREFIX)
      {
      uCurrRead++;
      uCharCurr = UINT32 (((UCHAR *) pszBuffer) [uCurrRead]);

      switch (uCharCurr)
        {
        case 'a':  {uCharCurr = '\a'; break;};
        case 'b':  {uCharCurr = '\b'; break;};
        case 'f':  {uCharCurr = '\f'; break;};
        case 'n':  {uCharCurr = '\n'; break;};
        case 'r':  {uCharCurr = '\r'; break;};
        case 't':  {uCharCurr = '\t'; break;};
        case 'v':  {uCharCurr = '\v'; break;};
        case '0':  {uCharCurr = '\0'; break;};
        default:   {--uNumReplaced; (((UCHAR *) pszBuffer) [uCurrWrite]) = kESCAPE_PREFIX; ++uCurrWrite; break;};
        };
      ++uNumReplaced;
      };
    (((UCHAR *) pszBuffer) [uCurrWrite]) = uCharCurr;
    };
  // the string is now shorter since we collapsed two-byte codes into one byte codes
  uStringLength -= uNumReplaced;
  (((UCHAR *) pszBuffer) [uCurrWrite]) = '\0';
  uHash = 0;

  return (INT32 (uNumReplaced));
  };


//------------------------------------------------------------------------
VOID  RStr::Empty          (VOID)
  {

  uStringLength = 0;
  uHash         = 0;
  if (pszBuffer != const_cast <char *> (RStr::szEmpty))
    {
    pszBuffer [0] = '\0';
    };
  };


//------------------------------------------------------------------------
VOID  RStr::Reset          (VOID)
  {

  uStringLength = 0;
  uHash         = 0;
  SetBufferSize (0);
  };


//------------------------------------------------------------------------
VOID  RStr::Grow  (UINT32  uSizeIn)
  {
  UINT32           uGrowSize = uBufferSize;

  // special case zero sized grows for speed.  Leave as-is
  if (uSizeIn == 0)
    {
    return;
    };

  // grow as needed
  if ((uStringLength + uSizeIn + 1) > uBufferSize)
    {
    // grow the buffer.

    while (uGrowSize < (uStringLength + uSizeIn))
      {
      uGrowSize += uGrowIncrement;
      };
    SetBufferSize (uGrowSize);
    };
  };


//------------------------------------------------------------------------
VOID  RStr::GrowAbsolute  (UINT32  uSizeIn)
  {
  // Pass this function the number of characters you need the string to hold.
  // SetBufferSize will handle allocating space for the terminating null
  // NOTE:  This function only grows the buffer to be large enough to
  //  accept the given uSizeIn.  It does NOT shrink the buffer.

  UINT32           uGrowSize = uBufferSize;

  if ((uSizeIn + 1) > uBufferSize)
    {
    // grow the buffer.

    while (uGrowSize < uSizeIn)
      {
      uGrowSize += uGrowIncrement;
      };
    SetBufferSize (uGrowSize);
    };
  };


//------------------------------------------------------------------------
RStr &  RStr::Set     (const RStr &  strIn,
                       BOOL          bCalcHashIn)
  {
  return (_Set (strIn, bCalcHashIn));
  };

//------------------------------------------------------------------------
RStr &  RStr::_Set     (const RStr &  strIn,
                       BOOL          bCalcHashIn)
  {
  uGrowIncrement = strIn.uGrowIncrement;
  GrowAbsolute (strIn.uStringLength);

  if (pszBuffer != const_cast <char *> (RStr::szEmpty))
    {
    memcpy  (pszBuffer, strIn.pszBuffer, strIn.uStringLength);

    uStringLength = strIn.uStringLength;

    pszBuffer [uStringLength] = '\0';
    if (bCalcHashIn || (strIn.uHash != 0))
      {
      CalcHash ();
      }
    else
      {
      uHash = 0;
      };
    };
  return *this;
  };


//------------------------------------------------------------------------
RStr &  RStr::Set     (const char *  pszIn,
                       BOOL          bCalcHashIn)
  {
  return (_Set (pszIn, bCalcHashIn));
  };

//------------------------------------------------------------------------
RStr &  RStr::_Set    (const char *  pszIn,
                       BOOL          bCalcHashIn)
  {
  UINT32          uNewLength;

  if (pszIn == NULL)
    {
    pszIn = "";
    };

  uNewLength = strlen (pszIn);

  // GrowAbsolute will take care of allocating space for the terminating null
  GrowAbsolute (uNewLength);

  if (uNewLength > 0)
    {
    memcpy (pszBuffer, pszIn, uNewLength);
    };

  uStringLength = uNewLength;

  //printf ("new length is %d\n", uNewLength);
  if (pszBuffer != const_cast <char *> (RStr::szEmpty))
    {
    pszBuffer [uStringLength] = '\0';
    if (bCalcHashIn)
      {
      CalcHash ();
      }
    else
      {
      uHash = 0;
      };
    };
  return *this;
  };


//------------------------------------------------------------------------
RStr &  RStr::operator =     (const RStr & strIn)
  {
  Init ();
  Set (strIn);
  return (*this);
  };


//------------------------------------------------------------------------
RStr &  RStr::operator =     (const char *  pszIn)
  {
  Set (pszIn);
  return (*this);
  };


//------------------------------------------------------------------------
VOID  RStr::operator +=    (const RStr &  strIn)
  {
  UINT32           uNewLength;

  if (strIn.uStringLength == 0) return;

  uNewLength = uStringLength + strIn.uStringLength;

  Grow (strIn.uStringLength);

  memcpy (pszBuffer + uStringLength, strIn.pszBuffer, strIn.uStringLength);

  uStringLength = uNewLength;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::operator +=    (const char *  pszIn)
  {
  UINT32          uNewLength;
  UINT32          uLengthIn;

  if (pszIn == NULL) return;
  uLengthIn = strlen (pszIn);

  if (uLengthIn == 0) return;

  uNewLength = uStringLength + uLengthIn;

  Grow (uLengthIn);

  memcpy (pszBuffer + uStringLength, pszIn, uLengthIn);

  uStringLength = uNewLength;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::operator +=    (const UINT32    uIn)
  {
  UINT32          uNewLength;


  uNewLength = uStringLength + 1;

  Grow (1);

  uStringLength = uNewLength;
  pszBuffer [uStringLength - 1] = static_cast <char> (uIn);
  pszBuffer [uStringLength]     = '\0';
  uHash = 0;
  //printf ("RStr+= char set %d, null at %d (%c)\n", uStringLength - 1, uStringLength, static_cast <char> (uIn));
  };


//------------------------------------------------------------------------
VOID  RStr::AppendString   (const RStr &  strIn)
  {
  *this += strIn;
  };


//------------------------------------------------------------------------
VOID  RStr::AppendString   (const char *  pszIn)
  {
  *this += pszIn;
  };


//------------------------------------------------------------------------
VOID  RStr::AppendChar     (UINT32  uIn)
  {
  *this += uIn;
  };


//------------------------------------------------------------------------
VOID  RStr::AppendChars    (const char *   pszIn,
                            INT32          iCopyLengthIn)
  {
  UINT32          uNewLength;

  if (iCopyLengthIn == -1)
    {
    iCopyLengthIn = strlen (pszIn);
    };

  if (iCopyLengthIn <= 0) return;

  uNewLength = uStringLength + UINT (iCopyLengthIn);

  Grow (UINT (iCopyLengthIn));

  memcpy (pszBuffer + uStringLength, pszIn, UINT (iCopyLengthIn));

  uStringLength = uNewLength;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::PrependString  (const RStr &  strIn)
  {
  UINT32           uNewLength;
  UINT32           uLengthIn;


  uLengthIn = strIn.uStringLength;
  if (uLengthIn == 0) return;

  uNewLength = uStringLength + uLengthIn;

  Grow (uLengthIn);

  memmove (pszBuffer + uLengthIn, pszBuffer, uStringLength);
  strncpy (pszBuffer, strIn.pszBuffer, uLengthIn);

  uStringLength = uNewLength;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::PrependString  (const char *  pszIn)
  {
  UINT32           uNewLength;
  UINT32           uLengthIn;


  uLengthIn = strlen (pszIn);
  if (uLengthIn == 0) return;

  uNewLength = uStringLength + uLengthIn;

  Grow (uLengthIn);

  memmove (pszBuffer + uLengthIn, pszBuffer, uStringLength);
  strncpy (pszBuffer, pszIn, uLengthIn);

  uStringLength = uNewLength;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::PrependChar    (UINT32          uIn)
  {
  UINT32           uNewLength;


  uNewLength = uStringLength + 1;

  Grow (1);

  memmove (pszBuffer + 1, pszBuffer, uStringLength);

  uStringLength = uNewLength;
  pszBuffer [0]             = static_cast <char> (uIn);
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::InsertChars  (UINT32        uCharIn,
                          UINT32        uCount,
                          UINT32        uPosIn)
  {
  UINT32           uNewLength;
  UINT32           uRemainingLength;


  if (uCount == 0) return;

  uNewLength       = uCount + uStringLength;
  uRemainingLength = uStringLength - uPosIn;

  Grow (uCount);

  // the first part of the string is already in place.  Move the end of the string,
  //  then insert the new part.

  if (uRemainingLength > 0)
    {
    memmove (pszBuffer + uPosIn + uCount, pszBuffer + uPosIn, uRemainingLength);
    };

  memset (pszBuffer + uPosIn, uCharIn, uCount);

  uStringLength = uNewLength;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::InsertString  (const RStr &  strIn,
                           UINT32        uPosIn)
  {
  UINT32           uNewLength;
  UINT32           uCopyLength;
  UINT32           uRemainingLength;


  uCopyLength = strIn.uStringLength;
  if (uCopyLength == 0) return;

  uNewLength       = uCopyLength + uStringLength;
  uRemainingLength = uStringLength - uPosIn;

  Grow (uCopyLength);

  // the first part of the string is already in place.  Move the end of the string,
  //  then insert the new part.

  if (uRemainingLength > 0)
    {
    memmove (pszBuffer + uPosIn + uCopyLength, pszBuffer + uPosIn, uRemainingLength);
    };

  strncpy (pszBuffer + uPosIn, strIn.pszBuffer, uCopyLength);

  uStringLength = uNewLength;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::InsertString  (const char *  pszIn,
                           UINT32        uPosIn)
  {
  UINT32           uNewLength;
  UINT32           uCopyLength;
  UINT32           uRemainingLength;


  uCopyLength = strlen (pszIn);
  if (uCopyLength == 0) return;

  uNewLength       = uCopyLength + uStringLength;
  uRemainingLength = uStringLength - uPosIn;

  Grow (uCopyLength);

  // the first part of the string is already in place.  Move the end of the string,
  //  then insert the new part.

  if (uRemainingLength > 0)
    {
    memmove (pszBuffer + uPosIn + uCopyLength, pszBuffer + uPosIn, uRemainingLength);
    };

  strncpy (pszBuffer + uPosIn, pszIn, uCopyLength);

  uStringLength = uNewLength;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::ClipLeft  (UINT32  uCountIn)
  {

  if (uCountIn > uStringLength)
    {
    uCountIn = uStringLength;
    };

  if (uCountIn == 0) return;

  memmove (pszBuffer, pszBuffer + uCountIn, uStringLength - uCountIn);

  uStringLength -= uCountIn;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::ClipRight  (UINT32  uCountIn)
  {

  if (uCountIn > uStringLength)
    {
    uCountIn = uStringLength;
    };

  uStringLength -= uCountIn;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::ClipMiddle  (UINT32  uStartCharIn,  // removes count chars from the middle of string
                         UINT32  uCountIn)
  {


  if (uStartCharIn >= uStringLength)  return;

  // clip uCountIn against the end of the string.

  if ((uStartCharIn + uCountIn) > uStringLength)
    {
    uCountIn = uStringLength - uStartCharIn;
    };
  if (uCountIn == 0)  return;

  // collapse the second part of the string over the middle.

  memmove (pszBuffer + uStartCharIn, pszBuffer + uStartCharIn + uCountIn, uStringLength - uCountIn - uStartCharIn);

  uStringLength -= uCountIn;
  pszBuffer [uStringLength] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
RStr  RStr::ChompLeft  (UINT32  uCountIn)
  {
  RStr             strReturn;


  strReturn.AppendChars (this->AsChar (), uCountIn);

  ClipLeft (uCountIn);
  return (strReturn);
  };


//------------------------------------------------------------------------
RStr  RStr::ChompRight  (UINT32  uCountIn)
  {
  RStr             strReturn;


  strReturn.AppendChars (this->AsChar (Length () - uCountIn), uCountIn);

  ClipRight (uCountIn);
  return (strReturn);
  };


//------------------------------------------------------------------------
RStr  RStr::SubString  (UINT32  uStartCharIn,
                        UINT32  uCountIn)
  {
  RStr  strReturn;

  SubString (uStartCharIn, uCountIn, strReturn);
  return strReturn;
  };


//------------------------------------------------------------------------
VOID  RStr::SubString  (UINT32  uStartCharIn,
                        UINT32  uCountIn,
                        RStr &  strReturn)
  {
  //DBG_INFO ("Substring start %d count %d size %d", uStartCharIn,  uCountIn, uStringLength);
  if (uStartCharIn >= uStringLength)
    {
    strReturn.Empty ();
    return;
    };

  // clip uCountIn against the end of the string.

  if ((uStartCharIn + uCountIn) > uStringLength)
    {
    uCountIn = uStringLength - uStartCharIn;
    };
  if (uCountIn == 0)
    {
    strReturn.Empty ();
    return;
    };

  // extract the needed characters
  strReturn.Empty ();
  strReturn.Grow (uCountIn);
  for (INT  iIndex = 0; iIndex < INT (uCountIn); ++iIndex)
    {
    strReturn.AppendChar (pszBuffer [uStartCharIn + iIndex]);
    };
  };



//------------------------------------------------------------------------
VOID  RStr::TruncateLeft   (INT32  iPosIn)
  {
  UINT32           uCount;

  if (iPosIn <= 0) return;


  uCount = (UINT32) iPosIn;

  ClipLeft (uCount);
  };


//------------------------------------------------------------------------
VOID  RStr::TruncateRight  (INT32  iPosIn)
  {
  UINT32           uCount;

  if (iPosIn >= (INT32) uStringLength) return;

  if (iPosIn < 0)
    {
    // truncate the entire string
    Empty ();
    return;
    }
  uCount = uStringLength - iPosIn - 1;

  ClipRight (uCount);
  };


//------------------------------------------------------------------------
const char *  RStr::Format  (const char *  szFormatString, ...)
  {

  // clear the current contents
  Empty ();


  // Step through the args, and process the string as you go through each
  va_list    vaArgList;


  va_start         (vaArgList, szFormatString);
  FormatProcessArg (szFormatString, vaArgList);
  va_end           (vaArgList);
  uHash = 0;
  return (AsChar ());
  };


//------------------------------------------------------------------------
VOID RStr::AppendFormat  (const char *  szFormatString, ...)
  {
  RStr             strTemp;

  // clear the current contents
  strTemp.Empty ();

  // Step through the args, and process the string as you go through each
  va_list    vaArgList;


  va_start                 (vaArgList, szFormatString);
  strTemp.FormatProcessArg (szFormatString, vaArgList);
  va_end                   (vaArgList);
  *this += strTemp;
  };


//------------------------------------------------------------------------
VOID RStr::FormatProcessArg (const char *  pszFormatStringIn,
                             va_list &     vaArgListIn)
  {

  for (;;)
    {
    const char *   pszNext;

    pszNext = strchr (pszFormatStringIn, '%');

    // if no format markeris found, break so you can copy the last of the string.
    if (pszNext == NULL) break;

    // if there is anything before the format marker, then append it to this string.
    if (pszNext > pszFormatStringIn)
      {
      AppendChars (pszFormatStringIn, pszNext - pszFormatStringIn);
      };

    // increment the string pointer to the character past the format marker
    pszFormatStringIn = pszNext + 1;

    // process this parameter

    if (! FormatMarker (pszFormatStringIn, vaArgListIn))
      {
      // marker processing error.  Copy the rest of the string as-is and exit.
      break;
      };
    };

  // copy the remaining part of the string

  *this += pszFormatStringIn;
  };



//------------------------------------------------------------------------
BOOL RStr::FormatMarker (const char * &  pszFormatStringIn,
                         va_list &       vaArgListIn)
  {
  CHAR             szFormatString [64];
  UINT32           uFSIndex;
  CHAR             cNextChar;
  const char       szPrefixChars    [] = "-+0 #*.123456789hlL";
  const char       szIntegerMarkers [] = "cCdiouxX";
  const char       szDoubleMarkers  [] = "eEfgG";
  CHAR             szBufferOut    [256];


  // this routine returns TRUE if the marker was successfully processed, or
  //  false if there was an error.


  // process the format marker in a printf type string


  // we will use sprintf to actually process the various argument-to-ASCII
  //   conversion.  Step through the marker and copy all of its elements
  //   to the szFormatString buffer, then pass this string and the arg
  //   to sprintf to get the text to write.

  // Note:  You may want to add a marker for RStr *


  szFormatString [0] = '%';
  uFSIndex = 1;

  // step through the prefix
  for (;;)
    {
    cNextChar = pszFormatStringIn [0];


    if (uFSIndex >= sizeof (szFormatString))
      {
      // read too many characters in!!!
      DBG_ERROR ("Incorrect format specifier in string.");
      return FALSE;
      };

    if (cNextChar == 0)
      {
      // found a terminating null.  This shouldn't be here after a format
      //  specifier.
      return FALSE;
      };

    ++pszFormatStringIn;

    if (strchr (szPrefixChars, cNextChar) != NULL)
      {
      // prefix character.  Add to format string and grab another character.

      szFormatString [uFSIndex] = cNextChar;

      ++uFSIndex;
      }
    else
      {
      // not a prefix char, so it must be a format marker.  Process it.
      break;
      };
    };

  // Process the format marker

  // strings
  if (cNextChar == 's')
    {
    const char *  pszStringOut = va_arg (vaArgListIn, const char *);
    if (pszStringOut == NULL) return (FALSE);

    INT32         iStrLength = strlen (pszStringOut);
    INT32         iCopyLength = iStrLength;
    char *        pszEnd;

    // get the string precision, in case a maximum number of chars to copy
    //  was specified
    szFormatString [uFSIndex] = 0;

    char *  pszPrecision = strchr (szFormatString, '.');
    if ((pszPrecision != NULL) &&
        (pszPrecision [1] != 0))
      {
      INT32  iPrecision = (UINT32) strtol (pszPrecision + 1, &pszEnd, 10);
      if ((iPrecision >= 0) && (iPrecision < iCopyLength))
        {
        iCopyLength = iPrecision;
        };
      };
    if (pszPrecision != NULL)
      {
      // replace the decimal with a null, so we can read the first part of the
      //  format string as an integer
      pszPrecision [0] = 0;
      };
    INT32  iPaddedLength = (UINT32) strtol (szFormatString + 1, &pszEnd, 10);

    // write any padded space
    INT32  iPadding = iPaddedLength - iCopyLength;
    while (iPadding > 0)
      {
      AppendChar (' ');
      --iPadding;
      };

    // write the string
    AppendChars (pszStringOut, (UINT32) iCopyLength);
    return (TRUE);
    };

  // integers
  if (strchr (szIntegerMarkers, cNextChar) != NULL)
    {
    szFormatString [uFSIndex    ] = cNextChar;
    szFormatString [uFSIndex + 1] = 0;

    INT32 iValue =  va_arg (vaArgListIn, INT32);

    sprintf (szBufferOut, szFormatString, iValue);

    AppendString (szBufferOut);

    return (TRUE);
    };

  // doubles
  if (strchr (szDoubleMarkers, cNextChar) != NULL)
    {
    szFormatString [uFSIndex    ] = cNextChar;
    szFormatString [uFSIndex + 1] = 0;

    DOUBLE dValue =  va_arg (vaArgListIn, DOUBLE);

    sprintf (szBufferOut, szFormatString, dValue);

    AppendString (szBufferOut);

    return (TRUE);
    };

  // pointers

  if (cNextChar == 'p')
    {
    szFormatString [uFSIndex    ] = cNextChar;
    szFormatString [uFSIndex + 1] = 0;

    PVOID pValue =  va_arg (vaArgListIn, PVOID);

    sprintf (szBufferOut, szFormatString, pValue);

    AppendString (szBufferOut);

    return (TRUE);

    };

  // we don't support 'n', which is the number of chars written so far.
  // we don't support 'S', which is a Unicode string

  if (cNextChar == 'n')
    {
    DBG_ERROR ("The 'n' (store number of chars written so far) format marker is not supported by RStr");
    return FALSE;
    };
  if (cNextChar == 'S')
    {
    DBG_ERROR ("The 'S' (print Unicode string) format marker is not supported by RStr");
    return FALSE;
    };

  // Not a processed marker.  Just write the character

  AppendChar (cNextChar);

  return (TRUE);
  };


//------------------------------------------------------------------------
VOID  RStr::ToLower  (VOID)
  {
  for (UINT  uIndex = 0; uIndex < uStringLength; ++uIndex)
    {
    pszBuffer [uIndex] = tolower (pszBuffer [uIndex]);
    };
  uHash = 0;
  };


//------------------------------------------------------------------------
VOID  RStr::ToUpper  (VOID)
  {
  for (UINT  uIndex = 0; uIndex < uStringLength; ++uIndex)
    {
    pszBuffer [uIndex] = toupper (pszBuffer [uIndex]);
    };
  uHash = 0;
  };


//------------------------------------------------------------------------
INT32  RStr::StripTrailingChar (UINT32  uCharIn)
  {
  INT32   uNumCharsStripped = 0;

  while ((uStringLength > 0) && (UINT32 (pszBuffer [uStringLength - 1]) == uCharIn))
    {
    pszBuffer [uStringLength - 1] = 0;
    --uStringLength;
    ++uNumCharsStripped;
    };
  uHash = 0;
  return (uNumCharsStripped);
  };


//------------------------------------------------------------------------
INT32  RStr::StripTrailingWhitespace (BOOL  bStripEOL)
  {
  INT32   uNumCharsStripped = 0;

  while ((uStringLength > 0) &&
         ((pszBuffer [uStringLength - 1] == ' ') ||
          (pszBuffer [uStringLength - 1] == '\t') ||

          ((bStripEOL) && (pszBuffer [uStringLength - 1] == '\r')) ||
          ((bStripEOL) && (pszBuffer [uStringLength - 1] == '\n'))

          )
         )
    {
    pszBuffer [uStringLength - 1] = 0;
    --uStringLength;
    ++uNumCharsStripped;
    };
  uHash = 0;
  return (uNumCharsStripped);
  };

//------------------------------------------------------------------------
INT32  RStr::StripLeadingWhitespace (VOID)
  {
  UINT32  uCount = 0;

  while ((uCount < uStringLength) &&
         ((pszBuffer [uCount] == ' ')  ||
          (pszBuffer [uCount] == '\t') ||

          (pszBuffer [uCount] == '\r') ||
          (pszBuffer [uCount] == '\n')

         )
        )
    {
    ++uCount;
    };
  if (uCount > 0)
    {
    ClipLeft (uCount);
    };
  return INT32 (uCount);
  };

//------------------------------------------------------------------------
VOID RStr::SetBufferSize  (UINT32   uSizeIn)
  {
  // This method sets the size of the memory allocated for this string.
  //  If the caller requests a size of zero, the buffer will be set to the
  //  constant empty string because this method guarantees a terminating zero.

  if (uBufferSize == (uSizeIn + 1)) return; // already the desired size

  if (uSizeIn == 0)
    {
    FreeBuffer ();
    pszBuffer = const_cast <char *> (RStr::szEmpty);
    uStringLength = 0;
    uBufferSize = 0;
    uHash = 0;
    return;
    };

  // allocate one larger than uSizeIn to account for the terminating null.
  if (pszBuffer == const_cast <char *> (RStr::szEmpty))
    {
    pszBuffer = (char *) malloc (uSizeIn + 1);
    }
  else
    {
    //DBG_INFO ("SetBufferSize pszBuffer (%x) NewSize (%d) uBufferSize (%d)",
    //          pszBuffer,
    //          uSizeIn + 1,
    //          uBufferSize);

    char *  pszNewBuffer = (char *) realloc (pszBuffer, uSizeIn + 1);
    if (pszNewBuffer != NULL)
      {
      pszBuffer = pszNewBuffer;
      }
    else
      {
      // failed to grow buffer!!!!!!
      DBG_ERROR ("RStr::SetBufferSize : Memory allocation failure!!!!");
      return;
      }
    };

  if ((uBufferSize > 0) &&
      (uSizeIn < (uBufferSize - 1)))
    {
    // smaller buffer.

    if (uStringLength > uSizeIn)
      {
      uStringLength = uSizeIn;
      };
    };

  uBufferSize = uSizeIn + 1;
  //printf ("SetBufferSize pszBuffer (%x) now sized uBufferSize (%d)\n",
  //          pszBuffer,
  //          uBufferSize);


  // set the terminating null
  pszBuffer [uSizeIn] = '\0';
  uHash = 0;
  };


//------------------------------------------------------------------------
RAVEN_EXPORT RStr  operator+      (const RStr & strOne, const RStr & strTwo)
  {
  RStr   strReturn (strOne.uStringLength + strTwo.Length ());


  strReturn =  strOne;
  strReturn += strTwo;
  return (strReturn);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT RStr  operator+      (const RStr & strOne, const char pszTwo [])
  {
  RStr   strReturn (strOne.uStringLength + strlen (pszTwo));


  strReturn =  strOne;
  strReturn += pszTwo;
  return (strReturn);
  };

//------------------------------------------------------------------------
RAVEN_EXPORT RStr  operator+      (const char * pszOne, const RStr & strTwo)
  {
  RStr   strReturn (strlen (pszOne) + strTwo.uStringLength);


  strReturn =  pszOne;
  strReturn += strTwo;
  return (strReturn);
  };

//------------------------------------------------------------------------
RAVEN_EXPORT RStr  operator+      (const RStr & strOne, const UINT32   uTwo)
  {
  RStr   strReturn (strOne.uStringLength + 1);


  strReturn =  strOne;
  strReturn += uTwo;
  return (strReturn);
  };

//------------------------------------------------------------------------
RAVEN_EXPORT RStr  operator+      (const UINT32   uOne,  const RStr & strTwo)
  {
  RStr   strReturn (1 + strTwo.uStringLength);


  strReturn += uOne;
  strReturn += strTwo;
  return (strReturn);
  };

//------------------------------------------------------------------------
BOOL  RStr::StartsWith  (const char *  szIn,
                         INT           iBufferOffsetIn)
  {
  INT  iLenIn = strlen (szIn);
  if ((UINT32)iLenIn > Length ()) return (FALSE);
  if (strncmp (GetBufferPtr (iBufferOffsetIn), szIn, iLenIn) == 0) return (TRUE);
  return (FALSE);
  };

//------------------------------------------------------------------------
BOOL  RStr::StartsWith  (UINT32  uIn,
                         INT     iBufferOffsetIn)
  {
  if (Length () < 1) return (FALSE);
  return (*GetBufferPtr (iBufferOffsetIn) == (char) uIn);
  };

//------------------------------------------------------------------------
BOOL  RStr::EndsWith  (const char *  szIn)
  {
  INT  iLenIn = strlen (szIn);
  if ((UINT32)iLenIn > Length ()) return (FALSE);
  if (strncmp (GetBufferPtr (Length () - iLenIn), szIn, iLenIn) == 0) return (TRUE);
  return (FALSE);
  };

//------------------------------------------------------------------------
BOOL  RStr::EndsWith  (UINT32  uIn)
  {
  if (Length () < 1) return (FALSE);
  return (*GetBufferPtr (Length () - 1) == (char) uIn);
  };

//------------------------------------------------------------------------
VOID  RStr::ToHex  (RStr &  strHexOut)
  {
  const char   acHex [] = "0123456789ABCDEF";
  INT  iLength = this->Length ();
  strHexOut.GrowAbsolute (iLength);
  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    INT  iCurr = GetAt (iIndex);
    strHexOut.SetAt (iIndex * 2,       acHex[iCurr >> 4]);
    strHexOut.SetAt ((iIndex * 2) + 1, acHex[iCurr & 0x0f]);
    };
  };

//------------------------------------------------------------------------
VOID  RStr::DebugHex  (INT  iOffsetIn,
                       INT  iCountIn)
  {
  RStr  strOut;
  RStr  strOutText;

  strOut.Format ("%x  ", iOffsetIn);
  for (INT  iIndex = 0; iIndex < iCountIn; ++iIndex)
    {
    strOut.AppendFormat ("%x ", GetAt(iOffsetIn + iIndex));
    strOutText.AppendFormat ("%c", isalnum (GetAt(iOffsetIn + iIndex)) ? GetAt(iOffsetIn+iIndex) : '.');
    }
  strOut.AppendString (strOutText);
  DBG_INFO (strOut.AsChar ());
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator== (const RStr & strOne,
                              const RStr & strTwo)
  {
  if (strOne.Length () != strTwo.Length ()) return (FALSE);

  if ((strOne.uHash != 0) && (strTwo.uHash != 0) && (strOne.uHash != strTwo.uHash))
    {
    return (FALSE);
    }

  return (strcmp (strOne.AsChar (), strTwo.AsChar ()) == 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator== (const RStr &  strOne,
                              const char *  pszTwo)
  {
  return (strcmp (strOne.AsChar (), pszTwo) == 0);
  };


//------------------------------------------------------------------------
//BOOL operator== (RStr &  strOne,
//                 char *     pszTwo)
//  {
//  return (strcmp (strOne.AsChar (), pszTwo) == 0);
//  };


//------------------------------------------------------------------------
//BOOL operator== (RStr &        strOne,
//                 const char *  pszTwo)
//
//  {
//  return (strcmp (strOne.AsChar (), pszTwo) == 0);
//  };

//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator== (const char *  pszOne,
                              const RStr &  strTwo)
  {
  return (strcmp (pszOne, strTwo.AsChar ()) == 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator!= (const RStr & strOne,
                              const RStr & strTwo)
  {
  if (strOne.Length () != strTwo.Length ()) return (TRUE);

  if ((strOne.uHash != 0) && (strTwo.uHash != 0) && (strOne.uHash == strTwo.uHash))
    {
    return (FALSE);
    };

  return (strcmp (strOne.AsChar (), strTwo.AsChar ()) != 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator!= (const RStr &  strOne,
                              const char *  pszTwo)
  {
  return (strcmp (strOne.AsChar (), pszTwo) != 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator!= (const char *  pszOne,
                              const RStr &  strTwo)
  {
  return (strcmp (pszOne, strTwo.AsChar ()) != 0);
  };



//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator< (const RStr & strOne,
                             const RStr & strTwo)
  {
  if ((strOne.uHash != 0) && (strTwo.uHash != 0) && (strOne.uHash == strTwo.uHash))
    {
    return (FALSE);
    };

  return (strcmp (strOne.AsChar (), strTwo.AsChar ()) < 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator< (const RStr &  strOne,
                             const char *  pszTwo)
  {
  return (strcmp (strOne.AsChar (), pszTwo) < 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator< (const char *  pszOne,
                             const RStr &  strTwo)
  {
  return (strcmp (pszOne, strTwo.AsChar ()) < 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator> (const RStr & strOne,
                             const RStr & strTwo)
  {
  if ((strOne.uHash != 0) && (strTwo.uHash != 0) && (strOne.uHash == strTwo.uHash))
    {
    return (FALSE);
    };

  return (strcmp (strOne.AsChar (), strTwo.AsChar ()) > 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator> (const RStr &  strOne,
                             const char *  pszTwo)
  {
  return (strcmp (strOne.AsChar (), pszTwo) > 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator> (const char *  pszOne,
                             const RStr &  strTwo)
  {
  return (strcmp (pszOne, strTwo.AsChar ()) > 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator<= (const RStr & strOne,
                              const RStr & strTwo)
  {
  return (strcmp (strOne.AsChar (), strTwo.AsChar ()) <= 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator<= (const RStr &  strOne,
                              const char *  pszTwo)
  {
  return (strcmp (strOne.AsChar (), pszTwo) <= 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator<= (const char *  pszOne,
                              const RStr &  strTwo)
  {
  return (strcmp (pszOne, strTwo.AsChar ()) <= 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator>= (const RStr & strOne,
                              const RStr & strTwo)
  {
  return (strcmp (strOne.AsChar (), strTwo.AsChar ()) >= 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator>= (const RStr &  strOne,
                              const char *  pszTwo)
  {
  return (strcmp (strOne.AsChar (), pszTwo) >= 0);
  };


//------------------------------------------------------------------------
RAVEN_EXPORT BOOL operator>= (const char *  pszOne,
                              const RStr &  strTwo)
  {
  return (strcmp (pszOne, strTwo.AsChar ()) >= 0);
  };

















