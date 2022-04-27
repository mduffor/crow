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

#include <stdlib.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Script/InkParser.hpp"
#include "Script/InkScriptManager.hpp"
#include "Script/Expression.hpp"
#include "Script/SequenceManager.hpp"
#include "Util/ParseTools.hpp"

#define OPT_DBG_INFO(...)      ((void)0)
//#define OPT_DBG_INFO  DBG_INFO

#define OPT_DBG_MARK(...)      ((void)0)
//#define OPT_DBG_MARK  DBG_MARK

//=============================================================================
// Sequence Command
//=============================================================================

//-----------------------------------------------------------------------------
SequenceCommand::SequenceCommand  ()
  {
  ptokParams   = NULL;
  };

//-----------------------------------------------------------------------------
SequenceCommand::~SequenceCommand  ()
  {
  if (ptokParams != NULL)
    {
    // NOTE: The destructor of Token will delete the other linked Tokens
    delete ptokParams;
    };
  };


//=============================================================================
// Sequence Manager
//=============================================================================

SequenceManager *  SequenceManager::pInstance = NULL;


//-----------------------------------------------------------------------------
SequenceManager::SequenceManager  (InkScriptManager *  pScriptManIn)
  {
  ASSERT (pScriptManIn != NULL);

  pScriptMan = pScriptManIn;

  ClearChoice ();
  ClearContinue ();
  };

//-----------------------------------------------------------------------------
SequenceManager::~SequenceManager ()
  {
  ReleaseExecutions ();
  DeleteCommands    ();
  UnregisterAllFunctions ();
  };

//-----------------------------------------------------------------------------
VOID  SequenceManager::CreateInstance  (InkScriptManager *  pScriptManIn)
  {
  if (pInstance == NULL)
    {
    pInstance = new SequenceManager (pScriptManIn);

    Expression::RegisterFunction (new ExpressionFn ("ScriptContinue", &SequenceManager::ScriptContinueFn));
    Expression::RegisterFunction (new ExpressionFn ("ScriptChoose",   &SequenceManager::ScriptChooseFn));
    Expression::RegisterFunction (new ExpressionFn ("ScriptSetChar",  &SequenceManager::ScriptSetCharFn));
    Expression::RegisterFunction (new ExpressionFn ("ScriptSay",      &SequenceManager::ScriptSayFn));
    Expression::RegisterFunction (new ExpressionFn ("RunScript",      &SequenceManager::RunScriptFn));
    };
  };

//-----------------------------------------------------------------------------
VOID  SequenceManager::DestroyInstance (VOID)
  {
  if (pInstance != NULL)
    {
    delete pInstance;
    pInstance = NULL;

    Expression::UnregisterFunction ("ScriptContinue");
    Expression::UnregisterFunction ("ScriptChoose");
    Expression::UnregisterFunction ("ScriptSetChar");
    Expression::UnregisterFunction ("ScriptSay");
    Expression::UnregisterFunction ("RunScript");
    };
  };

//-----------------------------------------------------------------------------
VOID SequenceManager::ReleaseExecutions (VOID)
  {
  ASSERT (pScriptMan != NULL);
  for (TListItr<InkExecute*>  itrCurr = listExecutions.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    pScriptMan->ReleaseExecution (*itrCurr);
    };
  listExecutions.Empty ();
  };

