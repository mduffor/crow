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

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Composite/Node.hpp"
#include "GameTech/TweenSetComponent.hpp"
#include "GameTech/UIHelper.hpp"
#include "Sys/Shell.hpp"

//-----------------------------------------------------------------------------
//  TweenSetComponent
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TweenSetComponent::TweenSetComponent ()
  {
  strType = szTweenSetComponentID;

  pCachedRegistry = NULL;

  pattrIntroOutroKey = (AttrString*) AddAttr ("introOutroKey", AttrString::Identifier());

  pattrPresetInName   = AddAttrString     ("presetInName");

  pattrMoveInType     = AddAttrString     ("moveInType");
  pattrMoveInFrom     = AddAttrFloatArray ("moveInFrom");
  pattrMoveInTo       = AddAttrFloatArray ("moveInTo");
  pattrMoveInDelay    = AddAttrFloat      ("moveInDelay");
  pattrMoveInDuration = AddAttrFloat      ("moveInDuration");
  pattrMoveInCooldown = AddAttrFloat      ("moveInCooldown");

  pattrRotateInType     = AddAttrString     ("rotateInType");
  pattrRotateInFrom     = AddAttrFloatArray ("rotateInFrom");
  pattrRotateInTo       = AddAttrFloatArray ("rotateInTo");
  pattrRotateInDelay    = AddAttrFloat      ("rotateInDelay");
  pattrRotateInDuration = AddAttrFloat      ("rotateInDuration");
  pattrRotateInCooldown = AddAttrFloat      ("rotateInCooldown");

  pattrScaleInType     = AddAttrString     ("scaleInType");
  pattrScaleInFrom     = AddAttrFloatArray ("scaleInFrom");
  pattrScaleInTo       = AddAttrFloatArray ("scaleInTo");
  pattrScaleInDelay    = AddAttrFloat      ("scaleInDelay");
  pattrScaleInDuration = AddAttrFloat      ("scaleInDuration");
  pattrScaleInCooldown = AddAttrFloat      ("scaleInCooldown");

  pattrFadeInType     = AddAttrString     ("fadeInType");
  pattrFadeInFrom     = AddAttrFloat      ("fadeInFrom");
  pattrFadeInTo       = AddAttrFloat      ("fadeInTo");
  pattrFadeInDelay    = AddAttrFloat      ("fadeInDelay");
  pattrFadeInDuration = AddAttrFloat      ("fadeInDuration");
  pattrFadeInCooldown = AddAttrFloat      ("fadeInCooldown");


  pattrPresetOutName   = AddAttrString     ("presetOutName");

  pattrMoveOutType     = AddAttrString     ("moveOutType");
  pattrMoveOutFrom     = AddAttrFloatArray ("moveOutFrom");
  pattrMoveOutTo       = AddAttrFloatArray ("moveOutTo");
  pattrMoveOutDelay    = AddAttrFloat      ("moveOutDelay");
  pattrMoveOutDuration = AddAttrFloat      ("moveOutDuration");
  pattrMoveOutCooldown = AddAttrFloat      ("moveOutCooldown");

  pattrRotateOutType     = AddAttrString     ("rotateOutType");
  pattrRotateOutFrom     = AddAttrFloatArray ("rotateOutFrom");
  pattrRotateOutTo       = AddAttrFloatArray ("rotateOutTo");
  pattrRotateOutDelay    = AddAttrFloat      ("rotateOutDelay");
  pattrRotateOutDuration = AddAttrFloat      ("rotateOutDuration");
  pattrRotateOutCooldown = AddAttrFloat      ("rotateOutCooldown");

  pattrScaleOutType     = AddAttrString     ("scaleOutType");
  pattrScaleOutFrom     = AddAttrFloatArray ("scaleOutFrom");
  pattrScaleOutTo       = AddAttrFloatArray ("scaleOutTo");
  pattrScaleOutDelay    = AddAttrFloat      ("scaleOutDelay");
  pattrScaleOutDuration = AddAttrFloat      ("scaleOutDuration");
  pattrScaleOutCooldown = AddAttrFloat      ("scaleOutCooldown");

  pattrFadeOutType     = AddAttrString     ("fadeOutType");
  pattrFadeOutFrom     = AddAttrFloat      ("fadeOutFrom");
  pattrFadeOutTo       = AddAttrFloat      ("fadeOutTo");
  pattrFadeOutDelay    = AddAttrFloat      ("fadeOutDelay");
  pattrFadeOutDuration = AddAttrFloat      ("fadeOutDuration");
  pattrFadeOutCooldown = AddAttrFloat      ("fadeOutCooldown");


  vecMoveInFrom.Zero ();
  vecMoveInTo.Zero ();
  vecRotateInFrom.Zero ();
  vecRotateInTo.Zero ();
  vecScaleInFrom.Zero ();
  vecScaleInTo.Set (1.0f, 1.0f, 1.0f);


  tweenMoveIn.eType   = Tween::kDisabled;
  tweenRotateIn.eType = Tween::kDisabled;
  tweenScaleIn.eType  = Tween::kDisabled;
  tweenFadeIn.eType   = Tween::kDisabled;

  tweenMoveOut.eType   = Tween::kDisabled;
  tweenRotateOut.eType = Tween::kDisabled;
  tweenScaleOut.eType  = Tween::kDisabled;
  tweenFadeOut.eType   = Tween::kDisabled;

  tweenMoveIn.sigOnComplete.Connect (this, &TweenSetComponent::OnTweenComplete);
  tweenRotateIn.sigOnComplete.Connect (this, &TweenSetComponent::OnTweenComplete);
  tweenScaleIn.sigOnComplete.Connect (this, &TweenSetComponent::OnTweenComplete);
  tweenFadeIn.sigOnComplete.Connect (this, &TweenSetComponent::OnTweenComplete);

  tweenMoveOut.sigOnComplete.Connect (this, &TweenSetComponent::OnTweenComplete);
  tweenRotateOut.sigOnComplete.Connect (this, &TweenSetComponent::OnTweenComplete);
  tweenScaleOut.sigOnComplete.Connect (this, &TweenSetComponent::OnTweenComplete);
  tweenFadeOut.sigOnComplete.Connect (this, &TweenSetComponent::OnTweenComplete);
  };

