/* -----------------------------------------------------------------
                           Trigger Tracker

     This module implements a delegate object for tracking entering
   and leaving a "trigger" object.  This class should be instantiated
   within a component that needs trigger behavior, updated when
   onCollide is called, and then subscribed to for OnEnterTrigger
   and OnExitTrigger events.

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
#include "Composite/TriggerTracker.hpp"
#include "Util/CalcHash.hpp"


//-----------------------------------------------------------------------------
TriggerTracker::TriggerTracker ()
  {
  };

//-----------------------------------------------------------------------------
TriggerTracker::~TriggerTracker ()
  {
  arrayNodes.Clear ();
  };

//-----------------------------------------------------------------------------
VOID  TriggerTracker::CallOnTriggerCollide  (VOID *  pNodeIn)
  {
  if (pNodeIn == NULL) return;

  Node *  pNode = static_cast<Node *>(pNodeIn);

  INT  iNumCurrCollisions = arrayNodes.Length ();
  for (INT  iIndex = 0; iIndex < iNumCurrCollisions; ++iIndex)
    {
    if (pNode->Equals (static_cast<Node *>(arrayNodes [iIndex])))
      {
      aiCollideThisUpdate[iIndex] = 1;
      return;
      };
    };
  // did not find node in list
  arrayNodes.Append (pNodeIn);
  aiCollideThisUpdate.Append (1);
  pNode->sigOnDelete.Connect (this, &TriggerTracker::OnNodeDeleteListener);
  sigOnEnterTrigger (pNode);
  };

//-----------------------------------------------------------------------------
VOID TriggerTracker::OnNodeDeleteListener (Node *  pNode)
  {
  // node is being deleted.  Remove from internal list.
  INT  iNumCurrCollisions = arrayNodes.Length ();
  for (INT  iIndex = 0; iIndex < iNumCurrCollisions; ++iIndex)
    {
    if (pNode->Equals (static_cast<Node *>(arrayNodes [iIndex])))
      {
      arrayNodes.Remove (iIndex);
      aiCollideThisUpdate.Remove (iIndex);
      pNode->sigOnDelete.Disconnect (this, &TriggerTracker::OnNodeDeleteListener);
      break;;
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  TriggerTracker::CallOnPostUpdate (VOID)
  {
  INT  iIndex;

  // OnPostUpdate, check for changes and then clear tracking.  Set tracking during update loop via the OnCollide messages.

  INT  iNumCollisions = aiCollideThisUpdate.Length ();
  for (iIndex = 0; iIndex < iNumCollisions; ++iIndex)
    {
    if (aiCollideThisUpdate [iIndex] == 0)
      {
      sigOnExitTrigger (static_cast<Node *>(arrayNodes [iIndex]));
      arrayNodes.Remove (iIndex);
      aiCollideThisUpdate.Remove (iIndex);
      --iIndex;
      };
    };

  // clear for the next run
  iNumCollisions = aiCollideThisUpdate.Length ();
  for (iIndex = 0; iIndex < iNumCollisions; ++iIndex)
    {
    aiCollideThisUpdate [iIndex] = 0;
    };
  };

