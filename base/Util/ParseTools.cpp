/* -----------------------------------------------------------------
                             Parse Tools

     This module contains tools and utilities for in-place parsing
   of buffers in various formats.

   ----------------------------------------------------------------- */

// Modified BSD License:
//
// Copyright (c) 2017, Michael T. Duffy II.  All rights reserved.
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

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "ParseTools.hpp"

///< The goal of this module is to collect together the various string parsing
///< functions across Crow in a format-independent way.  This will allow
///< the independent modules to be more separated from the scene parsing code.

//-----------------------------------------------------------------------------
ParseTools::ParseTools ()
  {
  };

//-----------------------------------------------------------------------------
BOOL  ParseTools::IsKVPColon (RStrParser &  parserBufferIn,
                              RStr &        strKeyWordOut,
                              RStrParser &  parserValueOut)
  {
  // reads a line that has a keyword, a colon separator, then a quoted string value
  //    key : "value"

  INT  iSavedPos = parserBufferIn.GetCursorStart ();
  strKeyWordOut = parserBufferIn.GetWord (TRUE, "[]:");
  if (parserBufferIn.PeekChar () == ':')
    {
    parserBufferIn.SkipChars (1);
    parserBufferIn.SkipWhitespace ();
    parserBufferIn.GetQuoteString (&parserValueOut);
    parserValueOut.ResetCursor ();
    return (TRUE);
    };

  // pattern not found.  Revert to start position.
  parserBufferIn.SetCursorStart (iSavedPos);
  return (FALSE);
  }

//-----------------------------------------------------------------------------
BOOL  ParseTools::IsKVPBracket (RStrParser &  parserBufferIn,
                                RStr &        strKeyOut,
                                RStrParser &  parserValueOut)
  {
  // reads a line that has a keyword, then a bracketed value.  Strings are further quoted.
  //    key [value]
  //    key ["string value"]

  INT  iSavedPos = parserBufferIn.GetCursorStart ();
  strKeyOut = parserBufferIn.GetWord (TRUE, "[]:");

  if (parserBufferIn.PeekChar () == '[')
    {
    parserBufferIn.GetBracketString (&parserValueOut, RStr ("["), RStr ("]"), TRUE, FALSE);
    parserValueOut.ResetCursor ();
    parserValueOut.SkipWhitespace ();
    return (TRUE);
    };

  // pattern not found.  Revert to start position.
  parserBufferIn.SetCursorStart (iSavedPos);
  return (FALSE);
  }

//-----------------------------------------------------------------------------
BOOL  ParseTools::IsKVPEquals (RStrParser &  parserBufferIn,
                               RStr &        strKeyOut,
                               RStrParser &  parserValueOut)
  {
  INT  iSavedPos = parserBufferIn.GetCursorStart ();
  strKeyOut = parserBufferIn.GetWord (TRUE, "=");

  if (parserBufferIn.PeekChar () == '=')
    {
    parserBufferIn.SkipChars (1);
    parserBufferIn.SkipWhitespace ();
    parserValueOut = parserBufferIn.GetWord ();
    parserValueOut.ResetCursor ();
    return (TRUE);
    };

  // pattern not found.  Revert to start position.
  parserBufferIn.SetCursorStart (iSavedPos);
  return (FALSE);
  };

