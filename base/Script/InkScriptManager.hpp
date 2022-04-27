/* -----------------------------------------------------------------
                                 Ink Script Manager

     This module implements a container for InkScripts and their execution.

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

#ifndef INKSCRIPTMANAGER_HPP
#define INKSCRIPTMANAGER_HPP

#include "Sys/Types.hpp"
#include "Util/RStrParser.hpp"
#include "Containers/RStrArray.hpp"
#include "Containers/TList.hpp"
#include "ValueRegistry/ContentDepot.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Script/InkScript.hpp"
#include "Script/InkExecute.hpp"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Class Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class InkScriptManager
  {
  private:

    TList<InkScript  *>     listScripts;
    TList<InkExecute *>     listExecutions;
    ContentDepot *          pDepot;
    ValueRegistry *         pRegistry;           /// registry for holding variables and knot/stitch/gather counts for execution

  private:

    VOID                   OnScriptBufferLoad  (const char *        pszNameIn,
                                                const char *        pszTypeIn,
                                                const RStrParser &  parserDataIn);

    BOOL                   IsExecuting         (const char *  szScriptNameIn);

    TListItr<InkScript *>  FindScript          (const char *  szScriptNameIn);

    TListItr<InkScript *>  FindScript          (RStr &        strScriptNameIn);

    VOID                   ReleaseAllExecutions (VOID);

    VOID                   ReleaseAllScripts    (VOID);

  public:

                           InkScriptManager    (ContentDepot *   pDepotIn,
                                                ValueRegistry *  pRegistryIn);

    virtual                ~InkScriptManager   ();

    VOID                   UpdateScriptList    (RStrArray &   arrayScriptNamesIn);

    VOID                   GetScriptList       (RStrArray &   arrayScriptNamesOut);

    VOID                   LoadScript          (const char *  szScriptNameIn);

    VOID                   UnloadScript        (const char *  szScriptNameIn);

    InkExecute *           RequestExecution    (const char *  szScriptNameIn,
                                                const char *  szStartingTagIn = NULL);

    VOID                   ReleaseExecution    (InkExecute *  pExecIn);

    VOID                   RunFunctionToEnd    (const char *  szScriptNameIn,
                                                const char *  szFunctionNameIn);

    VOID                   RunFunctionToEnd    (InkScript *   pScriptIn,
                                                const char *  szFunctionNameIn);

    VOID                   RunFunctionOnAll    (const char *  szFunctionNameIn);

    ValueRegistry *        GetRegistry         (VOID)         {return  pRegistry;};
  };










#endif // INKSCRIPTMANAGER_HPP
