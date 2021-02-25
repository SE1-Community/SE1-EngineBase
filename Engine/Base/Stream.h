/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef SE_INCL_STREAM_H
#define SE_INCL_STREAM_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <Engine/Base/Translation.h>
#include <Engine/Base/Lists.h>
#include <Engine/Base/CTString.h>
#include <Engine/Base/FileName.h>
#include <Engine/Base/ErrorReporting.h>
#include <Engine/Templates/DynamicStackArray.h>

// Maximum length of file that can be saved (default: 8Mb)
ENGINE_API extern ULONG _ulMaxLengthOfSavingFile;

#define CTSTREAM_BEGIN \
  CTStream::EnableStreamHandling(); \
  __try

#define CTSTREAM_END \
  __except (CTStream::ExceptionFilter(GetExceptionCode(), GetExceptionInformation())) { \
    CTStream::ExceptionFatalError(); \
  }; \
  CTStream::DisableStreamHandling();

// Chunk ID class
#define CID_LENGTH 4
class ENGINE_API CChunkID {
  public:
    char cid_ID[CID_LENGTH + 1];

  public:
    // Default constructor and string constructor
    inline CChunkID(const char *strString = "    ");

    // Comparison operator
    inline int operator==(const CChunkID &cidOther) const;
    inline int operator!=(const CChunkID &cidOther) const;

    inline operator const char *(void) const {
      return cid_ID;
    }

    inline operator char *(void) {
      return cid_ID;
    }
};

// Inline implementations

// Default constructor and string constructor
inline CChunkID::CChunkID(const char *strString /*= "    "*/) {
  ASSERT(strlen(strString) == CID_LENGTH);
  memcpy(cid_ID, strString, CID_LENGTH + 1);
};

// Comparison operators
inline int CChunkID::operator==(const CChunkID &cidOther) const {
  return (*((ULONG *)&cid_ID[0]) == *((ULONG *)&cidOther.cid_ID[0]));
};

inline int CChunkID::operator!=(const CChunkID &cidOther) const {
  return (*((ULONG *)&cid_ID[0]) != *((ULONG *)&cidOther.cid_ID[0]));
};

// CroTeam stream class -- abstract base class
class ENGINE_API CTStream {
  public:
    CListNode strm_lnListNode; // for linking into main library's list of opened streams

  public:
    CTString strm_strStreamDescription; // descriptive string

    enum DictionaryMode {
      DM_NONE,       // no dictionary on this file (yet)
      DM_ENABLED,    // dictionary is enabled, reading/writing rest of file
      DM_PROCESSING, // reading/writing the dictionary itself
    } strm_dmDictionaryMode; // dictionary mode of operation on this file

    SLONG strm_slDictionaryPos; // dictionary position in file (0 for no dictionary)
    INDEX strm_ctDictionaryImported; // how many filenames were imported
    class CNameTable_CTFileName &strm_ntDictionary; // name table for the dictionary
    CDynamicStackArray<CTFileName> strm_afnmDictionary; // dictionary is stored here

    // Throw an exception of formatted string
    void Throw_t(char *strFormat, ...); // throw char *

    // read the dictionary from given offset in file (internal function)
    void ReadDictionary_intenal_t(SLONG slOffset);

    // copy filename dictionary from another stream
    void CopyDictionary(CTStream &strmOther);

  public:
    // modes for opening streams
    enum OpenMode {
      OM_READ = 1,
      OM_WRITE = 2,
      OM_READTEXT = OM_READ,
      OM_WRITETEXT = OM_WRITE,
      OM_READBINARY = OM_READ,
      OM_WRITEBINARY = OM_WRITE,
    };

    // OBSOLETE!
    enum CreateMode {
      CM_TEXT = 1,
      CM_BINARY = 2,
    };

    // Direction for seeking
    enum SeekDir {
      SD_BEG = SEEK_SET,
      SD_END = SEEK_END,
      SD_CUR = SEEK_CUR,
    };

    // Static function enable stream handling
    static void EnableStreamHandling(void);

    // Static function disable stream handling
    static void DisableStreamHandling(void);

    #ifdef PLATFORM_WIN32 // rcg10042001 !!! FIXME
    // Static function to filter exceptions and intercept access violation
    static int ExceptionFilter(DWORD dwCode, _EXCEPTION_POINTERS *pExceptionInfoPtrs);
    #endif

