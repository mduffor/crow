/* -----------------------------------------------------------------
                       Expression Default Functions

     This module implements some default functions that can be called
     by the expression solver

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

#include "Sys/Shell.hpp"
#include "Sys/DeviceTime.hpp"
#include "Gfx/ColorOps.hpp"
#include "Script/ExpressionDefaultFn.hpp"
#include "Composite/Component.hpp"

//-----------------------------------------------------------------------------
VOID RegisterDefaultExpressionFn (VOID)
  {
  DBG_INFO ("RegisterDefaultExpressionFn");
  Expression::RegisterFunction (new ExpressionFn("Min",    &FnMath::Min));
  Expression::RegisterFunction (new ExpressionFn("Max",    &FnMath::Max));
  Expression::RegisterFunction (new ExpressionFn("Lerp",   &FnMath::Lerp));
  Expression::RegisterFunction (new ExpressionFn("Rand",   &FnMath::Rand));
  Expression::RegisterFunction (new ExpressionFn("Choose", &FnMath::Choose));

  Expression::RegisterFunction (new ExpressionFn("PrintRegistry", &FnUtil::PrintRegistry));
  Expression::RegisterFunction (new ExpressionFn("Print",         &FnUtil::Print));
  Expression::RegisterFunction (new ExpressionFn("GetInt",        &FnUtil::GetInt));
  Expression::RegisterFunction (new ExpressionFn("GetString",     &FnUtil::GetString));
  Expression::RegisterFunction (new ExpressionFn("Eval",          &FnUtil::Eval));

  Expression::RegisterFunction (new ExpressionFn("ShowNotification", &FnSystem::ShowNotification));
  Expression::RegisterFunction (new ExpressionFn("CloseApplication", &FnSystem::CloseApplication));

  // FnMedia
  Expression::RegisterFunction (new FnOpenURL);
  Expression::RegisterFunction (new FnOpenTwitter);
  Expression::RegisterFunction (new FnOpenFacebook);


  Expression::RegisterFunction (new ExpressionFn("Click",   &FnTouch::Click));
  Expression::RegisterFunction (new ExpressionFn("Press",   &FnTouch::Press));
  Expression::RegisterFunction (new ExpressionFn("Release", &FnTouch::Release));
  Expression::RegisterFunction (new ExpressionFn("Action",  &FnTouch::Action));

  Expression::RegisterFunction (new ExpressionFn("LocalBeforeDate",   &FnTimeDate::LocalBeforeDate));
  Expression::RegisterFunction (new ExpressionFn("LocalAfterDate",    &FnTimeDate::LocalAfterDate));
  Expression::RegisterFunction (new ExpressionFn("LocalBetweenDates", &FnTimeDate::LocalBetweenDates));
  Expression::RegisterFunction (new ExpressionFn("AddToDate",         &FnTimeDate::AddToDate));
  Expression::RegisterFunction (new ExpressionFn("LocalTimeRemaining", &FnTimeDate::LocalTimeRemaining));
  Expression::RegisterFunction (new ExpressionFn("SecondsToString",    &FnTimeDate::SecondsToString));

  Expression::RegisterFunction (new ExpressionFn("LocalCountdown", &FnTimeDate::LocalCountdown));



  };

//-----------------------------------------------------------------------------
VOID  FnMath::Min (Token *          ptokParamsIn,
                   ValueRegistry *  pRegistryIn,
                   Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2) return;

  // NOTE:  Need to add Token functions IsFloat / IsInt that check eType and kVariable types

  BOOL  bIntegerMath = TRUE;
  if (ptokParamsIn->IsFloat (pRegistryIn)) {bIntegerMath = FALSE;};
  if (ptokParamsIn->Next()->IsFloat (pRegistryIn)) {bIntegerMath = FALSE;};

  if (bIntegerMath)
    {
    INT  iA = ptokParamsIn->AsInt (pRegistryIn);
    INT  iB = ptokParamsIn->Next()->AsInt (pRegistryIn);

    ptokResultOut->eType = TokenType::kInt;
    ptokResultOut->Set (RMin (iA, iB), pRegistryIn);
    }
  else
    {
    FLOAT  fA = ptokParamsIn->AsFloat (pRegistryIn);
    FLOAT  fB = ptokParamsIn->Next()->AsFloat (pRegistryIn);

    ptokResultOut->eType = TokenType::kFloat;
    ptokResultOut->Set (RMin (fA, fB), pRegistryIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  FnMath::Max (Token *          ptokParamsIn,
                   ValueRegistry *  pRegistryIn,
                   Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2) return;

  // NOTE:  Need to add Token functions IsFloat / IsInt that check eType and kVariable types

  BOOL  bIntegerMath = TRUE;
  if (ptokParamsIn->IsFloat (pRegistryIn)) {bIntegerMath = FALSE;};
  if (ptokParamsIn->Next()->IsFloat (pRegistryIn)) {bIntegerMath = FALSE;};

  if (bIntegerMath)
    {
    INT  iA = ptokParamsIn->AsInt (pRegistryIn);
    INT  iB = ptokParamsIn->Next()->AsInt (pRegistryIn);

    ptokResultOut->eType = TokenType::kInt;
    ptokResultOut->Set (RMax (iA, iB), pRegistryIn);
    }
  else
    {
    FLOAT  fA = ptokParamsIn->AsFloat (pRegistryIn);
    FLOAT  fB = ptokParamsIn->Next()->AsFloat (pRegistryIn);

    ptokResultOut->eType = TokenType::kFloat;
    ptokResultOut->Set (RMax (fA, fB), pRegistryIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  FnMath::Lerp (Token *          ptokParamsIn,
                    ValueRegistry *  pRegistryIn,
                    Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 3) return;

  FLOAT  fA = ptokParamsIn->AsFloat (pRegistryIn);
  FLOAT  fB = ptokParamsIn->Next()->AsFloat (pRegistryIn);
  FLOAT  fT = ptokParamsIn->Next()->Next()->AsFloat (pRegistryIn);

  ptokResultOut->eType = TokenType::kFloat;
  ptokResultOut->Set (OpLerp (fA, fB, fT), pRegistryIn);
  };

//-----------------------------------------------------------------------------
VOID  FnMath::Rand (Token *          ptokParamsIn,
                    ValueRegistry *  pRegistryIn,
                    Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2) return;

  // NOTE:  Need to add Token functions IsFloat / IsInt that check eType and kVariable types

  BOOL  bIntegerMath = TRUE;
  if (ptokParamsIn->IsFloat (pRegistryIn)) {bIntegerMath = FALSE;};
  if (ptokParamsIn->Next()->IsFloat (pRegistryIn)) {bIntegerMath = FALSE;};

  if (bIntegerMath)
    {
    INT  iA = ptokParamsIn->AsInt (pRegistryIn);
    INT  iB = ptokParamsIn->Next()->AsInt (pRegistryIn);

    INT  iRange = iB - iA;

    ptokResultOut->eType = TokenType::kInt;
    ptokResultOut->Set (iA + (rand() % iRange), pRegistryIn);
    }
  else
    {
    FLOAT  fA = ptokParamsIn->AsFloat (pRegistryIn);
    FLOAT  fB = ptokParamsIn->Next()->AsFloat (pRegistryIn);
    FLOAT  fRange = fB - fA;


    ptokResultOut->eType = TokenType::kFloat;
    ptokResultOut->Set (fA + (fRange * ((float)rand()/(float)(RAND_MAX))), pRegistryIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  FnMath::Choose (Token *          ptokParamsIn,
                      ValueRegistry *  pRegistryIn,
                      Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();
  ptokResultOut->eType = TokenType::kUnknown;

  if (iNumTokens < 3)
    {
    DBG_ERROR ("Invalid number of parameters passed to Choose (iIndex, varChoice1, varChoice2, [varChoiceN])");
    return;
    };


  if (pRegistryIn != NULL)
    {
    INT  iIndex = ptokParamsIn->AsInt (pRegistryIn);
    //  ptokResultOut->Set (0, pRegistryIn);

    // find the token that corresponds with the index
    Token *  ptokCurr = ptokParamsIn->Next ();
    for (; iIndex > 0; --iIndex)
      {
      if (ptokCurr != NULL)
        {
        ptokCurr = ptokCurr->Next ();
        };
      };
    if (ptokCurr != NULL)
      {
      // found it.  Do the assignment.  If not found, the original kUnknown value will be returned.
      *ptokResultOut = *ptokCurr;
      };
    };
  };






//-----------------------------------------------------------------------------
VOID  FnUtil::PrintRegistry (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut)
  {
  if (pRegistryIn != NULL)
    {
    pRegistryIn->DebugPrint();
    };
  };

//-----------------------------------------------------------------------------
VOID  FnUtil::Print (Token *          ptokParamsIn,
                     ValueRegistry *  pRegistryIn,
                     Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  if (pRegistryIn != NULL)
    {
    DBG_RAW (ptokParamsIn->AsString (pRegistryIn));
    };
  };

//-----------------------------------------------------------------------------
VOID  FnUtil::GetInt (Token *          ptokParamsIn,
                      ValueRegistry *  pRegistryIn,
                      Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  if (ptokResultOut != NULL)
    {
    ptokResultOut->eType = TokenType::kInt;
    ptokResultOut->Set (0, pRegistryIn);

    if (pRegistryIn != NULL)
      {
      //DBG_INFO ("GetInt for %s returns %d", ptokParamsIn->AsString (pRegistryIn), pRegistryIn->GetInt (ptokParamsIn->AsString (pRegistryIn)));
      ptokResultOut->Set (pRegistryIn->GetInt (ptokParamsIn->AsString (pRegistryIn)), pRegistryIn);
      };
    }
  };

//-----------------------------------------------------------------------------
VOID  FnUtil::GetString (Token *          ptokParamsIn,
                         ValueRegistry *  pRegistryIn,
                         Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  if (ptokResultOut != NULL)
    {
    ptokResultOut->eType = TokenType::kString;
    ptokResultOut->Set (0, pRegistryIn);

    if (pRegistryIn != NULL)
      {
      ptokResultOut->Set (pRegistryIn->GetString (ptokParamsIn->AsString (pRegistryIn)), pRegistryIn);
      };
    }
  };

//-----------------------------------------------------------------------------
VOID  FnUtil::Eval (Token *          ptokParamsIn,
                    ValueRegistry *  pRegistryIn,
                    Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  DBG_INFO ("Eval() evaluating expression: %s", ptokParamsIn->AsString (pRegistryIn));

  Expression::Execute (ptokParamsIn->AsString (pRegistryIn),
                       pRegistryIn,
                       ptokResultOut);
  };

//-----------------------------------------------------------------------------
VOID  FnOpenURL::OnCall (Token *          ptokParamsIn,
                         ValueRegistry *  pRegistryIn,
                         Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  if (pRegistryIn != NULL)
    {
    const char *  szURL = ptokParamsIn->AsString (pRegistryIn);

    Shell::OpenURL (szURL);
    };
  };

//-----------------------------------------------------------------------------
VOID  FnSystem::ShowNotification (Token *          ptokParamsIn,
                                  ValueRegistry *  pRegistryIn,
                                  Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 4) return;

  if (pRegistryIn != NULL)
    {
    int iCode               = ptokParamsIn->AsInt (pRegistryIn);
    int iSecondsUntilShown  = ptokParamsIn->Next()->AsInt (pRegistryIn);
    const char *  szTitle   = ptokParamsIn->Next()->Next()->AsString (pRegistryIn);
    const char *  szText    = ptokParamsIn->Next()->Next()->Next()->AsString (pRegistryIn);

    Shell::SetNotification (iCode, iSecondsUntilShown, szTitle, szText);
    };
  };

//-----------------------------------------------------------------------------
VOID  FnSystem::CloseApplication (Token *          ptokParamsIn,
                                  ValueRegistry *  pRegistryIn,
                                  Token *          ptokResultOut)
  {
  Shell::CloseApplication ();
  };

//-----------------------------------------------------------------------------
VOID  FnOpenTwitter::OnCall (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  if (pRegistryIn != NULL)
    {
    const char *  szUserName = ptokParamsIn->AsString (pRegistryIn);

    Shell::OpenTwitter (szUserName);
    };
  };

//-----------------------------------------------------------------------------
VOID  FnOpenFacebook::OnCall (Token *          ptokParamsIn,
                              ValueRegistry *  pRegistryIn,
                              Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2) return;

  if (pRegistryIn != NULL)
    {
    const char *  szUserId   = ptokParamsIn->AsString (pRegistryIn);
    const char *  szUserName = ptokParamsIn->Next()->AsString (pRegistryIn);

    Shell::OpenFacebook (szUserId, szUserName);
    };
  };



//-----------------------------------------------------------------------------
VOID  FnTouch::Click (Token *          ptokParamsIn,
                      ValueRegistry *  pRegistryIn,
                      Token *          ptokResultOut)
  {
  // NOTE: Because this deals with components, you may want to move it into a
  //   another hierarchy rather than just the base/Script group.

  if (pRegistryIn != NULL)
    {
    // Get the component from the registry, and apply the command to each of its siblings.
    PVOID  pvSource = pRegistryIn->GetPtr (H("SourceComponent", 0));

    if (pvSource != NULL)
      {
      Component *  pComponent = static_cast<Component *>(pvSource);

      Component *  pCurr = pComponent->GetFirstSibling ();
      while (pCurr != NULL)
        {
        if (pCurr != pComponent)
          {
          pCurr->OnClick ();
          };
        };
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  FnTouch::Press (Token *          ptokParamsIn,
                      ValueRegistry *  pRegistryIn,
                      Token *          ptokResultOut)
  {
  // NOTE: Because this deals with components, you may want to move it into a
  //   another hierarchy rather than just the base/Script group.

  if (pRegistryIn != NULL)
    {
    // Get the component from the registry, and apply the command to each of its siblings.
    PVOID  pvSource = pRegistryIn->GetPtr (H("SourceComponent", 0));

    if (pvSource != NULL)
      {
      Component *  pComponent = static_cast<Component *>(pvSource);

      Component *  pCurr = pComponent->GetFirstSibling ();
      while (pCurr != NULL)
        {
        if (pCurr != pComponent)
          {
          pCurr->OnPress ();
          };
        };
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  FnTouch::Release (Token *          ptokParamsIn,
                        ValueRegistry *  pRegistryIn,
                        Token *          ptokResultOut)
  {
  // NOTE: Because this deals with components, you may want to move it into a
  //   another hierarchy rather than just the base/Script group.

  if (pRegistryIn != NULL)
    {
    // Get the component from the registry, and apply the command to each of its siblings.
    PVOID  pvSource = pRegistryIn->GetPtr (H("SourceComponent", 0));

    if (pvSource != NULL)
      {
      Component *  pComponent = static_cast<Component *>(pvSource);

      Component *  pCurr = pComponent->GetFirstSibling ();
      while (pCurr != NULL)
        {
        if (pCurr != pComponent)
          {
          pCurr->OnRelease ();
          };
        };
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  FnTouch::Action (Token *          ptokParamsIn,
                       ValueRegistry *  pRegistryIn,
                       Token *          ptokResultOut)
  {
  // NOTE: Because this deals with components, you may want to move it into a
  //   another hierarchy rather than just the base/Script group.

  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1) return;

  if (pRegistryIn != NULL)
    {
    // Get the component from the registry, and apply the command to each of its siblings.
    PVOID  pvSource = pRegistryIn->GetPtr (H("SourceComponent", 0));

    const char *  szActionName = ptokParamsIn->AsString (pRegistryIn);

    if (pvSource != NULL)
      {
      Component *  pComponent = static_cast<Component *>(pvSource);

      Component *  pCurr = pComponent->GetFirstSibling ();
      while (pCurr != NULL)
        {
        if (pCurr != pComponent)
          {
          pCurr->OnAction (szActionName);
          };
        };
      };
    };
  };





/*
  bool local_before_date (dateIn, currentTimeIn = now)
  bool local_after_date (dateIn, currentTimeIn = now)
  bool local_between_dates (startDateIn, endDateIn, currentTimeIn = now)
  date add_to_date (startDateIn, deltaTimeIn)
*/

