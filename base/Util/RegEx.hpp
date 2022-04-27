/* -----------------------------------------------------------------
                      Regular Expression Library

     This module implements regular expression and glob matching.

   ----------------------------------------------------------------- */

// Modified BSD License:
//
// Copyright (c) 2004,2014, Michael T. Duffy II.  All rights reserved.
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

#ifndef RREGEX_HPP
#define RREGEX_HPP


#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Containers/RStrArray.hpp"
#include "Containers/BaseArray.hpp"
#include "Containers/IntArray.hpp"
#include "Containers/TArray.hpp"
/**
  @addtogroup base
  @{
*/
//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


//------------------------------------------------------------------------
class RegExMatch
  {
  public:

    enum  EType {kNull         = 0,
                 kOr           = 1,
                 kRepeat       = 2,
                 kAnyChar      = 3,
                 kChar         = 4,
                 kCharRange    = 5,
                 kAlnum        = 6,
                 kAlpha        = 7,
                 kBlank        = 8,
                 kCntrl        = 9,
                 kDigits       = 10,
                 kGraph        = 11,
                 kLower        = 12,
                 kPrint        = 13,
                 kPunct        = 14,
                 kSpace        = 15,
                 kUpper        = 16,
                 kXDigit       = 17,
                 kWordBoundary = 18,
                 kWordInside   = 19,
                 kWordBegin    = 20,
                 kWordEnd      = 21,
                 kWordChar     = 22,
                 kNotWordChar  = 23,
                 kBufferStart  = 24,
                 kBufferEnd    = 25,
                 kLineStart    = 26,
                 kLineEnd      = 27
                 };

  public:
//  private:

    EType   eMatchType;
    UINT    uStartMatch;
    UINT    uEndMatch;
    BOOL    bInvertMatch;

    INT     iNextOne;
    INT     iNextTwo;

    INT     iMinMatches;
    INT     iMaxMatches;
    INT     iNumMatches;

    BOOL    bParsingError;

  private:

    BOOL           IsWordChar  (const char *  pchIn);

    BOOL           RawMatch    (const char *  pchIn,
                                const char *  pchBufferStart,
                                const char *  pchBufferEnd);



  public:

                   RegExMatch  ();

   explicit        RegExMatch  (EType  eMatchTypeIn,
                                UINT   uMatchStartIn  = 0,
                                UINT   uMatchEndIn    = 0,
                                INT    iNextOneIn     = 0,
                                INT    iNextTwoIn     = 0,
                                BOOL   bInvertMatchIn = FALSE);

                   RegExMatch (const RegExMatch &  matchIn);

                   ~RegExMatch ()                            {};

    RegExMatch &  operator=   (const RegExMatch &  remIn);

    BOOL           IsMatch     (INT           iMatchState,
                                const char *  pchIn,
                                const char *  pchBufferStart,
                                const char *  pchBufferEnd);

    VOID           SetType     (EType  eMatchTypeIn,
                                UINT   uStartMatchIn = 0x20,
                                UINT   uEndMatchIn   = 0x20)  {eMatchType = eMatchTypeIn; uStartMatch = uStartMatchIn; uEndMatch = uEndMatchIn;};

    UINT           GetStartMatch  (VOID)   {return uStartMatch;};

    UINT           GetEndMatch    (VOID)   {return uEndMatch;};

                   /** @brief Returns whether to increment to the next character if this matches.
                       @return True if the counter should be incremented, false if not.
                   */
    BOOL           IncOnMatch      (VOID);

    VOID           InvertMatch     (BOOL  bInvertIn)          {bInvertMatch = bInvertIn;};

    BOOL           GetInvertMatch  (VOID)                     {return (bInvertMatch);};

    BOOL           IsNull          (VOID)                     {return (eMatchType == kNull);};

    BOOL           IsOr            (VOID)                     {return (eMatchType == kOr);};

    BOOL           IsRepeat        (VOID)                     {return (eMatchType == kRepeat);};

    BOOL           IsCharRange     (VOID)                     {return (eMatchType == kCharRange);};

    BOOL           IsChar          (VOID)                     {return (eMatchType == kChar);};

    BOOL           IsCharOrRange   (VOID)                     {return ((eMatchType == kCharRange) || (eMatchType == kChar));};

    BOOL           MatchOnLineEnd  (VOID);

    BOOL           IsError         (VOID)                     {return (bParsingError);};

    VOID           SetNextOne      (INT  iNextIn)             {iNextOne = iNextIn;};

    VOID           SetNextTwo      (INT  iNextIn)             {iNextTwo = iNextIn;};

    INT            GetNextOne      (VOID)                     {return (iNextOne);};

    INT            GetNextTwo      (VOID)                     {return (iNextTwo);};

    VOID           SetNext         (INT  iNextOneIn,
                                    INT  iNextTwoIn)          {iNextOne = iNextOneIn; iNextTwo = iNextTwoIn;};


    VOID           SetMatchMin     (INT  iMinIn)              {iMinMatches = iMinIn;};

    VOID           SetMatchMax     (INT  iMaxIn)              {iMaxMatches = iMaxIn;};

    INT            GetMatchMin     (VOID)                     {return iMinMatches;};

    INT            GetMatchMax     (VOID)                     {return iMaxMatches;};

    VOID           SetNumMatches   (INT  iMinIn,
                                    INT  iMaxIn = -1)         {iMinMatches = iMinIn; iMaxMatches = iMaxIn;};

    VOID           ClearNumMatches (VOID)                     {iNumMatches = 0;};

    INT            GetNumMatches   (VOID)                     {return iNumMatches;};

    VOID           IncNumMatches   (VOID)                     {++iNumMatches;};

    BOOL           GetType         (const char *   pszPatternIn,
                                    BOOL           bInList,
                                    INT &          iSizeMatchedOut);


    const char *   GetTypeText     (VOID);

  };



