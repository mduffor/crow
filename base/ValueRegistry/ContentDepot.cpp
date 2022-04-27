/* -----------------------------------------------------------------
                            Content Depot

     This module enables registering a variety of files across local
   and network sources, detect version changes, download, cache,
   load into memory, and pass memory buffers to interested systems.

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

#include "ValueRegistry/ContentDepot.hpp"


#include "ValueRegistry/ValueRegistry.hpp"
#include "ValueRegistry/ConfigSubset.hpp"
#include "Script/Expression.hpp"
#include "Util/ParseTools.hpp"




//-----------------------------------------------------------------------------
const char *  ContentLocation::GetUri (const char *  szEnvIn)
  {
  if (arrayEnvUriPairs.FindKey (szEnvIn) != -1)
    {
    return (arrayEnvUriPairs[szEnvIn]);
    };
  if (arrayEnvUriPairs.FindKey ("default") != -1)
    {
    return (arrayEnvUriPairs["default"]);
    };
  return (arrayEnvUriPairs[""]);
  };









//-----------------------------------------------------------------------------
ContentDepot::ContentDepot  ()
  {
  };

//-----------------------------------------------------------------------------
ContentDepot::~ContentDepot  ()
  {
  RemoveAllAssets();
  };

//-----------------------------------------------------------------------------
ContentLocation * ContentDepot::AddAsset (const char *  szTagIn,
                                          const char *  szTypeIn,
                                          const char *  szEnvIn,
                                          const char *  szUrlIn,
                                          RStr *        pstrBufferIn)
  {
  // NOTE:  It would probably be better if we could add multiple
  //  Env/Url pairs at once, but there isn't a good way to pass that
  //  into the method call in C++ without some sort of garbage collection
  //  to clean up the temporary object.

  ContentLocation *  pLoc = FindAsset (szTagIn);

  if (pLoc == NULL)
    {
    pLoc = listContents.PushBack (new ContentLocation);
    pLoc->strTag.Set  (szTagIn, TRUE);
    pLoc->strType.Set (szTypeIn, TRUE);
    };

  pLoc->arrayEnvUriPairs.SetAt (szEnvIn, szUrlIn);

  if (pstrBufferIn != NULL)
    {
    pLoc->parserCachedData.Set (*pstrBufferIn);
    pLoc->bIsLoaded = TRUE;
    pLoc->bIsDirty  = TRUE;
    };

  return (pLoc);
  };

//-----------------------------------------------------------------------------
ContentLocation *  ContentDepot::FindAsset (const char *  szTagIn)
  {
  RStr  strTagSearch (szTagIn, TRUE);
  TListItr<ContentLocation*>  itrNext;

  for (TListItr<ContentLocation*>  itrCurr = listContents.First();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->strTag.Equals (strTagSearch))
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::RemoveAsset (const char *  szTagIn)
  {
  RStr  strTagSearch (szTagIn, TRUE);
  TListItr<ContentLocation*>  itrNext;

  for (TListItr<ContentLocation*>  itrCurr = listContents.First();
       itrCurr.IsValid ();
       itrCurr = itrNext)
    {
    itrNext = itrCurr; ++itrNext;

    if ((*itrCurr)->strTag.Equals (strTagSearch))
      {
      delete (*itrCurr);
      listContents.Delete (itrCurr);
      return;
      };
    };
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::RemoveAllAssets (VOID)
  {
  for (TListItr<ContentLocation*>  itrCurr = listContents.First();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  listContents.Empty ();
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::AddDependency (const char *  szTagIn,
                                  const char *  szDepTagIn)
  {
  ContentLocation *  pLoc = FindAsset (szTagIn);

  if (pLoc == NULL) return;

  pLoc->astrDependencies.AppendUnique (szDepTagIn);
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::AddDependencies (const char *  szTagIn,
                                    RStrArray &   arrayDepTagsIn)
  {
  ContentLocation *  pLoc = FindAsset (szTagIn);

  if (pLoc == NULL) return;

  INT  iNumTags = arrayDepTagsIn.Length();
  for (INT  iIndex = 0; iIndex < iNumTags; ++iIndex)
    {
    pLoc->astrDependencies.AppendUnique (arrayDepTagsIn[iIndex]);
    };
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::RecalcDependents (VOID)
  {
  for (TListItr<ContentLocation*>  itrCurr = listContents.First();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->astrDependents.Clear ();
    };

  for (TListItr<ContentLocation*>  itrCurr = listContents.First();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    INT  iNumDep = (*itrCurr)->astrDependencies.Length();
    for (INT  iIndex = 0; iIndex < iNumDep; ++iIndex)
      {
      ContentLocation *  pLocDep = FindAsset ((*itrCurr)->astrDependencies[iIndex]);

      if (pLocDep != NULL)
        {
        pLocDep->astrDependents.AppendUnique ((*itrCurr)->strTag);
        };
      };
    };
  };

//-----------------------------------------------------------------------------
BOOL ContentDepot::AreDependenciesLoaded (ContentLocation *  pLocIn)
  {
  // Checks its dependencies, and sees if they are loaded/returned.

  if (pLocIn == NULL) return (TRUE);

  INT  iNumDep = pLocIn->astrDependencies.Length();
  for (INT  iIndex = 0; iIndex < iNumDep; ++iIndex)
    {
    ContentLocation *  pLocDep = FindAsset (pLocIn->astrDependencies[iIndex]);

    if (pLocDep != NULL)
      {
      // if any unloaded asset is found, return that dependencies are not yet loaded.
      if (! pLocDep->bIsLoaded) {return (FALSE);};

      // check chained dependencies
      if (! AreDependenciesLoaded (pLocDep)) {return (FALSE);};
      };
    };
  return (TRUE);
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::GetRootDependants (ContentLocation *           plocIn,
                                      TList<ContentLocation *> &  listOut)
  {
  // step up dependency tree to find roots.

  INT  iNumDep = plocIn->astrDependents.Length();

  if (iNumDep == 0)
    {
    listOut.PushBack (plocIn);
    return;
    };

  for (INT  iIndex = 0; iIndex < iNumDep; ++iIndex)
    {
    ContentLocation *  plocDep = FindAsset (plocIn->astrDependents[iIndex]);

    if (plocDep != NULL)
      {
      GetRootDependants (plocDep, listOut);
      };
    };
  };


//-----------------------------------------------------------------------------
VOID ContentDepot::Refresh (BOOL   bForceRefreshIn)
  {
  TListItr<ContentLocation*>  itrCurr;

  if (bForceRefreshIn)
    {
    for (itrCurr = listContents.First(); itrCurr.IsValid (); ++itrCurr)
      {
      (*itrCurr)->bIsDirty = TRUE;
      };
    };

  if (strEnvironment.IsEmpty ())
    {
    DBG_WARNING ("ContentDepot::Refresh () - Environment is not set");
    };

  arrayEnvVars.SetAt ("Env", strEnvironment.AsChar());

  for (itrCurr = listContents.First(); itrCurr.IsValid (); ++itrCurr)
    {
    //printf ("StartAsyncLoad for %s\n", (*itrCurr)->strTag.AsChar ());
    StartAsyncLoad (*itrCurr);
    };
  };

//-----------------------------------------------------------------------------
VOID  ContentDepot::RunCallbacks (ContentLocation *  pLocIn)
  {
  // run the RetrieveData call on each asset, dependents first.  If IsEverythingReturned, call OnAllLoaded.

  if (pLocIn == NULL) return;

  // run callbacks on dependencies first
  INT  iNumDep = pLocIn->astrDependencies.Length();
  for (INT  iIndex = 0; iIndex < iNumDep; ++iIndex)
    {
    ContentLocation *  pLocDep = FindAsset (pLocIn->astrDependencies[iIndex]);
    RunCallbacks (pLocDep);
    };

  // Now you can run the callback on this location
  if (!pLocIn->bIsReported)
    {
    //DBG_INFO ("RunCallbacks for %s", pLocIn->strTag.AsChar());
    RetrieveData (pLocIn, strEnvironment.AsChar());

    if (IsEverythingReturned ())
      {
      sigOnAllLoaded();
      };
    };
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::LoadAsset (const char *       szTagIn)
  {
  // NOTE: This probably won't work if there are dependencies that are not yet
  //  loaded.  You'll need to think through that situation better.
  // This call is meant for forcing the callback for files that are already
  //  loaded and cached.
  ContentLocation *  pLoc = FindAsset (szTagIn);
  if (pLoc != NULL)
    {
    pLoc->bIsReported = FALSE;
    if (pLoc->bIsDirty)
      {
      StartAsyncLoad (pLoc);
      }
    else
      {
      OnAsyncLoadComplete (pLoc);
      };
    };
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::StartAsyncLoad (const char *  szTagIn)
  {
  ContentLocation *  pLoc = FindAsset (szTagIn);
  if (pLoc != NULL)
    {
    StartAsyncLoad (pLoc);
    };
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::StartAsyncLoad (ContentLocation *  pLocIn)
  {
  //Depending on URL prefix, make the call needed to start the download.  res, file, mem, http, https

  if (pLocIn == NULL) return;

  // Depending on the type of URI, we need to either start an async
  //  load process, or go right to synchronously loading it.
  if (pLocIn->bIsDirty)
    {
    RStrParser  parserExpandedUri (pLocIn->GetUri (strEnvironment.AsChar()));
    arrayEnvVars.ExpandVars (parserExpandedUri);
    parserExpandedUri.ResetCursor ();

    if (parserExpandedUri.StartsWith ("res://"))
      {
      // (Android) resource file
      pLocIn->bIsLoaded = TRUE;
      OnAsyncLoadComplete (pLocIn);
      }
    else if (parserExpandedUri.StartsWith ("file://"))
      {
      // local disk file
      pLocIn->bIsLoaded = TRUE;
      OnAsyncLoadComplete (pLocIn);
      }
    else if (parserExpandedUri.StartsWith ("mem://"))
      {
      // memory buffer (already stored in Data for content location)
      pLocIn->bIsLoaded = TRUE;
      //DBG_INFO ("mem OnAsyncLoadComplete");
      OnAsyncLoadComplete (pLocIn);
      }
    else if (parserExpandedUri.StartsWith ("http://") ||
             parserExpandedUri.StartsWith ("https://"))
      {
      // Remote File.  Need to distinguish between RESTful, S3 bucket, etc.
      pLocIn->bIsLoaded = FALSE;
      GetHttpHeader (parserExpandedUri.AsChar (), pLocIn->strTag.AsChar ());
      }
    else
      {
      // Error:  Don't know how to handle this URI
      }
    };
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::OnAsyncLoadComplete (const char *  szTagIn)
  {
  ContentLocation *  pLoc = FindAsset (szTagIn);
  if (pLoc != NULL)
    {
    OnAsyncLoadComplete (pLoc);
    };
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::OnAsyncLoadComplete (ContentLocation *  pLocIn)
  {
  if (pLocIn == NULL) return;

  //DBG_INFO ("OnAsyncLoadComplete for %s\n", pLocIn->strTag.AsChar());

  pLocIn->bIsDirty = FALSE;

  if (pLocIn->strTag.IsEmpty ())
    {
    // anonymous asset.
    RunCallbacks (pLocIn);
    }
  else
    {
    // This content has loaded, but we need to make sure its dependencies
    //  have also finished loading before sending it to the callbacks.
    TList<ContentLocation *>   listDeps;

    GetRootDependants (pLocIn, listDeps);

    for (TListItr<ContentLocation *>  itrCurr =  listDeps.First ();
         itrCurr.IsValid ();
         ++itrCurr)
      {
      if (! AreDependenciesLoaded (*itrCurr)) return;

      RunCallbacks (*itrCurr);
      };
    };
  };


//-----------------------------------------------------------------------------
VOID ContentDepot::RetrieveData (ContentLocation *  pLocIn,
                                 const char *       szEnvIn)
  {

  if (pLocIn == NULL) return;

  if (pLocIn->bIsDirty) return;

  RStrParser  parserExpandedUri (pLocIn->GetUri (szEnvIn));
  arrayEnvVars.ExpandVars (parserExpandedUri);
  parserExpandedUri.ResetCursor ();

  RStrParser &  parserCache = pLocIn->GetCachedData (szEnvIn);


  if (parserExpandedUri.StartsWith ("res://") ||
      parserExpandedUri.StartsWith ("file://"))
    {
    // (Android) resource file or
    // local disk file (both handled with RStrParser and FilePath)

    RStr  strFinalPath (FilePath::ExpandPathURI (parserExpandedUri.AsChar ()));

    //printf ("Loading file \"%s\"\n", strFinalPath.AsChar ());
    EStatus  status = parserCache.ReadFromFile (strFinalPath.AsChar ());
    if (status == EStatus::kSuccess)
      {
      if (!parserCache.IsEmpty ())
        {
        sigOnDataLoad (pLocIn->strTag.AsChar(), pLocIn->strType.AsChar(), parserCache);
        pLocIn->bIsReported = TRUE;
        };
      };
    }
  else if (parserExpandedUri.StartsWith ("mem://"))
    {
    // memory buffer (already stored in Data for content location)
    if (!parserCache.IsEmpty ())
      {
      sigOnDataLoad (pLocIn->strTag.AsChar(), pLocIn->strType.AsChar(), parserCache);
      pLocIn->bIsReported = TRUE;
      };
    }
  else if (parserExpandedUri.StartsWith ("http://") ||
            parserExpandedUri.StartsWith ("https://"))
    {
    // Remote File.  Need to distinguish between RESTful, S3 bucket, etc.
    }
  else
    {
    // Error:  Don't know how to handle this URI
    }


  };

//-----------------------------------------------------------------------------
BOOL ContentDepot::IsEverythingReturned (VOID)
  {
  for (TListItr<ContentLocation*>  itrCurr = listContents.First();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if (! (*itrCurr)->bIsLoaded)  return (FALSE);
    };
  return (TRUE);
  };






//-----------------------------------------------------------------------------
VOID ContentDepot::GetHttpHeader (const char *  szUrlIn,
                                  const char *  szTagIn)
  {
  // start a HEAD call so you can determine if an asset has changed.
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::OnHttpHeaderReceived ()
  {
  // Pull apart the header fields and look for the amazon version.
  //  if not a new version, LoadCachedFile and call OnAsyncLoadComplete(asset).  If a new versio nexists, call GetHttpFile
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::LoadCachedFile (ContentLocation *  pLocIn)
  {
  //load the file if it exists at cache site.  Read bytes into Data field, and clear dirty flag.  Call OnAsyncLoadCOmplete
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::GetHttpFile (const char *  szUrlIn,
                                const char *  szTagIn)
  {
  // start the load
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::OnHttpFileReceived ()
  {
  // find the asset, read the data.  Save data to cache dir.  Save version to meta.
  };










/*
//-----------------------------------------------------------------------------
VOID ContentDepot::RegisterResourceDirectory (const char *  szTagIn,
                                              const char *  szRelPathIn,
                                              const char *  szTypeIn,
                                              const char *  szEnvironmentIn)
  {

  // NOTE:  Removing the registering of directories.  Will just use AddAsset for this.

  // Registers all files by tag for later loading.
  // Add asset for each.  Store URL.


  FilePath::DirTreeSearch (const char *  pszStartingPath,
                                  RegEx         rexSearchOne,
                                  RegEx         rexSearchTwo,
                                  RStrArray &   arrayAllPathsOut);




  RegEx       rexSearch    (szGlobMatchIn, RegEx::EParser::kGlob);
  RegEx       rexSearchTwo ("");
  RStr        strExpandedPath (FilePath::ExpandPathURI (szRelPathIn));
  RStrArray   arrayAllPaths;

  FilePath::DirTreeSearch  (strExpandedPath.AsChar (),
                            rexSearch, rexSearchTwo,
                            arrayAllPaths);

  INT  iNumPaths = arrayAllPaths.Length();
  for (INT  iIndex = 0; iIndex < iNumPaths; ++iIndex)
    {
    printf ("Registering resource %s", arrayAllPaths[iIndex].AsChar ());

    ReadFile (arrayAllPaths[iIndex].AsChar ());

    };
  };

//-----------------------------------------------------------------------------
VOID ContentDepot::RegisterDirectory (const char *  szDirPathIn,
                                      const char *  szFileGlobSpecIn)
  {
  // for files on disk and not in resources
  }
*/