// NOTE: The following functions are written so start/before are inclusive,
//       and end/after are exclusive.  [begin,end)

//-----------------------------------------------------------------------------
VOID  FnTimeDate::LocalBeforeDate   (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if ((iNumTokens < 1) || (iNumTokens > 2)) return;

  const char *  szDateIn = ptokParamsIn->AsString (pRegistryIn);

  DeviceTime  timeIn (szDateIn);
  DeviceTime  timeNow;

  if (iNumTokens == 2)
    {
    timeNow.FromString (ptokParamsIn->Next()->AsString (pRegistryIn));
    }
  else
    {
    timeNow = DeviceTime::Now ();
    };

  ptokResultOut->eType = TokenType::kInt;
  ptokResultOut->Set ((timeNow < timeIn) ? 1 : 0, pRegistryIn);
  };

//-----------------------------------------------------------------------------
VOID  FnTimeDate::LocalAfterDate    (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if ((iNumTokens < 1) || (iNumTokens > 2)) return;

  const char *  szDateIn = ptokParamsIn->AsString (pRegistryIn);

  DeviceTime  timeIn (szDateIn);
  DeviceTime  timeNow;

  if (iNumTokens == 2)
    {
    timeNow.FromString (ptokParamsIn->Next()->AsString (pRegistryIn));
    }
  else
    {
    timeNow = DeviceTime::Now ();
    };

  ptokResultOut->eType = TokenType::kInt;
  ptokResultOut->Set ((timeNow >= timeIn) ? 1 : 0, pRegistryIn);
  };

