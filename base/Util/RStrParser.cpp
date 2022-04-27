/* -----------------------------------------------------------------
                           String Parser Class

    The RStrParser class is used to create a RStr that has member functions
   for extracting, parsing, and manipulating the contents of the string.

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

// *****************************************************************************
//  Compile Directives
// *****************************************************************************

// #pragma pack (1);

// *****************************************************************************
//  Header Files
// *****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#ifdef LINUX
#include <unistd.h>
#endif

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Util/RStrParser.hpp"
#include "Sys/FilePath.hpp"

const UINT32   RStrParser::kNULL         = 0x00;
const UINT32   RStrParser::kLF           = 0x0a;
const UINT32   RStrParser::kCR           = 0x0d;
const UINT32   RStrParser::kSPACE        = 0x20;
const UINT32   RStrParser::kTAB          = 0x09;
const UINT32   RStrParser::kDOUBLEQUOTES = 0x22;
const UINT32   RStrParser::kSINGLEQUOTES = 0x27;
const UINT32   RStrParser::kESCAPE       = 0x5c;

RStr  operator*  (const RStrParser &  strIn, RegEx &  regexIn)    {return (regexIn.Match (strIn, strIn.GetCursorStart ()));};
RStr  operator*  (RegEx &  regexIn, const RStrParser &    strIn)  {return (regexIn.Match (strIn, strIn.GetCursorStart ()));};

//------------------------------------------------------------------------------
RStrParser::RStrParser  ()
  {

  Init ();
  };


//------------------------------------------------------------------------------
RStrParser::RStrParser  (const RStr &   strIn)
  {
  Init ();
  _ParserSet (strIn, strIn.uHash != 0);
  };


//------------------------------------------------------------------------------
RStrParser::RStrParser  (const RStrParser &   parserIn) : RStr ()
  {
  Init ();
  _ParserSet (parserIn, parserIn.uHash != 0);
  };


//------------------------------------------------------------------------------
RStrParser::RStrParser  (const char *   pszIn)
  {
  Init ();
  _ParserSet (pszIn);
  };


//------------------------------------------------------------------------------
RStrParser::~RStrParser  ()
  {
  };


//------------------------------------------------------------------------------
VOID  RStrParser::Init  (VOID)
  {
  eSkipComments = kNone;
  ResetCursor  ();

  bBufferedWriting     = false;
  strBufferedFilename  = "";
  iBufferedWriteSize   = 4096;

  bGreedyRead = true;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::ResetCursor  (VOID)
  {

  iCursorStart = 0;
  iCursorEnd   = 0;
  iLineEnd     = 0;
  FindLineEnd ();
  };


//------------------------------------------------------------------------------
VOID  RStrParser::FindLineEnd  (VOID)
  {

  iLineEnd = -1;
  INT32  iEndSearch = iCursorStart;

  while (! IsEOL (iEndSearch))
    {
    if (IsComment (iEndSearch))
      {
      break;
      };
    ++iEndSearch;
    };
  iLineEnd = iEndSearch;
  };

//------------------------------------------------------------------------------
INT32  RStrParser::FindInLine  (const RStr &  strIn) const
  {
  return (Find (strIn, iCursorStart, iLineEnd));
  };

//------------------------------------------------------------------------------
INT32  RStrParser::FindInLine  (const char *  szIn) const
  {
  return (Find (szIn, iCursorStart, iLineEnd));
  };

//------------------------------------------------------------------------------
INT32  RStrParser::GetLineNumber (INT32  iCursorPosIn)
  {
  if (iCursorPosIn < 0) {iCursorPosIn = iCursorStart;};

  INT32 iSavedPos = iCursorStart;
  iCursorStart = 0;
  INT32 iLineCount = 0;
  FindLineEnd ();

  while (!IsEOF() && (iCursorStart <= iCursorPosIn))
    {
    GotoEOL ();
    SkipEOL ();
    ++iLineCount;
    };
  iCursorStart = iSavedPos;

  return (iLineCount);
  };

//------------------------------------------------------------------------------
RStr &  RStrParser::Set  (const RStr &  strIn,
                          BOOL          bCalcHashIn)
  {
  return (_ParserSet (strIn, bCalcHashIn));
  };

//------------------------------------------------------------------------------
RStr &  RStrParser::_ParserSet  (const RStr &  strIn,
                                 BOOL          bCalcHashIn)
  {
  // cppcheck-suppress constVariable
  RStr &  strReturn = this->_Set (strIn, bCalcHashIn || (strIn.uHash != 0));
  ResetCursor ();
  FindLineEnd ();
  return (strReturn);
  };

//------------------------------------------------------------------------------
RStr &  RStrParser::Set  (const char *  pszIn,
                          BOOL          bCalcHashIn)
  {
  return (_ParserSet (pszIn, bCalcHashIn));
  };

//------------------------------------------------------------------------------
RStr &  RStrParser::_ParserSet  (const char *  pszIn,
                                 BOOL          bCalcHashIn)
  {
  // cppcheck-suppress constVariable
  RStr &  strReturn = this->_Set (pszIn, bCalcHashIn);
  ResetCursor ();
  FindLineEnd ();
  return (strReturn);
  };

//------------------------------------------------------------------------------
INT  RStrParser::SkipWhitespace  (BOOL  bSkipEOL)
  {
  INT  iStartingPos = iCursorStart;


  if (IsEOF (iCursorStart)) return (0);

  while (! IsEOF (iCursorStart))
    {
    UINT32   uCurr = GetAt (iCursorStart);

    if (IsWhitespace (uCurr))
      {
      ++iCursorStart;
      }
    else if (bSkipEOL && ((uCurr == kLF) || (uCurr == kCR)))
      {
      while ((uCurr == kLF) || (uCurr == kCR))
        {
        ++iCursorStart;
        uCurr = GetAt (iCursorStart);
        };
      FindLineEnd ();
      }
    else if (IsComment (iCursorStart))
      {
      // skip comments as whitespace, if comment skipping is turned on.
      SkipComment ();
      if (bSkipEOL)
        {
        GotoNextLine ();
        }
      else
        {
        break;
        };
      }
    else
      {
      break;
      };
    };
  return (iCursorStart - iStartingPos);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::SkipEOL  (VOID)
  {
  UINT32   uCurr = GetAt (iCursorStart);

  if (uCurr == kCR)
    {
    ++iCursorStart;
    uCurr = GetAt (iCursorStart);
    };

  if (uCurr == kLF)
    {
    ++iCursorStart;
    };
  FindLineEnd ();
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsEOL  (INT32  iPosition)
  {
  if (iPosition == -1)  iPosition = iCursorStart;

  if (IsEOF (iPosition)) return (true);

  UINT32   uCurr = GetAt (iPosition);

  return ((uCurr == kNULL) ||
          (uCurr == kLF) ||
          (uCurr == kCR) ||
          (IsEOFAscii (iPosition)) ||
          (iPosition == iLineEnd));
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsEOF  (INT32  iPosition)
  {
  if (iPosition == -1)  iPosition = iCursorStart;

  if (uStringLength == 0) return (true);

  return (iPosition >= static_cast <int> (uStringLength));
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsEOFAscii  (INT32  iPosition)
  {
  if (iPosition == -1)  iPosition = iCursorStart;

  if (uStringLength == 0) return (true);

  if (iPosition >= static_cast <int> (uStringLength)) return (true);

  if (GetAt (iPosition) == kNULL) return (true);

  return (false);
  };

//------------------------------------------------------------------------------
BOOL  RStrParser::SkipToChar  (UINT32  uCharToSkipIn)
  {
  INT32  iPos = FindChar (uCharToSkipIn, GetCursorStart ());

  if (iPos != -1)
    {
    SetCursorStart (iPos);
    }
  return (iPos >= 0);
  };

//------------------------------------------------------------------------------
BOOL  RStrParser::SkipPastChar  (UINT32  uCharToSkipPastIn,
                                 BOOL    bSkipWhitespaceIn)
  {
  INT32  iPos = FindChar (uCharToSkipPastIn, GetCursorStart ());

  if (iPos != -1)
    {
    SetCursorStart (iPos + 1);
    if (bSkipWhitespaceIn)
      {
      SkipWhitespace ();
      };
    }
  return (iPos >= 0);
  };

//------------------------------------------------------------------------------
VOID  RStrParser::GotoEOL  (VOID)
  {
  if (IsEOF ()) return;

  // reset the line end so it won't trigger the EOL.
  iLineEnd = -1;

  // skip until an actual EOL
  while (! IsEOL ())
    {
    ++iCursorStart;
    };
  // calculate the line end.
  FindLineEnd ();
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::GotoNextLine  (VOID)
  {
  if (IsEOF ()) return (false);

  // reset the line end so it won't trigger the EOL.
  iLineEnd = -1;

  // skip until an actual EOL
  while (! IsEOL ())
    {
    ++iCursorStart;
    };
  SkipEOL ();

  // calculate the line end.
  FindLineEnd ();

  return (true);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::GetLine  (RStr &  strOut)
  {
  // read from the current position until the line end, and return
  //  a RStr with that info.
  FindLineEnd ();

  INT32  iStringLength = iLineEnd - iCursorStart;

  GetMiddle (iCursorStart, iStringLength, strOut);
  SkipChars (strOut.Length ());
  SkipComment ();
  if (IsEOL())
    {
    GotoNextLine ();
    };
  };


//------------------------------------------------------------------------------
RStr  RStrParser::GetLine  (VOID)
  {
  RStr   strOut ("");
  GetLine (strOut);
  return (strOut);
  };


//------------------------------------------------------------------------------
RStr &  RStrParser::GetWord  (BOOL          bSkipEOL,
                              const char *  szAlsoInvalidChars)
  {
  static RStr   strOut;

  GetWord (strOut, bSkipEOL, szAlsoInvalidChars);

  strOut.CalcHash ();
  return (strOut);
  };


//------------------------------------------------------------------------------
const char *  RStrParser::GetWord  (RStr &        strOut,
                                    BOOL          bSkipEOL,
                                    const char *  szAlsoInvalidChars)
  {
  GetWord (&strOut, bSkipEOL, szAlsoInvalidChars);
  return (strOut.AsChar ());
  };


//------------------------------------------------------------------------------
const char *  RStrParser::GetWord  (RStr *        pstrOut,
                                    BOOL          bSkipEOL,
                                    const char *  szAlsoInvalidChars)
  {
  // Note:  GetWord needs to properly handle strings contained within quotes.
  //  to be implemented

  //UINT32  uCharCurr;

  SkipWhitespace (bSkipEOL);

  // check for quoted strings
  if ((PeekChar () == kDOUBLEQUOTES) ||
      (PeekChar () == kSINGLEQUOTES))
    {
    GetQuoteString  (pstrOut, bSkipEOL);
    }
  else
    {
    // normal word

    UINT32   uCurr = GetAt (iCursorStart);
    INT32    iCopyStart = iCursorStart;

    while ((! IsWhitespace (uCurr)) &&
           (! IsEOL ()            ))
      {
      if (szAlsoInvalidChars != NULL)
        {
        if (strchr (szAlsoInvalidChars, uCurr) != NULL)
          {
          // found a char we want to signal end of word
          break;
          };
        };


      ++iCursorStart;
      uCurr = GetAt (iCursorStart);
      };

    if (pstrOut != NULL)
      {
      INT32  iStringLength = iCursorStart - iCopyStart;
      GetMiddle (iCopyStart, iStringLength, *pstrOut);
      };
    };

  SkipWhitespace (bSkipEOL);
  return (pstrOut == NULL ? NULL : pstrOut->AsChar ());
  };


//------------------------------------------------------------------------------
RStr  RStrParser::GetAlphaNum  (BOOL  bSkipEOL)
  {
  RStr   strOut;

  GetAlphaNum (strOut, bSkipEOL);

  return (strOut);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::GetAlphaNum  (RStr &  strOut,
                                BOOL    bSkipEOL)
  {
  GetAlphaNum (&strOut, bSkipEOL);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::GetAlphaNum  (RStr *  pstrOut,
                                BOOL    bSkipEOL)
  {

  // Note:  GetAlphaNum needs to properly handle strings contained within quotes.
  //  to be implemented

  // this routine reads A-Za-z0-9_
  //UINT32  uCharCurr;


  SkipWhitespace (bSkipEOL);

  // check for quoted strings
  if ((PeekChar () == kDOUBLEQUOTES) ||
      (PeekChar () == kSINGLEQUOTES))
    {
    GetQuoteString  (pstrOut, bSkipEOL);
    }
  else
    {
    // normal word

    UINT32   uCurr = GetAt (iCursorStart);
    INT32    iCopyStart = iCursorStart;

    while ((! IsWhitespace (uCurr)) &&
           ((isalnum (uCurr)) || (uCurr == '_')) &&
           (! IsEOL ()            ))
      {
      ++iCursorStart;
      uCurr = GetAt (iCursorStart);
      };

    if (pstrOut != NULL)
      {
      INT32  iStringLength = iCursorStart - iCopyStart;
      GetMiddle (iCopyStart, iStringLength, *pstrOut);
      };
    };

  SkipWhitespace (bSkipEOL);
  };



//------------------------------------------------------------------------------
UINT32  RStrParser::GetChar (BOOL  bSkipWhitespace)
  {

  // extract the next character
  if (bSkipWhitespace)
    {
    SkipWhitespace ();
    };

  UINT32   uCurr = (! IsEOF ()) ? GetAt (iCursorStart) : 0;

  if (! IsEOL ())
    {
    ++iCursorStart;
    };

  return (uCurr);
  };


//------------------------------------------------------------------------------
INT32  RStrParser::GetInt (BOOL  bSkipEOL)
  {
  RStr   strWord;

  // extract the characters that make up a valid number
  SkipWhitespace (bSkipEOL);

  UINT32   uCurr = GetAt (iCursorStart);
  INT32    iCopyStart = iCursorStart;

  while ( ((isdigit (uCurr)) ||
           (((uCurr == '-') || (uCurr == '+')) && (iCopyStart == iCursorStart)) ||
           (((uCurr == 'x') || (uCurr == 'X')) && (iCopyStart == iCursorStart + 1)) )
               &&
          (! IsEOL ()) )
    {
    ++iCursorStart;
    uCurr = GetAt (iCursorStart);
    };

  INT32  iStringLength = iCursorStart - iCopyStart;
  GetMiddle (iCopyStart, iStringLength, strWord);

  SkipWhitespace (bSkipEOL);

  return (static_cast <INT32> (strtol (strWord.AsChar (), NULL, 10)));
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsInt (BOOL  bSkipEOL)
  {
  INT32  iSavedPos = iCursorStart;

  // extract the characters that make up a valid number
  SkipWhitespace (bSkipEOL);

  UINT32   uCurr = GetAt (iCursorStart);
  INT32    iCopyStart = iCursorStart;

  while ( ((isdigit (uCurr)) ||
           (((uCurr == '-') || (uCurr == '+')) && (iCopyStart == iCursorStart)) ||
           (((uCurr == 'x') || (uCurr == 'X')) && (iCopyStart == iCursorStart + 1)) )
               &&
          (! IsEOL ()) )
    {
    ++iCursorStart;
    uCurr = GetAt (iCursorStart);
    };

  INT32  iStringLength = iCursorStart - iCopyStart;

  iCursorStart = iSavedPos;

  // special case out strings that start with a minus sign.
  if ((iStringLength == 1) && (PeekChar () == '-')) return (FALSE);

  return (iStringLength > 0);
  };


//------------------------------------------------------------------------------
UINT32  RStrParser::GetUInt  (BOOL  bSkipEOL)
  {
  RStr   strWord;

  // extract the characters that make up a valid number
  SkipWhitespace (bSkipEOL);

  UINT32   uCurr = GetAt (iCursorStart);
  INT32    iCopyStart = iCursorStart;

  while ( ((isdigit (uCurr)) ||
           (((uCurr == '-') || (uCurr == '+')) && (iCopyStart == iCursorStart)) ||
           (((uCurr == 'x') || (uCurr == 'X')) && (iCopyStart == iCursorStart + 1)) )
               &&
          (! IsEOL ()) )
    {
    ++iCursorStart;
    uCurr = GetAt (iCursorStart);
    };

  INT32  iStringLength = iCursorStart - iCopyStart;
  GetMiddle (iCopyStart, iStringLength, strWord);

  SkipWhitespace (bSkipEOL);

  return (static_cast <UINT32> (strtoul (strWord.AsChar (), NULL, 10)));
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsUInt  (BOOL  bSkipEOL)
  {
  INT32  iSavedPos = iCursorStart;

  // extract the characters that make up a valid number
  SkipWhitespace (bSkipEOL);

  UINT32   uCurr = GetAt (iCursorStart);
  INT32    iCopyStart = iCursorStart;

  while ( ((isdigit (uCurr)) ||
           (((uCurr == '-') || (uCurr == '+')) && (iCopyStart == iCursorStart)) ||
           (((uCurr == 'x') || (uCurr == 'X')) && (iCopyStart == iCursorStart + 1)) )
               &&
          (! IsEOL ()) )
    {
    ++iCursorStart;
    uCurr = GetAt (iCursorStart);
    };

  INT32  iStringLength = iCursorStart - iCopyStart;

  iCursorStart = iSavedPos;

  // special case out strings that start with a minus sign.
  if ((iStringLength == 1) && (PeekChar () == '-')) return (FALSE);

  return (iStringLength > 0);
  };


//------------------------------------------------------------------------------
FLOAT  RStrParser::GetFloat  (BOOL  bSkipEOL)
  {
  RStr   strWord ("");

  // extract the characters that make up a valid number
  SkipWhitespace (bSkipEOL);

  UINT32   uCurr = GetAt (iCursorStart);
  INT32    iCopyStart = iCursorStart;

  while ( ((isdigit (uCurr)) ||
           (uCurr == 'e') || (uCurr == 'E') ||
           (((uCurr == '-') || (uCurr == '+')) && ((iCopyStart == iCursorStart) || ( (GetAt (iCursorStart-1) == 'e') || (GetAt (iCursorStart-1) == 'E'))  )) ||
           (uCurr == '.') )
               &&
          (! IsEOL ()) )
    {
    ++iCursorStart;
    uCurr = GetAt (iCursorStart);
    };

  INT32  iStringLength = iCursorStart - iCopyStart;
  if (iStringLength)
    {
    GetMiddle (iCopyStart, iStringLength, strWord);
    };

  SkipWhitespace (bSkipEOL);

  return (FLOAT (strtod (strWord.AsChar (), NULL)));
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsFloat  (BOOL  bSkipEOL,
                            BOOL  bFractionalRequired)
  {
  INT32  iSavedPos = iCursorStart;

  // extract the characters that make up a valid number
  SkipWhitespace (bSkipEOL);

  UINT32   uCurr = GetAt (iCursorStart);
  INT32    iCopyStart = iCursorStart;
  BOOL     bHasFractional = FALSE;

  while ( ((isdigit (uCurr)) ||
           (uCurr == 'e') || (uCurr == 'E') ||
           (((uCurr == '-') || (uCurr == '+')) && ((iCopyStart == iCursorStart) || ( (GetAt (iCursorStart-1) == 'e') || (GetAt (iCursorStart-1) == 'E'))  )) ||
           (uCurr == '.') )
               &&
          (! IsEOL ()) )
    {
    if ((uCurr == 'e') || (uCurr == 'E') || (uCurr == '.'))
      {
      bHasFractional = TRUE;
      };

    ++iCursorStart;
    uCurr = GetAt (iCursorStart);
    };

  INT32  iStringLength = iCursorStart - iCopyStart;

  iCursorStart = iSavedPos;

  // special case out strings that start with a minus sign.
  if ((iStringLength == 1) && (PeekChar () == '-')) return (FALSE);

  if (bFractionalRequired && !bHasFractional) return (FALSE);

  return (iStringLength > 0);
  };


//------------------------------------------------------------------------------
DOUBLE  RStrParser::GetDouble  (BOOL  bSkipEOL)
  {
  RStr   strWord ("");

  // extract the characters that make up a valid number
  SkipWhitespace (bSkipEOL);

  UINT32   uCurr = GetAt (iCursorStart);
  INT32    iCopyStart = iCursorStart;

  while ( ((isdigit (uCurr)) ||
           (uCurr == 'e') || (uCurr == 'E') ||
           (((uCurr == '-') || (uCurr == '+')) && (iCopyStart == iCursorStart)) ||
           (uCurr == '.')  )
               &&
          (! IsEOL ()) )
    {
    ++iCursorStart;
    uCurr = GetAt (iCursorStart);
    };

  INT32  iStringLength = iCursorStart - iCopyStart;
  if (iStringLength)
    {
    GetMiddle (iCopyStart, iStringLength, strWord);
    };

  SkipWhitespace (bSkipEOL);

  return (strtod (strWord.AsChar (), NULL));
  };


//------------------------------------------------------------------------------
const char *  RStrParser::GetQuoteString  (RStr *  pstrOut,
                                           BOOL    bSkipEOL,
                                           BOOL    bCalcHashIn,
                                           BOOL    bReplaceString)
  {

  // Note:  May still need to deal with slashes on the end of lines to
  //         split strings across multiple lines.
  //  to be implemented
  UINT32       uCharCurr;

  if (pstrOut != NULL)
    {
    *pstrOut = "";
    };

  if (PeekChar () == kDOUBLEQUOTES)
    {
    // Get everything between the quotes, and unescape escaped characters.
    SkipChars (1);

    while (! IsEOF ())
      {
      uCharCurr = PeekChar ();
      SkipChars (1);
      if (uCharCurr == kDOUBLEQUOTES)
        {
        break;
        };

      // Note:  I'm skipping translation of escaped characters here
      //        and leaving them escaped, so that they may be passed
      //        to library functions.
      if (uCharCurr == kESCAPE)
        {
        uCharCurr = PeekChar ();
        SkipChars (1);

        switch (uCharCurr)
          {
          case 'a':  uCharCurr = '\a'; break;
          case 'b':  uCharCurr = '\b'; break;
          case 'f':  uCharCurr = '\f'; break;
          case 'n':  uCharCurr = '\n'; break;
          case 'r':  uCharCurr = '\r'; break;
          case 't':  uCharCurr = '\t'; break;
          case 'v':  uCharCurr = '\v'; break;
          case '0':  uCharCurr = '\0'; break;
          default: break;
          };
        };


      if (pstrOut != NULL)
        {
        pstrOut->AppendChar (uCharCurr);
        };
      };
    }
  else if (PeekChar () == kSINGLEQUOTES)
    {
    // Get everything between the quotes, and unescape escaped characters.
    SkipChars (1);

    while (! IsEOF ())
      {
      uCharCurr = PeekChar ();
      SkipChars (1);
      // pass escaped quotes explicitly so they don't terminate parsing.
      if (uCharCurr == kSINGLEQUOTES)
        {
        break;
        };
      if (uCharCurr == kESCAPE)
        {
        uCharCurr = PeekChar ();
        SkipChars (1);
        if (pstrOut != NULL)
          {
          pstrOut->AppendChar (kESCAPE);
          };
        };

      if (pstrOut != NULL)
        {
        pstrOut->AppendChar (uCharCurr);
        };
      };
    }
  else
    {
    // get everything until the end of the line.

    while (! IsEOL ())
      {
      if ((! bGreedyRead) && (IsWhitespace (PeekChar ())))
        {
        break;
        };

      if (pstrOut != NULL)
        {
        pstrOut->AppendChar (PeekChar ());
        };
      SkipChars (1);
      };
    };

  if (bCalcHashIn && (pstrOut != NULL))
    {
    pstrOut->CalcHash ();
    };

  SkipWhitespace (bSkipEOL);
  return (pstrOut == NULL ? NULL : pstrOut->AsChar ());
  };


//------------------------------------------------------------------------------
VOID  RStrParser::IncIntAtCursor (INT  iMultiplierIn)
  {
  // calc which column the cursor is under
  if (isdigit (GetAt (iCursorStart)))
    {
    StripTrailingWhitespace (TRUE);
    INT  iOrigLength = Length ();
    INT  iTensExp = iOrigLength - iCursorStart - 1;

    iCursorStart = 0;
    INT  iValueOrig = GetInt ();
    INT  iValue = iValueOrig + iMultiplierIn * (INT) pow (10, iTensExp);
    // debugging
    //printf ("Orig %d  New %d  Tens %d\n", iValueOrig, iValue, iTensExp);
    Format ("%d", iValue);
    INT  iNewLength = Length ();

    // now try to put the cursor under the appropriate digit
    iCursorStart = iNewLength - iTensExp - 1;
    };
  };


//------------------------------------------------------------------------------
VOID  RStrParser::IncFloatAtCursor (FLOAT fMultiplierIn)
  {
  // calc which column the cursor is under
  if (isdigit (GetAt (iCursorStart)))
    {
    StripTrailingWhitespace (TRUE);
    INT  iOrigLength = Length ();
    INT  iDecimal = FindChar ('.');
    INT  iTensExp = iOrigLength - iCursorStart - 1;
    if (iDecimal != -1)
      {
      // there is a decimal point.  Calc the correct tens exponent.
      if (iCursorStart < iDecimal)
        {
        // cursor over whole number part
        iTensExp = iDecimal - iCursorStart - 1;
        }
      else
        {
        // cursor over fractional part
        //               2            1
        iTensExp = iDecimal - iCursorStart;
        };
      };

    iCursorStart = 0;
    FLOAT  fValueOrig = GetFloat ();
    FLOAT  fValue = fValueOrig + fMultiplierIn * FLOAT (pow (10.0, (DOUBLE) iTensExp));
    // debugging
    //printf ("Orig %d  New %d  Tens %d\n", iValueOrig, iValue, iTensExp);
    Format ("%f", fValue);

    INT  iNewDecimal = FindChar ('.');
    if (iNewDecimal == -1)
      {
      // no decimal.  set decimal position to end of string.
      iNewDecimal = Length ();
      };

    // now try to put the cursor under the appropriate digit
    if (iTensExp >= 0)
      {
      iCursorStart = iNewDecimal - iTensExp - 1;
      }
    else
      {
      iCursorStart = iNewDecimal - iTensExp;
      };
    };
  };


//------------------------------------------------------------------------------
const char *  RStrParser::GetBracketString  (RStr *        pstrOut,
                                             const char *  szOpenBracketChars,
                                             const char *  szCloseBracketChars,
                                             BOOL          bSkipEOL,
                                             BOOL          bUnescapeChars,
                                             BOOL          bCalcHashIn)
  {

  // Note:  This is similar to GetQuoteString, except that you can define
  //         what the opening and closing bracket or quote characters are

  // Note:  May still need to deal with slashes on the end of lines to
  //         split strings across multiple lines.
  //  to be implemented

  UINT32   uCharCurr;
  BOOL     bHaveBracket = FALSE;
  INT32    iSearch = -1;

  if (szOpenBracketChars == NULL)
    {
    szOpenBracketChars = "[";
    };
  if (szCloseBracketChars == NULL)
    {
    szCloseBracketChars = "]";
    };

  if (pstrOut != NULL)
    {
    *pstrOut = "";
    };

  if ((szOpenBracketChars != NULL) && (szCloseBracketChars != NULL))
    {
    ASSERT (strlen (szOpenBracketChars) == strlen (szCloseBracketChars));

    RStr  strOpenBracket (szOpenBracketChars);
    iSearch = strOpenBracket.FindChar (PeekChar ());
    if (iSearch != -1)
      {
      bHaveBracket = TRUE;
      }
    }

  if (bHaveBracket)
    {
    // Get everything between the brackets, and unescape escaped characters.
    SkipChars (1);

    while (! IsEOFAscii ())
      {
      uCharCurr = PeekChar ();
      SkipChars (1);
      if (uCharCurr == (unsigned char) szCloseBracketChars [(UINT32)iSearch])
        {
        break;
        };


      if (uCharCurr == kESCAPE)
        {
        uCharCurr = PeekChar ();
        SkipChars (1);
        if (bUnescapeChars)
          {
          switch (uCharCurr)
            {
            case 'a':  uCharCurr = '\a'; break;
            case 'b':  uCharCurr = '\b'; break;
            case 'f':  uCharCurr = '\f'; break;
            case 'n':  uCharCurr = '\n'; break;
            case 'r':  uCharCurr = '\r'; break;
            case 't':  uCharCurr = '\t'; break;
            case 'v':  uCharCurr = '\v'; break;
            case '0':  uCharCurr = '\0'; break;
            default:   break;
            };
          }
        else
          {
          if (pstrOut != NULL)
            {
            pstrOut->AppendChar (kESCAPE);
            };
          }
        };

      if (pstrOut != NULL)
        {
        pstrOut->AppendChar (uCharCurr);
        };
      };
    }
  else
    {
    // get everything until the end of the line, escaping characters as you go.

    while (! IsEOL ())
      {
      if ((! bGreedyRead) && (IsWhitespace (PeekChar ())))
        {
        break;
        };

      uCharCurr = PeekChar ();
      SkipChars (1);

      if (uCharCurr == kESCAPE)
        {
        uCharCurr = PeekChar ();
        SkipChars (1);
        if (bUnescapeChars)
          {
          switch (uCharCurr)
            {
            case 'a':  uCharCurr = '\a'; break;
            case 'b':  uCharCurr = '\b'; break;
            case 'f':  uCharCurr = '\f'; break;
            case 'n':  uCharCurr = '\n'; break;
            case 'r':  uCharCurr = '\r'; break;
            case 't':  uCharCurr = '\t'; break;
            case 'v':  uCharCurr = '\v'; break;
            case '0':  uCharCurr = '\0'; break;
            default:   break;
            };
          }
        else
          {
          if (pstrOut != NULL)
            {
            pstrOut->AppendChar (kESCAPE);
            };
          };
        };

      if (pstrOut != NULL)
        {
        pstrOut->AppendChar (uCharCurr);
        };
      };
    };

  if (bCalcHashIn && (pstrOut != NULL))
    {
    pstrOut->CalcHash ();
    };

  SkipWhitespace (bSkipEOL);
  return (pstrOut == NULL ? NULL : pstrOut->AsChar ());
  };


//------------------------------------------------------------------------------
VOID  RStrParser::MakeQuoteString (VOID)
  {
  MakeEscapedString (TRUE);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::MakeEscapedString (BOOL   bAddQuotes)
  {
  RStr    strQuoted ;

  // guesstimate that 1 out of every 10 characters needs to be escaped, and preallocate for that
  strQuoted.GrowAbsolute (this->Length () + (this->Length () / 10) );

  if (bAddQuotes) {strQuoted.AppendChar (kDOUBLEQUOTES);};

  for (UINT32  uIndex = 0; uIndex < uStringLength; ++uIndex)
    {
    UINT32  uChar = GetAt (uIndex);

    // skip stuff that is already escaped
    if (uChar == kESCAPE)
      {
      strQuoted.AppendChar (uChar);
      ++uIndex;
      uChar = GetAt (uIndex);
      strQuoted.AppendChar (uChar);
      continue;
      };

    switch (uChar)
      {
      case kDOUBLEQUOTES:  {strQuoted.AppendChar (kESCAPE);}; break;
      case kESCAPE:        {strQuoted.AppendChar (kESCAPE);}; break;
      case '\a':           {strQuoted.AppendChar (kESCAPE); uChar = 'a';}; break;
      case '\b':           {strQuoted.AppendChar (kESCAPE); uChar = 'b';}; break;
      case '\f':           {strQuoted.AppendChar (kESCAPE); uChar = 'f';}; break;
      case '\n':           {strQuoted.AppendChar (kESCAPE); uChar = 'n';}; break;
      case '\r':           {strQuoted.AppendChar (kESCAPE); uChar = 'r';}; break;
      case '\t':           {strQuoted.AppendChar (kESCAPE); uChar = 't';}; break;
      case '\v':           {strQuoted.AppendChar (kESCAPE); uChar = 'v';}; break;
      case '\0':           {strQuoted.AppendChar (kESCAPE); uChar = '0';}; break;
      default: break;
      };
    strQuoted.AppendChar (uChar);
    };

  if (bAddQuotes) {strQuoted.AppendChar (kDOUBLEQUOTES);};
  *this = strQuoted;
  };



//------------------------------------------------------------------------------
UINT32  RStrParser::PeekChar  (INT32  iPosition)
  {

  if (iPosition == -1)  iPosition = iCursorStart;

  return (GetAt (iPosition));
  };


//------------------------------------------------------------------------------
UINT32  RStrParser::ReadChars  (PCHAR    pszBufferOut,
                                INT32    iCharsToCopy)
  {
  INT32  lBytesLeft = Length () - iCursorStart;

  iCharsToCopy = RMin (iCharsToCopy, lBytesLeft);


  if (iCharsToCopy > 0)
    {
    // if a null buffer is passed, just consume the chars you would have read.
    if (pszBufferOut != NULL)
      {
      memcpy (pszBufferOut, GetString () + iCursorStart, iCharsToCopy);
      };

    iCursorStart += iCharsToCopy;
    };

  return (RMax (iCharsToCopy, 0));
  };

//------------------------------------------------------------------------------
UINT32  RStrParser::ReadChars  (RStr &   strOut,
                                INT32    iCharsToCopy)
  {
  strOut.Grow (iCharsToCopy);
  UINT32  uNumCharsRead = ReadChars (strOut.GetBufferPtr (), iCharsToCopy);
  strOut.SetAt (iCharsToCopy, '\0');

  // Note:  We can't set the ending null directly, or we throw off the string length.
  //  so we set the next-to-last character.
  if (uNumCharsRead != 0)
    {
    strOut.SetAt ((INT) uNumCharsRead - 1, strOut.GetAt ((INT) uNumCharsRead - 1));
    };
  return (uNumCharsRead);
  };

//------------------------------------------------------------------------------
VOID  RStrParser::WriteChars  (PCHAR    pszBufferIn,
                               INT32    iCharsToCopy)
  {
  AppendChars (pszBufferIn, UINT32 (iCharsToCopy));
  iCursorStart = uStringLength;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::StripComments  (VOID)
  {
  // to be implemented
  };



//------------------------------------------------------------------------------
EStatus  RStrParser::ReadFromFile  (const char *  szFilenameIn,
                                    INT           iStartIndex,
                                    INT           iMaxBytesToRead)
  {
  UINT    uFileSize    = FilePath::GetFileSize (szFilenameIn);
  INT     iBytesToRead = uFileSize;

  if (iMaxBytesToRead > 0)
    {
    iBytesToRead = RMin ((UINT32) iMaxBytesToRead, uFileSize);
    };

  GrowAbsolute (iBytesToRead + 1);

  EStatus  rStatus = FilePath::ReadFromFile (szFilenameIn,
                                             iStartIndex,
                                             iBytesToRead,
                                             (unsigned char *) pszBuffer);

  uStringLength = iBytesToRead;
  iCursorStart  = 0;
  iCursorEnd    = 0;
  iLineEnd      = iBytesToRead;
  pszBuffer [iBytesToRead] = '\0';

  return (rStatus);
  };


//------------------------------------------------------------------------------
EStatus  RStrParser::WriteToFile  (const char *  szFilenameIn,
                                   INT           iBytesToWriteIn)
  {
  // Note:  You eventually need to create a progressive write function so that
  //         you don't have to keep the entire write buffer in memory before
  //         it is sent to disk.  This will matter when you start dealing with
  //         large datasets.

  INT                iBytesToWrite = iBytesToWriteIn;

  if (iBytesToWrite == -1)
    {
    iBytesToWrite = uStringLength;
    };
  return (FilePath::WriteToFile (szFilenameIn,
                                 FALSE,
                                 iBytesToWrite,
                                 (unsigned char *) pszBuffer));
  };


//------------------------------------------------------------------------------
EStatus  RStrParser::SkipComment  (VOID)
  {
  if (eSkipComments == kNone) return (EStatus::kSuccess);
  while (! IsEOF ())
    {
    if (IsEOLComment (iCursorStart))
      {
      // skip EOL comment

      // reset the line end so it won't trigger the EOL.
      iLineEnd = -1;

      // skip until an actual EOL
      while (! IsEOL ())
        {
        ++iCursorStart;
        };
      }
    else if (IsBlockComment (iCursorStart))
      {
      // skip block comment
      iCursorStart += GetBlockCommentTagLength ();
      while ((! IsEOF ()) && (! IsBlockCommentEnd  (iCursorStart)))
        {
        ++iCursorStart;
        };
      if (IsBlockCommentEnd  (iCursorStart))
        {
        iCursorStart += GetBlockCommentTagLength ();
        }
      else
        {
        // unable to find end of block comment
        return (EStatus::Failure ("RStrParser::SkipComment (): Unable to find end of block comment"));
        };
      }
    else
      {
      // didn't find either comment type.  Done skipping comments.
      break;
      };
    };
  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsComment  (INT  iIndexIn)
  {
  if (IsEOLComment (iIndexIn) ||
      IsBlockComment (iIndexIn))
    {
    return (TRUE);
    };
  return (FALSE);
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsBlockComment  (INT  iIndexIn)
  {
  if (eSkipComments == kCStyle)
    {
    // check for C-style Block comments

    UINT32  uCurr = GetAt (iIndexIn);
    if (uCurr == '/')
      {
      uCurr = GetAt (iIndexIn + 1);
      if (uCurr == '*')
        {
        // found a block comment
        return (TRUE);
        };
      }
    }
  else if (eSkipComments == kShellStyle)
    {
    // check for python-style block comments, since the shell doesn't have
    //  block commenting.

    UINT32  uCurr = GetAt (iIndexIn);
    if (uCurr == '"')
      {
      uCurr = GetAt (iIndexIn + 1);
      if (uCurr == '"')
        {
        uCurr = GetAt (iIndexIn + 2);
        if (uCurr == '"')
          {
          // found a block comment
          return (TRUE);
          };
        };
      };
    };
  return (FALSE);
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsBlockCommentEnd  (INT  iIndexIn)
  {
  if (eSkipComments == kCStyle)
    {
    // check for C-style Block comments

    UINT32  uCurr = GetAt (iIndexIn);
    if (uCurr == '*')
      {
      uCurr = GetAt (iIndexIn + 1);
      if (uCurr == '/')
        {
        // found a block comment
        return (TRUE);
        };
      }
    }
  else if (eSkipComments == kShellStyle)
    {
    // check for python-style block comments, since the shell doesn't have
    //  block commenting.

    UINT32  uCurr = GetAt (iIndexIn);
    if (uCurr == '"')
      {
      uCurr = GetAt (iIndexIn + 1);
      if (uCurr == '"')
        {
        uCurr = GetAt (iIndexIn + 2);
        if (uCurr == '"')
          {
          // found a block comment
          return (TRUE);
          };
        };
      };
    };
  return (FALSE);
  };


//------------------------------------------------------------------------------
INT  RStrParser::GetBlockCommentTagLength (VOID)
  {
  if (eSkipComments == kCStyle)
    {
    return (2);
    }
  else if (eSkipComments == kShellStyle)
    {
    return (3);
    };
  return (0);
  };


//------------------------------------------------------------------------------
BOOL  RStrParser::IsEOLComment  (INT  iIndexIn)
  {
  if (eSkipComments == kCStyle)
    {
    // check for C-style EOL comments

    UINT32  uCurr = GetAt (iIndexIn);
    if (uCurr == '/')
      {
      uCurr = GetAt (iIndexIn + 1);
      if (uCurr == '/')
        {
        // found an EOL comment
        return (TRUE);
        };
      }
    }
  else if (eSkipComments == kShellStyle)
    {
    // check for shell-style EOL comments

    UINT32  uCurr = GetAt (iIndexIn);
    if (uCurr == '#')
      {
      // found an EOL comment
      return (TRUE);
      };
    };
  return (FALSE);
  };


//------------------------------------------------------------------------------
UINT32  RStrParser::GetTrailingUInt (RStr *  pstrDigits)
  {
  FindLineEnd ();
  INT32   iIndex      = iLineEnd - 1;
  INT32   iMultiplier = 1;
  INT32   iReturn = 0;

  if (pstrDigits != NULL)
    {
    pstrDigits->Empty ();
    };

  while (isdigit (GetAt (iIndex)))
    {
    if (pstrDigits != NULL)
      {
      pstrDigits->PrependChar (GetAt (iIndex));
      };

    iReturn += iMultiplier * CharToInt (iIndex);

    --iIndex;
    iMultiplier *= 10;
    };
  return (iReturn);
  };


//------------------------------------------------------------------------------
UINT32 RStrParser::Tokenize  (UINT32       uSeparatorIn,
                              RStrArray &  arrayOut,
                              BOOL         bStripWhitespace,
                              BOOL         bSkipEOL,
                              BOOL         bKeepEmptyStrings)
  {
  UINT32     uNumStringsFound = 0;


  // if we are tokenizing on whitespace, be sure not to skip it.
  if (uSeparatorIn == ' ') bStripWhitespace = false;

  while (! IsEOFAscii ())
    {
    // skip leading whitespace
    if (bStripWhitespace) SkipWhitespace (bSkipEOL);

    // search for the separator
    INT32   iCopyStart = iCursorStart;
    UINT32  uCurr;
    while (uSeparatorIn != (uCurr = GetAt (iCursorStart)))
      {
      //DBG_INFO ("Tokenize %d sep %x char %x '%c'", iCursorStart, uSeparatorIn, uCurr, uCurr);
      if (uCurr == kDOUBLEQUOTES)
        {
        // skip the quoted string
        ++iCursorStart;
        while ( (! IsEOL ()) && (GetAt (iCursorStart) != kDOUBLEQUOTES) )
          {
          // escaped sequences skip two characters
          if (GetAt (iCursorStart) == kESCAPE)
            {
            ++iCursorStart;
            };
          ++iCursorStart;
          };
        };

      ++iCursorStart;
      if ((uSeparatorIn != '\n') && (IsEOL ())) break;
      };

    INT32  iStringLength = iCursorStart - iCopyStart;

    if ((iStringLength > 0) || (bKeepEmptyStrings))
      {
      arrayOut.Append (RStr (""));
      RStr &  strOut = arrayOut [arrayOut.Length () - 1];

      if (iStringLength > 0)
        {
        GetMiddle (iCopyStart, iStringLength, strOut);
        };
      if (bStripWhitespace)
        {
        strOut.StripTrailingWhitespace ();
        };
      ++uNumStringsFound;
      };


    // skip the separator
    ++iCursorStart;

    // skip trailing whitespace
    if (bStripWhitespace)
      {
      SkipWhitespace (bSkipEOL);
      };

    // break if we are at the end of a line, unless we are supposed to skip EOLs
    if ((uSeparatorIn != '\n') && (IsEOL ()))
      {
      break;
      };
    };

  return (uNumStringsFound);
  };


//------------------------------------------------------------------------------
VOID RStrParser::Join (UINT32       uSeparatorIn,
                       RStrArray &  arrayIn)
  {
  Empty ();

  INT  iNumElements = arrayIn.Length();
  if (iNumElements == 1)
    {
    Set (arrayIn[0]);
    }
  else if (iNumElements > 1)
    {
    AppendString (arrayIn[0]);
    for (INT  iIndex = 1; iIndex < iNumElements; ++iIndex)
      {
      AppendChar (uSeparatorIn);
      AppendString (arrayIn[iIndex]);
      }
    }
  }

/*
// NOTE:  This dups functionality in RStr.  Not sure why.
//------------------------------------------------------------------------------
VOID  RStrParser::StripTrailingWhitespace (BOOL  bStripEOL)
  {
  while ((uStringLength > 0) &&
         ((IsWhitespace (pszBuffer [uStringLength - 1])) ||
          (bStripEOL && ((UINT32 (pszBuffer [uStringLength - 1]) == kLF) || (UINT32 (pszBuffer [uStringLength - 1]) == kCR)) )
         )
        )
    {
    --uStringLength;
    };
  };
*/

