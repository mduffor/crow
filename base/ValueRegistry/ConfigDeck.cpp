/* -----------------------------------------------------------------
                            Config Deck

     This module implements a value registry composed of overlapping
     layers of key-value pairs.

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

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

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Util/CalcHash.hpp"
#include "Util/ParseTools.hpp"
#include "Sys/FilePath.hpp"
#include "ConfigDeck.hpp"


// TODO
// RemoteConfigFn: CheckABTestEnabled, MatchesABSegName, IsIos, IsAndroid, IsCountry, IsRegion, IsOrganic
// DateTimeFn: LocalBeforeDate, LocalAfterDate, LocalBetweenDates, AddToDate.  Network/UTC* versions


ConfigDeck *   ConfigDeck::pInstance = NULL;


//-----------------------------------------------------------------------------
VOID ConfigDeck::Init (VOID)
  {
  //RemoteConfigFn.register
  //DateTimeFn.Register
  //ExpressionDefaultFn.Register
  };


//-----------------------------------------------------------------------------
ConfigDeck::ConfigDeck (ValueRegistry *  pRegIn)
  {
  pReg = (pRegIn != NULL) ? pRegIn : ValueRegistry::Root();
  };


//-----------------------------------------------------------------------------
EStatus ConfigDeck::Deserialize (RStrParser &  parserJsonIn,
                                 const char *  szSourceIn)
  {

  parserJsonIn.SetSkipComments (RStrParser::kCStyle);
  parserJsonIn.SkipWhitespace ();

  // check if this is an array of layers or a single layer.
  if (parserJsonIn.PeekChar () == '[')
    {
    if (! ParseTools::SkipChar (parserJsonIn, '['))  {return (EStatus::kFailure);};

    // step through  list
    while (parserJsonIn.PeekChar () != ']')
      {
      // find layer and update if it exists, else create it

      ConfigLayer *   pLayer = listLayers.PushBack (new ConfigLayer ());

      EStatus  status;
      if ((status = pLayer->Deserialize (parserJsonIn, szSourceIn)) == EStatus::kFailure)
        {
        return (status);
        };

      // check for next
      if (! ParseTools::SkipChar (parserJsonIn, ','))  {break;};

      }; // while not end of list

    if (! ParseTools::SkipChar (parserJsonIn, ']'))  {return (EStatus::kFailure);};

    } else {
      ConfigLayer *   pLayer = listLayers.PushBack (new ConfigLayer ());
      return (pLayer->Deserialize (parserJsonIn, szSourceIn));
    };

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
ConfigLayer *  ConfigDeck::FindLayerByName (const char *  szNameIn)
  {
  HASH_T   uNameHash = HASH(szNameIn);

  for (TListItr<ConfigLayer*>  itrCurr = listLayers.First ();
        itrCurr.IsValid ();
        ++itrCurr)
    {
    if ((*itrCurr)->NameEquals (uNameHash, szNameIn))
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };


//-----------------------------------------------------------------------------
ConfigLayer *  ConfigDeck::FindOrCreateLayer (const char *  szNameIn,
                                              INT           iOrderIn)
  {
  ConfigLayer *  pLayer = FindLayerByName (szNameIn);

  if (pLayer == NULL)
    {
    pLayer = listLayers.PushBack (new ConfigLayer ());

    if (pLayer != NULL)
      {
      pLayer->SetOrder (iOrderIn);
      };
    };
  return (pLayer);
  };


//-----------------------------------------------------------------------------
INT  ConfigDeck::GetLayerCount (VOID)
  {
  return (listLayers.Size ());
  };


//-----------------------------------------------------------------------------
VOID  ConfigDeck::DeleteAllLayers (VOID)
  {
  for (TListItr<ConfigLayer*>  itrCurr = listLayers.First ();
        itrCurr.IsValid ();
        ++itrCurr)
    {
    delete (*itrCurr);
    };
  listLayers.Empty();
  };


//-----------------------------------------------------------------------------
EStatus  ConfigDeck::ReadBuffer (const char *  szNameIn,
                                 const char *  szTypeIn,
                                 const char *  szBufferIn)
  {
  RStrParser  parserBuffer (szBufferIn);
  return (ReadBuffer (szNameIn, szTypeIn, parserBuffer));
  };

//-----------------------------------------------------------------------------
EStatus  ConfigDeck::ReadBuffer (const char *  szNameIn,
                                 const char *  szTypeIn,
                                 RStrParser &  parserIn)
  {
  // NOTE: At one time the Name was intended to be set as the "source" of the
  //  layer, but I'm not sure how that will be utilized in practice.  So at this
  //  point in time, the name is ignored and the buffer is simply deserialized.

  RStr  strType (szTypeIn, TRUE);

  if (strType.Equals ("rcfg"))
    {
    EStatus status = Deserialize (parserIn);
    if (status == EStatus::kFailure)
      {
      RStr  strOut;
      strOut.Format ("ConfigDeck - Failed to load \"%s\": %s", szNameIn, status.GetDescription());
      status.SetDescription (strOut.AsChar ());
      return (status);
      }
    ResolveStack (pReg, TRUE);
    };
  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  ConfigDeck::ReadFile (const char *  szFilenameIn)
  {
  EStatus      status;
  RStrParser   parserFile;

  if ((status = parserFile.ReadFromFile (szFilenameIn)) == EStatus::kSuccess)
    {
    return (Deserialize (parserFile, FilePath::GetFilenameNoExtFromPath (szFilenameIn)));
    };
  return (status);
  };

//-----------------------------------------------------------------------------
VOID  ConfigDeck::ReadDirectory (const char *  szRelativePathIn,
                                 const char *  szGlobMatchIn)
  {
  // NOTE: This will read the files in a directory on disk, but not from
  //  a resource directory.

  RegEx       rexSearch    (szGlobMatchIn, RegEx::EParser::kGlob);
  RegEx       rexSearchTwo ("");
  RStr        strExpandedPath (FilePath::ExpandPathURI (szRelativePathIn));
  RStrArray   arrayAllPaths;

  FilePath::DirTreeSearch  (strExpandedPath.AsChar (),
                            rexSearch, rexSearchTwo,
                            arrayAllPaths);

  INT  iNumPaths = arrayAllPaths.Length();
  for (INT  iIndex = 0; iIndex < iNumPaths; ++iIndex)
    {
    // TODO : test
    printf ("Found Path %s", arrayAllPaths[iIndex].AsChar ());
    ReadFile (arrayAllPaths[iIndex].AsChar ());
    };
  }

//-----------------------------------------------------------------------------
INT ConfigDeck::CompareLayerOrders (ConfigLayer*  pAIn,
                                    ConfigLayer*  pBIn)
  {
  return ((pAIn->GetBaseSegment()->iOrder == pBIn->GetBaseSegment()->iOrder) ? 0 :
          (pAIn->GetBaseSegment()->iOrder  < pBIn->GetBaseSegment()->iOrder) ? -1 : 1);
  };

//-----------------------------------------------------------------------------
VOID ConfigDeck::ResolveStack (ValueRegistry *  pRegIn,
                               BOOL             bFullResetIn)
  {
  ValueRegistry *  pRegFinal = (pRegIn != NULL) ? pRegIn : pReg;

  if (bFullResetIn)
    {
    pRegFinal->Clear();
    }

  listLayers.Sort (&CompareLayerOrders);

  for (TListItr<ConfigLayer*>  itrCurr = listLayers.First ();
        itrCurr.IsValid ();
        ++itrCurr)
    {
    if ((*itrCurr)->IsExpressionDriven () == FALSE)
      {
      if ((*itrCurr)->IsActive (*pRegFinal))
        {
        //DBG_INFO ("ConfigDeck::ResolveStack Phase 1 Merging layer %s\n", (*itrCurr)->GetName());
        (*itrCurr)->MergeInto (*pRegFinal);
        };
      };
    };

  for (TListItr<ConfigLayer*>  itrCurr = listLayers.First ();
        itrCurr.IsValid ();
        ++itrCurr)
    {
    if ((*itrCurr)->IsExpressionDriven ())
      {
      if ((*itrCurr)->IsActive (*pRegFinal))
        {
        //DBG_INFO ("ConfigDeck::ResolveStack Phase 2 Merging layer %s\n", (*itrCurr)->GetName());
        (*itrCurr)->MergeInto (*pRegFinal);
        };
      };
    };

  sigOnResolveStack();
  };

//-----------------------------------------------------------------------------
const char *  ConfigDeck::GetConfigSavePath (VOID)
  {
  return GetConfigPath("rconfigs");
  };


//-----------------------------------------------------------------------------
const char *  ConfigDeck::GetConfigCachePath (VOID)
  {
  return GetConfigPath("rconfig_cache");
  };

//-----------------------------------------------------------------------------
const char *  ConfigDeck::GetConfigPath (const char *  szSubdirectory)
  {
  static RStr  strExpandedPath;

  RStr  strRawPath      (FilePath::Combine ("file://", szSubdirectory));

  strExpandedPath = FilePath::ExpandPathURI (strRawPath.AsChar ());

  FilePath::MakeDir (strExpandedPath.AsChar ());

  return (strExpandedPath.AsChar ());
  };

