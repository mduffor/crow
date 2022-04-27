/* -----------------------------------------------------------------
                              Expression

     This module implements an expression parser and state machine.

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
#include "Sys/Types.hpp"


#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Util/RStrParser.hpp"
#include "Script/Expression.hpp"
#include "Util/RegEx.hpp"

// NOTE:  Based off of http://en.wikipedia.org/wiki/Shunting-yard_algorithm

#ifdef MOVED
//-----------------------------------------------------------------------------
// Token
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
Token::Token  (const char *   szTextIn,
               INT            iTextLengthIn)
  {
  static RegEx  rexFloat      (REX_FLOAT);
  static RegEx  rexInt        (REX_INT);
  static RegEx  rexIdentifier (REX_IDENTIFIER);
  static RegEx  rexTrue       (REX_TRUE);
  static RegEx  rexFalse      (REX_FALSE);
  static RegEx  rexDoubleString (REX_DOUBLE_STRING);
  static RegEx  rexSingleString (REX_SINGLE_STRING);



  // Operator Predecence (higher numbers handled first)
  // 0: ( ) ;
  // 1: , = += -= *= /=
  // 2: && ||
  // 3: == !=
  // 4: <= >= < >
  // 5: + -
  // 6: * /
  // 7: ++ --

  Init ();

  strRaw.AppendChars (szTextIn, iTextLengthIn);

  // REFACTOR:  You can reduce the number of comparisons below by
  //  checking iTextLengthIn once per operator group/type, and by avoiding
  //  strncmp for single-character items.

  // Mathematic
  if ((iTextLengthIn == 2) && (strncmp (szTextIn, "++", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpInc;
    bIsOperator    = TRUE;
    bIsUnary       = TRUE;
    iPrecedence    = 7;
    eAssociativity = AssociativityType::kRight;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "--", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpDec;
    bIsOperator    = TRUE;
    bIsUnary       = TRUE;
    iPrecedence    = 7;
    eAssociativity = AssociativityType::kRight;
    }
  else if (strncmp (szTextIn, "+", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpPlus;
    bIsOperator    = TRUE;
    iPrecedence    = 5;
    eAssociativity = AssociativityType::kLeft;
    }
  else if (strncmp (szTextIn, "-", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpMinus;
    bIsOperator    = TRUE;
    iPrecedence    = 5;
    eAssociativity = AssociativityType::kLeft;
    }
  else if (strncmp (szTextIn, "*", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpMultiply;
    bIsOperator    = TRUE;
    iPrecedence    = 6;
    eAssociativity = AssociativityType::kLeft;
    }
  else if (strncmp (szTextIn, "/", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpDivide;
    bIsOperator    = TRUE;
    iPrecedence    = 6;
    eAssociativity = AssociativityType::kLeft;
    }
  else if (strncmp (szTextIn, "%", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpMod;
    bIsOperator    = TRUE;
    iPrecedence    = 6;
    eAssociativity = AssociativityType::kLeft;
    }

  // Grouping and Parameters
  else if (strncmp (szTextIn, "(", iTextLengthIn) == 0)
    {
    eType          = TokenType::kParenL;
    }
  else if (strncmp (szTextIn, ")", iTextLengthIn) == 0)
    {
    eType          = TokenType::kParenR;
    }
  else if (strncmp (szTextIn, ";", iTextLengthIn) == 0)
    {
    eType          = TokenType::kExpressionEnd;
    }
  else if (strncmp (szTextIn, ",", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpList;
    bIsOperator    = TRUE;
    iPrecedence    = 1;
    eAssociativity = AssociativityType::kLeft;
    }

  // Comparisons
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "==", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpCmpEQ;
    bIsOperator    = TRUE;
    iPrecedence    = 3;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "!=", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpCmpNE;
    bIsOperator    = TRUE;
    iPrecedence    = 3;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "<=", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpCmpLE;
    bIsOperator    = TRUE;
    iPrecedence    = 4;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, ">=", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpCmpGE;
    bIsOperator    = TRUE;
    iPrecedence    = 4;
    eAssociativity = AssociativityType::kLeft;
    }
  else if (strncmp (szTextIn, "<", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpCmpLT;
    bIsOperator    = TRUE;
    iPrecedence    = 4;
    eAssociativity = AssociativityType::kLeft;
    }
  else if (strncmp (szTextIn, ">", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpCmpGT;
    bIsOperator    = TRUE;
    iPrecedence    = 4;
    eAssociativity = AssociativityType::kLeft;
    }

  // Assignment
  else if (strncmp (szTextIn, "=", iTextLengthIn) == 0)
    {
    eType          = TokenType::kOpAssign;
    bIsOperator    = TRUE;
    iPrecedence    = 1;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "+=", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpPlusAssign;
    bIsOperator    = TRUE;
    iPrecedence    = 1;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "-=", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpMinusAssign;
    bIsOperator    = TRUE;
    iPrecedence    = 1;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "*=", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpMultiplyAssign;
    bIsOperator    = TRUE;
    iPrecedence    = 1;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "/=", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpDivideAssign;
    bIsOperator    = TRUE;
    iPrecedence    = 1;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "%=", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpModAssign;
    bIsOperator    = TRUE;
    iPrecedence    = 1;
    eAssociativity = AssociativityType::kLeft;
    }

  // Logical Operators
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "||", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpLogicalOr;
    bIsOperator    = TRUE;
    iPrecedence    = 2;
    eAssociativity = AssociativityType::kLeft;
    }
  else if ((iTextLengthIn == 2) && (strncmp (szTextIn, "&&", iTextLengthIn) == 0))
    {
    eType          = TokenType::kOpLogicalAnd;
    bIsOperator    = TRUE;
    iPrecedence    = 2;
    eAssociativity = AssociativityType::kLeft;
    }

  // identifiers, strings, contstants, and other non-reserved keyword stuff
  else
    {
    RStrParser  parserText;
    parserText.AppendChars (szTextIn, iTextLengthIn);

    if (rexFloat.HasMatch (parserText.AsChar ()))
      {
      // Floating point number
      eType = TokenType::kFloat;
      Set ((FLOAT) atof(szTextIn));
      }
    else if (rexInt.HasMatch (parserText.AsChar ()))
      {
      // Integer number
      eType = TokenType::kInt;
      Set (atoi(szTextIn));
      }
    else if ((iTextLengthIn == 6) && (strncmp (szTextIn, "return", iTextLengthIn) == 0))
      {
      // "return" keyword
      eType = TokenType::kReturn;
      }
    else if (rexTrue.HasMatch (parserText.AsChar ()))
      {
      eType = TokenType::kInt;
      Set (1);
      }
    else if (rexFalse.HasMatch (parserText.AsChar ()))
      {
      eType = TokenType::kInt;
      Set (0);
      }
    else if (rexIdentifier.HasMatch (parserText.AsChar ()))
      {
      eType = TokenType::kIdentifier;
      }
    else if (rexDoubleString.HasMatch (parserText.AsChar ()))
      {
      // strip the double quotes

      parserText.GetQuoteString (&strRaw);

      eType = TokenType::kString;
      }
    else if (rexSingleString.HasMatch (parserText.AsChar ()))
      {
      // strip the single quotes

      // works with both single and double quotes
      parserText.GetQuoteString (&strRaw);

      eType = TokenType::kString;
      }
    else
      {
      eType = TokenType::kUnknown;
      }
    };
  };

//-----------------------------------------------------------------------------
Token *  Token::MakeStringToken  (const char *   szTextIn,
                                  INT            iTextLengthIn)
  {
  Token *  pNew = new Token();
  pNew->eType = TokenType::kString;
  pNew->strRaw.AppendChars (szTextIn, iTextLengthIn);
  return (pNew);
  };

//-----------------------------------------------------------------------------
Token::~Token  ()
  {
  // delete any siblings in the parameter list
  while (pNext != NULL)
    {
    Token *  pToDelete = pNext;
    pNext = pToDelete->pNext;
    pToDelete->pNext = NULL;
    delete (pToDelete);
    }
  };

//-----------------------------------------------------------------------------
VOID Token::Init (VOID)
  {
  strRaw.Empty();
  fValue = 0.0f;
  iValue = 0;
  pNext  = NULL;

  eType          = TokenType::kUnknown;
  bIsOperator    = FALSE;
  bIsUnary       = FALSE;
  iPrecedence    = 0;
  eAssociativity = AssociativityType::kNone;
  };

//-----------------------------------------------------------------------------
Token::Token (const Token &  tokIn)
  {
  Init ();
  *this = tokIn;
  };

//-----------------------------------------------------------------------------
BOOL  Token::IsInt (ValueRegistry *  pRegistryIn,
                    KVPArray *       pEnvVarsIn)
  {
  static RStrParser  parserName;

  return ((IsType (TokenType::kInt)) ||
          (
          IsType (TokenType::kVariable) &&
          (pRegistryIn != NULL) &&
          (pRegistryIn->IsInt (ResolveVarName (&parserName, pEnvVarsIn)))
          )
         );
  };

//-----------------------------------------------------------------------------
BOOL  Token::IsFloat (ValueRegistry *  pRegistryIn,
                      KVPArray *       pEnvVarsIn)
  {
  static RStrParser  parserName;

  return ((IsType (TokenType::kFloat)) ||
          (
          IsType (TokenType::kVariable) &&
          (pRegistryIn != NULL) &&
          (pRegistryIn->IsFloat (ResolveVarName (&parserName, pEnvVarsIn)))
          )
         );
  };

//-----------------------------------------------------------------------------
BOOL  Token::IsString (ValueRegistry *  pRegistryIn,
                       KVPArray *       pEnvVarsIn)
  {
  static RStrParser  parserName;

  return ((IsType (TokenType::kString)) ||
          (
          IsType (TokenType::kVariable) &&
          (pRegistryIn != NULL) &&
          (pRegistryIn->IsString (ResolveVarName (&parserName, pEnvVarsIn)))
          )
         );
  };

//-----------------------------------------------------------------------------
const char *  Token::GetTypeString (VOID)
  {
  switch (eType)
    {
    case TokenType::kUnknown:          return ("Unknown");
    case TokenType::kFloat:            return ("Float");
    case TokenType::kInt:              return ("Int");
    case TokenType::kString:           return ("String");
    case TokenType::kIdentifier:       return ("Identifier");
    case TokenType::kFunction:         return ("Function");
    case TokenType::kVariable:         return ("Variable");
    case TokenType::kExpressionEnd:    return ("ExpressionEnd");
    case TokenType::kParenL:           return ("ParenL");
    case TokenType::kParenR:           return ("ParenR");
    case TokenType::kOpList:           return ("OpList");
    case TokenType::kOpInc:            return ("OpInc");
    case TokenType::kOpDec:            return ("OpDec");
    case TokenType::kOpPlus:           return ("OpPlus");
    case TokenType::kOpMinus:          return ("OpMinus");
    case TokenType::kOpMultiply:       return ("OpMultiply");
    case TokenType::kOpDivide:         return ("OpDivide");
    case TokenType::kOpCmpEQ:          return ("OpCmpEQ");
    case TokenType::kOpCmpNE:          return ("OpCmpNE");
    case TokenType::kOpCmpLE:          return ("OpCmpLE");
    case TokenType::kOpCmpGE:          return ("OpCmpGE");
    case TokenType::kOpCmpLT:          return ("OpCmpLT");
    case TokenType::kOpCmpGT:          return ("OpCmpGT");
    case TokenType::kOpAssign:         return ("OpAssign");
    case TokenType::kOpPlusAssign:     return ("OpPlusAssign");
    case TokenType::kOpMinusAssign:    return ("OpMinusAssign");
    case TokenType::kOpMultiplyAssign: return ("OpMultiplyAssign");
    case TokenType::kOpDivideAssign:   return ("OpDivideAssign");
    case TokenType::kOpLogicalAnd:     return ("OpLogicalAnd");
    case TokenType::kOpLogicalOr:      return ("OpLogicalOr");
    case TokenType::kOpMod:            return ("OpMod");
    case TokenType::kOpModAssign:      return ("OpModAssign");
    case TokenType::kReturn:           return ("Return");
    };
  return ("Unknown type value");
  };

//-----------------------------------------------------------------------------
Token &  Token::operator=  (const Token &  tokIn)
  {
  strRaw         = tokIn.strRaw;
  fValue         = tokIn.fValue;
  iValue         = tokIn.iValue;
  eType          = tokIn.eType;
  bIsOperator    = tokIn.bIsOperator;
  bIsUnary       = tokIn.bIsUnary;
  iPrecedence    = tokIn.iPrecedence;
  eAssociativity = tokIn.eAssociativity;
  pNext          = NULL;

  return (*this);
  };

//-----------------------------------------------------------------------------
FLOAT  Token::AsFloat  (ValueRegistry *  pRegistryIn,
                        KVPArray *       pEnvVarsIn)
  {
  static RStrParser  parserName;

  if (IsType (TokenType::kFloat))
    {
    return (fValue);
    }
  else if (IsType (TokenType::kInt))
    {
    return ((FLOAT) iValue);
    }
  else if ((eType == TokenType::kVariable) && (pRegistryIn != NULL))
    {
    // NOTE:  If this proves to be too slow in the long run, we can break
    //  encapsulation or cache the last found value in the registry so
    //  we don't have to do the variable lookup twice (for IsFloat and GetFloat)

    const char *  szResolvedName = ResolveVarName (&parserName, pEnvVarsIn);

    // REFACTOR: Why can't we just call pRegistryIn->GetFloat () and let the registry
    //  element do the type conversion?

    if (pRegistryIn->IsFloat (szResolvedName))
      {
      return (pRegistryIn->GetFloat (szResolvedName));
      }
    else
      {
      return ((FLOAT) pRegistryIn->GetInt (szResolvedName));
      };
    };
  return (0.0f);
  };

//-----------------------------------------------------------------------------
INT  Token::AsInt  (ValueRegistry *  pRegistryIn,
                    KVPArray *       pEnvVarsIn)
  {
  static RStrParser  parserName;

  if (IsType (TokenType::kFloat))
    {
    return ((INT) floor (fValue + 0.5f));
    }
  else if (IsType (TokenType::kInt))
    {
    return (iValue);
    }
  else if ((eType == TokenType::kVariable) && (pRegistryIn != NULL))
    {
    // NOTE:  If this proves to be too slow in the long run, we can break
    //  encapsulation or cache the last found value in the registry so
    //  we don't have to do the variable lookup twice (for IsFloat and GetFloat)

    const char *  szResolvedName = ResolveVarName (&parserName, pEnvVarsIn);

    // REFACTOR: Why can't we just call pRegistryIn->GetFloat () and let the registry
    //  element do the type conversion?

    if (pRegistryIn->IsFloat (szResolvedName))
      {
      return ((INT) floor (pRegistryIn->GetFloat (szResolvedName) + 0.5f));
      }
    else
      {
      //DBG_INFO ("Token is variable, and AsInt returns %d", pRegistryIn->GetInt (szResolvedName));
      return (pRegistryIn->GetInt (szResolvedName));
      };
    };
  return (0);
  };

//-----------------------------------------------------------------------------
VOID  Token::Set  (FLOAT            fIn,
                   ValueRegistry *  pRegistryIn,
                   KVPArray *       pEnvVarsIn)
  {
  static RStrParser  parserName;

  if (IsType (TokenType::kFloat))
    {
    fValue = fIn;
    }
  else if (IsType (TokenType::kInt))
    {
    // auto-convert ints to floats
    fValue = fIn;
    eType = TokenType::kFloat;
    }
  else if ((IsType (TokenType::kVariable)) && (pRegistryIn != NULL))
    {
    // for existing variables, we maintain their type
    const char *  szResolvedName = ResolveVarName (&parserName, pEnvVarsIn);

    if ((pRegistryIn->IsFloat (szResolvedName)) || (!pRegistryIn->HasKey (szResolvedName)))
      {
      pRegistryIn->SetFloat (szResolvedName, fIn);
      }
    else if (pRegistryIn->IsInt (szResolvedName))
      {
      pRegistryIn->SetInt (szResolvedName, (INT)floor (fIn + 0.5f));
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  Token::Set  (INT              iIn,
                   ValueRegistry *  pRegistryIn,
                   KVPArray *       pEnvVarsIn)
  {
  static RStrParser  parserName;

  if (IsType (TokenType::kFloat))
    {
    // auto-convert floats to ints with set
    iValue = iIn;
    eType = TokenType::kInt;
    }
  else if (IsType (TokenType::kInt))
    {
    iValue = iIn;
    }
  else if ((IsType (TokenType::kVariable)) && (pRegistryIn != NULL))
    {
    // for existing variables, we maintain their type
    const char *  szResolvedName = ResolveVarName (&parserName, pEnvVarsIn);

    if (pRegistryIn->IsFloat (szResolvedName))
      {
      pRegistryIn->SetFloat (szResolvedName, (FLOAT) iIn);
      }
    else if (pRegistryIn->IsBool (szResolvedName))
      {
      //DBG_INFO ("Setting token %s as bool %d", szResolvedName, iIn);
      pRegistryIn->SetInt (szResolvedName, iIn);
      }
    else if ((pRegistryIn->IsInt (szResolvedName)) || (!pRegistryIn->HasKey (szResolvedName)))
      {
      //DBG_INFO ("Setting token %s as int %d", szResolvedName, iIn);
      pRegistryIn->SetInt (szResolvedName, iIn);
      };
    };
  };

//-----------------------------------------------------------------------------
const char *  Token::AsString  (ValueRegistry *  pRegistryIn,
                                KVPArray *       pEnvVarsIn)
  {
  static RStrParser  parserName;

  if (IsType (TokenType::kString) || IsType (TokenType::kIdentifier))
    {
    return (strRaw.AsChar ());
    }
  else if ((IsType (TokenType::kVariable)) && (pRegistryIn != NULL))
    {
    return (pRegistryIn->GetString (ResolveVarName (&parserName, pEnvVarsIn)));
    }
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  Token::Set  (const char *     szIn,
                   ValueRegistry *  pRegistryIn,
                   KVPArray *       pEnvVarsIn)
  {
  RStrParser  parserName;

  if (IsType (TokenType::kString))
    {
    strRaw.Set (szIn);
    }
  else if ((IsType (TokenType::kVariable)) && (pRegistryIn != NULL))
    {
    const char *  szResolvedName = ResolveVarName (&parserName, pEnvVarsIn);

    //DBG_INFO ("Token Set String %s (%s)", szResolvedName, szIn);
    pRegistryIn->SetString (szResolvedName, szIn);
    //DBG_INFO ("Token Set String Result %s",pRegistryIn->GetString (szResolvedName));
    }
  };

//-----------------------------------------------------------------------------
VOID  Token::Evaluate  (Token *          ptokOperandL,
                        Token *          ptokOperandR,
                        ValueRegistry *  pRegistryIn,
                        KVPArray *       pEnvVarsIn)
  {
  // Note:  Pointers to the L and R operands are given to this routine.
  //        The result is placed in the L operand.  The R operand is deleted if needed.

  // special case unary operators
  if (ptokOperandL == NULL)
    {
    if (this->eType == TokenType::kOpMinus)
      {
      if (ptokOperandR != NULL)
        {
        if (ptokOperandR->IsInt (pRegistryIn, pEnvVarsIn))
          {
          ptokOperandR->Set (- ptokOperandR->AsInt (pRegistryIn, pEnvVarsIn));
          return;
          };
        if (ptokOperandR->IsFloat (pRegistryIn, pEnvVarsIn))
          {
          ptokOperandR->Set (- ptokOperandR->AsFloat (pRegistryIn, pEnvVarsIn));
          return;
          };
        };
      };
    };

  ASSERT (ptokOperandL != NULL);

  INT   iIntMath = 0;
  INT   iFloatMath   = 0;
  BOOL  bStringMath = FALSE;

  if ((ptokOperandL != NULL) && ptokOperandL->IsString (pRegistryIn, pEnvVarsIn)) {bStringMath = TRUE;};
  if ((ptokOperandR != NULL) && ptokOperandR->IsString (pRegistryIn, pEnvVarsIn)) {bStringMath = TRUE;};

  if ((ptokOperandL != NULL) && ptokOperandL->IsFloat (pRegistryIn, pEnvVarsIn)) {++iFloatMath;};
  if ((ptokOperandR != NULL) && ptokOperandR->IsFloat (pRegistryIn, pEnvVarsIn)) {++iFloatMath;};

  if ((ptokOperandL != NULL) && ptokOperandL->IsInt (pRegistryIn, pEnvVarsIn)) {++iIntMath;};
  if ((ptokOperandR != NULL) && ptokOperandR->IsInt (pRegistryIn, pEnvVarsIn)) {++iIntMath;};

  //DBG_INFO ("int count %d  float count %d  string math %s", iIntMath, iFloatMath, bStringMath ? "yes" : "no")

  // Try float math first
  if (iFloatMath > 0 && !bStringMath)
    {
    FLOAT  fValueL = (ptokOperandL == NULL) ? 0.0f : ptokOperandL->AsFloat (pRegistryIn, pEnvVarsIn);
    FLOAT  fValueR = (ptokOperandR == NULL) ? 0.0f : ptokOperandR->AsFloat (pRegistryIn, pEnvVarsIn);

    switch (this->eType)
      {
      case TokenType::kOpInc:             ptokOperandL->Set (fValueL + 1.0f); break;
      case TokenType::kOpDec:             ptokOperandL->Set (fValueL - 1.0f); break;
      case TokenType::kOpPlus:            ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set (fValueL + fValueR); break;
      case TokenType::kOpMinus:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set (fValueL - fValueR); break;
      case TokenType::kOpMultiply:        ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set (fValueL * fValueR); break;
      case TokenType::kOpDivide:          ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set (fValueL / fValueR); break;
      case TokenType::kOpMod:             ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set (fValueL / fValueR - floorf (fValueL / fValueR)); break;

      case TokenType::kOpCmpEQ:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set (FLT_APPROX_EQUAL (fValueL, fValueR) ? 1.0f : 0.0f); break;
      case TokenType::kOpCmpNE:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set (FLT_APPROX_EQUAL (fValueL, fValueR) ? 0.0f : 1.0f); break;
      case TokenType::kOpCmpLE:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set ((FLT_APPROX_EQUAL (fValueL, fValueR) || (fValueL < fValueR)) ? 1.0f : 0.0f); break;
      case TokenType::kOpCmpGE:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set ((FLT_APPROX_EQUAL (fValueL, fValueR) || (fValueL > fValueR)) ? 1.0f : 0.0f); break;
      case TokenType::kOpCmpLT:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set ((fValueL < fValueR) ? 1.0f : 0.0f); break;
      case TokenType::kOpCmpGT:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set ((fValueL > fValueR) ? 1.0f : 0.0f); break;

      case TokenType::kOpAssign:          ptokOperandL->Set (fValueR, pRegistryIn, pEnvVarsIn); break; // DBG_INFO ("Assigning value %f to %s", fValueR, ptokOperandL->strRaw.AsChar ());
      case TokenType::kOpPlusAssign:      ptokOperandL->Set (fValueL + fValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpMinusAssign:     ptokOperandL->Set (fValueL - fValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpMultiplyAssign:  ptokOperandL->Set (fValueL * fValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpDivideAssign:    ptokOperandL->Set (fValueL / fValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpModAssign:       ptokOperandL->Set (fValueL / fValueR - floorf (fValueL / fValueR), pRegistryIn, pEnvVarsIn); break;

      case TokenType::kOpLogicalAnd:      {
                                          ptokOperandL->eType = TokenType::kFloat;
                                          //DBG_INFO ("Logical Float And L: %f  R: %f", fValueL, fValueR);
                                          ptokOperandL->Set ((!FLT_APPROX_EQUAL (fValueL, 0.0f) && !FLT_APPROX_EQUAL (fValueR, 0.0f)) ? 1.0f : 0.0f);
                                          };
                                          break;
      case TokenType::kOpLogicalOr:       ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set ((!FLT_APPROX_EQUAL (fValueL, 0.0f) || !FLT_APPROX_EQUAL (fValueR, 0.0f)) ? 1.0f : 0.0f); break;

      case TokenType::kOpList:            ptokOperandL->ArrayAppend (ptokOperandR); return; // Return so we don't deallocate ptokOperandR
      default:                            ptokOperandL->Set (0.0f); break;
      };
    }
  else if (iIntMath > 0 && !bStringMath)
    {
    INT  iValueL = (ptokOperandL == NULL) ? 0 : ptokOperandL->AsInt (pRegistryIn, pEnvVarsIn);
    INT  iValueR = (ptokOperandR == NULL) ? 0 : ptokOperandR->AsInt (pRegistryIn, pEnvVarsIn);

    switch (this->eType)
      {
      case TokenType::kOpInc:             ptokOperandL->Set (iValueL + 1); break;
      case TokenType::kOpDec:             ptokOperandL->Set (iValueL - 1); break;
      case TokenType::kOpPlus:            ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set (iValueL + iValueR); break;
      case TokenType::kOpMinus:           ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set (iValueL - iValueR); break;
      case TokenType::kOpMultiply:        ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set (iValueL * iValueR); break;
      case TokenType::kOpDivide:          ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set (iValueL / iValueR); break;
      case TokenType::kOpMod:             ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set (iValueL % iValueR); break;

      case TokenType::kOpCmpEQ:           ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set ((iValueL == iValueR) ? 1 : 0); break;
      case TokenType::kOpCmpNE:           ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set ((iValueL != iValueR) ? 1 : 0); break;
      case TokenType::kOpCmpLE:           ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set ((iValueL <= iValueR) ? 1 : 0); break;
      case TokenType::kOpCmpGE:           ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set ((iValueL >= iValueR) ? 1 : 0); break;
      case TokenType::kOpCmpLT:           ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set ((iValueL < iValueR) ? 1 : 0); break;
      case TokenType::kOpCmpGT:           ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set ((iValueL > iValueR) ? 1 : 0); break;

      case TokenType::kOpAssign:          ptokOperandL->Set (iValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpPlusAssign:      ptokOperandL->Set (iValueL + iValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpMinusAssign:     ptokOperandL->Set (iValueL - iValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpMultiplyAssign:  ptokOperandL->Set (iValueL * iValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpDivideAssign:    ptokOperandL->Set (iValueL / iValueR, pRegistryIn, pEnvVarsIn); break;
      case TokenType::kOpModAssign:       ptokOperandL->Set (iValueL % iValueR, pRegistryIn, pEnvVarsIn); break;

      case TokenType::kOpLogicalAnd:      {
                                          ptokOperandL->eType = TokenType::kInt;
                                          //DBG_INFO ("Logical Int And L: %d  R: %d", iValueL, iValueR);
                                          ptokOperandL->Set ((iValueL && iValueR) ? 1 : 0);
                                          };
                                          break;
      case TokenType::kOpLogicalOr:       ptokOperandL->eType = TokenType::kInt; ptokOperandL->Set ((iValueL || iValueR) ? 1 : 0); break;

      case TokenType::kOpList:            ptokOperandL->ArrayAppend (ptokOperandR); return; // Return so we don't deallocate ptokOperandR
      default:                            ptokOperandL->Set (0); break;
      };
    }

  else if (bStringMath)
    {
    switch (this->eType)
      {
      case TokenType::kOpPlus:            {
                                          RStr strOut (ptokOperandL->AsString (pRegistryIn, pEnvVarsIn));
                                          strOut += ptokOperandR->AsString (pRegistryIn, pEnvVarsIn);
                                          ptokOperandL->eType = TokenType::kString;
                                          ptokOperandL->Set (strOut.AsChar ());
                                          break;
                                          };
      case TokenType::kOpCmpEQ:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set ((ptokOperandL->AsString (pRegistryIn, pEnvVarsIn) == ptokOperandR->AsString (pRegistryIn, pEnvVarsIn)) ? 1.0f : 0.0f); break;
      case TokenType::kOpCmpNE:           ptokOperandL->eType = TokenType::kFloat; ptokOperandL->Set ((ptokOperandL->AsString (pRegistryIn, pEnvVarsIn) == ptokOperandR->AsString (pRegistryIn, pEnvVarsIn)) ? 0.0f : 1.0f); break;
      case TokenType::kOpAssign:          {
                                          ptokOperandL->Set (ptokOperandR->AsString (pRegistryIn, pEnvVarsIn), pRegistryIn, pEnvVarsIn); break;
                                          }
      case TokenType::kOpPlusAssign:      {
                                          RStr strOut (ptokOperandL->AsString (pRegistryIn, pEnvVarsIn));
                                          strOut += ptokOperandR->AsString (pRegistryIn, pEnvVarsIn);
                                          if (!ptokOperandL->IsType(TokenType::kVariable))
                                            {
                                            ptokOperandL->eType = TokenType::kString;
                                            }
                                          else
                                            {
                                            //DBG_INFO ("%s is a variable",  ptokOperandL->strRaw.AsChar ());

                                            };
                                          //DBG_INFO ("Setting PlusAssign (%s) to %s", ptokOperandL->strRaw.AsChar (), strOut.AsChar ());
                                          ptokOperandL->Set (strOut.AsChar (), pRegistryIn, pEnvVarsIn);
                                          break;
                                          };
      case TokenType::kOpList:            ptokOperandL->ArrayAppend (ptokOperandR); return; // Return so we don't deallocate ptokOperandR
      default:                            ptokOperandL->Set (""); break;
      };


    };
  delete (ptokOperandR);
  };

//-----------------------------------------------------------------------------
INT  Token::ComparePrecedence  (const Token &  tokOperandIn)
  {
  if (!bIsOperator || !tokOperandIn.bIsOperator) {return 0;};

  if (iPrecedence > tokOperandIn.iPrecedence) return 1;
  if (iPrecedence < tokOperandIn.iPrecedence) return -1;
  return 0;
  };

//-----------------------------------------------------------------------------
VOID  Token::ArrayAppend  (Token *  ptokAppendIn)
  {
  Token *  pCurr = this;

  while (pCurr->pNext != NULL)
    {
    pCurr = pCurr->pNext;
    };
  pCurr->pNext = ptokAppendIn;
  };

//-----------------------------------------------------------------------------
INT  Token::ArrayCount  (VOID)
  {
  // NOTE:  The "this" pointer may be null.
  Token *  pToken = this;
  INT  iReturn = 0;

  while (pToken != NULL)
    {
    ++iReturn;
    pToken = pToken->pNext;
    };

  return (iReturn);
  };

//-----------------------------------------------------------------------------
const char *  Token::ResolveVarName  (RStrParser *  pParserIn,
                                      KVPArray *    pEnvVarIn)
  {
  if ((pParserIn == NULL) || (pEnvVarIn == NULL))
    {
    return (strRaw.AsChar ());
    };
  if (strRaw.FindChar ('$') == -1)
    {
    return (strRaw.AsChar ());
    };

  pParserIn->Set (strRaw);

  pEnvVarIn->ExpandVars (*pParserIn);
  return (pParserIn->AsChar ());
  };
#endif // MOVED



//-----------------------------------------------------------------------------
//  Expression Functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
ExpressionFn::ExpressionFn ()
  {
  // This constructor is for functions that are part of a derived class.
  ptrFn = NULL;
  };

//-----------------------------------------------------------------------------
ExpressionFn::ExpressionFn (const char *      szNameIn,
                            ExpressionFnPtr   ptrFnIn)
  {
  // This constructor is for associating a callback function with a name.
  strName.Set (szNameIn, TRUE);
  ptrFn = ptrFnIn;
  };

//-----------------------------------------------------------------------------
VOID  ExpressionFn::OnCall (Token *          ptokParamsIn,
                            ValueRegistry *  pRegistryIn,
                            Token *          ptokResultOut)
  {
  // This method is for expressions that are not part of a derived class, but
  //  rather simply a callback function associated with a name.

  if (ptrFn != NULL)
    {
    (*ptrFn)(ptokParamsIn, pRegistryIn, ptokResultOut);
    };
  };

//-----------------------------------------------------------------------------
//  Expression
//-----------------------------------------------------------------------------

TList<ExpressionFn*>   Expression::listFn;

//-----------------------------------------------------------------------------
VOID  Expression::RegisterFunction (ExpressionFn *  pfnIn)
  {
  //DBG_INFO ("Expression::RegisterFunction - %s", pfnIn->Name ().AsChar ());
  listFn.PushFront (pfnIn);
  };

//-----------------------------------------------------------------------------
VOID  Expression::UnregisterFunction (const char *  pszNameIn)
  {
  for (TListItr<ExpressionFn*>  itrCurr = listFn.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->MatchesName (pszNameIn))
      {
      UnregisterFunction (*itrCurr);
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  Expression::UnregisterFunction (ExpressionFn *  pfnIn)
  {
  //DBG_INFO ("Expression::UnregisterFunction - %s", pfnIn->Name ().AsChar ());
  listFn.Delete (pfnIn);
  delete (pfnIn);
  };

//-----------------------------------------------------------------------------
VOID  Expression::UnregisterAllFunctions (VOID)
  {
  for (TListItr<ExpressionFn*> itrCurr = listFn.First (); itrCurr.IsValid (); ++itrCurr)
    {
    delete (*itrCurr);
    };
  listFn.Empty ();
  };

//-----------------------------------------------------------------------------
VOID  Expression::CallFunction  (const char *            szNameIn,
                                 Token *                 ptokParamsIn,
                                 ValueRegistry *         pRegistryIn,
                                 TList<ExpressionFn*> *  plistLocalFnIn,
                                 Token *                 ptokResultOut)
  {
  RStr   strName (szNameIn);
  strName.CalcHash ();

  TListItr<ExpressionFn*>  itrCurr;
  // NOTE: Local functions override global functions
  if (plistLocalFnIn != NULL)
    {
    for (itrCurr = plistLocalFnIn->First (); itrCurr.IsValid (); ++itrCurr)
      {
      if ((*itrCurr)->CanHandleFunction (strName))
        {
        (*itrCurr)->OnCall (ptokParamsIn, pRegistryIn, ptokResultOut);
        return;
        };
      };
    };
  for (itrCurr = listFn.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->CanHandleFunction (strName))
      {
      (*itrCurr)->OnCall (ptokParamsIn, pRegistryIn, ptokResultOut);
      return;
      };
    };

  // Error:  Didn't find the function.
  // TODO: Error Handling
  DBG_ERROR ("Unable to find Function : %s", szNameIn);
  };

//-----------------------------------------------------------------------------
VOID Expression::FreeTokenList (TList<Token*> ** pplistIn)
  {
  FreeTokenList (*(*pplistIn));
  delete (*pplistIn);
  *pplistIn = NULL;
  };

//-----------------------------------------------------------------------------
VOID Expression::FreeTokenList (TList<Token*> & listIn)
  {
  for (TListItr<Token*>  itrCurr = listIn.First (); itrCurr.IsValid (); ++itrCurr)
    {
    delete (*itrCurr);
    };
  listIn.Empty ();
  };

//-----------------------------------------------------------------------------
INT  Expression::ParseOutMacros (const char *  szScriptIn,
                                 KVPArray &    arrayMacrosOut)
  {

  RStrParser  parserScript;
  RStr        strKey;
  RStr        strValue;

  parserScript.AttachBuffer (szScriptIn);

  INT  iDefineStart;
  while ((iDefineStart = parserScript.FindIdentifier ("#define", parserScript.GetCursorStart())) != -1)
    {
    parserScript.SetCursorStart (iDefineStart);
    parserScript.SkipChars (8); // Skip #define and following whitespace char
    parserScript.SkipWhitespace ();

    strKey.Empty ();
    strValue.Empty ();

    parserScript.GetWord (strKey);
    parserScript.SkipWhitespace ();
    parserScript.GetLine (strValue);

    arrayMacrosOut.Append (strKey.AsChar(), strValue.AsChar ());
    };
  parserScript.DetachBuffer ();
  return (arrayMacrosOut.Length());
  };

//-----------------------------------------------------------------------------
TList<Token*> *  Expression::Tokenize  (const char *  szScriptIn,
                                        INT           iScriptLengthIn)
  {
  const char *  pszCurr = szScriptIn;

  // TODO: Implement Macros here.  Scan the script for the
  //  macro keyword (#define?).  Build a KVP dictionary.  If any macros are found,
  //  duplicate the script buffer and perform a Search and Replace on it.

  // Most expressions won't require macros substitution, and we can just work
  //  with the szScriptIn directly.  However those that do have macros require
  //  us to edit the string, which will require some (costly) allocation to be done.
  //  Therefore we make sure there are macros before doing any substitution.

  KVPArray    arrayMacros;

  INT  iNumMacros = ParseOutMacros (szScriptIn, arrayMacros);


  // TODO: Implement an Include keyword.  It will be used for macro inclusion, but
  //  could be used for additional parsing as well.


  // This RegEx defines how to split up the script into tokens, ignoring whitespace
  // NOTE:  I may need to strip whitespace since I lack group matching
  // TODO: Implement more escaped characters in RegEx, such as \s and \S

  /*
  [ ]*(

  ==|
  !=|
  <=|
  >=|
  <|
  >|
  =|
  +=|
  -=|
  *=|
  /=|
  ,|
  ++|
  --|
  |||
  &&|
  -|
  +|
  *|
  /|
  (|
  )|
  [0-9]+(.[0-9]+)?|[a-zA-Z\\$_]?[a-zA-Z0-9\\${}_.]*|
  ;|
  "[^"]*"

  )[ ]*
  */


  RegEx  rexTokens ("[ ]*(==|\\!=|<=|>=|<|>|=|\\+=|\\-=|\\*=|/=|%=|,|\\+\\+|\\-\\-|\\-|\\|\\||%|&&|\\+|\\*|/|\\(|\\)|[0-9]+(\\.[0-9]+)?|[a-zA-Z_\\$]?[a-zA-Z0-9\\${}_\\.]*|;|\"[^\"]*\"|'[^']*')[ ]*");
