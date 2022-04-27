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

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Util/RegEx.hpp"
#include "Util/RStr.hpp"
#include <cctype>
#include <cstdlib>
//#include <cstdio>
#include <cstring>


// NOTE: This class needs to be refactored.  Currently the compiled state and
//  the state variables that change during scanning are intermingled.  Ideally
//  Compilation and Execution and their respective variables should be separate.
//  The ultimate goal is to pass a RegEx reference to operator* as a const,
//  which requires Match() to be const.  This should improve performance.



// NOTE: The states are built such that each starts with a null, which directs the
//  flow into the rest of the graph that handles that state.  Each term exits into
//  the state after its graph.  This allows us to place them one after another to
//  chain them, but we can also redirect the first term to set up loops, without
//  needing to insert additional nodes before those already placed in the list.


RStr  operator*  (const RStr &    strIn,  RegEx &  regexIn)  {return (regexIn.Match (strIn));};
RStr  operator*  (RegEx &  regexIn, const RStr &    strIn)   {return (regexIn.Match (strIn));};

#define DEBUGMSG
//#define DEBUGMSG_MATCH
//#define DEBUGMSG_PARSE

//------------------------------------------------------------------------
// RegExMatch
//------------------------------------------------------------------------

//------------------------------------------------------------------------
RegExMatch::RegExMatch  ()
  {
  eMatchType    = kChar;
  uStartMatch   = 0x20;
  uEndMatch     = 0x20;

  bInvertMatch  = FALSE;
  iNextOne      = 0;
  iNextTwo      = 0;

  iMinMatches   = -1;
  iMaxMatches   = -1;
  iNumMatches   = 0;

  bParsingError = FALSE;
  //DBG_INFO ("RegExMatch Blank Constructor %d", eMatchType);
  };


//------------------------------------------------------------------------
RegExMatch::RegExMatch  (EType  eMatchTypeIn,
                         UINT   uMatchStartIn,
                         UINT   uMatchEndIn,
                         INT    iNextOneIn,
                         INT    iNextTwoIn,
                         BOOL   bInvertMatchIn)
  {
  eMatchType    = eMatchTypeIn;
  uStartMatch   = uMatchStartIn;
  uEndMatch     = uMatchEndIn;
  iNextOne      = iNextOneIn;
  iNextTwo      = iNextTwoIn;
  bInvertMatch  = bInvertMatchIn;
  bParsingError = FALSE;
  iMinMatches   = -1;
  iMaxMatches   = -1;
  iNumMatches   = 0;

  //DBG_INFO ("RegExMatch Set Constructor %d %d", eMatchType, eMatchTypeIn);
  };


//------------------------------------------------------------------------
RegExMatch::RegExMatch (const RegExMatch &  matchIn)
  {
  eMatchType    = matchIn.eMatchType;
  uStartMatch   = matchIn.uStartMatch;
  uEndMatch     = matchIn.uEndMatch;

  bInvertMatch  = matchIn.bInvertMatch;
  iNextOne      = matchIn.iNextOne;
  iNextTwo      = matchIn.iNextTwo;

  iMinMatches   = matchIn.iMinMatches;
  iMaxMatches   = matchIn.iMaxMatches;
  iNumMatches   = matchIn.iNumMatches;

  bParsingError = matchIn.bParsingError;
  //DBG_INFO ("RegExMatch Copy Constructor %d", eMatchType, matchIn.eMatchType);
  };

//------------------------------------------------------------------------
RegExMatch &  RegExMatch::operator=   (const RegExMatch &  remIn)
  {
  // NOTE:  This function and the constructors will be called repeatedly.
  //   The matches are stored in an array that requires reallocation and
  //   copying as the array expands.  This means these calls need to remain
  //   fast and without any dynamic allocation, as it is currently implemented.

  eMatchType    = remIn.eMatchType;
  uStartMatch   = remIn.uStartMatch;
  uEndMatch     = remIn.uEndMatch;
  bInvertMatch  = remIn.bInvertMatch;
  iNextOne      = remIn.iNextOne;
  iNextTwo      = remIn.iNextTwo;
  iMinMatches   = remIn.iMinMatches;
  iMaxMatches   = remIn.iMaxMatches;
  iNumMatches   = remIn.iNumMatches;
  bParsingError = remIn.bParsingError;
  //DBG_INFO ("RegExMatch operator = %d %d  (%x)", eMatchType, remIn.eMatchType, this);
  return *this;
  };

//------------------------------------------------------------------------
BOOL  RegExMatch::IsMatch   (INT           iMatchState,
                             const char *  pchIn,
                             const char *  pchBufferStart,
                             const char *  pchBufferEnd)
  {
  // NOTE:  This performs no allocation.  Should be better unit tested.

  BOOL  bReturn = RawMatch (pchIn, pchBufferStart, pchBufferEnd);

  if (bInvertMatch)
    {
    bReturn = ! bReturn;
    };

  #ifdef DEBUGMSG_MATCH
    DBG_INFO ("  IsMatch %d looking at match type %s starting with %c against%s character \'%c\' %s", iMatchState, GetTypeText (), GetStartMatch (), bInvertMatch ? " (inverted)" : "", *pchIn, bReturn ? "True" : "False");
  #endif

  return (bReturn);
  };


//------------------------------------------------------------------------
BOOL  RegExMatch::RawMatch   (const char *  pchIn,
                              const char *  pchBufferStart,
                              const char *  pchBufferEnd)
  {
  // NOTE:  This performs no allocation.  Should be better unit tested.

  if (pchIn == pchBufferEnd)
    {
    // end of buffer.  Only line end matches
    return (eMatchType == kLineEnd);
    };

  switch (eMatchType)
    {
    case kAnyChar:      {return TRUE;};
    case kChar:         {
                         #ifdef DEBUGMSG_MATCH
                         //DBG_INFO ("  matching \'%c\' on char \'%c\' is %s", UINT (*pchIn), uStartMatch, (UINT (*pchIn) == uStartMatch) ? "True" : "False");
                         #endif
                         return (UINT (*pchIn) == uStartMatch);};
    case kCharRange:    {return ((UINT (*pchIn) >= uStartMatch) &&
                                 (UINT (*pchIn) <= uEndMatch));};
    case kAlnum:        {return (isalnum  (int (*pchIn)));};
    case kAlpha:        {return (isalpha  (int (*pchIn)));};
    case kBlank:        {return ((*pchIn == ' ') || (*pchIn == '\t'));};
    case kCntrl:        {return (iscntrl  (int (*pchIn)));};
    case kDigits:       {return (isdigit  (int (*pchIn)));};
    case kGraph:        {return (isgraph  (int (*pchIn)));};
    case kLower:        {return (islower  (int (*pchIn)));};
    case kPrint:        {return (isprint  (int (*pchIn)));};
    case kPunct:        {return (ispunct  (int (*pchIn)));};
    case kSpace:        {return (isspace  (int (*pchIn)));};
    case kUpper:        {return (isupper  (int (*pchIn)));};
    case kXDigit:       {return (isxdigit (int (*pchIn)));};
    case kWordBoundary: {return ( (! IsWordChar (pchIn - 1) &&   IsWordChar (pchIn) ) ||
                                  (  IsWordChar (pchIn - 1) && ! IsWordChar (pchIn) ) );};


    case kWordInside:   {return (IsWordChar (pchIn - 1) && IsWordChar (pchIn));};
    case kWordBegin:    {return (! IsWordChar (pchIn - 1) &&   IsWordChar (pchIn) );};
    case kWordEnd:      {return (  IsWordChar (pchIn - 1) && ! IsWordChar (pchIn) );};
    case kWordChar:     {return (IsWordChar (pchIn));};
    case kNotWordChar:  {return (! IsWordChar (pchIn));};
    case kBufferStart:  {return (pchIn == pchBufferStart);};
    case kBufferEnd:    {return (*pchIn == '\0');};
    case kLineStart:    {return ((pchIn == pchBufferStart) || (*(pchIn - 1) == '\n'));};
    case kLineEnd:      {return ((*pchIn == '\0') || (*pchIn == '\n'));};
    default:            break;
    };

  return (FALSE);
  };


