/* -----------------------------------------------------------------
                             Component

     This module implements components that can be attached to nodes
     to form compositions.


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

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Sys/Types.hpp"
#include "Composite/Attr.hpp"
#include "Composite/AttrInt.hpp"
#include "Composite/AttrFloat.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/AttrIntArray.hpp"
#include "Composite/AttrFloatArray.hpp"
#include "Composite/AttrStringArray.hpp"
#include "Containers/TList.hpp"
#include "Containers/PtrArray.hpp"
#include "Util/Signal.h"

/**
   TODO:
    Attr Related
      ?GetAttr/SetAttr shortcuts for all types  (All attr names should be const char *)
      GetAttrAllMatches
      GetFirst/LastAttr
      GetNumAttrs, etc.


    Store an internal list of templates

  */


extern VOID AttrTemplatesInitialize   (VOID);
extern VOID AttrTemplatesUninitialize (VOID);


//-----------------------------------------------------------------------------
class Component
  {
  protected:

    RStr                  strType;
    TList<Attr*>          listAttr;
    INT                   iVersion; ///< used to detect when this component was changed/dirtied.
    BOOL                  bIsActive; ///< Active status is set/cleared when parent is active/inactive.
    BOOL                  bIsLoading; ///< Set to true if component is being loaded, and thus lots of attrs are changing.

    BOOL                  bIsListSentinel;
    Component *           pPrev;
    Component *           pNext;
    PVOID                 pParentNode;
    const char *          pParentID;

  public:
                          Component    ();
    virtual               ~Component   ();

            const char *  Type         (VOID) const              {return strType.AsChar ();};

    virtual Component *   GetInterface (const char *  szTypeIn)  {return NULL;};

    virtual Component *   Instantiate  (VOID) const;

    virtual VOID          CloneAttrs   (Component *  pcmpSourceIn);

    static Signal0<>      sigOnUpdate;     ///< Event fired before OnUpdate for node tree.  Best used for one-time setup.
    static Signal0<>      sigOnPreUpdate;  ///< Event fired before OnPreUpdate for node tree.  Best used for one-time setup.

    Signal0<>             sigOnDirty;      ///< Event fired when this component is marked as dirty.

    Signal2<Component*, BOOL>   sigOnActive;  ///< Event fired when this component is Active or Inactive

            INT           Version      (VOID)                    {return iVersion;};
            VOID          MarkAsDirty  (VOID)                    {++iVersion; sigOnDirty();};

    virtual VOID          OnEvent      (HASH_T  hEventIn);
    virtual VOID          OnAwake      (VOID);
    virtual VOID          OnStart      (VOID);
    virtual VOID          OnDelete     (VOID);
    virtual VOID          OnPreUpdate  (VOID); // before Update, after processing input
    virtual VOID          OnUpdate     (VOID);
    virtual VOID          OnPostUpdate (VOID); // after Update, before Display
    virtual VOID          OnClick      (VOID);
    virtual VOID          OnPress      (VOID);
    virtual VOID          OnRelease    (VOID);
    virtual VOID          OnUnpress    (VOID); // is this used?
    virtual VOID          OnRecache    (VOID);

    virtual VOID          OnDragStart  (FLOAT   fNDCPosX,
                                        FLOAT   fNDCPosY,
                                        FLOAT   fOrthoWidth,
                                        FLOAT   fOrthoHeight);
    virtual VOID          OnDrag       (FLOAT   fNDCPosX,
                                        FLOAT   fNDCPosY,
                                        FLOAT   fNDCDeltaX,   // given in parametric NDC coordinates
                                        FLOAT   fNDCDeltaY,
                                        FLOAT   fOrthoWidth,
                                        FLOAT   fOrthoHeight);
    virtual VOID          OnDrop       (const char *  szDroppedNodePath);
    virtual VOID          OnDragStop   (VOID);
    //virtual VOID          OnEnable     (VOID); // is this used?  Or is only active/inactive used?
    //virtual VOID          OnDisable    (VOID); // is this used?
    virtual VOID          OnActive     (VOID);
    virtual VOID          OnInactive   (VOID);
    virtual VOID          OnRenderStart(VOID);
    virtual VOID          OnDisplay    (VOID);
    virtual VOID          OnLoading    (BOOL  bIsLoadingIn);

    virtual VOID          OnTriggerCollide  (VOID *  pNodeIn); // called every update a moving object is colliding with this object, and this object's collider is marked as a trigger.

    virtual BOOL          IsDragLock   (VOID) {return FALSE;};

    virtual VOID          OnAction     (const char *  szActionIn);

    virtual VOID          GetActions   (PtrArray &  apActionsOut);

            VOID          SetActive    (BOOL  bStatusIn);

            BOOL          IsActive     (VOID)              {return bIsActive;};

    virtual VOID          SetLoading   (BOOL  bStatusIn);

            BOOL          IsLoading    (VOID)              {return bIsLoading;};

    virtual VOID          SetParentNode (PVOID         pParentIn,
                                         const char *  pszParentIDIn);  // pszParentID is for debugging.

            VOID *        ParentNode    (VOID)             {return pParentNode;};

    TListItr<Attr*>       FirstAttr     (VOID)             {return (listAttr.First());};

            Attr *        GetAttr      (const char *  szNameIn);

    virtual Attr *        SetAttr      (const char *  szNameIn,
                                        const char *  szValueIn);

    virtual Attr *        SetArrayAttr (const char *  szNameIn,
                                        INT           iIndex,
                                        const char *  szValueIn);

            Attr *        AddAttr      (const char *  szNameIn,
                                        const char *  szTypeIn,
                                        AttrString *  pattrRegistryKeyIn = NULL);

          AttrFloat *     AddAttrFloat       (const char *  szNameIn,
                                              AttrString *  pattrRegistryKeyIn = NULL);

            AttrInt *     AddAttrInt         (const char *  szNameIn,
                                              AttrString *  pattrRegistryKeyIn = NULL);

         AttrString *     AddAttrString      (const char *  szNameIn,
                                              AttrString *  pattrRegistryKeyIn = NULL);

     AttrFloatArray *     AddAttrFloatArray  (const char *  szNameIn,
                                              AttrString *  pattrRegistryKeyIn = NULL);

       AttrIntArray *     AddAttrIntArray    (const char *  szNameIn,
                                              AttrString *  pattrRegistryKeyIn = NULL);

    AttrStringArray *     AddAttrStringArray (const char *  szNameIn,
                                              AttrString *  pattrRegistryKeyIn = NULL);

            VOID          DeleteAttr   (const char *  szNameIn);

    virtual EStatus       Load         (RStrParser &  parserIn);

    virtual EStatus       Save         (RStrParser &  parserIn,
                                        RStr          strLineIndentIn) const;


    virtual VOID          DebugPrint   (const char *  szIndentIn,
                                        RStr &        strOut) const;

                                       /** @brief  Returns a pointer to the next entry in the list
                                           @return The pointer to the next entry
                                       */
    Component *           Next         (VOID) const                  {return pNext;};

                                       /** @brief  Returns a pointer to the previous entry in the list
                                           @return The pointer to the previous entry
                                       */
    Component *           Prev         (VOID) const                  {return pPrev;};

                                       /** @brief  Inserts an entry after this one.  The pNext and pPrev pointers are
                                                   updated for this entry, the one that currently follows it, and
                                                   the passed entry.
                                           @param  pcmpIn Pointer to the entry that will be inserted after this entry.
                                           @return None
                                       */
    EStatus               InsertAfter  (Component *  pcmpIn);

                                       /** @brief  Inserts an entry before this one.  The pNext and pPrev pointers are
                                                   updated for this entry, the one that currently precedes it, and
                                                   the passed entry.
                                           @param  pentIn Pointer to the entry that will be inserted before this entry.
                                           @return None
                                       */
    EStatus               InsertBefore (Component *  pentIn) {return (pPrev == NULL) ? EStatus::kFailure : pPrev->InsertAfter (pentIn);};


                                       /** @brief  Removes this entry from the linked list.  The pNext and pPrev pointers
                                                   are updated for the entries that follow and precede this entry,
                                                   and the pNext and pPrev pointers for this entry are set to NULL.
                                           @return None
                                       */
    VOID                  Remove       (VOID);

    Component *           GetFirstSibling  (VOID) const;

    BOOL                  IsValid          (VOID) const   {return !bIsListSentinel;};

    VOID                  MakeListSentinel (VOID)         {bIsListSentinel = TRUE; pNext = pPrev = this;};


  };

#endif // COMPONENT_HPP