/*
[ ]*(==|\!=|<=|>=|<|>|=|\+=|\-=|\*=|/=|,|\+\+|\-\-|\-|\+|\*|/|\(|\)|[0-9]+(\.[0-9]+)?|[a-zA-Z_]?[a-zA-Z0-9_\.]*|;|"[^"]*")[ ]*
    4  7   1  1  1 1 2 2   2   3   3  3 4    4    5  5  5  5 6  6  6      7        8 8                         1 1       2
           1  4  7 9 1 3   7   1   5  8 0    5    0  3  6  9 1  4  7      4        3 5                         1 3       1
*/
  //RegEx  rexTokens ("[ ]*(\\+|\\-|\\*|\\\\|[0-9]+(\\.[0-9]+)?|;)[ ]*");

  INT  iScriptLength = (iScriptLengthIn == -1) ? (INT) strlen (pszCurr) : iScriptLengthIn;

  TList<Token*> *  pListOut = new TList<Token*>;

  //DBG_INFO ("Calling Expression::Tokenize on len(%d) : \"%s\"", iScriptLength, pszCurr);

  // NOTE:  Pointer arithmetic may not be the best way to solve this problem.  It is
  //         clear to write, but may not be supported in the future by the compiler.

  const char *  pszEnd = pszCurr + iScriptLength;
  const char *  pszMatchStart = NULL;
  INT           iMatchLength = 0;

  while (pszCurr < pszEnd)
    {
    rexTokens.Match (pszCurr, pszEnd - pszCurr, 0, &pszMatchStart, iMatchLength);

    //DBG_INFO ("curr %x  end %x  rem %d  match  %x  matchlen  %d", pszCurr, pszEnd, pszEnd - pszCurr, pszMatchStart, iMatchLength);

    RStr  strTest;
    strTest.AppendChars (pszMatchStart, iMatchLength);
    //DBG_INFO ("Match returned \"%s\"", strTest.AsChar ());

    // NOTE: Since capture groups are not currently implement we strip off whitespace

    if (iMatchLength != 0)
      {
      pszCurr += iMatchLength;

      while ((pszMatchStart[0] == ' ') && (iMatchLength > 0)) {++pszMatchStart; --iMatchLength;};
      while ((pszMatchStart[iMatchLength - 1] == ' ') && (iMatchLength > 0)) {--iMatchLength;};

      if (iMatchLength != 0)
        {
        pListOut->PushBack (new Token (pszMatchStart, iMatchLength));
        };
      }
    else
      {
      // no match.  Skip the current character and try again.  This should handle
      //  weird whitespace and line end characters.
      DBG_INFO ("No match, skip char \'%c\'", pszCurr[0]);
      ++pszCurr;
      };
    };

  //DBG_INFO ("Tokenize done");
  //DebugTokenList (pListOut);

  return (pListOut);
  };


