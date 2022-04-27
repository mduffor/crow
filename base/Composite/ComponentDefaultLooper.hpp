/* -----------------------------------------------------------------
                         ComponentDefaultLooper

     This module implements a class to loop through components
   and call the default callbacks.

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

#ifndef COMPONENTDEFAULTLOOPER_HPP
#define COMPONENTDEFAULTLOOPER_HPP

#include "Sys/Types.hpp"
#include "Composite/Node.hpp"
#include "Composite/Component.hpp"
#include "Composite/NodeDelegate.hpp"
#include "Util/CalcHash.hpp"

//-----------------------------------------------------------------------------
class ComponentDefaultLooper : public NodeDelegate
  {
  public:
    enum Callback {kPreUpdate,
                   kUpdate,
                   kPostUpdate,
                   kClick,
                   //kEnable,
                   //kDisable,
                   kRenderStart,     // before rendering the frame (to either eye) starts
                   kDisplay,         // drawing to the screen / eye
                   kTriggerCollide,
                   kRecache,
                   kEvent};

  private:
    Callback  callbackMode;
    VOID *    pVoidParameter;
    HASH_T    uEventHash;
    BOOL      bVisitInactiveBase;

  public:
                  ComponentDefaultLooper          () {
                                                      callbackMode       = kPreUpdate;
                                                      pVoidParameter     = NULL;
                                                      uEventHash         = 0;
                                                      bVisitInactiveBase = FALSE;
                                                      };
    virtual       ~ComponentDefaultLooper         () {};

    virtual BOOL  VisitNode             (Node *  pnodeIn)  override
                                           {
                                           pnodeIn->Awake ();

                                           if (pnodeIn->IsActive() && (callbackMode == kUpdate))
                                             {
                                             pnodeIn->Update ();
                                             }
                                           return (pnodeIn->IsActive() || bVisitInactiveBase);
                                           };

    virtual BOOL  VisitChildren         (Node *  pnodeIn)  override
                                           {
                                           return (pnodeIn->IsActive());
                                           };

    virtual BOOL  VisitComponent        (Node *       pnodeIn,
                                         Component *  pcomponentIn)  override
                                           {
                                           switch (callbackMode)
                                             {
                                             case kPreUpdate:       pcomponentIn->OnPreUpdate (); break;
                                             case kPostUpdate:      pcomponentIn->OnPostUpdate (); break;
                                             case kClick:           pcomponentIn->OnClick (); break;
                                             //case kEnable:          pcomponentIn->OnEnable (); break;
                                             //case kDisable:         pcomponentIn->OnDisable (); break;
                                             case kRenderStart:     pcomponentIn->OnRenderStart (); break;
                                             case kDisplay:         pcomponentIn->OnDisplay (); break;
                                             case kTriggerCollide:  pcomponentIn->OnTriggerCollide (pVoidParameter); break;
                                             case kRecache:         pcomponentIn->OnRecache (); break;
                                             case kEvent:           pcomponentIn->OnEvent (uEventHash); break;
                                             default: break;
                                             }
                                           return (TRUE);
                                           };

    VOID          IterateNodes          (Callback  modeIn,
                                         Node *    pnodeRootIn,
                                         VOID *    pParamIn     = NULL,
                                         HASH_T    uEventHashIn = 0,
                                         INT       iMaxDepthIn  = INT32_MAX,
                                         BOOL      bVisitInactiveBaseIn = FALSE)
                                           {
                                           pVoidParameter = pParamIn;
                                           uEventHash     = uEventHashIn;
                                           bVisitInactiveBase = bVisitInactiveBaseIn;
                                           SetCallbackMode (modeIn);
                                           RecurseNodeTree (pnodeRootIn, TRUE, iMaxDepthIn);
                                           };
  private:

    VOID          SetCallbackMode       (Callback  modeIn)  {callbackMode = modeIn;};



  };


#endif // COMPONENTDEFAULTLOOPER_HPP
