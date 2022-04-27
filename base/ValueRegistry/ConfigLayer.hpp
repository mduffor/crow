/* -----------------------------------------------------------------
                            Config Layer

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

#ifndef CONFIGLAYER_HPP
#define CONFIGLAYER_HPP

#include "Sys/Types.hpp"
#include "Containers/TList.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
//#include "Util/Signal.h"
#include "ValueRegistry/ConfigSubset.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ConfigLayer
  {
  protected:

    ConfigSubset *        pBase;

  public:

    //BOOL    bClearOnReset;

  public:

                          ConfigLayer       ()                    {//bClearOnReset = FALSE;
                                                                   pBase = NULL;};

    virtual               ~ConfigLayer      ()                    {RemoveBaseSegment(); Clear();};

    const char *          GetName           (VOID)                {return (pBase->strName.AsChar());};

    UINT32                GetNameHash       (VOID)                {return (pBase->strName.GetHash());};

    BOOL                  NameEquals        (UINT32        uNameHashIn,
                                             const char *  szNameIn)     {return ((pBase == NULL) ? FALSE : (pBase->strName.Equals (uNameHashIn, szNameIn)));};

    const char *          GetDesc           (VOID)                {return (pBase->strDesc.AsChar());};

    INT                   GetOrder          (VOID)                {return ((pBase == NULL) ? 0 : pBase->iOrder);};

    VOID                  SetOrder          (INT  iOrderIn)       {if (pBase != NULL) {pBase->iOrder = iOrderIn;};};

    VOID                  Clear             (VOID)                {RemoveAllSegments ();};

    EStatus               Deserialize       (RStrParser &  parserIn,
                                             const char *  szSourceIn = NULL);

    VOID                  ToJSON            (RStrParser &  parserOut,
                                             const char *  strSectionIndent = "    ",
                                             const char *  strLineIndent    = "  ",
                                             const char *  strLineEnd       = "\n");

    BOOL                  IsActive          (ValueRegistry &  regIn);

    BOOL                  IsExpressionDriven (VOID);

    ConfigSubset *        GetBaseSegment     (VOID);

    ConfigSubset *        GetActiveSegment   (ValueRegistry &  regIn);

    VOID                  RemoveBaseSegment  (VOID);

    VOID                  RemoveAllSegments  (VOID);

    VOID                  MergeInto          (ValueRegistry &  regTargetIn);

  };



#endif // CONFIGLAYER_HPP
