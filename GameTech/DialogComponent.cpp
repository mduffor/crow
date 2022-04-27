/* -----------------------------------------------------------------
                         DialogComponent

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
#include "GameTech/DialogComponent.hpp"
#include "GameTech/UIHelper.hpp"

#define OPT(...)      ((void)0)
//#define OPT(a)  (a)

//-----------------------------------------------------------------------------
//  DialogComponent
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
DialogComponent::DialogComponent () : WindowBaseComponent ()
  {
  strType = szDialogComponentID;

  pattrDialogStateKey = (AttrString*) AddAttr ("dialogStateKey", AttrString::Identifier());
  pelemDialogState = NULL;
  };

//-----------------------------------------------------------------------------
DialogComponent::~DialogComponent ()
  {
  DisconnectFromRegistry ();
  };

//-----------------------------------------------------------------------------
Attr *  DialogComponent::SetAttr (const char *  szNameIn,
                                  const char *  szValueIn)
  {
  Attr * pattrChanged = Component::SetAttr (szNameIn, szValueIn);

  // handle any changes that result from setting new values in attrs
  if (pattrChanged != NULL)
    {
    if (pattrChanged == (Attr *) pattrDialogStateKey)
      {
      DisconnectFromRegistry ();
      };
    }
  return pattrChanged;
  };

//-----------------------------------------------------------------------------
VOID DialogComponent::OnEvent (HASH_T  hEventIn)
  {
  //DBG_INFO ("DialogComponent::OnEvent");
  WindowBaseComponent::OnEvent (hEventIn);

  if (hEventIn == HASH("OnHidden"))
    {
    if (pCachedRegistry != NULL)
      {
      pCachedRegistry->DeleteStringInArray (szVisibleDialogsKey,
                                            pattrDialogStateKey->Value());
      };
    }
  else if (hEventIn == HASH("OnIntro"))
    {
    if (pCachedRegistry != NULL)
      {
      pCachedRegistry->DeleteStringInArray (szVisibleDialogsKey,
                                            pattrDialogStateKey->Value());
      pCachedRegistry->SetStringInArray    (szVisibleDialogsKey,
                                            pattrDialogStateKey->Value());
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  DialogComponent::OnUpdate  (VOID)
  {
  if (!pattrDialogStateKey->IsEmpty ())
    {
    CacheRegistry ();
    };

  // if we haven't been able to hook up to the cached value yet, keep trying until we do
  if ((pelemDialogState == NULL) && (!pattrDialogStateKey->IsEmpty ()))
    {
    OPT(DBG_INFO ("DialogComponent::UpdateFromRegistry from OnUpdate"));

    UpdateFromRegistry ();
    }
  };

//-----------------------------------------------------------------------------
VOID DialogComponent::UpdateFromRegistry (VOID)
  {
  CacheRegistry ();
  OPT(DBG_INFO ("DialogComponent::UpdateFromRegistry ()"));
  if (pCachedRegistry != NULL)
    {
    //DBG_INFO ("DialogComponent::UpdateFromRegistry () Registry was cached %s : %s",
    //(pelemDialogState == NULL)  ? "true" : "false",
    //!pattrDialogStateKey->IsEmpty ()  ? "true" : "false");

    // cache the value if needed, and sign up for it changing
    if ((pelemDialogState == NULL) && (!pattrDialogStateKey->IsEmpty ()))
      {
      ValueElem *  pelemSearch = pCachedRegistry->Find (pattrDialogStateKey->GetString ());

      //DBG_INFO ("DialogComponent::UpdateFromRegistry () Searched elem is %x", (unsigned long) pelemSearch);
      if (pelemSearch != NULL)
        {
        DisconnectFromRegistry ();
        pelemDialogState = pelemSearch;
        pelemDialogState->sigOnChanged.Connect (this, &DialogComponent::OnChangedListener);
        pelemDialogState->sigOnDelete.Connect (this, &DialogComponent::OnDeletedListener);

        //DBG_INFO ("DialogComponent  Set up listeners for %s", pelemSearch->GetName());

        }
      else
        {
        DBG_WARNING ("Unable to find attibute named %s in value registry", pattrDialogStateKey->GetString());
        };
      };

    // if we are hooked up, refresh the active state.
    if (pelemDialogState != NULL)
      {

      HASH_T  uMode = GetIntroOutroMode ();

      /*
      DBG_INFO ("DialogComponent  Looking up dialog state for UpdateFromRegistry.  Mode %x", uMode);
      DBG_INFO ("DialogComponent  Intro %x", HASH("Intro"));
      DBG_INFO ("DialogComponent  Idle %x", HASH("Idle"));
      DBG_INFO ("DialogComponent  Outro %x", HASH("Outro"));
      DBG_INFO ("DialogComponent  Hidden %x", HASH("Hidden"));
      */


      if (pelemDialogState->GetBool())
        {
        if (uMode == HASH("Hidden"))
          {
          UIHelper::Instance()->PlayIntro ((Node *)ParentNode());
          };
        }
      else
        {
        if (uMode == HASH("Idle"))
          {
          UIHelper::Instance()->PlayOutro ((Node *)ParentNode());
          };
        };
      };

    };
  };

//-----------------------------------------------------------------------------
VOID  DialogComponent::DisconnectFromRegistry (VOID)
  {
  // Disconnect from the registered signal, if we are connected.
  if (pelemDialogState != NULL)
    {
    pelemDialogState->sigOnChanged.Disconnect (this, &DialogComponent::OnChangedListener);
    pelemDialogState->sigOnDelete.Disconnect (this, &DialogComponent::OnDeletedListener);
    pelemDialogState = NULL;
    }
  pelemIntroOutro = NULL;
  };

