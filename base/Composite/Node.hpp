/* -----------------------------------------------------------------
                             Node

     This module implements scene graph nodes for holding components

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

#ifndef NODE_HPP
#define NODE_HPP

#include "Sys/Types.hpp"
#include "Composite/Component.hpp"
#include "Gfx/TransformComponent.hpp"
#include "Containers/TList.hpp"

/**

  Name
  Active Flag
  Visibility Flag

  Link to Resource node is in
  ComponentList

  ?Smart Pointer (update/callback/links)
  ?Class/Category/Flags (for filtering.  Drive Camera Visibility)

  Heirarchy (maintained in parallel with Transform Component)
    GetFirst/LastChild, NumChildren, FindChildByName/ID
    FindRoot
    ?GetParentDepth
    GetFullPath
    ParentTo

  Save/Load
  DebugPrintInfo

  */
//-----------------------------------------------------------------------------
class Node
  {
  private:
    RStr                  strName;        ///< Unique name identifying this object at its sibling level.
    BOOL                  bActive;        ///< whether or not the object is visible to the update loop.
    BOOL                  bVisible;       ///< whether or not the object and its children are seen by the renderer.
    BOOL                  bAwake;
    BOOL                  bStarted;

    UINT                  iResourceID;    ///< Identifies which resource this node is part of.  0 denotes the default resource.

    Component             listComponents; ///< List of Components attached to this node.
    TransformComponent *  pcmpTransform;  ///< specially maintained link to the Transform component on this node, for parenting.

    Node *                pnodeParent;    ///< The parent node in the scene graph heirarchy.
    TList<Node*>          listChildren;   ///< List of children of this node in the scene graph heirarchy.
    VOID *                pCollider;      ///< Pointer to a BVolume collider used for faster lookup.  Not used directly by Node since it is in another module.

  public:

    Signal1<Node*>        sigOnDelete;


  public:
                          Node              ();

                          ~Node             ();

    Node *                Clone             (VOID) const;

    VOID                  DeleteChildren    (VOID);

    VOID                  SetName           (const char *  szNameIn)  {strName.Set (szNameIn, TRUE);};

    const char *          Name              (VOID)                    {return strName.AsChar ();};

    UINT32                NameHash          (VOID)                    {return strName.GetHash ();};

    BOOL                  NameEquals        (UINT32        uHashIn,
                                             const char *  szNameIn)  {return strName.Equals (uHashIn, szNameIn);};

    BOOL                  Equals            (Node *  pNodeIn);        ///< Checks for node equality

    INT                   ResourceID        (VOID)                    {return iResourceID;};

    VOID                  SetResourceID     (INT  iIDIn)              {iResourceID = iIDIn;};

    Component *           FirstComponent    (VOID) const              {return listComponents.Next ();};

    TListItr<Node*>       FirstChild        (VOID) const              {return listChildren.First ();};

    INT                   NumChildren       (VOID)                    {return listChildren.Size ();};

    Node *                FindChild         (const char *  szNameIn);

    Node *                RootParent        (VOID);

    const char *          CalcFullPath      (RStr &   strPathOut,
                                             UINT     uSeparatorIn = '|') const;

    VOID                  CalcResourcePath  (RStr &   strPathOut) const;

    Node *                FindByName        (const char *  pszNameIn);

    Node *                FindByPath        (const char *  pszPathIn);

    Node *                FindByName        (UINT  uNameHashIn);

    Node *                FindByPath        (UINT  uFullNameHashIn,
                                             UINT  uPathHashIn = 0);

    Node *                GetParent         (VOID)  {return (pnodeParent);};

    VOID                  ParentTo          (Node *  pnodeNewParentIn);

    Component *           AddComponent      (const char *  szTypeIn);

    Component *           FindComponent     (const char *  szTypeIn,
                                             Component *   pSearchStartIn = NULL);

    Component *           FindComponentInParents  (const char *  szTypeIn,
                                                   Component *   pSearchStartIn = NULL);

    EStatus               Load              (RStrParser &  parserIn);

    EStatus               Save              (RStrParser &  parserIn,
                                             const RStr &  strLineIndentIn) const;

    VOID                  DebugPrint        (const char *  szIndentIn,
                                             RStr &        strOut) const;

    VOID                  DebugPrintHeirarchy  (const char *  szIndentIn,
                                                RStr &        strOut) const;

    static VOID           AddComponentTemplate (Component *  componentIn);

    static VOID           DeleteAllComponentTemplates (VOID);

    static const char *   GetParentName        (Component *  componentIn);

    static const char *   GetParentFullName    (Component *  componentIn,
                                                UINT         uSeparatorIn = '|');

    BOOL                  IsAwake              (VOID) const   {return bAwake;};

    VOID                  SetAwake             (BOOL  bIn)    {bAwake = bIn;};

    BOOL                  IsActive             (VOID) const   {return bActive;};

    VOID                  SetActive            (BOOL  bIn);

    VOID                  SetComponentsActive  (BOOL  bIn);

    BOOL                  AreParentsActive     (VOID);

    BOOL                  IsStarted            (VOID) const   {return bStarted;};

    VOID                  SetStarted           (BOOL  bIn)    {bStarted = bIn;};

    VOID                  Update               (VOID);

    VOID                  Awake                (VOID);

    VOID                  OnDelete             (VOID);

    VOID                  SetCollider          (VOID *  pColliderIn)   {pCollider = pColliderIn;};

    VOID *                Collider             (BOOL  bRecurseTree = FALSE);

    TransformComponent *  Transform            (VOID)                  {return pcmpTransform;};

    VOID                  MarkTransformsDirty  (VOID);

    VOID                  MarkComponentsDirty  (VOID);


  private:
    VOID                  CalcFullPathInternal (RStr &   strPathOut,
                                                INT      iLength,
                                                BOOL     bMatchResourceIDs,
                                                UINT     uSeparatorIn = '|') const;

  };

#endif // NODE_HPP
