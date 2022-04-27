/* -----------------------------------------------------------------
                             File Path

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

//-----------------------------------------------------------------------------
//  FilePath
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------
RStrArray  FilePath::ls  (const char *  pszFullPathSpec,
                          BOOL          bShowFiles,
                          BOOL          bShowDirs,
                          BOOL          bFullPaths,
                          EStatus *     pStatusOut)
  {
  RStrArray        arrayOut;

  // Note:  Anytime you store or use a path in TEdit, use UNIX style forward slashes.
  //   However anytime a string is passed to windows, make sure you reverse the slashes
  //   to back-slashes.  You must do any regular expression compares as forward slashes
  //   otherwise you'll wind up escaping characters.


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

  // make sure the directory ends in a slash
  if (strPathOut [strPathOut.Length () -1] != DIRECTORY_SEPARATOR)
    {
    strPathOut += DIRECTORY_SEPARATOR;
    };

  /*
  // if the first part of the filename is a drive letter, remove the opening slash.
  RStr  strDosPathOut = strPathOut;

  if (strDosPathOut * RegEx ("^/?[[:alpha:]]:") != "")
    {
    // found a drive letter
    if (strDosPathOut.GetAt (0) == UINT32 (DIRECTORY_SEPARATOR))
      {
      strDosPathOut.ClipLeft (1);
      };
    };
  */

  // convert to forward slashes so Windows can handle it.
  strPathOut.ReplaceChar (DIRECTORY_SEPARATOR, WIN32_DIRECTORY_SEPARATOR);

  // set a wildcard to search the entire directory.
  RStr  strPathOutWild = strPathOut + "*";


  WIN32_FIND_DATA fdFindFileData;


  HANDLE hFile = FindFirstFile (strPathOutWild.AsChar (), &fdFindFileData);

  if (hFile == INVALID_HANDLE_VALUE)
    {
    // unable to find the first file.
    if (pStatusOut)  *pStatusOut = EStatus (EStatus::kFailure, RStr ("Unable to find first file for ") + strPathOutWild);
    }
  else
    {
    do
      {
      if (fdFindFileData.cFileName [0] != '.')
        {
        RStr  strFullPath = strPathOut + fdFindFileData.cFileName;

        if (GetFileAttributes (strFullPath.AsChar ()) & FILE_ATTRIBUTE_DIRECTORY)
          {
          // directory bit set.  This is a directory
          if (bShowDirs)
            {
            RStr  strPathReturned;

            if (bFullPaths)
              {
              strPathReturned = strFullPath;
              }
            else
              {
              strPathReturned = fdFindFileData.cFileName;
              };
            strPathReturned.ReplaceChar (WIN32_DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);
            arrayOut.Append (strPathReturned);
            };
          }
        else
          {
          // this is a file
          if (bShowFiles)
            {
            // Perform filename glob matching.
            RStr  strFileNameReturned = fdFindFileData.cFileName;
            strFileNameReturned.ReplaceChar (WIN32_DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);


            if (rexFileSpec.Match (strFileNameReturned) == strFileNameReturned)
              {
              // we have a match.
              RStr  strPathReturned;

              if (bFullPaths)
                {
                strPathReturned = strFullPath;
                }
              else
                {
                strPathReturned = strFileNameReturned;
                };
              strPathReturned.ReplaceChar (WIN32_DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);
              arrayOut.Append (strPathReturned);
              };
            };
          };
        }
      else
        {
        /* bypass hidden files and . and .. */;
        };
      } while (FindNextFile (hFile, &fdFindFileData) != 0);

    FindClose (hFile);
    };

  return (arrayOut);
  };


//------------------------------------------------------------------------
EStatus  FilePath::cd  (const RStr &  strDirIn,
                         BOOL          bChangeShellDir)
  {
  RStr             strNewPath;

  if (strDirIn [0] == UINT32 (DIRECTORY_SEPARATOR))
    {
    // absolute directory
    strNewPath = strDirIn;
    }
  else
    {
    // relative path
    strNewPath = strCurrPath + strDirIn;
    };

  // make sure the path ends in a slash

  if (strNewPath [strNewPath.Length () - 1] != UINT32 (DIRECTORY_SEPARATOR))
    {
    strNewPath += DIRECTORY_SEPARATOR;
    };

  // if the first part of the filename is a drive letter, remove the opening slash.
  RStr  strDosPathOut = strNewPath;

  if (strDosPathOut * RegEx ("^/?[[:alpha:]]:") != "")
    {
    // found a drive letter
    if (strDosPathOut.GetAt (0) == UINT32 (DIRECTORY_SEPARATOR))
      {
      strDosPathOut.ClipLeft (1);
      };
    };


  // convert to forward slashes so Windows can handle it.
  strDosPathOut.ReplaceChar (DIRECTORY_SEPARATOR, WIN32_DIRECTORY_SEPARATOR);

  if (GetFileAttributes (strDosPathOut.AsChar ()) & FILE_ATTRIBUTE_DIRECTORY)
    {
    // directory bit set.  This is a directory
    strCurrPath = strNewPath;

    strCurrPath.ReplaceChar (WIN32_DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR);


    return (EStatus::kSuccess);
    };
  return (EStatus::kFailure);
  };


