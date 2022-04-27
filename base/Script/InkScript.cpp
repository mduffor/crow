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

#include "Script/InkScript.hpp"


#define DBG_INKSCRIPT 0
//#define DBG_INKSCRIPT 1

TKVP<INT> *      InkElem::pkvpLabels = NULL;
ValueRegistry *  InkElem::pRegistry = NULL;

//=============================================================================

//-----------------------------------------------------------------------------
InkElem::InkElem  ()
  {
  Init ();
  };

//-----------------------------------------------------------------------------
InkElem::InkElem  (InkElemType::Type  typeIn)
  {
  Init ();
  eType = typeIn;
  };

//-----------------------------------------------------------------------------
InkElem::~InkElem  ()
  {
  InkElem *  pToDelete;

  // delete children
  while (pChildren != NULL)
    {
    pToDelete = pChildren;
    pChildren = pChildren->pSibling;
    delete (pToDelete);
    };

  // delete followers
  while (pNext != NULL)
    {
    pToDelete = pNext;
    pNext = pToDelete->pNext;
    pToDelete->pNext = NULL;
    delete (pToDelete);
    }
  };

//-----------------------------------------------------------------------------
VOID  InkElem::Init (VOID)
  {
  eType        = InkElemType::kUnknown;
  pChildren    = NULL;
  pSibling     = NULL;
  pNext        = NULL;
  iGatherLevel = -1;
  };

//-----------------------------------------------------------------------------
VOID  InkElem::AddChild  (InkElem *  pelemIn)
  {
  /// Add to end of pChildren->pSibling list

  ASSERT (pelemIn->pSibling == NULL);
  if (pChildren == NULL)
    {
    pChildren = pelemIn;
    }
  else
    {
    InkElem *  pLast = pChildren;
    while (pLast->pSibling != NULL)
      {
      pLast = pLast->pSibling;
      };
    pLast->pSibling = pelemIn;
    };
  };

//-----------------------------------------------------------------------------
VOID  InkElem::AppendElem  (InkElem *  pelemIn)
  {
  /// Add to end of pNext list
  InkElem *  pLast = this;
  while (pLast->pNext != NULL)
    {
    pLast = pLast->pNext;
    };
  pLast->pNext = pelemIn;
  };

//-----------------------------------------------------------------------------
INT  InkElem::GetNumChildren  (VOID)
  {
  INT  iCount = 0;

  InkElem *  pCurr = pChildren;
  while (pCurr != NULL)
    {
    ++iCount;
    pCurr = pCurr->pSibling;
    };
  return (iCount);
  };

