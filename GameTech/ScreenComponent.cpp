/* -----------------------------------------------------------------
                         ScreenComponent

     This module implements a component for activating or
   deactivating nodes based on an entry in a value registry.

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
#include "GameTech/ScreenComponent.hpp"
#include "GameTech/UIHelper.hpp"
#include "Component/KeySubComponent.hpp"
#include "GameTech/IntroOutroComponent.hpp"

#define OPT_DBG_INFO(...)      ((void)0)
//#define OPT_DBG_INFO  DBG_INFO

//-----------------------------------------------------------------------------
//  ScreenComponent
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
ScreenComponent::ScreenComponent () : WindowBaseComponent ()
  {
  strType = szScreenComponentID;

  pattrUiGroupKey    = (AttrString*) AddAttr ("groupKey",      AttrString::Identifier());
  pattrScreenName    = (AttrString*) AddAttr ("screenName",    AttrString::Identifier());

  pelemUiGroup    = NULL;

  pattrUiGroupKey->Set ("UI.Screen");
  };

//-----------------------------------------------------------------------------
ScreenComponent::~ScreenComponent ()
  {
  DisconnectFromRegistry ();
  };

//-----------------------------------------------------------------------------
Attr *  ScreenComponent::SetAttr (const char *  szNameIn,
                                  const char *  szValueIn)
  {
  Attr * pattrChanged = Component::SetAttr (szNameIn, szValueIn);

  // handle any changes that result from setting new values in attrs
  if (pattrChanged != NULL)
    {
    if (pattrChanged == (Attr *) pattrUiGroupKey)
      {
      DisconnectFromRegistry ();
      };
    }
  return pattrChanged;
  };

//-----------------------------------------------------------------------------
VOID  ScreenComponent::OnUpdate  (VOID)
  {
  if (!pattrUiGroupKey->IsEmpty ())
    {
    CacheRegistry ();
    };

  // if we haven't been able to hook up to the cached value yet, keep trying until we do
  if ((pelemUiGroup == NULL) && (!pattrUiGroupKey->IsEmpty ()))
    {
    UpdateFromRegistry ();
    }
  };

//-----------------------------------------------------------------------------
VOID ScreenComponent::UpdateFromRegistry (VOID)
  {
  CacheRegistry ();
  OPT_DBG_INFO ("ScreenComponent::UpdateFromRegistry ()");
  if (pCachedRegistry != NULL)
    {
    // cache the value if needed, and sign up for it changing
    if ((pelemUiGroup == NULL) && (!pattrUiGroupKey->IsEmpty ()))
      {
      RStrParser  parserGroupKeyFinal (pattrUiGroupKey->GetString ());
      KeySubComponent::ExpandVars  ((Node*)ParentNode (), parserGroupKeyFinal);

      ValueElem *  pelemSearch = pCachedRegistry->Find (parserGroupKeyFinal);

      if (pelemSearch != NULL)
        {
        DisconnectFromRegistry ();
        pelemUiGroup = pelemSearch;
        pelemUiGroup->sigOnChanged.Connect (this, &ScreenComponent::OnChangedListener);
        pelemUiGroup->sigOnDelete.Connect (this, &ScreenComponent::OnDeletedListener);

        strNameCache.Set (pelemUiGroup->GetString(), TRUE);
        }
      else
        {
        DBG_WARNING ("Unable to find attibute named %s in value registry", parserGroupKeyFinal.AsChar ());
        };
      };

    // if we are hooked up, refresh the active state.
    if (pelemUiGroup != NULL)
      {
      HASH_T  uMode = GetIntroOutroMode ();

      RStrParser  parserScreenNameFinal (pattrScreenName->Value());
      KeySubComponent::ExpandVars  ((Node*)ParentNode (), parserScreenNameFinal);

      RStr  strPath; ((Node *) ParentNode ())->CalcFullPath (strPath);
      OPT_DBG_INFO ("ScreenComponent::UpdateFromRegistry () screen trigger set to \"%s\".  Checking if this affects \"%s\" (%s) (currently at mode %s)",
                    pelemUiGroup->GetString(), parserScreenNameFinal.AsChar(), strPath.AsChar (), IntroOutroComponent::ModeHashToString (uMode));

      if (parserScreenNameFinal.Equals (pelemUiGroup->GetString()))
        {
        // the group name matches this component's screen name
        if (uMode == HASH("Hidden"))
          {
          OPT_DBG_INFO("Play Intro for %s", pelemUiGroup->GetString());
          UIHelper::Instance()->PlayIntro ((Node *)ParentNode());
          };
        }
      else
        {
        // the group name does not match this component
        if (uMode == HASH("Idle"))
          {
          OPT_DBG_INFO("Play Outro for %s", pelemUiGroup->GetString());
          UIHelper::Instance()->PlayOutro ((Node *)ParentNode());
          };
        };
      };

    };
  };

//-----------------------------------------------------------------------------
VOID  ScreenComponent::DisconnectFromRegistry (VOID)
  {
  // Disconnect from the registered signal, if we are connected.
  if (pelemUiGroup != NULL)
    {
    pelemUiGroup->sigOnChanged.Disconnect (this, &ScreenComponent::OnChangedListener);
    pelemUiGroup->sigOnDelete.Disconnect (this, &ScreenComponent::OnDeletedListener);
    pelemUiGroup = NULL;
    }
  pelemIntroOutro = NULL;
  };