//-----------------------------------------------------------------------------
VOID SequenceManager::DeleteCommands (VOID)
  {
  for (TListItr<SequenceCommand*>  itrCurr = listCommands.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listCommands.Empty();
  };

//-----------------------------------------------------------------------------
VOID SequenceManager::RunScript (const char *  szScriptNameIn,
                                 const char *  szLabelIn)
  {
  OPT_DBG_INFO ("RunScript (%s, %s) being pushed to execution list", szScriptNameIn, szLabelIn);
  InkExecute *  pExec = pScriptMan->RequestExecution (szScriptNameIn, szLabelIn);

  listExecutions.PushBack (pExec);
  };

//-----------------------------------------------------------------------------
VOID SequenceManager::Update (VOID) // pass time delta in?
  {
  OPT_DBG_MARK ("Update");
  RStrParser    parserNextLine;

  // NOTE: Since this isn't driving animations, directly (just probably queueing
  //  them up), this Update() call probably shouldn't be called once per frame,
  //  but once per second (and immediately after MakeChoice or TriggerContinue is
  //  called)

  // execute queue of commands
  while ((listExecutions.Size () > 0) || (listCommands.Size () > 0))

    {
    // if queue is empty, try to re-fill it with the script, and keep executing.
    while (listCommands.Size () == 0)
      {
      InkExecute *  pExec = *(listExecutions.First ());
      if (pExec != NULL)
        {
        InkScript * pScr = pExec->GetScript ();

        //DBG_INFO ("Starting execution of script %s", pScr->strName.AsChar());
        //pScr->DebugPrint ();

        // Note on logic flow: If a "say" command has a choice, it will have already
        //  been queued in listCommands.  Then it will notice when a choice is made,
        //  terminate, re-run this outer loop, and wind up here to be resolved.
        if (HasMadeChoice ())
          {
          pExec->ChooseChoiceIndex  (iChoiceIndex);
          ClearChoice ();

          // Read and discard the choice text, due to the way questions are asked via
          // buttons in Figment.  If you were doing a revealed continuous story like 80 Days,
          // you would want to keep the returned text and display it.
          pExec->Continue ();
          };

        if (pExec->CanContinue ())
          {
          parserNextLine.Set (pExec->Continue ());

          // process line
          SequenceCommand *  pcmdSay = ExtractCommands (parserNextLine);

          OPT_DBG_INFO ("Line is \"%s\", num choices is %d", parserNextLine.AsChar (), pExec->GetNumChoices ());

          // In addition to the text dialog, we also attach any choices to this command.
          if ((pcmdSay != NULL) && (pExec->HasChoices ()))
            {
            for (INT  iIndex = 0; iIndex < pExec->GetNumChoices (); ++iIndex)
              {
              const char *  pszChoiceText = pExec->GetChoiceText (iIndex);
              INT           iTextLength = strlen (pszChoiceText);

              OPT_DBG_INFO ("Choice %d has text of \"%s\" and length of %d", iIndex, pszChoiceText, iTextLength);

              pcmdSay->ptokParams->ArrayAppend (new Token (pszChoiceText, iTextLength));
              };
            };
          };
        // If script has completed, remove it from listExecutions
        if (! pExec->HasContent ())
          {
          pScriptMan->ReleaseExecution (listExecutions.PopFront());
          if (listExecutions.Size () == 0) break;
          };
        };
      };

    // TODO: Determine if listCommands needs to be part of each execution, or if there is one master list.
    while (listCommands.Size () != 0)
      {
      Token   tokenReturn;

      tokenReturn.eType = TokenType::kInt;
      tokenReturn.Set (1);

      // commands return TRUE (1) when they are complete, and can thus be removed from the queue.
      // commands return FALSE (0) if they were unable to complete this time, and will need to be called
      //  again next Update loop iteration.

      // NOTE:  This could be sped up by caching the ExpressionFn the first time the command
      //  is run so that it isn't looked up every update
      SequenceCommand *  pcmdCurr = listCommands.PeekFront ();
      Expression::CallFunction (pcmdCurr->strFnName.AsChar (),
                                pcmdCurr->ptokParams,
                                pScriptMan->GetRegistry (),
                                &listSequenceFn,
                                &tokenReturn);
      if (tokenReturn.AsInt () == 1)
        {
        // complete
        delete (listCommands.PopFront ());
        }
      else
        {
        // still running
        return;
        };
      };
    };
  };


//-----------------------------------------------------------------------------
VOID  SequenceManager::MakeChoice (INT  iChoiceIndexIn)
  {
  bHasChosen   = TRUE;
  iChoiceIndex = iChoiceIndexIn;
  };

//-----------------------------------------------------------------------------
VOID  SequenceManager::ClearChoice (VOID)
  {
  SetNeedsChoice (FALSE);
  bHasChosen   = FALSE;
  iChoiceIndex = -1;
  pScriptMan->GetRegistry ()->SetInt ("Script.NumChoices", 0);
  };

//-----------------------------------------------------------------------------
VOID  SequenceManager::TriggerContinue (VOID)
  {
  bCanContinue = TRUE;
  };

//-----------------------------------------------------------------------------
VOID  SequenceManager::ClearContinue (VOID)
  {
  SetNeedsContinue (FALSE);
  bCanContinue = FALSE;
  };

//-----------------------------------------------------------------------------
SequenceCommand *  SequenceManager::ExtractCommands (RStrParser &  parserLineIn)
  {

  // NOTE: This call will modify parserLineIn
  RStrArray     arrayCommands;
  RStrParser    parserLine;
  RStr          strParam;
  Token *       ptokCurr;
  SequenceCommand *  pCommand;

  ExtractSequenceTags (parserLineIn, arrayCommands);

  INT  iNumCommands = arrayCommands.Length();
  for (INT  iIndex = 0; iIndex < iNumCommands; ++iIndex)
    {
    pCommand = listCommands.PushBack (new SequenceCommand ());

    parserLine.Set (arrayCommands [iIndex]);

    // First word is the function name, the rest are parameters
    pCommand->strFnName = parserLine.GetWord ();
    while (! parserLine.IsEOL())
      {
      parserLine.GetQuoteString (&strParam);


      ptokCurr = new Token (strParam.AsChar (), strParam.Length ());

      if (pCommand->ptokParams == NULL)
        {
        pCommand->ptokParams = ptokCurr;
        }
      else
        {
        pCommand->ptokParams->ArrayAppend (ptokCurr);
        };
      };
    };

  // If anything remains after extracting sequence tags, treat it is content to
  //  be displayed to the player
  parserLineIn.StripTrailingWhitespace (TRUE);
  parserLineIn.StripLeadingWhitespace  ();

  if (parserLineIn.Length() > 0)
    {
    RStr  strCharacterName;
    INT  iColonIndex = parserLineIn.FindChar (':');

    if (iColonIndex != -1)
      {
      strCharacterName = parserLineIn.ChompLeft (iColonIndex);
      strCharacterName.StripTrailingWhitespace ();
      strCharacterName.StripLeadingWhitespace  ();

      parserLineIn.TruncateLeft (1);
      parserLineIn.StripLeadingWhitespace  ();
      };

    if (! strCharacterName.IsEmpty ())
      {
      pCommand = listCommands.PushFront (new SequenceCommand ());
      pCommand->strFnName.Set ("ScriptSetChar");
      pCommand->ptokParams = Token::MakeStringToken (strCharacterName.AsChar (), strCharacterName.Length ());
      };

    pCommand = listCommands.PushBack (new SequenceCommand ());
    pCommand->strFnName.Set ("ScriptSay");
    pCommand->ptokParams = Token::MakeStringToken (strCharacterName.AsChar (), strCharacterName.Length ());
    pCommand->ptokParams->ArrayAppend (Token::MakeStringToken (parserLineIn.AsChar (), parserLineIn.Length ()));

    return (pCommand);
    };
  return (NULL);
  };


//-----------------------------------------------------------------------------
VOID SequenceManager::ExtractSequenceTags (RStrParser &  parserLineIn,
                                           RStrArray &   arrayCommandsInOut)
  {
  // strip out sequence commands from double-parenthesis
  RStrParser    parserCurrCommands;
  const char *  szOpenParen  = "((";
  const char *  szCloseParen = "))";
  INT           iTagLength = strlen (szOpenParen);

  for (INT  iOpenParenIndex = parserLineIn.Find (szOpenParen);
       iOpenParenIndex != -1;
       iOpenParenIndex = parserLineIn.Find (szOpenParen, iOpenParenIndex))
    {
    INT  iCloseParenIndex = parserLineIn.Find (szCloseParen, iOpenParenIndex + iTagLength);

    if (iCloseParenIndex == -1)
      {
      // Error: Missing closing parenthesis
      DBG_ERROR ("Missing closing sequence parenthesis");
      return;
      }

    // if paren are surrounded by two spaces, remove one of them
    INT  iSpaceAdjust = 0;
    if ((iOpenParenIndex > 0) &&
        (iCloseParenIndex + iTagLength < (INT) parserLineIn.Length ()) &&
        (parserLineIn.PeekChar (iOpenParenIndex - 1) == ' ') &&
        (parserLineIn.PeekChar (iCloseParenIndex + iTagLength) == ' '))
      {
      iSpaceAdjust = 1;
      };

    INT  iTotalLength = iCloseParenIndex - iOpenParenIndex + iTagLength;
    parserLineIn.SubString (iOpenParenIndex + iTagLength,
                            iTotalLength - iTagLength - iTagLength,
                            parserCurrCommands);
    parserLineIn.ClipMiddle (iOpenParenIndex, (UINT32) iTotalLength + iSpaceAdjust);

    parserCurrCommands.Tokenize (';', arrayCommandsInOut);
    };
  };

//-----------------------------------------------------------------------------
VOID  SequenceManager::RegisterFunction (const char *      szNameIn,
                                         ExpressionFnPtr   ptrFnIn)
  {
  listSequenceFn.PushFront (new ExpressionFn (szNameIn, ptrFnIn));
  };

//-----------------------------------------------------------------------------
VOID  SequenceManager::UnregisterFunction (const char *      szNameIn)
  {
  for (TListItr<ExpressionFn*> itrCurr = listSequenceFn.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->MatchesName (szNameIn))
      {
      listSequenceFn.Delete (itrCurr);
      break;
      }
    };
  };

