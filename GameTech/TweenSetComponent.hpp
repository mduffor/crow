/* -----------------------------------------------------------------
                         Tween Set Component

     This component implements an IntroOutro component that drives
   tween based animations.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2021, Michael T. Duffy II.  All rights reserved.
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

#ifndef TWEENSETCOMPONENT_HPP
#define TWEENSETCOMPONENT_HPP

#include "Sys/Types.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/AttrFloatArray.hpp"
#include "Composite/Component.hpp"
#include "Component/MeshComponent.hpp"
#include "Gfx/TransformComponent.hpp"
#include "Gfx/Tween.hpp"
#include "Math/RVec.hpp"
#include "Render/Atlas.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "GameTech/IntroOutroComponent.hpp"


// NOTE:  It may be a good idea to turn Fade into Color, so you can
//  handle both alpha transitions as well as darken/brighten animations.
/**
  @addtogroup gametech
  @{
*/

const char szTweenSetComponentID[] = "TweenSet";

//-----------------------------------------------------------------------------
class TweenSetComponent : public IntroOutroComponent
  {
  protected:


    // NOTE: It would probably be a good idea to move each Tween parameter set
    //  into its own class, and then move the In and Out sets to their own classes.
    //  You can still pass the Component and add attrs directly to it.  Only once
    //  this is done should you consider implementing presets.

    // NOTE: Have to make sure all of this works before any refactor is undertaken.

    AttrString *              pattrPresetInName;

    AttrString *              pattrMoveInType; // default to none/off
    AttrFloatArray *          pattrMoveInFrom;
    AttrFloatArray *          pattrMoveInTo;
    AttrFloat *               pattrMoveInDelay;
    AttrFloat *               pattrMoveInDuration;
    AttrFloat *               pattrMoveInCooldown;

    AttrString *              pattrRotateInType; // default to none/off
    AttrFloatArray *          pattrRotateInFrom;
    AttrFloatArray *          pattrRotateInTo;
    AttrFloat *               pattrRotateInDelay;
    AttrFloat *               pattrRotateInDuration;
    AttrFloat *               pattrRotateInCooldown;

    AttrString *              pattrScaleInType; // default to none/off
    AttrFloatArray *          pattrScaleInFrom;
    AttrFloatArray *          pattrScaleInTo;
    AttrFloat *               pattrScaleInDelay;
    AttrFloat *               pattrScaleInDuration;
    AttrFloat *               pattrScaleInCooldown;

    AttrString *              pattrFadeInType; // default to none/off
    AttrFloat *               pattrFadeInFrom;
    AttrFloat *               pattrFadeInTo;
    AttrFloat *               pattrFadeInDelay;
    AttrFloat *               pattrFadeInDuration;
    AttrFloat *               pattrFadeInCooldown;

    Tween                     tweenMoveIn;
    Tween                     tweenRotateIn;
    Tween                     tweenScaleIn;
    Tween                     tweenFadeIn;

    RVec3                     vecMoveInFrom;
    RVec3                     vecMoveInTo;
    RVec3                     vecRotateInFrom;
    RVec3                     vecRotateInTo;
    RVec3                     vecScaleInFrom;
    RVec3                     vecScaleInTo;
    FLOAT                     fFadeInFrom;
    FLOAT                     fFadeInTo;






    AttrString *              pattrPresetOutName;

    AttrString *              pattrMoveOutType; // default to none/off
    AttrFloatArray *          pattrMoveOutFrom;
    AttrFloatArray *          pattrMoveOutTo;
    AttrFloat *               pattrMoveOutDelay;
    AttrFloat *               pattrMoveOutDuration;
    AttrFloat *               pattrMoveOutCooldown;

    AttrString *              pattrRotateOutType; // default to none/off
    AttrFloatArray *          pattrRotateOutFrom;
    AttrFloatArray *          pattrRotateOutTo;
    AttrFloat *               pattrRotateOutDelay;
    AttrFloat *               pattrRotateOutDuration;
    AttrFloat *               pattrRotateOutCooldown;

    AttrString *              pattrScaleOutType; // default to none/off
    AttrFloatArray *          pattrScaleOutFrom;
    AttrFloatArray *          pattrScaleOutTo;
    AttrFloat *               pattrScaleOutDelay;
    AttrFloat *               pattrScaleOutDuration;
    AttrFloat *               pattrScaleOutCooldown;

    AttrString *              pattrFadeOutType; // default to none/off
    AttrFloat *               pattrFadeOutFrom;
    AttrFloat *               pattrFadeOutTo;
    AttrFloat *               pattrFadeOutDelay;
    AttrFloat *               pattrFadeOutDuration;
    AttrFloat *               pattrFadeOutCooldown;


    Tween                     tweenMoveOut;
    Tween                     tweenRotateOut;
    Tween                     tweenScaleOut;
    Tween                     tweenFadeOut;

    RVec3                     vecMoveOutFrom;
    RVec3                     vecMoveOutTo;
    RVec3                     vecRotateOutFrom;
    RVec3                     vecRotateOutTo;
    RVec3                     vecScaleOutFrom;
    RVec3                     vecScaleOutTo;
    FLOAT                     fFadeOutFrom;
    FLOAT                     fFadeOutTo;

    TransformComponent *      pCachedTransform;

  public:
                          TweenSetComponent  ();

    virtual               ~TweenSetComponent ();

    static  const char *  Identifier             (VOID)                         {return szTweenSetComponentID;};

    virtual Component *   GetInterface           (const char *  szTypeIn)       {if streq(szTypeIn, szTweenSetComponentID) return ((Component *) this); return IntroOutroComponent::GetInterface (szTypeIn);};

    virtual Component *   Instantiate            (VOID) const                   {return new TweenSetComponent;};

    virtual Attr *        SetAttr                (const char *  szNameIn,
                                                  const char *  szValueIn);

    VOID                  AttrToVec              (AttrFloatArray &  attrIn,
                                                  RVec3 &           vecOut);

    virtual VOID          OnAwake                (VOID);

    virtual VOID          OnRenderStart          (VOID);

    virtual VOID          OnEvent                (HASH_T  hEventIn);


    VOID                  ApplyMove              (FLOAT    fTweenValueIn,
                                                  RVec3 &  vecFrom,
                                                  RVec3 &  vecTo);

    VOID                  ApplyEulerRotate       (FLOAT    fTweenValueIn,
                                                  RVec3 &  vecFrom,
                                                  RVec3 &  vecTo);

    VOID                  ApplyScale             (FLOAT    fTweenValueIn,
                                                  RVec3 &  vecFrom,
                                                  RVec3 &  vecTo);

    VOID                  Refresh                (VOID);

    VOID                  OnTweenComplete        (VOID);

  };
/** @} */ // end of gametech group

#endif // TWEENSETCOMPONENT_HPP
