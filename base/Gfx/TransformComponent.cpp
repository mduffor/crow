/* -----------------------------------------------------------------
                             Transform Component

     This module implements a component for holding Transform data.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2014, Michael T. Duffy II.  All rights reserved.
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
#include "Gfx/TransformComponent.hpp"

//-----------------------------------------------------------------------------
//  TransformComponent
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TransformComponent::TransformComponent ()
  {
  strType = szTransformComponentID;

  iTranslationVersion = -1;
  iRotationVersion    = -1;
  iScaleVersion       = -1;

  pattrTx = dynamic_cast<AttrFloat*>(AddAttr ("tx", AttrFloat::Identifier()));
  pattrTy = dynamic_cast<AttrFloat*>(AddAttr ("ty", AttrFloat::Identifier()));
  pattrTz = dynamic_cast<AttrFloat*>(AddAttr ("tz", AttrFloat::Identifier()));

  pattrRx = dynamic_cast<AttrFloat*>(AddAttr ("rx", AttrFloat::Identifier()));
  pattrRy = dynamic_cast<AttrFloat*>(AddAttr ("ry", AttrFloat::Identifier()));
  pattrRz = dynamic_cast<AttrFloat*>(AddAttr ("rz", AttrFloat::Identifier()));

  pattrSx = dynamic_cast<AttrFloat*>(AddAttr ("sx", AttrFloat::Identifier()));
  pattrSy = dynamic_cast<AttrFloat*>(AddAttr ("sy", AttrFloat::Identifier()));
  pattrSz = dynamic_cast<AttrFloat*>(AddAttr ("sz", AttrFloat::Identifier()));

  pattrRotateOrder = dynamic_cast<AttrInt*>(AddAttr ("ro", AttrInt::Identifier()));

  pattrTxRegistryKey = dynamic_cast<AttrString *>(AddAttr ("txRegistryKey", AttrString::Identifier()));
  pattrTyRegistryKey = dynamic_cast<AttrString *>(AddAttr ("tyRegistryKey", AttrString::Identifier()));
  pattrTzRegistryKey = dynamic_cast<AttrString *>(AddAttr ("tzRegistryKey", AttrString::Identifier()));

  pattrSx->Set (1.0f);
  pattrSy->Set (1.0f);
  pattrSz->Set (1.0f);

  // NOTE:  I'm concerned if all three transform components get set, then
  //         ApplyTranslation will be called too often and hurt performance.
  //         The other option is to poll for changes and do lazy updating,
  //         but that has its own performance concerns.
  pattrTx->sigOnChanged.Connect (this, &TransformComponent::TranslationChangedListener);
  pattrTy->sigOnChanged.Connect (this, &TransformComponent::TranslationChangedListener);
  pattrTz->sigOnChanged.Connect (this, &TransformComponent::TranslationChangedListener);
  };

//-----------------------------------------------------------------------------
TransformComponent::~TransformComponent ()
  {
  pattrTx->SetRegistryKey ("");
  pattrTy->SetRegistryKey ("");
  pattrTz->SetRegistryKey ("");
  };

//-----------------------------------------------------------------------------
Attr *  TransformComponent::SetAttr (const char *  szNameIn,
                                     const char *  szValueIn)
  {
  Attr *  pattrChanged = Component::SetAttr (szNameIn, szValueIn);

  if (pattrChanged == NULL)
    {
    return NULL;
    }

  // handle any changes that result from setting new values in attrs
  if ((pattrChanged == static_cast<Attr *>(pattrTx)) ||
      (pattrChanged == static_cast<Attr *>(pattrTy)) ||
      (pattrChanged == static_cast<Attr *>(pattrTz)))
    {
    ApplyTranslation ();
    }
  else if ((pattrChanged == static_cast<Attr *>(pattrRx)) ||
           (pattrChanged == static_cast<Attr *>(pattrRy)) ||
           (pattrChanged == static_cast<Attr *>(pattrRz)) ||
           (pattrChanged == static_cast<Attr *>(pattrRotateOrder)))
    {
    ApplyRotation ();
    }
  else if ((pattrChanged == static_cast<Attr *>(pattrSx)) ||
           (pattrChanged == static_cast<Attr *>(pattrSy)) ||
           (pattrChanged == static_cast<Attr *>(pattrSz)))
    {
    ApplyScale ();
    }
  else if (pattrChanged == static_cast<Attr *>(pattrTxRegistryKey))
    {
    pattrTx->SetRegistryKey (pattrTxRegistryKey->GetString ());
    }
  else if (pattrChanged == static_cast<Attr *>(pattrTyRegistryKey))
    {
    pattrTy->SetRegistryKey (pattrTyRegistryKey->GetString ());
    }
  else if (pattrChanged == static_cast<Attr *>(pattrTzRegistryKey))
    {
    pattrTz->SetRegistryKey (pattrTzRegistryKey->GetString ());
    }
  return pattrChanged;
  }

//-----------------------------------------------------------------------------
VOID  TransformComponent::SetParent  (TransformComponent *  pcmpParentIn)
  {
  if (pcmpParentIn == NULL) return;

  transform.SetParent (&pcmpParentIn->transform);
  transform.pszID = pParentID;
  };

//-----------------------------------------------------------------------------
VOID TransformComponent::CacheTransform (Component *             pcmpSiblingIn,
                                         TransformComponent * *  ppCachedTransformOut)
  {
  ASSERT (ppCachedTransformOut != NULL);
  ASSERT (pcmpSiblingIn != NULL);
  if ((ppCachedTransformOut == NULL) || (*ppCachedTransformOut != NULL)) {return;};

  Component *  pSearch = pcmpSiblingIn->GetFirstSibling ();

  while (pSearch->IsValid ())
    {
    Component *  pcmpInterface = pSearch->GetInterface (TransformComponent::Identifier ());
    if (pcmpInterface != NULL)
      {
      *ppCachedTransformOut = dynamic_cast<TransformComponent *>(pcmpInterface);
      return;
      };
    pSearch = pSearch->Next();
    };
  };

//-----------------------------------------------------------------------------
VOID  TransformComponent::TranslationChangedListener (Attr *  pattrIn)
  {
  ApplyTranslation ();
  };

//-----------------------------------------------------------------------------
VOID TransformComponent::ApplyTranslation (VOID)
  {
  INT  iCurrVersion = pattrTx->Version () + pattrTy->Version () + pattrTz->Version ();
  if (iCurrVersion > iTranslationVersion)
    {
    //DBG_INFO ("TransformComponent::ApplyTranslation");
    iTranslationVersion = iCurrVersion;
    MarkAsDirty ();
    transform.SetPosition (pattrTx->Value (), pattrTy->Value (), pattrTz->Value ());
    };
  };

//-----------------------------------------------------------------------------
VOID TransformComponent::ApplyRotation    (VOID)
  {
  INT  iCurrVersion = pattrRx->Version () + pattrRy->Version () + pattrRz->Version ();
  if (iCurrVersion > iRotationVersion)
    {
    iRotationVersion = iCurrVersion;
    MarkAsDirty ();
    transform.SetEuler (pattrRx->Value (), pattrRy->Value (), pattrRz->Value (), (Euler::EOrder) pattrRotateOrder->Value ());
    };
  };

//-----------------------------------------------------------------------------
VOID TransformComponent::ApplyScale       (VOID)
  {
  INT  iCurrVersion = pattrSx->Version () + pattrSy->Version () + pattrSz->Version ();
  if (iCurrVersion > iScaleVersion)
    {
    iScaleVersion = iCurrVersion;
    MarkAsDirty ();
    transform.SetScale (pattrSx->Value (), pattrSy->Value (), pattrSz->Value ());
    };
  };