//-----------------------------------------------------------------------------
VOID  SequenceManager::UnregisterAllFunctions (VOID)
  {
  for (TListItr<ExpressionFn*> itrCurr = listSequenceFn.First (); itrCurr.IsValid (); ++itrCurr)
    {
    delete (*itrCurr);
    };
  listSequenceFn.Empty ();
  };

//------------------------------------------------------------------------------
VOID SequenceManager::ScriptContinueFn (Token *          ptokParamsIn,
                                        ValueRegistry *  pRegistryIn,
                                        Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 0) return;

  SequenceManager *  pSeq = SequenceManager::Instance();
  ASSERT (pSeq != NULL);

  pSeq->TriggerContinue();
  };

//------------------------------------------------------------------------------
VOID SequenceManager::ScriptChooseFn (Token *          ptokParamsIn,
                                      ValueRegistry *  pRegistryIn,
                                      Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  SequenceManager *  pSeq = SequenceManager::Instance();
  ASSERT (pSeq != NULL);

  int iChoice = ptokParamsIn->AsInt (pRegistryIn);

  pSeq->MakeChoice(iChoice);
  };

//------------------------------------------------------------------------------
VOID SequenceManager::ScriptSetCharFn (Token *          ptokParamsIn,
                                       ValueRegistry *  pRegistryIn,
                                       Token *          ptokResultOut)
  {
  // NOTE:  The ScriptSetChar and ScriptSay methods are represented as ExpressionFn
  //  so that they can be queued as commands to be executed.

  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  const char *  szCharName = ptokParamsIn->AsString (pRegistryIn);

  pRegistryIn->SetString ("Script.CurrChar", szCharName);
  };

