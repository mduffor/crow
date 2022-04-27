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

#ifndef INKEXECUTE_HPP
#define INKEXECUTE_HPP

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
class FnInkScript : public ExpressionFn
  {
  public:
    InkScript *             pScript;    /// NOTE:  This may work better as a string ID instead of a pointer
    INT                     iFnIndex;   /// Index of last funciton queried via CanHandleFunction()
    INT                     iOffset;

  public:

                  FnInkScript  ();

    virtual       ~FnInkScript () {};

    virtual BOOL  CanHandleFunction (RStr &  strNameIn) override;

    virtual VOID  OnCall       (Token *          ptokParamsIn,
                                ValueRegistry *  pRegistryIn,
                                Token *          ptokResultOut) override;
  };

//-----------------------------------------------------------------------------
class InkExecute
  {
  public:
    ValueRegistry *         pRegistry;           /// registry for holding variables and knot/stitch/gather counts - execute
    INT                     iCurrElem;           /// index of current execution point - execute
    RStr                    strTextOut;          /// temp working buffer so const char * can be returned - execute
    InkKnot *               pCurrKnot;           /// ptr to the last knot visited.  Used by diverts - execute
    TArray<InkElem*>        arrayChoices;        /// list of current choices - execute
    IntArray                arrayChoiceIndexes;  /// indexes of current choices - execute
    INT                     iChosen;             /// The choice that was chosen and which should drive the next branching decision - execute

    InkScript *             pScript;             /// The script we are executing
    BOOL                    bOwnsScript;
    INT                     iStartPos;           /// When reset, this is the offset in the script we start at.

    FnInkScript             fnRouter;
    TList<ExpressionFn*>    listLocalFn;
    InkContext              context;

  public:

                  InkExecute         ();

    explicit      InkExecute         (InkScript *  pScriptIn,
                                      INT          iStartPosIn = 0);

                  ~InkExecute        ();

    VOID          Init               (VOID);

    VOID          Start              (INT  iPosIn);

    VOID          StartFunction      (const char *  szFunctionNameIn);

    VOID          StartLabel         (const char *  szLabelNameIn);

    //VOID          Start              (const char *  szLabelIn)                {iStartPos = iPosIn;};

    VOID          Restart            (VOID);

    VOID          SetScript          (InkScript *  pScriptIn);

    InkScript *   GetScript          (VOID)                           {return (pScript);};

    VOID          Parse              (const char *  szScriptIn);

    VOID          ReleaseScript      (VOID);

    VOID          SetRegistry        (ValueRegistry *  pRegistryIn);

    BOOL          HasContent         (VOID); // True if there is more content, False if not

    BOOL          CanContinue        (VOID); // True if we can display another line of text.

    const char *  Continue           (BOOL  bReturnOnKnot = FALSE); // return the curent line of content.

    INT           GetNumChoices      (VOID); // return current number of choices.  0 if not on a choice.

    BOOL          HasChoices         (VOID);

    const char *  GetChoiceText      (INT           iIndexIn,
                                      InkContext *  pContextIn = NULL); // return the text for a given choice.  0 <= iIndex < GetNumChioces ()

    VOID          ChooseChoiceIndex  (INT  iIndexIn); // go to the content of the choice.

    VOID          ClearChoices       (VOID);

    BOOL          IsKnot             (VOID)         {return (pScript->GetElem (iCurrElem)->IsType (InkElemType::kKnot));};


  };







#endif // INKEXECUTE_HPP
