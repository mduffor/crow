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

#ifndef INKSCRIPT_HPP
#define INKSCRIPT_HPP

#include "Sys/Types.hpp"
#include "Containers/TArray.hpp"
#include "Util/RStr.hpp"
#include "Containers/RStrArray.hpp"
#include "Containers/IntArray.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Sys/TKeyValuePair.hpp"
#include "Script/Expression.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------


// NOTE:  The Ink format scripts are parsed into a list of InkElem instances.
//         Each instance can be parsed to generate some text or perform
//         some action.  InkElem instances can be chained together through
//         their pNext pointer.  When an InkElem instance is evaluated,
//         all of the elements in the chain are processed to form the output.
//         Some elements such as lists and conditional choices require
//         an expression to be evaluated, and then one of several InkElem chains
//         to be processed as a result. This is represented by the pChildren
//         pointer to the first option, with subsequent options linked through
//         the pSibling pointer.

//        This creates some unfortunate complexity, but is needed to fully
//         express the grammar.  It should hopefully not be too bad in practice.
//         It should help that the graph does not have to be dynamically altered
//         once it is generated.

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

namespace InkElemType
  {
  enum Type
    {
    kUnknown          = 0,
    kParagraph        = 1,
    kKnot             = 2,
    kStitch           = 3,
    kGather           = 4,
    kChoice           = 5,
    kDivert           = 6,
    kExpression       = 7,
    kConditional      = 8,
    kCode             = 9,
    kList             = 10,
    kVariable         = 11
    };
  };

namespace InkListType
  {
  enum Type
    {
    kUnknown    = 0,
    kSequence   = 1,
    kCycle      = 2,
    kOnce       = 3,
    kShuffle    = 4,
    kExpression = 5
    };
  };

//-----------------------------------------------------------------------------
class InkContext  // Data related to the current line of execution
  {
  public:
    TList<ExpressionFn*> *  plistLocalFn;
  };


//-----------------------------------------------------------------------------
class InkElem
  {
  private:

    InkElemType::Type  eType;
    InkElem *          pChildren;     // ptr to a list of sub elements linked through pSibling
    InkElem *          pSibling;      // links the children of an element together
    InkElem *          pNext;         // forms a chain of elements to be evaluated.
    INT                iGatherLevel;  // This gives the "indentation" of each line, in the context of choices and gathers.

  protected:

    static TKVP<INT> *      pkvpLabels;
    static ValueRegistry *  pRegistry;

  public:

                    InkElem         ();

                    InkElem         (InkElemType::Type  eTypeIn);

    virtual         ~InkElem        ();

    VOID            Init            ();

    BOOL            IsType          (InkElemType::Type  eTypeIn)   {return (eType == eTypeIn);};

    VOID            AddChild        (InkElem *  pelemIn);

    VOID            AppendElem      (InkElem *  pelemIn);

    InkElem *       GetNext         (VOID)                         {return (pNext);};

    InkElem *       GetSibling      (VOID)                         {return (pSibling);};

    InkElem *       GetFirstChild   (VOID)                         {return (pChildren);};

    INT             GetNumChildren  (VOID);

    InkElem *       GetChild        (INT  iIndexIn);

    virtual BOOL    Eval            (InkContext *  pContextIn) = 0;  /// return true if we should return from Continue after this element.

    virtual BOOL    HasText         (VOID)                         {return FALSE;};

    virtual VOID    GetText         (InkContext *  pContextIn,
                                     RStr &        strOut)         {return;};

    VOID            GetAllText      (InkContext *  pContextIn,
                                     RStr &        strOut);

    virtual INT     GetChoiceLevel  (VOID)                         {return (-1);};

    VOID            SetGatherLevel  (INT  iLevelIn)                {iGatherLevel = iLevelIn;};

    INT             GetGatherLevel  (VOID)                         {return (iGatherLevel);};

    virtual VOID    GetChoiceText   (InkContext *  pContextIn,
                                     RStr &        strOut)          {return;};

    virtual INT     GetNextIndex    (INT &        iCurrIndexIn,
                                     InkElem *    pCurrKnotIn = NULL);

    static VOID     SetLabels       (TKVP<INT> *  pkvpLabelsIn)    {pkvpLabels = pkvpLabelsIn;};

    static VOID     SetRegistry     (ValueRegistry * pRegistryIn)  {pRegistry = pRegistryIn;};

    const char *    GetTypeName     (VOID);

  };



