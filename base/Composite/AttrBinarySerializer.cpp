/* -----------------------------------------------------------------
                        Attr Binary Serializer

     This module implements a class using the Visitor pattern to
   serialize attrs into a binary blob.


   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2015, Michael T. Duffy II.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
// Redistributions of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.
//
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

#include "Composite/AttrBinarySerializer.hpp"

//-----------------------------------------------------------------------------
//  AttrBinarySerializer
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
AttrBinarySerializer::AttrBinarySerializer ()
  {
  };

//-----------------------------------------------------------------------------
AttrBinarySerializer::~AttrBinarySerializer ()
  {
  };

//-----------------------------------------------------------------------------
// serialize to and from an array of attrs.  If attr already in array, update it.


//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::LoadList (RStrParser &    parserIn,
                                         TList<Attr*> *  plistAttr)
  {
  INT32  iSavedPos = parserIn.GetCursorStart ();
  EStatus  status = EStatus::kSuccess;

  if (parserIn.GetU4_LEnd () != MAKE_FOUR_CODE("AATR"))
    {
    parserIn.SetCursorStart (iSavedPos);
    DBG_ERROR ("Load List Failed");
    return (EStatus::kFailure);
    };
  INT32  iBlockSize = parserIn.GetU4_LEnd ();
  INT32  iEndPos = iSavedPos + sizeof (INT32) + sizeof (INT32) + iBlockSize;

  while (parserIn.GetCursorStart () < iEndPos)
    {
    Attr *  pattrNew = NULL;
    if   ((LoadInt (parserIn, &pattrNew) == EStatus::kSuccess) ||
          (LoadFloat (parserIn, &pattrNew) == EStatus::kSuccess) ||
          (LoadString (parserIn, &pattrNew) == EStatus::kSuccess) ||
          (LoadStringArray (parserIn, &pattrNew) == EStatus::kSuccess) ||
          (LoadIntArray (parserIn, &pattrNew) == EStatus::kSuccess) ||
          (LoadFloatArray (parserIn, &pattrNew) == EStatus::kSuccess)
         )
      {
      // check for duplicates, and save if unique
      // REFACTOR : This search may be slow, because it searches the existing list
      //  every time an attr is added.  For short lists, this won't be a problem.  For
      //  long lists, you need to find a faster way to search the list, or at least
      //  a faster way to compare names.
      Attr *  pattrSearch = NULL;
      for (TListItr<Attr*> itrCurr = plistAttr->First ();
          itrCurr.IsValid();
          ++itrCurr)
        {
        if (streq ((*itrCurr)->Name (), pattrNew->Name ()))
          {
          // this attr is already in the list
          pattrSearch = *itrCurr;
          break;
          };
        };
      if (pattrSearch != NULL)
        {
        pattrSearch->Set (pattrNew);
        delete pattrNew;
        pattrNew = NULL;
        }
      else
        {
        plistAttr->PushBack (pattrNew);
        };
      }
    else
      {
      // unable to handle this block type.  Skip it.
      INT  iFourCode = parserIn.GetU4_LEnd ();
      INT  iSize     = parserIn.GetU4_LEnd ();

      RStr  strOut;
      strOut.Format ("Unable to process IFF block of type %c%c%c%c size %d", iFourCode >> 24, (iFourCode >> 16) & 0x00ff, (iFourCode >> 8) & 0x00ff, iFourCode & 0x00ff, iSize);
      DBG_ERROR (strOut.AsChar ());
      parserIn.SkipChars (iSize);
      status = EStatus::Failure (strOut.AsChar ());
      };
    };
  return (status);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::SaveList (RStrParser &    parserIn,
                                         TList<Attr*> *  plistAttr)
  {
  //INT32  iNumAttrs = plistAttr->Size();

  parserIn.SetU4_LEnd (MAKE_FOUR_CODE("AATR"));
  INT  iSizeLocation = parserIn.GetCursorStart ();
  parserIn.SetU4_LEnd (0);  // reserve a location for the total size

  for (TListItr<Attr*> itrCurr = plistAttr->First ();
       itrCurr.IsValid();
       ++itrCurr)
    {
    if ((*itrCurr)->IsType (AttrFloat::Identifier ()))
      {
      SaveFloat (parserIn, *((AttrFloat *) *itrCurr));
      }
    else if ((*itrCurr)->IsType (AttrInt::Identifier ()))
      {
      SaveInt (parserIn, *((AttrInt *) *itrCurr));
      }
    else if ((*itrCurr)->IsType (AttrString::Identifier ()))
      {
      SaveString (parserIn, *((AttrString *) *itrCurr));
      }
    else if ((*itrCurr)->IsType (AttrStringArray::Identifier ()))
      {
      SaveStringArray (parserIn, *((AttrStringArray *) *itrCurr));
      }
    else if ((*itrCurr)->IsType (AttrIntArray::Identifier ()))
      {
      SaveIntArray (parserIn, *((AttrIntArray *) *itrCurr));
      }
    else if ((*itrCurr)->IsType (AttrFloatArray::Identifier ()))
      {
      SaveFloatArray (parserIn, *((AttrFloatArray *) *itrCurr));
      }
    };
  // now that we know how much data we saved, we can store it in the block header
  INT32  iEndPos = parserIn.GetCursorStart ();
  parserIn.SetCursorStart (iSizeLocation);
  parserIn.SetU4_LEnd (iEndPos - iSizeLocation - sizeof (INT32));
  parserIn.SetCursorStart (iEndPos);
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::LoadInt  (RStrParser &  parserIn,
                                         Attr * *      ppattrIn)
  {
  INT32  iSavedPos = parserIn.GetCursorStart ();

  if (parserIn.GetU4_LEnd () != AttrInt::CCIdentifier ())
    {
    parserIn.SetCursorStart (iSavedPos);
    return (EStatus::kFailure);
    };
  //INT32  iSize = parserIn.GetU4_LEnd ();  iSize = iSize;
  parserIn.GetU4_LEnd ();

  if (*ppattrIn == NULL)
    {
    *ppattrIn = new AttrInt ("");
    }

  INT32  iNameLength = parserIn.GetU4_LEnd ();
  parserIn.GetData (&(*ppattrIn)->strName, iNameLength);

  ((AttrInt *)*ppattrIn)->Set (parserIn.GetU4_LEnd ());

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::SaveInt  (RStrParser &  parserIn,
                                         AttrInt &     attrIn)
  {
  parserIn.SetU4_LEnd (AttrInt::CCIdentifier ());
  parserIn.SetU4_LEnd (sizeof (INT32) + attrIn.strName.Length () + sizeof (INT32));

  parserIn.SetU4_LEnd (attrIn.strName.Length ());
  parserIn.SetData    ((const unsigned char *)attrIn.strName.AsChar (), attrIn.strName.Length ());

  parserIn.SetU4_LEnd ((INT32)attrIn.Value ());

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::LoadFloat (RStrParser &  parserIn,
                                          Attr * *      ppattrIn)
  {
  INT32  iSavedPos = parserIn.GetCursorStart ();

  if (parserIn.GetU4_LEnd () != AttrFloat::CCIdentifier ())
    {
    parserIn.SetCursorStart (iSavedPos);
    return (EStatus::kFailure);
    };
  //INT32  iSize = parserIn.GetU4_LEnd ();  iSize = iSize;
  parserIn.GetU4_LEnd ();

  if (*ppattrIn == NULL)
    {
    *ppattrIn = new AttrFloat ("");
    }

  INT32  iNameLength = parserIn.GetU4_LEnd ();
  parserIn.GetData (&(*ppattrIn)->strName, iNameLength);

  ((AttrFloat *)*ppattrIn)->Set (parserIn.GetF4_LEnd ());

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::SaveFloat  (RStrParser &  parserIn,
                                           AttrFloat &   attrIn)
  {
  parserIn.SetU4_LEnd (AttrFloat::CCIdentifier ());
  parserIn.SetU4_LEnd (sizeof (INT32) + attrIn.strName.Length () + sizeof (FLOAT32));

  parserIn.SetU4_LEnd (attrIn.strName.Length ());
  parserIn.SetData    ((const unsigned char *)attrIn.strName.AsChar (), attrIn.strName.Length ());

  parserIn.SetF4_LEnd ((FLOAT32)attrIn.Value ());

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::LoadString (RStrParser &  parserIn,
                                           Attr * *      ppattrIn)
  {
  INT32  iSavedPos = parserIn.GetCursorStart ();

  if (parserIn.GetU4_LEnd () != AttrString::CCIdentifier ())
    {
    parserIn.SetCursorStart (iSavedPos);
    return (EStatus::kFailure);
    };
  //INT32  iSize = parserIn.GetU4_LEnd ();  iSize = iSize;
  parserIn.GetU4_LEnd ();

  if (*ppattrIn == NULL)
    {
    *ppattrIn = new AttrString ("");
    }

  INT32  iNameLength = parserIn.GetU4_LEnd ();
  parserIn.GetData (&(*ppattrIn)->strName, iNameLength);

  INT32  iStringLength = parserIn.GetU4_LEnd ();
  parserIn.GetData (&((AttrString *)*ppattrIn)->strValue, iStringLength);

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::SaveString  (RStrParser &  parserIn,
                                            AttrString &  attrIn)
  {
  parserIn.SetU4_LEnd (AttrString::CCIdentifier ());
  parserIn.SetU4_LEnd (sizeof (UINT32) + attrIn.strName.Length () + sizeof (UINT32) + attrIn.strValue.Length ());

  parserIn.SetU4_LEnd (attrIn.strName.Length ());
  parserIn.SetData    ((const unsigned char *)attrIn.strName.AsChar (), attrIn.strName.Length ());

  parserIn.SetU4_LEnd (attrIn.strValue.Length ());
  parserIn.SetData    ((const unsigned char *)attrIn.strValue.AsChar (), attrIn.strValue.Length ());

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::LoadStringArray (RStrParser &  parserIn,
                                                Attr * *      ppattrIn)
  {
  INT32  iSavedPos = parserIn.GetCursorStart ();

  if (parserIn.GetU4_LEnd () != AttrStringArray::CCIdentifier ())
    {
    parserIn.SetCursorStart (iSavedPos);
    return (EStatus::kFailure);
    };
  //INT32  iSize = parserIn.GetU4_LEnd ();  iSize = iSize;
  parserIn.GetU4_LEnd ();

  if (*ppattrIn == NULL)
    {
    *ppattrIn = new AttrStringArray ("");
    }

  INT32  iNameLength = parserIn.GetU4_LEnd ();
  parserIn.GetData (&(*ppattrIn)->strName, iNameLength);

  INT  iNumStrings = (INT) parserIn.GetU4_LEnd ();
  ((AttrStringArray *)*ppattrIn)->arrayValue.SetMinLength (iNumStrings);

  for (INT  iIndex = 0; iIndex < iNumStrings; ++iIndex)
    {
    INT32  iStringLength = parserIn.GetU4_LEnd ();
    parserIn.GetData (&((AttrStringArray *)*ppattrIn)->arrayValue[iIndex], iStringLength);
    };

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::SaveStringArray  (RStrParser &       parserIn,
                                                 AttrStringArray &  attrIn)
  {

  parserIn.SetU4_LEnd (AttrStringArray::CCIdentifier ());
  INT  iSizeLocation = parserIn.GetCursorStart ();
  parserIn.SetU4_LEnd (0);  // reserve a location for the total size

  // Attr name
  parserIn.SetU4_LEnd (attrIn.strName.Length ());
  parserIn.SetData    ((const unsigned char *)attrIn.strName.AsChar (), attrIn.strName.Length ());

  // num of strings
  INT  iNumStrings = attrIn.arrayValue.Length ();
  parserIn.SetU4_LEnd (iNumStrings);

  // save strings
  for (INT  iIndex = 0; iIndex < iNumStrings; ++iIndex)
    {
    parserIn.SetU4_LEnd (attrIn.arrayValue[iIndex].Length ());
    parserIn.SetData    ((const unsigned char *)attrIn.arrayValue[iIndex].AsChar (), attrIn.arrayValue[iIndex].Length ());
    };

  // now that we know how much data we saved, we can store it in the block header
  INT32  iEndPos = parserIn.GetCursorStart ();
  parserIn.SetCursorStart (iSizeLocation);
  parserIn.SetU4_LEnd (iEndPos - iSizeLocation - sizeof (INT32));
  parserIn.SetCursorStart (iEndPos);

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::LoadIntArray (RStrParser &  parserIn,
                                             Attr * *      ppattrIn)
  {
  INT32  iSavedPos = parserIn.GetCursorStart ();

  if (parserIn.GetU4_LEnd () != AttrIntArray::CCIdentifier ())
    {
    parserIn.SetCursorStart (iSavedPos);
    return (EStatus::kFailure);
    };
  //INT32  iSize = parserIn.GetU4_LEnd ();  iSize = iSize;
  parserIn.GetU4_LEnd ();

  if (*ppattrIn == NULL)
    {
    *ppattrIn = new AttrIntArray ("");
    }

  INT32  iNameLength = parserIn.GetU4_LEnd ();
  parserIn.GetData (&(*ppattrIn)->strName, iNameLength);

  INT  iNumElements = (INT) parserIn.GetU4_LEnd ();
  ((AttrIntArray *)*ppattrIn)->arrayValue.SetMinLength (iNumElements);

  for (INT  iIndex = 0; iIndex < iNumElements; ++iIndex)
    {
    ((AttrIntArray *)*ppattrIn)->arrayValue[iIndex] = parserIn.GetU4_LEnd ();
    };

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::SaveIntArray  (RStrParser &    parserIn,
                                              AttrIntArray &  attrIn)
  {

  parserIn.SetU4_LEnd (AttrIntArray::CCIdentifier ());
  INT  iSizeLocation = parserIn.GetCursorStart ();
  parserIn.SetU4_LEnd (0);  // reserve a location for the total size

  // Attr name
  parserIn.SetU4_LEnd (attrIn.strName.Length ());
  parserIn.SetData    ((const unsigned char *)attrIn.strName.AsChar (), attrIn.strName.Length ());

  // num of elements
  INT  iNumElements = attrIn.arrayValue.Length ();
  parserIn.SetU4_LEnd (iNumElements);

  // save ints
  for (INT  iIndex = 0; iIndex < iNumElements; ++iIndex)
    {
    parserIn.SetU4_LEnd (attrIn.arrayValue[iIndex]);
    };

  // now that we know how much data we saved, we can store it in the block header
  INT32  iEndPos = parserIn.GetCursorStart ();
  parserIn.SetCursorStart (iSizeLocation);
  parserIn.SetU4_LEnd (iEndPos - iSizeLocation - sizeof (INT32));
  parserIn.SetCursorStart (iEndPos);

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::LoadFloatArray (RStrParser &  parserIn,
                                               Attr * *      ppattrIn)
  {
  INT32  iSavedPos = parserIn.GetCursorStart ();

  if (parserIn.GetU4_LEnd () != AttrFloatArray::CCIdentifier ())
    {
    parserIn.SetCursorStart (iSavedPos);
    return (EStatus::kFailure);
    };
  //INT32  iSize = parserIn.GetU4_LEnd ();  iSize = iSize;
  parserIn.GetU4_LEnd ();

  if (*ppattrIn == NULL)
    {
    *ppattrIn = new AttrFloatArray ("");
    }

  INT32  iNameLength = parserIn.GetU4_LEnd ();
  parserIn.GetData (&(*ppattrIn)->strName, iNameLength);

  INT  iNumElements = (INT) parserIn.GetU4_LEnd ();
  ((AttrFloatArray *)*ppattrIn)->arrayValue.SetMinLength (iNumElements);

  for (INT  iIndex = 0; iIndex < iNumElements; ++iIndex)
    {
    ((AttrFloatArray *)*ppattrIn)->arrayValue[iIndex] = parserIn.GetF4_LEnd ();
    };

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  AttrBinarySerializer::SaveFloatArray  (RStrParser &      parserIn,
                                                AttrFloatArray &  attrIn)
  {

  parserIn.SetU4_LEnd (AttrFloatArray::CCIdentifier ());
  INT  iSizeLocation = parserIn.GetCursorStart ();
  parserIn.SetU4_LEnd (0);  // reserve a location for the total size

  // Attr name
  parserIn.SetU4_LEnd (attrIn.strName.Length ());
  parserIn.SetData    ((const unsigned char *)attrIn.strName.AsChar (), attrIn.strName.Length ());

  // num of elements
  INT  iNumElements = attrIn.arrayValue.Length ();
  parserIn.SetU4_LEnd (iNumElements);

  // save floats
  for (INT  iIndex = 0; iIndex < iNumElements; ++iIndex)
    {
    parserIn.SetF4_LEnd (attrIn.arrayValue[iIndex]);
    };

  // now that we know how much data we saved, we can store it in the block header
  INT32  iEndPos = parserIn.GetCursorStart ();
  parserIn.SetCursorStart (iSizeLocation);
  parserIn.SetU4_LEnd (iEndPos - iSizeLocation - sizeof (INT32));
  parserIn.SetCursorStart (iEndPos);

  return (EStatus::kSuccess);
  };











