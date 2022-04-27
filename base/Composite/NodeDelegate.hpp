/* -----------------------------------------------------------------
                             NodeDelegate

     This module implements a deleagate parent class that is used
     to perform actions on an iterated over list of Nodes.

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

#ifndef NODEDELEGATE_HPP
#define NODEDELEGATE_HPP

#include "Sys/Types.hpp"
#include "Composite/Attr.hpp"
#include "Composite/Component.hpp"
#include "Composite/Node.hpp"
#include "Composite/Resource.hpp"
#include "Containers/TList.hpp"

/**
  // TODO

  Recursive traversal done on Node object with class passed that is derived from a node delegate object.  Delegate object called once for node, then once for each component thereunder.
  This is part of Node, but have a fascade on NodeDelegate to call on the root node.

  */

//-----------------------------------------------------------------------------
class NodeDelegate
  {
  private:
    BOOL          bDebugMode;


  public:
                  NodeDelegate          ();

    virtual       ~NodeDelegate         ();

                                        /** @brief Called for each node as part of the iteration through the tree.
                                            @param pnodeIn  The node being visited
                                            @return True if this node's components and (possibly) children should be iterated, false if not.
                                        */
    virtual BOOL  VisitNode             (Node *  pnodeIn) = 0;

                                        /** @brief Called for each node as part of the iteration through the tree.
                                            @param pnodeIn  The node being visited
                                            @return True if this node's children should be iterated, false if not.
                                        */
    virtual BOOL  VisitChildren        (Node *  pnodeIn) = 0;

                                        /** @brief Called for each component on each node as part of the iteration through the tree.
                                            @param pnodeIn       The node being visited
                                            @param pcomponentIn  The component being visited
                                            @return True if this node's remaining components and children should be iterated, false if not.
                                        */
    virtual BOOL  VisitComponent        (Node *       pnodeIn,
                                         Component *  pcomponentIn) = 0;

            VOID  RecurseNodeTree       (Node *  pnodeRootIn,
                                         BOOL    bVisitComponentsIn = TRUE,
                                         INT     iMaxDepthIn        = INT32_MAX);

            VOID  SetDebugMode          (BOOL   bStatusIn)    {bDebugMode = bStatusIn;};
  };

#endif // NODEDELEGATE_HPP




