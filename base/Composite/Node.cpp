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

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Composite/Node.hpp"
#include "Composite/Component.hpp"
#include "Composite/NodeDelegate.hpp"

static BOOL                bComponentTemplatesInitialized = FALSE;
static TList<Component *>  listComponentTemplates;

//-----------------------------------------------------------------------------
class MarkTransformComponentsDirty : public NodeDelegate
  {
  public:
                  MarkTransformComponentsDirty          () {};
    virtual       ~MarkTransformComponentsDirty         () {};

    BOOL  VisitNode             (Node *  pnodeIn) override
                                          {
                                          TransformComponent *  pcmpTrans = pnodeIn->Transform ();
                                          if (pcmpTrans != NULL)
                                            {
                                            pcmpTrans->MarkAsDirty ();
                                            }
                                          return TRUE;
                                          };

    BOOL  VisitChildren         (Node *       pnodeIn)      override {return TRUE;};

    BOOL  VisitComponent        (Node *       pnodeIn,
                                 Component *  pcomponentIn) override {return (TRUE);};

  };

//-----------------------------------------------------------------------------
class MarkAllComponentsDirty : public NodeDelegate
  {
  public:
                  MarkAllComponentsDirty          () {};
    virtual       ~MarkAllComponentsDirty         () {};

    BOOL  VisitNode             (Node *  pnodeIn) override {return TRUE;};

    BOOL  VisitChildren         (Node *  pnodeIn) override {return TRUE;};

    BOOL  VisitComponent        (Node *       pnodeIn,
                                 Component *  pcomponentIn) override
                                          {
                                          pcomponentIn->MarkAsDirty ();
                                          return (TRUE);
                                          };

  };


//-----------------------------------------------------------------------------
//  Node
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
VOID InitializeComponentTemplates (VOID)
  {
  //listComponentTemplates.PushBack (static_cast<Component*>(new AttrFloat ("float")));
  bComponentTemplatesInitialized = TRUE;
  };

//-----------------------------------------------------------------------------
VOID  Node::AddComponentTemplate (Component *  componentIn)
  {
  ASSERT (componentIn != NULL);
  listComponentTemplates.PushBack (componentIn);
  };

