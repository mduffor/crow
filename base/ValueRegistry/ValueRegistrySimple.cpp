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

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "ValueRegistry/ValueRegistrySimple.hpp"


#define USE_NAME_STRING

//-----------------------------------------------------------------------------
VOID  ValueElemStringArray::ToParser  (RStrParser &  parser)
  {
  INT  iNumElem = arrayValue.Length ();

  parser.SetU4_BEnd (iNumElem);
  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    parser.SetU4_BEnd (arrayValue[iIndex].Length ()),
    parser.SetData    (arrayValue[iIndex].AsUChar (), arrayValue[iIndex].Length ());
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueElemStringArray::FromParser  (RStrParser &  parser)
  {
  arrayValue.Clear ();
  INT  iNumElem = parser.GetU4_BEnd ();

  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    RStr  strNew;

    INT  iLength = parser.GetU4_BEnd ();
    parser.GetData (&strNew, iLength);
    arrayValue.Append (strNew);
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueElemStringArray::SetStringArray (RStrArray &  astrIn)
  {
  if (IsUniqueSet ())
    {
    INT  iCount = astrIn.Length();
    for (INT  iIndex = 0; iIndex < iCount; ++iIndex)
      {
      arrayValue.AppendUnique (astrIn[iIndex]);
      };
    }
  else
    {
    arrayValue = astrIn;
    };
  CallOnChanged (this, FALSE);
  };

//-----------------------------------------------------------------------------
VOID  ValueElemStringArray::SetStringInArray (const char *  szIn,
                                              INT           iIndexIn)
  {
  if (iIndexIn < 0)
    {
    if ((! IsUniqueSet ()) || (arrayValue.Find (szIn) == -1))
      {
      arrayValue.Append (szIn);
      }
    }
  else
    {
    arrayValue.SetAt(iIndexIn, szIn);
    };
  CallOnChanged (this, FALSE);
  };

//-----------------------------------------------------------------------------
VOID  ValueElemIntArray::ToParser  (RStrParser &  parser)
  {
  INT  iNumElem = arrayValue.Length ();

  parser.SetU4_BEnd (iNumElem);
  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    parser.SetU4_BEnd (arrayValue[iIndex]);
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueElemIntArray::FromParser  (RStrParser &  parser)
  {
  arrayValue.Clear ();
  INT  iNumElem = parser.GetU4_BEnd ();

  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    arrayValue.Append (parser.GetU4_BEnd ());
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueElemFloatArray::ToParser  (RStrParser &  parser)
  {
  INT  iNumElem = arrayValue.Length ();

  parser.SetU4_BEnd (iNumElem);
  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    parser.SetF4_BEnd (arrayValue[iIndex]);
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueElemFloatArray::FromParser  (RStrParser &  parser)
  {
  arrayValue.Clear ();
  INT  iNumElem = parser.GetU4_BEnd ();

  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    arrayValue.Append (parser.GetF4_BEnd ());
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueElemDoubleArray::ToParser  (RStrParser &  parser)
  {
  INT  iNumElem = arrayValue.Length ();

  parser.SetU4_BEnd (iNumElem);
  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    parser.SetF8_BEnd (arrayValue[iIndex]);
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueElemDoubleArray::FromParser  (RStrParser &  parser)
  {
  arrayValue.Clear ();
  INT  iNumElem = parser.GetU4_BEnd ();

  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    arrayValue.Append (parser.GetF8_BEnd ());
    };
  };


//-----------------------------------------------------------------------------
//  ValueRegistrySimple
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ValueRegistrySimple::ValueRegistrySimple ()
  {
  };

//-----------------------------------------------------------------------------
ValueRegistrySimple::~ValueRegistrySimple ()
  {
  ClearLocal ();
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::Find  (const char *  szNameIn) const
  {
  RStr   strNameIn (szNameIn);
  strNameIn.CalcHash ();

  for (TListItr<ValueElem*>  itrCurr = listValues.First(); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->MatchesName (strNameIn))
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::Find  (HASH_T  uNameHashIn) const
  {
  for (TListItr<ValueElem*>  itrCurr = listValues.First(); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->MatchesNameHash (uNameHashIn))
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::FindByOrder  (INT  iOrderIn) const
  {
  for (TListItr<ValueElem*>  itrCurr = listValues.First(); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->GetOrder () == iOrderIn)
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
INT  ValueRegistrySimple::Size (VOID) const
  {
  return (listValues.Size ());
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::FindByIndex (INT  iIndexIn) const
  {
  TListItr<ValueElem*>  itrCurr = listValues.AtIndex (iIndexIn);
  return  (itrCurr.IsValid () ? (*itrCurr) : NULL);
  };

//-----------------------------------------------------------------------------
INT  ValueRegistrySimple::GetInt  (const char *  szNameIn) const
  {
  return (GetInt (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
INT  ValueRegistrySimple::GetInt  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->GetInt ();
    }
  if (pFallback != NULL) {return (pFallback->GetInt (uNameHashIn));};
  return (0);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetInt  (const char *  szNameIn,
                                           INT           iIn,
                                           BOOL          bUpdating)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewInt  (szNameIn);
    pSearch->SetInt (iIn, bUpdating);
    return (pSearch);
  #else
    return (SetInt (CalcHashValue (szNameIn), iIn, bUpdating));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetInt  (HASH_T        uNameHashIn,
                                           INT           iIn,
                                           BOOL          bUpdating)
  {
  ValueElem *  pSearch = NewInt  (uNameHashIn);
  pSearch->SetInt (iIn, bUpdating);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
FLOAT  ValueRegistrySimple::GetFloat  (const char *  szNameIn) const
  {
  return (GetFloat (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
FLOAT  ValueRegistrySimple::GetFloat  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->GetFloat ();
    }
  if (pFallback != NULL) {return (pFallback->GetFloat (uNameHashIn));};
  return (0.0f);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetFloat  (const char *  szNameIn,
                                             FLOAT         fIn,
                                             BOOL          bUpdating)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewFloat (szNameIn);
    pSearch->SetFloat (fIn, bUpdating);
    return (pSearch);
  #else
    return (SetFloat (CalcHashValue (szNameIn), fIn, bUpdating));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetFloat  (HASH_T        uNameHashIn,
                                             FLOAT         fIn,
                                             BOOL          bUpdating)
  {
  ValueElem *  pSearch = NewFloat (uNameHashIn);
  pSearch->SetFloat (fIn, bUpdating);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
DOUBLE  ValueRegistrySimple::GetDouble  (const char *  szNameIn) const
  {
  return (GetDouble (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
DOUBLE  ValueRegistrySimple::GetDouble  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->GetDouble ();
    }
  if (pFallback != NULL) {return (pFallback->GetDouble (uNameHashIn));};
  return (0.0);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetDouble  (const char *  szNameIn,
                                              DOUBLE        dIn,
                                              BOOL          bUpdating)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewDouble (szNameIn);
    pSearch->SetDouble (dIn, bUpdating);
    return (pSearch);
  #else
    return (SetDouble (CalcHashValue (szNameIn), dIn, bUpdating));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetDouble  (HASH_T        uNameHashIn,
                                              DOUBLE        dIn,
                                              BOOL          bUpdating)
  {
  ValueElem *  pSearch = NewDouble (uNameHashIn);
  pSearch->SetDouble (dIn, bUpdating);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
RVec4 &  ValueRegistrySimple::GetVec  (const char *  szNameIn) const
  {
  return (GetVec (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
RVec4 &  ValueRegistrySimple::GetVec  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->GetVec ();
    }
  if (pFallback != NULL) {return (pFallback->GetVec (uNameHashIn));};
  return (RVec4::kZero);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetVec  (const char *  szNameIn,
                                           RVec4 &       vecIn,
                                           BOOL          bUpdating)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewVec (szNameIn);
    pSearch->SetVec (vecIn, bUpdating);
    return (pSearch);
  #else
    return (SetVec (CalcHashValue (szNameIn), vecIn, bUpdating));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetVec  (HASH_T        uNameHashIn,
                                           RVec4 &       vecIn,
                                           BOOL          bUpdating)
  {
  ValueElem *  pSearch = NewVec (uNameHashIn);
  pSearch->SetVec (vecIn, bUpdating);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::GetBool  (const char *  szNameIn) const
  {
  return (GetBool (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::GetBool  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->GetBool ();
    }
  if (pFallback != NULL) {return (pFallback->GetBool (uNameHashIn));};
  return (FALSE);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetBool  (const char *  szNameIn,
                                            BOOL          bIn,
                                            BOOL          bUpdating)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewBool (szNameIn);
    pSearch->SetBool (bIn, bUpdating);
    return (pSearch);
  #else
    return (SetBool (CalcHashValue (szNameIn), bIn, bUpdating));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetBool  (HASH_T        uNameHashIn,
                                            BOOL          bIn,
                                            BOOL          bUpdating)
  {
  ValueElem *  pSearch = NewBool (uNameHashIn);
  pSearch->SetBool (bIn, bUpdating);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
const char *  ValueRegistrySimple::GetString  (const char *  szNameIn) const
  {
  return (GetString (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
const char *  ValueRegistrySimple::GetString  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    //DBG_INFO("GetString for %s of type %c%c%c%c", szNameIn, (pSearch->GetType () >> 0) &0x00ff, (pSearch->GetType () >> 8) &0x00ff,  (pSearch->GetType () >> 16) &0x00ff,  (pSearch->GetType () >> 24) &0x00ff );
    return (pSearch)->GetString ();
    };
  if (pFallback != NULL) {return (pFallback->GetString (uNameHashIn));};
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::GetString  (const char *  szNameIn,
                                       RStr &        strOut) const
  {
  GetString (CalcHashValue (szNameIn), strOut);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::GetString  (HASH_T        uNameHashIn,
                                       RStr &        strOut) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    (pSearch)->GetString (strOut);
    };
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetString  (const char *  szNameIn,
                                              const char *  szIn,
                                              BOOL          bUpdating)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewString (szNameIn);
    if (szIn != NULL)
      {
      //DBG_INFO ("ValueRegistrySimple::SetString  (%s, %s)",  szNameIn, szIn);
      pSearch->SetString (szIn, bUpdating);
      };
    return (pSearch);
  #else
    return (SetString (CalcHashValue (szNameIn), szIn, bUpdating));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetString  (HASH_T        uNameHashIn,
                                              const char *  szIn,
                                              BOOL          bUpdating)
  {
  ValueElem *  pSearch = NewString (uNameHashIn);
  if (szIn != NULL)
    {
    //DBG_INFO ("ValueRegistrySimple::SetString  (%s, %s)",  szNameIn, szIn);
    pSearch->SetString (szIn, bUpdating);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
const void *  ValueRegistrySimple::GetBlob  (const char *  szNameIn) const
  {
  return (GetBlob (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
const void *  ValueRegistrySimple::GetBlob  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->GetBlob ();
    };
  if (pFallback != NULL) {return (pFallback->GetBlob (uNameHashIn));};
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::GetBlob  (const char *  szNameIn,
                                     RStr &        strOut) const
  {
  return (GetBlob (CalcHashValue (szNameIn), strOut));
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::GetBlob  (HASH_T        uNameHashIn,
                                     RStr &        strOut) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    (pSearch)->GetBlob (strOut);
    return;
    };
  if (pFallback != NULL) {return (pFallback->GetBlob (uNameHashIn, strOut));};
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetBlob  (const char *  szNameIn,
                                            const RStr *  strIn)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewBlob (szNameIn);
    if (strIn != NULL)
      {
      pSearch->SetBlob (strIn);
      };
    return (pSearch);
  #else
    return (SetBlob (CalcHashValue (szNameIn), strIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetBlob  (HASH_T        uNameHashIn,
                                            const RStr *  strIn)
  {
  ValueElem *  pSearch = NewBlob (uNameHashIn);
  if (strIn != NULL)
    {
    pSearch->SetBlob (strIn);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
INT  ValueRegistrySimple::GetArrayLength (const char *  szNameIn) const
  {
  return (GetArrayLength (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
INT  ValueRegistrySimple::GetArrayLength (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return ((pSearch)->GetArrayLength ());
    };
  if (pFallback != NULL) {return (pFallback->GetArrayLength (uNameHashIn));};
  return (0);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::DeleteArrayElem (const char *  szNameIn,
                                            INT           iIndexIn)
  {
  DeleteArrayElem (CalcHashValue (szNameIn), iIndexIn);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::DeleteArrayElem (HASH_T        uNameHashIn,
                                            INT           iIndexIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    (pSearch)->DeleteArrayElem (iIndexIn >= 0 ? iIndexIn : (pSearch)->GetArrayLength () - 1);
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::ClearArray (const char *  szNameIn)
  {
  ClearArray (CalcHashValue (szNameIn));
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::ClearArray (HASH_T        uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    (pSearch)->ClearArray ();
    };
  };

//-----------------------------------------------------------------------------
const char *  ValueRegistrySimple::GetStringInArray  (const char *  szNameIn,
                                                      INT           iIndexIn) const
  {
  return (GetStringInArray (CalcHashValue (szNameIn), iIndexIn));
  };

//-----------------------------------------------------------------------------
const char *  ValueRegistrySimple::GetStringInArray  (HASH_T        uNameHashIn,
                                                      INT           iIndexIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    //DBG_INFO ("ValueRegistrySimple::GetArrayString %d", iIndexIn);
    return ((pSearch)->GetStringInArray (iIndexIn));
    };
  if (pFallback != NULL) {return (pFallback->GetStringInArray (uNameHashIn, iIndexIn));};
  return ("");
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetStringInArray  (const char *  szNameIn,
                                                     const char *  szIn,
                                                     INT           iIndexIn)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewStringArray  (szNameIn);
    if (szIn != NULL)
      {
      pSearch->SetStringInArray (szIn, iIndexIn);
      };
    return (pSearch);
  #else
    return (SetStringInArray (CalcHashValue (szNameIn), szIn, iIndexIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetStringInArray  (HASH_T        uNameHashIn,
                                                   const char *  szIn,
                                                   INT           iIndexIn)
  {
  ValueElem *  pSearch = NewStringArray  (uNameHashIn);
  if (szIn != NULL)
    {
    pSearch->SetStringInArray (szIn, iIndexIn);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::DeleteStringInArray  (const char *  szNameIn,
                                                        const char *  szIn)
  {
  return (DeleteStringInArray (CalcHashValue (szNameIn), szIn));
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::DeleteStringInArray  (HASH_T        uNameHashIn,
                                                        const char *  szIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if ((szIn != NULL) && (pSearch != NULL))
    {
    pSearch->DeleteStringInArray (szIn);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetStringArray (const char *  szNameIn,
                                                  RStrArray &   astrIn)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewStringArray (szNameIn);
    pSearch->SetStringArray (astrIn);
    return (pSearch);
  #else
    return (SetStringArray (CalcHashValue (szNameIn), astrIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetStringArray  (HASH_T        uNameHashIn,
                                                   RStrArray &   astrIn)
  {
  ValueElem *  pSearch = NewStringArray (uNameHashIn);
  pSearch->SetStringArray (astrIn);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
RStrArray *  ValueRegistrySimple::GetStringArray  (const char *  szNameIn) const
  {
  return (GetStringArray (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
RStrArray *  ValueRegistrySimple::GetStringArray  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch->GetStringArray ());
    };
  if (pFallback != NULL) {return (pFallback->GetStringArray (uNameHashIn));};
  return (NULL);
  };

//-----------------------------------------------------------------------------
IntArray *  ValueRegistrySimple::GetIntArray  (const char *  szNameIn) const
  {
  return (GetIntArray (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
IntArray *  ValueRegistrySimple::GetIntArray  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch->GetIntArray ());
    };
  if (pFallback != NULL) {return (pFallback->GetIntArray (uNameHashIn));};
  return (NULL);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetIntArray  (const char *  szNameIn,
                                                IntArray &    aiIn)
  {
  #if defined (DEBUG) || defined (USE_NAME_STRING)
    ValueElem *  pSearch = NewIntArray (szNameIn);
    pSearch->SetIntArray (aiIn);
    return (pSearch);
  #else
    return (SetIntArray (CalcHashValue (szNameIn), aiIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetIntArray  (HASH_T        uNameHashIn,
                                                IntArray &    aiIn)
  {
  ValueElem *  pSearch = NewIntArray (uNameHashIn);
  pSearch->SetIntArray (aiIn);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
FloatArray *  ValueRegistrySimple::GetFloatArray  (const char *  szNameIn) const
  {
  return (GetFloatArray (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
FloatArray *  ValueRegistrySimple::GetFloatArray  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch->GetFloatArray ());
    };
  if (pFallback != NULL) {return (pFallback->GetFloatArray (uNameHashIn));};
  return (NULL);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetFloatArray  (const char *  szNameIn,
                                                  FloatArray &  afIn)
  {
  #ifdef DEBUG
    ValueElem *  pSearch = NewFloatArray (szNameIn);
    pSearch->SetFloatArray (afIn);
    return (pSearch);
  #else
    return (SetFloatArray (CalcHashValue (szNameIn), afIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetFloatArray  (HASH_T        uNameHashIn,
                                                  FloatArray &  afIn)
  {
  ValueElem *  pSearch = NewFloatArray (uNameHashIn);
  pSearch->SetFloatArray (afIn);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetFloatArray  (const char *  szNameIn,
                                                  FLOAT *       pafIn)
  {
  #ifdef DEBUG
    ValueElem *  pSearch = NewFloatArray (szNameIn);
    pSearch->SetFloatArray (pafIn);
    return (pSearch);
  #else
    return (SetFloatArray (CalcHashValue (szNameIn), pafIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetFloatArray  (HASH_T        uNameHashIn,
                                                  FLOAT *       pafIn)
  {
  ValueElem *  pSearch = NewFloatArray (uNameHashIn);
  pSearch->SetFloatArray (pafIn);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
DoubleArray *  ValueRegistrySimple::GetDoubleArray  (const char *  szNameIn) const
  {
  return (GetDoubleArray (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
DoubleArray *  ValueRegistrySimple::GetDoubleArray  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch->GetDoubleArray ());
    };
  if (pFallback != NULL) {return (pFallback->GetDoubleArray (uNameHashIn));};
  return (NULL);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetDoubleArray  (const char *   szNameIn,
                                                   DoubleArray &  adIn)
  {
  #ifdef DEBUG
    ValueElem *  pSearch = NewDoubleArray (szNameIn);
    pSearch->SetDoubleArray (adIn);
    return (pSearch);
  #else
    return (SetDoubleArray (CalcHashValue (szNameIn), adIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetDoubleArray  (HASH_T         uNameHashIn,
                                                   DoubleArray &  adIn)
  {
  ValueElem *  pSearch = NewDoubleArray (uNameHashIn);
  pSearch->SetDoubleArray (adIn);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetDoubleArray  (const char *  szNameIn,
                                                   DOUBLE *      padIn)
  {
  #ifdef DEBUG
    ValueElem *  pSearch = NewDoubleArray (szNameIn);
    pSearch->SetDoubleArray (padIn);
    return (pSearch);
  #else
    return (SetDoubleArray (CalcHashValue (szNameIn), padIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetDoubleArray  (HASH_T        uNameHashIn,
                                                   DOUBLE *      pafIn)
  {
  ValueElem *  pSearch = NewDoubleArray (uNameHashIn);
  pSearch->SetDoubleArray (pafIn);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
PVOID  ValueRegistrySimple::GetPtr  (const char *  szNameIn) const
  {
  return (GetPtr (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
PVOID  ValueRegistrySimple::GetPtr  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->GetPtr ();
    }
  if (pFallback != NULL) {return (pFallback->GetPtr (uNameHashIn));};
  return (NULL);
  };

//-----------------------------------------------------------------------------
ValueElem *   ValueRegistrySimple::SetPtr  (const char *  szNameIn,
                                            PVOID         pvIn)
  {
  #ifdef DEBUG
    ValueElem *  pSearch = NewPtr  (szNameIn);
    pSearch->SetPtr (pvIn);
    return (pSearch);
  #else
    return (SetPtr (CalcHashValue (szNameIn), pvIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *   ValueRegistrySimple::SetPtr  (HASH_T        uNameHashIn,
                                            PVOID         pvIn)
  {
  ValueElem *  pSearch = NewPtr  (uNameHashIn);
  pSearch->SetPtr (pvIn);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
HASH_T  ValueRegistrySimple::GetLink  (const char *  szNameIn) const
  {
  return (GetLink (CalcHashValue (szNameIn)));
  };

//-----------------------------------------------------------------------------
HASH_T  ValueRegistrySimple::GetLink  (HASH_T        uNameHashIn) const
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->GetLink ();
    }
  if (pFallback != NULL) {return (pFallback->GetLink (uNameHashIn));};
  return (0);
  };

//-----------------------------------------------------------------------------
ValueElem *   ValueRegistrySimple::SetLink  (const char *  szNameIn,
                                             HASH_T        uIn)
  {
  #ifdef DEBUG
    ValueElem *  pSearch = NewLink (szNameIn);
    pSearch->SetLink (uIn);
    return (pSearch);
  #else
    return (SetLink (CalcHashValue (szNameIn), uIn));
  #endif
  };

//-----------------------------------------------------------------------------
ValueElem *   ValueRegistrySimple::SetLink  (HASH_T        uNameHashIn,
                                             HASH_T        uIn)
  {
  ValueElem *  pSearch = NewLink (uNameHashIn);
  pSearch->SetLink (uIn);
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewInt  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemInt;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewInt  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemInt;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewFloat  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemFloat;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewFloat  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemFloat;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewDouble  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemDouble;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewDouble  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemDouble;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewVec  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemVec;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewVec  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemVec;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewBool  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemBool;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewBool  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemBool;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewString  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemString;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewString  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemString;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewBlob  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemBlob;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewBlob  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemBlob;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewStringArray  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemStringArray;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewStringArray  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemStringArray;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewIntArray  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemIntArray;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewIntArray  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemIntArray;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewFloatArray  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemFloatArray;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewFloatArray  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemFloatArray;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewDoubleArray  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemDoubleArray;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::NewDoubleArray  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemDoubleArray;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *   ValueRegistrySimple::NewPtr  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemPtr;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *   ValueRegistrySimple::NewPtr  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemPtr;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *   ValueRegistrySimple::NewLink  (const char *  szNameIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemLink;
    pSearch->SetName (szNameIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *   ValueRegistrySimple::NewLink  (HASH_T  uNameHashIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch == NULL)
    {
    pSearch = new ValueElemLink;
    pSearch->SetNameHash (uNameHashIn);
    listValues.PushBack (pSearch);
    };
  return (pSearch);
  };

//-----------------------------------------------------------------------------
ValueElem *  ValueRegistrySimple::SetClone  (ValueElem *   pelemIn)
  {
  if (pelemIn == NULL) return (NULL);

  ValueElem *  pelemNew = NULL;

  if (pelemIn->GetType () == MAKE_FOUR_CODE("INT_"))
    {
    pelemNew = SetInt (pelemIn->GetNameHash (),  pelemIn->GetInt ());
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("FLT_"))
    {
    pelemNew = SetFloat (pelemIn->GetNameHash (),  pelemIn->GetFloat ());
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("DBL_"))
    {
    pelemNew = SetDouble (pelemIn->GetNameHash (),  pelemIn->GetDouble ());
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("BOOL"))
    {
    pelemNew = SetBool (pelemIn->GetNameHash (),  pelemIn->GetBool ());
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("STR_"))
    {
    pelemNew = SetString (pelemIn->GetNameHash (),  pelemIn->GetString ());
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("PTR_"))
    {
    // NOTE:  For Pointer types, we don't copy over the pointer for safety.
    //   The caller will need to handle the pointer itself as needed.
    pelemNew = SetPtr (pelemIn->GetNameHash (),  NULL);
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("LINK"))
    {
    // NOTE:  For cloned Link types, we don't copy over the link for safety.
    //   The caller will need to handle setting up the linked list again, as needed.
    pelemNew = SetLink (pelemIn->GetNameHash (),  0);
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("ASTR"))
    {
    // The trick with cloning a string array is that first you need
    //  to force creation of the string array, and then you need to
    //  fill it in with the least amount of reallocation possible.
    //  You also don't want to search for it more often than needed.

    pelemNew = NewStringArray (pelemIn->GetNameHash ());

    pelemNew->MakeUniqueSet (pelemIn->IsUniqueSet());
    INT  iNumStrings = pelemIn->GetArrayLength ();
    if (iNumStrings > 0)
      {
      SetStringInArray  (pelemIn->GetNameHash (), pelemIn->GetStringInArray (0), 0);

      pelemNew = Find (pelemIn->GetNameHash ());

      if (pelemNew != NULL)
        {
        for (INT iIndex = iNumStrings - 1; iIndex > 0; --iIndex)
          {
          pelemNew->SetStringInArray  (pelemIn->GetStringInArray (iIndex), iIndex);
          };
        };
      };
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("AINT"))
    {
    pelemNew = SetIntArray (pelemIn->GetNameHash (),  *(pelemIn->GetIntArray ()));
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("AFLT"))
    {
    pelemNew = SetFloatArray (pelemIn->GetNameHash (),  *(pelemIn->GetFloatArray ()));
    }
  else if (pelemIn->GetType () == MAKE_FOUR_CODE("ADBL"))
    {
    pelemNew = SetDoubleArray (pelemIn->GetNameHash (),  *(pelemIn->GetDoubleArray ()));
    }

  // we created pelemNew with a hash, so we have to copy over the string
  //  name if it is present.
  if ((pelemNew != NULL) && (!pelemIn->GetNameStr ().IsEmpty ()))
    {
    pelemNew->SetName (pelemIn->GetName());
    }

  return (pelemNew);
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::HasKey  (const char *  szNameIn) const
  {
  return (Find (szNameIn) != NULL);
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::HasKey  (HASH_T  uNameHashIn) const
  {
  return (Find (uNameHashIn) != NULL);
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsType  (const char *  szNameIn,
                                    UINT32        uTypeIn) const
  {
  ValueElem *  pElem = Find (szNameIn);
  if (pElem == NULL) return (FALSE);
  return (pElem->GetType () == uTypeIn);
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsType  (HASH_T  uNameHashIn,
                                    UINT32  uTypeIn) const
  {
  ValueElem *  pElem = Find (uNameHashIn);
  if (pElem == NULL) return (FALSE);
  return (pElem->GetType () == uTypeIn);
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsInt  (const char *  szNameIn) const
  {
  return (IsType (szNameIn,MAKE_FOUR_CODE("INT_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsInt  (HASH_T  uNameHashIn) const
  {
  return (IsType (uNameHashIn,MAKE_FOUR_CODE("INT_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsFloat  (const char *  szNameIn) const
  {
  return (IsType (szNameIn,MAKE_FOUR_CODE("FLT_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsFloat  (HASH_T  uNameHashIn) const
  {
  return (IsType (uNameHashIn,MAKE_FOUR_CODE("FLT_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsDouble  (const char *  szNameIn) const
  {
  return (IsType (szNameIn,MAKE_FOUR_CODE("DBL_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsDouble  (HASH_T  uNameHashIn) const
  {
  return (IsType (uNameHashIn,MAKE_FOUR_CODE("DBL_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsVec  (const char *  szNameIn) const
  {
  return (IsType (szNameIn,MAKE_FOUR_CODE("VEC_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsVec  (HASH_T  uNameHashIn) const
  {
  return (IsType (uNameHashIn,MAKE_FOUR_CODE("VEC_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsBool  (const char *  szNameIn) const
  {
  return (IsType (szNameIn,MAKE_FOUR_CODE("BOOL")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsBool  (HASH_T  uNameHashIn) const
  {
  return (IsType (uNameHashIn,MAKE_FOUR_CODE("BOOL")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsString  (const char *  szNameIn) const
  {
  return (IsType (szNameIn,MAKE_FOUR_CODE("STR_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsString  (HASH_T  uNameHashIn) const
  {
  return (IsType (uNameHashIn,MAKE_FOUR_CODE("STR_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsPtr  (const char *  szNameIn) const
  {
  return (IsType (szNameIn,MAKE_FOUR_CODE("PTR_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsPtr  (HASH_T  uNameHashIn) const
  {
  return (IsType (uNameHashIn,MAKE_FOUR_CODE("PTR_")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsLink  (const char *  szNameIn) const
  {
  return (IsType (szNameIn,MAKE_FOUR_CODE("LINK")));
  };

//-----------------------------------------------------------------------------
BOOL  ValueRegistrySimple::IsLink  (HASH_T  uNameHashIn) const
  {
  return (IsType (uNameHashIn,MAKE_FOUR_CODE("LINK")));
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::Clear  (VOID)
  {
  ClearLocal ();
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::ClearLocal  (VOID)
  {
  for (TListItr<ValueElem*>  itrCurr = listValues.First(); itrCurr.IsValid (); ++itrCurr)
    {
    delete (*itrCurr);
    };
  listValues.Empty ();
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::ClearElem  (const char *  szNameIn)
  {
  // delete specific value

  ValueElem *  pElem = Find (szNameIn);
  if (pElem == NULL) return;
  pElem->Clear ();
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::ClearElem  (HASH_T  uNameHashIn)
  {
  // delete specific value

  ValueElem *  pElem = Find (uNameHashIn);
  if (pElem == NULL) return;
  pElem->Clear ();
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::DeleteElem  (ValueElem *   pElemIn)
  {
  listValues.Delete (pElemIn);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::DeleteElem  (const char *  szNameIn)
  {
  ValueElem *  pElem = Find (szNameIn);
  if (pElem == NULL) return;
  DeleteElem (pElem);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::DeleteElem  (HASH_T  uNameHashIn)
  {
  ValueElem *  pElem = Find (uNameHashIn);
  if (pElem == NULL) return;
  DeleteElem (pElem);
  };

//-----------------------------------------------------------------------------
ValueRegistry &  ValueRegistrySimple::operator= (const ValueRegistry &  regIn)
  {
  ValueRegistrySimple *  pregIn = (ValueRegistrySimple *) &regIn;
  Clear ();

  for (TListItr<ValueElem*>  itrCurr = pregIn->listValues.First(); itrCurr.IsValid (); ++itrCurr)
    {
    listValues.PushBack ((*itrCurr)->Clone());
    };
  return (*this);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::SetOrder  (const char *  szNameIn,
                                      INT           iOrderIn)
  {
  ValueElem *  pSearch = Find (szNameIn);
  if (pSearch != NULL)
    {
    return (pSearch)->SetOrder (iOrderIn);
    }
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::SetOrder  (HASH_T  uNameHashIn,
                                      INT     iOrderIn)
  {
  ValueElem *  pSearch = Find (uNameHashIn);
  if (pSearch != NULL)
    {
    return (pSearch)->SetOrder (iOrderIn);
    }
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::DebugPrint  (const char *  szFilter)
  {
  // REFACTOR:  This would work much better if the output were alphabatized/sorted.

  // NOTE:  This function only works in debug mode, where the ID is stored
  //  as a string and not just as a hash.

  DBG_INFO ("ValueRegistry Contents (%d entries)", listValues.Size ());

  for (TListItr<ValueElem*>  itrCurr = listValues.First(); itrCurr.IsValid (); ++itrCurr)
    {
    UINT32  uType = (*itrCurr)->GetType ();
    RStr   strOut;
    (*itrCurr)->GetString (strOut);

    BOOL    bIsArray = (*itrCurr)->IsType (ValueElem::kTypeIntArray) ||
                       (*itrCurr)->IsType (ValueElem::kTypeFloatArray) ||
                       (*itrCurr)->IsType (ValueElem::kTypeDoubleArray) ||
                       (*itrCurr)->IsType (ValueElem::kTypeStringArray);

    if (bIsArray)
      {
      strOut.Format ("Array [%d]", (*itrCurr)->GetArrayLength());
      };

    if ((szFilter == NULL) ||
        ((*itrCurr)->NameContains (szFilter)))
      {
      DBG_INFO ("%30s %c%c%c%c  %s",
                (*itrCurr)->GetName (),
                (uType >>  0) & 0x000000ff,
                (uType >>  8) & 0x000000ff,
                (uType >> 16) & 0x000000ff,
                (uType >> 24) & 0x000000ff,
                strOut.AsChar ());
      };
    };

  if (pFallback != NULL)
    {
    DBG_INFO ("Fallback Registry --- ");
    pFallback->DebugPrint (szFilter);
    };

  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::ToParser  (RStrParser &  parser)
  {
  // TODO:  Need to make a version of this that uses name hash instead of name string

  INT  iNumToSave = 0;
  TListItr<ValueElem*>  itrCurr;

  for (itrCurr = listValues.First(); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->ShouldSave ())
      {
      ++iNumToSave;
      };
    };

  parser.SetU4_BEnd (iNumToSave);
  for (itrCurr = listValues.First(); itrCurr.IsValid (); ++itrCurr)
    {
    if ((*itrCurr)->ShouldSave ())
      {
      UINT32  uType = (*itrCurr)->GetType ();

      parser.SetU4_BEnd (uType);
      parser.SetU4_BEnd ((*itrCurr)->GetNameStr().Length ());
      parser.SetData    ((*itrCurr)->GetNameStr().AsUChar (), (*itrCurr)->GetNameStr().Length ());
      (*itrCurr)->ToParser (parser);
      };
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::FromParser  (RStrParser &  parser)
  {
  INT          iNumSaved = parser.GetU4_BEnd ();
  RStr         strName;

  for (INT  iIndex = 0; iIndex < iNumSaved; ++iIndex)
    {
    UINT32 uType = parser.GetU4_BEnd ();
    ValueElem *  pNew  = NULL;

    strName.Empty ();
    INT  iNameLength = parser.GetU4_BEnd ();
    parser.GetData (&strName, iNameLength);

    if (uType == MAKE_FOUR_CODE("INT_"))
      {
      pNew = NewInt (strName.AsChar ());
      }
    else if (uType == MAKE_FOUR_CODE("FLT_"))
      {
      pNew = NewFloat (strName.AsChar ());
      }
    else if (uType == MAKE_FOUR_CODE("DBL_"))
      {
      pNew = NewDouble (strName.AsChar ());
      }
    else if (uType == MAKE_FOUR_CODE("BOOL"))
      {
      pNew = NewBool (strName.AsChar ());
      }
    else if (uType == MAKE_FOUR_CODE("STR_"))
      {
      pNew = NewString (strName.AsChar ());
      }
    else if (uType == MAKE_FOUR_CODE("PTR_"))
      {
      // can't restore pointers.  This should never be called.
      ASSERT (FALSE);
      DBG_INFO (parser.AsChar());
      continue;
      }
    else if (uType == MAKE_FOUR_CODE("ASTR"))
      {
      pNew = NewStringArray (strName.AsChar ());
      }
    else if (uType == MAKE_FOUR_CODE("AINT"))
      {
      pNew = NewIntArray (strName.AsChar ());
      }
    else if (uType == MAKE_FOUR_CODE("AFLT"))
      {
      pNew = NewFloatArray (strName.AsChar ());
      }
    else if (uType == MAKE_FOUR_CODE("ADBL"))
      {
      pNew = NewDoubleArray (strName.AsChar ());
      };

    if (pNew != NULL)
      {
      pNew->FromParser (parser);
      };
    };
  };

//-----------------------------------------------------------------------------
INT  ValueRegistrySimple::LinkCount  (HASH_T        uListRootHashIn,
                                      const char *  pszNextTagIn)
  {
  INT     iCountOut = 0;
  HASH_T  uCurr = GetLink (uListRootHashIn);
  INT     iNextTagLen = strlen (pszNextTagIn);

  while (uCurr != 0)
    {
    ++iCountOut;
    HASH_T  uNext = CalcHashValue (pszNextTagIn, iNextTagLen, uCurr);
    uCurr = GetLink (uNext);
    };

  return (iCountOut);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::Unlink  (HASH_T        uListRootHashIn,
                                    const char *  pszNextTagIn)
  {
  INT     iNextTagLen = strlen (pszNextTagIn);
  HASH_T  uCurr = GetLink (uListRootHashIn);

  SetLink (uListRootHashIn, 0); // clear root of list

  while (uCurr != 0)
    {
    HASH_T  uNext = CalcHashValue (pszNextTagIn, iNextTagLen, uCurr);
    uCurr = GetLink (uNext);
    SetLink (uNext, 0);
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::LinkPushFront  (HASH_T        uListRootHashIn,
                                           const char *  pszNextTagIn,
                                           HASH_T        uLinkToStoreIn)
  {
  INT     iNextTagLen = strlen (pszNextTagIn);

  if (uLinkToStoreIn == 0) return;

  HASH_T  uListFirst = GetLink (uListRootHashIn);
  if (uListFirst == 0)
    {
    // empty list
    SetLink (uListRootHashIn, uLinkToStoreIn);
    return;
    };

  HASH_T  uLinkToStoreEnd = LinkGetLast (uLinkToStoreIn, pszNextTagIn);

  // set new link's Next ptr to the list head
  SetLink (CalcHashValue (pszNextTagIn, iNextTagLen, uLinkToStoreEnd), uListFirst);

  // set the list head to the new link
  SetLink (uListRootHashIn, uLinkToStoreIn);
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::LinkPushBack  (HASH_T        uListRootHashIn,
                                          const char *  pszNextTagIn,
                                          HASH_T        uLinkToStoreIn)
  {
  HASH_T  uCurr = GetLink (uListRootHashIn);
  INT     iNextTagLen = strlen (pszNextTagIn);

  if (uLinkToStoreIn == 0) return;

  // if list is empty, store in root
  if (uCurr == 0)
    {
    SetLink (uListRootHashIn, uLinkToStoreIn);
    return;
    };

  HASH_T  uListEnd = LinkGetLast (uCurr, pszNextTagIn);

  SetLink (CalcHashValue (pszNextTagIn, iNextTagLen, uListEnd), uLinkToStoreIn);
  };


//-----------------------------------------------------------------------------
HASH_T ValueRegistrySimple::LinkGetLast (HASH_T        uFirstElemIn,
                                         const char *  pszNextTagIn)
  {
  HASH_T  uCurr     = uFirstElemIn;
  HASH_T  uPrevBase = 0;

  INT     iNextTagLen = strlen (pszNextTagIn);

  // find the end of the list
  while (uCurr != 0)
    {
    uPrevBase = uCurr;
    HASH_T  uNext = CalcHashValue (pszNextTagIn, iNextTagLen, uCurr);
    uCurr = GetLink (uNext);
    };

  return (uPrevBase);
  };


//-----------------------------------------------------------------------------
VOID  ValueRegistrySimple::SwapIndexes (ValueElem &  elemOne,
                                        ValueElem &  elemTwo)
  {
  listValues.Swap (&elemOne, &elemTwo);
  };

