    // Static function to report fatal exception error
    static void ExceptionFatalError(void);

    // Constructor
    CTStream(void);

    // Destruction
    virtual ~CTStream(void);

    // Check if the stream can be read. -- used mainly for assertions
    virtual BOOL IsReadable(void) = 0;

    // Check if the stream can be written. -- used mainly for assertions
    virtual BOOL IsWriteable(void) = 0;

    // Check if the stream can be seeked. -- used mainly for assertions
    virtual BOOL IsSeekable(void) = 0;

    // Read a block of data from stream
    virtual void Read_t(void *pvBuffer, SLONG slSize) = 0; // throw char *

    // Write a block of data to stream
    virtual void Write_t(const void *pvBuffer, SLONG slSize) = 0; // throw char *

    // Seek in stream
    virtual void Seek_t(SLONG slOffset, enum SeekDir sd) = 0; // throw char *

    // Set absolute position in stream
    virtual void SetPos_t(SLONG slPosition) = 0; // throw char *

    // Get absolute position in stream
    virtual SLONG GetPos_t(void) = 0; // throw char *

    // Get size of stream
    virtual SLONG GetStreamSize(void) = 0;

    // Get CRC32 of stream
    virtual ULONG GetStreamCRC32_t(void) = 0;

    // Check if file position points to the EOF
    virtual BOOL AtEOF(void) = 0;

    // Get description of this stream (e.g. filename for a CFileStream)
    inline CTString &GetDescription(void) {
      return strm_strStreamDescription;
    };

    // [Cecil] 2021-02-20: Macro for reading objects
    #define DEFINE_STRM_READ(_Type) \
      inline CTStream &operator>>(_Type &_Val) { \
        Read_t(&_Val, sizeof(_Val)); \
        return *this; \
      }

    // Read an object from stream (throws char *)
    DEFINE_STRM_READ(float);
    DEFINE_STRM_READ(double);
    DEFINE_STRM_READ(ULONG);
    DEFINE_STRM_READ(UWORD);
    DEFINE_STRM_READ(UBYTE);
    DEFINE_STRM_READ(SLONG);
    DEFINE_STRM_READ(SWORD);
    DEFINE_STRM_READ(SBYTE);
    DEFINE_STRM_READ(BOOL);

    // [Cecil] 2021-02-20: Macro for writing objects
    #define DEFINE_STRM_WRITE(_Type) \
      inline CTStream &operator<<(const _Type &_Val) { \
        Write_t(&_Val, sizeof(_Val)); \
        return *this; \
      }

    // Write an object into stream (throws char *)
    DEFINE_STRM_WRITE(float);
    DEFINE_STRM_WRITE(double);
    DEFINE_STRM_WRITE(ULONG);
    DEFINE_STRM_WRITE(UWORD);
    DEFINE_STRM_WRITE(UBYTE);
    DEFINE_STRM_WRITE(SLONG);
    DEFINE_STRM_WRITE(SWORD);
    DEFINE_STRM_WRITE(SBYTE);
    DEFINE_STRM_WRITE(BOOL);

    // [Cecil] New timer: Ticks
    DEFINE_STRM_READ(TICK);
    DEFINE_STRM_WRITE(TICK);

    // CTFileName reading/writing
    ENGINE_API friend CTStream &operator>>(CTStream &strmStream, CTFileName &fnmFileName);
    ENGINE_API friend CTStream &operator<<(CTStream &strmStream, const CTFileName &fnmFileName);

    // Put a line of text into stream
    virtual void PutLine_t(const char *strBuffer); // throw char *
    virtual void PutString_t(const char *strString); // throw char *
    virtual void FPrintF_t(const char *strFormat, ...); // throw char *

    // Get a line of text from stream
    virtual void GetLine_t(char *strBuffer, SLONG slBufferSize, char cDelimiter = '\n'); // throw char *
    virtual void GetLine_t(CTString &strLine, char cDelimiter = '\n'); // throw char *

    virtual CChunkID GetID_t(void); // throw char *
    virtual CChunkID PeekID_t(void); // throw char *
    virtual void ExpectID_t(const CChunkID &cidExpected); // throw char *
    virtual void ExpectKeyword_t(const CTString &strKeyword); // throw char *
    virtual SLONG GetSize_t(void); // throw char *