//-----------------------------------------------------------------------------
VOID  FnTimeDate::LocalBetweenDates (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if ((iNumTokens < 2) || (iNumTokens > 3)) return;

  const char *  szDateBeginIn = ptokParamsIn->AsString (pRegistryIn);
  const char *  szDateEndIn   = ptokParamsIn->Next()->AsString (pRegistryIn);

  DeviceTime  timeEnd   (szDateEndIn);
  DeviceTime  timeBegin (szDateBeginIn);
  DeviceTime  timeNow;

  if (iNumTokens == 3)
    {
    timeNow.FromString (ptokParamsIn->Next()->AsString (pRegistryIn));
    }
  else
    {
    timeNow = DeviceTime::Now ();
    };

  ptokResultOut->eType = TokenType::kInt;
  ptokResultOut->Set (((timeBegin <= timeNow) && (timeNow < timeEnd)) ? 1 : 0, pRegistryIn);
  };

//-----------------------------------------------------------------------------
VOID  FnTimeDate::AddToDate         (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2) return;

  const char *  szDateIn = ptokParamsIn->AsString (pRegistryIn);
  DeviceTime    timeIn   (szDateIn);

  const char *  szDeltaIn = ptokParamsIn->Next()->AsString (pRegistryIn);
  TimeDelta     deltaIn (szDeltaIn);

  timeIn += deltaIn;

  RStr strTemp;
  ptokResultOut->eType = TokenType::kString;
  ptokResultOut->Set (timeIn.ToString(&strTemp), pRegistryIn);
  };