//------------------------------------------------------------------------
BOOL  RegExMatch::IncOnMatch  (VOID)
  {
  if (bInvertMatch)
    {
    // negative matches occur in a list.  We want to process these as a non-incrementing
    // sequence of character matches, rather than a branching tree of matches.
    return (FALSE);
    }

  switch (eMatchType)
    {
    case kAnyChar:
    case kChar:
    case kCharRange:
    case kAlnum:
    case kAlpha:
    case kBlank:
    case kCntrl:
    case kDigits:
    case kGraph:
    case kLower:
    case kPrint:
    case kPunct:
    case kSpace:
    case kUpper:
    case kXDigit:
         return (TRUE);

    case kWordBoundary:
    case kWordInside:
    case kWordBegin:
    case kWordEnd:
    case kWordChar:
    case kNotWordChar:
    case kBufferStart:
    case kBufferEnd:
    case kLineStart:
    case kLineEnd:
         return (FALSE);

    default:
         break;
    };

  return (TRUE);
  };


//------------------------------------------------------------------------
BOOL  RegExMatch::IsWordChar  (const char *  pchIn)
  {
  // returns true if the character is word-constituent, false if not.

  // Although some GNU RegEx libraries allow one to define a syntax table
  //   that defines what is part of a word and what isn't I am hard coding
  //   to the default value.  Letters, numbers, and the underscore are
  //   word-constituent characters.

  if (isalpha (int (*pchIn)) ||
      isdigit (int (*pchIn)) ||
      (*pchIn == '_'))
    {
    return (TRUE);
    };
  return (FALSE);
  };

//------------------------------------------------------------------------
BOOL  RegExMatch::MatchOnLineEnd  (VOID)
  {
  return ((eMatchType == kLineEnd) ||
          (eMatchType == kWordEnd) ||
          (eMatchType == kWordBoundary));
  };