//------------------------------------------------------------------------------
VOID SequenceManager::ScriptSayFn (Token *          ptokParamsIn,
                                   ValueRegistry *  pRegistryIn,
                                   Token *          ptokResultOut)
  {
  INT           iNumTokens = ptokParamsIn->ArrayCount ();


  if (iNumTokens < 2) return;

  // NOTE: This method is called back as a static, and then we use the
  //  static Instance () method to find the SequenceManager again.  This
  //  could lead to a disconnect if you attempt to use multiple
  //  SequenceManagers.

  SequenceManager *  pSeqStatic = SequenceManager::Instance ();

  // The "Say" command has two scenarios it deals with.  Either it is
  //  passed a Speaker:Text pair, in which case it waits for a ScriptContinue
  //  to progress, or it is passed Speaker:Text:Choices, and requires a
  //  ScriptChoose to continue.
  // This expression will be caused repeatedly, every frame, as it polls for
  //  the signal that this script is finished (continue or choice).  Therefore
  //  we need to make sure its processing is fast and efficient.  To that end,
  //  we set either NeedsContinue or NeedsChoice the first time through, and on
  //  subsequent iterations we detect that either flag has been set, we
  //  can skip the expensive setting of state and only focus on the faster polling
  //  of the Continue or Choice state.

  if (! pSeqStatic->NeedsContinue () && ! pSeqStatic->NeedsChoice ())
    {
    RStrArray     astrChoices;

    // Read the Say parameters, and save them to the registry
    //  so that the UI can access them.

    const char *  szCharName = ptokParamsIn->AsString (pRegistryIn);
    const char *  szCharText = ptokParamsIn->Next()->AsString (pRegistryIn);

    pRegistryIn->SetString ("Script.CurrChar",    szCharName);
    pRegistryIn->SetString ("Script.CurrText",    szCharText);

    DBG_INFO ("Setting Script.CurrChar to %s",    szCharName);
    DBG_INFO ("Setting Script.CurrText to %s",    szCharText);

    astrChoices.Clear ();
    Token *  pParam = ptokParamsIn->Next()->Next();
    INT   iChoiceIndex = 0;
    RStr  strTextKey;
    RStr  strKeyPrefix  ("Script.");

    for (INT  iIndex = 2; iIndex < iNumTokens; ++iIndex, ++iChoiceIndex)
      {
      RStr  strChoiceName ("Choice");
      strChoiceName.AppendFormat ("%d", iChoiceIndex);

      RStr  strFullPrefix = strKeyPrefix + strChoiceName;

      astrChoices.Append (strChoiceName);

      // Parse out the stat locked tags from this format
      // # require (Player.Stat.Empathy, 3)
      RStrArray   arrayChoiceText;

      const char *  szChoiceText = NULL;
      szChoiceText = SequenceManager::ParseChoiceTags (pParam->AsString (pRegistryIn),
                                                      strFullPrefix,
                                                      pRegistryIn,
                                                      arrayChoiceText);

      DBG_INFO ("Say command has input string of \"%s\" and result of \"%s\"", pParam->AsString (pRegistryIn), szChoiceText);

      strTextKey = strKeyPrefix + strChoiceName + ".Text";
      pRegistryIn->SetString (strTextKey.AsChar (), szChoiceText);

      pParam = pParam->Next();
      };

    // The CurrChoices registry value will hold a list of the names of the
    //   active choices, i.e ["Choice1", "Choice2"].  This can be acted on by
    //   the TemplaterComponent to create the needed buttons.
    pRegistryIn->SetStringArray ("Script.CurrChoices", astrChoices);
    pRegistryIn->SetInt         ("Script.NumChoices",  astrChoices.Length());
    };

  ptokResultOut->eType = TokenType::kInt;
  ptokResultOut->Set (0);

  if (iNumTokens == 2)
    {
    // Text without choices.  Use "Continue" mechanic to advance.
    if (pSeqStatic->NeedsContinue ())
      {
      // called once already.  React to the "Continue" status
      if (pSeqStatic->CanContinue())
        {
        ptokResultOut->Set (1);
        pSeqStatic->ClearContinue();
        pRegistryIn->SetString ("Script.CurrChar", "");
        pRegistryIn->SetString ("Script.CurrText", "");
        }
      }
    else
      {
      // first time through.  Enable the "Continue" mechanic
      pSeqStatic->SetNeedsContinue (TRUE);
      return;
      };
    }
  else
    {
    // Text with choices.  Use "Choice" mechanic to advance.

    if (pSeqStatic->NeedsChoice ())
      {
      // called once already.  React to the "Choice" status
      if (pSeqStatic->HasMadeChoice())
        {
        ptokResultOut->Set (1);
        pSeqStatic->SetNeedsChoice (FALSE);
        pRegistryIn->SetString ("Script.CurrChar", "");
        pRegistryIn->SetString ("Script.CurrText", "");
        }
      }
    else
      {
      // first time through.  Enable the "Choice" mechanic
      pSeqStatic->SetNeedsChoice (TRUE);
      return;
      };
    };
  };

