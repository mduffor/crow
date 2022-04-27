/* -----------------------------------------------------------------
                            Config Deck

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

#ifndef CONFIGDECK_HPP
#define CONFIGDECK_HPP

#include "Sys/Types.hpp"
#include "Containers/TList.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Util/Signal.h"
#include "ValueRegistry/ConfigLayer.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------
using namespace Gallant;

//-----------------------------------------------------------------------------
class ConfigDeck
  {
  protected:

  public:

    TList<ConfigLayer*>   listLayers;

    Signal0<>             sigOnResolveStack;   ///< Event fired when the stack is re-solved

    ValueRegistry *       pReg;

    static ConfigDeck *   pInstance;

  public:

                         ConfigDeck         (ValueRegistry *  pRegIn = NULL);

    virtual              ~ConfigDeck        ()                    {};

    static ConfigDeck *  Instance           (VOID)                {if (pInstance == NULL) {pInstance = new ConfigDeck ();}; return pInstance;};

    static VOID          Init               (VOID);

    EStatus              Deserialize        (RStrParser &  parserJsonIn,
                                             const char *  szSourceIn = NULL);

    ConfigLayer *        FindLayerByName    (const char *  szNameIn);

    ConfigLayer *        FindOrCreateLayer  (const char *  szNameIn,
                                             INT           iOrderIn);

    INT                  GetLayerCount      (VOID);

    VOID                 DeleteAllLayers    (VOID);

    EStatus              ReadBuffer         (const char *  szNameIn,
                                             const char *  szTypeIn,
                                             const char *  szBufferIn);

    EStatus              ReadBuffer         (const char *  szNameIn,
                                             const char *  szTypeIn,
                                             RStrParser &  parserIn);

    EStatus              ReadFile           (const char *  szFileIn);

    VOID                 ReadDirectory      (const char *  szRelativePathIn,
                                             const char *  szGlobMatchIn);

    static INT           CompareLayerOrders (ConfigLayer*  pAIn,
                                             ConfigLayer*  pBIn);

    VOID                 ResolveStack       (ValueRegistry *  pRegIn,
                                             BOOL             bFullResetIn);

    static const char *  GetConfigSavePath  ();

    static const char *  GetConfigCachePath ();

    static const char *  GetConfigPath      (const char *  szSubdirectory);


  };



#endif // CONFIGDECK_HPP
