/* -----------------------------------------------------------------
                             Debug.cpp

     This class is for displaying error, warning, and debugging
       messages to the user and the error log.

                            Michael T. Duffy
   ----------------------------------------------------------------- */

// Debug.cpp
// Copyright 2002, 2008 Michael T. Duffy.  All Rights Reserved
// contact:  mduffor@gmail.com

// This file is part of TEdit.
//
// TEdit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
//
// TEdit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TEdit.  If not, see <http://www.gnu.org/licenses/>.


// ***********************************************************************
//  Compile Directives
// ***********************************************************************

// #pragma pack (1);

// ***********************************************************************
//  Header Files
// ***********************************************************************

#include <stdio.h>
#include <stdlib.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#ifdef ANDROID_NDK
  #include <jni.h>
  #include <android/log.h>
#endif // ANDROID_NDK


#ifdef ANDROID_NDK

  #define TERMINAL_RED    ""
  #define TERMINAL_YELLOW ""
  #define TERMINAL_GREEN  ""
  #define TERMINAL_CYAN   ""
  #define TERMINAL_BRIGHT ""
  #define TERMINAL_RESET  ""

#else

  #define TERMINAL_RED    "\u001b[31m"
  #define TERMINAL_YELLOW "\u001b[33m"
  #define TERMINAL_GREEN  "\u001b[32m"
  #define TERMINAL_CYAN   "\u001b[36m"
  #define TERMINAL_BRIGHT "\u001b[37;1m"
  #define TERMINAL_RESET  "\u001b[0m"

#endif

#define ANDROID_LOG_TAG "CrowDebug"

// ***********************************************************************
//  Module Variables
// ***********************************************************************


// Define the singleton

PDebugMessages  pdbgMessages = NULL;

RStr    strDbgBuffer;


// ***********************************************************************
//  DebugMessages
// ***********************************************************************


//------------------------------------------------------------------------
DebugMessages::DebugMessages  ()
  {
  if (IsDebugBuild ())
    {
    kMinLogLevel = kInfo;
    }
  else
    {
    // Only show Errors for Release builds.
    kMinLogLevel = kError;
    };
  };


//------------------------------------------------------------------------
DebugMessages::~DebugMessages ()
  {
  };


//------------------------------------------------------------------------
void  DebugMessages::Assert  (UINT32         uStatusIn,
                              const RStr &   strFileIn,
                              UINT32         uLineIn)
  {
  // NOTE: Asserts should always fire and never be filtered by kMinLogLevel

  if (uStatusIn)
    {
    // the status checks out.  Do nothing.
    }
  else
    {
    // failed status check.  Display a message.

    RStr strMsgOut;

    strMsgOut.Format ("%sASSERT ERROR in file %s at line %d.%s", TERMINAL_RED, strFileIn.AsChar (), uLineIn, TERMINAL_RESET);

    RawWriteLog (strMsgOut.AsChar (), kError);
    RawUserDisp ("Assert", strMsgOut.AsChar ());
    };
  };


//------------------------------------------------------------------------
void  DebugMessages::RunMark  (const char *  pszTextIn,
                               const RStr &  strFileIn,
                               UINT32        uLineIn)
  {
  if (kDebug >= kMinLogLevel)
    {
    RStr strMsgOut;

    strMsgOut.Format ("%sRUN MARK in file %s at line %d: %s%s", TERMINAL_CYAN, strFileIn.AsChar (), uLineIn, pszTextIn, TERMINAL_RESET);

    RawWriteLog (strMsgOut.AsChar (), kDebug);
    };
  };


//------------------------------------------------------------------------
void  DebugMessages::Info  (const char *  pszTextIn,
                            const RStr &  strFileIn,
                            UINT32        uLineIn)
  {
  if (kInfo >= kMinLogLevel)
    {
    RStr strMsgOut;

    // For info messages, we will only display the file name, not the entire path.

    strMsgOut.Format ("INFO %s(%s:%d):%s %s",
                      TERMINAL_BRIGHT,
                      strFileIn.AsChar (strFileIn.ReverseFindChar ('/') + 1),
                      uLineIn,
                      TERMINAL_RESET,
                      pszTextIn);

    RawWriteLog (strMsgOut.AsChar (), kInfo);
    };
  };


//------------------------------------------------------------------------
void  DebugMessages::Warning  (const char *  pszTextIn,
                               const RStr &  strFileIn,
                               UINT32        uLineIn)
  {
  if (kWarning >= kMinLogLevel)
    {
    RStr strMsgOut;

    strMsgOut.Format ("%sWARNING (%s:%d): %s%s", TERMINAL_YELLOW, strFileIn.AsChar (strFileIn.ReverseFindChar ('/') + 1), uLineIn, pszTextIn, TERMINAL_RESET);

    RawWriteLog (strMsgOut.AsChar (), kWarning);
    };
  };