//------------------------------------------------------------------------------
RStrParser &  RStrParser::operator=  (const RStrParser &  parserIn)
  {
  Set (parserIn, parserIn.uHash != 0);
  eSkipComments = parserIn.eSkipComments;
  bGreedyRead   = parserIn.bGreedyRead;
  ResetCursor ();
  return (*this);
  };


//------------------------------------------------------------------------------
RStrParser &  RStrParser::operator=  (const RStr & strIn)
  {
  Set (strIn, strIn.uHash != 0);
  ResetCursor ();
  return (*this);
  };


//------------------------------------------------------------------------------
RStrParser &  RStrParser::operator=  (const char *  pszIn)
  {
  Set (pszIn);
  ResetCursor ();
  return (*this);
  };


//------------------------------------------------------------------------------
INT32  RStrParser::CountWords  (bool  bStopAtEOL)
  {
  INT32  iCount = 0;
  INT32  iSavedCursor = iCursorStart;

  SkipWhitespace (FALSE);
  while (! IsEOFAscii ())
    {
    if ((bStopAtEOL) && (IsEOL ()))
      {
      break;
      };
    ++iCount;
    SkipWhitespace (TRUE);
    GetWord (NULL, FALSE);
    };

  iCursorStart = iSavedCursor;
  return (iCount);
  };