//-----------------------------------------------------------------------------
VOID  FnTimeDate::LocalTimeRemaining (Token *          ptokParamsIn,
                                      ValueRegistry *  pRegistryIn,
                                      Token *          ptokResultOut)
  {
  // int LocalTimeRemaining (timeIn, timeNow = NULL);
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if ((iNumTokens < 1) || (iNumTokens > 2)) return;

  const char *  szDateIn = ptokParamsIn->AsString (pRegistryIn);

  DeviceTime  timeIn (szDateIn);
  DeviceTime  timeNow;

  if (iNumTokens == 2)
    {
    timeNow.FromString (ptokParamsIn->Next()->AsString (pRegistryIn));
    }
  else
    {
    timeNow = DeviceTime::Now ();
    };

  ptokResultOut->eType = TokenType::kInt;

  // return the number of seconds between the times, clamped at 0.

  if (timeIn >= timeNow)
    {
    ptokResultOut->Set (0, pRegistryIn);
    }
  else
    {
    TimeDelta  delta = timeIn - timeNow;
    ptokResultOut->Set ((INT) delta.TotalSeconds(), pRegistryIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  FnTimeDate::SecondsToString (Token *          ptokParamsIn,
                                   ValueRegistry *  pRegistryIn,
                                   Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  // string SecondsToString (iSecondsIn, bHideSeconds = FALSE);

  if ((iNumTokens < 1) || (iNumTokens > 2)) return;

  int  iNumSeconds = ptokParamsIn->AsInt (pRegistryIn);

  TimeDelta delta (iNumSeconds);

  BOOL      bHideSeconds = (iNumTokens == 1) ? FALSE : ptokParamsIn->Next()->AsInt (pRegistryIn) != 0;

  RStr  strFormattedTime;

  delta.ToLetterString (&strFormattedTime, bHideSeconds);

  ptokResultOut->eType = TokenType::kString;
  ptokResultOut->Set (strFormattedTime.AsChar(), pRegistryIn);
  };

//-----------------------------------------------------------------------------
VOID  FnTimeDate::LocalCountdown (Token *          ptokParamsIn,
                                  ValueRegistry *  pRegistryIn,
                                  Token *          ptokResultOut)
  {
  // int LocalTimeRemaining (timeIn, timeNow, bHideSeconds);
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 3) return;

  const char *  szDateIn     = ptokParamsIn->AsString (pRegistryIn);
  const char *  szNowIn      = ptokParamsIn->Next()->AsString (pRegistryIn);
  BOOL          bHideSeconds = (iNumTokens == 1) ? FALSE : ptokParamsIn->Next()->Next()->AsInt (pRegistryIn) != 0;

  DeviceTime  timeIn (szDateIn);
  DeviceTime  timeNow;

  if (strlen (szNowIn) != 0)
    {
    timeNow.FromString (szNowIn);
    }
  else
    {
    timeNow = DeviceTime::Now ();
    };

  TimeDelta  delta (0);

  if (timeIn <= timeNow)
    {
    delta = timeIn - timeNow;
    };

  RStr  strFormattedTime;

  delta.ToLetterString (&strFormattedTime, bHideSeconds);

  ptokResultOut->eType = TokenType::kString;
  ptokResultOut->Set (strFormattedTime.AsChar(), pRegistryIn);
  };



