/* -----------------------------------------------------------------
                             Transform Component

     This module implements a component for holding Transform data.

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

#ifndef TRANSFORMCOMPONENT_HPP
#define TRANSFORMCOMPONENT_HPP

#include "Sys/Types.hpp"
#include "Composite/Component.hpp"
#include "Gfx/Transform.hpp"
#include "Composite/AttrFloat.hpp"
#include "Composite/AttrInt.hpp"
#include "Composite/AttrString.hpp"
#include "ValueRegistry/ValueRegistry.hpp"

const char szTransformComponentID[] = "Transform";

//-----------------------------------------------------------------------------
class TransformComponent : public Component
  {
  public:
    Transform             transform;

    AttrFloat *           pattrTx;
    AttrFloat *           pattrTy;
    AttrFloat *           pattrTz;
    AttrFloat *           pattrRx;
    AttrFloat *           pattrRy;
    AttrFloat *           pattrRz;
    AttrFloat *           pattrSx;
    AttrFloat *           pattrSy;
    AttrFloat *           pattrSz;
    AttrInt *             pattrRotateOrder;

    AttrString *          pattrTxRegistryKey;
    AttrString *          pattrTyRegistryKey;
    AttrString *          pattrTzRegistryKey;

    INT                   iTranslationVersion;
    INT                   iRotationVersion;
    INT                   iScaleVersion;

  public:
                          TransformComponent    ();
    virtual               ~TransformComponent   ();

    static  const char *  Identifier       (VOID)                             {return szTransformComponentID;};

    virtual Component *   GetInterface     (const char *  szTypeIn)  override {if streq(szTypeIn, szTransformComponentID) return (dynamic_cast<Component *>(this)); return (NULL);};

    virtual Component *   Instantiate      (VOID) const              override {return new TransformComponent;};

    virtual Transform &   GetTransform     (VOID)                             {ApplyTranslation(); ApplyRotation(); ApplyScale(); return transform;};

    virtual Attr *        SetAttr          (const char *  szNameIn,
                                            const char *  szValueIn) override;

    VOID                  SetParent        (TransformComponent *  pcmpParentIn);

    static VOID           CacheTransform   (Component *             pcmpSiblingIn,
                                            TransformComponent * *  ppCachedTransformOut);

    BOOL                  TransformIsDirty (VOID)                          {return (transform.AncestorsAreDirty ());};

    VOID                  TranslationChangedListener (Attr *  pattrIn);

    VOID                  ApplyTranslation (VOID);

    VOID                  ApplyRotation    (VOID);

    VOID                  ApplyScale       (VOID);

    VOID                  ApplyChanges     (VOID)         {ApplyTranslation(); ApplyRotation(); ApplyScale();};

    FLOAT                 GetTx            (VOID)         {return pattrTx->Value ();};
    VOID                  SetTx            (FLOAT  fIn)   {pattrTx->Set (fIn); pattrTx->MarkAsDirty ();};

    FLOAT                 GetTy            (VOID)         {return pattrTy->Value ();};
    VOID                  SetTy            (FLOAT  fIn)   {pattrTy->Set (fIn); pattrTy->MarkAsDirty ();};

    FLOAT                 GetTz            (VOID)         {return pattrTz->Value ();};
    VOID                  SetTz            (FLOAT  fIn)   {pattrTz->Set (fIn); pattrTz->MarkAsDirty ();};


    FLOAT                 GetRx            (VOID)         {return pattrRx->Value ();};
    VOID                  SetRx            (FLOAT  fIn)   {pattrRx->Set (fIn); pattrRx->MarkAsDirty ();};

    FLOAT                 GetRy            (VOID)         {return pattrRy->Value ();};
    VOID                  SetRy            (FLOAT  fIn)   {pattrRy->Set (fIn); pattrRy->MarkAsDirty ();};

    FLOAT                 GetRz            (VOID)         {return pattrRz->Value ();};
    VOID                  SetRz            (FLOAT  fIn)   {pattrRz->Set (fIn); pattrRz->MarkAsDirty ();};


    FLOAT                 GetSx            (VOID)         {return pattrSx->Value ();};
    VOID                  SetSx            (FLOAT  fIn)   {pattrSx->Set (fIn); pattrSx->MarkAsDirty ();};

    FLOAT                 GetSy            (VOID)         {return pattrSy->Value ();};
    VOID                  SetSy            (FLOAT  fIn)   {pattrSy->Set (fIn); pattrSy->MarkAsDirty ();};

    FLOAT                 GetSz            (VOID)         {return pattrSz->Value ();};
    VOID                  SetSz            (FLOAT  fIn)   {pattrSz->Set (fIn); pattrSz->MarkAsDirty ();};

    VOID                  GetLocalMatrix   (RMatrix &  matOut)  {return transform.GetLocalMatrix (matOut);};

    VOID                  GetWorldMatrix   (RMatrix &  matOut)  {return transform.GetWorldMatrix (matOut);};

    RMatrix &             LocalMatrix      (VOID)         {return transform.LocalMatrix ();};

    RMatrix &             WorldMatrix      (VOID)         {return transform.WorldMatrix ();};

    RMatrix *             WorldMatrixPtr   (VOID)         {return transform.WorldMatrixPtr ();};

  };

#endif // TRANSFORMCOMPONENT_HPP
