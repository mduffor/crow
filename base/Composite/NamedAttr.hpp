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

#ifndef NAMEDATTR_HPP
#define NAMEDATTR_HPP

#include "Sys/Types.hpp"
#include "Composite/Attr.hpp"
#include "Util/RStr.hpp"
#include "Containers/TList.hpp"

// NOTE: Is this used anywhere?


//-----------------------------------------------------------------------------
class NamedAttr
  {
  private:
    RStr     strName;
    Attr *   pAttr;

  public:
                          NamedAttr         ();

                          NamedAttr         (const char *   szNameIn,
                                             Attr *         pAttrIn);

                          ~NamedAttr        ();

            const char *  Type              (VOID) const;

            const char *  Name              (VOID) const                {return strName.AsChar ();};

            const char *  OriginalName      (VOID) const;

            Attr *        Attribute         (VOID) const                {return (pAttr);};

    virtual const char *  GetAsString       (RStr *  pstrValueOut);

    virtual VOID          SetByString       (const char *  szValueIn);

    virtual VOID          SetArrayByString  (INT           iIndexIn,
                                             const char *  szValueIn);

  };

#endif // NAMEDATTR_HPP

#endif // REFERENCE
