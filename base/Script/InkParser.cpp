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

#include "Script/InkParser.hpp"


#define OPT_DBG_INFO(...)      ((void)0)
//#define OPT_DBG_INFO  DBG_INFO


//=============================================================================

//-----------------------------------------------------------------------------
InkParser::InkParser  ()
  {
  bIsBlockExpression = FALSE;
  };

//-----------------------------------------------------------------------------
InkParser::~InkParser  ()
  {
  };

//-----------------------------------------------------------------------------
InkScript *  InkParser::Parse  (const char *  szScriptIn)
  {
  TList<RStrParser*>      listScriptStack;   /// stack of scripts to parse, for implementing INCLUDE - parsing

  InkScript *  pScript = new InkScript ();

  pScript->FreeElementList ();

  pScript->parserFullScript = szScriptIn;
  INT  iGatherLevel = 0;

  RStrParser  parserCurrLine;

  // first element is the entry
  pScript->listElem.Append (new InkKnot ("START"));

  listScriptStack.PushFront (new RStrParser (szScriptIn));

  do
    {
    RStrParser *  pparserCurr = listScriptStack.PopFront ();
    pparserCurr->SetSkipComments (RStrParser::kCStyle);

    while (!pparserCurr->IsEOF ())
      {
      parserCurrLine.Empty ();
      pparserCurr->GetLine (parserCurrLine);
      pparserCurr->SkipComment ();
      parserCurrLine.ResetCursor ();
      parserCurrLine.SkipWhitespace ();  // leading whitespace is only trimmed on top-level lines

      OPT_DBG_INFO("Parse: %s", parserCurrLine.AsChar ());

      if (parserCurrLine.StartsWith ("INCLUDE "))
        {
        // Load a new file as the current buffer, and push the existing buffer onto the stack.
        parserCurrLine.GetWord ();
        parserCurrLine.SkipWhitespace ();
        RStrParser *  pparserNew = new RStrParser;
        if (pparserNew->ReadFromFile (parserCurrLine.GetCursorStartPtr ()) == EStatus::kFailure)
          {
          // TODO Signal error condition
          delete pparserNew;
          DBG_ERROR ("Unable to read file %s", parserCurrLine.GetCursorStartPtr ());
          }
        else
          {
          listScriptStack.PushFront (pparserCurr);
          pparserCurr = pparserNew;
          };
        continue;
        }
      else if (parserCurrLine.StartsWith ("//"))
        {
        // comment line
        continue;
        }
      else if (parserCurrLine.GetLineLength () == 0)
        {
        // empty line
        continue;
        }
      else if (parserCurrLine.StartsWith ("~~~"))
        {
        bIsBlockExpression = !bIsBlockExpression;
        parserCurrLine.SkipChars(strlen ("~~~"));
        };

      InkElem *  pNew = NULL;
      LineToElem (parserCurrLine, pScript, &iGatherLevel, &pNew);
      if (pNew != NULL)
        {
        pScript->listElem.Append (pNew);
        };

      };
    delete (pparserCurr);
    } while (listScriptStack.Size () > 0);

  //OPT_DBG_INFO ("\n\nScript Disassmbly\n");
  //pScript->DebugPrint ();

  return (pScript);
  };