//------------------------------------------------------------------------
class RegExDeque
  {
  private:

    IntArray   aiValues;
    IntArray   aiNext;
    IntArray   aiPrev;

    INT         iPool;
    INT         iHead;
    INT         iTail;

    INT         iSize;

  private:

    VOID  Grow          (INT   iGrowSizeIn);

    VOID  InitSentinels (VOID);

    INT   GetFromPool   (VOID);

    VOID  ReturnToPool  (INT   iIndexIn);


  public:

          RegExDeque  ();
          ~RegExDeque ();

    VOID  Push         (INT  iIn);
    INT   Pop          (VOID);
    VOID  Put          (INT  iIn);

    INT   Size         (VOID)    {return (iSize - 2);}; // ignore head/tail sentinels

    VOID  Clear        (VOID);

    BOOL  IsEmpty      (VOID)    {return (aiNext [iHead] == iTail);};

    BOOL  InQueueHead  (INT  iIn);

    BOOL  InQueueTail  (INT  iIn);

    VOID  DebugPrint   (VOID);


  };


//------------------------------------------------------------------------
class RegEx
  {
  public:

    enum EParser {kRegEx = 0,
                  kGlob  = 1} ;

  public:
    TArray<RegExMatch> arrayStates;
    RegExDeque         deque;

    RStr               strPattern;
    UINT32             iPatternPos;
    INT32              iState;

    BOOL               bParsingError;
    INT                iNumParenthesis;


  private:

    INT   RegExExpression   (BOOL  bNegativeMatchIn = false);

    INT   RegExTerm         (BOOL  bNegativeMatchIn);

    INT   RegExFactor       (BOOL  bNegativeMatchIn);

    VOID  ParseRepetition   (RegExMatch &  matchIn);

    BOOL  ParseList         (INT &   iBranchTwo,
                             BOOL &  bParsingError);


    INT   RegExListInverted (VOID);

    INT   RegExListItemInverted (VOID);

    INT   RegExList         (VOID);

    INT   RegExListItem     (INT   iListStartState);

  public:


             RegEx      ();

             ~RegEx     ();

             RegEx      (const RegEx &  reIn);

    explicit RegEx      (const RStr &  strExprIn,
                         EParser       eParseType = kRegEx);

    explicit RegEx      (const char *  pszExprIn,
                         EParser       eParseType = kRegEx);

    VOID     Init       (VOID);

    EStatus  Set        (const RStr &  strExprIn,
                         EParser       eParseType = kRegEx);

    BOOL     HasMatch   (const char *  szSourceIn,
                         INT32         iSearchStartIn = 0);


    RStr     Match      (const RStr &  strSourceIn,
                         INT32         iSearchStartIn = 0,
                         RStrArray *   pstraSubstringsOut = NULL);

    VOID     Match      (const char *    szSourceIn,
                         INT32           iSourceLengthIn,
                         INT32           iSearchStartIn,
                         const char * *  szMatchStartOut,
                         INT32 &         iMatchSizeOut,
                         RStrArray *     pstraSubstringsOut = NULL);

    RStr     Substitute (const RStr &    strSourceIn,
                         const RStr &    strReplaceIn);


    EStatus  ParseRegEx (const RStr &  strExprIn);

    EStatus  ParseGlob  (const RStr &  strExprIn);

    VOID     Debug      (VOID);

    const RStr &  Pattern    (VOID) const       {return (strPattern);};


  };

RStr  operator*  (const RStr &   strIn,   RegEx &  regexIn);
RStr  operator*  (RegEx &  regexIn, const RStr &   strIn);


/** @} */ // end of base group


#endif // REGEX_HPP
