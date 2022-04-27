/* -----------------------------------------------------------------
                          Sequence Manager

     This module implements a system to run InkScripts and drive the
   game with the script contents.

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

#ifndef SEQUENCEMANAGER_HPP
#define SEQUENCEMANAGER_HPP

#include "Sys/Types.hpp"
#include "Util/RStrParser.hpp"
#include "Containers/RStrArray.hpp"
#include "Containers/TList.hpp"
#include "ValueRegistry/ContentDepot.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Script/InkScript.hpp"
#include "Script/InkExecute.hpp"
#include "Script/SequenceManager.hpp"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Class Definitions
//-----------------------------------------------------------------------------


/*
* So, we create a SequenceManager class, and give it the InkScriptManager.
* We tell the SM to run a script.  It gets an executor, which comes with the script,

and the script has its value registry attached.
Need to be able to set a variable, and if it doesn't exist in other
registries/fallback registries, it is created in this one.


We call exec.Continue() while we can, and it
feeds us lines.  We break the lines apart and put them in the command queue.
We execute the command queue per frame. Some commands take time, some execute
immediately, and some require a continue touch input to finish.  Write the
command registry structure needed here, probably utilizing the existing script
system.   If the queue is empty, we check for touch input to continue and
get the next line.  Actually this touch input will come from specific input
blockers, and be directed specifically to the SM.  So we take the line, parse
out the double paren, break the insides into lines, and create commands.
Speaker:Line constructs will be turned into their own commands.  The commands
will set the registry variables that will then drive the UI.

*/

//-----------------------------------------------------------------------------
class SequenceCommand
  {
  public:
    RStr      strFnName;
    Token *   ptokParams;

  public:

    SequenceCommand  ();

    ~SequenceCommand ();

  };

// RegisterFunction/UnregisterFunction/UnregisterAllFunctions are used to maintain
//  a list of expressions that are specific to the sequence manager and not used
//  outside of it.

// NOTE: "CreateInstance()" must be called before using "Instance()"

//-----------------------------------------------------------------------------
class SequenceManager
  {
  private:

    InkScriptManager *       pScriptMan;
    TList<InkExecute*>       listExecutions;
    TList<SequenceCommand*>  listCommands;
    TList<ExpressionFn*>     listSequenceFn;

    BOOL                     bNeedsChoice;
    BOOL                     bHasChosen;
    INT                      iChoiceIndex;

    BOOL                     bNeedsContinue;
    BOOL                     bCanContinue;

    static SequenceManager *  pInstance;

  public:

    explicit           SequenceManager      (InkScriptManager *  pScriptManIn);

                       ~SequenceManager     ();

    static SequenceManager *  Instance        (VOID)                              {return pInstance;};
    static VOID               CreateInstance  (InkScriptManager *  pScriptManIn);
    static VOID               DestroyInstance (VOID);

    VOID               ReleaseExecutions    (VOID);

    VOID               DeleteCommands       (VOID);

    VOID               RunScript            (const char *  szScriptNameIn,
                                             const char *  szLabelIn = NULL);

    INT                GetNumExecutions     (VOID)                    {return listExecutions.Size ();};

    INT                GetNumCommands       (VOID)                    {return listCommands.Size ();};

    VOID               Update               (VOID); // pass time delta in?


    VOID               SetNeedsChoice       (BOOL  bStatusIn)         {bNeedsChoice = bStatusIn;};
    BOOL               NeedsChoice          (VOID)                    {return bNeedsChoice;};

    VOID               MakeChoice           (INT  iChoiceIndexIn);

    VOID               ClearChoice          (VOID);

    BOOL               HasMadeChoice        (VOID)      {return (bHasChosen);};


    VOID               SetNeedsContinue     (BOOL  bStatusIn)         {bNeedsContinue = bStatusIn;};
    BOOL               NeedsContinue        (VOID)                    {return bNeedsContinue;};

    VOID               TriggerContinue      (VOID);

    VOID               ClearContinue        (VOID);

    BOOL               CanContinue          (VOID)                    {return bCanContinue;};


    SequenceCommand *  ExtractCommands      (RStrParser &  parserLineIn);

    VOID               ExtractSequenceTags  (RStrParser &  parserLineIn,
                                             RStrArray &   arrayCommandsInOut);

    VOID               RegisterFunction       (const char *      szNameIn,
                                               ExpressionFnPtr   ptrFnIn);

    VOID               UnregisterFunction     (const char *      szNameIn);

    VOID               UnregisterAllFunctions (VOID);

    static VOID        ScriptContinueFn       (Token *          ptokParamsIn,
                                               ValueRegistry *  pRegistryIn,
                                               Token *          ptokResultOut);

    static VOID        ScriptChooseFn         (Token *          ptokParamsIn,
                                               ValueRegistry *  pRegistryIn,
                                               Token *          ptokResultOut);

    static VOID        ScriptSetCharFn        (Token *          ptokParamsIn,
                                               ValueRegistry *  pRegistryIn,
                                               Token *          ptokResultOut);

    static VOID        ScriptSayFn            (Token *          ptokParamsIn,
                                               ValueRegistry *  pRegistryIn,
                                               Token *          ptokResultOut);

    static VOID        RunScriptFn            (Token *          ptokParamsIn,
                                               ValueRegistry *  pRegistryIn,
                                               Token *          ptokResultOut);

    static const char * ParseChoiceTags       (const char *     pszTextIn,
                                               const RStr &     strKeyPrefixIn,
                                               ValueRegistry *  pRegistryIn,
                                               RStrArray &      arrayChoiceText);

  };


#endif // SEQUENCEMANAGER_HPP