//-----------------------------------------------------------------------------
VOID  InkParser::LineToElem (RStrParser &  parserLineIn,
                             InkScript *   pScriptIn,
                             INT *         piGatherLevelInOut,
                             InkElem **    ppInkElemOut)
  {
  InkElem *         pelemStart = NULL;
  static InkKnot *  pPrevKnot  = NULL;
  INT               iGatherLevel = (piGatherLevelInOut == NULL) ? 0 : *piGatherLevelInOut;
  INT               iNextGatherLevel = iGatherLevel;

  // handle empty case.  Needed for lists with empty elements.
  if ((parserLineIn.Length () == 0) && (!bIsBlockExpression))
    {
    pScriptIn->AddElem (new InkParagraph (NULL, 0), &pelemStart);
    }

  //OPT_DBG_INFO ("----- New Line : %s", parserLineIn.GetCursorStartPtr ());
  while (!parserLineIn.IsEOL ())
    {
    // store line into a new elem.  Parser further as needed.
    // TODO: More in-depth parsing

    // TODO: Break line into components
    OPT_DBG_INFO ("Sub Line : %s", parserLineIn.GetCursorStartPtr ());

    if (bIsBlockExpression || parserLineIn.StartsWith ("~"))
      {
      ParseExpression (parserLineIn, pScriptIn, &pelemStart);
      }
    else if (parserLineIn.StartsWith ("=="))
      {
      ParseKnot (parserLineIn, pScriptIn, &pPrevKnot, &pelemStart);
      iGatherLevel = iNextGatherLevel = 0;
      }
    else if (parserLineIn.StartsWith ("="))
      {
      ParseStitch (parserLineIn, pScriptIn, pPrevKnot, &pelemStart);
      }
    else if (parserLineIn.StartsWith ("->"))
      {
      ParseDivert (parserLineIn, pScriptIn, &pelemStart);
      }
    else if (parserLineIn.StartsWith ("-"))
      {
      ParseGather (parserLineIn, &iGatherLevel, &iNextGatherLevel);
      }
    else if ((parserLineIn.StartsWith ("*")) ||
             (parserLineIn.StartsWith ("+")))
      {
      // TODO: Separate out the + options
      ParseChoice (parserLineIn, pScriptIn, &iGatherLevel, &iNextGatherLevel, &pelemStart);
      }
    else if (parserLineIn.StartsWith ("{"))
      {
      ParseList (parserLineIn, pScriptIn, &pelemStart);
      }
    else if ((parserLineIn.StartsWith ("CONST")) ||
             (parserLineIn.StartsWith ("VAR")))
      {
      ParseVarConst (parserLineIn, pScriptIn, &pelemStart);
      }
    else
      {
      ParseParagraph (parserLineIn, pScriptIn, &pelemStart);
      };
    };

  //DBG_INFO ("---- return from Line");

  if (pelemStart != NULL)
    {
    pelemStart->SetGatherLevel (iGatherLevel);
    iGatherLevel = iNextGatherLevel;
    if (piGatherLevelInOut != NULL)
      {
      *piGatherLevelInOut = iGatherLevel;
      };
    };

  *ppInkElemOut = pelemStart;
  };

//-----------------------------------------------------------------------------
VOID InkParser::ParseKnot (RStrParser &  parserLineIn,
                           InkScript *   pScriptIn,
                           InkKnot * *   ppPrevKnotOut,
                           InkElem * *   ppelemStartOut)
  {

  OPT_DBG_INFO ("+++ Knot +++");
  // knot
  // TODO: handle functions
  while (parserLineIn.PeekChar () == '=')
    {
    parserLineIn.SkipChars (1);
    }
  parserLineIn.SkipWhitespace ();
  RStr  strTag = parserLineIn.GetWord ();
  strTag.StripTrailingChar ('=');

  // determine if this is a knot or a function
  if (strTag == "function")
    {
    strTag = parserLineIn.GetWord ();
    strTag.StripTrailingChar ('=');

    pScriptIn->kvpFunctions [strTag.AsChar()] = pScriptIn->listElem.Length ();

    // NOTE: Remove this function register call once everything works.
    //Expression::RegisterFunction (new FnInkScript (strTag.AsChar (), pScriptIn, pScriptIn->listElem.Length ()));
    // TODO: Read parameters as local variables.  Need to know the name to assign to each variable.
    };

  parserLineIn.GotoEOL ();

  InkKnot *  pNew = new InkKnot (strTag.AsChar ());
  pScriptIn->AddElem (pNew, ppelemStartOut);
  pScriptIn->kvpLabels [strTag.AsChar()] = pScriptIn->listElem.Length (); // Storing what will be its eventual position.

  *ppPrevKnotOut = pNew;
  };

