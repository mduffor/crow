/* -----------------------------------------------------------------
                         ScreenComponent

     This module implements a component for activating or
   deactivating nodes based on an entry in a value registry.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2021, Michael T. Duffy II.  All rights reserved.
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

#ifndef SCREENCOMPONENT_HPP
#define SCREENCOMPONENT_HPP

#include "Sys/Types.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/Component.hpp"
#include "Component/MeshComponent.hpp"
#include "Gfx/TransformComponent.hpp"
#include "Render/Atlas.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "GameTech/WindowBaseComponent.hpp"
/**
  @addtogroup gametech
  @{
*/

const char szScreenComponentID[] = "Screen";

//-----------------------------------------------------------------------------
class ScreenComponent : public WindowBaseComponent
  {
  protected:

    AttrString *              pattrUiGroupKey;
    AttrString *              pattrScreenName;

    ValueElem *               pelemUiGroup;

    RStr                      strNameCache;

  public:
                          ScreenComponent  ();

    virtual               ~ScreenComponent ();

    static  const char *  Identifier             (VOID)                         {return szScreenComponentID;};

    virtual Component *   GetInterface           (const char *  szTypeIn)       {if streq(szTypeIn, szScreenComponentID) return ((Component *) this); return WindowBaseComponent::GetInterface (szTypeIn);};

    virtual Component *   Instantiate            (VOID) const                   {return new ScreenComponent;};

    virtual Attr *        SetAttr                (const char *  szNameIn,
                                                  const char *  szValueIn);

    virtual VOID          OnUpdate               (VOID);

    virtual VOID          UpdateFromRegistry     (VOID);

    VOID                  DisconnectFromRegistry (VOID);
  };
/** @} */ // end of gametech group

#endif // SCREENCOMPONENT_HPP
