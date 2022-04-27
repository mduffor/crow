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

#ifndef WORLD_HPP
#define WORLD_HPP

#include "Sys/Types.hpp"
#include "Composite/Node.hpp"
#include "Composite/Resource.hpp"
#include "Containers/TList.hpp"

#include "Util/Signal.h"

using namespace Gallant;

/**
  // TODO
  List of ComponentTemplates
    CreateNode (add to default resource)
    CreateComponent
    CreateTemplate (given component or list of nodes)
    LoadTemplates
    LoadTemplatesAtPath


  Load/Save Resources in world

  Load/Save Globals
  Enumerate resources for debugging


  Recursive traversal done on Node object with class passed that is derived from a node delegate object.  Delegate object called once for node, then once for each component thereunder.
  This is part of Node, but have a fascade on World to call on the root node.

  Node Functions
    Search for node
    Traverse nodes
    Clear
    CreateNode
    DeleteNode
    FindNodeByName
    ?FindAttrByName (finds attr with a "|Full|Path|Node|Component.Attr" path def)
    GetRootNode
    GetNumNodes
  */


// NOTE: The purpose of NodeFinder and listCreateWatchers is to assist in hooking up
//   one object to another.  A Component can be initialized with the name of another
//   node.  This node may or may not have been loaded at the time of the component's
//   creation.  This way on creation, the component can find the node if it exists,
//   or register a listener that will be checked as each node is created.  This should
//   make finding nodes easier than putting polling routines in a lot of Update calls.

//-----------------------------------------------------------------------------
class NodeFinder
  {
  public:

    UINT32    uNameHash;
    UINT32    uFullNameHash;

    NodeFinder (UINT32   uNameHashIn,
                UINT32   uFullNameHashIn)
      {
      uNameHash     = uNameHashIn;
      uFullNameHash = uFullNameHashIn;
      };

    virtual ~NodeFinder () {}

    // NOTE: Return True if this is the node that is being searched for, so the
    //  NodeFinder can be deleted.
    virtual BOOL  OnFound (Node *  pNode) = 0;
  };


//-----------------------------------------------------------------------------
class World
  {
  private:
    Node   nodeRoot; ///< Root of all nodes in the scene graph.

    TList<NodeFinder*>  listCreateWatchers;

    static World *  pInstance;

  public:
             World          ();

             ~World         ();

    static World *  Instance         (VOID)   {if (pInstance == NULL) {pInstance = new World();}; return pInstance;};

    static VOID     DestroyInstance  (VOID)   {if (pInstance != NULL) {delete pInstance;}; pInstance = NULL;};

    VOID     ClearScene     (VOID); ///< deletes all children of the root node.

    Node *   CreateNode     (const char *  szFullPathIn = "|NewNode"); ///< create a node somewhere in the world.  Parent to root if no parent given.

    VOID     CreateNodeFinish  (Node *  pnodeIn,
                                BOOL    bIsActive);

    VOID     DeleteNode     (const char *  szFullPathIn); ///< Delete the node at the given path, and all of its children.

    VOID     DeleteNode     (Node *  pnodeIn);

    Node *   FindNode       (const char *  szNameOrPathIn); ///< Return the node whose name is given.  If name starts with a pipe character, it is considered to be a path.

    Node *   FindNodeByPath (const char *  szFullPathIn); ///< Return the node pointed to by the path, if the path is valid.

    Node *   FindNode       (UINT  uNameHashIn);

    Node *   FindNodeByPath (UINT  uFullPathHashIn);

    VOID     FindNode       (NodeFinder *  pSearch);

    Node *   RootNode       (VOID)  {return &nodeRoot;};

    EStatus  Load           (RStrParser &  parserIn);

    EStatus  Save           (RStrParser &  parserIn,
                             const RStr &  strLineIndentIn) const;

    VOID     DebugPrint     (const char *  szIndentIn,
                             RStr &        strOut) const;



  };

#endif // WORLD_HPP