//-----------------------------------------------------------------------------
VOID InkParser::ParseStitch (RStrParser &  parserLineIn,
                             InkScript *   pScriptIn,
                             InkKnot *     pPrevKnotIn,
                             InkElem * *   ppelemStartOut)
  {

  OPT_DBG_INFO ("+++ Stitch +++");
  // stitch
  parserLineIn.SkipChars (1);
  parserLineIn.SkipWhitespace ();
  RStr  strTag = parserLineIn.GetWord ();
  strTag.StripTrailingChar ('=');
  parserLineIn.GotoEOL ();

  RStr  strFullTag;
  if (pPrevKnotIn != NULL)
    {
    strFullTag.Set (pPrevKnotIn->GetLabel ());
    strFullTag.AppendChar ('.');
    };
  strFullTag.AppendString (strTag);

  InkStitch *  pNew = new InkStitch (strFullTag.AsChar ());
  pScriptIn->AddElem (pNew, ppelemStartOut);
  pScriptIn->kvpLabels [strFullTag.AsChar()] = pScriptIn->listElem.Length (); // Storing what will be its eventual position.
  };

//-----------------------------------------------------------------------------
VOID InkParser::ParseGather (RStrParser &  parserLineIn,
                             INT *         piGatherLevel,
                             INT *         piNextGatherLevel)
  {
  OPT_DBG_INFO ("+++ Gather +++");
  // gather
  INT  iGatherDepth = 0;

  while (parserLineIn.PeekChar () == '-')
    {
    parserLineIn.SkipChars (1);
    parserLineIn.SkipWhitespace ();
    ++iGatherDepth;
    };

  *piGatherLevel = (iGatherDepth * 2) - 2;
  *piNextGatherLevel = *piGatherLevel;

  // NOTE:  Gathers don't generate elements, but rather mark elements with a gather depth.
  };

//-----------------------------------------------------------------------------
VOID InkParser::ParseDivert (RStrParser &  parserLineIn,
                             InkScript *   pScriptIn,
                             InkElem * *   ppelemStartOut)
  {
  // divert
  OPT_DBG_INFO ("+++ Divert +++");

  parserLineIn.SkipChars (INT (strlen ("->")));
  parserLineIn.SkipWhitespace ();
  RStr  strTag = parserLineIn.GetWord ();

  pScriptIn->AddElem (new InkDivert (strTag.AsChar ()), ppelemStartOut);
  parserLineIn.GotoEOL ();
  };

