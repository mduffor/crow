/* -----------------------------------------------------------------
                             Attr Float

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

#ifndef ATTRFLOAT_HPP
#define ATTRFLOAT_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Composite/Attr.hpp"

/**

  */

//-----------------------------------------------------------------------------
class AttrFloat : public Attr
  {
  public:

  private:
    FLOAT      fValue;    //< Float value for this attr

  public:
    explicit              AttrFloat    (const char *  szNameIn);
                          AttrFloat    (const char *  szNameIn,
                                        FLOAT         fValueIn);
    virtual               ~AttrFloat   ();

    AttrFloat &           operator=    (const AttrFloat &  attrIn);

                                       /// @brief Factory method to create AttrFloat
                                       /// @param szNameIn The name of the new Attr
                                       /// @return A new AttrFloat instance
    Attr *                Instantiate  (const char *  szNameIn)  override {return new AttrFloat (szNameIn);};

                                       /// @brief Get the string identifier for this Attr type.
                                       /// @return The string identifier
    static const char *   Identifier   (VOID)                    {return "float";};

                                       /// @brief Get the string identifier for this Attr type.
                                       /// @return The FOURCC (32 bit int) identifier for this Attr type
    static UINT32         CCIdentifier (VOID)                    {return MAKE_FOUR_CODE("FLT_");};

                                       /// @brief Convert this attr's value to a string
                                       /// @param pstrValueOut The RStr to store the resulting value into
    const char *          GetAsString  (RStr *  pstrValueOut) const override;

                                       /// @brief Read this attr's value from a string.
                                       /// @param szValueIn The string from which the value will be parsed
    VOID                  SetByString  (const char *  szValueIn) override;

                                       /// @brief Set value via float
                                       /// @param fIn The new value for this attr
    VOID                  Set          (FLOAT  fIn)              {fValue = fIn;};

                                       /// @brief Set value via another AttrFloat
                                       /// @param fIn The attr containing the new value for this attr
    VOID                  Set          (Attr *  pattrIn) override        {if (pattrIn->IsCCType (CCIdentifier())) {fValue = dynamic_cast<AttrFloat*>(pattrIn)->fValue;};};

                                       /// @brief Set value to default (0.0f)
    VOID                  Clear        (VOID) override;

                                       /// @brief Query the value of this attr.
                                       /// @return The floating point value of this attr
    FLOAT                 Value        (VOID)                    {return fValue;};

    VOID                  UpdateFromRegistry  (BOOL  bUpdatingIn) override;
  };

#endif // ATTRFLOAT_HPP
