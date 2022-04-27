/* -----------------------------------------------------------------
                         IntroOutroComponent

     This component serves as the base for components that display
   intro and outro animations.

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

#ifndef INTROOUTROCOMPONENT_HPP
#define INTROOUTROCOMPONENT_HPP

#include "Sys/Types.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/Component.hpp"
#include "Component/MeshComponent.hpp"
#include "Gfx/TransformComponent.hpp"
#include "Render/Atlas.hpp"
#include "ValueRegistry/ValueRegistry.hpp"

/**
  @addtogroup gametech
  @{
*/

const char szIntroOutroComponentID[] = "IntroOutro";

//-----------------------------------------------------------------------------
class IntroOutroComponent : public Component
  {
  protected:

    AttrString *              pattrIntroOutroKey;

    // TODO
    //AttrString *              pattrExprOnIntroComplete;
    //AttrString *              pattrExprOnIdleComplete;
    //AttrString *              pattrExprOnOutroComplete;


    ValueElem *               pelemIntroOutro;

    ValueRegistry *           pCachedRegistry;

  public:
                          IntroOutroComponent  ();

    virtual               ~IntroOutroComponent ();

    static  const char *  Identifier             (VOID)                         {return szIntroOutroComponentID;};

    virtual Component *   GetInterface           (const char *  szTypeIn)       {if streq(szTypeIn, szIntroOutroComponentID) return ((Component *) this); return Component::GetInterface (szTypeIn);};

    virtual Component *   Instantiate            (VOID) const                   {return new IntroOutroComponent;};

    virtual Attr *        SetAttr                (const char *  szNameIn,
                                                  const char *  szValueIn);

    virtual VOID          OnAwake                (VOID);

    virtual VOID          OnRecache              (VOID);

    HASH_T                GetIntroOutroMode      (VOID);

    VOID                  SetIntroOutroMode      (HASH_T  uModeIn);

    inline BOOL           CacheRegistry          (VOID);

    inline BOOL           CacheStateElements     (VOID);

    virtual VOID          DisconnectFromRegistry (VOID);

    static const char *   ModeHashToString       (HASH_T  hEventIn);

    // NOTE:  These are the two functions that overridden classes must primarily deal with.
    virtual VOID          OnRenderStart          (VOID);

    virtual VOID          OnEvent                (HASH_T  hEventIn);



  };
/** @} */ // end of gametech group

#endif // INTROOUTROCOMPONENT_HPP