//-----------------------------------------------------------------------------
VOID InkParser::ParseList (RStrParser &  parserLineIn,
                           InkScript *   pScriptIn,
                           InkElem * *   ppelemStartOut)
  {

  OPT_DBG_INFO ("+++ List +++");
  parserLineIn.SkipChars (1); // skip open bracket

  INT  iTypeChar = parserLineIn.PeekChar ();

  InkListType::Type  eListType = InkListType::kSequence;

  if (iTypeChar == '&') {eListType = InkListType::kCycle;   parserLineIn.SkipChars (1);}
  if (iTypeChar == '!') {eListType = InkListType::kOnce;    parserLineIn.SkipChars (1);}
  if (iTypeChar == '~') {eListType = InkListType::kShuffle; parserLineIn.SkipChars (1);}

  // TODO: Handle Expressions { exp : itemA | itemB }

  // list
  InkElem *  pNew = new InkList (eListType);

  // separate out the top-level list elements, knowing there may be nested elements
  INT  iListElemStart = parserLineIn.GetCursorStart ();
  INT  iLineLength = parserLineIn.Length ();
  INT  iTextLength = 0;
  INT  iElemLength = 0;

  INT  iDepth = 0;
  while (iTextLength < iLineLength)
    {
    INT  iChar = parserLineIn.GetChar (FALSE);
    ++iTextLength;
    ++iElemLength;
    if      (iChar == '{')   {++iDepth;}
    else if (iChar == ':')
      {
      // expression
      if ((! dynamic_cast<InkList*>(pNew)->IsListType (InkListType::kSequence)) || (pNew->GetFirstChild () != NULL))
        {
        // Syntax error.  We've processed more than expected
        DBG_ERROR ("InkScript::LineToElem () - List Syntax Error");
        return;
        }

      RStrParser  parserExpression = parserLineIn.SubString (iListElemStart, iElemLength - 1);
      //DBG_INFO ("Expression : %s", parserExpression.AsChar ());

      iElemLength = 0;
      iListElemStart = parserLineIn.GetCursorStart ();

      pNew->AddChild (new InkExpression (parserExpression.AsChar (), parserExpression.Length ()));
      dynamic_cast<InkList*>(pNew)->SetListType (InkListType::kExpression);
      }
    else if ((iChar == '}') || (iChar == '|')) // list elem enders
      {
      if (iDepth == 0)
        {
        // extract the list element and parse it as its own entity.
        RStrParser  parserListElem = parserLineIn.SubString (iListElemStart, iElemLength - 1);
        //DBG_INFO ("List Part : %s", parserListElem.AsChar ());
        InkElem *   pListElemNew = NULL;
        LineToElem (parserListElem, pScriptIn, 0, &pListElemNew);
        iElemLength = 0;
        iListElemStart = parserLineIn.GetCursorStart ();

        //DBG_INFO ("Returned from list part.  depth is %d, remaining is %s", iDepth, parserLineIn.GetCursorStartPtr ());

        pNew->AddChild (pListElemNew);
        };
      if (iChar == '}')   {--iDepth;};
      };
    if (iDepth < 0) break;
    }
  // determine if we have a variable expansion instead of a list.
  if ((pNew->GetNumChildren () == 1) && (pNew->GetFirstChild()->IsType (InkElemType::kParagraph)))
    {
    // we have a single-child. If the paragraph is one word, treat it as a variable expansion
    RStrParser  parserParagraph;
    RStrParser  parserName;

    pNew->GetFirstChild()->GetText (NULL, parserParagraph);
    parserParagraph.ResetCursor ();
    parserParagraph.SkipWhitespace ();
    parserParagraph.GetWord (parserName);
    parserParagraph.SkipWhitespace ();
    if (parserParagraph.IsEOL ())
      {
      // only one word
      delete (pNew);
      pNew = new InkVariable (parserName.AsChar ());
      }
    else if (parserParagraph.StartsWith ("(") && (parserParagraph.FindChar (')') != -1))
      {
      // probably an inline function.  This determination could be better.
      // TODO: Handle calling inline functions
      delete (pNew);
      pNew = new InkExpression (parserParagraph.AsChar (0), parserParagraph.Length ());

      //DBG_INFO ("-=-=-=-=-= Inline Function -=-=-=--= %s", parserParagraph.AsChar (0));
      };


    };

  pScriptIn->AddElem (pNew, ppelemStartOut);
  };

