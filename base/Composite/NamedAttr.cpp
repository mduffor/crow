/* -----------------------------------------------------------------
                           Named Attr

     This module implements attributes with an additional name.

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

#ifdef REFERENCE

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Composite/NamedAttr.hpp"

//-----------------------------------------------------------------------------
//  NamedAttr
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
NamedAttr::NamedAttr () : pAttr (NULL)
  {
  };

//-----------------------------------------------------------------------------
NamedAttr::NamedAttr  (const char *  szNameIn,
                       Attr *        pAttrIn) : strName (szNameIn),
                                                pAttr   (pAttrIn)
  {
  };

//-----------------------------------------------------------------------------
NamedAttr::~NamedAttr ()
  {
  };

//-----------------------------------------------------------------------------
const char *  NamedAttr::Type (VOID) const
  {
  ASSERT (pAttr != NULL);
  return pAttr->Type ();
  };

//-----------------------------------------------------------------------------
const char *  NamedAttr::OriginalName  (VOID) const
  {
  ASSERT (pAttr != NULL);
  return pAttr->Name ();
  };

//-----------------------------------------------------------------------------
const char *  NamedAttr::GetAsString  (RStr *  pstrValueOut)
  {
  ASSERT (pAttr != NULL);
  return pAttr->GetAsString (pstrValueOut);
  };

//-----------------------------------------------------------------------------
VOID  NamedAttr::SetByString  (const char *  szValueIn)
  {
  ASSERT (pAttr != NULL);
  pAttr->SetByString (szValueIn);
  };

//-----------------------------------------------------------------------------
VOID  NamedAttr::SetArrayByString  (INT           iIndexIn,
                                    const char *  szValueIn)
  {
  ASSERT (pAttr != NULL);
  pAttr->SetArrayByString (iIndexIn, szValueIn);
  };

#endif // REFERENCE
