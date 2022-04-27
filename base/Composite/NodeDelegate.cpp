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

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Composite/NodeDelegate.hpp"

//-----------------------------------------------------------------------------
//  NodeDelegate
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
NodeDelegate::NodeDelegate  ()
  {
  bDebugMode = FALSE;
  };

//-----------------------------------------------------------------------------
NodeDelegate::~NodeDelegate  ()
  {
  };

//-----------------------------------------------------------------------------
VOID  NodeDelegate::RecurseNodeTree  (Node *  pnodeRootIn,
                                      BOOL    bVisitComponentsIn,
                                      INT     iMaxDepthIn)
  {

  if (bDebugMode) DBG_INFO ("NodeDelegate::RecurseNodeTree visiting node (%x) : %s", pnodeRootIn, pnodeRootIn->Name());
  if (--iMaxDepthIn < 0) return;

  if (VisitNode (pnodeRootIn))
    {
    if (bVisitComponentsIn)
      {
      for (Component *  pcmpCurr = pnodeRootIn->FirstComponent ();
          pcmpCurr->IsValid ();
          pcmpCurr = pcmpCurr->Next ())
        {
        if (bDebugMode) DBG_INFO ("NodeDelegate::RecurseNodeTree visiting component %s:%s ver %d", pnodeRootIn->Name(), pcmpCurr->Type (), pcmpCurr->Version ());
        if (! VisitComponent (pnodeRootIn, pcmpCurr))
          {
          // component indicated that recursion down this line should stop.
          return;
          };
        };
      };

    if (VisitChildren (pnodeRootIn))
      {
      for (TListItr<Node*>  itrChild = pnodeRootIn->FirstChild ();
          itrChild.IsValid ();
          ++itrChild)
        {
        if (bDebugMode) DBG_INFO ("NodeDelegate::RecurseNodeTree visiting child %s", (*itrChild)->Name());
        RecurseNodeTree (*itrChild, bVisitComponentsIn, iMaxDepthIn);
        };
      };
    };
  };