//-----------------------------------------------------------------------------
class InkParagraph : public InkElem
  {
  private:
    RStr    strText;

  public:

            InkParagraph  () {};

            InkParagraph  (const char *  szTextIn,
                           INT           iTextLength) : InkElem (InkElemType::kParagraph)  {strText.AppendChars (szTextIn, iTextLength);};

            ~InkParagraph ()                             {};

    BOOL    Eval          (InkContext *  pContextIn)  override   {return (FALSE);};

    BOOL    HasText       (VOID)                      override   {return (TRUE);};

    VOID    GetText       (InkContext *  pContextIn,
                           RStr &       strOut)       override;
  };

//-----------------------------------------------------------------------------
class InkKnot : public InkElem
  {
  private:
    RStr                    strLabel;

  public:

                 InkKnot         () {};

    explicit     InkKnot         (const char *  szLabelIn) : InkElem (InkElemType::kKnot)
                                                         {
                                                         strLabel.SetHash (szLabelIn);
                                                         SetGatherLevel (0);
                                                         };

                 ~InkKnot        ()                      {};

    BOOL         Eval            (InkContext *  pContextIn) override {pRegistry->SetInt (strLabel.AsChar (), pRegistry->GetInt (strLabel.AsChar ()) + 1); return (FALSE);};

    const char * GetLabel        (VOID)                              {return (strLabel.AsChar ());};
  };

//-----------------------------------------------------------------------------
class InkStitch : public InkElem
  {
  private:
    RStr                    strLabel;

  public:

                 InkStitch       () {};

    explicit     InkStitch       (const char *  szLabelIn) : InkElem (InkElemType::kStitch)
                                                         {
                                                         strLabel.SetHash (szLabelIn);
                                                         SetGatherLevel (0);
                                                         };

                 ~InkStitch      ()                      {};

    BOOL         Eval            (InkContext *  pContextIn) override {pRegistry->SetInt (strLabel.AsChar (), pRegistry->GetInt (strLabel.AsChar ()) + 1); return (FALSE);};

    const char * GetLabel        (VOID)                              {return (strLabel.AsChar ());};
  };

//-----------------------------------------------------------------------------
class InkDivert : public InkElem
  {
  private:
    RStr                    strLabel;

  public:

                 InkDivert    () {};

    explicit     InkDivert    (const char *  szLabelIn) : InkElem (InkElemType::kDivert)  {strLabel.SetHash (szLabelIn);};

                 ~InkDivert   ()                         {};

    BOOL         Eval         (InkContext *  pContextIn) override {return (FALSE);};

    const char * GetLabel     (VOID)                              {return (strLabel.AsChar ());};

    INT          GetNextIndex (INT &      iCurrIndexIn,
                               InkElem *  pCurrKnotIn = NULL) override;
  };

//-----------------------------------------------------------------------------
class InkList : public InkElem
  {
  private:
    InkListType::Type     eListType;
    INT                   iCount;

  public:

    explicit InkList  (InkListType::Type  eListTypeIn) : InkElem (InkElemType::kList)  {eListType = eListTypeIn; iCount = -1;};

             ~InkList ()                                  {};

    BOOL     IsListType    (InkListType::Type  eTypeIn)           {return (eListType == eTypeIn);};

    VOID     SetListType   (InkListType::Type  eTypeIn)           {eListType = eTypeIn;};

    BOOL     Eval          (InkContext *  pContextIn)    override {++iCount; return (FALSE);};

    BOOL     HasText       (VOID)                        override {return (TRUE);};

    VOID     GetText       (InkContext *  pContextIn,
                           RStr &         strOut)        override;

    INT      GetNextIndex  (INT &         iCurrIndexIn,
                           InkElem *      pCurrKnotIn = NULL) override;

  };