//------------------------------------------------------------------------------
VOID SequenceManager::RunScriptFn (Token *          ptokParamsIn,
                                   ValueRegistry *  pRegistryIn,
                                   Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2) return;

  const char *  szScriptName = ptokParamsIn->AsString (pRegistryIn);
  const char *  szKnotName   = ptokParamsIn->Next()->AsString (pRegistryIn);

  SequenceManager::Instance()->RunScript (szScriptName, szKnotName);
  };

//------------------------------------------------------------------------------
const char *  SequenceManager::ParseChoiceTags (const char *     pszTextIn,
                                                const RStr &     strKeyPrefixIn,
                                                ValueRegistry *  pRegistryIn,
                                                RStrArray &      arrayChoiceText)
  {
  RStrParser  parserChoiceText (pszTextIn);
  RStr        strKey;

  if (parserChoiceText.FindChar ('#') == -1)
    {
    return (pszTextIn);
    };

  UINT32  uNumStrings = parserChoiceText.Tokenize ('#', arrayChoiceText);

  for (INT  iIndex = 1; iIndex < INT(uNumStrings); ++iIndex)
    {
    RStrParser  parserTag (arrayChoiceText[iIndex]);
    if (parserTag.StartsWith ("require"))
      {
      parserTag.GetWord ();
      if (ParseTools::SkipChar (parserTag, '('))
        {
        RStr  strRequireAttr = parserTag.GetWord ();
        if (ParseTools::SkipChar (parserTag, ','))
          {
          INT  iRequireValue = parserTag.GetInt ();

          if (ParseTools::SkipChar (parserTag, ')'))
            {
            // correctly parsed

            strKey = strKeyPrefixIn + ".Elem";
            pRegistryIn->SetString (strKey.AsChar (), strRequireAttr.AsChar ());

            RStr  strIconKey = strRequireAttr + ".Icon";
            if (pRegistryIn->HasKey (strIconKey.AsChar ()))
              {
              strKey = strKeyPrefixIn + ".Icon";
              pRegistryIn->SetString (strKey.AsChar (),
                                      pRegistryIn->GetString (strIconKey.AsChar ()));
              };

            strKey = strKeyPrefixIn + ".Value";
            pRegistryIn->SetInt (strKey.AsChar (), iRequireValue);

            // TODO: Set evaluation too, so you can activate UI elems.
            // TODO: Store choice ordinal
            };
          };
        };
      };
    };
  return (arrayChoiceText[0].AsChar());
  };