//-----------------------------------------------------------------------------
EStatus  ParseTools::GetKey (RStrParser &  parserIn,
                             UINT32        uSeparatorIn,
                             RStr &        strKeyOut)
  {
  strKeyOut.Empty ();

  parserIn.SkipWhitespace ();
  parserIn.GetQuoteString (&strKeyOut, TRUE, TRUE);

  if (parserIn.GetChar () != uSeparatorIn)
    {
    // format error.
    return (EStatus::kFailure);
    }
  parserIn.SkipWhitespace ();
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
VOID  ParseTools::GetIntArray (RStrParser &  parserIn,
                               UINT32        uBeginningCharIn,
                               UINT32        uEndingCharIn,
                               IntArray &    arrayOut)
  {
  // parse out integers separated by commas and optional whitespace.  Append to arrayOut.
  // Beginning and Ending characters will be skipped, if present.  Neither is required.

  RStr        strCurr;

  parserIn.SkipWhitespace ();
  if (parserIn.PeekChar() == uBeginningCharIn)
    {
    parserIn.SkipChars (1);
    parserIn.SkipWhitespace ();
    };

  INT iIndex = arrayOut.Length ();
  while ((!parserIn.IsEOL ()) &&
         (parserIn.PeekChar() != uEndingCharIn))
    {
    arrayOut.SetMinLength (iIndex + 1);
    arrayOut.SetAt (iIndex, parserIn.GetInt());
    ++iIndex;

    parserIn.SkipWhitespace ();
    if (parserIn.PeekChar() != ',') break;
    parserIn.SkipChars(1);
    parserIn.SkipWhitespace ();
    };

  if (parserIn.PeekChar() == uEndingCharIn)
    {
    parserIn.SkipChars (1);
    parserIn.SkipWhitespace ();
    };
  };

//-----------------------------------------------------------------------------
VOID  ParseTools::GetFloatArray (RStrParser &  parserIn,
                                 UINT32        uBeginningCharIn,
                                 UINT32        uEndingCharIn,
                                 FloatArray &  arrayOut)
  {
  // parse out floats separated by commas and optional whitespace.  Append to arrayOut.
  // Beginning and Ending characters will be skipped, if present.  Neither is required.

  RStr        strCurr;

  parserIn.SkipWhitespace ();
  if (parserIn.PeekChar() == uBeginningCharIn)
    {
    parserIn.SkipChars (1);
    parserIn.SkipWhitespace ();
    };

  INT iIndex = arrayOut.Length ();
  while ((!parserIn.IsEOL ()) &&
         (parserIn.PeekChar() != uEndingCharIn))
    {
    arrayOut.SetMinLength (iIndex + 1);
    arrayOut.SetAt (iIndex, parserIn.GetFloat());
    ++iIndex;

    parserIn.SkipWhitespace ();
    if (parserIn.PeekChar() != ',') break;
    parserIn.SkipChars(1);
    parserIn.SkipWhitespace ();
    };

  if (parserIn.PeekChar() == uEndingCharIn)
    {
    parserIn.SkipChars (1);
    parserIn.SkipWhitespace ();
    };
  };

//-----------------------------------------------------------------------------
VOID  ParseTools::GetDoubleArray (RStrParser &  parserIn,
                                 UINT32        uBeginningCharIn,
                                 UINT32        uEndingCharIn,
                                 DoubleArray &  arrayOut)
  {
  // parse out doubles separated by commas and optional whitespace.  Append to arrayOut.
  // Beginning and Ending characters will be skipped, if present.  Neither is required.

  RStr        strCurr;

  parserIn.SkipWhitespace ();
  if (parserIn.PeekChar() == uBeginningCharIn)
    {
    parserIn.SkipChars (1);
    parserIn.SkipWhitespace ();
    };

  INT iIndex = arrayOut.Length ();
  while ((!parserIn.IsEOL ()) &&
         (parserIn.PeekChar() != uEndingCharIn))
    {
    arrayOut.SetMinLength (iIndex + 1);
    arrayOut.SetAt (iIndex, parserIn.GetDouble());
    ++iIndex;

    parserIn.SkipWhitespace ();
    if (parserIn.PeekChar() != ',') break;
    parserIn.SkipChars(1);
    parserIn.SkipWhitespace ();
    };

  if (parserIn.PeekChar() == uEndingCharIn)
    {
    parserIn.SkipChars (1);
    parserIn.SkipWhitespace ();
    };
  };

//-----------------------------------------------------------------------------
VOID  ParseTools::GetStringArray (RStrParser &  parserIn,
                                  UINT32        uBeginningCharIn,
                                  UINT32        uEndingCharIn,
                                  RStrArray &   arrayOut)
  {
  // parse out integers separated by commas and optional whitespace.  Append to arrayOut.
  // Beginning and Ending characters will be skipped, if present.  Neither is required.

  RStr        strCurr;

  parserIn.SkipWhitespace ();
  if (parserIn.PeekChar() == uBeginningCharIn)
    {
    parserIn.SkipChars (1);
    parserIn.SkipWhitespace ();
    };

  INT iIndex = arrayOut.Length ();
  while ((!parserIn.IsEOL ()) &&
         (parserIn.PeekChar() != uEndingCharIn))
    {
    arrayOut.SetMinLength (iIndex + 1);
    arrayOut.SetAt (iIndex, parserIn.GetQuoteString(&strCurr));
    ++iIndex;

    parserIn.SkipWhitespace ();
    if (parserIn.PeekChar() != ',') break;
    parserIn.SkipChars(1);
    parserIn.SkipWhitespace ();
    };

  if (parserIn.PeekChar() == uEndingCharIn)
    {
    parserIn.SkipChars (1);
    parserIn.SkipWhitespace ();
    };
  };

//-----------------------------------------------------------------------------
BOOL  ParseTools::SkipChar  (RStrParser &  parserIn,
                             UINT32        uCharIn)
  {
  BOOL  bFound = FALSE;

  // To make parsing easier, skip any leading whitespace and EOLs
  parserIn.SkipWhitespace ();

  if (parserIn.PeekChar () == uCharIn)
    {
    bFound = TRUE;
    parserIn.SkipChars (1);
    };
  parserIn.SkipWhitespace ();
  return (bFound);
  };




