//-----------------------------------------------------------------------------
VOID  Node::DeleteAllComponentTemplates (VOID)
  {
  for (TListItr<Component*> itrCurr = listComponentTemplates.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listComponentTemplates.Empty ();
  };

//-----------------------------------------------------------------------------
const char *  Node::GetParentName  (Component *  componentIn)
  {
  // This is a helper function for finding the node name for identification
  //  purposes, given any given component.  Since Components don't know about
  //  Nodes, we have to work with an abstract handle for the node (void *)

  if (componentIn == NULL) return "";

  return (static_cast<Node*>(componentIn->ParentNode())->Name());
  };

//-----------------------------------------------------------------------------
const char *  Node::GetParentFullName  (Component *  componentIn,
                                        UINT         uSeparatorIn)
  {
  // This is a helper function for finding the node name for identification
  //  purposes, given any given component.  Since Components don't know about
  //  Nodes, we have to work with an abstract handle for the node (void *)

  static RStr  strOut;

  if (componentIn == NULL) return "";

  return (static_cast<Node*>(componentIn->ParentNode())->CalcFullPath(strOut, uSeparatorIn));
  };


//-----------------------------------------------------------------------------
Node::Node ()
  {
  if (!bComponentTemplatesInitialized)
    {
    InitializeComponentTemplates ();
    };

  strName = "";
  bActive  = FALSE;
  bVisible = TRUE;
  bAwake   = FALSE;
  bStarted = FALSE;

  iResourceID = -1;
  pnodeParent = NULL;
  pcmpTransform = NULL;
  pCollider     = NULL;
  listComponents.MakeListSentinel ();
  };

//-----------------------------------------------------------------------------
Node::~Node ()
  {
  // we call OnDelete first, so any cleanup can trigger according to the state
  //  of the Node at the time of deletion.  This will allow unparenting during the
  //  OnDelete callback, or sending messages to its relatives in the heirarchy,
  //  or marking children for deletion as well.
  OnDelete ();

  Component *  pcmpNext;
  for (Component*  pcmpCurr = FirstComponent ();
       pcmpCurr->IsValid ();
       )
    {
    pcmpNext = pcmpCurr->Next ();
    delete pcmpCurr;
    pcmpCurr = pcmpNext;
    };
  listComponents.MakeListSentinel ();

  DeleteChildren ();
  ParentTo (NULL);
  };

//-----------------------------------------------------------------------------
VOID  Node::DeleteChildren (VOID)
  {
  // The following loop is a little obtuse.  We increment the iterator
  //  just ahead of the child node deletion, and we allow the
  //  ParentTo(NULL) call in the child's destructor to actually
  //  remove the entries from the listChildren container behind
  //  our iterator's position.
  for (TListItr<Node*>  itrNode = listChildren.First ();
       itrNode.IsValid ();
       ++itrNode)
    {
    Node *  pnodeToDelete = (*itrNode);
    ++itrNode;
    pnodeToDelete->ParentTo (NULL);
    delete pnodeToDelete;
    }
  };

//-----------------------------------------------------------------------------
Node *  Node::Clone (VOID) const
  {
  Node *  pnodeNew = new Node();

  pnodeNew->SetName (strName.AsChar());
  pnodeNew->SetActive (bActive);
  pnodeNew->bVisible = bVisible;

  // clone components
  for (Component *  pSearch = FirstComponent(); pSearch->IsValid (); pSearch = pSearch->Next ())
    {
    Component *  pcmpNew = pnodeNew->AddComponent  (pSearch->Type());
    pcmpNew->CloneAttrs (pSearch);
    }

  // clone children
  for (TListItr<Node*>  itrChild = FirstChild (); itrChild.IsValid(); ++itrChild)
    {
    Node *  pnodeNewChild = (*itrChild)->Clone();
    pnodeNewChild->ParentTo (pnodeNew);
    };
  return (pnodeNew);
  };

//-----------------------------------------------------------------------------
BOOL  Node::Equals  (Node *  pNodeIn)
  {
  // This function is for determining "are these two nodes the same?"  There
  //  are several ways to do this, including just comparing memory pointers
  //  or comparing names, assuming names are unique across the heirarchy (esp.
  //  since they include the heirarchy in their names.)

  // For now, unless it proves unreliable, I'm going to use mem pointers as
  //  the comparison metric
  return (pNodeIn == this);
  };


//-----------------------------------------------------------------------------
Node *  Node::FindChild  (const char *  szNameIn)
  {
  for (TListItr<Node *> itrCurr = listChildren.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if (streq ((*itrCurr)->Name (), szNameIn))
      {
      return (*itrCurr);
      };
    };
  // unable to find named child.
  return (NULL);
  };

//-----------------------------------------------------------------------------
Node *  Node::RootParent  (VOID)
  {

  Node *  pnodeCurr = this;
  while (pnodeCurr->pnodeParent != NULL)
    {
    pnodeCurr = pnodeCurr->pnodeParent;
    };
  return (pnodeCurr);
  };

//-----------------------------------------------------------------------------
const char *  Node::CalcFullPath  (RStr &   strPathOut,
                                   UINT     uSeparatorIn) const
  {
  INT  iPathLength = 0;

  this->CalcFullPathInternal (strPathOut, iPathLength, FALSE, uSeparatorIn);
  return (strPathOut.AsChar());
  };

//-----------------------------------------------------------------------------
VOID  Node::CalcResourcePath  (RStr &   strPathOut) const
  {
  INT  iPathLength = 0;

  this->CalcFullPathInternal (strPathOut, iPathLength, TRUE, '|');
  };

//-----------------------------------------------------------------------------
VOID  Node::CalcFullPathInternal (RStr &   strPathOut,
                                  INT      iLength,
                                  BOOL     bMatchResourceIDs,
                                  UINT     uSeparatorIn) const
  {

  iLength += strName.Length () + 1;

  if ((pnodeParent == NULL) ||
      (bMatchResourceIDs && (iResourceID != pnodeParent->iResourceID)))
    {
    // we are the root node
    strPathOut.Grow (iLength);
    strPathOut.Empty ();
    }
  else
    {
    pnodeParent->CalcFullPathInternal (strPathOut, iLength, bMatchResourceIDs);
    }
  if (!strName.IsEmpty ())
    {
    strPathOut.AppendChar (uSeparatorIn);
    strPathOut.AppendString (strName);
    };
  };

//-----------------------------------------------------------------------------
Node *  Node::FindByName  (const char *  pszNameIn)
  {
  // Perform a depth-first recursive search of all nodes for the one matching the given name.
  // REFACTOR:  This needs to be sped up with the hash lookup now built into RStr

  ASSERT (pszNameIn[0] != '|');
  TListItr<Node *> itrCurr;

  for (itrCurr = this->listChildren.First ();
      itrCurr.IsValid ();
      ++itrCurr)
    {
    if (streq (pszNameIn, (*itrCurr)->Name ()))
      {
      return (*itrCurr);
      }
    Node *  pFound = (*itrCurr)->FindByName (pszNameIn);
    if (pFound != NULL)
      {
      return (pFound);
      };
    };

  return (NULL);
  };

//-----------------------------------------------------------------------------
Node *  Node::FindByPath  (const char *  pszPathIn)
  {
  INT  iPathIndex = 0;

  // This routine assumes that the path starts with a child's name
  ASSERT (pszPathIn[iPathIndex] != '|');
  Node *  pnodeCurr = this;
  TListItr<Node *> itrCurr;

  while (pszPathIn[iPathIndex] != '\0')
    {
    INT  iNameLength = 0;
    INT  iNameStart = iPathIndex;
    while ((pszPathIn[iPathIndex] != '\0') &&
          (pszPathIn[iPathIndex] != '|'))
      {
      ++iPathIndex;
      ++iNameLength;
      }
    // we have the name from the path.  Now find the child that matches that name.
    for (itrCurr = pnodeCurr->listChildren.First ();
        itrCurr.IsValid ();
        ++itrCurr)
      {
      //DBG_INFO ("Comparing node \"%s\" against \"%s\"", (*itrCurr)->Name (), &pszPathIn[iNameStart]);
      if (((INT)(*itrCurr)->strName.Length () == iNameLength) &&
          (strncmp ((*itrCurr)->Name (), &pszPathIn[iNameStart], iNameLength) == 0))
        {
        // match at this level
        pnodeCurr = (*itrCurr);
        if (pszPathIn [iPathIndex] == '\0')
          {
          // last entry in the path
          return (pnodeCurr);
          }
        // keep searching
        ++iPathIndex;
        break;
        };
      };
    if (!itrCurr.IsValid ())
      {
      // reached the end of the loop without finding the named child.  Path is invalid.
      break;
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
Node *  Node::FindByName  (UINT  uNameHashIn)
  {
  // Perform a depth-first recursive search of all nodes for the one matching the given name.

  TListItr<Node *> itrCurr;

  for (itrCurr = this->listChildren.First ();
      itrCurr.IsValid ();
      ++itrCurr)
    {
    if ((*itrCurr)->strName.EqualsHash (uNameHashIn))
      {
      return (*itrCurr);
      }
    Node *  pFound = (*itrCurr)->FindByName (uNameHashIn);
    if (pFound != NULL)
      {
      return (pFound);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
Node *  Node::FindByPath  (UINT  uFullNameHashIn,
                           UINT  uPathHashIn)  // Hash for path thus far, includeing separator
  {


  TListItr<Node *> itrCurr;

  for (itrCurr = this->listChildren.First ();
      itrCurr.IsValid ();
      ++itrCurr)
    {
    UINT  uPathWithChild = CalcHashValue ((*itrCurr)->strName.AsChar (), (*itrCurr)->strName.Length (), uPathHashIn);

    if (uFullNameHashIn == uPathWithChild)
      {
      return (*itrCurr);
      };

    uPathWithChild = CalcHashValue ("|", 1, uPathWithChild);

    Node *  pFound = (*itrCurr)->FindByPath (uFullNameHashIn, uPathWithChild);
    if (pFound != NULL)
      {
      return (pFound);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  Node::ParentTo  (Node *  pnodeNewParentIn)
  {
  // unparent
  if (pnodeParent != NULL)
    {
    pnodeParent->listChildren.Delete (this);

    if (pcmpTransform != NULL)
      {
      pcmpTransform->SetParent (NULL);
      };
    }
  pnodeParent = NULL;

  // reparent
  if (pnodeNewParentIn != NULL)
    {
    pnodeNewParentIn->listChildren.PushBack (this);
    pnodeParent = pnodeNewParentIn;

    // Set up parenting at the transform component level
    if ((pcmpTransform != NULL) && (pnodeNewParentIn->pcmpTransform != NULL))
      {
      pcmpTransform->SetParent (pnodeNewParentIn->pcmpTransform);
      };
    };
  };

//-----------------------------------------------------------------------------
Component *  Node::AddComponent  (const char *  szTypeIn)
  {
  for (TListItr<Component*> itrCurr = listComponentTemplates.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if (streq ((*itrCurr)->Type (), szTypeIn))
      {
      Component *  pcmpNew = (*itrCurr)->Instantiate ();
      listComponents.InsertBefore (pcmpNew);
      pcmpNew->SetParentNode ((PVOID) this, Name());

      Component *  pcmpInterface = pcmpNew->GetInterface (TransformComponent::Identifier ());
      if (pcmpInterface != NULL)
        {
        pcmpTransform = dynamic_cast<TransformComponent *>(pcmpInterface);

        // Set up parenting at the transform component level
        if ((pcmpTransform != NULL) && (pnodeParent != NULL) && (pnodeParent->pcmpTransform != NULL))
          {
          pcmpTransform->SetParent (pnodeParent->pcmpTransform);
          }
        }

      return (pcmpNew);
      }
    }
  // unable to find component type in template list.
  DBG_ERROR ("Unable to find component type \"%s\" in component template list.", szTypeIn);
  return (NULL);
  };

//-----------------------------------------------------------------------------
Component *  Node::FindComponent (const char *  szTypeIn,
                                  Component *   pSearchStartIn)
  {
  Component *  pSearch = pSearchStartIn;

  if (pSearch == NULL)
    {
    pSearch = &listComponents;
    }
  pSearch = pSearch->Next ();
  for (; pSearch->IsValid (); pSearch = pSearch->Next ())
    {
    if (streq (szTypeIn, pSearch->Type ()))
      {
      // found the first component of this type, following pSearchStartIn
      return (pSearch);
      }
    }
  return (NULL);
  };

//-----------------------------------------------------------------------------
Component *  Node::FindComponentInParents  (const char *  szTypeIn,
                                            Component *   pSearchStartIn)
  {
  Node *  pNode = this;
  if (pSearchStartIn != NULL)
    {
    pNode = static_cast<Node *>(pSearchStartIn->ParentNode ());
    if (pNode == NULL)
      {
      pNode = this;
      pSearchStartIn = NULL;
      };
    };
  Component *  pSearch = pNode->FindComponent (szTypeIn, pSearchStartIn);
  if (pSearch != NULL) {return pSearch;};
  if (pNode->pnodeParent == NULL) {return NULL;};
  return (pNode->pnodeParent->FindComponentInParents (szTypeIn));
  };

//-----------------------------------------------------------------------------
EStatus  Node::Load  (RStrParser &  parserIn)
  {
  // TODO
  return (EStatus::kFailure);
  };

//-----------------------------------------------------------------------------
EStatus  Node::Save  (RStrParser &  parserIn,
                      const RStr &  strLineIndentIn) const
  {
  // TODO
  return (EStatus::kFailure);
  };

//-----------------------------------------------------------------------------
VOID  Node::SetActive  (BOOL  bIn)
  {
  // NOTE:  Nodes are now INACTIVE by default, as are their components.  Need to
  //  figure out how and when they are made active.

  // NOTE:  Nodes are active by default, and only set inactive via an explicit
  //  call to SetActive().  Components are meant to be active if their parent
  //  node and its ancestry are all active.  So the active state of nodes
  //  is meant to be persistent and stateful, while the components reflect a
  //  temporary state.

  bActive = bIn;

  BOOL  bParentsActive = AreParentsActive ();

  //DBG_INFO ("Parents of %s are active: %d", Name(), bParentsActive ? 1 : 0);
  //DBG_INFO ("Setting %s %s", Name(), (bActive && bParentsActive) ? "Active" : "Disabled");

  // recursively turn on self and children if we can.
  SetComponentsActive (bActive && bParentsActive);

  // NOTE:  Already happening, I think.
  //if (bActive)
  //  {
  //  MarkTransformsDirty ();
  //  };
  };

//-----------------------------------------------------------------------------
VOID  Node::SetComponentsActive (BOOL  bIn)
  {
  Component *  pcmpCurr;

  for (pcmpCurr = FirstComponent (); pcmpCurr->IsValid (); pcmpCurr = pcmpCurr->Next ())
    {
    //RStr  strFullPath; ((Node*)(pcmpCurr->ParentNode()))->CalcFullPath (strFullPath);
    //DBG_INFO (" --- Setting %s:%s : %s", strFullPath.AsChar(), pcmpCurr->Type(), bIn ? "Active" : "Disabled");
    pcmpCurr->SetActive (bIn);
    // NOTE:  If components need to be marked as dirty when they become active, then
    //  that component's OnActive() & OnInactive() methods should be overridden to call MarkAsDirty()
    };

  //DBG_INFO ("Object %s has %d children", strName.AsChar (), listChildren.Size ());
  for (TListItr<Node*> itrCurr = listChildren.First (); itrCurr.IsValid (); ++itrCurr)
    {
    //RStr  strFullPath; (*itrCurr)->CalcFullPath (strFullPath);
    //DBG_INFO ("SetComponentsActive for child object %s", strFullPath.AsChar ());

    // If a node is inactive, its child components should be inactive
    (*itrCurr)->SetComponentsActive (bIn && (*itrCurr)->IsActive ());
    };
  };


//-----------------------------------------------------------------------------
BOOL  Node::AreParentsActive (VOID)
  {
  // step up the parent chain and find out if all parents are active.
  for (Node *  pNode = pnodeParent; pNode != NULL; pNode = pNode->pnodeParent)
    {
    if (!pNode->IsActive ())
      {
      //RStr  strFullPath;
      //pNode->CalcFullPath (strFullPath);
      //DBG_INFO ("Parent node inactive : %s", strFullPath.AsChar ());

      return FALSE;
      }
    }
  return TRUE;
  };


//-----------------------------------------------------------------------------
VOID  Node::Update (VOID)
  {
  Component *  pcmpCurr;

  if (!IsStarted ())
    {
    SetStarted (TRUE);
    for (pcmpCurr = FirstComponent (); pcmpCurr->IsValid (); pcmpCurr = pcmpCurr->Next ())
      {
      pcmpCurr->OnStart ();
      };
    }
  for (pcmpCurr = FirstComponent (); pcmpCurr->IsValid (); pcmpCurr = pcmpCurr->Next ())
    {
    pcmpCurr->OnUpdate ();
    };
  };

//-----------------------------------------------------------------------------
VOID  Node::Awake (VOID)
  {
  // Wake up this node and its components.
  if (!IsAwake ())
    {
    //RStr  strFullPath; CalcFullPath (strFullPath);
    //DBG_INFO (" --- Awake %s", strFullPath.AsChar());

    SetAwake (TRUE);
    for (Component *  pcmpCurr = FirstComponent (); pcmpCurr->IsValid (); pcmpCurr = pcmpCurr->Next ())
      {
      //DBG_INFO ("       Awake component %s", pcmpCurr->Type ());
      pcmpCurr->OnAwake ();
      };
    }
  };

//-----------------------------------------------------------------------------
VOID  Node::OnDelete (VOID)
  {

  sigOnDelete(this);

  for (Component*  pcmpCurr = FirstComponent ();
       pcmpCurr->IsValid ();
       pcmpCurr = pcmpCurr->Next ())
    {
    pcmpCurr->SetActive (FALSE);
    pcmpCurr->OnDelete();
    };
  listComponents.MakeListSentinel ();
  };


//-----------------------------------------------------------------------------
VOID  Node::DebugPrint  (const char *  szIndentIn,
                         RStr &        strOut) const
  {
  RStr  strComponentIndent;
  strComponentIndent.Format ("%s  ", szIndentIn);
  RStr  strFullPath;
  CalcFullPath (strFullPath);
  strOut.AppendFormat ("%sNode: %s (%x) (Active: %s)\n", szIndentIn, strFullPath.AsChar (), this, bActive ? "True" : "False");

  for (Component * pcmpCurr = FirstComponent (); pcmpCurr->IsValid (); pcmpCurr = pcmpCurr->Next ())
    {
    pcmpCurr->DebugPrint (strComponentIndent.AsChar (), strOut);
    };
  };

//-----------------------------------------------------------------------------
VOID  Node::DebugPrintHeirarchy  (const char *  szIndentIn,
                                  RStr &        strOut) const
  {
  RStr  strChildIndent;
  strChildIndent.Format ("%s    ", szIndentIn);

  this->DebugPrint (szIndentIn, strOut);

  for (TListItr<Node*> itrCurr = listChildren.First (); itrCurr.IsValid (); ++itrCurr)
    {
    (*itrCurr)->DebugPrintHeirarchy (strChildIndent.AsChar (), strOut);
    };
  }

//-----------------------------------------------------------------------------
VOID *  Node::Collider  (BOOL  bRecurseTree)
  {
  if (pCollider != NULL) {return pCollider;}

  // NOTE: Eventually we should sum all of the children into a composite volume.
  //        For the moment, I'm just choosing the first collider we find.
  if (bRecurseTree)
    {
    for (TListItr<Node*> itrCurr = listChildren.First (); itrCurr.IsValid (); ++itrCurr)
      {
      if ((*itrCurr)->IsActive ())
        {
        VOID *  pChildCollider = (*itrCurr)->Collider (TRUE);
        if (pChildCollider != NULL)
          {
          return (pChildCollider);
          };
        };
      };
    };

  return NULL;
  };


//-----------------------------------------------------------------------------
VOID Node::MarkTransformsDirty  (VOID)
  {
  DBG_INFO ("Node::MarkTransformsDirty ()");
  MarkTransformComponentsDirty delegate;

  delegate.RecurseNodeTree (this, FALSE);

  if (pcmpTransform != NULL)
    {
    pcmpTransform->GetTransform().MarkRectHierarchyDirty();
    };
  };


//-----------------------------------------------------------------------------
VOID Node::MarkComponentsDirty  (VOID)
  {
  MarkAllComponentsDirty delegate;

  delegate.RecurseNodeTree (this, FALSE);
  };