//-----------------------------------------------------------------------------
TList<Token*> *  Expression::Compile  (const char *  szScriptIn,
                                       INT           iScriptLengthIn)
  {
  TList<Token*> *  plistTokenized = Tokenize (szScriptIn, iScriptLengthIn);
  TList<Token*> *  plistCompiled  = Compile  (plistTokenized);

  FreeTokenList (&plistTokenized);
  return (plistCompiled);
  };


//-----------------------------------------------------------------------------
TList<Token*> *  Expression::Compile  (TList<Token*> *  plistTokensIn)
  {
  // listOutput and listOpr are stacks, with the top of the stack being the
  //  first element in the list.  Pushing and popping should be done to the
  //  first element.

  ASSERT (plistTokensIn != NULL);
  if (plistTokensIn == NULL) return NULL;

  TList<Token*> *  plistOutput = new TList<Token*>;
  if (plistTokensIn->Size () == 0) return (plistOutput);


  TList<Token*> & listOutput = *plistOutput; // this is so I can treat listOutput and listOpr similarly.
  TList<Token*>   listOpr;

  TListItr<Token*>  itrNext;

  for (TListItr<Token*> itrCurr = plistTokensIn->First (); itrCurr.IsValid (); ++itrCurr)
    {
    // Copy the token, so you leave plistTokensIn unmodified
    Token *  pToken = new Token (*(*itrCurr));

    if (pToken->bIsOperator)
      {
      while (listOpr.Size() > 0)
        {
        Token *  pNextToken = listOpr.PeekFront ();

        if (pNextToken->bIsOperator && (
            (pToken->eAssociativity == AssociativityType::kLeft  && pToken->ComparePrecedence(*pNextToken) <= 0) ||
            (pToken->eAssociativity == AssociativityType::kRight && pToken->ComparePrecedence(*pNextToken) < 0)))
          {
          listOutput.PushFront (listOpr.PopFront ());
          }
        else
          {
          break;
          };
        }
      listOpr.PushFront(pToken);
      }
    else if (pToken->eType == TokenType::kParenL)
      {
      listOpr.PushFront(pToken);
      }
    else if (pToken->eType == TokenType::kParenR)
      {
      BOOL  bParenFound = FALSE;
      while (listOpr.Size() > 0)
        {
        // copy listOpr to Output until we find the left parenthesis
        Token *  pNextToken = listOpr.PeekFront ();
        if (pNextToken->eType == TokenType::kParenL)
          {
          bParenFound = TRUE;
          break;
          }
        else
          {
          listOutput.PushFront (listOpr.PopFront ());
          };
        };
      if (!bParenFound)
        {
        // TODO: Error: No left side parenthesis
        FreeTokenList (listOutput);
        FreeTokenList (listOpr);
        delete (pToken);
        return NULL;
        }
      // pop off left paren
      delete (listOpr.PopFront ());
      delete (pToken);

      // check to see if these parenthesis enclosed the parameter list of a function
      if ( (listOpr.PeekFront() != NULL) &&
           ((listOpr.PeekFront()->eType == TokenType::kFunction) ||
            (listOpr.PeekFront()->eType == TokenType::kReturn)))
        {
        listOutput.PushFront (listOpr.PopFront ());
        };
      }
    else if ((pToken->IsType (TokenType::kFloat)) ||
             (pToken->IsType (TokenType::kInt)) ||
             (pToken->IsType (TokenType::kString)))
      {
      listOutput.PushFront (pToken);
      }
    else if (pToken->eType == TokenType::kReturn)
      {
      listOpr.PushFront (pToken);
      }
    else if (pToken->eType == TokenType::kIdentifier)
      {
      // see if this is a variable or a function name

      itrNext = itrCurr; ++itrNext;
      if (itrNext.IsValid () && (*itrNext)->eType == TokenType::kParenL)
        {
        // function
        pToken->eType = TokenType::kFunction;
        listOpr.PushFront (pToken);
        }
      else
        {
        // variable
        pToken->eType = TokenType::kVariable;
        listOutput.PushFront (pToken);
        };
      }
    else if (pToken->eType == TokenType::kExpressionEnd)
      {
      // put Opr to Output, to prepare for the next expression (if any)
      while (listOpr.Size () > 0)
        {
        listOutput.PushFront (listOpr.PopFront ());
        };
      listOutput.PushFront (pToken);
      }
    else
      {
      // TODO: Error: Unknown Token
      FreeTokenList (listOutput);
      FreeTokenList (listOpr);
      delete (pToken);
      DBG_INFO ("Expression::Compile Error: Unknown Token");
      return (NULL);
      };
    }

  // put Opr to Output, to prepare for the next expression (if any)
  while (listOpr.Size () > 0)
    {
    listOutput.PushFront (listOpr.PopFront ());
    };

  // Reverse stack output and return.
  listOutput.Reverse ();


  // TODO:  Scan output list, and look for constant-constant-operator patterns.
  //  Because they are determinant, you can pre-process them and store the result.
  //  This may generate further C-C-O patterns that can be reduced.  This is a
  //  good and needed optimization step, especially with macros in play.


  return (plistOutput);
  };

