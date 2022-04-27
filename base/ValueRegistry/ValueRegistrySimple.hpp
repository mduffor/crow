/* -----------------------------------------------------------------
                            Value Registry Simple

     This module implements a simple version of the ValueRegistry

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

#ifndef VALUEREGISTRYSIMPLE_HPP
#define VALUEREGISTRYSIMPLE_HPP

#include <stdlib.h>

#include "Sys/Types.hpp"
#include "Util/Signal.h"
#include "Containers/TList.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Containers/IntArray.hpp"
#include "Containers/FloatArray.hpp"
#include "Containers/DoubleArray.hpp"
#include "Containers/RStrArray.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ValueElemInt : public ValueElem
  {
  private:
    INT            iValue;

  public:

                  ValueElemInt  ()            {SetType ("INT_"); iValue = 0;};

                  ~ValueElemInt ()            {};

    INT           GetInt        (VOID)                      override {return iValue;};

    VOID          SetInt        (INT  iIn,
                                 BOOL bUpdating = FALSE)    override {iValue = iIn; CallOnChanged (this, bUpdating);};

    FLOAT         GetFloat      (VOID)                      override {return ((FLOAT)iValue);};

    VOID          SetFloat      (FLOAT  fIn,
                                 BOOL   bUpdating = FALSE)  override {iValue = (INT) floorf (fIn); CallOnChanged (this, bUpdating);};

    DOUBLE        GetDouble     (VOID)                      override {return ((DOUBLE)iValue);};

    VOID          SetDouble     (DOUBLE  dIn,
                                 BOOL    bUpdating = FALSE) override {iValue = (INT) floor (dIn); CallOnChanged (this, bUpdating);};

    VOID          GetString     (RStr &  strOut) const      override {strOut.AppendFormat ("%d", iValue);};

    const char *  GetString     (VOID) const                override {strStringOut.Empty (); GetString (strStringOut); return (strStringOut.AsChar ());};

    VOID          SetString     (const char *  szIn,
                                 BOOL          bUpdating = FALSE) override {iValue = static_cast <INT32> (strtol (szIn, NULL, 10)); CallOnChanged (this, bUpdating);};

    BOOL          GetBool       (VOID)                      override {return (iValue != 0);};

    ValueElem *   Clone         (VOID)                      override {ValueElemInt * pelemNew = new ValueElemInt; pelemNew->CloneValues (*this); pelemNew->iValue = iValue; return pelemNew;};

    VOID          Clear         (VOID)                      override {iValue = 0; CallOnChanged (this, FALSE);};

    VOID          ToParser      (RStrParser &  parser)      override {parser.SetU4_BEnd (iValue);};

    VOID          FromParser    (RStrParser &  parser)      override {iValue = parser.GetU4_BEnd ();};
  };

//-----------------------------------------------------------------------------
class ValueElemFloat : public ValueElem
  {
  private:
    FLOAT        fValue;

  public:

                  ValueElemFloat  ()          {SetType ("FLT_"); fValue = 0.0f;};

                  ~ValueElemFloat ()          {};

    VOID          SetInt        (INT  iIn,
                                 BOOL bUpdating = FALSE)    override {fValue = (FLOAT) iIn; CallOnChanged (this, bUpdating);};

    INT           GetInt        (VOID)                      override {return ((INT) floorf (fValue));};

    FLOAT         GetFloat      (VOID)                      override {return fValue;};

    VOID          SetFloat      (FLOAT  fIn,
                                 BOOL   bUpdating = FALSE)  override {fValue = fIn; CallOnChanged (this, bUpdating);};

    DOUBLE        GetDouble     (VOID)                      override {return (DOUBLE) fValue;};

    VOID          SetDouble     (DOUBLE  dIn,
                                 BOOL    bUpdating = FALSE) override {fValue = (FLOAT) dIn; CallOnChanged (this, bUpdating);};

    BOOL          GetBool       (VOID)                      override {return (!FLT_APPROX_EQUAL (fValue, 0.0f));};

    VOID          GetString     (RStr &  strOut) const      override {strOut.AppendFormat ("%f", fValue);};

    const char *  GetString     (VOID) const                override {strStringOut.Empty (); GetString (strStringOut); return (strStringOut.AsChar ());};

    VOID          SetString     (const char *  szIn,
                                 BOOL          bUpdating = FALSE) override  {fValue = FLOAT (strtod (szIn, NULL)); CallOnChanged (this, bUpdating);};

    ValueElem *   Clone         (VOID)                      override {ValueElemFloat * pelemNew = new ValueElemFloat; pelemNew->CloneValues (*this); pelemNew->fValue = fValue; return pelemNew;};

    VOID          Clear         (VOID)                      override {fValue = 0.0f; CallOnChanged (this, FALSE);};

    VOID          ToParser      (RStrParser &  parser)      override {parser.SetF4_BEnd (fValue);};

    VOID          FromParser    (RStrParser &  parser)      override {fValue = parser.GetF4_BEnd ();};

  };

//-----------------------------------------------------------------------------
class ValueElemDouble : public ValueElem
  {
  private:
    DOUBLE       dValue;

  public:

                  ValueElemDouble  ()          {SetType ("DBL_"); dValue = 0.0;};

                  ~ValueElemDouble ()          {};

    VOID          SetInt      (INT  iIn,
                               BOOL bUpdating = FALSE)    override {dValue = (DOUBLE) iIn; CallOnChanged (this, bUpdating);};

    INT           GetInt      (VOID)                      override {return ((INT) floor (dValue));};

    FLOAT         GetFloat    (VOID)                      override {return (FLOAT) dValue;};

    VOID          SetFloat    (FLOAT  fIn,
                               BOOL   bUpdating = FALSE)  override {dValue = (DOUBLE) fIn; CallOnChanged (this, bUpdating);};

    DOUBLE        GetDouble   (VOID)                      override {return dValue;};

    VOID          SetDouble   (DOUBLE  dIn,
                               BOOL    bUpdating = FALSE) override {dValue = dIn; CallOnChanged (this, bUpdating);};

    BOOL          GetBool     (VOID)                      override {return (!DBL_APPROX_EQUAL (dValue, 0.0));};

    VOID          GetString   (RStr &  strOut) const      override {strOut.AppendFormat ("%g", dValue);}; // Approximation

    const char *  GetString   (VOID) const                override {strStringOut.Empty (); GetString (strStringOut); return (strStringOut.AsChar ());};

    VOID          SetString   (const char *  szIn,
                               BOOL          bUpdating = FALSE) override {dValue = DOUBLE (strtod (szIn, NULL)); CallOnChanged (this, bUpdating);};

    ValueElem *   Clone       (VOID)                      override {ValueElemDouble * pelemNew = new ValueElemDouble; pelemNew->CloneValues (*this); pelemNew->dValue = dValue; return pelemNew;};

    VOID          Clear       (VOID)                      override {dValue = 0.0; CallOnChanged (this, FALSE);};

    VOID          ToParser    (RStrParser &  parser)      override {parser.SetF8_BEnd (dValue);};

    VOID          FromParser  (RStrParser &  parser)      override {dValue = parser.GetF8_BEnd ();};

  };

//-----------------------------------------------------------------------------
class ValueElemVec : public ValueElem
  {
  private:
    RVec4          vecValue;

  public:

                  ValueElemVec  ()          {SetType ("VEC_"); vecValue.Zero ();};

                  ~ValueElemVec ()          {};

    RVec4 &       GetVec        (VOID)                 override {return vecValue;};

    VOID          SetVec        (RVec4 &  vecIn,
                                 BOOL     bUpdating = FALSE) override {vecValue.Set (vecIn); CallOnChanged (this, bUpdating);};

    VOID          GetString     (RStr &  strOut) const       override {strOut.AppendFormat ("(%f, %f, %f, %f)", vecValue.fX, vecValue.fY, vecValue.fZ, vecValue.fW);};

    const char *  GetString     (VOID) const                 override {strStringOut.Empty (); GetString (strStringOut); return (strStringOut.AsChar ());};

    //VOID          SetString     (const char *  szIn,
    //                             BOOL          bUpdating = FALSE)   {vecValue = FLOAT (strtod (szIn, NULL)); CallOnChanged (this, bUpdating);};

    ValueElem *   Clone         (VOID)                 override {ValueElemVec * pelemNew = new ValueElemVec; pelemNew->CloneValues (*this); pelemNew->vecValue = vecValue; return pelemNew;};

    VOID          Clear         (VOID)                 override {vecValue.Zero (); CallOnChanged (this, FALSE);};

    VOID          ToParser      (RStrParser &  parser) override {parser.SetF4_BEnd (vecValue.fX); parser.SetF4_BEnd (vecValue.fY); parser.SetF4_BEnd (vecValue.fZ); parser.SetF4_BEnd (vecValue.fW);};

    VOID          FromParser    (RStrParser &  parser) override {vecValue.fX = parser.GetF4_BEnd (); vecValue.fY = parser.GetF4_BEnd (); vecValue.fZ = parser.GetF4_BEnd (); vecValue.fW = parser.GetF4_BEnd (); };

  };


//-----------------------------------------------------------------------------
class ValueElemBool : public ValueElem
  {
  private:
    BOOL     bValue;

  public:

                  ValueElemBool  ()            {SetType ("BOOL"); bValue = FALSE;};

                  ~ValueElemBool ()            {};

    INT           GetInt         (VOID)                    override {return (bValue ? 1 : 0);};

    FLOAT         GetFloat       (VOID)                    override {return (bValue ? 1.0f : 0.0f);};

    DOUBLE        GetDouble      (VOID)                    override {return (bValue ? 1.0 : 0.0);};

    BOOL          GetBool        (VOID)                    override {return bValue;};

    VOID          SetBool        (BOOL  bIn,
                                  BOOL  bUpdating = FALSE) override {bValue = bIn; CallOnChanged (this, bUpdating);};

    VOID          SetInt         (INT   iIn,
                                  BOOL  bUpdating = FALSE) override {bValue = (iIn != 0); CallOnChanged (this, bUpdating);};

    VOID          GetString      (RStr &  strOut) const    override {strOut.AppendFormat ("%s", bValue ? "True" : "False");};

    const char *  GetString      (VOID) const              override {strStringOut.Empty (); GetString (strStringOut); return (strStringOut.AsChar ());};

    ValueElem *   Clone          (VOID)                    override {ValueElemBool * pelemNew = new ValueElemBool; pelemNew->CloneValues (*this); pelemNew->bValue = bValue; return pelemNew;};

    VOID          Clear          (VOID)                    override {bValue = FALSE; CallOnChanged (this, FALSE);};

    VOID          ToParser       (RStrParser &  parser)    override {parser.SetU4_BEnd (bValue ? 1 : 0);};

    VOID          FromParser     (RStrParser &  parser)    override {bValue = (parser.GetU4_BEnd () == 1);};
  };

//-----------------------------------------------------------------------------
class ValueElemString : public ValueElem
  {
  private:
    RStr       strValue;

  public:

                  ValueElemString  ()             {SetType ("STR_"); strValue.Empty ();};

                  ~ValueElemString ()             {};

    const char *  GetString        (VOID) const           override {return strValue.AsChar ();};

    VOID          SetString        (const char *  szIn,
                                    BOOL          bUpdating = FALSE) override {strValue.Set (szIn, TRUE); CallOnChanged (this, bUpdating);};

    VOID          GetString        (RStr &  strOut) const override {strOut.AppendString (strValue);};

    BOOL          GetBool          (VOID)                 override {if (strValue.IsEmpty ()) return FALSE;
                                                                    if (streqi ("false", strValue.AsChar())) return FALSE;
                                                                    return TRUE;};

    ValueElem *   Clone            (VOID)                 override {ValueElemString * pelemNew = new ValueElemString; pelemNew->CloneValues (*this); pelemNew->strValue = strValue; return pelemNew;};

    VOID          Clear            (VOID)                 override {strValue.Empty (); CallOnChanged (this, FALSE);};

    VOID          ToParser         (RStrParser &  parser) override {parser.SetU4_BEnd (strValue.Length ()),
                                                                    parser.SetData    (strValue.AsUChar (), strValue.Length ());};

    VOID          FromParser       (RStrParser &  parser) override {strValue.Empty ();
                                                                    INT  iLength = parser.GetU4_BEnd ();
                                                                    parser.GetData (&strValue, iLength);};
  };

//-----------------------------------------------------------------------------
class ValueElemBlob : public ValueElem
  {
  private:
    RStr       strValue;

  public:

                  ValueElemBlob  ()                     {SetType ("BLOB"); strValue.Empty ();};

                  ~ValueElemBlob ()                     {};

    const void *  GetBlob        (VOID)                 override {return ((PVOID) strValue.AsChar ());};

    VOID          SetBlob        (const RStr *  strIn)  override {strValue.Set (*strIn); CallOnChanged (this, FALSE);};

    VOID          GetBlob        (RStr &  strOut)       override {strOut.AppendString (strValue);};

    ValueElem *   Clone          (VOID)                 override {ValueElemBlob * pelemNew = new ValueElemBlob; pelemNew->CloneValues (*this); pelemNew->strValue = strValue; return pelemNew;};

    VOID          Clear          (VOID)                 override {strValue.Empty (); CallOnChanged (this, FALSE);};

    VOID          ToParser       (RStrParser &  parser) override {parser.SetU4_BEnd (strValue.Length ()),
                                                                  parser.SetData    (strValue.AsUChar (), strValue.Length ());};

    VOID          FromParser     (RStrParser &  parser) override {strValue.Empty ();
                                                                  INT  iLength = parser.GetU4_BEnd ();
                                                                  parser.GetData (&strValue, iLength);};
  };

//-----------------------------------------------------------------------------
class ValueElemStringArray : public ValueElem
  {
  private:
    RStrArray       arrayValue;

  public:

                  ValueElemStringArray  ()                {SetType ("ASTR"); arrayValue.Clear ();};

                  ~ValueElemStringArray ()                {};

    const char *  GetString           (VOID) const                      override {return ((arrayValue.Length () > 0) ? arrayValue[0].AsChar() : "");};

    VOID          SetString           (const char *  szIn,
                                       BOOL          bUpdating = FALSE) override {arrayValue.Append (szIn); CallOnChanged (this, FALSE);};

    VOID          GetString           (RStr &  strOut) const        override {strOut.AppendString (GetString());};

    INT           GetArrayLength      (VOID)                        override {return (arrayValue.Length ());};

    VOID          DeleteArrayElem     (INT  iIndexIn)               override {arrayValue.Remove (iIndexIn, 1);};

    VOID          ClearArray          (VOID)                        override {arrayValue.Clear ();};

    const char *  GetStringInArray    (INT  iIndexIn)               override {if (iIndexIn < 0)  {iIndexIn += arrayValue.Length();}  return (arrayValue[iIndexIn].AsChar());};

    VOID          SetStringInArray    (const char *  szIn,
                                       INT           iIndexIn = -1) override;

    VOID          DeleteStringInArray (const char *  szIn)          override {INT iSearch = -1; while ((iSearch = arrayValue.Find (szIn)) != -1) {arrayValue.Remove (iSearch, 1);};};

    VOID          SetStringArray      (RStrArray &  astrIn)         override;

    RStrArray *   GetStringArray      (VOID)                        override {return &arrayValue;};

    ValueElem *   Clone               (VOID)                        override {
                                                                             ValueElemStringArray * pelemNew = new ValueElemStringArray;
                                                                             pelemNew->CloneValues (*this);
                                                                             pelemNew->arrayValue = arrayValue;
                                                                             return pelemNew;
                                                                             };

    VOID          Clear            (VOID)                           override {arrayValue.Clear (); CallOnChanged (this, FALSE);};

    VOID          ToParser         (RStrParser &  parser)           override;

    VOID          FromParser       (RStrParser &  parser)           override;
  };

//-----------------------------------------------------------------------------
class ValueElemIntArray : public ValueElem
  {
  private:
    IntArray       arrayValue;

  public:

                  ValueElemIntArray  ()                     {SetType ("AINT"); arrayValue.Clear ();};

                  ~ValueElemIntArray ()                     {};

    INT           GetInt             (VOID)                 override {return ((arrayValue.Length () > 0) ? arrayValue[0] : 0);};

    VOID          SetInt             (INT  iIn,
                                      BOOL bUpdating = FALSE) override {arrayValue.Append (iIn); CallOnChanged (this, FALSE);};

    INT           GetArrayLength     (VOID)                 override {return (arrayValue.Length ());};

    VOID          DeleteArrayElem    (INT  iIndexIn)        override {arrayValue.Remove (iIndexIn, 1);};

    VOID          ClearArray         (VOID)                 override {arrayValue.Clear ();};

    IntArray *    GetIntArray        (VOID)                 override {return (&arrayValue);};

    VOID          SetIntArray        (IntArray &  aiIn)     override {arrayValue = aiIn; CallOnChanged (this, FALSE);};

    ValueElem *   Clone              (VOID)                 override {
                                                                     ValueElemIntArray * pelemNew = new ValueElemIntArray;
                                                                     pelemNew->CloneValues (*this);
                                                                     pelemNew->arrayValue = arrayValue;
                                                                     return pelemNew;
                                                                     };

    VOID          Clear              (VOID)                 override {arrayValue.Clear (); CallOnChanged (this, FALSE);};

    VOID          ToParser           (RStrParser &  parser) override;

    VOID          FromParser         (RStrParser &  parser) override;
  };

//-----------------------------------------------------------------------------
class ValueElemFloatArray : public ValueElem
  {
  private:
    FloatArray       arrayValue;

  public:

                  ValueElemFloatArray  ()                   {SetType ("AFLT"); arrayValue.Clear ();};

                  ~ValueElemFloatArray ()                   {};

    FLOAT         GetFloat           (VOID)                 override {return ((arrayValue.Length () > 0) ? arrayValue[0] : 0.0f);};

    VOID          SetFloat           (FLOAT  fIn,
                                      BOOL bUpdating = FALSE) override {arrayValue.Append (fIn); CallOnChanged (this, FALSE);};

    INT           GetArrayLength      (VOID)                override {return (arrayValue.Length ());};

    VOID          DeleteArrayElem    (INT  iIndexIn)        override {arrayValue.Remove (iIndexIn, 1);};

    VOID          ClearArray         (VOID)                 override {arrayValue.Clear ();};

    FloatArray *  GetFloatArray      (VOID)                 override {return (&arrayValue);};

    VOID          SetFloatArray      (FloatArray &  afIn)   override {arrayValue = afIn; CallOnChanged (this, FALSE);};

    VOID          SetFloatArray      (FLOAT *       pafIn)  override {arrayValue.Set (pafIn, 16); CallOnChanged (this, FALSE);}; /// Assumes 4x4 matrix

    ValueElem *   Clone              (VOID)                 override {
                                                                     ValueElemFloatArray * pelemNew = new ValueElemFloatArray;
                                                                     pelemNew->CloneValues (*this);
                                                                     pelemNew->arrayValue = arrayValue;
                                                                     return pelemNew;
                                                                     };

    VOID          Clear              (VOID)                 override {arrayValue.Clear (); CallOnChanged (this, FALSE);};

    VOID          ToParser           (RStrParser &  parser) override;

    VOID          FromParser         (RStrParser &  parser) override;
  };


//-----------------------------------------------------------------------------
class ValueElemDoubleArray : public ValueElem
  {
  private:
    DoubleArray           arrayValue;

  public:

                  ValueElemDoubleArray  ()                  {SetType ("ADBL"); arrayValue.Clear ();};

                  ~ValueElemDoubleArray ()                  {};

    DOUBLE        GetDouble          (VOID)                 override {return ((arrayValue.Length () > 0) ? arrayValue[0] : 0.0);};

    VOID          SetDouble          (DOUBLE dIn,
                                      BOOL   bUpdating = FALSE) override {arrayValue.Append (dIn); CallOnChanged (this, FALSE);};

    INT           GetArrayLength     (VOID)                 override {return (arrayValue.Length ());};

    VOID          DeleteArrayElem    (INT  iIndexIn)        override {arrayValue.Remove (iIndexIn, 1);};

    VOID          ClearArray         (VOID)                 override {arrayValue.Clear ();};

    DoubleArray * GetDoubleArray     (VOID)                 override {return (&arrayValue);};

    VOID          SetDoubleArray     (DoubleArray &  adIn)  override {arrayValue = adIn; CallOnChanged (this, FALSE);};

    VOID          SetDoubleArray     (DOUBLE *       padIn) override {arrayValue.Set (padIn, 16); CallOnChanged (this, FALSE);};  /// Assumes 4x4 matrix

    ValueElem *   Clone              (VOID)                 override {
                                                                     ValueElemDoubleArray * pelemNew = new ValueElemDoubleArray;
                                                                     pelemNew->CloneValues (*this);
                                                                     pelemNew->arrayValue = arrayValue;
                                                                     return pelemNew;
                                                                     };

    VOID          Clear              (VOID)                 override {arrayValue.Clear (); CallOnChanged (this, FALSE);};

    VOID          ToParser           (RStrParser &  parser) override;

    VOID          FromParser         (RStrParser &  parser) override;
  };

//-----------------------------------------------------------------------------
class ValueElemPtr : public ValueElem
  {
  private:
    PVOID         pvValue;

  public:

                  ValueElemPtr  ()              {SetType ("PTR_"); pvValue = NULL;};

                  ~ValueElemPtr ()              {};

    PVOID         GetPtr        (VOID)            override {return pvValue;};

    VOID          SetPtr        (PVOID  pvIn)     override {pvValue = pvIn; CallOnChanged (this, FALSE);};

    ValueElem *   Clone         (VOID)            override {ValueElemPtr * pelemNew = new ValueElemPtr; pelemNew->CloneValues (*this); pelemNew->pvValue = pvValue; return pelemNew;};

    VOID          Clear         (VOID)            override {pvValue = NULL; CallOnChanged (this, FALSE);};

    VOID          MarkForSave   (BOOL  bStatusIn) override {return;}; // cannot save/restore pointer types

  };

//-----------------------------------------------------------------------------
class ValueElemLink : public ValueElem
  {
  private:
    HASH_T        uValue;

  public:

                  ValueElemLink  ()             {SetType ("LINK"); uValue = 0;};

                  ~ValueElemLink ()             {};

    HASH_T        GetLink       (VOID)            override {return uValue;};

    VOID          SetLink       (HASH_T  uIn)     override {uValue = uIn; CallOnChanged (this, FALSE);};

    ValueElem *   Clone         (VOID)            override {ValueElemLink * pelemNew = new ValueElemLink; pelemNew->CloneValues (*this); pelemNew->uValue = uValue; return pelemNew;};

    VOID          Clear         (VOID)            override {uValue = 0; CallOnChanged (this, FALSE);};

    VOID          MarkForSave   (BOOL  bStatusIn) override {return;}; // cannot save/restore pointer types

  };

//-----------------------------------------------------------------------------
class ValueRegistrySimple : public ValueRegistry
  {
  private:

    // NOTE:  This would be much faster lookup with a hash map or a balanced
    //         RBTree.
    TList<ValueElem*>  listValues;

  protected:

  public:

                  ValueRegistrySimple  ();

                  ~ValueRegistrySimple ();

    ValueElem *   NewInt               (const char *  szNameIn);
    ValueElem *   NewInt               (HASH_T        uNameHashIn);

    INT           GetInt               (const char *  szNameIn)    const override;
    INT           GetInt               (HASH_T        uNameHashIn) const override;

    ValueElem *   SetInt               (const char *  szNameIn,
                                        INT           iIn,
                                        BOOL          bUpdating = FALSE) override;
    ValueElem *   SetInt               (HASH_T        uNameHashIn,
                                        INT           iIn,
                                        BOOL          bUpdating = FALSE) override;

    ValueElem *   NewFloat             (const char *  szNameIn);
    ValueElem *   NewFloat             (HASH_T        uNameHashIn);

    FLOAT         GetFloat             (const char *  szNameIn)    const override;
    FLOAT         GetFloat             (HASH_T        uNameHashIn) const override;

    ValueElem *   SetFloat             (const char *  szNameIn,
                                        FLOAT         fIn,
                                        BOOL          bUpdating = FALSE) override;
    ValueElem *   SetFloat             (HASH_T        uNameHashIn,
                                        FLOAT         fIn,
                                        BOOL          bUpdating = FALSE) override;

    ValueElem *   NewDouble            (const char *  szNameIn);
    ValueElem *   NewDouble            (HASH_T        uNameHashIn);

    DOUBLE        GetDouble            (const char *  szNameIn)    const override;
    DOUBLE        GetDouble            (HASH_T        uNameHashIn) const override;

    ValueElem *   SetDouble            (const char *  szNameIn,
                                        DOUBLE        dIn,
                                        BOOL          bUpdating = FALSE) override;
    ValueElem *   SetDouble            (HASH_T        uNameHashIn,
                                        DOUBLE        dIn,
                                        BOOL          bUpdating = FALSE) override;

    ValueElem *   NewVec               (const char *  szNameIn);
    ValueElem *   NewVec               (HASH_T        uNameHashIn);

    RVec4 &       GetVec               (const char *  szNameIn)    const override;
    RVec4 &       GetVec               (HASH_T        uNameHashIn) const override;

    ValueElem *   SetVec               (const char *  szNameIn,
                                        RVec4 &       vecIn,
                                        BOOL          bUpdating = FALSE) override;
    ValueElem *   SetVec               (HASH_T        uNameHashIn,
                                        RVec4 &       vecIn,
                                        BOOL          bUpdating = FALSE) override;

    ValueElem *   NewBool              (const char *  szNameIn);
    ValueElem *   NewBool              (HASH_T        uNameHashIn);

    BOOL          GetBool              (const char *  szNameIn)    const override;
    BOOL          GetBool              (HASH_T        uNameHashIn) const override;

    ValueElem *   SetBool              (const char *  szNameIn,
                                        BOOL          bIn,
                                        BOOL          bUpdating = FALSE) override;
    ValueElem *   SetBool              (HASH_T        uNameHashIn,
                                        BOOL          bIn,
                                        BOOL          bUpdating = FALSE) override;

    ValueElem *   NewString            (const char *  szNameIn);
    ValueElem *   NewString            (HASH_T        uNameHashIn);

    const char *  GetString            (const char *  szNameIn)    const override;
    const char *  GetString            (HASH_T        uNameHashIn) const override;

    VOID          GetString            (const char *  szNameIn,
                                        RStr &        strOut)      const override;
    VOID          GetString            (HASH_T        uNameHashIn,
                                        RStr &        strOut)      const override;

    ValueElem *   SetString            (const char *  szNameIn,
                                        const char *  szIn = NULL,
                                        BOOL          bUpdating = FALSE) override;
    ValueElem *   SetString            (HASH_T        uNameHashIn,
                                        const char *  szIn = NULL,
                                        BOOL          bUpdating = FALSE) override;

    ValueElem *   NewBlob              (const char *  szNameIn);
    ValueElem *   NewBlob              (HASH_T        uNameHashIn);

    const void *  GetBlob              (const char *  szNameIn)    const override;
    const void *  GetBlob              (HASH_T        uNameHashIn) const override;

    VOID          GetBlob              (const char *  szNameIn,
                                        RStr &        strOut)      const override;
    VOID          GetBlob              (HASH_T        uNameHashIn,
                                        RStr &        strOut)      const override;

    ValueElem *   SetBlob              (const char *  szNameIn,
                                        const RStr *  strIn)       override;
    ValueElem *   SetBlob              (HASH_T        uNameHashIn,
                                        const RStr *  strIn)       override;

    INT           GetArrayLength       (const char *  szNameIn)    const override;
    INT           GetArrayLength       (HASH_T        uNameHashIn) const override;

    VOID          DeleteArrayElem      (const char *  szNameIn,
                                        INT           iIndexIn = -1) override;
    VOID          DeleteArrayElem      (HASH_T        uNameHashIn,
                                        INT           iIndexIn = -1) override;

    VOID          ClearArray           (const char *  szNameIn)    override;
    VOID          ClearArray           (HASH_T        uNameHashIn) override;

    ValueElem *   NewStringArray       (const char *  szNameIn);
    ValueElem *   NewStringArray       (HASH_T        uNameHashIn);

    const char *  GetStringInArray     (const char *  szNameIn,
                                        INT           iIndexIn)   const override;
    const char *  GetStringInArray     (HASH_T        uNameHashIn,
                                        INT           iIndexIn)   const override;

    RStrArray *   GetStringArray       (const char *  szNameIn)    const override;
    RStrArray *   GetStringArray       (HASH_T        uNameHashIn) const override;

    ValueElem *   SetStringInArray     (const char *  szNameIn,
                                        const char *  szIn = NULL,
                                        INT           iIndexIn = -1) override;
    ValueElem *   SetStringInArray     (HASH_T        uNameHashIn,
                                        const char *  szIn = NULL,
                                        INT           iIndexIn = -1) override;

    ValueElem *   SetStringArray       (const char *  szNameIn,
                                        RStrArray &   astrIn)     override;
    ValueElem *   SetStringArray       (HASH_T        uNameHashIn,
                                        RStrArray &   astrIn)     override;

    ValueElem *   DeleteStringInArray  (const char *  szNameIn,
                                        const char *  szIn = NULL) override;
    ValueElem *   DeleteStringInArray  (HASH_T        uNameHashIn,
                                        const char *  szIn = NULL) override;

    ValueElem *   NewIntArray          (const char *  szNameIn);
    ValueElem *   NewIntArray          (HASH_T        uNameHashIn);

    IntArray *    GetIntArray          (const char *  szNameIn)    const override;
    IntArray *    GetIntArray          (HASH_T        uNameHashIn) const override;

    ValueElem *   SetIntArray          (const char *  szNameIn,
                                        IntArray &    aiIn)        override;
    ValueElem *   SetIntArray          (HASH_T        uNameHashIn,
                                        IntArray &    aiIn)        override;

    ValueElem *   NewFloatArray        (const char *  szNameIn);
    ValueElem *   NewFloatArray        (HASH_T        uNameHashIn);

    FloatArray *  GetFloatArray        (const char *  szNameIn)    const override;
    FloatArray *  GetFloatArray        (HASH_T        uNameHashIn) const override;

    ValueElem *   SetFloatArray        (const char *  szNameIn,
                                        FloatArray &  afIn)       override;
    ValueElem *   SetFloatArray        (HASH_T        uNameHashIn,
                                        FloatArray &  afIn)       override;

    ValueElem *   SetFloatArray        (const char *  szNameIn,
                                        FLOAT *       pafIn)      override;
    ValueElem *   SetFloatArray        (HASH_T        uNameHashIn,
                                        FLOAT *       pafIn)      override;

    ValueElem *   NewDoubleArray       (const char *  szNameIn);
    ValueElem *   NewDoubleArray       (HASH_T        uNameHashIn);

    DoubleArray * GetDoubleArray       (const char *  szNameIn)    const override;
    DoubleArray * GetDoubleArray       (HASH_T        uNameHashIn) const override;

    ValueElem *   SetDoubleArray       (const char *   szNameIn,
                                        DoubleArray &  afIn)       override;
    ValueElem *   SetDoubleArray       (HASH_T         uNameHashIn,
                                        DoubleArray &  afIn)       override;

    ValueElem *   SetDoubleArray       (const char *  szNameIn,
                                        DOUBLE *      pdfIn)      override;
    ValueElem *   SetDoubleArray       (HASH_T        uNameHashIn,
                                        DOUBLE *      pdfIn)      override;

    ValueElem *   NewPtr               (const char *  szNameIn);
    ValueElem *   NewPtr               (HASH_T        uNameHashIn);

    PVOID         GetPtr               (const char *  szNameIn)    const override;
    PVOID         GetPtr               (HASH_T        uNameHashIn) const override;

    ValueElem *   SetPtr               (const char *  szNameIn,
                                        PVOID         pvIn)       override;
    ValueElem *   SetPtr               (HASH_T        uNameHashIn,
                                        PVOID         pvIn)       override;

    ValueElem *   NewLink              (const char *  szNameIn);
    ValueElem *   NewLink              (HASH_T        uNameHashIn);

    HASH_T        GetLink              (const char *  szNameIn)    const override;
    HASH_T        GetLink              (HASH_T        uNameHashIn) const override;

    ValueElem *   SetLink              (const char *  szNameIn,
                                        HASH_T        uIn)        override;
    ValueElem *   SetLink              (HASH_T        uNameHashIn,
                                        HASH_T        uIn)        override;

    ValueElem *   SetClone             (ValueElem *   pelemIn) override;

    BOOL          HasKey               (const char *  szNameIn)    const override;
    BOOL          HasKey               (HASH_T        uNameHashIn) const override;

    BOOL          IsType               (const char *  szNameIn,
                                        UINT32        uTypeIn) const;
    BOOL          IsType               (HASH_T        uNameHashIn,
                                        UINT32        uTypeIn) const;

    BOOL          IsInt                (const char *  szNameIn)    const override;
    BOOL          IsInt                (HASH_T        uNameHashIn) const override;

    BOOL          IsFloat              (const char *  szNameIn)    const override;
    BOOL          IsFloat              (HASH_T        uNameHashIn) const override;

    BOOL          IsDouble             (const char *  szNameIn)    const override;
    BOOL          IsDouble             (HASH_T        uNameHashIn) const override;

    BOOL          IsVec                (const char *  szNameIn)    const override;
    BOOL          IsVec                (HASH_T        uNameHashIn) const override;

    BOOL          IsBool               (const char *  szNameIn)    const override;
    BOOL          IsBool               (HASH_T        uNameHashIn) const override;

    BOOL          IsString             (const char *  szNameIn)    const override;
    BOOL          IsString             (HASH_T        uNameHashIn) const override;

    BOOL          IsPtr                (const char *  szNameIn)    const override;
    BOOL          IsPtr                (HASH_T        uNameHashIn) const override;

    BOOL          IsLink               (const char *  szNameIn)    const override;
    BOOL          IsLink               (HASH_T        uNameHashIn) const override;

    VOID          Clear                (VOID)                      override; // clear/reset all values
    VOID          ClearLocal           (VOID);                               // clear/reset all values

    VOID          ClearElem            (const char *  szNameIn)    override; // clear specific value
    VOID          ClearElem            (HASH_T        uNameHashIn) override; // clear specific value

    VOID          DeleteElem           (ValueElem *   pElemIn)     override;
    VOID          DeleteElem           (const char *  szNameIn);    // delete specific value
    VOID          DeleteElem           (HASH_T        uNameHashIn); // delete specific value

    ValueRegistry &  operator=         (const ValueRegistry &  regIn) override;

    INT           Size                 (VOID) const override;

    VOID          SetOrder             (const char *  szNameIn,
                                        INT           iOrderIn)    override;

    VOID          SetOrder             (HASH_T        uNameHashIn,
                                        INT           iOrderIn)    override;

    ValueElem *   Find                 (const char *  szNameIn) const override;

    ValueElem *   Find                 (HASH_T        uNameHashIn) const override;

    ValueElem *   FindByOrder          (INT  iOrderIn) const override;

    ValueElem *   FindByIndex          (INT  iIndexIn) const override;

    VOID          DebugPrint           (const char *  szFilter = NULL) override;

    VOID          ToParser             (RStrParser &  parser) override;

    VOID          FromParser           (RStrParser &  parser) override;

    INT           LinkCount            (HASH_T        uListRootHashIn, // RootHash is the registry entry that contains the hash to base name of the first element in the list
                                        const char *  pszNextTagIn);

    VOID          Unlink               (HASH_T        uListRootHashIn,
                                        const char *  pszNextTagIn);

    VOID          LinkPushFront        (HASH_T        uListRootHashIn,
                                        const char *  pszNextTagIn,
                                        HASH_T        uLinkToStoreIn);

    VOID          LinkPushBack         (HASH_T        uListRootHashIn,
                                        const char *  pszNextTagIn,
                                        HASH_T        uLinkToStoreIn);

    HASH_T        LinkGetLast          (HASH_T        uFirstElemIn,
                                        const char *  pszNextTagIn);

    VOID          SwapIndexes          (ValueElem &  elemOne,
                                        ValueElem &  elemTwo) override;

  };

#endif // VALUEREGISTRYSIMPLE_HPP