//------------------------------------------------------------------------------
DOUBLE  RStrParser::GetF8_LEnd  (VOID)
  {
  UINT64   uReturn = GetU8_LEnd ();
  // cppcheck-suppress invalidPointerCast
  return (*reinterpret_cast<DOUBLE*>(&uReturn));
  };

//------------------------------------------------------------------------------
FLOAT  RStrParser::GetF4_LEnd  (VOID)
  {
  UINT32   uReturn = GetU4_LEnd ();
  // cppcheck-suppress invalidPointerCast
  return (*reinterpret_cast<FLOAT*>(&uReturn));
  };

//------------------------------------------------------------------------------
UINT64  RStrParser::GetU8_LEnd  (VOID)
  {
  UINT64 uReturn = (((UINT64) GetAt (iCursorStart + 0) << 0)  |
                    ((UINT64) GetAt (iCursorStart + 1) << 8)  |
                    ((UINT64) GetAt (iCursorStart + 2) << 16) |
                    ((UINT64) GetAt (iCursorStart + 3) << 24) |
                    ((UINT64) GetAt (iCursorStart + 4) << 32) |
                    ((UINT64) GetAt (iCursorStart + 5) << 40) |
                    ((UINT64) GetAt (iCursorStart + 6) << 48) |
                    ((UINT64) GetAt (iCursorStart + 7) << 56));
  SkipChars (8);
  return (uReturn);
  };

