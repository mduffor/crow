/* -----------------------------------------------------------------
                             World

     This module implements the top level storage for the scene graph.
     It is designed to be a singleton.

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
#include "Composite/World.hpp"
#include "Util/CalcHash.hpp"




//-----------------------------------------------------------------------------
//  World
//-----------------------------------------------------------------------------

World *  World::pInstance = NULL;


//-----------------------------------------------------------------------------
World::World  ()
  {
  nodeRoot.SetActive (TRUE);
  };

//-----------------------------------------------------------------------------
World::~World  ()
  {
  ClearScene ();
  };

//-----------------------------------------------------------------------------
VOID  World::ClearScene  (VOID)
  {

  for (TListItr<Node*>  itrNode = nodeRoot.FirstChild ();
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
Node *  World::CreateNode  (const char *  szFullPathIn)
  {
  //DBG_INFO ("World::CreateNode %s", szFullPathIn);
  // separate out the name from the path
  RStr  strNodeName ("NewNode");
  RStr  strPath (szFullPathIn);

  INT  iFinalSeparator = strPath.ReverseFindChar  ('|');
  if (iFinalSeparator != -1)
    {
    strNodeName = strPath.SubString (iFinalSeparator + 1, strPath.Length () - iFinalSeparator - 1);
    strPath.TruncateRight (iFinalSeparator - 1);
    };

  Node *  pnodeNew = new Node;
  pnodeNew->SetName (strNodeName.AsChar ());

  // parent, if needed
  Node *  pnodeParent = &nodeRoot;
  if (!strPath.IsEmpty ())
    {
    pnodeParent = FindNodeByPath (strPath.AsChar ());
    if (pnodeParent == NULL)
      {
      DBG_ERROR ("World::CreateNode () - Unable to find parent path \"%s\" for new node : %s", strPath.AsChar (), szFullPathIn);
      pnodeParent = &nodeRoot;
      };
    };
  pnodeNew->ParentTo (pnodeParent);

  return (pnodeNew);
  };

//-----------------------------------------------------------------------------
VOID World::CreateNodeFinish  (Node *  pnodeIn,
                               BOOL    bIsActive)
  {
  // NOTE:  Call after all the components have been attached to the node

  if (pnodeIn == NULL) return;

  RStr  strFullPath;
  pnodeIn->CalcFullPath (strFullPath);
  strFullPath.CalcHash ();

  for (TListItr<NodeFinder*>  itrSearch = listCreateWatchers.First ();
       itrSearch.IsValid ();
       ++itrSearch)
    {
    if (((*itrSearch)->uNameHash == pnodeIn->NameHash()) ||
        ((*itrSearch)->uFullNameHash == strFullPath.GetHash()))
      {
      if ((*itrSearch)->OnFound (pnodeIn))
        {
        delete (*itrSearch);
        listCreateWatchers.Delete (itrSearch);
        break;
        };
      };
    };

  pnodeIn->SetActive (bIsActive);
  };


//-----------------------------------------------------------------------------
VOID  World::DeleteNode  (const char *  szFullPathIn)
  {
  Node *  pnodeToDelete = FindNodeByPath (szFullPathIn);
  if (pnodeToDelete != NULL)
    {
    delete (pnodeToDelete);
    };
  };

//-----------------------------------------------------------------------------
VOID  World::DeleteNode  (Node *  pnodeIn)
  {
  delete (pnodeIn);
  };

//-----------------------------------------------------------------------------
Node *  World::FindNode  (const char *  szNameOrPathIn)
  {
  if (szNameOrPathIn[0] == '|')
    {
    return (nodeRoot.FindByPath (&szNameOrPathIn[1]));
    }
  else
    {
    return (nodeRoot.FindByName (&szNameOrPathIn[0]));
    };
  };

//-----------------------------------------------------------------------------
Node *  World::FindNodeByPath  (const char *  szFullPathIn)
  {
  ASSERT (szFullPathIn[0] == '|');

  return (nodeRoot.FindByPath (&szFullPathIn[1]));
  };

//-----------------------------------------------------------------------------
Node *  World::FindNode  (UINT  uNameHashIn)
  {
  return (nodeRoot.FindByName (uNameHashIn));
  };

//-----------------------------------------------------------------------------
Node *  World::FindNodeByPath  (UINT  uFullPathHashIn)
  {
  // NOTE: World path should start with a pipe separator '|'
  return (nodeRoot.FindByPath (uFullPathHashIn, CalcHashValue ("|", 1)));
  };

//-----------------------------------------------------------------------------
VOID World::FindNode (NodeFinder *  pSearch)
  {
  // NOTE:  Find node if it exists, otherwise wait for its creation.
  if (pSearch == NULL) return;

  if ((pSearch->uNameHash == 0) && (pSearch->uFullNameHash == 0))
    {
    // no hashes to search with
    return;
    };


  Node *  pFound = NULL;
  if (pSearch->uNameHash != 0)
    {
    pFound = FindNode (pSearch->uNameHash);
    }
  if ((pFound == NULL) && (pSearch->uFullNameHash != 0))
    {
    pFound = FindNodeByPath (pSearch->uFullNameHash);
    }

  if (pFound != NULL)
    {
    // node already exists
    pSearch->OnFound (pFound);
    delete (pSearch);
    return;
    };

  // Node doesn't exist yet.  Watch for it later.

  listCreateWatchers.PushFront (pSearch);
  };

//-----------------------------------------------------------------------------
EStatus  World::Load  (RStrParser &  parserIn)
  {
  // TODO
  return (EStatus::kFailure);
  };

//-----------------------------------------------------------------------------
EStatus  World::Save  (RStrParser &  parserIn,
                       const RStr &  strLineIndentIn) const
  {
  // TODO
  return (EStatus::kFailure);
  };

//-----------------------------------------------------------------------------
VOID  World::DebugPrint  (const char *  szIndentIn,
                          RStr &        strOut) const
  {
  nodeRoot.DebugPrintHeirarchy (szIndentIn,
                                strOut);
  };






