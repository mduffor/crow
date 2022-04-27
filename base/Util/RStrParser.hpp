/* -----------------------------------------------------------------
                           String Parser Class

    The RStrParser class is used to create a RStr that has member functions
   for extracting, parsing, and manipulating the contents of the string.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2002-2014, Michael T. Duffy II.  All rights reserved.
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

#ifndef RSTRPARSER_HPP
#define RSTRPARSER_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RegEx.hpp"
#include "Sys/FilePath.hpp"

/**
  @addtogroup base
  @{
*/
//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// RStrParser
//------------------------------------------------------------------------

/**
    The RStrParser class adds text parsing abilities and cursor tracking to the RStr class.
*/
//------------------------------------------------------------------------
class RStrParser : public RStr
  {
  public:

    enum  ECommentType    {kNone    = 0,     ///< Skipping comments is off
                           kCStyle  = 1,     ///< C-style comments  //
                           kShellStyle  = 2  ///< Shell-style comments  #
                          };

  private:

                   /// If buffered writing is set to true, then every time iBufferedWriteSize bytes are placed in the parser, the contents are cleared and written to disk.  The memory buffer is then reset to the start.
    BOOL           bBufferedWriting;
                   /// If buffered writing is set to true, this is the file the buffer is written to.
    RStr           strBufferedFilename;
                   /// The size the buffer will grow to before it is flushed out to disk.
    INT32          iBufferedWriteSize;

                   /// index of the character the cursor is on.
    INT32          iCursorStart;

                   /// index of the last character in the selection exclusive
    INT32          iCursorEnd;

                   /// index of the last character in the string, exclusive
    INT32          iLineEnd;

                   /// True if EOL comments affect the calculation of iLineEnd, false if they don't.
    ECommentType   eSkipComments;

                   /// True if GetQuoteString ignores spaces and reads to end of line, False if whitespace terminates the read.
    BOOL           bGreedyRead;

  public:

                          /// Constant escape character values.
    static const UINT32   kNULL;          ///< The null character 0x00
    static const UINT32   kLF;            ///< The line feed character 0x0a
    static const UINT32   kCR;            ///< The carriage return character 0x0d
    static const UINT32   kSPACE;         ///< The space character 0x20
    static const UINT32   kTAB;           ///< The tab character 0x09
    static const UINT32   kDOUBLEQUOTES;  ///< The double quote (") character 0x22
    static const UINT32   kSINGLEQUOTES;  ///< The single quote (') character 0x27
    static const UINT32   kESCAPE;        ///< The escape character 0x5c


                             /** @brief Constructor
                                 @return None
                             */
             RStrParser      ();

                             /** @brief Constructor
                                 @return None
                             */

             RStrParser      (const RStr &   strIn);  // cppcheck-suppress noExplicitConstructor

                             /** @brief Constructor
                                 @return None
                             */
    explicit RStrParser      (const RStrParser &   parserIn);

                             /** @brief Constructor
                                 @return None
                             */
                             // cppcheck-suppress noExplicitConstructor
             RStrParser      (const char *   pszIn);

                             /** @brief Destructor
                                 @return None
                             */
    virtual  ~RStrParser     ();

                             /** @brief Initializes internal variables.  Called from the constructors.
                                 @return None
                             */
    VOID     Init            (VOID);

                             /** @brief Resets the cursor to the start of the line
                                 @return None
                             */
    VOID     ResetCursor     (VOID);

                             /** @brief Calculates the end of the current line, taking EOL comments into consideration if needed.
                                 @return None
                             */
    VOID     FindLineEnd     (VOID);


                             /** @brief Locate the first occurance of the given string, searching forwards.
                                 @param strIn The string to search for.
                                 @return The zero-based index of the found character, or -1 if the character is not found.
                             */
    INT32    FindInLine      (const RStr &  strIn) const;

                             /** @brief Locate the first occurance of the given string, searching forwards.
                                 @param szIn The string to search for.
                                 @return The zero-based index of the found character, or -1 if the character is not found.
                             */
    INT32    FindInLine      (const char *  szIn) const;

                             /** @brief Returns the position within the buffer where the next line end has been calculated to fall.  The position returned is the first character not part of the line (usually an EOL character).
                                 @return None
                             */
    INT32    GetLineEnd      (VOID)     {return iLineEnd;};

                             /** @brief Returns the length of the current line, with the start of the line defined by the current cursor position.
                                 @return The length of the line.
                             */
    INT32    GetLineLength   (VOID)     {return (iLineEnd - iCursorStart);};

    INT32    GetLineNumber   (INT32  iCursorPosIn = -1);


                             /** @brief Assignment operator.  Copies the contents of the given string to this string.
                                 @param strIn The string that will be copied.
                                 @return A reference to this string.
                             */
    RStr &  Set              (const RStr &  strIn,
                              BOOL          bCalcHashIn = FALSE) override;
    RStr &  _ParserSet       (const RStr &  strIn,
                              BOOL          bCalcHashIn = FALSE);

                             /** @brief Assignment operator.  Copies the contents of the given zero terminated character array to this string.
                                 @param pszIn The character array that will be copied.
                                 @return A reference to this string.
                             */
    RStr &  Set              (const char *  pszIn,
                              BOOL          bCalcHashIn = FALSE) override;
    RStr &  _ParserSet       (const char *  pszIn,
                              BOOL          bCalcHashIn = FALSE);


                             /** @brief Returns whether or not the passed character value is considered whitespace or not.
                                 @param uCharIn The character to check.
                                 @return True if the character is whitespace, False if it is not.
                             */
    BOOL     IsWhitespace    (UINT32  uCharIn)           {return ((uCharIn == kSPACE) || (uCharIn == kTAB));};

                             /** @brief Moves the current cursor position forwards past any whitespace characters it is in.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters as well as whitespace characters.
                                 @return Number of whitespace characters skipped.
                             */
    INT      SkipWhitespace  (BOOL  bSkipEOL = TRUE);

                             /** @brief Moves the current cursor position past the current EOL marker it is on.
                                 @return None
                             */
    VOID     SkipEOL         (VOID);

                             /** @brief Returns whether or not the character at the given position is an "end of line" (EOL) character or not.  End of file counts as EOL.
                                 @param iPosition The position of the character to check.  If it is -1, then the current cursor position is used.
                                 @return True if the character is an EOL character, False if it is not.
                             */
    BOOL     IsEOL           (INT32  iPosition = -1);

                             /** @brief Returns whether or not the character at the given position is an "end of file" (EOF) character or not
                                 @param iPosition The position of the character to check.  If it is -1, then the current cursor position is used.
                                 @return True if the character is an EOF character, False if it is not.
                             */
    BOOL     IsEOF           (INT32  iPosition = -1);

                             /** @brief Returns whether or not the character at the given position is an "end of file" (EOF) character or not.  This version is meant for ASCII files because it also checks if the current character is a NULL character (0x00)
                                 @param iPosition The position of the character to check.  If it is -1, then the current cursor position is used.
                                 @return True if the character is an EOF character, False if it is not.
                             */
    BOOL     IsEOFAscii      (INT32  iPosition = -1);

                                     /** @brief Returns the current position of the cursor, zero based.
                                         @return The current position of the cursor.
                                     */
    INT32         GetCursorStart     (VOID) const     {return iCursorStart;};


                                     /** @brief Sets the current position of the cursor
                                         @param iPosIn  The position to move the cursor to, zero based.
                                         @return None
                                     */
    VOID          SetCursorStart     (INT32  iPosIn)       {iCursorStart = RMax (0, RMin (iPosIn, (INT32) uStringLength));};

                                     /** @brief Returns a pointer into the buffer at the current position of the cursor.
                                         @return A pointer to the character at the current cursor position.
                                     */
    const char *  GetCursorStartPtr  (VOID)                {return &(pszBuffer [iCursorStart]);};


                                     /** @brief Deletes the string, setting its length to zero.  The internal buffer remains allocated.
                                         @return None
                                     */
    VOID          Empty              (VOID) override       {RStr::Empty ();  ResetCursor ();};

                                     /** @brief Deletes the string, setting its length to zero.  The internal buffer is also discarded.
                                         @return None
                                     */
    VOID          Reset              (VOID) override       {RStr::Reset ();  ResetCursor ();};

                             /** @brief Moves the cursor position forward by the given amount.
                                 @return The current position of the cursor.
                             */
    INT32    SkipChars       (UINT32  uNumCharsToSkip = 1)  {iCursorStart += uNumCharsToSkip; iCursorStart = RMin (iCursorStart, INT32 (Length ()) ); return iCursorStart;};

    BOOL     SkipToChar      (UINT32  uCharToSkipIn);

    BOOL     SkipPastChar    (UINT32  uCharToSkipPastIn,
                              BOOL    bSkipWhitespaceIn = TRUE);

                             /** @brief Moves the current cursor position to the end of the current line.
                                 @return None
                             */
    VOID     GotoEOL         (VOID);

                             /** @brief Moves the current cursor position to the start of the next line.
                                 @return True if there is a next line to go to, False if the cursor is at the end of the file.
                             */
    BOOL     GotoNextLine    (VOID);

                             /** @brief Get the characters before the next end of line marker, and move the current cursor to the start of the next line.
                                 @return A string containing the line that was read.
                             */
    RStr     GetLine         (VOID);

                             /** @brief Get the characters before the next end of line marker, and move the current cursor to the start of the next line.
                                 @return None
                             */
    VOID     GetLine         (RStr &  strOut);

                             /** @brief Get the next word (run of characters without embedded whitespace) from the buffer.  Moves the cursor to the following word.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the word.
                                 @param szAlsoInvalidChars A null terminated list of other characters to not consider as part of a word.
                                 @return A string containing the characters that were read.
                             */
    RStr &   GetWord         (BOOL          bSkipEOL = TRUE,
                              const char *  szAlsoInvalidChars = NULL);

                             /** @brief Get the next word (run of characters without embedded whitespace) from the buffer.  Moves the cursor to the following word.
                                 @param strOut A string into which the read characters will be written.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the word.
                                 @param szAlsoInvalidChars A null terminated list of other characters to not consider as part of a word.
                                 @return None
                             */
    const char *  GetWord    (RStr &        strOut,
                              BOOL          bSkipEOL = TRUE,
                              const char *  szAlsoInvalidChars = NULL);

                             /** @brief Get the next word (run of characters without embedded whitespace) from the buffer.  Moves the cursor to the following word.
                                 @param pstrOut A string into which the read characters will be written.  If NULL, no chars will be read but the word will be skipped.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the word.
                                 @param szAlsoInvalidChars A null terminated list of other characters to not consider as part of a word.
                                 @return None
                             */
    const char *  GetWord    (RStr *        pstrOut,
                              BOOL          bSkipEOL = TRUE,
                              const char *  szAlsoInvalidChars = NULL);


                             /** @brief Get the next alphanum word (run of A-Za-z0-9_ without embedded whitespace) from the buffer.  Moves the cursor to the following word.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the word.
                                 @return A string containing the characters that were read.
                             */
    RStr     GetAlphaNum     (BOOL  bSkipEOL = TRUE);

                             /** @brief Get the next word (run of A-Za-z0-9_ without embedded whitespace) from the buffer.  Moves the cursor to the following word.
                                 @param strOut A string into which the read characters will be written.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the word.
                                 @return None
                             */
    VOID     GetAlphaNum     (RStr &  strOut,
                              BOOL    bSkipEOL = TRUE);

                             /** @brief Get the next word (run of A-Za-z0-9_ without embedded whitespace) from the buffer.  Moves the cursor to the following word.
                                 @param pstrOut A string into which the read characters will be written.  If NULL, no chars will be read but the word will be skipped.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the word.
                                 @return None
                             */
    VOID     GetAlphaNum     (RStr *  pstrOut,
                              BOOL    bSkipEOL = TRUE);



                             /** @brief Get the next character from the buffer.  Moves the cursor to the following character.
                                 @return The character that was read.
                             */
    UINT32    GetChar        (BOOL  bSkipWhitespace = TRUE);


                             /** @brief Get the next integer from the buffer.  Moves the cursor to the following word.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the integer.
                                 @return The integer that was read.
                             */
    INT32    GetInt          (BOOL  bSkipEOL = TRUE);
    BOOL     IsInt           (BOOL  bSkipEOL = TRUE);

                             /** @brief Get the next unsigned integer from the buffer.  Moves the cursor to the following word.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the unsigned integer.
                                 @return The unsigned integer that was read.
                             */
    UINT32   GetUInt         (BOOL  bSkipEOL = TRUE);
    BOOL     IsUInt          (BOOL  bSkipEOL = TRUE);

                             /** @brief Get the next floating point value from the buffer.  Moves the cursor to the following word.
                                 @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the float.
                                 @param bFractionalRequired If true, then the float must have a fractional part.  This is to force distinctions between floats and ints.  i.e. 3.0  is a float, 3 is an int.
                                 @return The floating point value that was read.
                             */
    FLOAT    GetFloat        (BOOL  bSkipEOL = TRUE);
    BOOL     IsFloat         (BOOL  bSkipEOL = TRUE,
                              BOOL  bFractionalRequired = FALSE);

                             /** @brief Get the next double-precision floating point value from the buffer.  Moves the cursor to the following word.
                                 @return The double-precision floating point value that was read.
                             */
    DOUBLE   GetDouble       (BOOL  bSkipEOL = TRUE);

    VOID     IncIntAtCursor   (INT    iMultiplierIn);
    VOID     IncFloatAtCursor (FLOAT  fMultiplierIn);


                                  /** @brief Get the next string in quotes.  The quotes will not be read, and escaped characters will be un-escaped.  If the first available character is not a double quote, then the string is read to the end of the line.  Moves the cursor to the following word.
                                      @param pstrOut The returned string, quotes removed.
                                      @param bSkipEOL Indicates if the cursor should skip over EOL characters both before and after reading the string.
                                      @param bCalcHash After setting the string, calculate and store the hash for that string.
                                      @return None.
                                  */
    const char *  GetQuoteString  (RStr *  pstrOut,
                                   BOOL    bSkipEOL       = TRUE,
                                   BOOL    bCalcHashIn    = FALSE,
                                   BOOL    bReplaceString = TRUE);

    const char *  GetBracketString  (RStr *        pstrOut,
                                     const char *  szOpenBracketChars = NULL,
                                     const char *  szCloseBracketChars = NULL,
                                     BOOL          bSkipEOL = TRUE,
                                     BOOL          bUnescapeChars = TRUE,
                                     BOOL          bCalcHashIn = FALSE);


                             /** @brief Add double quote marks before and after string.  Also escape all double quotes and backslashes with a backslash.
                                 @return None.
                             */
    VOID     MakeQuoteString (VOID);

                             /** @brief Escape all double quotes, backslashes, and escape characters with a backslash.  Add double quotes to the string if requested.
                                 @param  bAddQuotes  Add double quotes before and after the escaped string.
                                 @return None.
                             */
    VOID     MakeEscapedString (BOOL   bAddQuotes);

                             /** @brief Sets the status of greedy reading.  When Greedy Read is on, GetQuoteString will ignore spaces and read to the end of the line if quotes are not present.  If GreedyRead is off, a space will terminate GetQuoteString if quotes are not present.
                                 @param bGreedyReadIn  Whether to turn greedy reading on (true) or off (false).
                                 @return None.
                             */
    VOID     SetGreedyRead   (BOOL  bGreedyReadIn)        {bGreedyRead = bGreedyReadIn;};

                             /** @brief Gets the status of greedy reading.  When Greedy Read is on, GetQuoteString will ignore spaces and read to the end of the line if quotes are not present.  If GreedyRead is off, a space will terminate GetQuoteString if quotes are not present.
                                 @return The current greedy read status.
                             */
    BOOL     GetGreedyRead   (VOID)                       {return bGreedyRead;};

                             /** @brief Reads the character at the given position without affecting the current cursor position.
                                 @param iPosition The position of the character to look at.  If the value is -1, then the current cursort position is used.
                                 @return The character that was read.
                             */
    UINT32   PeekChar        (INT32  iPosition = -1);

                             /** @brief Copies a given number of characters to a provided buffer.
                                 @param pszBufferOut The buffer that the read characters will be copied into.
                                 @param iCharsToCopy The number of characters to copy.  If the remaining characters are fewer than the requested number, only the remaining characters will be copied.
                                 @return Returns the number of characters actually copied.
                             */
    UINT32   ReadChars       (PCHAR   pszBufferOut,
                              INT32   iCharsToCopy);

                             /** @brief Copies a given number of characters to a provided buffer.
                                 @param strOut The string that the read characters will be copied into.
                                 @param iCharsToCopy The number of characters to copy.  If the remaining characters are fewer than the requested number, only the remaining characters will be copied.
                                 @return Returns the number of characters actually copied.
                             */
    UINT32   ReadChars       (RStr &  strOut,
                              INT32   iCharsToCopy);

                             /** @brief Appends iCharsToCopy from pszBufferIn to the end of the parser string.
                                 @param pszBufferIn The buffer that the characters will read from.
                                 @param iCharsToCopy The number of characters to append.
                                 @return None
                             */
    VOID     WriteChars      (PCHAR    pszBufferIn,
                              INT32    iCharsToCopy);

                             /** @brief Removes comment text from the buffer.  This operation changes the contents of the buffer.
                                 @return None
                             */
    VOID     StripComments   (VOID);

                             /** @brief Reads the contents of the given file, and places them in the internal buffer.
                                 @param szFilenameIn The full path to the file to read in.
                                 @param iStartIndex The zero-based offset into the file from which reading should begin.
                                 @param iMaxBytesToRead Maximum number of bytes to read in from the file.  Used to read headers of files rather than the entire thing.
                                 @return None
                             */
    EStatus  ReadFromFile    (const char *  szFilenameIn,
                              INT           iStartIndex = 0,
                              INT           iMaxBytesToRead = -1);

                             /** @brief Writes the contents of the buffer to the given file.
                                 @param szFilenameIn The full path to the file to write to.
                                 @param iStartOffsetIn The zero-based offset into the string where the write operation will begin.  Defaults to 0.
                                 @param iBytesToWriteIn The number of bytes to write to disk.  The default value is -1, which indicates the entire buffer should be written.
                                 @return None
                             */
    EStatus  WriteToFile     (const char * szFilenameIn,
                              INT          iBytesToWriteIn = -1);


                             /** @brief Set whether or not comments should be ignored during processing.
                                 @param eStatusIn True to ignore the comment text, false to consider it.
                                 @return None
                             */
    VOID     SetSkipComments (ECommentType  eStatusIn)      {eSkipComments = eStatusIn;};

                             /** @brief Read whether or not comments are ignored during processing.
                                 @return kCStyle if C-style (double backslash) comment text is ingored, kShellStyle if shell style (pound sign) comment text is ignored, and kNone if text is considered.
                             */
    ECommentType   GetSkipComments (VOID)                         {return eSkipComments;};


                                      /** @brief Moves iCursorStart forward to skip any comments in the currently set commenting style.
                                          @return kSuccess if it was able to move iCursorStart, kFailure if it was unable to do so.
                                      */
    EStatus  SkipComment              (VOID);
                                      /** @brief Return whether or not the characters at the given index denote the start of a comment.
                                          @param iIndexIn Index into buffer where comment tags will be looked for.
                                          @return True if a comment starts at iIndexIn, false if not.
                                      */
    BOOL     IsComment                (INT  iIndexIn);
                                      /** @brief Return whether or not the characters at the given index denote the start of an End-Of-Line comment.
                                          @param iIndexIn iIndexIn into buffer where comment tags will be looked for.
                                          @return True if a comment starts at iIndexIn, false if not.
                                      */
    BOOL     IsEOLComment             (INT  iIndexIn);
                                      /** @brief Return whether or not the characters at the given index denote the start of a block comment.
                                          @param iIndexIn Index into buffer where comment tags will be looked for.
                                          @return True if a comment starts at iIndexIn, false if not.
                                      */
    BOOL     IsBlockComment           (INT  iIndexIn);
                                      /** @brief Return whether or not the characters at the given index denote the end of a block comment.
                                          @param iIndexIn Index into buffer where comment tags will be looked for.
                                          @return True if a comment ends at iIndexIn, false if not.
                                      */
    BOOL     IsBlockCommentEnd        (INT  iIndexIn);
                                      /** @brief Returns the number of characters that make up a block comment tag.  Used to unify code handling of different commenting styles.
                                          @return number of characters in the block comment tag
                                      */
    INT      GetBlockCommentTagLength (VOID);


                             /** @brief Scans the digits at the end of the current line, and returns them as an integer value
                                 @param pstrDigits If not NULL, the digits that were read will be put in pstrDigits, including any leading zeroes.
                                 @return The value of the trailing digits.  If no trailing digits were found, a value of zero will be returned.
                             */
    UINT32   GetTrailingUInt (RStr *  pstrDigits = NULL);


                             /** @brief This routine breaks its contents into an array of strings, using the uSeparatorIn character to determine where one string ends and the next one begins.
                                 @param uSeparatorIn The character that separates the strings.
                                 @param arrayOut  The returned array of strings.  The Separator character will have been stripped out.
                                 @param bStripWhitespace Sets whether opening and trailing whitespace characters should be stripped out from the returned strings.  Defaults to true.
                                 @param bSkipEOL Sets whether EOL characters should stop the scanning of elements to parse.  False means EOL will stop scanning, True means scanning will continue to end of buffer.
                                 @param bKeepEmptyStrings True signals that empty strings are placed in the arrayOut object, False signals that empty strings will be thrown out.  An empty string occurs when no characters are found between two separator characters.
                                 @return The number of strings found
                             */
    UINT32   Tokenize        (UINT32       uSeparatorIn,
                              RStrArray &  arrayOut,
                              BOOL         bStripWhitespace  = true,
                              BOOL         bSkipEOL          = true,
                              BOOL         bKeepEmptyStrings = false);

                                   /** @brief Combines the elements of a string array into a new string, separated by a given character.  Used to undo a Tokenize.
                                       @param uSeparatorIn The character to place between each string.
                                       @param arrayIn The array of strings to join together
                                       @return None.
                                   */
    VOID     Join                  (UINT32       uSeparatorIn,
                                    RStrArray &  arrayIn);


                                   /** @brief Remove any whitespace characters from the end of the string.
                                       @param bStripEOL Signals whether newline and carriage return characters are considered whitespace and removed.
                                       @return The number of characters removed.
                                   */
    //VOID     StripTrailingWhitespace (BOOL  bStripEOL = false);


    BOOL     StartsWith            (const char *  szIn,
                                    INT           iOffsetIn = -1)   {return RStr::StartsWith (szIn, iOffsetIn == -1 ? iCursorStart : 0);};

    BOOL     StartsWith            (UINT32        uIn,
                                    INT           iOffsetIn = -1)   {return RStr::StartsWith (uIn, iOffsetIn == -1 ? iCursorStart : 0);};

                                   /** @brief Assignment operator.  Copies the contents of the given string to this string.
                                       @param parserIn The string that will be copied.
                                       @return A reference to this string parser.
                                   */
    RStrParser &        operator=  (const RStrParser &  parserIn);

                                   /** @brief Assignment operator.  Copies the contents of the given string to this string.
                                       @param strIn The string that will be copied.
                                       @return A reference to this string.
                                   */
                                   // NOTE: This intentionally hides the RStr::operator= function because the return types differ.
    RStrParser &        operator=  (const RStr & strIn);

                                   /** @brief Assignment operator.  Copies the contents of the given zero terminated character array to this string.
                                       @param pszIn The character array that will be copied.
                                       @return A reference to this string.
                                   */
                                  // NOTE: This intentionally hides the RStr::operator= function because the return types differ.
    RStrParser &        operator=  (const char *  pszIn);

    void                SetStrParser  (const RStrParser &  parserIn)    {*this = parserIn;};
    void                SetRStr       (const RStr & strIn)              {*this = strIn;};
    void                SetCharPtr    (const char *  pszIn)             {*this = pszIn;};

                                   /** @brief Counts the number of words in the string, starting at the cursor position
                                       @param bStopAtEOL Specifies whether to stop the count when an end of line is reached.
                                       @return The number of words.
                                   */
    INT32    CountWords            (bool  bStopAtEOL = true);


                                   /** @brief Turns buffered writing on or off.  When buffered writing is on, then the parser will append the contents of the buffer to a file on disk and reset the cursor to zero.  Be certain to call FlushBuffer at the end to make sure any remaining data in memory has been written to disk.
                                       @param bStatusIn True to turn on buffered writing, false to turn it off.  Default when object is initialized is false.
                                       @return None
                                   */
    VOID     SetBufferedWriting (BOOL  bStatusIn)            {bBufferedWriting = bStatusIn;};

                                   /** @brief  Queries whether buffered writing is on or off.
                                       @return True if buffered writing is on, False if it is off.
                                   */
    BOOL     GetBufferedWriting (VOID)                       {return (bBufferedWriting);};

                                   /** @brief Sets the filename that buffered writing will append to.  Note that for overwriting, you must delete the files yourself since buffered writing only appends, it doesn't overwrite.
                                       @param strFilenameIn  The filename to append buffered writes to.
                                       @return None
                                   */
    VOID     SetBufferedFilename  (const RStr &  strFilenameIn)    {strBufferedFilename = strFilenameIn;};

                                   /** @brief Gets the filename that buffered writing will append to.
                                       @return The filename to append buffered writes to.
                                   */
    RStr     GetBufferedFilename  (VOID)                     {return strBufferedFilename;};

                                   /** @brief If buffered writing is on, then this value determines the size the buffer can grow to before it is written out to disk (when buffered writing is on.)
                                       @param  iBufferSizeIn  The size that the buffer is allowed to grow to before writing out to disk.
                                       @return None
                                   */
    VOID     SetBufferedWriteSize  (INT32  iBufferSizeIn)    {iBufferedWriteSize = iBufferSizeIn;};

                                   /** @brief Queries the size the buffer can grow to before being written out to disk (when buffered writing is on.)
                                       @return The size that the buffer is allowed to grow to before writing out to disk.
                                   */
    INT32    GetBufferedWriteSize  (VOID)                    {return iBufferedWriteSize;};

                                   /** @brief For buffered writing, this flushes any remaining data in memory out to disk.  This routine should be called after all data has been written to the buffer just to make sure it is saved out.
                                       @return The success or failure of the operation
                                   */
    EStatus  FlushBuffer           (VOID);


                                 /** @brief Increases the size of the internal buffer accept a string of the given length, if needed.  If the buffer is currently large enough, no change is made.
                                     @param uSizeIn Number of additional characters past the current string length the buffer should be allocated to hold, not including the terminating null.
                                     @return None
                                 */
    VOID     Grow                (UINT32  uSizeIn) override;

                                 /** @brief Increases the size of the internal buffer accept a string of the given length, if needed.  If the buffer is currently large enough, no change is made.
                                     @param uSizeIn Absolute number of characters the buffer should be allocated to hold, not including the terminating null.
                                     @return None

                                 */
    VOID     GrowAbsolute        (UINT32  uSizeIn) override;

    BOOL     GetNextEnvVar       (RStr &  strVarNameOut,
                                  RStr *  pstrFullTagOut = NULL);

    // Binary data extraction.  Little Endian is PC format.  Big Endian is network format.
                            /** @brief Parses from the string a 4-byte float in Little Endian ordering.  The cursor is moved past the read data upon completion.
                                @return The parsed float
                            */
    DOUBLE   GetF8_LEnd    (VOID);
    FLOAT    GetF4_LEnd    (VOID);
                            /** @brief Parses from the string a 4-byte unsigned integer in Little Endian ordering.  The cursor is moved past the read data upon completion.
                                @return The parsed unsigned integer
                            */
    UINT64   GetU8_LEnd    (VOID);
    UINT32   GetU4_LEnd    (VOID);


    UINT32   GetU2_LEnd    (VOID);
    UINT32   GetU1_LEnd    (VOID);

    DOUBLE   GetF8_BEnd    (VOID);
    FLOAT    GetF4_BEnd    (VOID);
    UINT64   GetU8_BEnd    (VOID);
    UINT32   GetU4_BEnd    (VOID);
    UINT32   GetU2_BEnd    (VOID);
    UINT32   GetU1_BEnd    (VOID);

    VOID     SetF8_LEnd    (DOUBLE  dIn);
    VOID     SetF4_LEnd    (FLOAT   fIn);
    VOID     SetU8_LEnd    (UINT64  uIn);
    VOID     SetU4_LEnd    (UINT32  uIn);
    VOID     SetU2_LEnd    (UINT32  uIn);
    VOID     SetU1_LEnd    (UINT32  uIn);

    VOID     SetF8_BEnd    (DOUBLE  dIn);
    VOID     SetF4_BEnd    (FLOAT   fIn);
    VOID     SetU8_BEnd    (UINT64  uIn);
    VOID     SetU4_BEnd    (UINT32  uIn);
    VOID     SetU2_BEnd    (UINT32  uIn);
    VOID     SetU1_BEnd    (UINT32  uIn);

    VOID     GetData       (unsigned char *  pbyDataOut,
                            INT32            iDataSizeIn);

    VOID     GetData       (RStr *           pstrDataOut,
                            INT32            iDataSizeIn);

    VOID     SetData       (const unsigned char *  pbyDataIn,
                            INT32                  iDataSizeIn);

    VOID     GetDataStr    (RStr *           pstrDataOut);

    VOID     SetDataStr    (const RStr &     strDataIn);

    VOID     DebugPrint    (const char *  szNameIn);

  };

typedef RStrParser *  PRStrParser;

RStr  operator*  (const RStrParser &  strIn,   RegEx &            regexIn);
RStr  operator*  (RegEx &            regexIn, const RStrParser &  strIn);
/** @} */ // end of base group

#endif // RSTRPARSER_HPP

