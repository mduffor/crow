/* -----------------------------------------------------------------
                                 InkScript Parser

     This module implements a scripting language which largely
     follows the Ink format.

     https://github.com/inkle/ink/blob/master/Documentation/WritingWithInk.md

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2016, Michael T. Duffy II.  All rights reserved.
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

#ifndef INKPARSER_HPP
#define INKPARSER_HPP

#include "Sys/Types.hpp"
#include "Containers/TArray.hpp"
#include "Util/RStr.hpp"
#include "Containers/RStrArray.hpp"
#include "Containers/IntArray.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Sys/TKeyValuePair.hpp"
#include "Script/Expression.hpp"
#include "Script/InkScript.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


//-----------------------------------------------------------------------------
class InkParser
  {
  private:
    BOOL          bIsBlockExpression;


  public:

                  InkParser          ();

                  ~InkParser         ();

    InkScript *   Parse              (const char *  szScriptIn);  /// Overwrites any existing content in this instance.

  private:

    VOID          LineToElem         (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      INT *         piGatherLevelInOut,
                                      InkElem **    ppInkElemOut);


    VOID          ParseKnot          (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      InkKnot * *   ppPrevKnotOut,
                                      InkElem * *   ppelemStartOut);

    VOID          ParseStitch        (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      InkKnot *     pPrevKnotIn,
                                      InkElem * *   ppelemStartOut);

    VOID          ParseGather        (RStrParser &  parserLineIn,
                                      INT *         piGatherLevel,
                                      INT *         piNextGatherLevel);

    VOID          ParseDivert        (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      InkElem * *   ppelemStartOut);

    VOID          ParseList          (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      InkElem * *   ppelemStartOut);

    VOID          ParseChoice        (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      INT *         piGatherLevel,
                                      INT *         piNextGatherLevel,
                                      InkElem * *   ppelemStartOut);

    VOID          ParseExpression    (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      InkElem * *   ppelemStartOut);

    VOID          ParseVarConst      (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      InkElem * *   ppelemStartOut);

    VOID          ParseParagraph     (RStrParser &  parserLineIn,
                                      InkScript *   pScriptIn,
                                      InkElem * *   ppelemStartOut);

  };




#endif // INKPARSER_HPP
