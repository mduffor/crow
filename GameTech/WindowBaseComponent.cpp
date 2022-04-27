/* -----------------------------------------------------------------
                         WindowBaseComponent

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
#include "GameTech/WindowBaseComponent.hpp"
#include "GameTech/UIHelper.hpp"
#include "GameTech/IntroOutroComponent.hpp"
#include "Component/KeySubComponent.hpp"
#include "Script/Expression.hpp"

#define OPT_DBG_INFO(...)      ((void)0)
//#define OPT_DBG_INFO  DBG_INFO

//-----------------------------------------------------------------------------
//  WindowBaseComponent
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
WindowBaseComponent::WindowBaseComponent ()
  {
  strType = szWindowBaseComponentID;

  pCachedRegistry = NULL;

  pattrIntroOutroKey = (AttrString*) AddAttr ("introOutroKey", AttrString::Identifier());

  pattrOnIntroExpression    = (AttrString*) AddAttr ("onIntroExpr",    AttrString::Identifier());
  pattrOnIdleExpression    = (AttrString*) AddAttr ("onIdleExpr",    AttrString::Identifier());
  pattrOnOutroExpression    = (AttrString*) AddAttr ("onOutroExpr",    AttrString::Identifier());
  pattrOnHiddenExpression    = (AttrString*) AddAttr ("onHiddenExpr",    AttrString::Identifier());

  pelemIntroOutro = NULL;
  };

//-----------------------------------------------------------------------------
WindowBaseComponent::~WindowBaseComponent ()
  {
  DisconnectFromRegistry ();
  };

//-----------------------------------------------------------------------------
VOID  WindowBaseComponent::OnAwake  (VOID)
  {
  // If the intro outro state is not yet defined or set, assume that it is hidden
  //   and apply that state.
  CacheRegistry ();
  if (pCachedRegistry != NULL)
    {
    // cache the value if needed, and sign up for it changing
    if (!pattrIntroOutroKey->IsEmpty ())
      {
      RStrParser  parserIntroOutroKeyFinal (pattrIntroOutroKey->GetString ());
      //DBG_INFO ("KeySub Expand Before \"%s\"", parserIntroOutroKeyFinal.AsChar());
      KeySubComponent::ExpandVars  ((Node*)ParentNode (), parserIntroOutroKeyFinal);
      //DBG_INFO ("KeySub Expand After \"%s\"", parserIntroOutroKeyFinal.AsChar());

      OPT_DBG_INFO ("WindowBaseComponent::OnAwake () key is \"%s\" (orig \"%s\")", parserIntroOutroKeyFinal.AsChar(), pattrIntroOutroKey->GetString ());

      INT  iMode = pCachedRegistry->GetInt (parserIntroOutroKeyFinal);

      if (iMode == 0)
        {
        pCachedRegistry->SetInt (parserIntroOutroKeyFinal, HASH("Hidden"));

        Node *  pnodeParent = (Node*) ParentNode ();
        pnodeParent->SetActive (FALSE);
        };

      OPT_DBG_INFO ("WindowBaseComponent::OnAwake () mode is now %s", IntroOutroComponent::ModeHashToString(pCachedRegistry->GetInt (parserIntroOutroKeyFinal)));
      };
    };
  UpdateFromRegistry ();
  };

//-----------------------------------------------------------------------------
VOID WindowBaseComponent::OnChangedListener (ValueElem *  pelemIn,
                                             BOOL         bUpdatingIn)
  {
  OPT_DBG_INFO ("WindowBaseComponent::UpdateFromRegistry from OnChangedListener");

  UpdateFromRegistry ();
  }

//-----------------------------------------------------------------------------
VOID WindowBaseComponent::OnDeletedListener (ValueElem *  pelemIn)
  {
  DisconnectFromRegistry ();
  };

//-----------------------------------------------------------------------------
VOID WindowBaseComponent::OnRecache (VOID)
  {
  DisconnectFromRegistry ();
  };

//-----------------------------------------------------------------------------
VOID WindowBaseComponent::OnEvent (HASH_T  hEventIn)
  {
  //DBG_INFO ("WindowBaseComponent::OnEvent");

  if (hEventIn == HASH("OnIntro"))
    {
    RunExpression (pattrOnIntroExpression);
    }
  else if (hEventIn == HASH("OnIdle"))
    {
    RunExpression (pattrOnIdleExpression);
    OPT_DBG_INFO ("WindowBaseComponent::UpdateFromRegistry from OnEvent OnIdle");
    UpdateFromRegistry ();
    }
  else if (hEventIn == HASH("OnOutro"))
    {
    RunExpression (pattrOnOutroExpression);
    }
  else if (hEventIn == HASH("OnHidden"))
    {
    OPT_DBG_INFO ("WindowBaseComponent::UpdateFromRegistry from OnEvent OnHidden");
    RunExpression (pattrOnHiddenExpression);
    UpdateFromRegistry ();
    };
  };

//-----------------------------------------------------------------------------
HASH_T WindowBaseComponent::GetIntroOutroMode (VOID)
  {
  // cache if null
  if (pCachedRegistry == NULL) return 0;
  if (pelemIntroOutro == NULL)
    {
    RStrParser  parserIntroOutroKeyFinal (pattrIntroOutroKey->GetString ());
    KeySubComponent::ExpandVars  ((Node*)ParentNode (), parserIntroOutroKeyFinal);

    pelemIntroOutro = pCachedRegistry->Find (parserIntroOutroKeyFinal);
    };

  return ((pelemIntroOutro == NULL) ? 0 : pelemIntroOutro->GetInt());
  };

//-----------------------------------------------------------------------------
VOID WindowBaseComponent::CacheRegistry (VOID)
  {
  // if we haven't cached the named value registry, do so now
  if (pCachedRegistry == NULL)
    {
    pCachedRegistry = ValueRegistry::Root ();
    };
  };

//-----------------------------------------------------------------------------
VOID WindowBaseComponent::RunExpression (AttrString *  pattrExpressionIn)
  {
  if (pCachedRegistry == NULL) return;
  if (pattrExpressionIn->Value().IsEmpty()) return;

  RStrParser  parserFinalExpression (pattrExpressionIn->Value());

  KeySubComponent::ExpandVars  ((Node*)ParentNode (), parserFinalExpression);
  parserFinalExpression.ResetCursor();
  pCachedRegistry->ExpandVars (parserFinalExpression);

  OPT_DBG_INFO ("WindowBaseComponent running expression \"%s\"", parserFinalExpression.AsChar ());

  // NOTE:  Keep in mind that Execute might cause the current object instance
  //         to be deleted.

  pCachedRegistry->SetPtr ("SourceComponent", (PVOID) (Component *) this);
  Token            tokenOut;
  Expression::Execute (parserFinalExpression.AsChar (), pCachedRegistry, &tokenOut);
  };