//-----------------------------------------------------------------------------
InkElem *  InkElem::GetChild  (INT  iIndexIn)
  {
  INT  iCount = 0;

  InkElem *  pCurr = pChildren;
  while (pCurr != NULL)
    {
    if (iCount == iIndexIn) return (pCurr);
    ++iCount;
    pCurr = pCurr->pSibling;
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  InkElem::GetAllText  (InkContext *  pContextIn,
                            RStr &        strOut)
  {
  InkElem *  pCurr = this;

  // step through the chain of elements and get all text from the chain.
  while (pCurr != NULL)
    {
    pCurr->Eval (pContextIn);

    // if this is a text type node, append the text to display
    if (pCurr->HasText ())
      {
      // return the curent line of content.
      pCurr->GetText (pContextIn, strOut);
      }
    pCurr = pCurr->GetNext ();
    };
  };

//-----------------------------------------------------------------------------
INT InkElem::GetNextIndex (INT &        iCurrIndexIn,
                           InkElem *    pCurrKnotIn)
  {
  return (pNext == NULL ? iCurrIndexIn + 1 : pNext->GetNextIndex(iCurrIndexIn, pCurrKnotIn));
  };

//-----------------------------------------------------------------------------
const char *  InkElem::GetTypeName (VOID)
  {
  switch (eType)
    {
    case InkElemType::kParagraph:     return ("Paragraph");
    case InkElemType::kKnot:          return ("Knot");
    case InkElemType::kStitch:        return ("Stitch");
    case InkElemType::kGather:        return ("Gather");
    case InkElemType::kChoice:        return ("Choice");
    case InkElemType::kDivert:        return ("Divert");
    case InkElemType::kExpression:    return ("Expression");
    case InkElemType::kConditional:   return ("Conditional");
    case InkElemType::kCode:          return ("Code");
    case InkElemType::kList:          return ("List");
    case InkElemType::kVariable:
    default:                          return ("Unknown");
    };
  };


//=============================================================================

//-----------------------------------------------------------------------------
VOID  InkParagraph::GetText  (InkContext *  pContextIn,
                              RStr &        strOut)
  {
  //printf ("InkParagraph::GetText() %s\n", strText.AsChar ());
  strOut.AppendString (strText);
  };



//=============================================================================

//-----------------------------------------------------------------------------
VOID  InkList::GetText  (InkContext *  pContextIn,
                         RStr &        strOut)
  {
  //printf ("InkParagraph::GetText() %s\n", strText.AsChar ());

  INT  iNumEntries = GetNumChildren ();

  switch (eListType)
    {
    case InkListType::kSequence:
      {
      iCount = (iCount >= iNumEntries) ? iNumEntries - 1 : iCount;
      };
      break;
    case InkListType::kCycle:
      {
      iCount = (iCount >= iNumEntries) ? 0 : iCount;
      };
      break;
    case InkListType::kOnce:
      {
      iCount = (iCount >= iNumEntries) ? iNumEntries : iCount;
      };
      break;
    case InkListType::kShuffle:
      {
      // TODO: Replace with Rand class
      iCount = rand () % iNumEntries;
      };
      break;
    case InkListType::kExpression:
      {
      iCount = 2;
      InkElem *  pElem = GetFirstChild ();
      if (pElem->IsType (InkElemType::kExpression))
        {
        InkExpression *  pExp = dynamic_cast<InkExpression *>(pElem);
        pExp->Eval (pContextIn);
        Token *  ptokResult = pExp->GetResult ();

        if (ptokResult->AsInt (pRegistry) > 0)
          {
          iCount = 1;
          }
        };
      };
      break;
    default:
      return;
    }

  InkElem *  pChild = GetChild (iCount);

  if (pChild != NULL)
    {
    pChild->Eval (pContextIn);
    pChild->GetText (pContextIn, strOut);
    };
  };

//-----------------------------------------------------------------------------
INT  InkList::GetNextIndex  (INT &        iCurrIndexIn,
                             InkElem *    pCurrKnotIn)
  {
  InkElem *  pChild = GetChild (iCount);
  if (pChild == NULL)
    {
    return iCurrIndexIn + 1;
    }
  else
    {
    return (pChild->GetNextIndex (iCurrIndexIn, pCurrKnotIn));
    };
  };

//=============================================================================
BOOL  InkExpression::Eval (InkContext *  pContextIn)
  {
  tokResult.eType = TokenType::kUnknown;
  BOOL  bReturned;

  //DBG_INFO ("InkExpression::Eval");
  //Expression::DebugTokenList(pCompiled);

  TList<ExpressionFn*> *  plistLocalFn = (pContextIn == NULL) ? NULL : pContextIn->plistLocalFn;

  Expression::Execute (pCompiled, pRegistry, &tokResult, &bReturned, NULL, plistLocalFn);

  //DBG_INFO ("InkExpression::Eval Done.  Returned token type : %s", tokResult.GetTypeString());
  return (bReturned);
  };

//=============================================================================

//-----------------------------------------------------------------------------
INT  InkDivert::GetNextIndex (INT &      iCurrIndexIn,
                              InkElem *  pCurrKnotIn)
  {
  if (strLabel.Equals ("END")) return (-1);

  if (pkvpLabels->HasKey (strLabel.AsChar()))
    {
    // knot
    return ((*pkvpLabels) [strLabel.AsChar()]);
    }
  else if (pCurrKnotIn != NULL)
    {
  // divert
    RStr  strFullTag (dynamic_cast<InkKnot*>(pCurrKnotIn)->GetLabel ());
    strFullTag.AppendChar ('.');
    strFullTag.AppendString (strLabel);

    if (pkvpLabels->HasKey (strFullTag.AsChar()))
      {
      // knot
      return ((*pkvpLabels) [strFullTag.AsChar()]);
      };
    };
  // couldn't find it!  Error!
  DBG_ERROR ("InkDivert: Unable to find label %s", strLabel.AsChar());
  return (InkElem::GetNextIndex (iCurrIndexIn, pCurrKnotIn));
  };




//=============================================================================

//-----------------------------------------------------------------------------
VOID  InkChoice::GetChoiceText  (InkContext *  pContextIn,
                                 RStr &        strOut)
  {
  if (GetFirstChild () == NULL) return;

  InkElem *  pHeader = NULL;
  InkElem *  pChoice = NULL;
  if (GetFirstChild ()->IsType (InkElemType::kExpression))
    {
    pHeader = GetFirstChild ()->GetSibling ();
    pChoice = GetFirstChild ()->GetSibling ()->GetSibling ();
    }
  else
    {
    pHeader = GetFirstChild ();
    pChoice = GetFirstChild ()->GetSibling ();
    };
  if ((pHeader != NULL) && (pHeader->HasText ()))
    {
    pHeader->GetAllText (pContextIn, strOut);
    };
  if ((pChoice != NULL) && (pChoice->HasText ()))
    {
    pChoice->GetAllText (pContextIn, strOut);
    };
  };

//-----------------------------------------------------------------------------
BOOL  InkChoice::HasText (VOID)
  {
  return (FALSE);
  };

//-----------------------------------------------------------------------------
VOID  InkChoice::GetText (InkContext *  pContextIn,
                          RStr &        strOut)
  {
  if (GetFirstChild () == NULL) return;

  InkElem *  pHeader = NULL;
  InkElem *  pSelect = NULL;
  if (GetFirstChild ()->IsType (InkElemType::kExpression))
    {
    pHeader = GetFirstChild ()->GetSibling ();
    pSelect = GetFirstChild ()->GetSibling ()->GetSibling ()->GetSibling ();
    }
  else
    {
    pHeader = GetFirstChild ();
    pSelect = GetFirstChild ()->GetSibling ()->GetSibling ();
    };
  if ((pHeader != NULL) && (pHeader->HasText ()))
    {
    pHeader->GetAllText (pContextIn, strOut);
    };
  if ((pSelect != NULL) && (pSelect->HasText ()))
    {
    pSelect->GetAllText (pContextIn, strOut);
    };
  };

//-----------------------------------------------------------------------------
INT InkChoice::GetNextIndex (INT &        iCurrIndexIn,
                             InkElem *    pCurrKnotIn)
  {
  // Where possible, we want to follow the chosen text path to see if there is
  //  a divert at the end.  Otherwise, we return the next index, as normal.
  if (GetFirstChild () == NULL) return (GetNext() == NULL ? iCurrIndexIn + 1 : GetNext()->GetNextIndex(iCurrIndexIn, pCurrKnotIn));

  InkElem *  pSelect = NULL;
  if (GetFirstChild ()->IsType (InkElemType::kExpression))
    {
    pSelect = GetFirstChild ()->GetSibling ()->GetSibling ()->GetSibling ();
    }
  else
    {
    pSelect = GetFirstChild ()->GetSibling ()->GetSibling ();
    };
  if (pSelect != NULL)
    {
    return (pSelect->GetNext() == NULL ? iCurrIndexIn + 1 : pSelect->GetNext()->GetNextIndex(iCurrIndexIn, pCurrKnotIn));
    };
  return (iCurrIndexIn + 1);
  };


//=============================================================================

//-----------------------------------------------------------------------------
InkScript::InkScript  ()
  {
  Init ();
  InkElem::SetLabels (&kvpLabels);
  };

//-----------------------------------------------------------------------------
InkScript::~InkScript  ()
  {

  };

//-----------------------------------------------------------------------------
VOID  InkScript::Init  (VOID)
  {
  parserFullScript.Empty ();
  };

//-----------------------------------------------------------------------------
INT  InkScript::GetFunctionIndex   (const char *  szFunctionNameIn)
  {
  if (!kvpFunctions.HasKey(szFunctionNameIn)) return (-1);
  return (kvpFunctions.Find (szFunctionNameIn));
  };

//-----------------------------------------------------------------------------
INT  InkScript::GetLabelIndex   (const char *  szLabelIn)
  {
  if (szLabelIn == NULL) return (0);  // if null, start at the first element
  if (!kvpLabels.HasKey(szLabelIn)) return (-1);
  return (kvpLabels.Find (szLabelIn));
  };

//-----------------------------------------------------------------------------
VOID  InkScript::AddElem (InkElem *    pNew,
                          InkElem * *  ppFirst)
  {
  if (*ppFirst == NULL)
    {
    *ppFirst = pNew;
    }
  else
    {
    (*ppFirst)->AppendElem (pNew);
    };
  };

//-----------------------------------------------------------------------------
VOID  InkScript::FreeElementList (VOID)
  {
  INT  iNumElements = listElem.Length ();
  for (INT  iIndex = 0; iIndex < iNumElements; ++iIndex)
    {
    delete (listElem [iIndex]);  // parsed version of the text script
    };
  listElem.SetLength (0); // the actual array will be freed in the destructor.
  }

//-----------------------------------------------------------------------------
VOID  InkScript::DebugPrint (VOID)
  {
  INT  iNumElements = listElem.Length ();
  for (INT  iIndex = 0; iIndex < iNumElements; ++iIndex)
    {
    const char *  szLabel = "";

    if (listElem[iIndex]->IsType (InkElemType::kKnot))    {szLabel = dynamic_cast<InkKnot*>(listElem[iIndex])->GetLabel();};
    if (listElem[iIndex]->IsType (InkElemType::kStitch))  {szLabel = dynamic_cast<InkStitch*>(listElem[iIndex])->GetLabel();};
    if (listElem[iIndex]->IsType (InkElemType::kDivert))  {szLabel = dynamic_cast<InkDivert*>(listElem[iIndex])->GetLabel();};

    DBG_INFO ("  [%3d] %10s %15s Gather: %d", iIndex, listElem[iIndex]->GetTypeName (), szLabel, listElem[iIndex]->GetGatherLevel ());
    };
  };

  /*
//-----------------------------------------------------------------------------
INT  InkScript::GetLabelStart      (const char *  szLabelIn);

    TKVP<INT>               kvpLabels;           /// named jump targets.  Indexes into listElem. - model



      return ((*pkvpLabels) [strFullTag.AsChar()]);

*/












