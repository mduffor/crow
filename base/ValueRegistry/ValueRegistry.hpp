/* -----------------------------------------------------------------
                            Value Registry

     This module implements a registry of Key Value pairs where the
     value can be one of several different base types.

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2016, Michael T. Duffy II.  All rights reserved.
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

#ifndef VALUEREGISTRY_HPP
#define VALUEREGISTRY_HPP

#include "Sys/Types.hpp"
#include "Containers/BaseArray.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Util/Signal.h"
#include "Containers/RStrArray.hpp"
#include "Containers/IntArray.hpp"
#include "Containers/FloatArray.hpp"
#include "Containers/DoubleArray.hpp"
#include "Math/RVec.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

// Define a macro that passes the full ValueRegistry Name if in debug mode,
//  but uses hash values in release mode.

#ifdef DEBUG
  #define VRN_(a) (a)
#else
  #define VRN_(a) (CalcHashValue(a))
#endif

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

using namespace Gallant;

//-----------------------------------------------------------------------------
class ValueElem
  {
  protected:
    UINT32  uType;
    RStr    strName;    ///< It would be nice for this to go away in release mode, using hash instead.
    HASH_T  uNameHash;
    RStr    strAltName;  ///< Note: This might not be used.
    INT     iOrder;      ///< For variable lists, gives left->right order of the variable.  For ValueRegistries set by ConfigDeck, gives the order of the layer that set this element.
    BOOL    bSave;
    BOOL    bIsUniqueSet;      ///< If true, the array type will be treated as a unique set of values.

    static RStr    strStringOut; // used by GetString for string conversion purposes.
                                 // since only one copy exists, it is not thread safe

  public:
    // event on_changed_event
    Signal2<ValueElem*, BOOL>  sigOnChanged;  ///< Event fired when the value of this attr changes
    Signal1<ValueElem*>        sigOnDelete;   ///< Event fired when this attr is deleted, so it can be removed from references and lists.

    // default types

    static const UINT32   kTypeInt;
    static const UINT32   kTypeFloat;
    static const UINT32   kTypeDouble;
    static const UINT32   kTypeBool;
    static const UINT32   kTypeString;
    static const UINT32   kTypeBlob;
    static const UINT32   kTypeIntArray;
    static const UINT32   kTypeFloatArray;
    static const UINT32   kTypeDoubleArray;
    static const UINT32   kTypeStringArray;
    static const UINT32   kTypePtr;
    static const UINT32   kTypeLink;
    static const UINT32   kTypeVec;

  public:

                          ValueElem     ()                    {iOrder = -1; uType = 0; bIsUniqueSet = FALSE; bSave = FALSE; uNameHash = 0;};

    virtual               ~ValueElem    ()                    {CallOnDelete (this);};

    UINT32                GetType       (VOID)                {return uType;};

    BOOL                  IsType        (UINT32  uTypeIn)     {return (uType == uTypeIn);};

    const char *          GetName       (VOID)                {return strName.AsChar ();};

    const RStr &          GetNameStr    (VOID)                {return strName;};

    HASH_T                GetNameHash   (VOID)                {return uNameHash;};

    VOID                  ZeroName      (VOID)                {strName.Empty(); uNameHash = 0;};

    BOOL                  MatchesName   (const char *  szNameIn,
                                         HASH_T        uHashIn)     {return (strName.Equals    (uHashIn, szNameIn) ||
                                                                             strAltName.Equals (uHashIn, szNameIn));};
    BOOL                  MatchesName   (const RStr &  strNameIn)   {return (strName.Equals    (strNameIn) ||
                                                                             strAltName.Equals (strNameIn));};

    BOOL                  MatchesNameHash  (HASH_T  uNameHashIn)    {return (uNameHash == uNameHashIn);};

    BOOL                  NameContains     (const char *  szIn)     {return (strName.Contains(szIn));};

    VOID                  SetType       (const char *  szIn)  {uType = MAKE_FOUR_CODE(szIn);};

    VOID                  SetName       (const char *  szIn);

    VOID                  SetAltName    (const char *  szIn);

    VOID                  SetNameHash   (HASH_T  uHashIn)     {uNameHash = uHashIn;};

    VOID                  CallOnChanged (ValueElem *  pelemChanged,
                                         BOOL         bUpdating)     {sigOnChanged(this, bUpdating);};

    VOID                  CallOnDelete  (ValueElem *  pelemChanged)  {sigOnDelete(this);};

    VOID                  SetOrder      (INT  iIn)    {iOrder = iIn;};

    INT                   GetOrder      (VOID)        {return (iOrder);};

    virtual INT           GetInt        (VOID)        {return 0;};

    virtual VOID          SetInt        (INT   iIn,
                                         BOOL  bUpdating = FALSE)    {};

    virtual FLOAT         GetFloat      (VOID)        {return 0.0f;};

    virtual VOID          SetFloat      (FLOAT  fIn,
                                         BOOL   bUpdating = FALSE)  {};

    virtual DOUBLE        GetDouble     (VOID)        {return 0.0;};

    virtual VOID          SetDouble     (DOUBLE  dIn,
                                         BOOL    bUpdating = FALSE)  {};

    virtual RVec4 &       GetVec        (VOID)        {return RVec4::kZero;};

    virtual VOID          SetVec        (RVec4 &  vecIn,
                                         BOOL     bUpdating = FALSE)  {};

    virtual BOOL          GetBool       (VOID)        {return FALSE;};

    virtual VOID          SetBool       (BOOL  bIn,
                                         BOOL  bUpdating = FALSE)   {};

    virtual const char *  GetString     (VOID) const           {return NULL;};

    virtual VOID          GetString     (RStr &  strOut) const {};

    virtual VOID          SetString     (const char *  szIn,
                                         BOOL          bUpdating = FALSE)  {};

    virtual const void *  GetBlob         (VOID)       {return (NULL);};

    virtual VOID          GetBlob         (RStr &        strOut)         {};

    virtual VOID          SetBlob         (const RStr *  strIn)          {};

    BOOL                  IsUniqueSet     (VOID)                         {return bIsUniqueSet;};

    VOID                  MakeUniqueSet   (BOOL  bIsUniqueSetIn)         {bIsUniqueSet = bIsUniqueSetIn;};

    virtual INT           GetArrayLength   (VOID)                        {return (0);};

    virtual VOID          DeleteArrayElem (INT  iIndexIn)                {};

    virtual VOID          ClearArray      (VOID)                         {};

    virtual const char *  GetStringInArray    (INT  iIndexIn)                {return "";};

    virtual VOID          SetStringInArray    (const char *  szIn,
                                               INT           iIndexIn = -1)  {};

    virtual VOID          DeleteStringInArray (const char *  szIn)           {};

    virtual VOID          SetStringArray      (RStrArray &  astrIn)          {};

    virtual RStrArray *   GetStringArray      (VOID)                         {return NULL;};

    virtual IntArray *    GetIntArray     (VOID)                {return NULL;};

    virtual VOID          SetIntArray     (IntArray &    aiIn)  {};

    virtual FloatArray *  GetFloatArray   (VOID)                {return NULL;};

    virtual VOID          SetFloatArray   (FloatArray &  afIn)  {};

    virtual VOID          SetFloatArray   (FLOAT *       pafIn) {};

    virtual DoubleArray * GetDoubleArray  (VOID)                {return NULL;};

    virtual VOID          SetDoubleArray  (DoubleArray &  adIn)  {};

    virtual VOID          SetDoubleArray  (DOUBLE *       padIn) {};

    virtual PVOID         GetPtr        (VOID)         {return NULL;};

    virtual VOID          SetPtr        (PVOID  pvIn)  {};

    virtual HASH_T        GetLink       (VOID)         {return 0;};

    virtual VOID          SetLink       (HASH_T  uIn)  {};

    virtual VOID          SetByType     (ValueElem &  elemIn);

    virtual ValueElem *   Clone         (VOID) = 0;

    virtual VOID          Clear         (VOID) = 0; // delete/reset value

    virtual VOID          MarkForSave   (BOOL  bStatusIn)    {bSave = bStatusIn;};

    BOOL                  ShouldSave    (VOID)               {return (bSave);};

    virtual VOID          ToParser      (RStrParser &  parser) {};

    virtual VOID          FromParser    (RStrParser &  parser) {};

  protected:
    VOID                  CloneValues   (ValueElem &  elemIn)   {uType = elemIn.uType;
                                                                 uNameHash = elemIn.uNameHash;
                                                                 SetName    (elemIn.strName);
                                                                 SetAltName (elemIn.strAltName);};

  };

// TODO: Serialize/Deserialize.  Mark ValueElem as savable or not.

//-----------------------------------------------------------------------------
class ValueRegistry
  {
  private:
    static ValueRegistry *  pRoot;

  protected:
    ValueRegistry *  pFallback; ///< If there is a Read that isn't in this registry, the fallback registry is used.  Does not affect writes.

  public:

                          ValueRegistry  () {pFallback = NULL;};

    virtual               ~ValueRegistry () {};

    VOID                  SetFallback   (ValueRegistry *  pRegIn);

    virtual INT           GetInt        (const char *  szNameIn)    const = 0;
    virtual INT           GetInt        (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetInt        (const char *  szNameIn,
                                         INT           iIn,
                                         BOOL          bUpdating = FALSE) = 0;
    virtual ValueElem *   SetInt        (HASH_T        uNameHashIn,
                                         INT           iIn,
                                         BOOL          bUpdating = FALSE) = 0;

    virtual FLOAT         GetFloat      (const char *  szNameIn)    const = 0;
    virtual FLOAT         GetFloat      (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetFloat      (const char *  szNameIn,
                                         FLOAT         fIn,
                                         BOOL          bUpdating = FALSE) = 0;
    virtual ValueElem *   SetFloat      (HASH_T        uNameHashIn,
                                         FLOAT         fIn,
                                         BOOL          bUpdating = FALSE) = 0;

    virtual DOUBLE        GetDouble     (const char *  szNameIn)    const = 0;
    virtual DOUBLE        GetDouble     (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetDouble     (const char *  szNameIn,
                                         DOUBLE        dIn,
                                         BOOL          bUpdating = FALSE) = 0;
    virtual ValueElem *   SetDouble     (HASH_T        uNameHashIn,
                                         DOUBLE        dIn,
                                         BOOL          bUpdating = FALSE) = 0;

    virtual RVec4 &       GetVec        (const char *  szNameIn)    const = 0;
    virtual RVec4 &       GetVec        (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetVec        (const char *  szNameIn,
                                         RVec4 &       vecIn,
                                         BOOL          bUpdating = FALSE) = 0;
    virtual ValueElem *   SetVec        (HASH_T        uNameHashIn,
                                         RVec4 &       vecIn,
                                         BOOL          bUpdating = FALSE) = 0;

    virtual BOOL          GetBool       (const char *  szNameIn)    const = 0;
    virtual BOOL          GetBool       (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetBool       (const char *  szNameIn,
                                         BOOL          bIn,
                                         BOOL          bUpdating = FALSE) = 0;
    virtual ValueElem *   SetBool       (HASH_T        uNameHashIn,
                                         BOOL          bIn,
                                         BOOL          bUpdating = FALSE) = 0;

    virtual const char *  GetString     (const char *  szNameIn)    const = 0;
    virtual const char *  GetString     (HASH_T        uNameHashIn) const = 0;

    virtual VOID          GetString     (const char *  szNameIn,
                                         RStr &        strOut)      const = 0;
    virtual VOID          GetString     (HASH_T        uNameHashIn,
                                         RStr &        strOut)      const = 0;

    virtual ValueElem *   SetString     (const char *  szNameIn,
                                         const char *  szIn,
                                         BOOL          bUpdating = FALSE) = 0;
    virtual ValueElem *   SetString     (HASH_T        uNameHashIn,
                                         const char *  szIn,
                                         BOOL          bUpdating = FALSE) = 0;

    virtual const void *  GetBlob       (const char *  szNameIn)    const = 0;
    virtual const void *  GetBlob       (HASH_T        uNameHashIn) const = 0;

    virtual VOID          GetBlob       (const char *  szNameIn,
                                         RStr &        strOut)      const = 0;
    virtual VOID          GetBlob       (HASH_T        uNameHashIn,
                                         RStr &        strOut)      const = 0;

    virtual ValueElem *   SetBlob       (const char *  szNameIn,
                                         const RStr *  strIn) = 0;
    virtual ValueElem *   SetBlob       (HASH_T        uNameHashIn,
                                         const RStr *  strIn) = 0;

    virtual INT           GetArrayLength  (const char *  szNameIn)    const = 0;
    virtual INT           GetArrayLength  (HASH_T        uNameHashIn) const = 0;

    virtual VOID          DeleteArrayElem (const char *  szNameIn,
                                           INT  iIndexIn) = 0;
    virtual VOID          DeleteArrayElem (HASH_T        uNameHashIn,
                                           INT  iIndexIn) = 0;

    virtual VOID          ClearArray      (const char *  szNameIn) = 0;
    virtual VOID          ClearArray      (HASH_T        uNameHashIn) = 0;

    virtual const char *  GetStringInArray (const char *  szNameIn,
                                            INT           iIndexIn) const = 0;
    virtual const char *  GetStringInArray (HASH_T        uNameHashIn,
                                            INT           iIndexIn) const = 0;

    virtual ValueElem *   SetStringInArray (const char *  szNameIn,
                                            const char *  szIn,
                                            INT           iIndexIn = -1) = 0;
    virtual ValueElem *   SetStringInArray (HASH_T        uNameHashIn,
                                            const char *  szIn,
                                            INT           iIndexIn = -1) = 0;

    virtual ValueElem *   SetStringArray  (const char *  szNameIn,
                                           RStrArray &    aiIn) = 0;
    virtual ValueElem *   SetStringArray  (HASH_T        uNameHashIn,
                                           RStrArray &    aiIn) = 0;

    virtual ValueElem *   DeleteStringInArray  (const char *  szNameIn,
                                                const char *  szIn) = 0;
    virtual ValueElem *   DeleteStringInArray  (HASH_T        uNameHashIn,
                                                const char *  szIn) = 0;

    virtual RStrArray *   GetStringArray  (const char *  szNameIn)    const = 0;
    virtual RStrArray *   GetStringArray  (HASH_T        uNameHashIn) const = 0;

    virtual IntArray *    GetIntArray     (const char *  szNameIn)    const = 0;
    virtual IntArray *    GetIntArray     (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetIntArray     (const char *  szNameIn,
                                           IntArray &    aiIn) = 0;
    virtual ValueElem *   SetIntArray     (HASH_T        uNameHashIn,
                                           IntArray &    aiIn) = 0;

    virtual FloatArray *  GetFloatArray   (const char *  szNameIn)    const = 0;
    virtual FloatArray *  GetFloatArray   (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetFloatArray   (const char *  szNameIn,
                                           FloatArray &  afIn) = 0;
    virtual ValueElem *   SetFloatArray   (HASH_T        uNameHashIn,
                                           FloatArray &  afIn) = 0;

    virtual ValueElem *   SetFloatArray   (const char *  szNameIn,
                                           FLOAT *       pafIn) = 0;
    virtual ValueElem *   SetFloatArray   (HASH_T        uNameHashIn,
                                           FLOAT *       pafIn) = 0;

    virtual DoubleArray * GetDoubleArray  (const char *  szNameIn)    const = 0;
    virtual DoubleArray * GetDoubleArray  (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetDoubleArray  (const char *  szNameIn,
                                           DoubleArray &  adIn) = 0;
    virtual ValueElem *   SetDoubleArray  (HASH_T        uNameHashIn,
                                           DoubleArray &  adIn) = 0;

    virtual ValueElem *   SetDoubleArray  (const char *  szNameIn,
                                           DOUBLE *       padIn) = 0;
    virtual ValueElem *   SetDoubleArray  (HASH_T        uNameHashIn,
                                           DOUBLE *       padIn) = 0;

    virtual PVOID         GetPtr        (const char *  szNameIn)    const = 0;
    virtual PVOID         GetPtr        (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetPtr        (const char *  szNameIn,
                                         PVOID         pvIn) = 0;
    virtual ValueElem *   SetPtr        (HASH_T        uNameHashIn,
                                         PVOID         pvIn) = 0;

    virtual HASH_T        GetLink       (const char *  szNameIn)    const = 0;
    virtual HASH_T        GetLink       (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   SetLink       (const char *  szNameIn,
                                         HASH_T        uIn) = 0;
    virtual ValueElem *   SetLink       (HASH_T        uNameHashIn,
                                         HASH_T        uIn) = 0;

    virtual ValueElem *   SetClone      (ValueElem *   pelemIn) = 0;

    virtual BOOL          HasKey        (const char *  szNameIn) const = 0;
    virtual BOOL          HasKey        (HASH_T        uNameHashIn) const = 0;

    virtual BOOL          IsInt         (const char *  szNameIn) const = 0;
    virtual BOOL          IsInt         (HASH_T        uNameHashIn) const = 0;

    virtual BOOL          IsFloat       (const char *  szNameIn) const = 0;
    virtual BOOL          IsFloat       (HASH_T        uNameHashIn) const = 0;

    virtual BOOL          IsDouble      (const char *  szNameIn) const = 0;
    virtual BOOL          IsDouble      (HASH_T        uNameHashIn) const = 0;

    virtual BOOL          IsVec         (const char *  szNameIn) const = 0;
    virtual BOOL          IsVec         (HASH_T        uNameHashIn) const = 0;

    virtual BOOL          IsBool        (const char *  szNameIn) const = 0;
    virtual BOOL          IsBool        (HASH_T        uNameHashIn) const = 0;

    virtual BOOL          IsString      (const char *  szNameIn) const = 0;
    virtual BOOL          IsString      (HASH_T        uNameHashIn) const = 0;

    virtual BOOL          IsPtr         (const char *  szNameIn) const = 0;
    virtual BOOL          IsPtr         (HASH_T        uNameHashIn) const = 0;

    virtual BOOL          IsLink        (const char *  szNameIn) const = 0;
    virtual BOOL          IsLink        (HASH_T        uNameHashIn) const = 0;

    virtual VOID          Clear         (VOID) = 0; // delete all values

    virtual VOID          ClearElem     (const char *  szNameIn) = 0; // delete specific value
    virtual VOID          ClearElem     (HASH_T        uNameHashIn) = 0; // delete specific value

    virtual VOID          DeleteElem    (ValueElem *   pElemIn) = 0;

    virtual ValueRegistry &  operator=  (const ValueRegistry &  regIn) = 0;

    virtual INT           Size          (VOID) const = 0; // returns the number of ValueElems in the registry.

    virtual VOID          SetOrder      (const char *  szNameIn,
                                         INT           iOrderIn) = 0;
    virtual VOID          SetOrder      (HASH_T        uNameHashIn,
                                         INT           iOrderIn) = 0;

    virtual ValueElem *   Find          (const char *  szNameIn) const = 0;
    virtual ValueElem *   Find          (HASH_T        uNameHashIn) const = 0;

    virtual ValueElem *   FindByOrder   (INT  iOrderIn) const = 0;

    virtual ValueElem *   FindByIndex   (INT  iIndexIn) const = 0;

    virtual VOID          DebugPrint    (const char *  szFilter = NULL) = 0;

    static  ValueRegistry *  Root       (VOID)    {return pRoot;};

    static  VOID             SetRoot    (ValueRegistry *  pRootRegistryIn)  {pRoot = pRootRegistryIn;};

    static  VOID             DestroyRoot (VOID)                             {if (pRoot != NULL) delete pRoot; pRoot = NULL;};

    virtual VOID          MarkForSave   (const char *  szNameIn,
                                         BOOL          bSaveStatusIn = TRUE);
    virtual VOID          MarkForSave   (HASH_T        uNameHashIn,
                                         BOOL          bSaveStatusIn = TRUE);

    virtual VOID          ToParser      (RStrParser &  parser) {};

    virtual VOID          FromParser    (RStrParser &  parser) {};

    VOID                  SetByType     (ValueElem &   elemIn);

    virtual VOID          Overlay       (ValueRegistry *  regToOverlay,
                                         INT              iOrderIn);


                          /** @brief Utility function to swap the position in storage of two elements.  Will be used to move elements to delete to the end of the list to avoid messing up indexed order.
                          */
    virtual VOID          SwapIndexes   (ValueElem &  elemOne,
                                         ValueElem &  elemTwo) = 0;

    const char *          ExpandVars (RStrParser &  parserIn) const;

  };

#endif // VALUEREGISTRY_HPP
