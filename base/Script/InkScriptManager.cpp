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

#include <stdlib.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Script/InkParser.hpp"
#include "Script/InkScriptManager.hpp"


//-----------------------------------------------------------------------------
InkScriptManager::InkScriptManager  (ContentDepot *   pDepotIn,
                                     ValueRegistry *  pRegistryIn)
  {
  pDepot    = pDepotIn;
  pRegistry = pRegistryIn;

  pDepot->sigOnDataLoad.Connect (this, &InkScriptManager::OnScriptBufferLoad);
  };

//-----------------------------------------------------------------------------
InkScriptManager::~InkScriptManager ()
  {
  pDepot->sigOnDataLoad.Disconnect (this, &InkScriptManager::OnScriptBufferLoad);

  ReleaseAllExecutions ();
  ReleaseAllScripts ();
  };


//-----------------------------------------------------------------------------
VOID InkScriptManager::ReleaseAllExecutions (VOID)
  {
  for (TListItr<InkExecute *>  itrCurr = listExecutions.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listExecutions.Empty();
  };

//-----------------------------------------------------------------------------
VOID InkScriptManager::ReleaseAllScripts (VOID)
  {
  TListItr<InkScript *>  itrNext;

  for (TListItr<InkScript *>  itrCurr = listScripts.First ();
       itrCurr.IsValid ();
       itrCurr = itrNext)
    {
    itrNext = itrCurr;
    ++itrNext;

    UnloadScript  ((*itrCurr)->strName.AsChar ());
    };
  listScripts.Empty();
  };

//-----------------------------------------------------------------------------
TListItr<InkScript *>  InkScriptManager::FindScript  (const char *  szScriptNameIn)
  {
  RStr  strScriptNameIn (szScriptNameIn, TRUE);
  return (FindScript (strScriptNameIn));
  };

//-----------------------------------------------------------------------------
TListItr<InkScript *> InkScriptManager::FindScript  (RStr &  strScriptNameIn)
  {
  TListItr<InkScript *>  itrCurr;

  for (itrCurr = listScripts.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->strName.Equals(strScriptNameIn))
      {
      break;
      };
    };
  return (itrCurr);
  };

//-----------------------------------------------------------------------------
BOOL  InkScriptManager::IsExecuting (const char *  szScriptNameIn)
  {
  RStr                    strScriptNameIn (szScriptNameIn, TRUE);
  TListItr<InkExecute *>  itrCurr;

  for (itrCurr = listExecutions.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->pScript->strName.Equals (strScriptNameIn))
      {
      return (TRUE);
      };
    };
  return (FALSE);
  };

