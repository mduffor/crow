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

#ifndef ATTRINT_HPP
#define ATTRINT_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Composite/Attr.hpp"

/**

  */

//-----------------------------------------------------------------------------
class AttrInt : public Attr
  {
  public:

  private:
    INT32         iValue;

  public:
    explicit              AttrInt      (const char *  szNameIn);
                          AttrInt      (const char *  szNameIn,
                                        INT32         iValueIn);
    virtual               ~AttrInt     ();

    AttrInt &             operator=    (const AttrInt &  attrIn);
    Attr *                Instantiate  (const char *  szNameIn) override {return new AttrInt (szNameIn);};
    static  const char *  Identifier   (VOID)                    {return "int";};
    static UINT32         CCIdentifier (VOID)                    {return MAKE_FOUR_CODE("INT_");};

    const char *          GetAsString  (RStr *  pstrValueOut) const override;
    VOID                  SetByString  (const char *  szValueIn) override;
    VOID                  SetByInt     (INT32  iIn)              {Set (iIn); OnChanged ();};
    VOID                  Set          (INT32  iIn)              {iValue = iIn;};
    VOID                  Set          (Attr *  pattrIn) override {if (pattrIn->IsCCType (CCIdentifier())) {iValue = dynamic_cast<AttrInt*>(pattrIn)->iValue;};};
    VOID                  Clear        (VOID) override;

    INT                   Value        (VOID)                    {return (INT)iValue;};

    VOID                  UpdateFromRegistry  (BOOL  bUpdatingIn) override;
  };

#endif // ATTRINT_HPP