/*
AddAsset (name, type, string/string pair of env and URL)
FindAsset (name)
RemoveAsset (name)
AddDependencies (name, prereq RStrArray of asset names)
AreDependenciesLoaded (latest loaded asset in)
	- Checks its dependencies, and sees if they are loaded.
RegisterResourceDirectory (RelPath, Type, Environment)
	- Registers all files by name for later loading.
	- Add asset for each.  Store URL.
RegisterDirectory (FullPath, FileSpec) for files on disk and not in resources
GetRootDependant () step up dependency tree to find root. assets are in a list, not a tree, so this requires multiple searches.
RunCallbacks(asset) run the RetrieveData call on each asset, dependents first.  If IsEverythingReturned, call OnAllLoaded.
StartAsyncLoad (asset name) Depending on URL prefix, make the call needed to start the download.  res, file, mem, http, https
GetHttpHeader (URL, source ID) start a HEAD call so you can determine if an asset has changed.
OnHttpHeaderReceived () Pull apart the header fields and look for the amazon version.  if not a new version, LoadCachedFile and call OnAsyncLoadComplete(asset).  If a new versio nexists, call GetHttpFile
LoadCachedFile (asset) load the file if it exists at cache site.  Read bytes into Data field, and clear dirty flag.  Call OnAsyncLoadCOmplete
GetHttpFile () start the load
OnHttpFileReceived () find the asset, read the data.  Save data to cache dir.  Save version to meta.
*/












