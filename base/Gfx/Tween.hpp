/* -----------------------------------------------------------------
                             Tween

     This module implements tweening approaches for animation.


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

#ifndef TWEEN_HPP
#define TWEEN_HPP

#include <math.h>

#include "Sys/Types.hpp"
#include "Util/Signal.h"

// Functions inspired by https://gist.github.com/gre/1650294 and the
//   jQuery easing plugin http://gsgd.co.uk/sandbox/jquery/easing/
//   https://github.com/AndrewRayCode/easing-utils/blob/master/src/easing.js
//   which are in turn inspired by http://gizma.com/easing/

// For future reference, here are some interesting applications of tweens
//  http://ianlunn.github.io/Hover/

// NOTE: This Tween class is dual-purpose.  First, it is a static class
//  for running tween-type calculations wherever and however you like.
//  Second, it is a state-full class that implements a more fully-featured
//  tweening solution.  This is done as one class instead of two in order
//  to keep all the tweening code centralized.

using namespace Gallant;

//-----------------------------------------------------------------------------
class Tween
  {
  public:
    enum EType {kDisabled     = 0,
                kLinear       = 1,
                kInQuad       = 2,
                kOutQuad      = 3,
                kInOutQuad    = 4,
                kInCubic      = 5,
                kOutCubic     = 6,
                kInOutCubic   = 7,
                kInQuar       = 8,
                kOutQuar      = 9,
                kInOutQuar    = 10,
                kInQuint      = 11,
                kOutQuint     = 12,
                kInOutQuint   = 13,
                kInSine       = 14,
                kOutSine      = 15,
                kInOutSine    = 16,
                kInExpo       = 17,
                kOutExpo      = 18,
                kInOutExpo    = 19,
                kInCirc       = 20,
                kOutCirc      = 21,
                kInOutCirc    = 22,
                kInBack       = 23,
                kOutBack      = 24,
                kInOutBack    = 25,
                kInElastic    = 26,
                kOutElastic   = 27,
                kInOutElastic = 28,
                kOutBounce    = 29,
                kInBounce     = 30,
                kInOutBounce  = 31};

    enum ELoopType {kNone     = 0,   ///< Play from state A to B.
                    kSwitch   = 1,   ///< Play one direction and then Pause.  If you Play again, it will move the opposite direction and then Pause.
                    kRepeat   = 2,   ///< Play from state A to B, then restart at A.
                    kPingPong = 3};  ///< Play from state A to B to A to B, etc...

    enum EPlayMode  {kReset    = 0,  ///< Tween is paused before delay, not animating
                     kDelay    = 1,  ///< Tween is in delay state, animating forwards
                     kStateA   = 2,  ///< Tween is in start state, not animating
                     kAnimAB   = 3,  ///< Tween is animating forward from start to end state
                     kStateB   = 4,  ///< Tween is in end state, not animating
                     kAnimBA   = 5}; ///< Tween is animating backwards from end to start state

  // state-less (static) calculations.
  // These convert a parametric time value (0.0-1.0) into a tweened output value
  //   between 0.0 and 1.0 (with some possible overshoot).  In practical use, the
  //   results would be multiplied by an amplitude scalar before being applied to
  //   an animated channel/value.

  public:

    static FLOAT  Linear     (FLOAT t)  {return (t);};
    static FLOAT  InQuad     (FLOAT t)  {return (t*t);};
    static FLOAT  OutQuad    (FLOAT t)  {return (1.0f - InQuad (1.0f-t));};
    static FLOAT  InOutQuad  (FLOAT t)  {if (t < 0.5f) return (InQuad (t * 2.0f) / 2.0f);
                                        return (1.0f - InQuad ((1.0f - t) * 2.0f) / 2.0f);};

    static FLOAT  InCubic    (FLOAT t)  {return (t*t*t);};
    static FLOAT  OutCubic   (FLOAT t)  {return (1.0f - InCubic (1.0f-t));};
    static FLOAT  InOutCubic (FLOAT t)  {if (t < 0.5f) return (InCubic (t * 2.0f) / 2.0f);
                                        return (1.0f - InCubic ((1.0f - t) * 2.0f) / 2.0f);};

    static FLOAT  InQuar    (FLOAT t)   {return (t*t*t*t);};
    static FLOAT  OutQuar   (FLOAT t)   {return (1.0f - InQuar (1.0f-t));};
    static FLOAT  InOutQuar (FLOAT t)   {if (t < 0.5f) return (InQuar (t * 2.0f) / 2.0f);
                                        return (1.0f - InQuar ((1.0f - t) * 2.0f) / 2.0f);};


    static FLOAT  InQuint    (FLOAT t)  {return (t*t*t*t*t);};
    static FLOAT  OutQuint   (FLOAT t)  {return (1.0f - InQuint (1.0f-t));};
    static FLOAT  InOutQuint (FLOAT t)  {if (t < 0.5f) return (InQuint (t * 2.0f) / 2.0f);
                                        return (1.0f - InQuint ((1.0f - t) * 2.0f) / 2.0f);};


    static FLOAT  InSine     (FLOAT t)  {return (-1.0f * cosf (t / (R_PI * 0.5f)) + 1.0f);};
    static FLOAT  OutSine    (FLOAT t)  {return (sinf (t * (R_PI * 0.5f)));};
    static FLOAT  InOutSine  (FLOAT t)  {return (-0.5f * (cosf(R_PI * t) - 1.0f));};

    static FLOAT  InExpo     (FLOAT t)  {return ((t <= 0.0f) ? 0.0f : powf (2.0f, 10.0f * (t - 1.0f)));};
    static FLOAT  OutExpo    (FLOAT t)  {return ((t >= 1.0f) ? 1.0f : (-powf (2.0f, -10.0f * t) + 1.0f));};

    static FLOAT  InOutExpo  (FLOAT t)  {if (t <= 0.0f) return (0.0f);
                                        if (t >= 1.0f) return (1.0f);

                                        FLOAT  t2 = t * 2.0f;
                                        FLOAT  t2Dec = t2 - 1;

                                        if (t2 < 1.0f) {return (0.5f * powf (2.0f, 10.0f * t2Dec));};
                                        return (0.5f * ( -powf (2.0f, -10.0f * t2Dec ) + 2.0f));
                                        };


    static FLOAT  InCirc     (FLOAT t)  {return (-1.0 * (sqrtf ( 1.0f - t * t ) - 1.0f));};

    static FLOAT  OutCirc    (FLOAT t)  {FLOAT  tDec = t - 1.0f;
                                        return (sqrtf( 1.0f - tDec * tDec));};

    static FLOAT  InOutCirc  (FLOAT t)  {
                                        FLOAT  t2    = t * 2.0f;
                                        FLOAT  t2Dec = t2 - 2.0f;

                                        if (t2 < 1.0f ) {return (-0.5f * ( sqrtf( 1.0f - t2 * t2 ) - 1.0f ));};
                                        return (0.5f * (sqrtf (1.0f - t2Dec * t2Dec ) + 1.0f));
                                        };

    static FLOAT  InBack     (FLOAT t,
                              FLOAT mag = 1.70158f) {return (t*t * ((mag + 1.0f) * t - mag));};

    static FLOAT  OutBack    (FLOAT t,
                              FLOAT mag = 1.70158f) {FLOAT  tDec = t - 1.0f;
                                                    return (tDec*tDec * ((mag + 1.0f) * tDec - mag));};

    static FLOAT  InOutBack  (FLOAT t,
                              FLOAT mag = 1.70158f)
                                        {
                                        FLOAT  t2    = t * 2.0f;
                                        FLOAT  t2Dec = t2 - 2.0f;
                                        FLOAT  s     = mag * 1.525f;

                                        if (t2 < 1.0f) {return (0.5f * t2*t2 * (((s + 1.0f) * t2) - s));};
                                        return (0.5f * (t2Dec * t2Dec * ((s + 1.0f) * t2Dec + s) + 2.0f));
                                        };

    static FLOAT  InElastic (FLOAT t,
                             FLOAT mag = 0.7f )
                                        {
                                        if (t <= 0.0f || t >= 1.0f ) {return t;}

                                        FLOAT scaledT = t - 1.0f;
                                        FLOAT p = 1.0f - mag;
                                        FLOAT s = p / (2.0f * R_PI) * asinf (1.0f);

                                        return (-(powf( 2.0f, 10.0f * scaledT ) *
                                                  sinf ((scaledT - s) * (2.0f * R_PI) / p)));
                                        };

    static FLOAT  OutElastic (FLOAT t,
                              FLOAT mag = 0.7f )
                                        {
                                        FLOAT p  = 1.0f - mag;
                                        FLOAT t2 = t * 2.0f;
                                        if (t <= 0.0f || t >= 1.0f ) {return t;}

                                        FLOAT s = p / ( 2 * R_PI ) * asinf (1.0f);
                                        return ((powf( 2.0f, -10.0f * t2 ) *
                                                sinf( ( t2 - s ) * ( 2.0f * R_PI ) / p )) + 1.0f);
                                        };

    static FLOAT  InOutElastic (FLOAT t,
                                FLOAT mag = 0.65f )
                                        {
                                        FLOAT p = 1.0f - mag;

                                        if (t <= 0.0f || t >= 1.0f ) {return t;}

                                        FLOAT t2 = t * 2.0f;
                                        FLOAT t2Dec = t2 - 1.0f;

                                        FLOAT s = p / R_2PI * asinf (1.0f);

                                        if (t2 < 1.0f) {return (-0.5f *
                                                                (powf (2.0f, 10.0f * t2Dec) *
                                                                sinf ((t2Dec - s) * (2.0f * R_PI) / p)));};
                                        return ((powf (2.0f, -10.0f * t2Dec ) *
                                                sinf ((t2Dec - s) * (2.0f * R_PI) / p) * 0.5f) + 1.0f);
                                        };

    static FLOAT  OutBounce (FLOAT t)   {
                                        if (t < (1.0f / 2.75f))
                                          {
                                          return (7.5625f * t * t);
                                          }
                                        else if (t < ( 2.0f / 2.75f))
                                          {
                                          FLOAT scaledT = t - ( 1.5f / 2.75f );
                                          return ( 7.5625f * scaledT * scaledT ) + 0.75f;
                                          }
                                        else if (t < ( 2.5f / 2.75f ) )
                                          {
                                          FLOAT scaledT = t - ( 2.25f / 2.75f );
                                          return ( 7.5625f * scaledT * scaledT ) + 0.9375f;
                                          }
                                        else
                                          {
                                          FLOAT scaledT = t - ( 2.625f / 2.75f );
                                          return ( 7.5625f * scaledT * scaledT ) + 0.984375f;
                                          }
                                        };

    static FLOAT  InBounce    (FLOAT t) {return (1.0f - OutBounce (1.0f - t));};

    static FLOAT  InOutBounce (FLOAT t) {if (t < 0.5) {return (InBounce (t * 2.0f) * 0.5f);};
                                        return ((OutBounce ((t * 2.0f) - 1.0f) * 0.5f) + 0.5f);};



  // state-less (static)
  public:
    static FLOAT      Apply            (FLOAT  fT,
                                        EType  typeIn = kLinear);

    static EType      StringToType     (const char *  szTypeIn);
    static ELoopType  StringToLoopType (const char *  szTypeIn);




  // stateful
  public:
    EType      eType;          ///< Type of tweening expression to apply to time. (ease in, ease out, etc)
    ELoopType  eLoopType;      ///< Type of looping to use
    INT        iPlayMode;      ///< State-based play control
    BOOL       bPaused;        ///< True if paused
    BOOL       bExiting;       ///< True if the loop should be exited on next forward cycle

    FLOAT      fDelay;         ///< Time in seconds before the tweening begins
    FLOAT      fDuration;      ///< Time in seconds over which the tweening happens.
    FLOAT      fCooldown;      ///< Time after tweening is over before the OnComplete signal is fired

    FLOAT      fTime;          ///< Current time of the tween, in seconds
    FLOAT      fDirection;     ///< Direction of the tween (1.0 is forward, -1.0 is reverse)

    Signal0<>  sigOnComplete;  ///< Fires at the end of the cooldown
    Signal0<>  sigOnLoop;      ///< Fires on wrap or direction change


  public:
           Tween        (EType      typeIn      = kLinear,
                         ELoopType  loopTypeIn  = ELoopType::kNone,
                         FLOAT      fDelayIn    = 0.0f,
                         FLOAT      fDurationIn = 1.0f,
                         FLOAT      fCooldownIn = 0.0f);

    BOOL   IncTime      (FLOAT  fDeltaSec); ///< Call regularly with a time delta to move time forwards.  Returns True if entering, exiting, or in the tweening duration period.

    BOOL   IsPlaying    (VOID);             ///< Returns true if time is still within delay, duration, or cooldown.

    FLOAT  GetValue     (VOID);             ///< Returns the tweened value between 0 and 1.
    VOID   Pause        (BOOL  bPausedIn);  ///< IncTime will no longer affec ttime
    VOID   Reset        (VOID);             ///< Reset time back to start of delay, and forward play direction
    VOID   Reverse      (VOID);
    VOID   Play         (BOOL  bForward);

    VOID   SetPlayMode  (EPlayMode  iModeIn);

    VOID   Finish       (BOOL  bFinishIn);  ///< Mark tween so it plays through to the end, breaking out of repeats and ping pongs

  };

#endif // TWEEN_HPP