//------------------------------------------------------------------------------
bool FilePath::DirExists (const char *  pszPathIn)
  {
  // Note: May want to strip out double slashes here.
  // to be implemented

  if (GetFileAttributes (pszPathIn) & FILE_ATTRIBUTE_DIRECTORY)
    {
    return (true);
    };
  return (false);
  };


//------------------------------------------------------------------------------
bool FilePath::FileExists (const char *  pszPathIn) const
  {
  // Note:  Ideally we should be using GetFileAttributes to check and see if this
  //   is a file or not, but apparently Windows is broken and can't find the file
  //   on a Linux server.  So we will try to open the file to see if exists.


  FILE *  fp;
  if ((fp = fopen (pszPathIn, "rb")) == NULL)
    {
    return (false);
    };
  fclose (fp);
  return (true);

  //if (GetFileAttributes (pszPathIn) & FILE_ATTRIBUTE_NORMAL)
  //  {
  //  return (true);
  //  };
  //return (false);
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
        CreateDirectory (strCurrPath.AsChar (), NULL);
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
RStr  FilePath::GetCwd  (VOID)
  {
  // to be implemented
  return ("");
  };

//------------------------------------------------------------------------------
UINT32  FilePath::Separator (VOID)
  {
  return (WIN32_DIRECTORY_SEPARATOR);
  };

//------------------------------------------------------------------------------
EStatus  FilePath::ReadFromFile  (const char *     szFilenameIn,
                                  INT              iStartIndexIn,
                                  INT &            iBufferSizeInOut,
                                  unsigned char *  pbyBufferOut)
  {
  /// Reads up to iBufferSize bytes into pbyBufferOut from szFilenameIn starting at offset iStartIndexIn.
  /// Does not do any of its own allocation; only uses the buffer provided.

  FILE *             fp;
  RStr               strFilenameFinal = szFilenameIn;


  // Make sure input values are valid
  //ASSERT (strFilenameFinal != "");
  if (strFilenameFinal == RStr("")) return EStatus (EStatus::kFailure, "FilePath::ReadFromFile () :  Empty filename passed");
  if (iBufferSizeInOut == 0) return EStatus (EStatus::kFailure, "FilePath::ReadFromFile () : Zero length buffer passed.");

  strFilenameFinal.ReplaceChar (DIRECTORY_SEPARATOR, WIN32_DIRECTORY_SEPARATOR);

  fp = fopen (strFilenameFinal.AsChar (), "rb");
  if (fp == NULL)
    {
    RStr  strOut;
    strOut.Format ("FilePath::ReadFromFile - Unable to open file %s", strFilenameFinal.AsChar ());
    return (EStatus (EStatus::kFailure, strOut.AsChar ()));
    };

  fseek (fp, 0, SEEK_END);
  UINT32  uSize = ftell (fp);
  fseek (fp, iStartIndexIn, SEEK_SET);

  iBufferSizeInOut = RMin (iBufferSizeInOut, (INT) uSize);

  if (fread (pbyBufferOut, 1, (UINT) iBufferSizeInOut, fp) != (UINT) iBufferSizeInOut)
    {
    fclose (fp);
    return (EStatus (EStatus::kFailure, "FilePath::ReadFromFile () : Unable to read entire file"));
    };

  fclose (fp);
  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------------
EStatus  FilePath::WriteToFile  (const char *     szFilenameIn,
                                 BOOL             bAppend,
                                 INT              iBytesToWriteIn,
                                 unsigned char *  pbyBufferIn)
  {
  FILE *             fp;
  INT                iBytesToWrite = iBytesToWriteIn;
  RStr               strFilenameFinal = szFilenameIn;


  strFilenameFinal.ReplaceChar (DIRECTORY_SEPARATOR, WIN32_DIRECTORY_SEPARATOR);

  // Make sure input values are valid
  //ASSERT (strFilenameFinal != "");
  if (strFilenameFinal == RStr("")) return EStatus (EStatus::kFailure, "FilePath::WriteToFile () : Empty Filename");
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
  if (fwrite (pbyBufferIn, 1, iBytesToWrite, fp) != (unsigned int) iBytesToWrite)
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
    // TODO: to be implemented
    //strPathOut = filePath_localStoragePath;
    //strPathOut += DIRECTORY_SEPARATOR_STR;
    //strPathOut += &pszPathIn[URI_PREFIX_FILE_LENGTH];
    }

  return (strPathOut.AsChar ());
  };