//------------------------------------------------------------------------------
UINT32  RStrParser::GetU4_LEnd  (VOID)
  {
  UINT32 uReturn = ((GetAt (iCursorStart + 0) << 0)  |
                    (GetAt (iCursorStart + 1) << 8)  |
                    (GetAt (iCursorStart + 2) << 16) |
                    (GetAt (iCursorStart + 3) << 24));
  //printf ("GetU4_LEnd %x  reading %d of %d (max %d)\n", this, iCursorStart, uStringLength, uBufferSize);

  SkipChars(4);
  return (uReturn);
  };


//------------------------------------------------------------------------------
UINT32  RStrParser::GetU2_LEnd  (VOID)
  {
  UINT32 uReturn = ((GetAt (iCursorStart + 0) << 0)  |
                    (GetAt (iCursorStart + 1) << 8));
  //printf ("GetU2_LEnd %x  reading %d of %d (max %d)\n", this, iCursorStart, uStringLength, uBufferSize);
  SkipChars (2);
  return (uReturn);
  };


//------------------------------------------------------------------------------
UINT32  RStrParser::GetU1_LEnd  (VOID)
  {
  UINT32 uReturn = GetAt (iCursorStart + 0);
  SkipChars (1);
  return (uReturn);
  };


//------------------------------------------------------------------------------
DOUBLE  RStrParser::GetF8_BEnd  (VOID)
  {
  UINT64   uReturn = GetU8_BEnd ();

  // cppcheck-suppress invalidPointerCast
  return (*reinterpret_cast<DOUBLE*>(&uReturn));
  };

