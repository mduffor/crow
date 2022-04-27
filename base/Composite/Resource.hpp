/* -----------------------------------------------------------------
                             Resource

     This module implements trees of component-laden nodes that
     can be saved to and loaded from storage as a single unit.

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

#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "Sys/Types.hpp"
#include "Composite/Attr.hpp"
#include "Composite/Component.hpp"
#include "Composite/Node.hpp"
#include "Containers/TList.hpp"

/**
  Store an internal list of templates
  Handle instancing of a resource (store a templated resource?  Use to calc deltas.)

  Filename (base of filename used as name of resource... must be unique without path)
  Unique ID (list of instance ids)
  Master list of resources
  Hold list of Nodes
    GetResourceRoot
    AddNodeToResource

  Manage Loading/Saving
    Provide valid node IDs (will we need IDs or can we use paths?)
    Calc Node path within resource

  Track active items (animations, etc.) and handle scene events.
    DirtyAnimatedAttrs

  Functions to enumerate scene for debugging.
  */

class Resource;


//-----------------------------------------------------------------------------
class Resource
  {
  private:
    RStr          strFilename;

    IntArray      iaInstanceIDs;
    TList<Node*>  listRootNodes;

    Node *        pnodeTemplateRoot;


  public:
    Resource ();
    ~Resource ();




    //EStatus               Load              (RStrParser &  parserIn);

    //EStatus               Save              (RStrParser &  parserIn,
    //                                         RStr          strLineIndentIn) const;

    //VOID                  DebugPrint        (const char *  szIndentIn) const;
  };

#endif // RESOURCE_HPP