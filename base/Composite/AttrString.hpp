/* -----------------------------------------------------------------
                             Attr String

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

#ifndef ATTRSTRING_HPP
#define ATTRSTRING_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Composite/Attr.hpp"

/**

  */

//-----------------------------------------------------------------------------
class AttrString : public Attr
  {
  public:

    RStr                   strValue;
    Signal1<const char *>  sigOnChangedString;  ///< Event fired when the value of this string attr changes

  public:
    explicit              AttrString    (const char *  szNameIn);
                          AttrString    (const char *  szNameIn,
                                         const char *  szValueIn);
    virtual               ~AttrString   ();

    AttrString &           operator=   (const AttrString &  attrIn);
    Attr *                Instantiate  (const char *  szNameIn)  override {return new AttrString (szNameIn);};
    static  const char *  Identifier   (VOID)                    {return "string";};
    static UINT32         CCIdentifier (VOID)                    {return MAKE_FOUR_CODE("STR_");};

    const char *          GetAsString  (RStr *  pstrValueOut) const override;
    VOID                  SetByString  (const char *  szValueIn) override;
    VOID                  Set          (const char *  szValueIn) {strValue.Set (szValueIn, TRUE);};
    VOID                  Set          (Attr *  pattrIn)  override  {if (pattrIn->IsCCType (CCIdentifier())) {strValue.Set (dynamic_cast<AttrString*>(pattrIn)->strValue, TRUE);};};
    const char *          GetString    (VOID)                    {return strValue.AsChar ();};
    UINT32                GetHash      (VOID)                    {return strValue.GetHash ();};

    VOID                  Clear        (VOID) override;

    BOOL                  IsEmpty      (VOID)                    {return strValue.IsEmpty ();};

    RStr &                Value        (VOID)                    {return strValue;};

    VOID                  UpdateFromRegistry  (BOOL  bUpdatingIn) override;

  };

#endif // ATTRSTRING_HPP
