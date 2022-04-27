/* -----------------------------------------------------------------
                         IntroOutroComponent

     This component serves as the base for components that display
   intro and outro animations.

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
#include "GameTech/IntroOutroComponent.hpp"
#include "GameTech/UIHelper.hpp"
#include "Component/KeySubComponent.hpp"

#define OPT(...)      ((void)0)
//#define OPT(a)  (a)

//-----------------------------------------------------------------------------
//  IntroOutroComponent
//-----------------------------------------------------------------------------


/*

How does this work?

Key given in attr.  Cache the elem for ease of use.
  We don't react to elem changes though.
Elem holds the current anim state.  We set it as tweens end.

Anim state changes driven by OnEvent.
Animation/Tween advanced by OnRenderStart and delta time.
When animation is done, OnEvent is fired for all components on node.

*/


//-----------------------------------------------------------------------------
IntroOutroComponent::IntroOutroComponent ()
  {
  strType = szIntroOutroComponentID;

  pCachedRegistry = NULL;

  pattrIntroOutroKey = (AttrString*) AddAttr ("introOutroKey", AttrString::Identifier());

  pelemIntroOutro = NULL;
  };

//-----------------------------------------------------------------------------
IntroOutroComponent::~IntroOutroComponent ()
  {
  DisconnectFromRegistry ();
  };

//-----------------------------------------------------------------------------
Attr *  IntroOutroComponent::SetAttr (const char *  szNameIn,
                                      const char *  szValueIn)
  {
  Attr * pattrChanged = Component::SetAttr (szNameIn, szValueIn);

  // handle any changes that result from setting new values in attrs
  if (pattrChanged != NULL)
    {
    if (pattrChanged == (Attr *) pattrIntroOutroKey)
      {
      DisconnectFromRegistry ();
      };
    }
  return pattrChanged;
  };

//-----------------------------------------------------------------------------
VOID  IntroOutroComponent::OnAwake  (VOID)
  {
  CacheRegistry ();
  CacheStateElements ();
  };

//-----------------------------------------------------------------------------
VOID IntroOutroComponent::OnRecache (VOID)
  {
  DisconnectFromRegistry ();
  };

//-----------------------------------------------------------------------------
HASH_T IntroOutroComponent::GetIntroOutroMode (VOID)
  {
  if (!CacheRegistry      ())  return 0;
  if (!CacheStateElements ())  return 0;

  return (pelemIntroOutro->GetInt());
  };

//-----------------------------------------------------------------------------
VOID IntroOutroComponent::SetIntroOutroMode (HASH_T  uModeIn)
  {
  if (!CacheRegistry      ())  return;
  if (!CacheStateElements ())  return;

  pelemIntroOutro->SetInt (uModeIn);
  };

//-----------------------------------------------------------------------------
BOOL IntroOutroComponent::CacheRegistry (VOID)
  {
  // if we haven't cached the named value registry, do so now
  if (pCachedRegistry == NULL)
    {
    pCachedRegistry = ValueRegistry::Root ();
    };
  return (pCachedRegistry != NULL);
  };

//-----------------------------------------------------------------------------
BOOL IntroOutroComponent::CacheStateElements (VOID)
  {
  if (pCachedRegistry == NULL) return (FALSE);
  if (pelemIntroOutro == NULL)
    {
    RStrParser  parserIntroOutroKeyFinal (pattrIntroOutroKey->GetString ());
    KeySubComponent::ExpandVars  ((Node*)ParentNode (), parserIntroOutroKeyFinal);

    pelemIntroOutro = pCachedRegistry->Find (parserIntroOutroKeyFinal);
    };
  return (pelemIntroOutro != NULL);
  };

//-----------------------------------------------------------------------------
VOID  IntroOutroComponent::DisconnectFromRegistry (VOID)
  {
  pelemIntroOutro = NULL;
  };




//-----------------------------------------------------------------------------
VOID IntroOutroComponent::OnEvent (HASH_T  hEventIn)
  {

  OPT(DBG_INFO ("IntroOutroComponent::OnEvent"));

  // start animations or tweens as needed
  // Store new IntroOutroMode as needed.

  Node *  pnodeParent = (Node *)ParentNode();

  if (hEventIn == HASH("OnIntro"))
    {
    // TODO: Mark as active
    OPT(DBG_INFO ("OnEvent Intro for %s", Node::GetParentFullName(this)));

    pnodeParent->SetActive (TRUE);
    SetIntroOutroMode (HASH("Intro"));
    OPT(DBG_INFO ("Mode = Intro"));
    }
  else if (hEventIn == HASH("OnIdle"))
    {
    OPT(DBG_INFO ("OnEvent Idle for %s", Node::GetParentFullName(this)));
    // TODO: Mark as active
    pnodeParent->SetActive (TRUE);
    SetIntroOutroMode (HASH("Idle"));
    OPT(DBG_INFO ("Mode = Idle"));
    }
  else if (hEventIn == HASH("OnOutro"))
    {
    OPT(DBG_INFO ("OnEvent Outro for %s", Node::GetParentFullName(this)));

    // TODO: Mark as active
    pnodeParent->SetActive (TRUE);
    SetIntroOutroMode (HASH("Outro"));
    OPT(DBG_INFO ("Mode = Outro"));
    }
  else if (hEventIn == HASH("OnHidden"))
    {
    OPT(DBG_INFO ("OnEvent Hidden for %s", Node::GetParentFullName(this)));

    // TODO: Mark as not active
    pnodeParent->SetActive (FALSE);
    SetIntroOutroMode (HASH("Hidden"));
    OPT(DBG_INFO ("Mode = Hidden"));
    };
  };

//-----------------------------------------------------------------------------
const char *  IntroOutroComponent::ModeHashToString (HASH_T  hModeIn)
  {
  if (hModeIn == HASH("Intro"))
    {
    return ("Intro");
    }
  else if (hModeIn == HASH("Idle"))
    {
    return ("Idle");
    }
  else if (hModeIn == HASH("Outro"))
    {
    return ("Outro");
    }
  else if (hModeIn == HASH("Hidden"))
    {
    return ("Hidden");
    };
  return ("Unknown");
  };

//-----------------------------------------------------------------------------
VOID  IntroOutroComponent::OnRenderStart  (VOID)
  {
  HASH_T  uMode = GetIntroOutroMode ();

  // advance animations or tweens

  // Fire events when anim state changes.

  // Test code

  if (uMode == HASH("Intro"))
    {
    UIHelper::Instance()->PlayIdle ((Node*) ParentNode());
    }
  else if (uMode == HASH("Idle"))
    {
    }
  else if (uMode == HASH("Outro"))
    {
    UIHelper::Instance()->PlayHidden ((Node*) ParentNode());
    }
  else if (uMode == HASH("Hidden"))
    {
    // disable node?
    };
  };