//-----------------------------------------------------------------------------
TweenSetComponent::~TweenSetComponent ()
  {
  tweenMoveIn.sigOnComplete.Disconnect (this, &TweenSetComponent::OnTweenComplete);
  tweenRotateIn.sigOnComplete.Disconnect (this, &TweenSetComponent::OnTweenComplete);
  tweenScaleIn.sigOnComplete.Disconnect (this, &TweenSetComponent::OnTweenComplete);
  tweenFadeIn.sigOnComplete.Disconnect (this, &TweenSetComponent::OnTweenComplete);

  tweenMoveOut.sigOnComplete.Disconnect (this, &TweenSetComponent::OnTweenComplete);
  tweenRotateOut.sigOnComplete.Disconnect (this, &TweenSetComponent::OnTweenComplete);
  tweenScaleOut.sigOnComplete.Disconnect (this, &TweenSetComponent::OnTweenComplete);
  tweenFadeOut.sigOnComplete.Disconnect (this, &TweenSetComponent::OnTweenComplete);
  };

//-----------------------------------------------------------------------------
Attr *  TweenSetComponent::SetAttr (const char *  szNameIn,
                                    const char *  szValueIn)
  {
  Attr * pattrChanged = IntroOutroComponent::SetAttr (szNameIn, szValueIn);

  // handle any changes that result from setting new values in attrs
  if (pattrChanged != NULL)
    {
    // MoveIn
    if (pattrChanged == (Attr *) pattrMoveInType)
      {
      tweenMoveIn.eType = Tween::StringToType (pattrMoveInType->Value().AsChar());
      };
    if (pattrChanged == (Attr *) pattrMoveInDelay)
      {
      tweenMoveIn.fDelay = pattrMoveInDelay->Value ();
      };
    if (pattrChanged == (Attr *) pattrMoveInDuration)
      {
      tweenMoveIn.fDuration = pattrMoveInDuration->Value ();
      };
    if (pattrChanged == (Attr *) pattrMoveInCooldown)
      {
      tweenMoveIn.fCooldown = pattrMoveInCooldown->Value ();
      };
    if (pattrChanged == (Attr *) pattrMoveInFrom)
      {
      AttrToVec (*pattrMoveInFrom, vecMoveInFrom);
      };
    if (pattrChanged == (Attr *) pattrMoveInTo)
      {
      AttrToVec (*pattrMoveInTo, vecMoveInTo);
      };

    // RotateIn
    if (pattrChanged == (Attr *) pattrRotateInType)
      {
      tweenRotateIn.eType = Tween::StringToType (pattrRotateInType->Value().AsChar());
      };
    if (pattrChanged == (Attr *) pattrRotateInDelay)
      {
      tweenRotateIn.fDelay = pattrRotateInDelay->Value ();
      };
    if (pattrChanged == (Attr *) pattrRotateInDuration)
      {
      tweenRotateIn.fDuration = pattrRotateInDuration->Value ();
      };
    if (pattrChanged == (Attr *) pattrRotateInCooldown)
      {
      tweenRotateIn.fCooldown = pattrRotateInCooldown->Value ();
      };
    if (pattrChanged == (Attr *) pattrRotateInFrom)
      {
      AttrToVec (*pattrRotateInFrom, vecRotateInFrom);
      };
    if (pattrChanged == (Attr *) pattrRotateInTo)
      {
      AttrToVec (*pattrRotateInTo, vecRotateInTo);
      };

    // ScaleIn
    if (pattrChanged == (Attr *) pattrScaleInType)
      {
      tweenScaleIn.eType = Tween::StringToType (pattrScaleInType->Value().AsChar());
      };
    if (pattrChanged == (Attr *) pattrScaleInDelay)
      {
      tweenScaleIn.fDelay = pattrScaleInDelay->Value ();
      };
    if (pattrChanged == (Attr *) pattrScaleInDuration)
      {
      tweenScaleIn.fDuration = pattrScaleInDuration->Value ();
      };
    if (pattrChanged == (Attr *) pattrScaleInCooldown)
      {
      tweenScaleIn.fCooldown = pattrScaleInCooldown->Value ();
      };
    if (pattrChanged == (Attr *) pattrScaleInFrom)
      {
      AttrToVec (*pattrScaleInFrom, vecScaleInFrom);
      };
    if (pattrChanged == (Attr *) pattrScaleInTo)
      {
      AttrToVec (*pattrScaleInTo, vecScaleInTo);
      };

    // FadeIn
    if (pattrChanged == (Attr *) pattrFadeInType)
      {
      tweenFadeIn.eType = Tween::StringToType (pattrFadeInType->Value().AsChar());
      };
    if (pattrChanged == (Attr *) pattrFadeInDelay)
      {
      tweenFadeIn.fDelay = pattrFadeInDelay->Value ();
      };
    if (pattrChanged == (Attr *) pattrFadeInDuration)
      {
      tweenFadeIn.fDuration = pattrFadeInDuration->Value ();
      };
    if (pattrChanged == (Attr *) pattrFadeInCooldown)
      {
      tweenFadeIn.fCooldown = pattrFadeInCooldown->Value ();
      };
    if (pattrChanged == (Attr *) pattrFadeInFrom)
      {
      fFadeInFrom = pattrFadeInFrom->Value ();
      };
    if (pattrChanged == (Attr *) pattrFadeInTo)
      {
      fFadeInTo = pattrFadeInTo->Value ();
      };


    // MoveOut
    if (pattrChanged == (Attr *) pattrMoveOutType)
      {
      tweenMoveOut.eType = Tween::StringToType (pattrMoveOutType->Value().AsChar());
      };
    if (pattrChanged == (Attr *) pattrMoveOutDelay)
      {
      tweenMoveOut.fDelay = pattrMoveOutDelay->Value ();
      };
    if (pattrChanged == (Attr *) pattrMoveOutDuration)
      {
      tweenMoveOut.fDuration = pattrMoveOutDuration->Value ();
      };
    if (pattrChanged == (Attr *) pattrMoveOutCooldown)
      {
      tweenMoveOut.fCooldown = pattrMoveOutCooldown->Value ();
      };
    if (pattrChanged == (Attr *) pattrMoveOutFrom)
      {
      AttrToVec (*pattrMoveOutFrom, vecMoveOutFrom);
      };
    if (pattrChanged == (Attr *) pattrMoveOutTo)
      {
      AttrToVec (*pattrMoveOutTo, vecMoveOutTo);
      };

    // RotateOut
    if (pattrChanged == (Attr *) pattrRotateOutType)
      {
      tweenRotateOut.eType = Tween::StringToType (pattrRotateOutType->Value().AsChar());
      };
    if (pattrChanged == (Attr *) pattrRotateOutDelay)
      {
      tweenRotateOut.fDelay = pattrRotateOutDelay->Value ();
      };
    if (pattrChanged == (Attr *) pattrRotateOutDuration)
      {
      tweenRotateOut.fDuration = pattrRotateOutDuration->Value ();
      };
    if (pattrChanged == (Attr *) pattrRotateOutCooldown)
      {
      tweenRotateOut.fCooldown = pattrRotateOutCooldown->Value ();
      };
    if (pattrChanged == (Attr *) pattrRotateOutFrom)
      {
      AttrToVec (*pattrRotateOutFrom, vecRotateOutFrom);
      };
    if (pattrChanged == (Attr *) pattrRotateOutTo)
      {
      AttrToVec (*pattrRotateOutTo, vecRotateOutTo);
      };

    // ScaleOut
    if (pattrChanged == (Attr *) pattrScaleOutType)
      {
      tweenScaleOut.eType = Tween::StringToType (pattrScaleOutType->Value().AsChar());
      };
    if (pattrChanged == (Attr *) pattrScaleOutDelay)
      {
      tweenScaleOut.fDelay = pattrScaleOutDelay->Value ();
      };
    if (pattrChanged == (Attr *) pattrScaleOutDuration)
      {
      tweenScaleOut.fDuration = pattrScaleOutDuration->Value ();
      };
    if (pattrChanged == (Attr *) pattrScaleOutCooldown)
      {
      tweenScaleOut.fCooldown = pattrScaleOutCooldown->Value ();
      };
    if (pattrChanged == (Attr *) pattrScaleOutFrom)
      {
      AttrToVec (*pattrScaleOutFrom, vecScaleOutFrom);
      };
    if (pattrChanged == (Attr *) pattrScaleOutTo)
      {
      AttrToVec (*pattrScaleOutTo, vecScaleOutTo);
      };

    // FadeOut
    if (pattrChanged == (Attr *) pattrFadeOutType)
      {
      tweenFadeOut.eType = Tween::StringToType (pattrFadeOutType->Value().AsChar());
      };
    if (pattrChanged == (Attr *) pattrFadeOutDelay)
      {
      tweenFadeOut.fDelay = pattrFadeOutDelay->Value ();
      };
    if (pattrChanged == (Attr *) pattrFadeOutDuration)
      {
      tweenFadeOut.fDuration = pattrFadeOutDuration->Value ();
      };
    if (pattrChanged == (Attr *) pattrFadeOutCooldown)
      {
      tweenFadeOut.fCooldown = pattrFadeOutCooldown->Value ();
      };
    if (pattrChanged == (Attr *) pattrFadeOutFrom)
      {
      fFadeOutFrom = pattrFadeOutFrom->Value ();
      };
    if (pattrChanged == (Attr *) pattrFadeOutTo)
      {
      fFadeOutTo = pattrFadeOutTo->Value ();
      };
    }
  return pattrChanged;
  };

