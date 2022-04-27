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

#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "Sys/Types.hpp"
#include "Containers/TList.hpp"
#include "Containers/KVPArray.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Script/ExpressionToken.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

#ifdef MOVED
namespace TokenType
  {
  enum Type
    {
    kUnknown          = 0,
    kFloat            = 1,
    kInt              = 2,
    kString           = 3,
    kIdentifier       = 4,
    kFunction         = 5,
    kVariable         = 6,
    kExpressionEnd    = 7,
    kParenL           = 8,
    kParenR           = 9,
    kOpList           = 10,
    kOpInc            = 11,
    kOpDec            = 12,
    kOpPlus           = 13,
    kOpMinus          = 14,
    kOpMultiply       = 15,
    kOpDivide         = 16,
    kOpCmpEQ          = 17,
    kOpCmpNE          = 18,
    kOpCmpLE          = 19,
    kOpCmpGE          = 20,
    kOpCmpLT          = 21,
    kOpCmpGT          = 22,
    kOpAssign         = 23,
    kOpPlusAssign     = 24,
    kOpMinusAssign    = 25,
    kOpMultiplyAssign = 26,
    kOpDivideAssign   = 27,
    kOpLogicalAnd     = 28,
    kOpLogicalOr      = 29,
    kOpMod            = 30,
    kOpModAssign      = 31,
    kReturn           = 32
    };
  };

namespace AssociativityType
  {
  enum Type
    {
    kNone  = 0,
    kRight = 1,
    kLeft  = 2
    };
  };

// NOTE: Probably move these to const strings.
// NOTE:  I don't currently handle non-capturing groups!
#define REX_FLOAT      "^[0-9]+\\.[0-9]+"
#define REX_INT        "^[0-9]+"
//#define REX_IDENTIFIER "^[a-zA-Z_]?[a-zA-Z0-9_\\.]*"
#define REX_TRUE "^[Tt][Rr][Uu][Ee]"
#define REX_FALSE "^[Ff][Aa][Ll][Ss][Ee]"
#define REX_IDENTIFIER "^[a-zA-Z_\\${]?[a-zA-Z0-9\\${}_\\.]*"
#define REX_DOUBLE_STRING     "^\"[^\"]*\""
#define REX_SINGLE_STRING     "^\'[^\']*\'"

//-----------------------------------------------------------------------------
class Token
  {
  public:
    RStr                     strRaw;
    FLOAT                    fValue;
    INT                      iValue;
    Token *                  pNext;

    TokenType::Type          eType;
    BOOL                     bIsOperator;
    BOOL                     bIsUnary;
    INT                      iPrecedence;
    AssociativityType::Type  eAssociativity;



  public:

                   Token             ()                                {Init ();};

                   Token             (const char *   szTextIn,
                                      INT            iTextLengthIn);

                   ~Token            ();

                   Token             (const Token &  tokIn);

    Token &        operator=         (const Token &  tokIn);

    VOID           Init              (VOID);

    BOOL           IsType            (TokenType::Type  typeIn)         {return (eType == typeIn);};

    BOOL           IsInt             (ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);

    BOOL           IsFloat           (ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);

    BOOL           IsString          (ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);

    const char *   GetTypeString     (VOID);

    FLOAT          AsFloat           (ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);

    INT            AsInt             (ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);

    VOID           Set               (FLOAT            fIn,
                                      ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);

    VOID           Set               (INT              iIn,
                                      ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);


    const char *   AsString          (ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);

    VOID           Set               (const char *     szIn,
                                      ValueRegistry *  pRegistryIn = NULL,
                                      KVPArray *       pEnvVarsIn  = NULL);

    VOID           Evaluate          (Token *          ptokOperandL,
                                      Token *          ptokOperandR,
                                      ValueRegistry *  pRegistryIn,
                                      KVPArray *       pEnvVarsIn  = NULL);

    INT            ComparePrecedence (const Token &  tokOperandIn);

    VOID           ArrayAppend       (Token *  ptokAppendIn);

    INT            ArrayCount        (VOID);

    Token *        Next              (VOID)   {return (pNext);};

    const char *   ResolveVarName    (RStrParser *  pParserIn,
                                      KVPArray *    pEnvVarIn);

    static Token * MakeStringToken   (const char *   szTextIn,
                                      INT            iTextLengthIn);


  };
#endif // MOVED


typedef VOID (*ExpressionFnPtr) (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut);

//-----------------------------------------------------------------------------
class ExpressionFn
  {
  protected:
    RStr              strName;
    ExpressionFnPtr   ptrFn;

  protected:
    VOID           SetName      (const char *  szNameIn)       {strName.Set (szNameIn); strName.CalcHash ();};

  public:

                   ExpressionFn ();

                   ExpressionFn (const char *      szNameIn,
                                 ExpressionFnPtr   ptrFnIn);

    virtual       ~ExpressionFn () {};

    virtual BOOL  CanHandleFunction (RStr &  strNameIn)    {return (strName.Equals (strNameIn));};
    //const RStr &  Name          (VOID)                     {return strName;};

    const char *  GetName       (VOID)                     {return (strName.AsChar ());};
    BOOL          MatchesName   (const char *  szNameIn)   {return (strName.Equals (szNameIn));};

    virtual VOID  OnCall        (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut);


  };


//-----------------------------------------------------------------------------
class Expression
  {
  private:
    static TList<ExpressionFn*>   listFn;

  public:

                            Expression       ()   {};

                            ~Expression      ()   {};

    static VOID             RegisterFunction       (ExpressionFn *  pfnIn);

    static VOID             UnregisterFunction     (const char *  pszNameIn);

    static VOID             UnregisterFunction     (ExpressionFn *  pfnIn);

    static VOID             UnregisterAllFunctions (VOID);

    static VOID             CallFunction     (const char *            szNameIn,
                                              Token *                 ptokParamsIn,
                                              ValueRegistry *         pRegistryIn,
                                              TList<ExpressionFn*> *  plistLocalFnIn,
                                              Token *                 ptokResultOut);

    static VOID             FreeTokenList    (TList<Token*> ** pplistIn);

    static VOID             FreeTokenList    (TList<Token*> &  listIn);

    static INT              ParseOutMacros   (const char *     szScriptIn,
                                              KVPArray &       arrayMacrosOut);

    static TList<Token*> *  Tokenize         (const char *     szScriptIn,
                                              INT              iScriptLengthIn = -1);

    static TList<Token*> *  Compile          (const char *     szScriptIn,
                                              INT              iScriptLengthIn = -1);

    static TList<Token*> *  Compile          (TList<Token*> *  plistTokensIn);

    static FLOAT            Execute          (const char *            pszExpressionIn,
                                              ValueRegistry *         pRegistryIn    = NULL,
                                              Token *                 pTokenOut      = NULL,
                                              KVPArray *              pEnvVarsIn     = NULL,
                                              TList<ExpressionFn*> *  plistLocalFnIn = NULL);



    static FLOAT            Execute          (TList<Token*> *         plistStackIn,
                                              ValueRegistry *         pRegistryIn    = NULL,
                                              Token *                 pTokenOut      = NULL,
                                              BOOL *                  pbReturned     = NULL,
                                              KVPArray *              pEnvVarsIn     = NULL,
                                              TList<ExpressionFn*> *  plistLocalFnIn = NULL);

    static VOID             DebugTokenList   (TList<Token*> *  plistTokensIn);

    static VOID             DebugTokenLinkedList (Token*  plistIn); // this is for parameter lists

  };

#endif // EXPRESSION_HPP
