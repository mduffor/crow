/* -----------------------------------------------------------------
                             File Path Android

     This module implements cross-platform routines for dealing with
     file and directory paths

                            Michael T. Duffy
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

// NOTE:  This file is conditionally included from FilePath.cpp

// NOTE:  The header guards should avoid double-inclusion here when included from FilePath.cpp
#include "Sys/Types.hpp"
#include "Debug.hpp"
#include "Containers/RStrArray.hpp"
#include "Util/RegEx.hpp"
#include "Sys/FilePath.hpp"


#ifdef ANDROID_NDK
  #define LINUX
  #include <jni.h>
  #include <string.h>
  #include <sys/types.h>
  #include <android/asset_manager.h>
  #include <android/asset_manager_jni.h>
  #include <android/log.h>

  // global variables needed for interfacing with the java native interface
  //   under Android.
  //JNIEnv*  filePath_jniEnv;
  //jobject  filePath_jobjAssetManager;
  AAssetManager*  filePath_assetManager;
  RStr            filePath_localStoragePath;

#endif // ANDROID_NDK

#ifdef LINUX
  #include <sys/stat.h>
  #include <unistd.h>
  #include <dirent.h>
  #include <stdio.h>
#endif // LINUX


//------------------------------------------------------------------------
RStrArray  FilePath::ls  (const char *  pszFullPathSpec,
                          BOOL          bShowFiles,
                          BOOL          bShowDirs,
                          BOOL          bFullPaths,
                          EStatus *     pStatusOut)
  {
  RStrArray        arrayOut;
  //struct stat      statINode;

  if (pStatusOut)  *pStatusOut = EStatus::kSuccess;

  // use the given path, or the current path, or the current directory

  RStr  strPathOut = pszFullPathSpec;
  if (strPathOut.IsEmpty ())
    {
    strPathOut = strCurrPath;
    };
  if (strPathOut.IsEmpty ())
    {
    strPathOut = ".";
    };
  // Note:  Extract any file spec for glob matching.

  RegEx  rexFileSpec ("*", RegEx::kGlob);
  INT32  iFinalSlash = strPathOut.ReverseFindChar (DIRECTORY_SEPARATOR);
  if ((iFinalSlash != -1) && (iFinalSlash != (INT32 (strPathOut.Length ()) - 1)))
    {
    RStr  strFileSpec = strPathOut.ChompRight (strPathOut.Length () - iFinalSlash - 1);

    rexFileSpec.Set (strFileSpec, RegEx::kGlob);
    };

  // make sure the directory does NOT end in a slash
  strPathOut.StripTrailingChar (DIRECTORY_SEPARATOR);

  //DBG_INFO ("Contents of directory: %s", strPathOut.AsChar ());

  AAssetDir *assetDir = AAssetManager_openDir (filePath_assetManager, strPathOut.AsChar ());

  if (assetDir == NULL)
    {
    // Error.  Couldn't open directory.
    RStr  strOut;
    strOut.Format ("FilePath::ls - Unable to open directory %s\n", strPathOut.AsChar ());
    DBG_ERROR (strOut.AsChar ());
    if (pStatusOut)  *pStatusOut = EStatus (EStatus::kFailure, strOut.AsChar ());
    }
  else
    {
    BOOL  bDone = false;
    while (!bDone)
      {
      const char * fName = AAssetDir_getNextFileName (assetDir);

      if (fName != NULL)
        {
        //DBG_INFO ("ls found file %s", fName);
        if (fName [0] != '.')
          {

          RStr  strFullPath = strPathOut + DIRECTORY_SEPARATOR_STR + fName;

          //DBG_INFO (strFullPath.AsChar ());
          if ((bShowFiles) && (rexFileSpec.Match (fName) == fName))
            {
            if (bFullPaths)
              {
              arrayOut.Append (strFullPath);
              }
            else
              {
              arrayOut.Append (fName);
              };
            };
          // Note:  android NDK does not currently allow showing of directories.

          /*
          if (stat (strFullPath.AsChar (), &statINode) != -1)
            {
            if (bShowDirs && S_ISDIR (statINode.st_mode))
              {
              if (bFullPaths)
                {
                arrayOut.Append (strFullPath);
                }
              else
                {
                arrayOut.Append (fName);
                };
              };
            if (bShowFiles && S_ISREG (statINode.st_mode))
              {
              // Perform filename glob matching.
              if (rexFileSpec.Match (fName) == fName)
                {
                // we have a match.
                if (bFullPaths)
                  {
                  arrayOut.Append (strFullPath);
                  }
                else
                  {
                  arrayOut.Append (fName);
                  };
                };
              };
            };
          */
          }
        else
          {
          /* bypass hidden files and . and .. */;
          };
        }
      else
        {
        bDone = true;
        };
      }
    };
  //DBG_INFO ("Done reading directory");
  AAssetDir_close (assetDir);
  return (arrayOut);
  };