//-----------------------------------------------------------------------------
FLOAT  Expression::Execute  (const char *            pszExpressionIn,
                             ValueRegistry *         pRegistryIn,
                             Token *                 pTokenOut,
                             KVPArray *              pEnvVarsIn,
                             TList<ExpressionFn*> *  plistLocalFnIn)
  {
  TList<Token*> *  pCompiled  = NULL;
  Token            tokResult;

  if (pRegistryIn == NULL)
    {
    pRegistryIn = ValueRegistry::Root ();
    };

  if (pTokenOut == NULL)
    {
    pTokenOut = &tokResult;
    };

  //DBG_INFO ("Expression::Execute running: %s", pszExpressionIn);

  pCompiled = Expression::Compile (pszExpressionIn);
  FLOAT  fResult = Expression::Execute (pCompiled, pRegistryIn, pTokenOut, NULL, pEnvVarsIn, plistLocalFnIn);
  Expression::FreeTokenList (&pCompiled);
  return (fResult);
  }

//-----------------------------------------------------------------------------
FLOAT  Expression::Execute  (TList<Token*> *         plistStackIn,
                             ValueRegistry *         pRegistryIn,
                             Token *                 pTokenOut,
                             BOOL *                  pbReturned,
                             KVPArray *              pEnvVarsIn,
                             TList<ExpressionFn*> *  plistLocalFnIn)
  {
  TList<Token*>  stackVar;  // treat this list as a stack (push front, pop front)
  FLOAT          fResult = 0.0f;
  Token *        pToken = NULL;

  //DBG_INFO ("Execute");
  //DebugTokenList (plistStackIn);

  if (pTokenOut != NULL)
    {
    pTokenOut->eType = TokenType::kUnknown;
    };
  if (pbReturned != NULL)
    {
    *pbReturned = FALSE;
    };

  // NOTE:  This function does a lot of allocation / deallocation as it
  //  executes.  This is not ideal, but it is simple to implement.  A future
  //  optimization pass should reduce this allocation through pools or
  //  pre-allocated stacks, or through some clever re-use of the passed
  //  token objects without actually modifying them.

  // execute stack.  Remember to leave original stack untouched.

  for (TListItr<Token*>  itrCurr = plistStackIn->First (); itrCurr.IsValid (); ++itrCurr)
    {
    // do not delete pToken, since the data remains in plistStackIn
    pToken = (*itrCurr);
    if ((pToken->IsType (TokenType::kInt)) ||
        (pToken->IsType (TokenType::kFloat)) ||
        (pToken->IsType (TokenType::kVariable)) ||
        (pToken->IsType (TokenType::kString)))
      {
      stackVar.PushFront (new Token (*pToken));
      }
    else if (pToken->bIsOperator)
      {
      Token *  pR = NULL;
      Token *  pL = NULL;

      if (pToken->bIsUnary)
        {
        pL = stackVar.PopFront ();
        }
      else
        {
        pR = stackVar.PopFront ();
        pL = stackVar.PopFront ();
        }

      // NOTE: pR will be deleted in the following call if needed. pL is recycled.
      pToken->Evaluate (pL, pR, pRegistryIn, pEnvVarsIn);
      stackVar.PushFront (pL != NULL ? pL : pR);
      }
    else if (pToken->IsType (TokenType::kExpressionEnd))
      {
      fResult = stackVar.PeekFront ()->AsFloat (pRegistryIn, pEnvVarsIn);
      if (pTokenOut != NULL)
        {
        *pTokenOut = *(stackVar.PeekFront());
        //DBG_INFO ("Return type is %s", pTokenOut->GetTypeString());
        };
      delete (stackVar.PopFront ());
      //if (pTokenOut != NULL) {DBG_INFO ("Saved Token Out Mark 0a %s", pTokenOut->strRaw.AsChar ());};
      }
    else if (pToken->IsType (TokenType::kFunction))
      {
      Token *   ptokenResult = new Token ();
      Token *   ptokenParams = stackVar.PopFront ();

      //DBG_INFO ("Expression: Calling Function : %s", pToken->strRaw.AsChar ());
      CallFunction  (pToken->strRaw.AsChar (), ptokenParams, pRegistryIn, plistLocalFnIn, ptokenResult);

      delete (ptokenParams);
      stackVar.PushFront (ptokenResult);
      }
    else if (pToken->IsType (TokenType::kReturn))
      {
      if (pbReturned != NULL)
        {
        *pbReturned = TRUE;
        };

      // the return value should be on the stack, ready to be popped.
      if ((stackVar.Size () == 0) || stackVar.PeekFront ()->IsType (TokenType::kExpressionEnd))
        {
        fResult = 0.0f;
        }
      else
        {
        fResult = stackVar.PeekFront ()->AsFloat (pRegistryIn, pEnvVarsIn);
        if (pTokenOut != NULL)
          {
          *pTokenOut = *(stackVar.PeekFront());
          //DBG_INFO ("Return type is %s", pTokenOut->GetTypeString());
          };
        delete (stackVar.PopFront ());
        };
      break;
      };
    };

  // catch the case where the expression didn't end in a semicolon (end expression token)
  if ((pToken != NULL) && (!pToken->IsType (TokenType::kExpressionEnd)) && (stackVar.Size () > 0))
    {
    fResult = stackVar.PeekFront ()->AsFloat (pRegistryIn, pEnvVarsIn);
    if (pTokenOut != NULL) {*pTokenOut = *(stackVar.PeekFront());};
    };

  // cleanup
  FreeTokenList (stackVar);
  return (fResult);
  };


//-----------------------------------------------------------------------------
VOID Expression::DebugTokenList (TList<Token*> * plistIn)
  {
  ASSERT (plistIn != NULL);

  DBG_INFO ("Token list with %d items", plistIn->Size());

  for (TListItr<Token*>  itrCurr = plistIn->First (); itrCurr.IsValid (); ++itrCurr)
    {
    DBG_INFO (" %s(%s)", (*itrCurr)->GetTypeString (), (*itrCurr)->strRaw.AsChar ());
    };
  };

//-----------------------------------------------------------------------------
VOID Expression::DebugTokenLinkedList (Token*  plistIn)
  {
  Token*  pCurr = plistIn;
  ASSERT (pCurr != NULL);

  //DBG_INFO ("Token list with %d items", pCurr->Size());

  for (; pCurr != NULL; pCurr = pCurr->Next())
    {
    DBG_INFO (" %s(%s)", pCurr->GetTypeString (), pCurr->strRaw.AsChar ());
    };
  };