//-----------------------------------------------------------------------------
VOID InkParser::ParseChoice (RStrParser &  parserLineIn,
                             InkScript *   pScriptIn,
                             INT *         piGatherLevel,
                             INT *         piNextGatherLevel,
                             InkElem * *   ppelemStartOut)
  {
  // TODO: Handle choices that start with + (sticky choice)

  INT  iChoiceLevel = 0;

  while (parserLineIn.PeekChar () == '*')
    {
    parserLineIn.SkipChars (1); // skip * or +
    parserLineIn.SkipWhitespace ();
    ++iChoiceLevel;
    };

  *piGatherLevel = (iChoiceLevel * 2) - 1;
  *piNextGatherLevel = *piGatherLevel + 1;

  // Choices are stored in three or four pieces.
  //    Expression : For conditional choices.  Multiple expressions can be chained.
  //    Header : paragraph giving the text before the square brackets.  Used for both GetText and GetChoiceText
  //    Choice Text : Suffix for GetChoiceText.  In square brackets
  //    Select Text : Text and processing used for GetText, when the choice is made.
  //
  //  The Expression part is optional (and its presence checked for by looking for an InkExpression)
  //  but the other three always appear in-order as children of the Choice node.

  // When evaluating, if there are no valid options, the default option is used (starts with empty square brackets)
  // or the choice is skipped entirely.  Calling ChooseChoiceIndex () sets up GetText to evaluate
  // the select text of the chosen option, after which the choices are cleared before returning from GetText.

  InkChoice * pNewChoice = new InkChoice ();

  if (parserLineIn.PeekChar () == '{')
    {
    // expression
    parserLineIn.SkipChars (1);

    INT  iExpressionStart = parserLineIn.GetCursorStart ();
    INT  iExpressionLength = 0;
    while (!parserLineIn.IsEOF ())
      {
      if (parserLineIn.PeekChar () == '}')
        {
        pNewChoice->AddChild (new InkExpression (parserLineIn.GetBufferPtr (iExpressionStart), iExpressionLength));
        parserLineIn.SkipChars (1);
        parserLineIn.SkipWhitespace ();
        break;
        };
      ++iExpressionLength;
      }
    }

  // find any square brackets
  INT  iLineStart = parserLineIn.GetCursorStart ();
  INT  iLineEnd   = parserLineIn.GetLineEnd ();
  INT  iSquareOpen  = parserLineIn.FindChar ('[', parserLineIn.GetCursorStart ());
  INT  iSquareClose = parserLineIn.FindChar (']', parserLineIn.GetCursorStart ());

  RStrParser  parserHeader;
  RStrParser  parserChoice;
  RStrParser  parserSelect;

  if ((iSquareOpen  == -1) || (iSquareOpen  > iLineEnd)  ||
      (iSquareClose == -1) || (iSquareClose > iLineEnd))
    {
    // No square brackets

    parserHeader = parserLineIn.SubString (iLineStart, iLineEnd - iLineStart);

    // TODO: Error Handling
    //DBG_ERROR ("InkScript::ParseChoice - square brackets need to be on same line as choice");
    //parserLineIn.GotoEOL ();
    //return;
    }
  else
    {
    // square brackets
    parserHeader = parserLineIn.SubString (iLineStart, iSquareOpen - iLineStart);
    parserChoice = parserLineIn.SubString (iSquareOpen + 1, iSquareClose - iSquareOpen - 1);
    parserSelect = parserLineIn.SubString (iSquareClose + 1, iLineEnd - iSquareClose - 1);
    };

  parserLineIn.SetCursorStart (iLineEnd);

  //DBG_INFO ("Choice Header : %s", parserHeader.AsChar ());
  //DBG_INFO ("Choice Choice : %s", parserChoice.AsChar ());
  //DBG_INFO ("Choice Select : %s", parserSelect.AsChar ());

  InkElem *   pListElemNew = NULL;
  LineToElem (parserHeader, pScriptIn, NULL, &pListElemNew);
  if (pListElemNew != NULL)
    {
    pNewChoice->AddChild (pListElemNew);
    pListElemNew = NULL;
    }
  else
    {
    pNewChoice->AddChild (new InkParagraph ("", 0));
    }
  LineToElem (parserChoice, pScriptIn, NULL, &pListElemNew);
  if (pListElemNew != NULL)
    {
    pNewChoice->AddChild (pListElemNew);
    pListElemNew = NULL;
    }
  else
    {
    pNewChoice->AddChild (new InkParagraph ("", 0));
    }
  LineToElem (parserSelect, pScriptIn, NULL, &pListElemNew);
  if (pListElemNew != NULL)
    {
    pNewChoice->AddChild (pListElemNew);
    }
  else
    {
    pNewChoice->AddChild (new InkParagraph ("", 0));
    }

  pScriptIn->AddElem (pNewChoice, ppelemStartOut);
  };

