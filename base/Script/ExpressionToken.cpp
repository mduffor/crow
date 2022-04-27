/* -----------------------------------------------------------------
                              Expression Token

     This module implements the object that stores a tokenized
   expressino for the Expression parser.

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