//------------------------------------------------------------------------
EStatus  FilePath::cd  (const char *  pszDirIn,
                        BOOL          bChangeShellDir)
  {
  DIR *            dirHandle;
  RStr             strNewPath;

  if (pszDirIn == NULL) {return (EStatus::kFailure);};
  if ((unsigned char) pszDirIn [0] == UINT32 (DIRECTORY_SEPARATOR))
    {
    // absolute directory
    strNewPath = pszDirIn;
    }
  else
    {
    // relative path
    strNewPath = strCurrPath + pszDirIn;
    };

  // make sure the path ends in a slash

  if (strNewPath [strNewPath.Length () - 1] != UINT32 (DIRECTORY_SEPARATOR))
    {
    strNewPath += DIRECTORY_SEPARATOR;
    };

  if ((dirHandle = opendir (strNewPath.AsChar ())) == NULL)
    {
    // Error.  Couldn't open directory.
    return (EStatus::kFailure);
    }

  // valid directory
  closedir (dirHandle);

  strCurrPath = strNewPath;

  if (bChangeShellDir)
    {
    if (chdir (strCurrPath.AsChar ()) != 0)
      {
      return (EStatus::kFailure);
      };
    };

  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------------
bool FilePath::DirExists (const char *  pszPathIn)
  {
  struct stat  statInfo;

  // Note: May want to strip out double slashes here.
  // to be implemented

  if (stat (pszPathIn, &statInfo) == 0)
    {
    if (S_ISDIR (statInfo.st_mode))
      {
      return (true);
      };
    };
  return (false);
  };


//------------------------------------------------------------------------------
bool FilePath::FileExists (const char *  pszPathIn)
  {
  if (strncmp (pszPathIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    AAsset* asset = AAssetManager_open(filePath_assetManager, &pszPathIn[URI_PREFIX_RESOURCE_LENGTH], AASSET_MODE_UNKNOWN);
    if (asset == NULL)
      {
      return (false);
      }
    AAsset_close(asset);
    return (true);
    }
  else if (strncmp (pszPathIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    RStr  strFullPath = filePath_localStoragePath;
    strFullPath += DIRECTORY_SEPARATOR_STR;
    strFullPath += &pszPathIn[URI_PREFIX_FILE_LENGTH];

    FILE* file = fopen(strFullPath.AsChar (), "rb");
    if (file == NULL)
      {
      return (false);
      }
    fclose (file);
    return (true);
    }
  return (false);
  };


//------------------------------------------------------------------------------
UINT  FilePath::GetFileSize  (const char *  pszPathIn)
  {
  UINT     ulFileSize = 0;

  // read from the asset director via the asset manager.

  //AAssetManager* mgr = AAssetManager_fromJava (filePath_jniEnv, filePath_jobjAssetManager);

  if (strncmp (pszPathIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    AAsset* asset = AAssetManager_open(filePath_assetManager, &pszPathIn[URI_PREFIX_RESOURCE_LENGTH], AASSET_MODE_UNKNOWN);
    if (asset == NULL)
      {
      DBG_ERROR ("_ASSET_NOT_FOUND_ : %s", pszPathIn);
      return (UINT(0));
      }

    ulFileSize = AAsset_getLength (asset);
    AAsset_close(asset);
    }
  else if (strncmp (pszPathIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    RStr  strFullPath = filePath_localStoragePath;
    strFullPath += DIRECTORY_SEPARATOR_STR;
    strFullPath += &pszPathIn[URI_PREFIX_FILE_LENGTH];

    FILE* file = fopen(strFullPath.AsChar (), "rb");
    if (file == NULL)
      {
      DBG_ERROR ("_ASSET_NOT_FOUND_ : %s", pszPathIn);
      return (UINT (0));
      }
    fseek (file, 0, SEEK_END);
    ulFileSize = ftell (file);
    fclose (file);
    }

  return (ulFileSize);
  };



//------------------------------------------------------------------------------
VOID FilePath::MakeDir (const char *  pszPathIn)
  {
  if (! DirExists (pszPathIn))
    {
    RStrArray   arrayPath;
    RStr        strCurrPath;

    INT  iNumDirs = SplitPath (pszPathIn, DIRECTORY_SEPARATOR, arrayPath);

    for (INT  iDirIndex = 0; iDirIndex < iNumDirs; ++iDirIndex)
      {
      if ((iDirIndex == 0) && (pszPathIn[0] != DIRECTORY_SEPARATOR))
        {
        // path is relative, so don't start it with a slash.
        strCurrPath += arrayPath [iDirIndex];
        }
      else
        {
        strCurrPath += "/" + arrayPath [iDirIndex];
        };
      if (! DirExists (strCurrPath))
        {
        mkdir (strCurrPath.AsChar (), 0777);
        //printf ("Does not exist %s\n", strCurrPath.AsChar ());
        }
      else
        {
        //printf ("Exists %s\n", strCurrPath.AsChar ());
        };
      };
    };
  };


//------------------------------------------------------------------------------
const char *  FilePath::GetCwd  (VOID)
  {
  static char   szBuffer [512];

  memset (szBuffer, 0, 512);
  if (getcwd (&szBuffer[0], 511) == NULL)
    {
    return (RStr::kEmpty);
    };

  return (&szBuffer[0]);
  };

//------------------------------------------------------------------------------
UINT32  FilePath::Separator (VOID)
  {
  return (DIRECTORY_SEPARATOR);
  };

//------------------------------------------------------------------------------
EStatus  FilePath::ReadFromFile  (const char *     szFilenameIn,
                                  INT              iStartIndexIn,
                                  INT &            iBufferSizeInOut,
                                  unsigned char *  pbyBufferOut)
  {
  /// Reads up to iBufferSize bytes into pbyBufferOut from szFilenameIn starting at offset iStartIndexIn.
  /// Does not do any of its own allocation; only uses the buffer provided.

  if ((szFilenameIn == NULL) || (szFilenameIn[0] == '\0'))  return EStatus (EStatus::kFailure, "FilePath::ReadFromFile () :  Empty filename passed");
  if (iBufferSizeInOut == 0) return EStatus (EStatus::kFailure, "FilePath::ReadFromFile () : Zero length buffer passed.");

  // read from the asset director via the asset manager.

  //AAssetManager* mgr = AAssetManager_fromJava (filePath_jniEnv, filePath_jobjAssetManager);

  if (strncmp (szFilenameIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    AAsset* asset = AAssetManager_open(filePath_assetManager, &szFilenameIn[URI_PREFIX_RESOURCE_LENGTH], AASSET_MODE_UNKNOWN);
    if (asset == NULL)
      {
      __android_log_print(ANDROID_LOG_ERROR, "FilePath", "_ASSET_NOT_FOUND_");
      RStr  strOut ("FilePath::ReadFromFile () : Unable to find asset ");
      strOut += szFilenameIn;
      return EStatus (EStatus::kFailure, strOut.AsChar ());
      }

    long lSize = AAsset_getLength (asset);
    iBufferSizeInOut = RMin (iBufferSizeInOut, lSize);

    long  uLocal = iBufferSizeInOut;
    size_t tBufferSize = size_t(uLocal);
    AAsset_read (asset, pbyBufferOut, iBufferSizeInOut);
    AAsset_close(asset);
    iBufferSizeInOut = INT (tBufferSize);
    }
  else if (strncmp (szFilenameIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    RStr  strFullPath = filePath_localStoragePath;
    strFullPath += DIRECTORY_SEPARATOR_STR;
    strFullPath += &szFilenameIn[URI_PREFIX_FILE_LENGTH];

    FILE* file = fopen(strFullPath.AsChar (), "rb");
    if (file == NULL)
      {
      RStr  strOut ("FilePath::ReadFromFile () : Unable to find file ");
      strOut += szFilenameIn;
      return EStatus (EStatus::kFailure, strOut.AsChar ());
      }

    fseek (file, 0, SEEK_END);
    UINT32  uSize = ftell (file);
    fseek (file, long (int (iStartIndexIn)), SEEK_SET);

    iBufferSizeInOut = RMin (iBufferSizeInOut, (INT) uSize);

    if (fread (pbyBufferOut, 1, (UINT) iBufferSizeInOut, file) != (UINT) iBufferSizeInOut)
      {
      fclose (file);
      return (EStatus (EStatus::kFailure, "FilePath::ReadFromFile () : Unable to read entire file"));
      };
    fclose (file);
    };

  return (EStatus::kSuccess);
  };

//------------------------------------------------------------------------------
EStatus  FilePath::WriteToFile  (const char *     szFilenameIn,
                                 BOOL             bAppend,
                                 INT              iBytesToWriteIn,
                                 unsigned char *  pbyBufferIn)
  {
  FILE *             fp;
  unsigned int       uBytesToWrite = UINT (iBytesToWriteIn);

  if (strncmp (szFilenameIn, URI_PREFIX_RESOURCE, URI_PREFIX_RESOURCE_LENGTH) == 0)
    {
    RStr  strOut ("FilePath::WriteToFile () : Writing to resource path not supported ");
    strOut += szFilenameIn;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    }
  else if (strncmp (szFilenameIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) != 0)
    {
    RStr  strOut ("FilePath::WriteToFile () : File path must have file:// URI prefix ");
    strOut += szFilenameIn;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    }
  RStr  strPath = filePath_localStoragePath;
  strPath += DIRECTORY_SEPARATOR_STR;

  RStr  strFilenameFinal = strPath;
  strFilenameFinal += &szFilenameIn[URI_PREFIX_FILE_LENGTH];

  // Make sure input values are valid
  //ASSERT (strFilenameFinal != "");
  if (strFilenameFinal == RStr("")) return EStatus (EStatus::kFailure, "FilePath::WriteToFile () : Empty Filename");

  // make sure the directory exists.
  MakeDir (strPath.AsChar ());

  if (bAppend)
    {
    fp = fopen (strFilenameFinal.AsChar (), "ab");
    }
  else
    {
    fp = fopen (strFilenameFinal.AsChar (), "wb");
    }
  if (fp == NULL)
    {
    //sprintf (szDbgBuffer, "Unable to open file %s", szFilename); DebugMessage (szDbgBuffer);
    RStr  strOut ("FilePath::WriteToFile () : Unable to open file ");
    strOut += strFilenameFinal;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    };
  if (fwrite (pbyBufferIn, 1, size_t(uBytesToWrite), fp) != uBytesToWrite)
    {
    fclose (fp);

    RStr  strOut ("FilePath::WriteToFile () : Unable to write to file ");
    strOut += strFilenameFinal;
    return EStatus (EStatus::kFailure, strOut.AsChar ());
    };

  fclose (fp);
  return (EStatus::kSuccess);
  };

//------------------------------------------------------------------------------
const char *  FilePath::ExpandPathURI (const char *  szPathIn)
  {
  static RStr        strPathOut;

  strPathOut = szPathIn;

  if (strncmp (szPathIn, URI_PREFIX_FILE, URI_PREFIX_FILE_LENGTH) == 0)
    {
    strPathOut = filePath_localStoragePath;
    strPathOut += DIRECTORY_SEPARATOR_STR;
    strPathOut += &szPathIn[URI_PREFIX_FILE_LENGTH];
    }

  return (strPathOut.AsChar ());
  };









