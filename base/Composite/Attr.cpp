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

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Composite/Attr.hpp"
#include "Containers/TList.hpp"

static TList<Attr *>  listAttrTemplates;

// TODO:  Method to delete all templates.

//-----------------------------------------------------------------------------
//  Attr
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Attr::Attr (const char *  szNameIn) : strName (szNameIn)
  {
  szType      = NULL;
  ccType      = 0;
  bLocked     = FALSE;
  bPublished  = TRUE;
  bDirty      = TRUE;
  iVersion    = 0;
  iArrayIndex = 0;

  pCachedRegistry = NULL;
  strRegistryKey.Empty ();
  pCachedElem     = NULL;
  };

//-----------------------------------------------------------------------------
Attr::~Attr ()
  {
  OnDelete ();
  };

//-----------------------------------------------------------------------------
VOID  Attr::CopyCommonVars (const Attr &  attrIn)
  {
  strName     = attrIn.strName;
  szType      = attrIn.szType;
  ccType      = attrIn.ccType;
  bLocked     = attrIn.bLocked;
  bPublished  = attrIn.bPublished;
  bDirty      = attrIn.bDirty;
  iVersion    = attrIn.iVersion;
  iArrayIndex = attrIn.iArrayIndex;

  strRegistryKey  = attrIn.strRegistryKey;
  pCachedRegistry = attrIn.pCachedRegistry;
  pCachedElem     = NULL;
  };

//-----------------------------------------------------------------------------
VOID  Attr::DebugPrint   (const char *  szIndentIn,
                          RStr &        strOut) const
  {
  // first try any custom debug print
  DebugPrint (strOut);

  // then do the general AsString DebugPrint
  RStr  strValue;
  GetAsString (&strValue);

  strOut.AppendFormat ("%sAttr (%s): %s [%s]\n", szIndentIn, Type (), Name (), strValue.AsChar ());
  };

//-----------------------------------------------------------------------------
Attr *  Attr::New (const char *  szNameIn,
                   const char *  szTypeIn)
  {
  for (TListItr<Attr*> itrCurr = listAttrTemplates.First ();
       itrCurr.IsValid();
       ++itrCurr)
    {
    if (streq ((*itrCurr)->Type (), szTypeIn))
      {
      // found the type template
      Attr *  pNew = (*itrCurr)->Instantiate (szNameIn);
      return (pNew);
      };
    };
  return NULL;
  }

//-----------------------------------------------------------------------------
VOID  Attr::AddTemplate (Attr *  pAttrIn)
  {
  // NOTE: The object passed to AddTemplate via pointer will be owned by Attr
  //   from here on out, and will be deallocated by Attr as needed.

  for (TListItr<Attr*> itrCurr = listAttrTemplates.First ();
       itrCurr.IsValid();
       ++itrCurr)
    {
    if ((*itrCurr)->IsType (pAttrIn->Type ()))
      {
      // we already have this type in the templates
      delete pAttrIn;
      return;
      };
    };
  listAttrTemplates.PushBack (pAttrIn);
  }

//-----------------------------------------------------------------------------
VOID  Attr::DeleteAllTemplates (VOID)
  {
  for (TListItr<Attr*> itrCurr = listAttrTemplates.First ();
       itrCurr.IsValid();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listAttrTemplates.Empty ();
  }

//-----------------------------------------------------------------------------
VOID  Attr::SetRegistryKey (const char *  szKeyIn)
  {
  strRegistryKey.Set (szKeyIn, TRUE);
  DisconnectFromRegistry ();
  CacheElemConnections ();
  };

//-----------------------------------------------------------------------------
VOID  Attr::CacheRegistry (VOID)
  {
  // if we haven't cached the named value registry, do so now
  if ((pCachedRegistry == NULL) && (!strRegistryKey.IsEmpty ()))
    {
    pCachedRegistry = ValueRegistry::Root ();
    };
  };

//-----------------------------------------------------------------------------
VOID  Attr::DisconnectFromRegistry (VOID)
  {
  // Disconnect from the registered signal, if we are connected.
  if (pCachedElem != NULL)
    {
    pCachedElem->sigOnChanged.Disconnect (this, &Attr::OnElemChangedListener);
    pCachedElem->sigOnDelete.Disconnect (this, &Attr::OnElemDeletedListener);
    pCachedElem = NULL;
    }
  };

//-----------------------------------------------------------------------------
VOID Attr::OnElemChangedListener (ValueElem *  pelemIn,
                                  BOOL         bUpdatingIn)
  {
  UpdateFromRegistry (bUpdatingIn);
  }

//-----------------------------------------------------------------------------
VOID Attr::OnElemDeletedListener (ValueElem *  pelemIn)
  {
  DisconnectFromRegistry ();
  }

//-----------------------------------------------------------------------------
VOID  Attr::CacheElemConnections (VOID)
  {
  CacheRegistry ();

  if (pCachedRegistry != NULL)
    {
    // cache the value if needed, and sign up for it changing
    if ((pCachedElem == NULL) && (!strRegistryKey.IsEmpty ()))
      {
      ValueElem *  pelemSearch = pCachedRegistry->Find (strRegistryKey.AsChar ());

      if (pelemSearch != NULL)
        {
        // NOTE:  We do not filter on type, so compatible types will have to be
        //  determined in the UpdateFromRegistry call.
        pCachedElem = pelemSearch;
        pCachedElem->sigOnChanged.Connect (this, &Attr::OnElemChangedListener);
        pCachedElem->sigOnDelete.Connect (this, &Attr::OnElemDeletedListener);

        // now that we are attached, init from the current value of the value elem
        UpdateFromRegistry (TRUE);
        }
      else
        {
        DBG_WARNING ("Attr unable to find registry key : %s", strRegistryKey.AsChar ());
        };
      };
    }
  // NOTE:  For now, let's silently fail here.  This will occur if a registry key isn't set,
  //          but that should be ok.
  //else
  //  {
  //  DBG_WARNING ("Attr unable to cache element connection because no registry has been set.");
  //  };
  };

//-----------------------------------------------------------------------------
VOID  Attr::UpdateFromRegistry (BOOL  bUpdatingIn)
  {
  CacheElemConnections ();
  //UpdateText ();
  };