//------------------------------------------------------------------------------
FLOAT  RStrParser::GetF4_BEnd  (VOID)
  {
  UINT32   uReturn = GetU4_BEnd ();

  // cppcheck-suppress invalidPointerCast
  return (*reinterpret_cast<FLOAT*>(&uReturn));
  };

//------------------------------------------------------------------------------
UINT64  RStrParser::GetU8_BEnd  (VOID)
  {
  UINT64 uReturn = (((UINT64) GetAt (iCursorStart + 0) << 56) |
                    ((UINT64) GetAt (iCursorStart + 1) << 48) |
                    ((UINT64) GetAt (iCursorStart + 2) << 40) |
                    ((UINT64) GetAt (iCursorStart + 3) << 32) |
                    ((UINT64) GetAt (iCursorStart + 4) << 24) |
                    ((UINT64) GetAt (iCursorStart + 5) << 16) |
                    ((UINT64) GetAt (iCursorStart + 6) <<  8) |
                    ((UINT64) GetAt (iCursorStart + 7) <<  0));
  SkipChars (8);
  return (uReturn);
  };

//------------------------------------------------------------------------------
UINT32  RStrParser::GetU4_BEnd  (VOID)
  {
  UINT32 uReturn = ((GetAt (iCursorStart + 0) << 24) |
                    (GetAt (iCursorStart + 1) << 16) |
                    (GetAt (iCursorStart + 2) <<  8) |
                    (GetAt (iCursorStart + 3) <<  0));
  SkipChars (4);
  return (uReturn);
  };