//------------------------------------------------------------------------
void  DebugMessages::Error  (const char *  pszTextIn,
                             const RStr &  strFileIn,
                             UINT32        uLineIn)
  {
  if (kError >= kMinLogLevel)
    {
    RStr strMsgOut;

    strMsgOut.Format ("%s***** ERROR!!! (%s:%d): %s *****%s", TERMINAL_RED, strFileIn.AsChar (strFileIn.ReverseFindChar ('/') + 1), uLineIn, pszTextIn, TERMINAL_RESET);

    RawWriteLog (strMsgOut.AsChar (), kError);
    //RawUserDisp ("Error!!!", strMsgOut.AsChar ());
    };
  };


//------------------------------------------------------------------------
void  DebugMessages::RawError  (const char *  pszTextIn)
  {
  if (kError >= kMinLogLevel)
    {
    RStr strMsgOut (pszTextIn);

    RawWriteLog (strMsgOut.AsChar (), kError);
    //RawUserDisp ("Error!!!", strMsgOut.AsChar ());
    };
  };


//------------------------------------------------------------------------
void  DebugMessages::RunMark  (const RStr &  strTextIn,
                               const RStr &  strFileIn,
                               UINT32        uLineIn)
  {
  RunMark (strTextIn.AsChar (), strFileIn, uLineIn);
  };


//------------------------------------------------------------------------
void  DebugMessages::Warning  (const RStr &  strTextIn,
                               const RStr &  strFileIn,
                               UINT32        uLineIn)
  {
  Warning (strTextIn.AsChar (), strFileIn, uLineIn);
  };


//------------------------------------------------------------------------
void  DebugMessages::Error  (const RStr &  strTextIn,
                             const RStr &  strFileIn,
                             UINT32        uLineIn)
  {
  Error (strTextIn.AsChar (), strFileIn, uLineIn);
  };


//------------------------------------------------------------------------
void  DebugMessages::RawError  (const RStr &  strTextIn)
  {
  RawError (strTextIn.AsChar ());
  };


//------------------------------------------------------------------------
void  DebugMessages::RawUserDisp  (const char *  pszDialogTitleIn,
                                   const char *  pszTextIn)
  {
  // There is no default behavior for displaying user messages.  Generally anything
  //  that will display a user message will also write out a log message.

  printf ("%s\n", pszTextIn);
  };


//------------------------------------------------------------------------
void  DebugMessages::RawWriteLog  (const char *  pszTextIn,
                                   ELogLevel     kLevelIn)
  {
  // The default behavior for RawWriteLog is to display a message to a
  //   file called "debug.err".

  // we open and close the file each time in order to assure that the
  //  message is written, even if we crash.  This is slow and reliable
  //  for debug builds, and this code is removed at the macro level
  //  for release builds.

  #ifdef ANDROID_NDK
    // NOTE:  We currently do not use ANDROID_LOG_VERBOSE
    if (kLevelIn == kInfo)
      {
      __android_log_print(ANDROID_LOG_INFO, ANDROID_LOG_TAG, "%s", pszTextIn);
      }
    else if (kLevelIn == kDebug)
      {
      __android_log_print(ANDROID_LOG_DEBUG, ANDROID_LOG_TAG, "%s", pszTextIn);
      }
    else if (kLevelIn == kWarning)
      {
      __android_log_print(ANDROID_LOG_WARN, ANDROID_LOG_TAG, "%s", pszTextIn);
      }
    else
      {
      __android_log_print(ANDROID_LOG_ERROR, ANDROID_LOG_TAG, "%s", pszTextIn);
      };
  #else
    printf ("%s\n", pszTextIn);
    FILE *  fp = fopen ("debug.err", "at");
    if (fp != NULL)
      {
      fprintf (fp, "%s\n", pszTextIn);
      fclose (fp);
      };
  #endif
  };



// ***********************************************************************
//  DebugMessagesFactory
// ***********************************************************************



//------------------------------------------------------------------------
DebugMessagesFactory::DebugMessagesFactory  ()
  {
  };


//------------------------------------------------------------------------
DebugMessagesFactory::~DebugMessagesFactory ()
  {
  };


//------------------------------------------------------------------------
PDebugMessages  DebugMessagesFactory::Initialize  (VOID)
  {
  #ifdef ANDROID_NDK
  #else
    remove ("debug.err");
  #endif

  if (pdbgMessages == NULL)
    {
    SetSingleton (new DebugMessages);
    };

  return (pdbgMessages);
  };


//------------------------------------------------------------------------
void  DebugMessagesFactory::Uninitialize  (VOID)
  {
  delete pdbgMessages;
  SetSingleton (NULL);
  };


//------------------------------------------------------------------------
void  DebugMessagesFactory::SetSingleton  (PDebugMessages  pdbgMessagesIn)
  {
  pdbgMessages = pdbgMessagesIn;
  };






//------------------------------------------------------------------------
bool IsDebugBuild (void)
  {
  #ifdef DEBUG
    return (TRUE);
  #else
    return (FALSE);
  #endif
  };