    virtual void ReadRawChunk_t(void *pvBuffer, SLONG slSize); // throw char *
    virtual void ReadChunk_t(void *pvBuffer, SLONG slExpectedSize); // throw char *
    virtual void ReadFullChunk_t(const CChunkID &cidExpected, void *pvBuffer, SLONG slExpectedSize); // throw char *
    virtual void *ReadChunkAlloc_t(SLONG slSize = 0); // throw char *
    virtual void ReadStream_t(CTStream &strmOther); // throw char *

    virtual void WriteID_t(const CChunkID &cidSave); // throw char *
    virtual void WriteSize_t(SLONG slSize); // throw char *
    virtual void WriteRawChunk_t(void *pvBuffer, SLONG slSize); // doesn't write length (throws char *)
    virtual void WriteChunk_t(void *pvBuffer, SLONG slSize); // throw char *
    virtual void WriteFullChunk_t(const CChunkID &cidSave, void *pvBuffer, SLONG slSize); // throw char *
    virtual void WriteStream_t(CTStream &strmOther); // throw char *

    // Whether or not the given pointer is coming from this stream (mainly used for exception handling)
    virtual BOOL PointerInStream(void *pPointer);

    // Filename dictionary operations

    // Start writing a with a dictionary
    void DictionaryWriteBegin_t(const CTFileName &fnmImportFrom, SLONG slImportOffset);

    // Stop writing a with a dictionary
    void DictionaryWriteEnd_t(void);

    // Start reading a with a dictionary
    SLONG DictionaryReadBegin_t(void); // returns offset of dictionary for cross-file importing

    // Stop reading a with a dictionary
    void DictionaryReadEnd_t(void);

    // Preload all files mentioned in the dictionary
    void DictionaryPreload_t(void);
};

// CroTeam file stream class
class ENGINE_API CTFileStream : public CTStream {
  private:
    FILE *fstrm_pFile; // ptr to opened file

    INDEX fstrm_iZipHandle; // handle of zip-file entry
    INDEX fstrm_iZipLocation; // location in zip-file entry
    UBYTE *fstrm_pubZipBuffer; // buffer for zip-file entry
    SLONG fstrm_slZipSize; // size of the zip-file entry

    BOOL fstrm_bReadOnly; // set if file is opened in read-only mode

  public:
    // Constructor
    CTFileStream(void);

    // Destructor
    virtual ~CTFileStream(void);

    // Open an existing file
    void Open_t(const CTFileName &fnFileName, enum CTStream::OpenMode om = CTStream::OM_READ); // throw char *

    // Create a new file or overwrite existing
    void Create_t(const CTFileName &fnFileName, enum CTStream::CreateMode cm = CTStream::CM_BINARY); // throw char *

    // Close an open file
    void Close(void);

    // Get CRC32 of stream
    ULONG GetStreamCRC32_t(void);

    // Write a block of data to stream
    void Write_t(const void *pvBuffer, SLONG slSize); // throw char *

    // Read a block of data from stream
    void Read_t(void *pvBuffer, SLONG slSize); // throw char *

    // Seek in stream
    void Seek_t(SLONG slOffset, enum SeekDir sd); // throw char *

    // Set absolute position in stream
    void SetPos_t(SLONG slPosition); // throw char *

    // Get absolute position in stream
    SLONG GetPos_t(void); // throw char *

    // Get size of stream
    SLONG GetStreamSize(void);

    // Check if file position points to the EOF
    BOOL AtEOF(void);

    // Whether or not the given pointer is coming from this stream (mainly used for exception handling)
    virtual BOOL PointerInStream(void *pPointer);

    // From CTStream
    inline virtual BOOL IsWriteable(void) {
      return !fstrm_bReadOnly;
    };

    inline virtual BOOL IsReadable(void) {
      return TRUE;
    };

    inline virtual BOOL IsSeekable(void) {
      return TRUE;
    };
};

// CroTeam memory stream class
class ENGINE_API CTMemoryStream : public CTStream {
  public:
    BOOL mstrm_bReadable; // set if stream is readable
    BOOL mstrm_bWriteable; // set if stream is writeable
    INDEX mstrm_ctLocked; // counter for buffer locking

    UBYTE *mstrm_pubBuffer; // buffer of the stream
    UBYTE *mstrm_pubBufferEnd; // pointer to the end of the stream buffer
    SLONG mstrm_slLocation; // location in the stream
    UBYTE *mstrm_pubBufferMax; // furthest that the stream location has ever gotten