//-----------------------------------------------------------------------------
class InkExpression : public InkElem
  {
  private:
    TList<Token*> *  pCompiled;
    Token            tokResult;

  public:

                 InkExpression   (const char *  szScriptIn,
                                  INT           iScriptLengthIn) : InkElem (InkElemType::kExpression)
                                                           {pCompiled = Expression::Compile (szScriptIn, iScriptLengthIn);};

                 ~InkExpression  ()                        {Expression::FreeTokenList (&pCompiled);};

    BOOL         Eval            (InkContext *  pContextIn) override;//                    {tokResult.eType = TokenType::kUnknown; Expression::Execute (pCompiled, pRegistry, &tokResult);};

    BOOL         HasText         (VOID)                     override {return (tokResult.eType == TokenType::kString);};

    VOID         GetText         (InkContext *  pContextIn,
                                  RStr &        strOut)     override {if (tokResult.eType == TokenType::kString) strOut.AppendString(tokResult.strRaw);};

    Token *      GetResult       (VOID)                              {return (&tokResult);};
  };

//-----------------------------------------------------------------------------
class InkVariable : public InkElem
  {
  private:
    RStr         strName;

  public:

    explicit     InkVariable     (const char *  szNameIn) : InkElem (InkElemType::kVariable)
                                                           {strName.Set (szNameIn);};

                 ~InkVariable    ()                        {};

    BOOL         Eval            (InkContext *  pContextIn) override {return (FALSE);};

    BOOL         HasText         (VOID)                     override {return TRUE;};

    VOID         GetText         (InkContext *  pContextIn,
                                  RStr &        strOut)     override {if (pRegistry != NULL) {pRegistry->GetString (strName.AsChar (), strOut);};};
  };

//-----------------------------------------------------------------------------
class InkChoice : public InkElem
  {
  private:
    INT               iChoiceLevel;

  public:

  public:

                 InkChoice       () : InkElem (InkElemType::kChoice)
                                                         {
                                                         iChoiceLevel = 0;
                                                         };

                 ~InkChoice      ()                      {};

    BOOL         Eval            (InkContext *  pContextIn) override {return (FALSE);};

    VOID         SetChoiceLevel  (INT  iLevelIn)                     {iChoiceLevel = iLevelIn;};

    INT          GetChoiceLevel  (VOID)                     override {return (iChoiceLevel);};

    VOID         GetChoiceText   (InkContext *  pContextIn,
                                  RStr &        strOut)     override;

    BOOL         HasText         (VOID)                     override;

    VOID         GetText         (InkContext *  pContextIn,
                                  RStr &        strOut)     override;

    INT          GetNextIndex    (INT &        iCurrIndexIn,
                                  InkElem *    pCurrKnotIn = NULL) override;

  };



//-----------------------------------------------------------------------------
class InkScript
  {
  // NOTE: This class stores a single parsed script

  public:
    RStr                    strName;             /// unique name identifying this script
    TArray<InkElem*>        listElem;            /// parsed version of the text script - model
    TKVP<INT>               kvpLabels;           /// named jump targets.  Indexes into listElem. - model
    TKVP<INT>               kvpFunctions;        /// named jump targets.  Indexes into listElem. - model
    RStrParser              parserFullScript;    /// raw script text - parsing
    TList<RStrParser*>      listScriptStack;     /// stack of scripts to parse, for implementing INCLUDE - parsing


  public:

                  InkScript          ();

                  ~InkScript         ();

    VOID          DebugPrint        (VOID);

    InkElem *     GetElem            (INT           iIndexIn) {return (IsValidIndex (iIndexIn) ? listElem[iIndexIn] : NULL);};

    INT           GetNumElements     (VOID)                   {return (listElem.Length ());};

    BOOL          IsValidIndex       (INT  iIndex)            {return ((iIndex >= 0) &&                   // in list bottom bounds
                                                                       (iIndex < listElem.Length ()));};  // in list top bounds
    VOID          FreeElementList    (VOID);

    INT           GetFunctionIndex   (const char *  szFunctionNameIn);

    INT           GetLabelIndex      (const char *  szLabelIn);

    VOID          AddElem            (InkElem *     pNew,
                                      InkElem * *   ppFirst);

  private:


    VOID          Init               (VOID);

  };


#endif // INKSCRIPT_HPP
