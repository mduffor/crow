/* -----------------------------------------------------------------
                             Component

     This module implements components that can be attached to nodes
     to form compositions.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2004-2014, Michael T. Duffy II.  All rights reserved.
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
#include "Composite/Component.hpp"
#include "Containers/TList.hpp"
#include "Composite/Attr.hpp"
#include "Composite/AttrFloat.hpp"
#include "Composite/AttrInt.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/AttrStringArray.hpp"
#include "Composite/AttrFloatArray.hpp"
#include "Composite/AttrIntArray.hpp"

static BOOL    bAttrTemplatesInitialized = FALSE;
Signal0<>      Component::sigOnUpdate;
Signal0<>      Component::sigOnPreUpdate;

//-----------------------------------------------------------------------------
//  Component
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
VOID AttrTemplatesInitialize (VOID)
  {
  Attr::AddTemplate (static_cast<Attr*>(new AttrFloat  ("float")));
  Attr::AddTemplate (static_cast<Attr*>(new AttrInt    ("int")));
  Attr::AddTemplate (static_cast<Attr*>(new AttrString ("string")));
  Attr::AddTemplate (static_cast<Attr*>(new AttrStringArray ("stringarray")));
  Attr::AddTemplate (static_cast<Attr*>(new AttrFloatArray ("floatarray")));
  Attr::AddTemplate (static_cast<Attr*>(new AttrIntArray ("intarray")));
  bAttrTemplatesInitialized = TRUE;
  }

//-----------------------------------------------------------------------------
VOID AttrTemplatesUninitialize (VOID)
  {
  Attr::DeleteAllTemplates ();

  bAttrTemplatesInitialized = FALSE;
  };

//-----------------------------------------------------------------------------
Component::Component (VOID)
  {
  if (!bAttrTemplatesInitialized)
    {
    AttrTemplatesInitialize ();
    };

  iVersion = 1;
  bIsActive  = FALSE;  // we want to start inactive to handle templates and lots of hidden objects.
  bIsLoading = FALSE;
  bIsListSentinel = FALSE;
  pPrev = NULL;
  pNext = NULL;
  pParentNode = NULL;
  pParentID   = NULL;
  };

//-----------------------------------------------------------------------------
Component::~Component ()
  {
  for (TListItr<Attr*> itrCurr = listAttr.First (); itrCurr.IsValid (); ++itrCurr)
    {
    delete (*itrCurr);
    };
  listAttr.Empty();
  };

//-----------------------------------------------------------------------------
Component *  Component::Instantiate  (VOID) const
 {
 return new Component;
 };

//-----------------------------------------------------------------------------
VOID  Component::CloneAttrs  (Component *  pcmpSourceIn)
 {

 RStr         strTemp;

 bIsLoading = TRUE;
 for (TListItr<Attr*> itrCurr = pcmpSourceIn->listAttr.First(); itrCurr.IsValid (); ++itrCurr)
   {
   // Although it may be faster to copy direct from attr to attr, we go ahead
   //   and convert to a string to pass through SetAttr(), so that any hooks in
   //   SetAttr will be hit.

   strTemp.Empty ();
   SetAttr ((*itrCurr)->Name(), (*itrCurr)->GetAsString (&strTemp));
   };
 bIsLoading = FALSE;
 };

//-----------------------------------------------------------------------------
VOID  Component::OnEvent (HASH_T  hEventIn)
  {
  // This is a generic event that can be thrown and overridden.
  };

//-----------------------------------------------------------------------------
VOID  Component::OnAwake (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnStart (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnDelete (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnPreUpdate (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnUpdate (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnPostUpdate (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnClick (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnPress (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnRelease (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnUnpress (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnRecache (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnDrag (FLOAT   fNDCPosX,
                         FLOAT   fNDCPosY,
                         FLOAT   fNDCDeltaX,   // given in parametric NDC coordinates
                         FLOAT   fNDCDeltaY,
                         FLOAT   fOrthoWidth,
                         FLOAT   fOrthoHeight)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnDragStart (FLOAT   fNDCPosX,
                              FLOAT   fNDCPosY,
                              FLOAT   fOrthoWidth,
                              FLOAT   fOrthoHeight)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnDragStop (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnDrop (const char *  szDroppedNodePath)
  {

  };

//-----------------------------------------------------------------------------
//VOID  Component::OnEnable (VOID)
//  {
//  // NOTE:  Not sure if this is used.
//  };

//-----------------------------------------------------------------------------
//VOID  Component::OnDisable (VOID)
//  {
//  };

//-----------------------------------------------------------------------------
VOID  Component::OnActive     (VOID)
  {
  //DBG_INFO ("Component::OnActive called for %s", Type());
  };

//-----------------------------------------------------------------------------
VOID  Component::OnInactive   (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnRenderStart (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnDisplay (VOID)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnTriggerCollide (VOID *  pNodeIn)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnLoading (BOOL  bIsLoadingIn)
  {
  };

//-----------------------------------------------------------------------------
VOID  Component::OnAction (const char *  szActionIn)
  {
  // Called whenever an agent wants to activate this object.  The passsed string
  //  can target specific actions, or if it is null then the default
  //  action should trigger.
  };

//-----------------------------------------------------------------------------
VOID  Component::GetActions   (PtrArray &  apActionsOut)
  {
  // This allows each component on a node to add possible actions to the
  //  action list.  Action objects should be added to the apActionsOut
  //  instance.  I will flesh this out later when I implement gaze actions.
  };


//-----------------------------------------------------------------------------
VOID  Component::SetActive (BOOL  bStatusIn)
  {
  bIsActive = bStatusIn;
  if (bIsActive)
    {
    OnActive ();
    }
  else
    {
    OnInactive ();
    };
  sigOnActive (this, bIsActive);
  };

//-----------------------------------------------------------------------------
VOID  Component::SetLoading   (BOOL  bStatusIn)
  {
  // NOTE: Scene loaders set loading to True before reading a component from a
  //   scene file, and then to False after all the component's fields have been
  //   loaded.  This allows a component to act once it has been fully read in.
  bIsLoading = bStatusIn;
  OnLoading (bIsLoading);
  };

//-----------------------------------------------------------------------------
VOID  Component::SetParentNode (PVOID         pParentIn,
                                const char *  pszParentIDIn)
  {
  pParentNode = pParentIn;
  pParentID   = pszParentIDIn;
  };

//-----------------------------------------------------------------------------
Attr *  Component::GetAttr (const char *  szNameIn)
  {
  for (TListItr<Attr*> itrCurr = listAttr.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if (streq ((*itrCurr)->Name(), szNameIn))
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
Attr *  Component::SetAttr (const char *  szNameIn,
                            const char *  szValueIn)
  {
  Attr *  pAttr = GetAttr (szNameIn);
  if (pAttr != NULL)
    {
    //DBG_INFO ("SetAttr calling SetByString %s = \"%s\"", szNameIn, szValueIn == NULL ? "Null" : szValueIn);
    pAttr->Clear ();
    pAttr->SetByString (szValueIn);
    };
  return pAttr;
  };

//-----------------------------------------------------------------------------
Attr *  Component::SetArrayAttr (const char *  szNameIn,
                                 INT           iIndex,
                                 const char *  szValueIn)
  {
  Attr *  pAttr = GetAttr (szNameIn);
  if (pAttr != NULL)
    {
    pAttr->SetArrayByString (iIndex, szValueIn);
    };
  return pAttr;
  };

//-----------------------------------------------------------------------------
Attr *  Component::AddAttr (const char *  szNameIn,
                            const char *  szTypeIn,
                            AttrString *  pattrRegistryKeyIn)
  {
  Attr *  pNew = Attr::New (szNameIn, szTypeIn);
  if (pNew != NULL)
    {
    if (pattrRegistryKeyIn != NULL)
      {
      // We want to utilize a second Attr that contains the registry key,
      //  and update this attr anytime the key attr changes as well.
      pattrRegistryKeyIn->sigOnChangedString.Connect (pNew, &Attr::SetRegistryKey);
      };

    listAttr.PushBack (pNew);
    return (pNew);
    };

  DBG_ERROR ("Unable to create Attr named \"%s\" because cannot find type : %s", szNameIn, szTypeIn);
  return (NULL);
  };

// NOTE: The below AddAttr* () methods are for convenience of use.
//-----------------------------------------------------------------------------
AttrFloat *  Component::AddAttrFloat (const char *  szNameIn,
                                      AttrString *  pattrRegistryKeyIn)
  {
  return (dynamic_cast<AttrFloat*>(AddAttr (szNameIn,
                                            AttrFloat::Identifier(),
                                            pattrRegistryKeyIn)));
  };

//-----------------------------------------------------------------------------
AttrInt *  Component::AddAttrInt (const char *  szNameIn,
                                  AttrString *  pattrRegistryKeyIn)
  {
  return (dynamic_cast<AttrInt*>(AddAttr (szNameIn,
                                          AttrInt::Identifier(),
                                          pattrRegistryKeyIn)));
  };

//-----------------------------------------------------------------------------
AttrString *  Component::AddAttrString (const char *  szNameIn,
                                        AttrString *  pattrRegistryKeyIn)
  {
  return (dynamic_cast<AttrString*>(AddAttr (szNameIn,
                                             AttrString::Identifier(),
                                             pattrRegistryKeyIn)));
  };


//-----------------------------------------------------------------------------
AttrFloatArray *  Component::AddAttrFloatArray (const char *  szNameIn,
                                                AttrString *  pattrRegistryKeyIn)
  {
  return (dynamic_cast<AttrFloatArray*>(AddAttr (szNameIn,
                                                 AttrFloatArray::Identifier(),
                                                 pattrRegistryKeyIn)));
  };

//-----------------------------------------------------------------------------
AttrIntArray *  Component::AddAttrIntArray (const char *  szNameIn,
                                            AttrString *  pattrRegistryKeyIn)
  {
  return (dynamic_cast<AttrIntArray*>(AddAttr (szNameIn,
                                               AttrIntArray::Identifier(),
                                               pattrRegistryKeyIn)));
  };

//-----------------------------------------------------------------------------
AttrStringArray *  Component::AddAttrStringArray (const char *       szNameIn,
                                                  AttrString *  pattrRegistryKeyIn)
  {
  return (dynamic_cast<AttrStringArray*> (AddAttr (szNameIn,
                                                   AttrStringArray::Identifier(),
                                                   pattrRegistryKeyIn)));
  };


//-----------------------------------------------------------------------------
VOID  Component::DeleteAttr (const char *  szNameIn)
  {
  Attr *  pAttr = GetAttr (szNameIn);
  if (pAttr != NULL)
    {
    delete (pAttr);
    listAttr.Delete (pAttr);
    };
  };

//-----------------------------------------------------------------------------
EStatus  Component::Load (RStrParser &  parserIn)
  {
  // TODO
  return (EStatus::kFailure);
  };

//-----------------------------------------------------------------------------
EStatus  Component::Save (RStrParser &  parserIn,
                          RStr          strLineIndentIn) const
  {
  // TODO
  return (EStatus::kFailure);
  };

//-----------------------------------------------------------------------------
VOID  Component::DebugPrint (const char *  szIndentIn,
                             RStr &        strOut) const
  {
  RStr  strAttrIndent;
  strAttrIndent.Format ("%s  ", szIndentIn);
  strOut.AppendFormat ("%sComponent %s (%x)\n", szIndentIn, Type (), this);

  Component *  nonConstThis = const_cast<Component *>(this);

  for (TListItr<Attr*> itrCurr = nonConstThis->listAttr.First (); itrCurr.IsValid (); ++itrCurr)
    {
    (*itrCurr)->DebugPrint (strAttrIndent.AsChar (), strOut);
    };
  };

//-----------------------------------------------------------------------------
EStatus  Component::InsertAfter  (Component *  pcmpIn)
  {
  if (pNext == NULL) return (EStatus::kFailure);

  // check for an invalid entry
  ASSERT (pNext         != NULL);
  ASSERT (pcmpIn        != NULL);
  ASSERT (pcmpIn->pPrev == NULL);
  ASSERT (pcmpIn->pNext == NULL);


  pNext->pPrev  = pcmpIn;
  pcmpIn->pNext = pNext;
  pcmpIn->pPrev = this;
  pNext         = pcmpIn;

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
VOID Component::Remove  (VOID)
  {
  if (pPrev != NULL)   pPrev->pNext = pNext;
  if (pNext != NULL)   pNext->pPrev = pPrev;

  pNext = NULL;
  pPrev = NULL;
  };

//-----------------------------------------------------------------------------
Component *  Component::GetFirstSibling (VOID) const
  {
  const Component *  pSentinel = this;

  while (!pSentinel->bIsListSentinel)
    {
    pSentinel = pSentinel->pPrev;
    if (pSentinel == this) return NULL;
    }
  return pSentinel->pNext;
  };
