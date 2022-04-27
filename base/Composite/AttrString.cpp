/* -----------------------------------------------------------------
                             Attr String

     This module implements string attributes

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

#include "Composite/AttrString.hpp"

//-----------------------------------------------------------------------------
//  AttrString
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
AttrString::AttrString (const char *  szNameIn) : Attr (szNameIn)
  {
  strValue.Empty ();
  szType = Identifier ();
  ccType = CCIdentifier ();
  };

//-----------------------------------------------------------------------------
AttrString::AttrString (const char *  szNameIn,
                        const char *  szValueIn) : Attr (szNameIn)
  {
  strValue.Set (szValueIn, TRUE);
  szType = Identifier ();
  ccType = CCIdentifier ();
  };

//-----------------------------------------------------------------------------
AttrString::~AttrString ()
  {
  };

//-----------------------------------------------------------------------------
AttrString &  AttrString::operator= (const AttrString &  attrIn)
  {
  Attr::CopyCommonVars (attrIn);
  strValue = attrIn.strValue;
  strValue.CalcHash();
  sigOnChangedString (strValue.AsChar ());
  OnChanged ();
  return (*this);
  };

//-----------------------------------------------------------------------------
const char *  AttrString::GetAsString  (RStr *  pstrValueOut) const
  {
  if (pstrValueOut == NULL) return ("");
  pstrValueOut->Set (strValue);
  return (pstrValueOut->AsChar ());
  };

//-----------------------------------------------------------------------------
VOID  AttrString::SetByString  (const char *  szValueIn)
  {
  Attr::SetByString (szValueIn);
  strValue.Set (szValueIn, TRUE);
  OnChanged ();
  };

//-----------------------------------------------------------------------------
VOID  AttrString::Clear  (VOID)
  {
  Attr::Clear ();
  strValue.Empty ();
  sigOnChangedString (strValue.AsChar ());
  OnChanged ();
  };

//-----------------------------------------------------------------------------
VOID  AttrString::UpdateFromRegistry  (BOOL  bUpdatingIn)
  {
  // if we are hooked up, refresh the text.
  if (pCachedRegistry == NULL) return;
  if (pCachedElem    == NULL) return;

  if (pCachedElem->IsType (ValueElem::kTypeString))
    {
    // strings
    strValue.Set (pCachedElem->GetString (), TRUE);
    }
  else if (pCachedElem->IsType (ValueElem::kTypeInt))
    {
    // ints
    strValue.Format ("%d", pCachedElem->GetInt ());
    }
  else if (pCachedElem->IsType (ValueElem::kTypeFloat))
    {
    // floats
    strValue.Format ("%f", pCachedElem->GetFloat ());
    };
  sigOnChangedString (strValue.AsChar ());
  OnChanged ();
  MarkAsDirty ();
  };



