/* -----------------------------------------------------------------
                             Tween

     This module implements transition approaches for animation.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2018, Michael T. Duffy II.  All rights reserved.
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

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Gfx/Tween.hpp"


//-----------------------------------------------------------------------------
//  Tween
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FLOAT  Tween::Apply  (FLOAT  fT,
                      EType  typeIn)
  {
  switch (typeIn)
    {
    case kLinear:
         return Linear (fT);
    case kInQuad:
         return InQuad (fT);
    case kOutQuad:
         return OutQuad (fT);
    case kInOutQuad:
         return InOutQuad (fT);
    case kInCubic:
         return InCubic (fT);
    case kOutCubic:
         return OutCubic (fT);
    case kInOutCubic:
         return InOutCubic (fT);
    case kInQuar:
         return InQuar (fT);
    case kOutQuar:
         return OutQuar (fT);
    case kInOutQuar:
         return InOutQuar (fT);
    case kInQuint:
         return InQuint (fT);
    case kOutQuint:
         return OutQuint (fT);
    case kInOutQuint:
         return InOutQuint (fT);
    case kInSine:
         return InSine (fT);
    case kOutSine:
         return OutSine (fT);
    case kInOutSine:
         return InOutSine (fT);
    case kInExpo:
         return InExpo (fT);
    case kOutExpo:
         return OutExpo (fT);
    case kInOutExpo:
         return InOutExpo (fT);
    case kInCirc:
         return InCirc (fT);
    case kOutCirc:
         return OutCirc (fT);
    case kInOutCirc:
         return InOutCirc (fT);
    case kInBack:
         return InBack (fT);
    case kOutBack:
         return OutBack (fT);
    case kInOutBack:
         return InOutBack (fT);
    case kInElastic:
         return InElastic (fT);
    case kOutElastic:
         return OutElastic (fT);
    case kInOutElastic:
         return InOutElastic (fT);
    case kOutBounce:
         return OutBounce (fT);
    case kInBounce:
         return InBounce (fT);
    case kInOutBounce:
         return InOutBounce (fT);
    default:
      return 0.0f;
    }
  };

//-----------------------------------------------------------------------------
Tween::EType  Tween::StringToType (const char *  szTypeIn)
  {
  // NOTE: This function will not be fast, so the results should be cached.

  if (streqi (szTypeIn, "disabled"))     return kDisabled;
  if (streqi (szTypeIn, "linear"))       return kLinear;
  if (streqi (szTypeIn, "inquad"))       return kInQuad;
  if (streqi (szTypeIn, "outquad"))      return kOutQuad;
  if (streqi (szTypeIn, "inoutquad"))    return kInOutQuad;
  if (streqi (szTypeIn, "incubic"))      return kInCubic;
  if (streqi (szTypeIn, "outcubic"))     return kOutCubic;
  if (streqi (szTypeIn, "inoutcubic"))   return kInOutCubic;
  if (streqi (szTypeIn, "inquar"))       return kInQuar;
  if (streqi (szTypeIn, "outquar"))      return kOutQuar;
  if (streqi (szTypeIn, "inoutquar"))    return kInOutQuar;
  if (streqi (szTypeIn, "inquint"))      return kInQuint;
  if (streqi (szTypeIn, "outquint"))     return kOutQuint;
  if (streqi (szTypeIn, "inoutquint"))   return kInOutQuint;
  if (streqi (szTypeIn, "insine"))       return kInSine;
  if (streqi (szTypeIn, "outsine"))      return kOutSine;
  if (streqi (szTypeIn, "inoutsine"))    return kInOutSine;
  if (streqi (szTypeIn, "inexpo"))       return kInExpo;
  if (streqi (szTypeIn, "outexpo"))      return kOutExpo;
  if (streqi (szTypeIn, "inoutexpo"))    return kInOutExpo;
  if (streqi (szTypeIn, "incirc"))       return kInCirc;
  if (streqi (szTypeIn, "outcirc"))      return kOutCirc;
  if (streqi (szTypeIn, "inoutcirc"))    return kInOutCirc;
  if (streqi (szTypeIn, "inback"))       return kInBack;
  if (streqi (szTypeIn, "outback"))      return kOutBack;
  if (streqi (szTypeIn, "inoutback"))    return kInOutBack;
  if (streqi (szTypeIn, "inelastic"))    return kInElastic;
  if (streqi (szTypeIn, "outelastic"))   return kOutElastic;
  if (streqi (szTypeIn, "inoutelastic")) return kInOutElastic;
  if (streqi (szTypeIn, "outbounce"))    return kOutBounce;
  if (streqi (szTypeIn, "inbounce"))     return kInBounce;
  if (streqi (szTypeIn, "inoutbounce"))  return kInOutBounce;

  return kLinear;
  };

//-----------------------------------------------------------------------------
Tween::ELoopType  Tween::StringToLoopType (const char *  szTypeIn)
  {
  // NOTE: This function will not be fast, so the results should be cached.

  if (streqi (szTypeIn, "none"))         return kNone;
  if (streqi (szTypeIn, "switch"))       return kSwitch;
  if (streqi (szTypeIn, "repeat"))       return kRepeat;
  if (streqi (szTypeIn, "pingpong"))     return kPingPong;

  return kNone;
  };

//-----------------------------------------------------------------------------
Tween::Tween (EType      typeIn,
              ELoopType  loopTypeIn,
              FLOAT      fDelayIn,
              FLOAT      fDurationIn,
              FLOAT      fCooldownIn)
  {
  eType     = typeIn;
  eLoopType = loopTypeIn;
  iPlayMode = kReset;

  fDelay    = fDelayIn;
  fDuration = fDurationIn;
  fCooldown = fCooldownIn;

  Reset ();
  };

//-----------------------------------------------------------------------------
BOOL  Tween::IncTime (FLOAT  fDeltaSec)
  {
  // returns TRUE if fTime is actively updating the value (not on delay or cooldown)
  //  FALSE if delay, cooldown, or finished.

  FLOAT  fPrevTime = fTime;

  if (!bPaused)
    {
    fTime += fDeltaSec * fDirection;

    if (eLoopType == kPingPong)
      {
      // bounce bottom
      if ((fPrevTime >= 0.0f) && (fTime < 0.0f))
        {
        iPlayMode  = kAnimAB;
        fTime      = -fTime;
        fDirection *= -1.0f;
        sigOnLoop();
        };

      // bounce top
      if ((!bExiting) && (fPrevTime <= fDuration) && (fTime > fDuration))
        {
        iPlayMode  = kAnimBA;
        fTime      = fDuration - (fTime - fDuration);
        fDirection *= -1.0f;
        sigOnLoop();
        };
      };

    if (eLoopType == kRepeat)
      {
      // wrap top
      if ((!bExiting) && (fPrevTime <= fDuration) && (fTime > fDuration))
        {
        iPlayMode =  kAnimAB;
        fTime     -= fDuration;
        sigOnLoop();
        };
      };

    if (eLoopType == kSwitch)
      {
      if ((iPlayMode == kDelay) && (fPrevTime < 0.0f) && (fTime >= 0.0f))
        {
        // delay is finished.  Lock at StateA.
        iPlayMode  = kStateA;
        fTime      = 0.0f;
        fDirection = 0.0f;
        sigOnLoop();
        }

      // clamp top
      if ((fPrevTime <= fDuration) && (fTime > fDuration))
        {
        iPlayMode  = kStateB;
        fTime      = fDuration;
        fDirection = 0.0f;
        sigOnLoop();
        sigOnComplete();
        };

      // clamp bottom
      if ((fPrevTime >= 0.0f) && (fTime < 0.0f))
        {
        iPlayMode  = kStateA;
        fTime      = 0.0f;
        fDirection = 0.0f;
        sigOnLoop();
        sigOnComplete();
        };
      };

    // end
    if (((fDirection > 0.0f) && (fTime > fDuration + fCooldown)) ||
        ((fDirection < 0.0f) && (fTime < 0.0f)))
      {
      bPaused = TRUE;
      sigOnLoop();
      sigOnComplete();
      };

    };
  return (((fTime     >= 0) && (fTime <= fDuration)) ||
          ((fPrevTime >= 0) && (fPrevTime <= fDuration)));
  };

//-----------------------------------------------------------------------------
BOOL Tween::IsPlaying (VOID)
  {
  if (eType == kDisabled) return (FALSE);

  return ((bPaused == FALSE) &&

          (((fDirection > 0.0f) && (fTime <= fDuration + fCooldown)) ||
           ((fDirection < 0.0f) && (fTime >= 0.0f))));
  };

//-----------------------------------------------------------------------------
FLOAT  Tween::GetValue (VOID)
  {
  return (Apply (fTime, eType));
  };


//-----------------------------------------------------------------------------
VOID  Tween::Pause (BOOL  bPausedIn)
  {
  bPaused = bPausedIn;
  };

//-----------------------------------------------------------------------------
VOID  Tween::Reset (VOID)
  {
  bPaused    = FALSE;
  bExiting   = FALSE;
  fTime      = -fDelay;
  fDirection = 1.0f;
  };

//-----------------------------------------------------------------------------
VOID  Tween::Reverse (VOID)
  {
  bPaused     = FALSE;
  bExiting    = FALSE;
  fDirection *= -1.0f;
  };

//-----------------------------------------------------------------------------
VOID  Tween::Play (BOOL  bForward)
  {
  bPaused    = FALSE;
  bExiting   = FALSE;
  fDirection = bForward ? 1.0f : -1.0f;
  };

//-----------------------------------------------------------------------------
VOID  Tween::SetPlayMode  (EPlayMode  iModeIn)
  {
  // if there is no change, then there is nothing to do.
  if (iPlayMode == iModeIn) return;

  // SetPlayMode is used for an external caller to set the tween's play mode.
  //   It is not for internally setting the mode as time progresses.

  bPaused    = FALSE;

  switch (iPlayMode)
    {
    case kReset: // Tween is paused before delay, not animating
      bPaused = TRUE;
      fTime = -fDelay;
      break;

    case kDelay: // Tween is in delay state, animating forwards
      fTime      = -fDelay;
      fDirection = 1.0f;
      break;

    case kStateA: // Tween is in start state, not animating
      fTime      = 0.0f;
      fDirection = 0.0f;
      break;

    case kAnimAB: // Tween is animating forward from start to end state
      fTime      = 0.0f;
      fDirection = 1.0f;
      break;

    case kStateB: // Tween is in end state, not animating
      fTime      = fDuration;
      fDirection = 0.0f;
      break;

    case kAnimBA: // Tween is animating backwards from end to start state
      fTime      = fDuration;
      fDirection = -1.0f;
      break;
    };
  };


//-----------------------------------------------------------------------------
VOID  Tween::Finish (BOOL  bFinishIn)
  {
  bExiting = bFinishIn;
  };