//-----------------------------------------------------------------------------
VOID  InkScriptManager::UpdateScriptList (RStrArray &   arrayScriptNamesIn)
  {
  TListItr<InkScript*>  itrFound;

  // Find the scripts to load
  INT  iNumScriptsIn = arrayScriptNamesIn.Length();
  for (INT  iIndex = 0; iIndex < iNumScriptsIn; ++iIndex)
    {
    itrFound = FindScript ((arrayScriptNamesIn)[iIndex]);

    if (! itrFound.IsValid ())
      {
      // Script isn't in list yet
      LoadScript ((arrayScriptNamesIn)[iIndex].AsChar());
      };
    };

  // Find the scripts to unload
  TListItr<InkScript *>  itrCurr;
  TListItr<InkScript *>  itrNext;

  for (itrCurr = listScripts.First (); itrCurr.IsValid (); itrCurr = itrNext)
    {
    itrNext = itrCurr;
    ++itrNext;
    if (arrayScriptNamesIn.Find ((*itrCurr)->strName) == -1)
      {
      UnloadScript ((*itrCurr)->strName.AsChar());
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  InkScriptManager::GetScriptList  (RStrArray &   arrayScriptNamesOut)
  {
  for (TListItr<InkScript *>  itrCurr = listScripts.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    arrayScriptNamesOut.AppendUnique ((*itrCurr)->strName);
    };
  };

//-----------------------------------------------------------------------------
VOID  InkScriptManager::LoadScript (const char *  szScriptNameIn)
  {
  // load script and call Activate()

  TListItr<InkScript *>  itrFound = FindScript  (szScriptNameIn);

  if (! itrFound.IsValid ())
    {
    //DBG_INFO ("LoadAsset (%s)", szScriptNameIn);
    pDepot->LoadAsset (szScriptNameIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  InkScriptManager::UnloadScript  (const char *  szScriptNameIn)
  {
  // We can't unload a script if it is being used.
  if (IsExecuting (szScriptNameIn)) {return;};

  // call Deactivate and unload script
  TListItr<InkScript *>  itrScript = FindScript  (szScriptNameIn);
  if (itrScript.IsValid ())
    {
    RunFunctionToEnd ((*itrScript), "Deactivate");
    };
  listScripts.Delete (itrScript);
  };

//-----------------------------------------------------------------------------
InkExecute *  InkScriptManager::RequestExecution (const char *  szScriptNameIn,
                                                  const char *  szStartingTagIn)
  {
  TListItr<InkScript *>  itrScript = FindScript  (szScriptNameIn);

  if (! itrScript.IsValid ()) return (NULL);

  InkExecute *  pExec = listExecutions.PushBack (new InkExecute (*itrScript));
  pExec->SetRegistry (pRegistry);
  pExec->StartLabel (szStartingTagIn);
  return (pExec);
  };

//-----------------------------------------------------------------------------
VOID  InkScriptManager::ReleaseExecution (InkExecute *  pExecIn)
  {
  ASSERT (pExecIn != NULL);
  TListItr<InkExecute *>  itrSearch;
  if (listExecutions.Find (pExecIn, itrSearch))
    {
    listExecutions.Delete (itrSearch);
    };
  if (pExecIn != NULL)
    {
    delete (pExecIn);
    };
  };

//-----------------------------------------------------------------------------
VOID InkScriptManager::OnScriptBufferLoad (const char *        pszNameIn,
                                           const char *        pszTypeIn,
                                           const RStrParser &  parserDataIn)
  {
  // Upon receiving the buffer from the content depot, we need to create and
  //  parse the script, store it, and call any Activate()
  InkParser  parser;

  InkScript *  pScript = parser.Parse (parserDataIn.AsChar ());

  listScripts.PushBack (pScript);
  pScript->strName.Set (pszNameIn, TRUE);

  RunFunctionToEnd (pScript, "Activate");
  };

//-----------------------------------------------------------------------------
VOID InkScriptManager::RunFunctionToEnd  (const char *  szScriptNameIn,
                                          const char *  szFunctionNameIn)
  {
  TListItr<InkScript *>  itrScript = FindScript  (szScriptNameIn);
  if (! itrScript.IsValid ()) return;

  RunFunctionToEnd (*itrScript, szFunctionNameIn);
  };

//-----------------------------------------------------------------------------
VOID InkScriptManager::RunFunctionToEnd (InkScript *   pScriptIn,
                                         const char *  szFunctionNameIn)
  {
  // TODO:  Handle function parameters

  InkExecute  exec (pScriptIn);
  exec.SetRegistry (pRegistry);
  exec.StartFunction (szFunctionNameIn);

  while (exec.CanContinue ())
    {
    exec.Continue ();
    };
  };

//-----------------------------------------------------------------------------
VOID  InkScriptManager::RunFunctionOnAll    (const char *  szFunctionNameIn)
  {
  // Run this function call on all active scripts.

  for (TListItr<InkScript *>  itrCurr = listScripts.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->GetFunctionIndex (szFunctionNameIn) != -1)
      {
      RunFunctionToEnd ((*itrCurr), szFunctionNameIn);
      };
    };
  };