//-----------------------------------------------------------------------------
VOID TweenSetComponent::AttrToVec (AttrFloatArray &  attrIn,
                                   RVec3 &           vecOut)
  {
  INT  iNumValues = attrIn.Value().Length();
  if (iNumValues >= 0)
    {
    vecOut.fX = attrIn.ArrayValue (0);
    if (iNumValues >= 1)
      {
      vecOut.fY = attrIn.ArrayValue (1);
      if (iNumValues >= 2)
        {
        vecOut.fZ = attrIn.ArrayValue (2);
        };
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  TweenSetComponent::OnAwake  (VOID)
  {
  IntroOutroComponent::OnAwake ();

  TransformComponent::CacheTransform (this, &pCachedTransform);
  Refresh ();
  };

//-----------------------------------------------------------------------------
VOID TweenSetComponent::OnEvent (HASH_T  hEventIn)
  {
  //DBG_INFO ("TweenSetComponent::OnEvent");

  // This method is concerned with responding to event commands by
  //  tracking whichIntroOutroMode mode we are going into, and resetting
  //  the animations to the start of the mode.

  Node *  pnodeParent = (Node *)ParentNode();

  if (hEventIn == HASH("OnIntro"))
    {
    // TODO: Mark as active

    pnodeParent->SetActive (TRUE);
    SetIntroOutroMode (HASH("Intro"));

    tweenMoveIn.Reset   ();
    tweenRotateIn.Reset ();
    tweenScaleIn.Reset  ();
    tweenFadeIn.Reset   ();

    DBG_INFO ("Mode = Intro");
    }
  else if (hEventIn == HASH("OnIdle"))
    {
    // TODO: Mark as active
    pnodeParent->SetActive (TRUE);
    SetIntroOutroMode (HASH("Idle"));
    DBG_INFO ("Mode = Idle");
    }
  else if (hEventIn == HASH("OnOutro"))
    {
    // TODO: Mark as active
    pnodeParent->SetActive (TRUE);
    SetIntroOutroMode (HASH("Outro"));

    tweenMoveOut.Reset   ();
    tweenRotateOut.Reset ();
    tweenScaleOut.Reset  ();
    tweenFadeOut.Reset   ();

    DBG_INFO ("Mode = Outro");
    }
  else if (hEventIn == HASH("OnHidden"))
    {
    // TODO: Mark as not active
    pnodeParent->SetActive (FALSE);
    SetIntroOutroMode (HASH("Hidden"));
    DBG_INFO ("Mode = Hidden");
    };
  };

//-----------------------------------------------------------------------------
VOID  TweenSetComponent::OnRenderStart  (VOID)
  {
  // This method is repsonsible for firing every frame, and advancing the
  //  appropriate tween animations depending on which IntroOutroMode we are in,
  //  and then applying the changes to the nodes/components so the display is
  //  updated for the next time we render.

  // This method simply increments the tweens.  When tweens finish playing,
  //  they fire OnComplete messages which are handled by OnTweenComplete().

  if (pCachedTransform == NULL) return;

  HASH_T  uMode     = GetIntroOutroMode ();
  BOOL    bUpdated  = FALSE;
  FLOAT   fDeltaSec = Shell::GetRenderTimeDeltaSec ();
  if (uMode == HASH("Intro"))
    {
    tweenMoveIn.IncTime   (fDeltaSec);
    tweenRotateIn.IncTime (fDeltaSec);
    tweenScaleIn.IncTime  (fDeltaSec);
    tweenFadeIn.IncTime   (fDeltaSec);

    ApplyMove        (tweenMoveIn.GetValue (),   vecMoveInFrom,   vecMoveInTo);
    ApplyEulerRotate (tweenRotateIn.GetValue (), vecRotateInFrom, vecRotateInTo);
    ApplyScale       (tweenScaleIn.GetValue (),  vecScaleInFrom,  vecScaleInTo);
    // TODO: Fade

    bUpdated = TRUE;
    }
  else if (uMode == HASH("Outro"))
    {
    tweenMoveOut.IncTime   (fDeltaSec);
    tweenRotateOut.IncTime (fDeltaSec);
    tweenScaleOut.IncTime  (fDeltaSec);
    tweenFadeOut.IncTime   (fDeltaSec);

    ApplyMove        (tweenMoveOut.GetValue (),   vecMoveOutFrom,   vecMoveOutTo);
    ApplyEulerRotate (tweenRotateOut.GetValue (), vecRotateOutFrom, vecRotateOutTo);
    ApplyScale       (tweenScaleOut.GetValue (),  vecScaleOutFrom,  vecScaleOutTo);
    // TODO: Fade

    bUpdated = TRUE;
    }

  if (bUpdated)
    {
    Refresh ();
    };
  };

//-----------------------------------------------------------------------------
VOID  TweenSetComponent::ApplyMove (FLOAT    fTweenValueIn,
                                    RVec3 &  vecFrom,
                                    RVec3 &  vecTo)
  {
  RVec3  vecNew = LERP (vecFrom, vecTo, fTweenValueIn);

  // set position on transform
  pCachedTransform->SetTx (vecNew.fX);
  pCachedTransform->SetTy (vecNew.fY);
  pCachedTransform->SetTz (vecNew.fZ);
  };

//-----------------------------------------------------------------------------
VOID  TweenSetComponent::ApplyEulerRotate (FLOAT    fTweenValueIn,
                                           RVec3 &  vecFrom,
                                           RVec3 &  vecTo)
  {
  // NOTE: This lerps the Euler angle values.  This is subject to gimbal lock
  //        and weird rotations if taken too far.  It also does not take
  //        the shortest path to the destination if wrap-around is shorter.
  //        If in the future you need a 3D SLERP, you'll need to write a
  //        different Component.
  RVec3  vecNew = LERP (vecFrom, vecTo, fTweenValueIn);

  // set position on transform
  pCachedTransform->SetRx (vecNew.fX);
  pCachedTransform->SetRy (vecNew.fY);
  pCachedTransform->SetRz (vecNew.fZ);
  };

//-----------------------------------------------------------------------------
VOID  TweenSetComponent::ApplyScale (FLOAT    fTweenValueIn,
                                     RVec3 &  vecFrom,
                                     RVec3 &  vecTo)
  {
  RVec3  vecNew = LERP (vecFrom, vecTo, fTweenValueIn);

  // set position on transform
  pCachedTransform->SetSx (vecNew.fX);
  pCachedTransform->SetSy (vecNew.fY);
  pCachedTransform->SetSz (vecNew.fZ);
  };

//-----------------------------------------------------------------------------
VOID  TweenSetComponent::Refresh (VOID)
  {
  if (pCachedTransform != NULL)
    {
    pCachedTransform->MarkAsDirty ();
    }
  };

//-----------------------------------------------------------------------------
VOID  TweenSetComponent::OnTweenComplete (VOID)
  {
  // This method is called whenever one of the tweens reaches its end.
  //  This method is then responsible for detecting when the last tween in a
  //  set is done playing, and then firing the Event command to switch all the
  //  components on this node into the next state.
  HASH_T  uMode     = GetIntroOutroMode ();

  // Since a tween has completed, determine if it was the last tween
  //  in the set, and if so, advance the anim state and fire appropriate events.
  if (uMode == HASH("Intro"))
    {
    if (tweenMoveIn.IsPlaying () ||
        tweenRotateIn.IsPlaying () ||
        tweenScaleIn.IsPlaying () ||
        tweenFadeIn.IsPlaying ())
      {
      // one of the tweens is still going.
      return;
      };
    UIHelper::Instance()->PlayIdle ((Node*) ParentNode());
    }
  else if (uMode == HASH("Idle"))
    {
    }
  else if (uMode == HASH("Outro"))
    {
    if (tweenMoveOut.IsPlaying () ||
        tweenRotateOut.IsPlaying () ||
        tweenScaleOut.IsPlaying () ||
        tweenFadeOut.IsPlaying ())
      {
      // one of the tweens is still going.
      return;
      };
    UIHelper::Instance()->PlayHidden ((Node*) ParentNode());
    }
  else if (uMode == HASH("Hidden"))
    {
    // disable node?
    };

  /*
  // NOTE: Possibly put all the expression-on-mode-change commands in their own component,
  //  to keep the complexity modularized.
  if (!pattrExprOnComplete->IsEmpty ())
    {
    pCachedRegistry->SetPtr (H("SourceComponent", 0), (PVOID) this);
    Expression::Execute (pattrExprOnComplete->Value().AsChar(),
                         pCachedRegistry,
                         NULL,  // Token *     pTokenOut
                         NULL); // KVPArray *  pEnvVarsIn
    };
  */
  };
