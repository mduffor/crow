/* -----------------------------------------------------------------
                             Attr Int

     This module implements floating point attributes

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

#include "Composite/AttrInt.hpp"

//-----------------------------------------------------------------------------
//  AttrInt
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
AttrInt::AttrInt (const char *  szNameIn) : Attr (szNameIn)
  {
  iValue = 0;
  szType = Identifier ();
  ccType = CCIdentifier ();
  };

//-----------------------------------------------------------------------------
AttrInt::AttrInt (const char *  szNameIn,
                  INT32         iValueIn) : Attr (szNameIn)
  {
  iValue = iValueIn;
  szType = Identifier ();
  ccType = CCIdentifier ();
  };

//-----------------------------------------------------------------------------
AttrInt::~AttrInt ()
  {
  };

//-----------------------------------------------------------------------------
AttrInt &  AttrInt::operator= (const AttrInt &  attrIn)
  {
  Attr::CopyCommonVars (attrIn);
  iValue = attrIn.iValue;
  OnChanged ();
  return (*this);
  };

//-----------------------------------------------------------------------------
const char *  AttrInt::GetAsString  (RStr *  pstrValueOut) const
  {
  if (pstrValueOut == NULL) return ("");
  pstrValueOut->AppendFormat ("%d", iValue);
  return (pstrValueOut->AsChar ());
  };

//-----------------------------------------------------------------------------
VOID  AttrInt::SetByString  (const char *  szValueIn)
  {
  Attr::SetByString (szValueIn);
  iValue = (INT) strtol (szValueIn, NULL, 0);
  OnChanged ();
  };

//-----------------------------------------------------------------------------
VOID  AttrInt::Clear  (VOID)
  {
  Attr::Clear ();
  iValue = 0;
  OnChanged ();
  };

//-----------------------------------------------------------------------------
VOID  AttrInt::UpdateFromRegistry  (BOOL  bUpdatingIn)
  {
  // if we are hooked up, refresh the text.
  if (pCachedRegistry == NULL) return;
  if (pCachedElem    == NULL) return;

  if (pCachedElem->IsType (ValueElem::kTypeString))
    {
    // string
    SetByString (pCachedElem->GetString ());
    return;
    }
  else if (pCachedElem->IsType (ValueElem::kTypeInt))
    {
    // int
    iValue = pCachedElem->GetInt ();
    }
  else if (pCachedElem->IsType (ValueElem::kTypeFloat))
    {
    // float
    iValue = (INT) floorf (pCachedElem->GetFloat ());
    };
  MarkAsDirty ();
  OnChanged ();
  };