//------------------------------------------------------------------------------
UINT32  RStrParser::GetU2_BEnd  (VOID)
  {
  UINT32 uReturn = ((GetAt (iCursorStart + 0) << 8)  |
                    (GetAt (iCursorStart + 1) << 0));
  SkipChars (2);
  return (uReturn);
  };


//------------------------------------------------------------------------------
UINT32  RStrParser::GetU1_BEnd  (VOID)
  {
  UINT32 uReturn = GetAt (iCursorStart + 0);
  SkipChars (1);
  return (uReturn);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::SetF8_LEnd  (DOUBLE  dIn)
  {
  // cppcheck-suppress invalidPointerCast
  SetU8_LEnd (*reinterpret_cast<UINT64*>(&dIn));
  };

//------------------------------------------------------------------------------
VOID  RStrParser::SetF4_LEnd  (FLOAT  fIn)
  {
  // cppcheck-suppress invalidPointerCast
  SetU4_LEnd (*reinterpret_cast<UINT32*>(&fIn));
  };

/*
// NOTE:  For LEnd values, reinterpreting as a character * is likely faster.
//         I'm using the shift-and-mask approach for simplicity/consistency/clarity
//------------------------------------------------------------------------------
VOID  RStrParser::SetF8_LEnd  (DOUBLE  dIn)
  {
  unsigned char *  pVal = reinterpret_cast<unsigned char *>(&dIn);


  SetAt (iCursorStart + 0, *pVal); ++pVal;
  SetAt (iCursorStart + 1, *pVal); ++pVal;
  SetAt (iCursorStart + 2, *pVal); ++pVal;
  SetAt (iCursorStart + 3, *pVal); ++pVal;

  SetAt (iCursorStart + 4, *pVal); ++pVal;
  SetAt (iCursorStart + 5, *pVal); ++pVal;
  SetAt (iCursorStart + 6, *pVal); ++pVal;
  SetAt (iCursorStart + 7, *pVal); ++pVal;

  iCursorStart += 8;
  };
*/

//------------------------------------------------------------------------------
VOID  RStrParser::SetU8_LEnd  (UINT64  uIn)
  {
  SetAt (iCursorStart + 0, (uIn >>  0) & 0x000000ff);
  SetAt (iCursorStart + 1, (uIn >>  8) & 0x000000ff);
  SetAt (iCursorStart + 2, (uIn >> 16) & 0x000000ff);
  SetAt (iCursorStart + 3, (uIn >> 24) & 0x000000ff);
  SetAt (iCursorStart + 4, (uIn >> 32) & 0x000000ff);
  SetAt (iCursorStart + 5, (uIn >> 40) & 0x000000ff);
  SetAt (iCursorStart + 6, (uIn >> 48) & 0x000000ff);
  SetAt (iCursorStart + 7, (uIn >> 56) & 0x000000ff);
  iCursorStart += 8;
  };

//------------------------------------------------------------------------------
VOID  RStrParser::SetU4_LEnd  (UINT32  uIn)
  {
  SetAt (iCursorStart + 0, (uIn >>  0) & 0x000000ff);
  SetAt (iCursorStart + 1, (uIn >>  8) & 0x000000ff);
  SetAt (iCursorStart + 2, (uIn >> 16) & 0x000000ff);
  SetAt (iCursorStart + 3, (uIn >> 24) & 0x000000ff);
  iCursorStart += 4;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::SetU2_LEnd  (UINT32  uIn)
  {
  SetAt (iCursorStart + 0, (uIn >>  0) & 0x000000ff);
  SetAt (iCursorStart + 1, (uIn >>  8) & 0x000000ff);
  iCursorStart += 2;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::SetU1_LEnd  (UINT32  uIn)
  {
  SetAt (iCursorStart + 0, (uIn >>  0) & 0x000000ff);
  iCursorStart += 1;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::SetF8_BEnd  (DOUBLE  dIn)
  {
  // cppcheck-suppress invalidPointerCast
  SetU8_BEnd (*reinterpret_cast<UINT64*>(&dIn));
  };

//------------------------------------------------------------------------------
VOID  RStrParser::SetF4_BEnd  (FLOAT  fIn)
  {
  // cppcheck-suppress invalidPointerCast
  SetU4_BEnd (*reinterpret_cast<UINT32*>(&fIn));
  };

//------------------------------------------------------------------------------
VOID  RStrParser::SetU8_BEnd  (UINT64  uIn)
  {
  SetAt (iCursorStart + 0, (uIn >> 56) & 0x000000ff);
  SetAt (iCursorStart + 1, (uIn >> 48) & 0x000000ff);
  SetAt (iCursorStart + 2, (uIn >> 40) & 0x000000ff);
  SetAt (iCursorStart + 3, (uIn >> 32) & 0x000000ff);
  SetAt (iCursorStart + 4, (uIn >> 24) & 0x000000ff);
  SetAt (iCursorStart + 5, (uIn >> 16) & 0x000000ff);
  SetAt (iCursorStart + 6, (uIn >>  8) & 0x000000ff);
  SetAt (iCursorStart + 7, (uIn >>  0) & 0x000000ff);
  iCursorStart += 8;
  };

//------------------------------------------------------------------------------
VOID  RStrParser::SetU4_BEnd  (UINT32  uIn)
  {
  SetAt (iCursorStart + 0, (uIn >> 24) & 0x000000ff);
  SetAt (iCursorStart + 1, (uIn >> 16) & 0x000000ff);
  SetAt (iCursorStart + 2, (uIn >>  8) & 0x000000ff);
  SetAt (iCursorStart + 3, (uIn >>  0) & 0x000000ff);
  iCursorStart += 4;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::SetU2_BEnd  (UINT32  uIn)
  {
  SetAt (iCursorStart + 0, (uIn >>  8) & 0x000000ff);
  SetAt (iCursorStart + 1, (uIn >>  0) & 0x000000ff);
  iCursorStart += 2;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::SetU1_BEnd  (UINT32  uIn)
  {
  SetAt (iCursorStart + 0, (uIn >>  0) & 0x000000ff);
  iCursorStart += 1;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::GetData  (unsigned char *  pbyDataOut,
                            INT32            iDataSizeIn)
  {
  ReadChars (PCHAR (pbyDataOut), iDataSizeIn);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::GetData  (RStr *           pstrDataOut,
                            INT32            iDataSizeIn)
  {
  const char *  pszReadLocation = GetString (iCursorStart);

  // TODO: Do we need to do repeated SetAt() calls rather than AppendChars,
  //        to allow reading data into the middle of a target buffer?
  pstrDataOut->AppendChars (pszReadLocation, ReadChars (NULL, iDataSizeIn));
  };


//------------------------------------------------------------------------------
VOID  RStrParser::SetData  (const unsigned char *  pbyDataIn,
                            INT32                  iDataSizeIn)
  {
  WriteChars (PCHAR (pbyDataIn), iDataSizeIn);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::GetDataStr  (RStr *           pstrDataOut)
  {
  INT  iLength = GetU4_BEnd ();
  GetData  (pstrDataOut, iLength + 1);
  };

//------------------------------------------------------------------------------
VOID  RStrParser::SetDataStr  (const RStr &     strDataIn)
  {
  INT  iLength = strDataIn.Length ();
  SetU4_BEnd (iLength);
  SetData ((const unsigned char *) strDataIn.AsChar (), iLength + 1);
  };


//------------------------------------------------------------------------------
EStatus  RStrParser::FlushBuffer  (VOID)
  {
  if (bBufferedWriting)
    {
    FILE *             fp;

    // Make sure input values are valid
    if (strBufferedFilename == RStr("")) return EStatus::kFailure;

    fp = fopen (strBufferedFilename.AsChar (), "ab");
    if (fp == NULL)
      {
      //sprintf (szDbgBuffer, "Unable to open file %s", szFilename); DebugMessage (szDbgBuffer);
      return EStatus::kFailure;
      };

    if (fwrite (pszBuffer, 1, uStringLength, fp) != uStringLength)
      {
      fclose (fp);
      return EStatus::kFailure;
      };

    fclose (fp);

    // clear the buffer, but leave memory allocated
    Empty        ();
    ResetCursor  ();

    return EStatus::kSuccess;
    };

  return EStatus::kFailure;
  };


//------------------------------------------------------------------------------
VOID  RStrParser::Grow  (UINT32  uSizeIn)
  {
  if (bBufferedWriting)
    {
    if (uStringLength > UINT32 (iBufferedWriteSize))
      {
      FlushBuffer ();
      };
    };

  // Reset the line end since the buffer is changing
  iLineEnd  = -1;

  RStr::Grow (uSizeIn);
  };


//------------------------------------------------------------------------------
VOID  RStrParser::GrowAbsolute  (UINT32  uSizeIn)
  {
  if (bBufferedWriting)
    {
    if (uStringLength > UINT32 (iBufferedWriteSize))
      {
      FlushBuffer ();
      };
    };
  // Reset the line end since the buffer is changing
  iLineEnd  = -1;

  RStr::GrowAbsolute (uSizeIn);
  };

//------------------------------------------------------------------------------
BOOL  RStrParser::GetNextEnvVar (RStr &  strVarNameOut,
                                 RStr *  pstrFullTagOut)
  {
  // scan for environment-style variables, and return when you find one.
  // An environment variable name starts with a dollar sign ($), followed by
  //  a string of alpha_numeric and/or underscore characters.  This string may
  //  optionally be enclosed in either curly brackets {} or parenthesis ()

  // Returns TRUE if a variable name was found, false otherwise.

  INT32  iVarNameStart = FindChar ('$', iCursorStart);  // start and size of variable name only

  if (iVarNameStart != -1)
    {
    // we found a variable
    // iTotalStart is the start and size of the variable, including '$' and any brackets
    INT32  iTotalStart = iVarNameStart;
    INT32  iTotalCount = 1;
    INT32  iSkipEnd    = 0;

    // skip the $
    iCursorStart = iVarNameStart;
    SkipChars (1);
    // skip the opening bracket
    if (GetAt (iCursorStart) == '{')
      {
      iSkipEnd = '}';
      SkipChars (1);
      ++iTotalCount;
      }
    else if (GetAt (iCursorStart) == '(')
      {
      iSkipEnd = ')';
      SkipChars (1);
      ++iTotalCount;
      };
    iVarNameStart = iCursorStart;

    // skip all chars in the variable name
    INT32  iCurr = GetAt (iCursorStart);

    INT32  iVarNameCount   = 0;
    while ( (isalnum (iCurr)) ||
            (iCurr == '_')    ||
            (iCurr == '.'))
      {
      SkipChars (1);
      iCurr = GetAt (iCursorStart);
      ++iVarNameCount;
      ++iTotalCount;
      };

    // skip the ending bracket
    if ((iSkipEnd != 0) && (iCurr == iSkipEnd))
      {
      SkipChars (1);
      ++iTotalCount;
      };

    // extract the variable name from the string.
    GetMiddle (iVarNameStart, iVarNameCount, strVarNameOut);

    if (pstrFullTagOut != NULL)
      {
      GetMiddle (iTotalStart, iTotalCount, *pstrFullTagOut);
      };

    ClipMiddle (UINT32 (iTotalStart), UINT32 (iTotalCount));

    // set the cursor to the location where a substitution should occur
    SetCursorStart (iTotalStart);
    return (TRUE);
    };
  return (FALSE);
  };



//------------------------------------------------------------------------------
VOID  RStrParser::DebugPrint  (const char *  pszNameIn)
  {
  DBG_INFO ("RStrParser - Debug [%s] : this (%x) pszBuffer (%x) uBufferSize (%d) uGrowIncrement (%d) uStringLength (%d) uHash (%x) iCursorStart (%d) iCursorEnd (%d) iLineEnd (%d)",
            (pszNameIn == NULL) ? "" : pszNameIn,
            (long) this,
            (long) pszBuffer,
            uBufferSize,
            uGrowIncrement,
            uStringLength,
            uHash,
            iCursorStart,
            iCursorEnd,
            iLineEnd);


  /*
    enum  ECommentType    {kNone    = 0,     ///< Skipping comments is off
                           kCStyle  = 1,     ///< C-style comments  //
                           kShellStyle  = 2  ///< Shell-style comments  #
                          };
                   /// If buffered writing is set to true, then every time iBufferedWriteSize bytes are placed in the parser, the contents are cleared and written to disk.  The memory buffer is then reset to the start.
    BOOL           bBufferedWriting;
                   /// If buffered writing is set to true, this is the file the buffer is written to.
    RStr           strBufferedFilename;
                   /// The size the buffer will grow to before it is flushed out to disk.
    INT32          iBufferedWriteSize;
                   /// True if EOL comments affect the calculation of iLineEnd, false if they don't.
    ECommentType   eSkipComments;
                   /// True if GetQuoteString ignores spaces and reads to end of line, False if whitespace terminates the read.
    BOOL           bGreedyRead;

  */
  };






