/* -----------------------------------------------------------------
                             Attr Float Array

     This module implements arrays of floating point attributes

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

#include <stdlib.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Composite/AttrFloatArray.hpp"

//-----------------------------------------------------------------------------
//  AttrFloatArray
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
AttrFloatArray::AttrFloatArray (const char *  szNameIn) : Attr (szNameIn)
  {
  arrayValue.Clear ();
  szType = Identifier ();
  ccType = CCIdentifier ();
  };

//-----------------------------------------------------------------------------
AttrFloatArray::~AttrFloatArray ()
  {
  };

//-----------------------------------------------------------------------------
AttrFloatArray &  AttrFloatArray::operator= (const AttrFloatArray &  attrIn)
  {
  Attr::CopyCommonVars (attrIn);
  arrayValue = attrIn.arrayValue;
  OnChanged ();
  return (*this);
  };

//-----------------------------------------------------------------------------
const char *  AttrFloatArray::GetAsString  (RStr *  pstrValueOut) const
  {
  if (pstrValueOut == NULL) return ("");
  RStrParser  parserCurr;

  INT  iNumStrings = arrayValue.Length ();
  for (INT  iIndex = 0; iIndex < iNumStrings; ++iIndex)
    {
    if (iIndex < iNumStrings - 1)
      {
      pstrValueOut->AppendFormat ("%f, ", arrayValue[iIndex]);
      }
    else
      {
      pstrValueOut->AppendFormat ("%f", arrayValue[iIndex]);
      };
    };
  return (pstrValueOut->AsChar ());
  };

//-----------------------------------------------------------------------------
VOID  AttrFloatArray::SetByString  (const char *  szValueIn)
  {
  // parse out floats separated by commas and optional whitespace
  // NOTE:  When setting by string, values are appended.  Original values are not replaced.

  RStrParser  parser (szValueIn);
  RStr        strCurr;

  parser.SkipWhitespace ();

  INT iIndex = arrayValue.Length ();
  while (!parser.IsEOL ())
    {
    Attr::SetArrayByString (iIndex, ""); // This will mark attr as dirty
    arrayValue.SetMinLength (iIndex + 1);
    arrayValue.SetAt (iIndex, parser.GetFloat());
    ++iIndex;

    parser.SkipWhitespace ();
    if (parser.PeekChar() != ',') break;
    parser.SkipChars(1);
    parser.SkipWhitespace ();
    }
  };

//-----------------------------------------------------------------------------
VOID  AttrFloatArray::SetArrayByString  (INT           iIndexIn,
                                         const char *  szValueIn)
  {
    DBG_INFO ("FloatArray Set by string : %s", szValueIn);
  Attr::SetArrayByString (iIndexIn, szValueIn);
  arrayValue.SetMinLength (iIndexIn + 1);
  arrayValue.SetAt (iIndexIn, (FLOAT)atof(szValueIn));
  OnChanged ();
  };

//-----------------------------------------------------------------------------
VOID  AttrFloatArray::Clear  (VOID)
  {
  Attr::Clear ();
  arrayValue.Clear ();
  OnChanged ();
  };





