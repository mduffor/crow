/* -----------------------------------------------------------------
                                 InkScript

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

#include <stdlib.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Script/InkExecute.hpp"
#include "Script/InkParser.hpp"

#define OPT_DBG_INFO(...)      ((void)0)
//#define OPT_DBG_INFO  DBG_INFO


//=============================================================================

//-----------------------------------------------------------------------------
InkExecute::InkExecute ()
  {
  Init ();
  };

//-----------------------------------------------------------------------------
InkExecute::InkExecute (InkScript *  pScriptIn,
                        INT          iStartPosIn)
  {
  Init ();
  iStartPos = iStartPosIn;
  SetScript (pScriptIn);
  };

//-----------------------------------------------------------------------------
InkExecute::~InkExecute ()
  {
  ReleaseScript ();
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::Init  (VOID)
  {
  listLocalFn.PushBack (&fnRouter);
  context.plistLocalFn = &listLocalFn;

  pScript     = NULL;
  iStartPos   = 0;
  pRegistry   = NULL;
  bOwnsScript = FALSE;
  Restart ();
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::Start (INT  iPosIn)
  {
  iStartPos = iPosIn;
  Restart ();
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::StartFunction      (const char *  szFunctionNameIn)
  {
  ASSERT (pScript != NULL);
  Start (pScript->GetFunctionIndex (szFunctionNameIn));
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::StartLabel         (const char *  szLabelNameIn)
  {
  ASSERT (pScript != NULL);
  //DBG_INFO ("StartLabel found %s at %d", szLabelNameIn, pScript->GetLabelIndex (szLabelNameIn));
  Start (pScript->GetLabelIndex (szLabelNameIn));
  };

  /*
//-----------------------------------------------------------------------------
VOID  InkExecute::Start (const char *  szLabelIn)
  {
  if (pScript != NULL)
    {
    iStartPos = iPosIn;
    };
  };
*/
//-----------------------------------------------------------------------------
VOID  InkExecute::Restart (VOID)
  {
  iCurrElem = iStartPos;
  pCurrKnot = NULL;
  iChosen = -1;
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::SetScript  (InkScript *  pScriptIn)
  {
  ReleaseScript ();

  pScript = pScriptIn;
  fnRouter.pScript = pScript;
  Restart ();
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::ReleaseScript (VOID)
  {
  if (bOwnsScript && (pScript != NULL))
    {
    delete pScript;
    bOwnsScript = FALSE;
    };
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::Parse (const char *  szScriptIn)
  {
  InkParser  parser;

  ReleaseScript ();
  SetScript (parser.Parse (szScriptIn));
  bOwnsScript = TRUE;
  Restart ();
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::SetRegistry  (ValueRegistry *  pRegistryIn)
  {
  pRegistry = pRegistryIn;
  InkElem::SetRegistry (pRegistryIn);
  };

//-----------------------------------------------------------------------------
BOOL  InkExecute::HasContent (VOID)
  {
  // True if there is more content, False if not
  if (pScript == NULL)
    {
    DBG_ERROR ("InkExecute::HasContent - pScript is null");
    return (FALSE);
    }
  //DBG_INFO ("IsValidIndex %d ", iCurrElem);//, (pScript->IsValidIndex (iCurrElem) ? "true" : "false"));
  return (pScript->IsValidIndex (iCurrElem));
  };

//-----------------------------------------------------------------------------
BOOL  InkExecute::CanContinue  (VOID)
  {
  // True if there is more content, False if not
  return (HasContent () &&                // in list bounds
          ((GetNumChoices () == 0) || (iChosen != -1)));       // not on a choice
  };

//-----------------------------------------------------------------------------
const char *  InkExecute::Continue  (BOOL  bReturnOnKnot)
  {
  strTextOut.Empty ();
  OPT_DBG_INFO ("Continue start at curr elem %d :%s", iCurrElem, strTextOut.AsChar ());
  BOOL  bAdvanceElem = TRUE;
  BOOL  bReturnAfterElem = FALSE;

  while (CanContinue ())
    {
    InkElem *  pCurr = pScript->GetElem (iCurrElem);

    INT   iStartingGatherLevel = pCurr->GetGatherLevel();

    // step through the chain of (sub) elements.
    while (pCurr != NULL)
      {
      //OPT_DBG_INFO ("Continue() processing elem: %d \"%s\"", iCurrElem, pCurr->GetTypeName())));
      if (pCurr->IsType (InkElemType::kChoice))
        {
        // TODO:  Handle when you reach a choice that is not the top choice.
        //         in that case you need to find the nearest subsequent gather or knot/stitch.
        OPT_DBG_INFO ("Choice");

        if (iChosen != -1)
          {
          OPT_DBG_INFO ("A choice has Chosen %d", iChosen);

          // Go to desired choice and process as text.  Clear choices
          iCurrElem = arrayChoiceIndexes[iChosen];
          pCurr = pScript->GetElem (iCurrElem);
          pCurr->GetText (&context, strTextOut);
          ClearChoices ();
          break;
          }
        else if (GetNumChoices () == 0)
          {
          OPT_DBG_INFO ("Filling out choices");

          // we landed on a choice.  Find and fill out other choices.
          INT  iCurrLevel = dynamic_cast<InkChoice*>(pCurr)->GetChoiceLevel ();
          INT  iNumElem = pScript->GetNumElements ();
          for (INT iIndex = iCurrElem; iIndex < iNumElem; ++iIndex)
            {
            InkElem *  pelemScan = pScript->GetElem (iIndex);
            if ((pelemScan->IsType (InkElemType::kKnot)) ||
                (pelemScan->IsType (InkElemType::kStitch)))
              {
              OPT_DBG_INFO ("Found a knot at index %d", iIndex);

              break;
              };
            if (pelemScan->IsType (InkElemType::kGather))
              {
              //OPT_DBG_INFO ("Found a gather at index %d", iIndex);

              // TODO: Handle Gathers.
              // NOTE: This might be deleteable
              };
            if (pelemScan->IsType (InkElemType::kChoice))
              {
              InkChoice *  pChoice = dynamic_cast<InkChoice*>(pelemScan);
              if (pChoice->GetChoiceLevel () < iCurrLevel) break;

              OPT_DBG_INFO ("Appending Choice from index %d", iIndex);


              arrayChoices.Append (pChoice);
              arrayChoiceIndexes.Append (iIndex);
              };
            };
          // choices are filled out, but we don't have any text to return;
          bAdvanceElem = FALSE;
          break;
          };
        }
      else if (pCurr->IsType (InkElemType::kKnot))
        {
        OPT_DBG_INFO ("Knot");

        // store the current knot for use by divert addressing
        pCurrKnot = dynamic_cast<InkKnot *>(pCurr);
        };

      bReturnAfterElem = pCurr->Eval (&context);

      // if this is a text type node, append the text to display
      if (pCurr->HasText ())
        {
        OPT_DBG_INFO ("HasText");

        // return the curent line of content.
        pCurr->GetText (&context, strTextOut);
        }

      pCurr = pCurr->GetNext ();
      };

    if (bAdvanceElem)
      {
      OPT_DBG_INFO ("Advance Elem");
      // move to the next element
      INT  iNextElem = pScript->GetElem (iCurrElem)->GetNextIndex (iCurrElem, pCurrKnot);

      if (!pScript->IsValidIndex (iNextElem))
        {
        // The next index isn't valid.  Abort further processing.
        iCurrElem = -1;
        }
      else
        {
        // REFACTOR: We're calling GetElem too much, and each call does a bounds check.  Try to cache and reduce.
        //BOOL  bGoToGather = FALSE;
        if (((pScript->GetElem (iCurrElem)->IsType (InkElemType::kChoice)) &&
            (pScript->GetElem (iNextElem)->IsType (InkElemType::kChoice)) &&
            (pScript->GetElem (iCurrElem)->GetGatherLevel () == pScript->GetElem (iNextElem)->GetGatherLevel ())) ||

            ((iNextElem == (iCurrElem + 1)) &&
            (pScript->GetElem (iCurrElem)->GetGatherLevel () >
            pScript->GetElem (iNextElem)->GetGatherLevel ()))

           )
          {
          // We have a choice with no children, or are in a child/indented block and reached the end.
          //   skip to next non-choice line that is a lower indent level than we are at now.

          //bGoToGather = TRUE;
          INT   iCurrLevel = pScript->GetElem (iCurrElem)->GetGatherLevel();

          while (pScript->IsValidIndex (iCurrElem))
            {
            if ((!pScript->GetElem (iCurrElem)->IsType (InkElemType::kChoice)) &&
                (pScript->GetElem(iCurrElem)->GetGatherLevel() < iCurrLevel))
              {
              //OPT_DBG_INFO ("Found higher gather level on line %d which is level %d when we are at level %d", iCurrElem, listElem[iCurrElem]->GetGatherLevel(), iCurrLevel);
              break;
              };


            ++iCurrElem;
            }
          }
        else
          {
          // advance normally
          iCurrElem = iNextElem;
          };
        };
      }; // if advance elem


    OPT_DBG_INFO ("About to exit... curr elem is %d Text out is \"%s\"", iCurrElem, strTextOut.AsChar ());

    BOOL  bNextElemIsChoice = ((iCurrElem != -1) &&
                               (pScript->GetElem (iCurrElem)->IsType (InkElemType::kChoice)) &&
                               (pScript->GetElem (iCurrElem)->GetGatherLevel() == iStartingGatherLevel + 1));

    // if we found text, return it.
    if (!strTextOut.IsEmpty ())
      {

      // if text ends in glue, strip glue and keep composing
      if (strTextOut.EndsWith ("<>"))
        {
        strTextOut.ClipRight (strlen ("<>"));
        }
      else if (!bNextElemIsChoice)
        {
        OPT_DBG_INFO ("Break on text without glue that doesn't have a choice");
        break;
        };
      };

    if (bReturnOnKnot)
      {
      // break on the next knot we encounter.
      if (pScript->IsValidIndex (iCurrElem) && IsKnot ())
        {
        OPT_DBG_INFO ("Returning on knot %d", iCurrElem);
        break;
        };
      };

    if (bReturnAfterElem)
      {
      OPT_DBG_INFO ("Return After Elem");
      iCurrElem = -1;
      break;
      };
    OPT_DBG_INFO ("Looping on while CanContinue");
    };
  OPT_DBG_INFO ("Loop complete");
  // strip any ending newline character.  These generally come from function calls.
  // TODO: May need to deal with this more gracefully.

  strTextOut.StripTrailingChar ('\n');
  strTextOut.TranslateEscapedChars ();

  //OPT_DBG_INFO ("Continue returns at curr elem %d :%s", iCurrElem, strTextOut.AsChar ());
  return (strTextOut.AsChar ());
  };

//-----------------------------------------------------------------------------
INT  InkExecute::GetNumChoices  (VOID)
  {
  // return current number of choices.  0 if not on a choice.
  return (arrayChoices.Length ());
  };

//-----------------------------------------------------------------------------
BOOL  InkExecute::HasChoices (VOID)
  {
  return (GetNumChoices () > 0);
  };

//-----------------------------------------------------------------------------
const char *  InkExecute::GetChoiceText  (INT  iIndexIn,
                                          InkContext *  pContextIn)
  {
  // return the text for a given choice.  0 <= iIndexIn < GetNumChoices ()
  if ((iIndexIn < 0) || (iIndexIn >= GetNumChoices ())) {return "";};
  strTextOut.Empty ();
  arrayChoices[iIndexIn]->GetChoiceText (pContextIn, strTextOut);
  return (strTextOut.AsChar ());
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::ChooseChoiceIndex  (INT  iIndexIn)
  {
  // go to the content of the choice.
  // set iCurrElem to arrayChoiceIndexes[iIndexIn], move past the choice, and process from there.
  iChosen = iIndexIn;
  };

//-----------------------------------------------------------------------------
VOID  InkExecute::ClearChoices (VOID)
  {
  arrayChoices.Clear ();
  arrayChoiceIndexes.Clear ();
  iChosen = -1;
  };




//-----------------------------------------------------------------------------
FnInkScript::FnInkScript ()
  {
  SetName ("");
  pScript = NULL;
  iFnIndex = -1;
  iOffset  = 0;
  };

//-----------------------------------------------------------------------------
BOOL  FnInkScript::CanHandleFunction (RStr &  strNameIn)
  {
  if (pScript == NULL)
    {
    return (FALSE);
    };

  iOffset = -1;
  if (pScript->kvpFunctions.HasKey (strNameIn.AsChar()))
    {
    iOffset = pScript->kvpFunctions[strNameIn.AsChar()];
    };

  return (iOffset != -1);
  };

//-----------------------------------------------------------------------------
VOID  FnInkScript::OnCall  (Token *          ptokParamsIn,
                            ValueRegistry *  pRegistryIn,
                            Token *          ptokResultOut)
  {
  // TODO:  Deal with parameters by feeding them into the registry

  //OPT_DBG_INFO ("FnInkScript::OnCall");

  if (iOffset == -1) {return;};

  InkExecute  exec (pScript, iOffset);
  RStr  strOut;

  while (exec.CanContinue ())
    {
    strOut.AppendString (exec.Continue (TRUE));
    strOut.AppendChar ('\n');
    if (!exec.CanContinue ())
      {
      break;
      }
    if (exec.IsKnot())
      {
      //DBG_INFO ("FnInkScript::OnCall IsKnot");
      break;
      };
    };

  // NOTE: The InkScript spec is unclear here.  If you return a value,
  //  the script prints out that value, as it would a variable substitution.
  //  However the function can also print text normally, which we will accumulate
  //  here in the strOut variable.  Since our expression system is separate
  //  from the script system, rather than one system as Inkle implements it,
  //  we may wind up diverting from their expected behavior.

  // For now, I'm returning the accumulated text as a return value

  // TODO: Parse functions in knot definitions

  // TODO: Implement return keyword in expression language.  When an expression
  //  line is run that ends in a return, the executor needs to pull the
  //  return value of the expression and expand it as the text returned from Continue()

  ptokResultOut->eType = TokenType::kString;
  ptokResultOut->Set (strOut.AsChar (), pRegistryIn);
  //DBG_INFO ("FnInkScript returning %s ", strOut.AsChar ());
  };









