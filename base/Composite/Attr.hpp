/* -----------------------------------------------------------------
                             Attr

     This module implements named attributes

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

#ifndef ATTR_HPP
#define ATTR_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Util/Signal.h"
#include "ValueRegistry/ValueRegistry.hpp"

/**

  */


/*
 * TODO:
    //InputConnection
    //OutputConnections
    //Animation Curve (handled through connection?)

    ?Comparison
    ?Binary operators
    Some way to do composite nodes (vector with values mapped to .x, .y, .z, etc)

    //GetFullPath // can it get the full path without knowing its parent?

    //VOID *  pComponent;   //< Component that contains this attr


    Connect/Disconnect/DisconnectAll/DisconnectOutputs
    Update from animation
    Write value to animation curve
    ?Functions to access and modify animation curve

 */

using namespace Gallant;

class Attr;

//-----------------------------------------------------------------------------
class Attr
  {
  protected:

    const char *  szType;      ///< Type of data contained in this attr.  Always points to a constant in derived classes.
    INT32         ccType;      ///< Four CC Identifier.

    BOOL          bLocked;      ///< Able to be edited.
    BOOL          bPublished;   ///< Visible to the UI.
    BOOL          bDirty;       ///< True if the value has been changed.
    INT           iVersion;     ///< Version of the data, incremented each time it is changed.

    INT           iArrayIndex;  ///< If the attr represents one element in an array, this is its index into the array.

  public:
    RStr          strName;     ///< String by which this attr is identified.  Encapsulation broken (public) to allow for efficient serialization.

    Signal1<Attr*>  sigOnChanged;  ///< Event fired when the value of this attr changes
    Signal1<Attr*>  sigOnDelete;   ///< Event fired when this attr is deleted, so it can be removed from references and lists.

    ValueRegistry *         pCachedRegistry;
    RStr                    strRegistryKey;
    ValueElem *             pCachedElem;

  protected:

    virtual VOID          CopyCommonVars (const Attr &  attrIn);

  public:
                          Attr         (const char *  szNameIn);
    virtual               ~Attr        ();

                          /// @brief  Get the string identifier for this Attr type.
                          /// @return Pointer to a null terminated string that identifies the type of this attr.
            const char *  Type         (VOID) const   {return szType;};

                          /// @brief  Get the FOURCC (32-bit int) identifier for this Attr type.
                          /// @return A 32-bit int that identifies the type of this attr.
            INT32         CCType       (VOID) const   {return ccType;};

                          /// @brief  Get the name for the instance of this attr
                          /// @return Pointer to a null terminated string that gives the name of this attr.
            const char *  Name         (VOID) const   {return strName.AsChar ();};

                          /// @brief  Set the name for this attr
                          /// @param  szNameIn Pointer to a null terminated string giving the attr's name.
            VOID          SetName      (const char *  szNameIn)    {strName = szNameIn;};

                          /// @brief  Check if this attr matches a given type
                          /// @param  szTypeIn Pointer to a null terminated string giving the type to test.
                          /// @return True if this attr's type matches the given string.
            BOOL          IsType       (const char *  szTypeIn)    {return streq (szTypeIn, szType);};

                          /// @brief  Check if this attr matches a given type
                          /// @param  iTypeIn Pointer to the FOURCC code giving the type to test.
                          /// @return True if this attr's type matches the given code.
            BOOL          IsCCType     (INT32  iTypeIn)            {return iTypeIn == ccType;};

            INT           Version      (VOID)                    {return iVersion;};
            VOID          MarkAsDirty  (VOID)                    {++iVersion;};

    virtual Attr *        Instantiate  (const char *  szName) = 0;

    virtual const char *  GetAsString       (RStr *  pstrValueOut) const = 0;

    virtual VOID          SetByString       (const char *  szValueIn)   {MarkAsDirty ();};

    virtual VOID          SetArrayByString  (INT           iIndexIn,
                                             const char *  szValueIn)   {MarkAsDirty ();};

    virtual VOID          Set               (Attr *  pattrIn) = 0;

    virtual VOID          Clear             (VOID)                      {MarkAsDirty ();};

    virtual VOID          DebugPrint   (RStr &        strOut) const  {}; // customizeable

    virtual VOID          DebugPrint   (const char *  szIndentIn,
                                        RStr &        strOut) const; // general / default


                          /// @brief OnChanged must be called by the derived classes whenever they change or clear their value.
                          ///           Attach with pAttr->sigOnChanged.Connect( &l, &Label::OnChangedListener );
                          /// @return None.
            VOID          OnChanged    (VOID)  {sigOnChanged(this);};

            VOID          OnDelete     (VOID)  {sigOnDelete(this);};

    static Attr *         New          (const char *  szNameIn,
                                        const char *  szTypeIn);

    static VOID           AddTemplate  (Attr *  pAttrIn);

    static VOID           DeleteAllTemplates     (VOID);

    VOID                  SetRegistryKey         (const char *  szKeyIn);

    VOID                  CacheRegistry          (VOID);

    VOID                  DisconnectFromRegistry (VOID);

    VOID                  OnElemChangedListener  (ValueElem *  pelemIn,
                                                  BOOL         bUpdatingIn);

    VOID                  OnElemDeletedListener  (ValueElem *  pelemIn);

    virtual VOID          UpdateFromRegistry     (BOOL         bUpdatingIn);

    VOID                  CacheElemConnections   (VOID);


  };

#endif // ATTR_HPP