//-----------------------------------------------------------------------------
VOID InkParser::ParseExpression (RStrParser &  parserLineIn,
                                 InkScript *   pScriptIn,
                                 InkElem * *   ppelemStartOut)
  {
  // expressions
  OPT_DBG_INFO ("+++ Expression +++");

  if (parserLineIn.StartsWith ("~"))
    {
    parserLineIn.SkipChars (1); // skip open bracket
    parserLineIn.SkipWhitespace ();
    };

  INT  iStart = parserLineIn.GetCursorStart ();
  INT  iLength = parserLineIn.GetLineLength ();

  pScriptIn->AddElem (new InkExpression (parserLineIn.GetBufferPtr (iStart), iLength), ppelemStartOut);
  parserLineIn.GotoEOL ();
  }

//-----------------------------------------------------------------------------
VOID InkParser::ParseVarConst (RStrParser &  parserLineIn,
                               InkScript *   pScriptIn,
                               InkElem * *   ppelemStartOut)
  {
  OPT_DBG_INFO ("+++ VAR / CONST +++");

  // these should be simple assignments, but we will treat them the same as expressions.
  // TODO: Narrow functionality more

  parserLineIn.GetWord (FALSE);
  parserLineIn.SkipWhitespace (FALSE);

  INT  iStart = parserLineIn.GetCursorStart ();
  INT  iLength = parserLineIn.GetLineLength ();

  parserLineIn.GetWord (FALSE); // RStr  strVarName
  parserLineIn.SkipWhitespace (FALSE);
  if (parserLineIn.PeekChar () != '=')
    {
    DBG_ERROR ("Syntax error with CONST/VAR : %s", parserLineIn.AsChar (iStart));
    return;
    };
  parserLineIn.SkipChars (1);
  if (parserLineIn.PeekChar () == '=')
    {
    DBG_ERROR ("Syntax error with CONST/VAR : %s", parserLineIn.AsChar (iStart));
    return;
    };

  pScriptIn->AddElem (new InkExpression (parserLineIn.GetBufferPtr (iStart), iLength), ppelemStartOut);
  parserLineIn.GotoEOL ();
  };

//-----------------------------------------------------------------------------
VOID InkParser::ParseParagraph (RStrParser &  parserLineIn,
                                InkScript *   pScriptIn,
                                InkElem * *   ppelemStartOut)
  {

  // paragraph
  OPT_DBG_INFO ("+++ Paragraph +++");

  // Scan the line for printable text.  Printable text ends
  //  when you find a square bracket (part of a choice), divert,
  //  glue, curly bracket (conditional or list), or parenthesis (label)

  INT  iLineLength = parserLineIn.Length ();
  INT  iTextLength = 0;
  INT  iTextStart = parserLineIn.GetCursorStart ();

  while (iTextLength < iLineLength)
    {
    INT  iChar = parserLineIn.GetChar (FALSE);
    INT  iCharNext = parserLineIn.PeekChar ();

    if (iChar == '[')
      {
      // choice text
      }
    else if (iChar == ']')
      {
      // end choice text
      }
    else if (iChar == '(')
      {
      // label text
      // TODO: Ignore double-parenthesis since we will use those for stage commands
      // TODO: Implement lables
      }
    else if (iChar == ')')
      {
      // end label text
      }
    else if ((iChar == '<') && (iCharNext == '>'))
      {
      // glue: Copy characters to text for later use.
      iTextLength += strlen ("<>");
      }
    else if (((iChar == '-') && (iCharNext == '>')) ||   // divert
              ((iChar == '{') || (iChar == '}')))         // list
      {
      break;
      }
    else
      {
      // printable character
      ++iTextLength;
      };
    };
  parserLineIn.SetCursorStart (iTextStart);
  if (iLineLength > 0)
    {
    pScriptIn->AddElem (new InkParagraph (parserLineIn.GetCursorStartPtr (), iTextLength), ppelemStartOut);
    }
  parserLineIn.SkipChars (iTextLength);
  };