  public:
    // Create dynamically resizing stream for reading/writing
    CTMemoryStream(void);

    // Create static stream from given buffer
    CTMemoryStream(void *pvBuffer, SLONG slSize, CTStream::OpenMode om = CTStream::OM_READ);

    // Destructor
    virtual ~CTMemoryStream(void);

    // Lock the buffer contents and it's size
    void LockBuffer(void **ppvBuffer, SLONG *pslSize);

    // Unlock buffer
    void UnlockBuffer(void);

    // Write a block of data to stream
    void Write_t(const void *pvBuffer, SLONG slSize); // throw char *

    // Read a block of data from stream
    void Read_t(void *pvBuffer, SLONG slSize); // throw char *

    // Seek in stream
    void Seek_t(SLONG slOffset, enum SeekDir sd); // throw char *

    // Set absolute position in stream
    void SetPos_t(SLONG slPosition); // throw char *

    // Get absolute position in stream
    SLONG GetPos_t(void); // throw char *

    // Get size of stream.
    SLONG GetSize_t(void); // throw char *

    // Get size of stream
    SLONG GetStreamSize(void);

    // Get CRC32 of stream
    ULONG GetStreamCRC32_t(void);

    // Check if file position points to the EOF
    BOOL AtEOF(void);

    // Whether or not the given pointer is coming from this stream (mainly used for exception handling)
    virtual BOOL PointerInStream(void *pPointer);

    // Memory stream can be opened only for reading and writing
    virtual BOOL IsWriteable(void);
    virtual BOOL IsReadable(void);
    virtual BOOL IsSeekable(void);
};

// Test if a file exists
ENGINE_API BOOL FileExists(const CTFileName &fnmFile);

// Test if a file exists for writing
// (this is can be diferent than normal FileExists() if a mod uses basewriteexclude.lst
ENGINE_API BOOL FileExistsForWriting(const CTFileName &fnmFile);

// Get file timestamp
ENGINE_API SLONG GetFileTimeStamp_t(const CTFileName &fnmFile); // throw char *

// Get CRC32 of a file
ENGINE_API ULONG GetFileCRC32_t(const CTFileName &fnmFile); // throw char *

// Test if a file is read only (also returns FALSE if file does not exist)
ENGINE_API BOOL IsFileReadOnly(const CTFileName &fnmFile);

// Delete a file (called 'remove' to avid name clashes with win32)
ENGINE_API BOOL RemoveFile(const CTFileName &fnmFile);

// Expand a file's filename to full path

// These are input flags for describing what you need the file for
#define EFP_READ   (1UL << 0) // will open for reading
#define EFP_WRITE  (1UL << 1) // will open for writing
#define EFP_NOZIPS (1UL << 31) // add this flag to forbid searching in zips

// These are return values
#define EFP_NONE    0 // doesn't exist
#define EFP_FILE    1 // generic file on disk
#define EFP_BASEZIP 2 // file in one of base zips
#define EFP_MODZIP  3 // file in one of mod zips

ENGINE_API INDEX ExpandFilePath(ULONG ulType, const CTFileName &fnmFile, CTFileName &fnmExpanded);

// These are input flags for directory reading
#define DLI_RECURSIVE (1UL << 0) // recurse into subdirs
#define DLI_SEARCHCD  (1UL << 1) // search the CD path also

// Make a list of all files in a directory
ENGINE_API void MakeDirList(CDynamicStackArray<CTFileName> &adeDir,
                            const CTFileName &fnmDir, // directory to list
                            const CTString &strPattern, // pattern for each file to match ("" matches all)
                            ULONG ulFlags // additional flags
);

// Global string with application path
ENGINE_API extern CTFileName _fnmApplicationPath;

// Global string with current MOD path
ENGINE_API extern CTFileName _fnmMod;

// Global string with current name (the parameter that is passed on cmdline)
ENGINE_API extern CTString _strModName;

// Global string with url to be shown to users that don't have the mod installed
// (should be set by game.dll)
ENGINE_API extern CTString _strModURL;

// Global string with CD path (for minimal installations)
ENGINE_API extern CTFileName _fnmCDPath;

// Global string with filename of the started application
ENGINE_API extern CTFileName _fnmApplicationExe;

// Application path usage funtions
ENGINE_API void UseApplicationPath(void);
ENGINE_API void IgnoreApplicationPath(void);

#endif /* include-once check. */
