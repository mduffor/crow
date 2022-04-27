/* -----------------------------------------------------------------
                            Config Subset

     This module implements a value registry composed of overlapping
     layers of key-value pairs.

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2021, Michael T. Duffy II.  All rights reserved.
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

#ifndef CONFIGSUBSET_HPP
#define CONFIGSUBSET_HPP

#include "Sys/Types.hpp"
#include "Containers/TList.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"
//#include "Util/Signal.h"


//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ConfigSubset
  {
  protected:

    // JSON keys
    //static string


  public:
    RStr    strName;
    RStr    strDesc;
    RStr    strEnabled;
    RStr    strPreReq;
    RStr    strPrefix; // prefix string added at the front of every key
    INT     iOrder;

    ValueRegistrySimple   regVariables;

    TList<ConfigSubset*>  listSegments;

  public:

                  ConfigSubset        ()                   {iOrder = 0;};

    virtual       ~ConfigSubset       ()                   {RemoveAll ();};

    static EStatus  ParseSubsetList     (RStrParser &            parserIn,
                                         const char *            szPrefixIn,
                                         TList<ConfigSubset*> &  listOut);

    VOID          RemoveAll           (VOID);

    EStatus       ParseSubset         (RStrParser &  parserIn,
                                       const char *  szPrefixIn);

    EStatus       ParseVariables      (RStrParser &  parserIn);

    const char *  GuessValueType      (RStrParser &  parserIn);

    VOID          ToJSON              (RStrParser &  parserOut,
                                       const char *  strSectionIndent = "    ",
                                       const char *  strLineIndent    = "  ",
                                       const char *  strLineEnd       = "\n");

    const char *  ElemToTypePrefix    (ValueElem &      elemIn);

    BOOL          IsActive            (ValueRegistry &  regIn);

    BOOL          EvaluateExpression  (RStr &           strExpressionIn,
                                       ValueRegistry &  regIn);

    BOOL          IsExpressionDriven  (VOID);

    VOID          MergeInto           (ValueRegistry &  regTargetIn,
                                       INT              iOrderIn);

  };



#endif // CONFIGSUBSET_HPP