//------------------------------------------------------------------------
BOOL  RegExMatch::GetType  (const char *   pszPatternIn,
                            BOOL           bInList,
                            INT &          iSizeMatchedOut)
  {
  // NOTE:  This performs no allocation.  Should be better unit tested.

  // try to match each type

  eMatchType = kNull;

  #ifdef DEBUGMSG
    //DBG_INFO ("GetType %s ", pszPatternIn);
  #endif

  // Make sure you are within a list bracket to match the following

  if (bInList)
    {
         if (strncmp (pszPatternIn, "[:alnum:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kAlnum; }
    else if (strncmp (pszPatternIn, "[:alpha:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kAlpha; }
    else if (strncmp (pszPatternIn, "[:blank:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kBlank; }
    else if (strncmp (pszPatternIn, "[:cntrl:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kCntrl; }
    else if (strncmp (pszPatternIn, "[:digits:]", 10) == 0)  {iSizeMatchedOut = 10; eMatchType = kDigits;}
    else if (strncmp (pszPatternIn, "[:graph:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kGraph; }
    else if (strncmp (pszPatternIn, "[:lower:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kLower; }
    else if (strncmp (pszPatternIn, "[:print:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kPrint; }
    else if (strncmp (pszPatternIn, "[:punct:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kPunct; }
    else if (strncmp (pszPatternIn, "[:space:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kSpace; }
    else if (strncmp (pszPatternIn, "[:upper:]",   9) == 0)  {iSizeMatchedOut =  9; eMatchType = kUpper; }
    else if (strncmp (pszPatternIn, "[:xdigit:]", 10) == 0)  {iSizeMatchedOut = 10; eMatchType = kXDigit;}

    // find chars that are valid in a list, but not outside of it.
    else if ((*pszPatternIn == '|') ||
             (*pszPatternIn == '{') ||
             (*pszPatternIn == '}') ||
             (*pszPatternIn == '?') ||
             (*pszPatternIn == '+') ||
             (*pszPatternIn == '*'))
      {
      iSizeMatchedOut = 1;
      uStartMatch     = *(pszPatternIn);
      eMatchType      = kChar;
      }
    else if (isalnum (*pszPatternIn) &&
             (*(pszPatternIn + 1) == '-') &&
             isalnum (*(pszPatternIn + 2)))
      {
      uStartMatch = *(pszPatternIn);
      uEndMatch   = *(pszPatternIn + 2);
      if (uStartMatch > uEndMatch)
        {
        // range values are out-of-order, larger to smaller.
        bParsingError = TRUE;
        };
      iSizeMatchedOut = 3;
      eMatchType      = kCharRange;
      };
    }
  else
    {
    // not in list.
    };

  // check the chars that appear inside or outside of lists

  if (*pszPatternIn == '.')  {iSizeMatchedOut = 1; eMatchType = kAnyChar;}

  else if (strncmp (pszPatternIn, "\\b", 2) == 0)  {iSizeMatchedOut = 2; eMatchType = kWordBoundary;}
  else if (strncmp (pszPatternIn, "\\B", 2) == 0)  {iSizeMatchedOut = 2; eMatchType = kWordInside  ;}
  else if (strncmp (pszPatternIn, "\\<", 2) == 0)  {iSizeMatchedOut = 2; eMatchType = kWordBegin   ;}
  else if (strncmp (pszPatternIn, "\\>", 2) == 0)  {iSizeMatchedOut = 2; eMatchType = kWordEnd     ;}
  else if (strncmp (pszPatternIn, "\\w", 2) == 0)  {iSizeMatchedOut = 2; eMatchType = kWordChar    ;}
  else if (strncmp (pszPatternIn, "\\W", 2) == 0)  {iSizeMatchedOut = 2; eMatchType = kNotWordChar ;}
  else if (strncmp (pszPatternIn, "\\`", 2) == 0)  {iSizeMatchedOut = 2; eMatchType = kBufferStart ;}
  else if (strncmp (pszPatternIn, "\\'", 2) == 0)  {iSizeMatchedOut = 2; eMatchType = kBufferEnd   ;}
  else if (*pszPatternIn == '\\')
    {

    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("Escaping character \'%c\'", *(pszPatternIn + 1));
    #endif

    // escaped char
    iSizeMatchedOut = 2;
    uStartMatch     = *(pszPatternIn + 1);
    eMatchType      = kChar;
    }


  else if (strncmp (pszPatternIn, "^",   1) == 0)  {iSizeMatchedOut = 1; eMatchType = kLineStart   ;}
  else if (strncmp (pszPatternIn, "$",   1) == 0)  {iSizeMatchedOut = 1; eMatchType = kLineEnd     ;}

  else if ((*pszPatternIn != '(') &&
           (*pszPatternIn != ')') &&
           (*pszPatternIn != '|') &&
           (*pszPatternIn != '[') &&
           (*pszPatternIn != ']') &&
           (*pszPatternIn != '{') &&
           (*pszPatternIn != '}') &&
           (*pszPatternIn != '?') &&
           (*pszPatternIn != '+') &&
           (*pszPatternIn != '*') &&
           (eMatchType == kNull))
    {
    iSizeMatchedOut = 1;
    uStartMatch     = *(pszPatternIn);
    eMatchType      = kChar;
    };

  #ifdef DEBUGMSG_PARSE
    DBG_INFO ("eMatchType is type \'%s\' for char %c pattern %c ", GetTypeText(), uStartMatch, pszPatternIn[0]);
  #endif

  return (eMatchType != kNull);
  };


//------------------------------------------------------------------------
const char *  RegExMatch::GetTypeText  (VOID)
  {

  switch (eMatchType)
    {
    case kNull:          return "Null";
    case kOr:            return "Or";
    case kRepeat:        return "Repeat";
    case kAnyChar:       return (bInvertMatch) ? "NotAnyChar" : "AnyChar";
    case kChar:          return (bInvertMatch) ? "NotChar" : "Char";
    case kCharRange:     return (bInvertMatch) ? "NotCharRange" : "CharRange";
    case kAlnum:         return (bInvertMatch) ? "NotAlnum" : "Alnum";
    case kAlpha:         return (bInvertMatch) ? "NotAlpha" : "Alpha";
    case kBlank:         return (bInvertMatch) ? "NotBlank" : "Blank";
    case kCntrl:         return (bInvertMatch) ? "NotCntrl" : "Cntrl";
    case kDigits:        return (bInvertMatch) ? "NotDigits" : "Digits";
    case kGraph:         return (bInvertMatch) ? "NotGraph" : "Graph";
    case kLower:         return (bInvertMatch) ? "NotLower" : "Lower";
    case kPrint:         return (bInvertMatch) ? "NotPrint" : "Print";
    case kPunct:         return (bInvertMatch) ? "NotPunct" : "Punct";
    case kSpace:         return (bInvertMatch) ? "NotSpace" : "Space";
    case kUpper:         return (bInvertMatch) ? "NotUpper" : "Upper";
    case kXDigit:        return (bInvertMatch) ? "NotXDigit" : "XDigit";
    case kWordBoundary:  return "WordBoundary";
    case kWordInside:    return "WordInside";
    case kWordBegin:     return "WordBegin";
    case kWordEnd:       return "WordEnd";
    case kWordChar:      return "WordChar";
    case kNotWordChar:   return "NotWordChar";
    case kBufferStart:   return "BufferStart";
    case kBufferEnd:     return "BufferEnd";
    case kLineStart:     return "LineStart";
    case kLineEnd:       return "LineEnd";
    default:
         break;
    };

  return ("");
  };




//------------------------------------------------------------------------
// RegExDeque
//------------------------------------------------------------------------

// Note:  This is a bit of a hack job, but should be faster to implement
//          than a full deque class.  Essentially instead of allocating
//          elements that contain an index, a next pointer, and a prev pointer,
//          I'm using three parallel arrays.  Additionally I'm setting the
//          first three values as sentinels for the queue head, queue tail,
//          and pool of unallocated entries.  Since there will be a lot of
//          allocation and deallocation, a pool is useful for speed.

// NOTE:  This system needs to be separately unit tested.  I think it
//          is correct, but have no guarantees. - mduffy

//------------------------------------------------------------------------
RegExDeque::RegExDeque ()
  {
  // clearing will initialize everything.
  Clear ();
  };


//------------------------------------------------------------------------
RegExDeque::~RegExDeque ()
  {
  };


//------------------------------------------------------------------------
VOID  RegExDeque::InitSentinels (VOID)
  {
  // Allocate and initialize the head and tail sentinels
  iHead = GetFromPool ();
  iTail = GetFromPool ();

  aiNext [iHead] = iTail;
  aiPrev [iHead] = iTail;

  aiNext [iTail] = iHead;
  aiPrev [iTail] = iHead;
  };


//------------------------------------------------------------------------
VOID  RegExDeque::Grow  (INT   iGrowSizeIn)
  {

  INT  iNewStart = aiValues.Length ();

  // allocate new entries

  aiValues.Insert (iNewStart, iGrowSizeIn);
  aiNext.  Insert (iNewStart, iGrowSizeIn);
  aiPrev.  Insert (iNewStart, iGrowSizeIn);

  // setup the Next pointer to link all the allocations together
  for (INT  iIndex = 0; iIndex < iGrowSizeIn; ++iIndex)
    {
    aiNext [iNewStart + iIndex] = iNewStart + iIndex + 1;
    };

  // add the new allocations to the pool
  aiNext [iNewStart + iGrowSizeIn - 1] = iPool;
  iPool = iNewStart;
  };


//------------------------------------------------------------------------
INT  RegExDeque::GetFromPool (VOID)
  {
  if (iPool == -1)
    {
    // empty pool.  Allocate more entries

    Grow (50);
    };

  INT   iReturn = iPool;
  iPool = aiNext [iPool];
  ++iSize;

  return (iReturn);
  };


//------------------------------------------------------------------------
VOID  RegExDeque::ReturnToPool (INT   iIndexIn)
  {
  aiNext [iIndexIn] = iPool;
  iPool = iIndexIn;
  --iSize;
  };


//------------------------------------------------------------------------
VOID  RegExDeque::Push  (INT  iIn)
  {
  // push to head of queue

  INT  iNew = GetFromPool ();

  aiValues [iNew] = iIn;

  aiNext [iNew] = aiNext [iHead];
  aiPrev [iNew] = iHead;

  aiNext [iHead] = iNew;
  aiPrev [aiNext [iNew]] = iNew;
  };


//------------------------------------------------------------------------
INT   RegExDeque::Pop  (VOID)
  {
  // pop from head of queue

  INT  iReturnValue;
  INT  iPopIndex;

  if (IsEmpty ())
    {
    // empty queue
    return (-1);
    }

  iPopIndex = aiNext [iHead];

  aiPrev [aiNext [iPopIndex]] = iHead;
  aiNext [iHead] = aiNext [iPopIndex];

  iReturnValue = aiValues [iPopIndex];

  ReturnToPool (iPopIndex);

  return (iReturnValue);
  };


//------------------------------------------------------------------------
VOID  RegExDeque::Put  (INT  iIn)
  {
  // put to tail of queue

  INT  iNew = GetFromPool ();

  aiValues [iNew] = iIn;

  aiNext [iNew] = iTail;
  aiPrev [iNew] = aiPrev [iTail];

  aiPrev [iTail] = iNew;
  aiNext [aiPrev [iNew]] = iNew;
  };


//------------------------------------------------------------------------
VOID  RegExDeque::Clear (VOID)
  {
  // force the return of all entries to the pool

  INT  iNumEntries = aiValues.Length ();
  iSize = 0;

  // setup the Next pointer to link all the allocations together
  for (INT  iIndex = 0; iIndex < iNumEntries; ++iIndex)
    {
    aiNext [iIndex] = iIndex + 1;
    };

  if (iNumEntries > 0)
    {
    iPool = 0;
    aiNext [iNumEntries - 1] = -1;
    }
  else
    {
    iPool = -1;
    };

  InitSentinels ();
  };


//------------------------------------------------------------------------
BOOL  RegExDeque::InQueueHead  (INT  iIn)
  {
  INT   iCurrIndex = aiNext [iHead];

  while ((iCurrIndex != iTail) && (aiValues [iCurrIndex] != -1))
    {
    if (aiValues [iCurrIndex] == iIn)
      {
      return (TRUE);
      };
    iCurrIndex = aiNext [iCurrIndex];
    };
  return (FALSE);
  };


//------------------------------------------------------------------------
BOOL  RegExDeque::InQueueTail  (INT  iIn)
  {
  INT   iCurrIndex = aiPrev [iTail];

  while ((iCurrIndex != iHead) && (aiValues [iCurrIndex] != -1))
    {
    if (aiValues [iCurrIndex] == iIn)
      {
      return (TRUE);
      };
    iCurrIndex = aiPrev [iCurrIndex];
    };
  return (FALSE);
  };

//------------------------------------------------------------------------
VOID  RegExDeque::DebugPrint  (VOID)
  {
  INT   iCurrIndex = aiNext [iHead];

  while (iCurrIndex != iTail)
    {
    DBG_INFO (" %d", aiValues [iCurrIndex]);
    iCurrIndex = aiNext [iCurrIndex];
    };
  };



//------------------------------------------------------------------------
// RegEx
//------------------------------------------------------------------------

//------------------------------------------------------------------------
RegEx::RegEx  ()
  {
  Init ();
  };


//------------------------------------------------------------------------
RegEx::~RegEx  ()
  {
  };

//------------------------------------------------------------------------
RegEx::RegEx  (const RegEx &  reIn)
  {
  Init ();
  arrayStates.Copy (reIn.arrayStates);
  strPattern  = reIn.strPattern;
  iPatternPos = reIn.iPatternPos;
  iState      = reIn.iState;
  arrayStates.SetAutoGrow (TRUE);
  };

//------------------------------------------------------------------------
RegEx::RegEx  (const RStr &  strExprIn,
               EParser       eParseType)
  {
  Init ();
  Set (strExprIn, eParseType);
  };

//------------------------------------------------------------------------
RegEx::RegEx  (const char *  pszExprIn,
               EParser       eParseType)
  {
  Init ();
  Set (RStr (pszExprIn), eParseType);
  };

//------------------------------------------------------------------------
VOID  RegEx::Init  (VOID)
  {
  strPattern.Set ("");
  iPatternPos     = 0;
  iState          = 0;
  bParsingError   = FALSE;
  iNumParenthesis = 0;
  arrayStates.SetAutoGrow (TRUE);
  };

//------------------------------------------------------------------------
EStatus  RegEx::Set  (const RStr &   strExprIn,
                      EParser        eParseType)
  {
  switch (eParseType)
    {
    case kRegEx:  return ParseRegEx (strExprIn);
    case kGlob:   return ParseGlob  (strExprIn);
    };
  return (EStatus::kFailure);
  };

//------------------------------------------------------------------------
RStr  RegEx::Match  (const RStr &  strSourceIn,
                     INT32         iSearchStartIn,
                     RStrArray *   pstraSubstringsOut)
  {
  RStr         strReturn("");
  const char * pszMatch;
  INT32        iMatchLength;

  Match (strSourceIn.AsChar(),
         strSourceIn.Length(),
         iSearchStartIn,
         &pszMatch,
         iMatchLength,
         pstraSubstringsOut);
  if (iMatchLength > 0)
    {
    strReturn.AppendChars (pszMatch, iMatchLength);
    };
  return (strReturn);
  };

//------------------------------------------------------------------------
BOOL  RegEx::HasMatch  (const char *  szSourceIn,
                        INT32         iSearchStartIn)
  {
  const char * pszMatch;
  INT32        iMatchLength;

  Match (szSourceIn,
         INT32 (strlen (szSourceIn)),
         iSearchStartIn,
         &pszMatch,
         iMatchLength,
         NULL);

  return (iMatchLength > 0);
  };

//------------------------------------------------------------------------
VOID  RegEx::Match  (const char *    szSourceIn,
                     INT32           iSourceLengthIn,
                     INT32           iSearchStartIn,
                     const char * *  szMatchStartOut,
                     INT32 &         iMatchSizeOut,
                     RStrArray *     pstraSubstringsOut)
  {
  INT32         iStartMatch   = 0;
  INT32         iMaxMatch     = iSourceLengthIn;
  *szMatchStartOut = NULL;
  iMatchSizeOut    = 0;

  #ifdef DEBUGMSG_MATCH
    DBG_INFO ("");
  #endif


  if (strPattern.Length () == 0) return;

  for (iStartMatch = iSearchStartIn; iStartMatch <= iMaxMatch; ++iStartMatch)
    {
    INT           iLongestMatchPos = -1;
    #ifdef DEBUGMSG_MATCH
      //const char *  pszStartMatch = &szSourceIn[iStartMatch];
    #endif
    const INT     iScan = -1;
    INT32         iPos  = iStartMatch;

    #ifdef DEBUGMSG_MATCH
      DBG_INFO ("----- Starting new search from position %d on string \"%s\"", iPos, szSourceIn);
    #endif


    iPatternPos = 0;

    // clear the repeat flags
    for (UINT32  uIndex = 0; uIndex < UINT32 (arrayStates.Length ()); ++uIndex)
      {
      arrayStates [uIndex].ClearNumMatches ();
      };


    deque.Clear ();
    deque.Put (iScan);

    INT iMatchState = arrayStates [0].GetNextOne ();

    do
      {
      #ifdef DEBUGMSG_MATCH
        if (iMatchState >= 0)
          {
          DBG_INFO ("Considiering Match State %d (%s) %d %d",
                  iMatchState,
                  arrayStates [iMatchState].GetTypeText (),
                  arrayStates [iMatchState].GetNextOne (),
                  arrayStates [iMatchState].GetNextTwo ());
          };
      #endif

      if (iMatchState == iScan)
        {
        #ifdef DEBUGMSG_MATCH
          DBG_INFO ("Rex: Found scan code.  Moving to next string character '%c'", szSourceIn[iPos+1]);
        #endif
        // We read the scan value off the queue.  move to next character
        iPos = iPos + 1;
        deque.Put (iScan);
        }
      else if ((arrayStates [iMatchState].IsNull ()) && (iMatchState != 0))
        {
        #ifdef DEBUGMSG_MATCH
          DBG_INFO ("  Rex: isNull %d", iMatchState);
        #endif
        if (! deque.InQueueHead (arrayStates [iMatchState].GetNextOne ()))
          {
          #ifdef DEBUGMSG_MATCH
            DBG_INFO ("  Rex: push state %d to deque", arrayStates [iMatchState].GetNextOne ());
          #endif
          deque.Push (arrayStates [iMatchState].GetNextOne ());
          };
        }
      else if (arrayStates [iMatchState].IsOr ())
        {
        #ifdef DEBUGMSG_MATCH
          DBG_INFO ("  Rex: isOr %d", iMatchState);
        #endif
        if (! deque.InQueueHead (arrayStates [iMatchState].GetNextOne ()))
          {
          #ifdef DEBUGMSG_MATCH
            DBG_INFO ("  Rex: push state %d to deque", arrayStates [iMatchState].GetNextOne ());
          #endif
          deque.Push (arrayStates [iMatchState].GetNextOne ());
          };

        if (! deque.InQueueHead (arrayStates [iMatchState].GetNextTwo ()))
          {
          #ifdef DEBUGMSG_MATCH
            DBG_INFO ("  Rex: push state %d to deque", arrayStates [iMatchState].GetNextTwo ());
          #endif
          deque.Push (arrayStates [iMatchState].GetNextTwo ());
          };
        }
      else if (arrayStates [iMatchState].IsRepeat ())
        {
        // The first branch is the continue state.  The second is the repeat state.

        // Note: Check for iterations here.  To be implemented.
        // Note:  How do we handle zero matches?


        // check curr match against min and max.
        //  If less than min, we must only repeat.
        //  If greater than max, we must fail and not add further checks.
        //  If between min and max inclusive, we add the continue state.

        INT  iNumMatches = arrayStates [iMatchState].GetNumMatches ();
        INT  iMin        = arrayStates [iMatchState].GetMatchMin ();
        INT  iMax        = arrayStates [iMatchState].GetMatchMax ();

        #ifdef DEBUGMSG_MATCH
          DBG_INFO ("  Rex: isRepeat %d NumMatches:%d Min:%d Max:%d", iMatchState, iNumMatches, iMin, iMax);
        #endif

        if (iNumMatches < iMin)
          {
          if (! deque.InQueueHead (arrayStates [iMatchState].GetNextTwo ()))
            {
            #ifdef DEBUGMSG_MATCH
              DBG_INFO ("  Rex: push state %d to deque", arrayStates [iMatchState].GetNextTwo ());
            #endif
            deque.Push (arrayStates [iMatchState].GetNextTwo ());
            };
          }
        else if ((iNumMatches > iMax) && (iMax != -1))
          {
          // do nothing
          }
        else
          {
          if (! deque.InQueueHead (arrayStates [iMatchState].GetNextOne ()))
            {
            #ifdef DEBUGMSG_MATCH
              DBG_INFO ("  Rex: push state %d to deque", arrayStates [iMatchState].GetNextOne ());
            #endif
            deque.Push (arrayStates [iMatchState].GetNextOne ());
            };
          if (! deque.InQueueHead (arrayStates [iMatchState].GetNextTwo ()))
            {
            #ifdef DEBUGMSG_MATCH
              DBG_INFO ("  Rex: push state %d to deque", arrayStates [iMatchState].GetNextTwo ());
            #endif
            deque.Push (arrayStates [iMatchState].GetNextTwo ());
            };
          };

        arrayStates [iMatchState].IncNumMatches ();
        }
      else if (((iPos == iMaxMatch) && (arrayStates [iMatchState].MatchOnLineEnd ())) ||
               ((iPos < iMaxMatch)  && (arrayStates [iMatchState].IsMatch (iMatchState, &szSourceIn[iPos], szSourceIn, &szSourceIn[iSourceLengthIn]))))
        {
        #ifdef DEBUGMSG_MATCH
        if (iPos < iMaxMatch)
          {
          if (arrayStates[iMatchState].IsCharRange ())
            {
            DBG_INFO ("  Rex: isMatch  \'%c\' MatchesRange:\'%c-%c\' %s",
                  (int)szSourceIn[iPos],
                  (int)arrayStates [iMatchState].GetStartMatch (),
                  (int)arrayStates [iMatchState].GetEndMatch (),
                  arrayStates [iMatchState].GetTypeText ());

            }
          else
            {
            DBG_INFO ("  Rex: isMatch  \'%c\' Matches:\'%c\' %s",
                  (int)szSourceIn[iPos],
                  (int)arrayStates [iMatchState].GetStartMatch (),
                  arrayStates [iMatchState].GetTypeText ());
            };
          DBG_INFO ("  ->%s", &szSourceIn[iPos]);
          }
        #endif

        if (arrayStates [iMatchState].IncOnMatch ())
          {
          if (! deque.InQueueTail (arrayStates [iMatchState].GetNextOne ()))
            {
            #ifdef DEBUGMSG_MATCH
              DBG_INFO ("  Rex: put state %d to deque", arrayStates [iMatchState].GetNextOne ());
            #endif
            deque.Put (arrayStates [iMatchState].GetNextOne ());
            };
          }
        else
          {
          if (! deque.InQueueHead (arrayStates [iMatchState].GetNextOne ()))
            {
            #ifdef DEBUGMSG_MATCH
              DBG_INFO ("  Rex: push state %d to deque", arrayStates [iMatchState].GetNextOne ());
            #endif
            deque.Push (arrayStates [iMatchState].GetNextOne ());
            };
          };
        }
      iMatchState = deque.Pop ();

      if (iMatchState == 0)
        {
        // found a match
        #ifdef DEBUGMSG_MATCH
          DBG_INFO ("=== found match at position %d", (int)iPos);
        #endif

        iLongestMatchPos = iPos > iLongestMatchPos ? iPos : iLongestMatchPos;
        iMatchState = deque.Pop ();
        };

      #ifdef DEBUGMSG_MATCH
        DBG_INFO ("  Looping to next state (%d).  iPos %d maxMatch %d.  DeQue Size %d (", iMatchState, (int)iPos, iMaxMatch, deque.Size ());
        deque.DebugPrint ();
        DBG_INFO (")");
      #endif

      } while ((iPos <= (iMaxMatch + 1)) && (! deque.IsEmpty ())); // (iMatchState != 0) &&

    if (iLongestMatchPos > 0)
      {
      // found a successful match.  This is the leftmost one we can find, so run with it.
      #ifdef DEBUGMSG_MATCH
        DBG_INFO ("Found match ending at pos %d, starting at position %d, char \'%c\'", (int)iLongestMatchPos, (int)iStartMatch, szSourceIn[iStartMatch]);
      #endif

      *szMatchStartOut = &szSourceIn[iStartMatch];
      iMatchSizeOut = iLongestMatchPos - iStartMatch; // start match is inclusive, longest match is exclusive
      //strSourceIn.GetMiddle (iStartMatch, iLongestMatchPos - iStartMatch, strReturn);
      break;
      };
    }; // for each char
  return;
  };


//------------------------------------------------------------------------
VOID  RegEx::Debug (VOID)
  {
  INT  iMax = arrayStates.Length ();

  DBG_INFO("\n");
  DBG_INFO("------------------- RegEx Debug -------------------");
  for (iState = 0; iState < iMax; ++iState)
    {
    int  iStartChar = arrayStates [iState].GetStartMatch ();
    if ((iStartChar < 32) || (iStartChar > 127)) {iStartChar = ' ';};

    int  iEndChar = arrayStates [iState].GetEndMatch ();
    if ((iEndChar < 32) || (iEndChar > 127)) {iEndChar = ' ';};


    DBG_INFO ("%4d %12s %3c%c%c%c%c %3d %3d", int (iState),
                                     arrayStates [iState].GetTypeText (),

                                     arrayStates[iState].IsCharOrRange () ? '\'' : ' ',
                                     char (iStartChar),
                                     arrayStates[iState].IsCharRange () ? '-' : arrayStates[iState].IsChar () ? '\'' : ' ',

                                     arrayStates[iState].IsCharRange () ? char (iEndChar) : ' ',
                                     arrayStates[iState].IsCharRange () ? '\'' : ' ',

                                     arrayStates [iState].GetNextOne (),
                                     arrayStates [iState].GetNextTwo ());
    };
  };


//------------------------------------------------------------------------
RStr  RegEx::Substitute  (const RStr &  strSourceIn,
                          const RStr &  strReplaceIn)
  {
  // to be implemented
  return (RStr (""));
  };


//------------------------------------------------------------------------
EStatus  RegEx::ParseRegEx (const RStr &  strExprIn)
  {
  #ifdef DEBUGMSG_PARSE
    DBG_INFO ("RegEx::ParseRegEx");
  #endif
  arrayStates.Clear ();

  iState = 1;
  iPatternPos = 0;
  strPattern.Reset();
  strPattern = strExprIn;
  bParsingError = FALSE;
  iNumParenthesis = 0;

  INT  iStartIndex = RegExExpression ();

  if ((iStartIndex == -1) || (bParsingError))
    {
    return (EStatus::kFailure);
    }

  // very first state.  May not be needed after rework.
  arrayStates [0] = RegExMatch  (RegExMatch::kNull, 0, 0, iStartIndex, -1);

  // the last state needs to be the null which signifies the end has been reached.
  arrayStates [iState] = RegExMatch  (RegExMatch::kNull, 0, 0, 0, 0);

  // TEMP
  //Debug ();
  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------
EStatus  RegEx::ParseGlob  (const RStr &  strExprIn)
  {
  #ifdef DEBUGMSG_PARSE
    DBG_INFO ("RegEx::ParseGlob");
  #endif
  arrayStates.Clear ();

  iState = 1;
  iPatternPos = 0;
  strPattern = strExprIn;


  arrayStates [0] = RegExMatch  (RegExMatch::kNull, 0, 0, iState, iState);

  UINT32  uPatternLength = strPattern.Length ();

  for (iPatternPos = 0; iPatternPos < uPatternLength; ++iPatternPos)
    {
    if (strPattern [iPatternPos] == '*')
      {
      RegExMatch    matchNew (RegExMatch::kRepeat, 0, 0, iState + 2, iState + 1);
      matchNew.SetNumMatches (0, -1);
      arrayStates [iState] = matchNew;
      ++iState;

      arrayStates [iState] = RegExMatch  (RegExMatch::kAnyChar, 0, 0, iState - 1, iState - 1);
      }
    else if (strPattern [iPatternPos] == '?')
      {
      arrayStates [iState] = RegExMatch  (RegExMatch::kAnyChar, 0, 0, iState + 1, iState + 1);
      }
    else if (strPattern [iPatternPos] == '\\')
      {
      ++iPatternPos;
      arrayStates [iState] = RegExMatch  (RegExMatch::kChar, strPattern [iPatternPos], 0, iState + 1, iState + 1);
      }
    else
      {
      arrayStates [iState] = RegExMatch  (RegExMatch::kChar, strPattern [iPatternPos], 0, iState + 1, iState + 1);
      };
    ++iState;
    };
  arrayStates [iState] = RegExMatch  (RegExMatch::kNull, 0, 0, 0, 0);
  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------
INT  RegEx::RegExExpression  (BOOL  bNegativeMatchIn)
  {
  // Expressions are one or more terms separated by alternations ('|' characters)

  INT   iBranchOne;
  INT   iReturn;

  // this routine returns the entrance into the expression.  The exits from the expression are all
  //  recursively set up.

  iBranchOne = RegExTerm (bNegativeMatchIn);
  iReturn    = iBranchOne;

  if (strPattern [iPatternPos] == '|')
    {
    // handle alternation
    INT   iBranchTwo;

    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("------------------- Alternation at offset %d -------------------", iPatternPos);
    #endif

    // reserve two spots on the array state array, at (iBranchTwo) and (iBranchTwo - 1)
    ++iPatternPos;
    ++iState;

    iReturn = iBranchTwo = iState;
    ++iState;

    INT  iLeft = RegExExpression (); // This can add additional stuff to arrayStates, as it parses the buffer.
    arrayStates [iBranchTwo]     = RegExMatch  (RegExMatch::kOr,   0, 0, iLeft, iBranchOne);

    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("Setting 'Or' at branch %d - (%d) %s", iBranchTwo, arrayStates [iBranchTwo].eMatchType, arrayStates [iBranchTwo].GetTypeText ());
    #endif

    arrayStates [iBranchTwo - 1] = RegExMatch  (RegExMatch::kNull, 0, 0, iState, -1); // must be called after above RegExExpression, because iState will change.
    // NOTE:  All of the kNull states will be set to go to the command after the alternation list.

    };
  return (iReturn);
  };


//------------------------------------------------------------------------
INT  RegEx::RegExTerm  (BOOL  bNegativeMatchIn)
  {
  // Terms are one or more Factors strung together

  INT  iReturn;
  INT  iNewReturn;

  // this routine returns the entrance into the expression.  The exits from the expression are all
  //  recursively set up.

  iReturn = RegExFactor (bNegativeMatchIn);

  // Factors are always set up so that the next state is the exit from this state.  So factors
  //  that exist one after the other should all line up correctly.

  while ((iNewReturn = RegExFactor (bNegativeMatchIn)) >= 0)
    {
    };

  // return the entrance of the first factor.
  return (iReturn);
  };


//------------------------------------------------------------------------
INT  RegEx::RegExFactor  (BOOL  bNegativeMatchIn)
  {
  // Factors are a single matchable unit that could have a repetition.

  INT   iBranchOne = iState;
  INT   iBranchTwo = -1;

  // this routine returns the entrance into the expression.  The exits from the expression are all
  //  recursively set up.

  #ifdef DEBUGMSG_PARSE
    DBG_INFO ("RegExFactor");
  #endif

  // handle groups
  if (strPattern [iPatternPos] == '(')
    {
    INT   iGroupStart = iState;
    arrayStates [iGroupStart] = RegExMatch  (RegExMatch::kNull, 0, 0, -1, -1);
    ++iState;

    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("RegExFactor: Found opening parenthesis at %d", iPatternPos);
    #endif
    ++iNumParenthesis;
    ++iPatternPos;
    // Note:  You need to handle beginning sub-strings here.  To be implemented
    iBranchTwo = RegExExpression (bNegativeMatchIn);
    arrayStates [iGroupStart].SetNext (iBranchTwo, -1);
    iBranchTwo = iGroupStart;

    // close group
    if (strPattern [iPatternPos] == ')')
      {
      #ifdef DEBUGMSG_PARSE
        DBG_INFO ("RegExFactor: Found closing parenthesis at %d", iPatternPos);
      #endif
      --iNumParenthesis;
      // Note:  You need to handle ending sub-strings here.  To be implemented
      ++iPatternPos;
      // end the group with a null.  Since several paths in the group could converge
      //   here, we need a null where we can redirect those paths into the next pattern.
      arrayStates [iState] = RegExMatch  (RegExMatch::kNull, 0, 0, iState + 1, -1);
      ++iState;
      }
    else
      {
      // Error.  Group not closed
      #ifdef DEBUGMSG_PARSE
        DBG_INFO ("RegExFactor returning %d", -1);
      #endif
      return (-1);
      };
    }
  // handle lists
  else if (strPattern [iPatternPos] == '[')
    {
    if (!ParseList (iBranchTwo, bParsingError))
      {
      // error
      #ifdef DEBUGMSG_PARSE
        DBG_INFO ("RegExFactor returning %d", -1);
      #endif
      return (-1);
      };
    } // end handle lists


  // we have a character to match if we are not closing a
  // group, or have a repetition character, or are at the
  // end of the string
  else if ( (strPattern [iPatternPos] != ')') &&
            (strPattern [iPatternPos] != '\0') &&
            (strPattern [iPatternPos] != '{') &&
            (strPattern [iPatternPos] != '?') &&
            (strPattern [iPatternPos] != '+') &&
            (strPattern [iPatternPos] != '*'))
    {
    // we have a character.

    INT           iMatchSize;
    RegExMatch    matchNew;

    if (matchNew.GetType (strPattern.AsChar (iPatternPos), false, iMatchSize))
      {
      if (matchNew.IsError ()) {bParsingError = TRUE;};
      // found a new character

      matchNew.SetNext (iState + 1, -1);
      matchNew.InvertMatch (bNegativeMatchIn);
      arrayStates [iState] = matchNew;

      iBranchTwo = iState;
      iPatternPos += iMatchSize;
      ++iState;
      }
    else
      {
      if (matchNew.IsError ()) {bParsingError = TRUE;};
      // error:  not a character, and there were no other options
      // Character might be part of an alternation, or otherwise not part of a factor.  Silently return.
      //DBG_ERROR ("RegEx Match is not a character.  Not sure what it is.");
      #ifdef DEBUGMSG_PARSE
        DBG_INFO ("RegExFactor returning %d", -1);
      #endif
      return (-1);
      };
    }
  else if ((strPattern [iPatternPos] == ')') && (iNumParenthesis == 0))
    {
    // unbalanced closing parenthesis
    bParsingError = TRUE;
    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("RegExFactor returning %d", -1);
    #endif
    return (-1);
    }
  else
    {
    // error:  no factor

    // This isn't necessarily an error, since it could just mean that a string of
    //  factors is complete and the next match simply isn't a factor.  This is how
    //  groups close.

    //if (strPattern [iPatternPos] != '\0')
    //  {
    //  DBG_ERROR ("No Factor recognized at character \'%c\' pos %d NumParen %d : %s", strPattern [iPatternPos], iPatternPos, iNumParenthesis, strPattern.AsChar());
    //  }
    //return (-1);
    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("RegExFactor returning %d", iBranchTwo);
    #endif
    return (iBranchTwo);
    };

  // check for repetition
  if (strPattern [iPatternPos] == '?')
    {
    RegExMatch    matchPass (RegExMatch::kNull, 0, 0, iState + 2, -1);
    RegExMatch    matchOr   (RegExMatch::kOr,   0, 0, iState + 2, iBranchTwo);

    INT  iPassPos = iState;
    INT  iOrPos   = iState + 1;

    arrayStates [iPassPos] = matchPass;
    arrayStates [iOrPos]   = matchOr;

    arrayStates [iBranchOne - 1].SetNextOne (iOrPos);
    //DBG_INFO ("pattern \'?\' branch one -1 is %d.  PassPos is %d", iBranchOne - 1, iPassPos);

    ++iPatternPos;
    iState += 2;
    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("RegExFactor returning %d", iPassPos);
    #endif
    return (iPassPos); // return match pass
    }
  else if ((strPattern [iPatternPos] == '{') ||
           (strPattern [iPatternPos] == '+') ||
           (strPattern [iPatternPos] == '*'))
    {
    RegExMatch    matchNew (RegExMatch::kRepeat, 0, 0, iState + 1, iBranchTwo);

    //if (strPattern [iPatternPos] == '?')  {matchNew.SetNumMatches (0, 1);};
    if (strPattern [iPatternPos] == '+')  {matchNew.SetNumMatches (1, -1);};
    if (strPattern [iPatternPos] == '*')
      {
      if (strPattern [iPatternPos + 1] == '*')
        {
        // Catch double asterisks
        bParsingError = TRUE;
        };
      matchNew.SetNumMatches (0, -1);
      };

    // Parse repetion defined by "{min,max}"
    ParseRepetition (matchNew);

    // Now save the repeat.

    arrayStates [iState] = matchNew;
    arrayStates [iBranchOne - 1].SetNextOne (iState); // not correct.  Only works if last element before branch one is pointing to branch one.

    ++iPatternPos;
    ++iState;
    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("RegExFactor returning %d", iState - 1);
    #endif
    return (iState - 1);
    }
  else
    {
    #ifdef DEBUGMSG_PARSE
      DBG_INFO ("RegExFactor returning %d", iBranchTwo);
    #endif
    return (iBranchTwo);
    };
  };


//------------------------------------------------------------------------
VOID RegEx::ParseRepetition (RegExMatch &  matchIn)
  {
  #ifdef DEBUGMSG_PARSE
    DBG_INFO ("RegEx:ParseRepetition");
  #endif
  if (strPattern [iPatternPos] == '{')
    {
    const char *  pchStartOne = strPattern.AsChar (iPatternPos + 1);
    const char *  pchStartTwo;

    long int lStartRange = strtol (pchStartOne, (char **)&pchStartTwo, 10);
    matchIn.SetMatchMin (INT (lStartRange));
    const char *  pchEnd = pchStartTwo;


    if (*pchStartTwo == ',')
      {
      // Might be a second number.
      ++pchStartTwo;

      if (*pchStartTwo == '}')
        {
        // open ended range
        matchIn.SetMatchMax (-1);
        pchEnd = pchStartTwo;
        }
      else
        {
        long int lEndRange = strtol (pchStartTwo, (char **)&pchEnd, 10);
        matchIn.SetMatchMax (INT (lEndRange));
        };
      }
    else if (*pchStartTwo == '}')
      {
      // exact count
      matchIn.SetMatchMax (INT (lStartRange));
      pchEnd = pchStartTwo;
      }
    else
      {
      // error: invalid character in range
      };

    if (*pchEnd == '}')
      {
      iPatternPos += pchEnd - pchStartOne + 1;
      }
    else
      {
      // not able to find the end
      };
    };
  };


//------------------------------------------------------------------------
BOOL RegEx::ParseList (INT &   iBranchTwo,
                       BOOL &  bParsingError)
  {
  // return TRUE if the list is parsed out, FALSE if there was an error

  ++iPatternPos;
  BOOL  bNegativeMatch = FALSE;

  #ifdef DEBUGMSG_PARSE
    DBG_INFO ("RegExParseList");
  #endif
  // check to see if this is a "not match" list
  if (strPattern [iPatternPos] == '^')
    {
    ++iPatternPos;
    bNegativeMatch = TRUE;
    };

  // check for empty list
  if ((iPatternPos >= strPattern.Length ()) || (strPattern [iPatternPos] == ']'))
    {
    bParsingError = TRUE;
    }
  if (bNegativeMatch)
    {
    iBranchTwo = RegExListInverted ();
    }
  else
    {
    iBranchTwo = RegExList ();
    }

  // close list
  if ((iPatternPos >= strPattern.Length ()) || (strPattern [iPatternPos] == ']'))
    {
    ++iPatternPos;
    }
  else
    {
    // Error.  List not closed
    bParsingError = TRUE;
    return (FALSE);
    };
  return (TRUE);
  };


//------------------------------------------------------------------------
INT  RegEx::RegExListInverted (VOID)
  {
  INT  iNewReturn;
  INT  iStartState = iState;
  INT  iMatchState = iState + 1;


  // create a null for the entrance situation (to skip the match null)
  arrayStates [iStartState] = RegExMatch  (RegExMatch::kNull, 0, 0, iMatchState + 1, -1);
  ++iState;

  // create a null that we can target for the "match" situation
  arrayStates [iMatchState] = RegExMatch  (RegExMatch::kNull, 0, 0, -1, -1);
  ++iState;

  INT iChainStart = RegExListItemInverted ();
  do
    {
    iNewReturn = RegExListItemInverted ();
    }
  while (iNewReturn != -1);

  // add a final "any character" to accept the sequence if we've gotten this far
  arrayStates [iState] = RegExMatch  (RegExMatch::kAnyChar, 0, 0, iMatchState, -1);
  ++iState;

  // point the "match" null to the state after the chain
  arrayStates [iMatchState].SetNextOne (iState);

  return (iChainStart);
  };

//------------------------------------------------------------------------
INT  RegEx::RegExListItemInverted (VOID)
  {
  INT            iMatchSize;
  RegExMatch     matchNew;

  #ifdef DEBUGMSG_PARSE
    DBG_INFO ("RegExListItemInverted");
  #endif
  if (matchNew.GetType  (strPattern.AsChar (iPatternPos), true, iMatchSize))
    {
    if (matchNew.IsError ()) {bParsingError = TRUE;};
    // found a new character

    // add a state for the character to match.
    matchNew.SetNext  (iState + 1, -1);
    matchNew.InvertMatch (TRUE);
    arrayStates [iState] = matchNew;
    iPatternPos += iMatchSize;

    // prepare for the next state.
    ++iState;
    return (iState - 1);
    };
  if (matchNew.IsError ()) {bParsingError = TRUE;};

  return (-1);
  }

//------------------------------------------------------------------------
INT  RegEx::RegExList (VOID)
  {
  INT  iLastAddedOr;
  INT  iNewReturn;
  INT  iStartState = iState;
  INT  iMatchState = iState + 1;
  INT  iChainStart;


  // create a null for the entrance situation (to skip the match null)
  arrayStates [iStartState] = RegExMatch  (RegExMatch::kNull, 0, 0, iMatchState + 1, -1);
  ++iState;

  // create a null that we can target for the "match" situation
  arrayStates [iMatchState] = RegExMatch  (RegExMatch::kNull, 0, 0, -1, -1);
  ++iState;

  iLastAddedOr = iChainStart = RegExListItem (iMatchState);

  while ((iNewReturn = RegExListItem (iMatchState)) != -1)
    {
    iLastAddedOr = iNewReturn;
    };

  // point the "match" null to the state after the chain
  arrayStates [iMatchState].SetNextOne (iState);

  // and turn the last Or into a null, so it will just move to the final list element
  arrayStates [iLastAddedOr].SetType    (RegExMatch::kNull);
  arrayStates [iLastAddedOr].SetNextTwo (-1);

  return (iStartState);
  //return (iChainStart); // debugging... have first state be entrance
  };


//------------------------------------------------------------------------
INT  RegEx::RegExListItem (INT   iListStartState)
  {
  INT            iMatchSize;
  RegExMatch     matchNew;

  #ifdef DEBUGMSG_PARSE
    DBG_INFO ("RegExListItem");
  #endif
  if (matchNew.GetType  (strPattern.AsChar (iPatternPos), true, iMatchSize))
    {
    if (matchNew.IsError ()) {bParsingError = TRUE;};
    // found a new character

    // add an "or" branch
    arrayStates [iState] = RegExMatch  (RegExMatch::kOr,   0, 0, iState + 1, iState + 2);
    INT  iOrState = iState;

    // add a state for the character to match.
    ++iState;
    matchNew.SetNext  (iListStartState, -1);
    arrayStates [iState] = matchNew;
    iPatternPos += iMatchSize;

    // prepare for the next state.
    ++iState;

    return (iOrState);
    };
  if (matchNew.IsError ()) {bParsingError = TRUE;};

  return (-1);
  };



