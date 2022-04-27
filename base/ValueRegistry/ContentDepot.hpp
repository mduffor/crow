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

#ifndef CONTENTDEPOT_HPP
#define CONTENTDEPOT_HPP

#include "Sys/Types.hpp"
#include "Containers/TList.hpp"
#include "Containers/KVPArray.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"
#include "Util/Signal.h"


//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------
using namespace Gallant;


//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

// NOTE:  If looking up dependencies by name becomes too time consuming, you
//  will probably need to cache pointers to ContentLocation alongside the names.
//  You can't replace the names with pointers, because the names may be defined
//  before the associated ContentLocation is registered.

//-----------------------------------------------------------------------------
class ContentLocation
  {
  public:
    RStr       strTag;
    RStr       strType;
    KVPArray   arrayEnvUriPairs;
    RStrArray  astrDependencies; // which items need to be loaded before this one.
    RStrArray  astrDependents;   // which items list this one as a dependency

    BOOL       bIsDirty;     // if data has been invaldiated and needs to be re-[down]loaded
    BOOL       bIsReported;  // if data has been passed to callback
    BOOL       bIsLoaded;  // if we have data from remote systems

    RStrParser  parserCachedData;  //

  public:
                  ContentLocation    ()        {bIsDirty    = TRUE;
                                                bIsReported = FALSE;
                                                bIsLoaded = FALSE;};

    RStrParser &  GetCachedData      (const char *  szEnvIn)   {return (parserCachedData);};

    const char *  GetUri             (const char *  szEnvIn);

  };


//-----------------------------------------------------------------------------
class ContentDepot
  {
  protected:

  public:
    TList<ContentLocation*>  listContents;

    RStr                     strEnvironment;

    KVPArray                 arrayEnvVars;

    /// sigOnDataLoad (pszName, pszType, &parserData)
    Signal3<const char *, const char *, const RStrParser&>  sigOnDataLoad;

    Signal0<>  sigOnAllLoaded;


  public:

                       ContentDepot          ();

    virtual            ~ContentDepot         ();

    VOID               SetEnvironment        (const char *  szEnvIn)   {strEnvironment.Set(szEnvIn, TRUE);};

    ContentLocation *  AddAsset              (const char *  szTagIn,
                                              const char *  szTypeIn,
                                              const char *  szEnvIn,
                                              const char *  szUrlIn,
                                              RStr *        pstrBufferIn = NULL);

    ContentLocation *  FindAsset             (const char *  szTagIn);

    VOID               RemoveAsset           (const char *  szTagIn);

    VOID               RemoveAllAssets       (VOID);

    INT                Size                  (VOID)   {return (listContents.Size ());};

    VOID               AddDependency         (const char *  szTagIn,
                                              const char *  szDepTagIn);

    VOID               AddDependencies       (const char *  szTagIn,
                                              RStrArray &   arrayDepTagsIn);

    VOID               RecalcDependents      (VOID);


    BOOL               AreDependenciesLoaded (ContentLocation *  pLocIn);

    VOID               GetRootDependants     (ContentLocation *           plocIn,
                                              TList<ContentLocation *> &  listOut);

    VOID               Refresh               (BOOL   bForceRefreshIn);

    /*
    VOID               RegisterResourceDirectory (const char *  szTagIn,
                                    const char *  szRelPathIn, // path with filename/glob
                                    const char *  szTypeIn,
                                    const char *  szEnvironmentIn)
    //- Registers all files by tag for later loading.
    //- Add asset for each.  Store URL.

    VOID RegisterDirectory (const char *  szDirPathIn,
                            const char *  szFileGlobSpecIn)
    //for files on disk and not in resources
    */

    VOID               RunCallbacks          (ContentLocation *  pLocIn);

    VOID               LoadAsset             (const char *       szTagIn);

    VOID               StartAsyncLoad        (const char *       szTagIn);

    VOID               StartAsyncLoad        (ContentLocation *  pLocIn);

    VOID               OnAsyncLoadComplete   (const char *       szTagIn);

    VOID               OnAsyncLoadComplete   (ContentLocation *  pLocIn);

    VOID               RetrieveData          (ContentLocation *  pLocIn,
                                              const char *       szEnvIn);

    BOOL               IsEverythingReturned  (VOID);


    VOID               GetHttpHeader         (const char *       szUrlIn,
                                              const char *       szTagIn);

    VOID               OnHttpHeaderReceived  ();

    VOID               LoadCachedFile        (ContentLocation *  pLocIn);

    VOID               GetHttpFile           (const char *  szUrlIn,
                                              const char *  szTagIn);

    VOID               OnHttpFileReceived    ();



  };



#endif // CONTENTDEPOT_HPP
