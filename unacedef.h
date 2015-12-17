#ifndef __UNACEDEF_H_BY_K_INABA__
#define __UNACEDEF_H_BY_K_INABA__
#ifdef __cplusplus
 extern "C" {
#endif

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//ออออออออออออออออ=====---  Part 1: different structures ---====อออออออออออออออ
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  //---------------------------------------------------------------------------
  //
  // Here in different structures used at callback functions and
  // ACE.DLL functions are declared.
  //
  //---------------------------------------------------------------------------
  //
  // Contents:
  //   Part 1.1: structures used in callback structures
  //   Part 1.2: structures used in function structures
  //
  //---------------------------------------------------------------------------


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //========---  Part 1.1: structures used in callback structures  ---=========
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  comment buffer structure  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Used in tACEGlobalDataStruc. Application has to specify where the
    // comment is or should be stored.
    //-------------------------------------------------------------------------

        typedef struct sACECommentStruc
        {
          LPSTR Buf;                  // area where the comments are stored
                                      // by the application or by the Dll
                                      // comments must be zero-terminated
          ULONG BufSize;              // size of this area
          ULONG State;                // state of comment - see ACE_COMMENT
                                      // constants below
        } tACECommentStruc,
         *pACECommentStruc;


      //-----------------------------------------------------------------------
      // Defines for the tACECommentStruc.State field, which tells about
      // the result of the last comment extraction.
      //-----------------------------------------------------------------------

        #define ACE_COMMENT_OK          0    // comment extraction went fine

        #define ACE_COMMENT_SMALLBUF    1    // comment buffer too small to
                                             // store the whole comment in

        #define ACE_COMMENT_NONE        255  // No comment present


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  global data structure  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // This structure contains information for the Dll being interesting for
    // nearly all functions. The Dll has to be initialized with this
    // structure passed to tACEInitDll(). This structure is also passed
    // by the callback functions.
    //-------------------------------------------------------------------------

              //---------------------------------------------------------------
              // Forward declarations of callback function parameters
              // used in this structure.
              //---------------------------------------------------------------

                typedef union sACEInfoCallbackProcStruc
                                       *pACEInfoCallbackProcStruc;

                typedef union sACEErrorCallbackProcStruc
                                       *pACEErrorCallbackProcStruc;

                typedef union sACERequestCallbackProcStruc
                                       *pACERequestCallbackProcStruc;

                typedef union sACEStateCallbackProcStruc
                                       *pACEStateCallbackProcStruc;


        typedef struct sACEGlobalDataStruc
        {
        //------------------------ fields for ACE and UnACE -------------------

          void  *Obj;           // ---- reserved for application! ----
                                // thought to be used as a pointer to
                                // an object; when a callback-function is
                                // called, the object pointer can be used to
                                // handle the callback in a specific way;
                                // the pointer has to be initialized by
                                // ACEInitDll()

          ULONG MaxArchiveTestBytes;

                                // how many bytes of a file should be
                                // looked upon at archive header search?

          ULONG MaxFileBufSize;

                                // maximum buffer size for buffered
                                // I/O operations

          tACECommentStruc      // used to exchange comment data
                Comment;        // between application and Dll
                                // using callback functions

          LPSTR DecryptPassword;

                                // the DecryptPassword specified at
                                // ACEInitDll() is overwritten by the
                                // DecryptPassword field of tACEAddStruc and
                                // other function-specific structures;
                                // but this field can be used to change the
                                // password at callback function calls

          BOOL UseVBStructures; //  passes structures to callback functions
          			//  much better suited for Visual Basic

          CHAR  Reserved1[60];  // has to be filled with zeros

        //--------------------------- fields for ACE only ---------------------

          LPSTR EncryptPassword;

                                // things stated at the description of the
                                // DecryptPassword field apply here as well


          LPSTR TempDir;        // directory to save temporary archive

        //-------- registration (ACE DLLs only, but not used at ACL) ----------

          LPSTR KeyPath;        // necessary for reading and writing key file

          LPSTR UserAV;         // Dll returns the AV string (if registered)
                                // in this field

          LPSTR IsGeneralKey;   // DLL returns the key, if it is a general key

          HWND  OwnerWindow;    // specifies the applications window to be
                                // parent of the registration reminder dialog

        //--------------------------- fields for ACE only ---------------------

          ULONG CompressionLevel;
                                // contains the currently used compression
                                // level - may be changed during compression
                                // operation

          CHAR  Reserved2[56];  // has to be filled with zeros


        //----------------------  callback functions (ACE and UnACE) ----------

          INT (__stdcall *InfoCallbackProc)
                          (pACEInfoCallbackProcStruc Info);

                                //  Informs application about details like the
                                //  creation of a temporary archive.
                                //  See ACE_CALLBACK_INFO definitions for
                                //  more information.

          INT (__stdcall *ErrorCallbackProc)
                           (pACEErrorCallbackProcStruc Error);

                                //  Called to let the application know about
                                //  errors. The Code field of Error will
                                //  contain an ACE_CALLBACK_ERROR value.

          INT (__stdcall *RequestCallbackProc)
                           (pACERequestCallbackProcStruc Request);

                                //  Called to let the application know about
                                //  requests for data. Code will contain an
                                //  ACE_CALLBACK_REQUEST value.

          INT (__stdcall *StateCallbackProc)
                           (pACEStateCallbackProcStruc State);

                                //  Called to let the application know about
                                //  the progress of operations.


        //-------------------------- different new fields ---------------------
          CHAR  Reserved3[64];  // has to be filled with zeros
        } tACEGlobalDataStruc,
         *pACEGlobalDataStruc;


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  archive data structure  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Informs the callback functions about the current archive, its volume
    // number, the archive-flags (see ACE_FLAG constants), the creation host
    // system (see ACE_HOST constants) and the AV if present in archive.
    // Also used at ACEReadArchiveData().
    //-------------------------------------------------------------------------

        typedef struct sACEArchiveDataStruc
        {
          LPSTR ArchiveName;
          ULONG VolumeNumber,
                Flags,          // see ACE_ARCFLAG defines below
                HostCreated,    // see ACE_HOST defines below
                TimeCreated,    // in MS-DOS format
                VersionCreated,
                VersionExtract; // version needed to extract files
          LPSTR AV;             // not used in ACL
          CHAR  Reserved[64];   // filled with zeros
        } tACEArchiveDataStruc,
         *pACEArchiveDataStruc;


      //-----------------------------------------------------------------------
      // Flag constants for tACEArchiveDataStruc.Flags field.
      //-----------------------------------------------------------------------

        #define ACE_ARCFLAG_MAINCOMMENT         2
        #define ACE_ARCFLAG_SFX                 512
        #define ACE_ARCFLAG_LIMITSFXJR          1024  // is an SFX archive
                                                      // that supports 256k
                                                      // dictionary only
        #define ACE_ARCFLAG_MULTIVOLUME         2048
        #define ACE_ARCFLAG_AV                  4096  // not used in ACL
        #define ACE_ARCFLAG_RECOVERYREC         8192
        #define ACE_ARCFLAG_LOCK                16384
        #define ACE_ARCFLAG_SOLID               32768


      //-----------------------------------------------------------------------
      // Host system used to create an archive. Used at
      // tACEArchiveDataStruc.HostCreated field.
      //-----------------------------------------------------------------------

        #define ACE_HOST_MSDOS                  0     // archive created by
                                                      // MSDOS ACE archiver
        #define ACE_HOST_OS2                    1     // created by OS2 ACE
        #define ACE_HOST_WIN32                  2     // created by Win32 ACE


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  file data structure  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Contains information about an archived file.
    //-------------------------------------------------------------------------

        typedef struct sACEFileDataStruc
        {
          LPSTR SourceFileName;      // relative file name
          LPSTR DestinationFileName; // absolute file name;
                                     // valid for add and extract only!
          ULONG Flags,               // see ACE_FILEFLAG defines below
                CRC32,
                Method,              // 0=stored, 1=LZ77, 2=V20Compression
                Dictionary;          // DictionarySize = 2^Dictionary
          DWORDLONG
                CompressedSize,
                Size;
          ULONG Time,
                Attributes;
          CHAR  Reserved[64];        // filled with zeros
        } tACEFileDataStruc,
         *pACEFileDataStruc;


      //-----------------------------------------------------------------------
      // Flag constants for the tACEFileData.Flags field.
      //-----------------------------------------------------------------------

        #define ACE_FILEFLAG_FILECOMMENT        2     // file has comment
        #define ACE_FILEFLAG_SPLITBEFORE        4096  // continued from
                                                      // previous volume
        #define ACE_FILEFLAG_SPLITAFTER         8192  // continued on
                                                      // next volume
        #define ACE_FILEFLAG_PASSWORD           16384 // is encrypted
        #define ACE_FILEFLAG_SOLID              32768 // uses data of previous
                                                      // files (solid mode)


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  copy info structure  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Is passed to ACEInfoCallbackProc with ACE_CALLBACK_INFO_COPY as code.
    // Informs application about the progress of copying either an archive to
    // a temporary archive, or a temporary archive back to a normal archive.
    //-------------------------------------------------------------------------

        typedef struct sACECopyInfoStruc
        {
          LPSTR SourceFileName,      // source file
                DestinationFileName; // the file copying the source to
          DWORDLONG
                CopiedBytes,         // bytes already copied
                FileSize;            // source file size
          CHAR  Reserved[64];        // filled with zeros
        } tACECopyInfoStruc,
         *pACECopyInfoStruc;


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  operation progress structure  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Used to state the progress of the current operation.
    //-------------------------------------------------------------------------

        typedef struct sACEProgressDataStruc
        {
          LPSTR     Addr;               // address of uncompressed data block
          ULONG     Size;               // size of uncompressed data block
          DWORDLONG TotalProcessedSize; // counted by Dll:
                                        // total uncompressed bytes processed
          DWORDLONG TotalCompressedSize;// total compressed bytes processed
          DWORDLONG TotalSize;          // total uncompressed bytes to process
                                        // (sum of all original file sizes)
          DWORDLONG FileProcessedSize;  // uncompr. bytes of file processed
          DWORDLONG FileCompressedSize; // compr. bytes of file processed
          DWORDLONG FileSize;           // uncompressed file size
        } tACEProgressDataStruc,
         *pACEProgressDataStruc;


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //========---  Part 1.2: structures used in function structures  ---=========
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  file list structure  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // This structure is used in the function specific structures.
    // The application has to use this structure to indicate which files
    // have to be processed by the DLL.
    //-------------------------------------------------------------------------

        typedef struct sACEFilesStruc
        {
          LPSTR SourceDir;            // maybe a real or an archive directory
          LPSTR FileList;             // pointer to list of files to process;
                                      // zero-terminated; files have to be
                                      // separated by carriage-return (0xd);
                                      // FileList may/will be modified by the
                                      // Dll; if nothing is specified, "*"
                                      // will be used
                                      // (attention at ACEDelete!!)
          LPSTR ExcludeList;          // list of files to exclude from process
          BOOL  FullMatch;            // specifications must fully match
                                      // (no files with the same name in
				      //  subdirs are processed)
          BOOL  RecurseSubDirs;       // recurse subdirectories
           			      // (valid for add operations only)
          CHAR  Reserved[60] ;         // has to be filled with zeros
                                      // for future: possibly in-/excluding
                                      // file attributes and date/time range
        } tACEFilesStruc,
         *pACEFilesStruc;


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  V2.0 compression structure ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Specifies whether to use v2.0 compression or not. If you use v2.0
    // compression you can also specify which v2.0 compression techniques
    // you want to use. (ACE only)
    //-------------------------------------------------------------------------

        typedef struct sACEV20CompressionStruc
        {
          BOOL  DoUse,                // if DoUse=1 and all other fields are
                DoUseDelta,           // zero, then all v2.0 compression
                DoUseExe,             // techniques will be used
                DoUsePic,
                DoUseSound;
          CHAR  Reserved[64];         // has to be filled with zeros
        } tACEV20CompressionStruc,
         *pACEV20CompressionStruc;


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  compression parameter structure  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Used in tACEAddStruc and tACEDeleteStruc. (ACE only)
    //-------------------------------------------------------------------------

        typedef struct sACECompressParamsStruc
        {
          ULONG Level,                // see ACE_LEVEL constants below
                Dictionary;           // 15(32k)..22(4Mb)

          tACEV20CompressionStruc     // indicates if (or which) v2.0
                V20Compression;       // compression techniques shall be used

          BOOL  TestAfter;            // make a test for CRC check errors
                                      // after compression
          CHAR  Reserved[64];         // has to be filled with zeros
        } tACECompressParamsStruc,
         *pACECompressParamsStruc;


      //-----------------------------------------------------------------------
      // Tells the Dll which compression level to use. (ACE only)
      //-----------------------------------------------------------------------

        #define ACE_LEVEL_STORE     0 // save file only; do not compress
        #define ACE_LEVEL_FASTEST   1 // compress very fast
        #define ACE_LEVEL_FAST      2 // compress fast
        #define ACE_LEVEL_NORMAL    3 // good compromise between speed and
                                      // compression rate
        #define ACE_LEVEL_GOOD      4 // achieves good compression
        #define ACE_LEVEL_BEST      5 // best compression; bit slow


//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//อออออออออออ=====---  Part 2: ACE.DLL callback declarations ---====อออออออออออ
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  //---------------------------------------------------------------------------
  //
  // ACE.DLL makes use of four callback functions to exchange data
  // with the application:
  //
  //   1) InfoCallbackProc   (pACEInfoCallbackProcStruc    Info)
  //   2) ErrorCallbackProc  (pACEErrorCallbackProcStruc   Error)
  //   3) RequestCallbackProc(pACERequestCallbackProcStruc Request)
  //   4) StateCallbackProc  (pACEStateCallbackProcStruc   State)
  //
  // Meaning of different callback types:
  //   Info    - lets the application know about actions that take some
  //             time but are not essential
  //             (Code is one of ACE_CALLBACK_INFO constants)
  //   Error   - an error occured; if the reason for this error can
  //             be solved by the application then the Dll can continue
  //             the current operation, otherwise the operation has to
  //             be canceled
  //             (Code is one of ACE_CALLBACK_ERROR constants)
  //   Request - the Dll needs some user input
  //             for ex.: "Overwrite file? (yes/no/cancel)"
  //             (Code is one of ACE_CALLBACK_REQUEST constants)
  //   State   - Dll informs application about the progress of an operation
  //             (Code is one of ACE_CALLBACK_STATE constants)
  //
  // The pointers to the callback functions has to be set by the application
  // when calling ACEInitDll(). If the application does not install
  // a callback function, is has set the corresponding pointer to NULL.
  // If the ACE.DLL has to call the Error or Request callback function
  // and they are not installed, the ACE.DLL will cancel the operation.
  //
  // The application has the possibility to cancel the current operation
  // at each callback function call. So if the user clicks on a Cancel-button,
  // the application should return ACE_CALLBACK_RETURN_CANCEL at the next
  // callback function call.
  //
  // All callback function parameters are declared as unions.
  // The StructureType field contains he type of the structure which is used.
  // When the application knows which type of structure it has to use,
  // it will have to interpret the Code field to get to know the reason
  // for the callback function call.
  //
  //---------------------------------------------------------------------------
  //
  // Contents:
  //   Part 2.1: operation types
  //   Part 2.2: callback function return codes
  //   Part 2.3: callback structure types
  //   Part 2.4: callback structures
  //   Part 2.5: info callback function
  //   Part 2.6: error callback function
  //   Part 2.7: request callback function
  //   Part 2.8: state callback function
  //
  //---------------------------------------------------------------------------


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //=====================---  Part 2.1: operation codes  ---===================
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  // Passed to callback functions indicating the current operation.
  //-------------------------------------------------------------------------

        #define ACE_CALLBACK_OPERATION_LIST         0
        #define ACE_CALLBACK_OPERATION_TEST         1
        #define ACE_CALLBACK_OPERATION_ANALYZE      2
        #define ACE_CALLBACK_OPERATION_EXTRACT      3
        #define ACE_CALLBACK_OPERATION_ADD          4
        #define ACE_CALLBACK_OPERATION_REPACK       5
        #define ACE_CALLBACK_OPERATION_DELETE       6
        #define ACE_CALLBACK_OPERATION_REPAIR       7    // repair without
                                                         // recovery record
        #define ACE_CALLBACK_OPERATION_SETCMT       8
        #define ACE_CALLBACK_OPERATION_ENCRYPT      9
        #define ACE_CALLBACK_OPERATION_KEEP         10   // file is to be
                                                         // taken along
                                                         // without recompress
        #define ACE_CALLBACK_OPERATION_RECOVER      11   // begin of
                                                         // recovering archive
                                                         // by recovery record
        #define ACE_CALLBACK_OPERATION_HEADSEARCH   12   // begin of searching
                                                         // for file headers
        #define ACE_CALLBACK_OPERATION_RECRECSEARCH 13   // begin of searching
                                                         // for recovery record
        #define ACE_CALLBACK_OPERATION_ADDSFX       14
        #define ACE_CALLBACK_OPERATION_LOCK         15
        #define ACE_CALLBACK_OPERATION_ADDAV        16   // not used in ACL
        #define ACE_CALLBACK_OPERATION_ADDRECOVREC  17
        #define ACE_CALLBACK_OPERATION_REGISTER     18   // not used in ACL


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //=============---  Part 2.2: callback function return codes  ---============
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  // One of these result codes has to be returned by the application-based
  // callback functions.
  //---------------------------------------------------------------------------

        #define ACE_CALLBACK_RETURN_OK              0    // also "yes" at
                                                         // requests
        #define ACE_CALLBACK_RETURN_NO              1    // no, do not/
                                                         // do not retry
        #define ACE_CALLBACK_RETURN_CANCEL          2    // abort operation


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //================---  Part 2.3: callback structure types  ---===============
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  // States of which type the passed structure is when a callback function
  // is called. The type is written to the StructureType field.
  //---------------------------------------------------------------------------

        #define ACE_CALLBACK_TYPE_GLOBAL                0x0

                // type of structure is
                // tACECallbackGlobalStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // InfoCallbackProc    | ACE_CALLBACK_INFO_GENERALKEY      // not used in ACL
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // ErrorCallbackProc   | ACE_CALLBACK_ERROR_MEMORY // fr ManyFilesError und ExtractMem andere Codes verwenden!?
                //                     | ACE_CALLBACK_ERROR_REGISTER       // not used in ACL
                //                     | ACE_CALLBACK_ERROR_READKEY        // not used in ACL
                //                     | ACE_CALLBACK_ERROR_WRITEKEY       // not used in ACL
                //                     | ACE_CALLBACK_ERROR_NOWINACEKEY    // not used in ACL
                //                     | ACE_CALLBACK_ERROR_NOACTIVEACEKEY // not used in ACL
                //                     | ACE_CALLBACK_ERROR_UNCSPACE       // wird noch nicht verwendet!
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // RequestCallbackProc | ACE_CALLBACK_REQUEST_REGISTER     // not used in ACL
                //


        #define ACE_CALLBACK_TYPE_ARCHIVE               0x1

                // type of structure is
                // tACECallbackArchiveStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // InfoCallbackProc    | ACE_CALLBACK_INFO_TMPARCCREATE
                //                     | ACE_CALLBACK_INFO_TMPARCCREATEEND
                //                     | ACE_CALLBACK_INFO_ADDRECREC
                //                     | ACE_CALLBACK_INFO_ADDRECRECEND
                //                     | ACE_CALLBACK_INFO_RECREC
                //                     | ACE_CALLBACK_INFO_NORECREC
                //                     | ACE_CALLBACK_INFO_RECOVERED
                //                     | ACE_CALLBACK_INFO_NODAMAGE
                //                     | ACE_CALLBACK_INFO_FNDMAINHEAD
                //                     | ACE_CALLBACK_INFO_FILELISTCREATE
                //                     | ACE_CALLBACK_INFO_FILELISTCREATEEND
                //                     | ACE_CALLBACK_INFO_FILESORT
                //                     | ACE_CALLBACK_INFO_FILESORTEND
                //                     | ACE_CALLBACK_INFO_COPYEND
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // ErrorCallbackProc   | ACE_CALLBACK_ERROR_MODIFYVOLUME
                //                     | ACE_CALLBACK_ERROR_MODIFYLOCKEDARCHIVE
                //                     | ACE_CALLBACK_ERROR_AV                  // not used in ACL
                //                     | ACE_CALLBACK_ERROR_TOODAMAGED
                //                     | ACE_CALLBACK_ERROR_ARCHIVEEXISTS
                //                     | ACE_CALLBACK_ERROR_OPENREPAIRARCHIVE
                //                     | ACE_CALLBACK_ERROR_OPENARCHIVEREAD
                //                     | ACE_CALLBACK_ERROR_OPENARCHIVEWRITE
                //                     | ACE_CALLBACK_ERROR_READARCHIVE
                //                     | ACE_CALLBACK_ERROR_WRITEARCHIVE
                //                     | ACE_CALLBACK_ERROR_ALREADYSFX
                //                     | ACE_CALLBACK_ERROR_ADDSFXTOVOLUME
                //                     | ACE_CALLBACK_ERROR_ARCHIVEBROKEN
                //                     | ACE_CALLBACK_ERROR_ARCHIVESAVE
                //                     | ACE_CALLBACK_ERROR_NOFILES
                //                     | ACE_CALLBACK_ERROR_ISNOTANARCHIVE
                //                     | ACE_CALLBACK_ERROR_TEMPDIRCREATE
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // RequestCallbackProc | ACE_CALLBACK_REQUEST_MARKASSOLID
                //                     | ACE_CALLBACK_REQUEST_CHANGEVOLUME
                //                     | ACE_CALLBACK_REQUEST_ARCHIVEEXISTS
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // StateCallbackProc   | ACE_CALLBACK_STATE_STARTARCHIVE
                //


        #define ACE_CALLBACK_TYPE_ARCHIVEDFILE          0x2

                // type of structure is
                // tACECallbackArchivedFileStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // InfoCallbackProc    | ACE_CALLBACK_INFO_TMPARCCREATE
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // ErrorCallbackProc   | ACE_CALLBACK_ERROR_CREATIONNAMEINUSE
                //                     | ACE_CALLBACK_ERROR_HIGHERVERSION
                //                     | ACE_CALLBACK_ERROR_ENCRYPTIONCRC
                //                     | ACE_CALLBACK_ERROR_WRITE
                //                     | ACE_CALLBACK_ERROR_READ
                //                     | ACE_CALLBACK_ERROR_OPENREAD
                //                     | ACE_CALLBACK_ERROR_OPENWRITE //wird noch gar nich benutzt?? sollte aber - bei extract!
                //                     | ACE_CALLBACK_ERROR_FILENAMETOOLONG
                //                     | ACE_CALLBACK_ERROR_REPACKCRC
                //                     | ACE_CALLBACK_ERROR_EXCLUDEPATH
                //                     | ACE_CALLBACK_ERROR_METHOD
                //                     | ACE_CALLBACK_ERROR_EXTRACTSPACE
                //                     | ACE_CALLBACK_ERROR_CREATION
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // RequestCallbackProc | ACE_CALLBACK_REQUEST_OVERWRITE
                //                     | ACE_CALLBACK_REQUEST_DELETEARCHIVEDSYSFILE
                //                     | ACE_CALLBACK_REQUEST_ADDBROKENFILE
                //                     | ACE_CALLBACK_REQUEST_PASSWORD
                //                     | ACE_CALLBACK_REQUEST_OVERWRITESYSFILE
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // StateCallbackProc   | ACE_CALLBACK_STATE_STARTFILE
                //                     | ACE_CALLBACK_STATE_ENDNOCRCCHECK
                //


        #define ACE_CALLBACK_TYPE_REALFILE              0x3

                // type of structure is
                // tACECallbackRealFileStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // InfoCallbackProc    | ACE_CALLBACK_INFO_FILELISTADD
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // ErrorCallbackProc   | ACE_CALLBACK_ERROR_MOVEDELETE
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // RequestCallbackProc | ACE_CALLBACK_REQUEST_MOVEDELETEREALSYSFILE
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // StateCallbackProc   | ACE_CALLBACK_STATE_STARTFILE
                //


        #define ACE_CALLBACK_TYPE_SPACE                 0x4

                // type of structure is
                // tACECallbackSpaceStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // ErrorCallbackProc   | ACE_CALLBACK_ERROR_TEMPDIRSPACE
                //                     | ACE_CALLBACK_ERROR_ARCHIVESPACE
                //


        #define ACE_CALLBACK_TYPE_SFXFILE               0x5

                // type of structure is
                // tACECallbackSFXFileStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // ErrorCallbackProc   | ACE_CALLBACK_ERROR_READINGSFXFILE
                //


        #define ACE_CALLBACK_TYPE_COPY                  0x6

                // type of structure is
                // tACECallbackCopyStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // InfoCallbackProc    | ACE_CALLBACK_INFO_COPY
                //


        #define ACE_CALLBACK_TYPE_PROGRESS              0x7

                // type of structure is
                // tACECallbackProgressStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // StateCallbackProc   | ACE_CALLBACK_STATE_PROGRESS
                //


        #define ACE_CALLBACK_TYPE_CRCCHECK              0x8

                // type of structure is
                // tACECallbackCRCCheckStruc
                //-------------------------------------------------------------
                // callback function   | codes using this structure
                // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
                // StateCallbackProc   | ACE_CALLBACK_STATE_ENDCRCCHECK
                //


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //==============---  Part 2.4: different callback structures  ---============
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  // These are the declarations of the different structures used in the
  // unions passed by the callback functions.
  //---------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // Only the Dll GlobalData is passed to the application.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackGlobalStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_GLOBAL
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_GLOBAL
          ULONG  Operation;             // ACE_CALLBACK_OPERATION constant

          pACEGlobalDataStruc GlobalData;       // see tACEGlobalDataStruc
        } tACECallbackGlobalStruc,
         *pACECallbackGlobalStruc;


      //-----------------------------------------------------------------------
      // The Dll GlobalData and the ArchiveData are passed.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackArchiveStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_ARCHIVE
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_ARCHIVE
          ULONG  Operation;             // ACE_CALLBACK_OPERATION constant

          pACEGlobalDataStruc  GlobalData;      // see tACEGlobalDataStruc
          pACEArchiveDataStruc ArchiveData;     // see tACEArchiveDataStruc
        } tACECallbackArchiveStruc,
         *pACECallbackArchiveStruc;


      //-----------------------------------------------------------------------
      // GlobalData, ArchiveData and FileData are passed.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackArchivedFileStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_ARCHIVEDFILE
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_ARCHIVEDFILE
          ULONG  Operation;             // ACE_CALLBACK_OPERATION constant

          pACEGlobalDataStruc  GlobalData;      // see tACEGlobalDataStruc
          pACEArchiveDataStruc ArchiveData;     // see tACEArchiveDataStruc
          pACEFileDataStruc    FileData;        // see tACEFileDataStruc
        } tACECallbackArchivedFileStruc,
         *pACECallbackArchivedFileStruc;


      //-----------------------------------------------------------------------
      // GlobalData, ArchiveData and a FileName are passed.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackRealFileStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_REALFILE
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_REALFILE
          ULONG  Operation;             // ACE_CALLBACK_OPERATION constant

          pACEGlobalDataStruc  GlobalData;      // see tACEGlobalDataStruc
          pACEArchiveDataStruc ArchiveData;     // see tACEArchiveDataStruc
          LPSTR                FileName;        // name of file
        } tACECallbackRealFileStruc,
         *pACECallbackRealFileStruc;


      //-----------------------------------------------------------------------
      // GlobalData, ArchiveData, the path of temp directory and the
      // bytes required in temp directory (archive size) are passed.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackSpaceStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_SPACE
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_SPACE
          ULONG  Operation;

          pACEGlobalDataStruc  GlobalData;      // see tACEGlobalDataStruc
          pACEArchiveDataStruc ArchiveData;     // see tACEArchiveDataStruc
          LPSTR                Directory;       // path of directory
          DWORDLONG            ArchiveSize;     // bytes required in temp dir
        } tACECallbackSpaceStruc,
         *pACECallbackSpaceStruc;


      //-----------------------------------------------------------------------
      // GlobalData, ArchiveData and SFXFileName are passed.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackSFXFileStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_SFXFILE
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_SFXFILE
          ULONG  Operation;             // ACE_CALLBACK_OPERATION constant

          pACEGlobalDataStruc  GlobalData;      // see tACEGlobalDataStruc
          pACEArchiveDataStruc ArchiveData;     // see tACEArchiveDataStruc
          LPSTR                SFXFileName;     // name of SFX
        } tACECallbackSFXFileStruc,
         *pACECallbackSFXFileStruc;


      //-----------------------------------------------------------------------
      // GlobalData, ArchiveData and CopyData are passed.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackCopyStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_COPY
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_COPY
          ULONG  Operation;             // ACE_CALLBACK_OPERATION constant

          pACEGlobalDataStruc  GlobalData;      // see tACEGlobalDataStruc
          pACEArchiveDataStruc ArchiveData;     // see tACEArchiveDataStruc
          pACECopyInfoStruc    CopyData;        // see tACECopyInfoStruc
        } tACECallbackCopyStruc,
         *pACECallbackCopyStruc;


      //-----------------------------------------------------------------------
      // GlobalData, ArchiveData, FileData and ProgressData are passed.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackProgressStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_PROGRESS
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_PROGRESS
          ULONG  Operation;             // ACE_CALLBACK_OPERATION constant

          pACEGlobalDataStruc   GlobalData;     // see tACEGlobalDataStruc
          pACEArchiveDataStruc  ArchiveData;    // see tACEArchiveDataStruc
          pACEFileDataStruc     FileData;       // see tACEFileDataStruc
          pACEProgressDataStruc ProgressData;   // see tACEProgressDataStruc
        } tACECallbackProgressStruc,
         *pACECallbackProgressStruc;


      //-----------------------------------------------------------------------
      // GlobalData, ArchiveData, FileData and CRC-check result are passed.
      //-----------------------------------------------------------------------

        typedef struct sACECallbackCRCCheckStruc
        {
          ULONG  StructureType;         // is ACE_CALLBACK_TYPE_CRCCHECK
          ULONG  Code;                  // see definition of
                                        // ACE_CALLBACK_TYPE_CRCCHECK
          ULONG  Operation;             // ACE_CALLBACK_OPERATION constant

          pACEGlobalDataStruc   GlobalData;     // see tACEGlobalDataStruc
          pACEArchiveDataStruc  ArchiveData;    // see tACEArchiveDataStruc
          pACEFileDataStruc     FileData;       // see tACEFileDataStruc
          BOOL CRCOk;                           // CRC-check result
        } tACECallbackCRCCheckStruc,
         *pACECallbackCRCCheckStruc;


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //=================---  Part 2.5: info callback function  ---================
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  // Declaration of ACEInfoCallbackProc() parameter and explanation of
  // callback info codes.
  //---------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // Union parameter used at ACEInfoCallbackProc().
      //-----------------------------------------------------------------------

        typedef union sACEInfoCallbackProcStruc
        {
          ULONG  StructureType;                 // indicates which of the fol-
                                                // lowing structures is used
          tACECallbackGlobalStruc   Global;
          tACECallbackArchiveStruc  Archive;
          tACECallbackRealFileStruc RealFile;
          tACECallbackCopyStruc     Copy;
        } tACEInfoCallbackProcStruc;


      //-----------------------------------------------------------------------
      // These values are passed to the ACEInfoCallbackProc callback function
      // to inform the application about actions (smaller parts of operations)
      // which may take some time or other things that might be of interest.
      //-----------------------------------------------------------------------

        #define ACE_CALLBACK_INFO_GENERALKEY            0x100

                                // key is a general one (no own AV; own key
                                // is obtainable for a special price!?)
                                // not used in ACL
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   ACERegister

        #define ACE_CALLBACK_INFO_TMPARCCREATE          0x110

                                // creating temporary archive for changes
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACERepair


        #define ACE_CALLBACK_INFO_TMPARCCREATEEND       0x111

                                // finished creating temporary archive
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACERepair


        #define ACE_CALLBACK_INFO_ADDRECREC             0x112

                                // adding recovery record
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord
                                //   ACERepair


        #define ACE_CALLBACK_INFO_ADDRECRECEND          0x113

                                // finished adding recovery record
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord
                                //   ACERepair


        #define ACE_CALLBACK_INFO_RECREC                0x114

                                // trying to recover files by recovery
                                // record; end indicated by
                                // ACE_CALLBACK_INFO_RECOVERED or
                                // ACE_CALLBACK_INFO_NODAMAGE
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_INFO_NORECREC              0x115

                                // found no recovery record
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_INFO_RECOVERED             0x116

                                // archive has been fully recovered
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_INFO_NODAMAGE              0x117

                                // ACERepair detected by recovery record that
                                // the archive is not damaged
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_INFO_FNDMAINHEAD           0x118

                                // found archive header
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_INFO_FILELISTCREATE        0x119

                                // creating a file list of specified files
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_INFO_FILELISTCREATEEND     0x11a

                                // sent when creating the list of files
                                // is finished
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_INFO_FILESORT              0x11b

                                // sorting files (for solid compression)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd


        #define ACE_CALLBACK_INFO_FILESORTEND           0x11c

                                // sorting files (for solid compression)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd


        #define ACE_CALLBACK_INFO_COPYEND               0x11d

                                // copying a file finished
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACERepair


        #define ACE_CALLBACK_INFO_FILELISTADD           0x140

                                // called at creation of file list; the name
	                        // of the file just added to file list is
                                // passed in tACECallbackRealFileStruc
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_REALFILE
                                // operations:
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_INFO_COPY                  0x150

                                // copying a file; file name, file size and
                                // copied bytes are passed via
                                // tACECallbackCopyStruc
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_COPY
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACERepair


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //=================---  Part 2.6: error callback function  ---===============
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  // Declaration of ACEErrorCallbackProc() parameter and explanation of
  // callback error codes.
  //---------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // Union parameter used at ACEErrorCallbackProc().
      //-----------------------------------------------------------------------

        typedef union sACEErrorCallbackProcStruc
        {
          ULONG  StructureType;                 // indicates which of the fol-
                                                // lowing structures is used
          tACECallbackGlobalStruc       Global;
          tACECallbackArchiveStruc      Archive;
          tACECallbackArchivedFileStruc ArchivedFile;
          tACECallbackRealFileStruc     RealFile;
          tACECallbackSpaceStruc        Space;
          tACECallbackSFXFileStruc      SFXFile;
        } tACEErrorCallbackProcStruc;


      //-----------------------------------------------------------------------
      // This structure is used by the ACEErrorCallback function to inform
      // the application about errors. The Code field of the used structure
      // contains an ACE_CALLBACK_ERROR value. At most problems modifications
      // to the passed structure can be made to fix it. Other problems can not
      // be solved and cause an operation abort immediately.
      // ErrorCallbackProc() has to return either ACE_CALLBACK_RETURN_OK or
      // ACE_CALLBACK_RETURN_CANCEL.
      //-----------------------------------------------------------------------

        #define ACE_CALLBACK_ERROR_MEMORY               0x200

                                // not enough memory to perform operation
                                // (dictionary too large?)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   all


        #define ACE_CALLBACK_ERROR_REGISTER             0x201

                                // registration key is invalid (or wrong
                                // format?); not used in ACL
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   ACERegister


        #define ACE_CALLBACK_ERROR_READKEY              0x202

                                // key could not be read (does not exist or
                                // is invalid); not used in ACL
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   ACEInitDll


        #define ACE_CALLBACK_ERROR_WRITEKEY             0x203

                                // could not write key; not used in ACL
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   ACERegister


        #define ACE_CALLBACK_ERROR_NOWINACEKEY          0x204

                                // key not valid for WinACE; not used in ACL
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   ACERegister


        #define ACE_CALLBACK_ERROR_NOACTIVEACEKEY       0x205

                                // key not valid for ActiveACE; not used in ACL
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   ACERegister


        #define ACE_CALLBACK_ERROR_UNCSPACE             0x206

                                // Win95_OSR1-bug: it is impossible to
                                // get available space of network drives by
                                // an UNC name; ACE will not stop but
                                // assumes there are 4Gb free
                                // --> the operation might fail if free
                                // disk space is low
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   all


        #define ACE_CALLBACK_ERROR_MODIFYVOLUME         0x220

                                // modification of volumes not possible
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord


        #define ACE_CALLBACK_ERROR_MODIFYLOCKEDARCHIVE  0x221

                                // modification of locked archive not possible
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord


        #define ACE_CALLBACK_ERROR_AV                   0x222

                                // AV of archive is NOT ok or does not match
                                // to the users AV (not used in ACL)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEReadArchiveData
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACERepair
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACEAddAV
                                //   ACELock
                                //   ACEAddRecoveryRecord


        #define ACE_CALLBACK_ERROR_TOODAMAGED           0x223

                                // can not repair by recovery record but
                                // can continue with normal repair
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_ERROR_ARCHIVEEXISTS        0x224

                                // destination file name already used;
                                // may occur if at
                                // ACE_CALLBACK_ERROR_ARCHIVESPACE a
                                // direcory is specified where a file
                                // with the same name as the current archive
                                // already exists
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd


        #define ACE_CALLBACK_ERROR_OPENREPAIRARCHIVE    0x225

                                // could not create archive for repairing
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_ERROR_OPENARCHIVEREAD      0x226

                                // could not open archive/volume for reading
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEReadArchiveData
                                //   ACEList
                                //   ACETest
                                //   ACEExtract
                                //   ACERepair


        #define ACE_CALLBACK_ERROR_OPENARCHIVEWRITE     0x227

                                // could not open archive/volume for writing
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEDelete
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord
                                //   ACERepair


        #define ACE_CALLBACK_ERROR_READARCHIVE          0x228

                                // error reading from archive
                                // (source disk removed?)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEReadArchiveData
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACERepair
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACEAddAV
                                //   ACELock
                                //   ACEAddRecoveryRecord


        #define ACE_CALLBACK_ERROR_WRITEARCHIVE         0x229

                                // error writing to archive
                                // (destination disk removed?)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEDelete
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord
                                //   ACERepair


        #define ACE_CALLBACK_ERROR_ALREADYSFX           0x22a

                                // ca not make to SFX: is already SFX
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAddSFX


        #define ACE_CALLBACK_ERROR_ADDSFXTOVOLUME       0x22b

                                // adding SFX to volumes not possible
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAddSFX


        #define ACE_CALLBACK_ERROR_ARCHIVEBROKEN        0x22c

                                // archive is broken (damaged)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEReadArchiveData
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACERepair
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACEAddAV
                                //   ACELock
                                //   ACEAddRecoveryRecord

        #define ACE_CALLBACK_ERROR_ARCHIVESAVE          0x22d

                                // not enough space to save archive;
                                // but normally
                                // ACE_CALLBACK_ERROR_ARCHIVESPACE
                                // should allow to change destination
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord
                                //   ACERepair


        #define ACE_CALLBACK_ERROR_NOFILES              0x22e

                                // no files specified/could not find files
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_ERROR_ISNOTANARCHIVE       0x22f

                                // specified archive file is not an
                                // ACE archive
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEReadArchiveData
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACERepair
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACEAddAV
                                //   ACELock
                                //   ACEAddRecoveryRecord


        #define ACE_CALLBACK_ERROR_TEMPDIRCREATE        0x230

                                // could not create file in temp directory
                                // (write protected or directory does
                                //  not exist)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord
                                //   ACERepair


        #define ACE_CALLBACK_ERROR_HIGHERVERSION        0x231

                                // this Dll version is not able to handle
                                // the archive
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord


        #define ACE_CALLBACK_ERROR_CREATIONNAMEINUSE    0x240

                                // name used by directory
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEExtract


        #define ACE_CALLBACK_ERROR_ENCRYPTIONCRC        0x242

                                // encryption failed because of CRC-Error at
                                // decompression
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_ERROR_READ                 0x243

                                // error reading file to be added
                                // (source disk removed?)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEAdd


        #define ACE_CALLBACK_ERROR_WRITE                0x244

                                // error at extraction
                                // (destination disk removed?)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEExtract


        #define ACE_CALLBACK_ERROR_OPENREAD             0x245

                                // error opening file for reading
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEAdd


        #define ACE_CALLBACK_ERROR_OPENWRITE            0x246

                                // error opening file for writing
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEExtract


        #define ACE_CALLBACK_ERROR_FILENAMETOOLONG      0x247

                                // resulting file name too long
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEAdd


        #define ACE_CALLBACK_ERROR_REPACKCRC            0x248

                                // CRC-check error at recompression
                                // (archive broken or wrong password)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEDelete
                                //   ACEAdd


        #define ACE_CALLBACK_ERROR_EXCLUDEPATH          0x249

                                // could not exclude path of file names; two
                                // or more files would have the same name
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEAdd


        #define ACE_CALLBACK_ERROR_METHOD               0x24a

                                // compression method not known to this
                                // Dll version
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_ERROR_EXTRACTSPACE         0x24b

                                // not enough space to extract file
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEExtract


        #define ACE_CALLBACK_ERROR_CREATION             0x24c

                                // creation failed (write-protection?)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEExtract


        #define ACE_CALLBACK_ERROR_OVERWRITEDELETE      0x24d

                                // could not overwrite because deletion of
                                // file failed
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEExtract


        #define ACE_CALLBACK_ERROR_MOVEDELETE           0x260

                                // deletion of file or directory failed
                                // (move operation)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_REALFILE
                                // operations:
                                //   ACEAdd

        #define ACE_CALLBACK_ERROR_TEMPDIRSPACE         0x270

                                // not enough space at current temp directory
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_SPACE
                                // operations:
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACEAddAV


        #define ACE_CALLBACK_ERROR_ARCHIVESPACE         0x271

                                // not enough space to save archive
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_SPACE
                                // operations:
                                //   ACEDelete
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACELock
                                //   ACEAddAV
                                //   ACEAddRecoveryRecord
                                //   ACERepair


        #define ACE_CALLBACK_ERROR_READINGSFXFILE       0x280

                                // error reading SFX file:
                                // is no SFX file,
                                // file does not exist or could not be opened
                                // for reading
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_SFXFILE
                                // operations:
                                //   ACEAdd
                                //   ACEAddSFX


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //================---  Part 2.7: request callback function  ---==============
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  // Declaration of ACERequestCallbackProc() parameter and explanation of
  // callback request codes.
  //---------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // Union parameter used at ACERequestCallbackProc().
      //-----------------------------------------------------------------------

        typedef union sACERequestCallbackProcStruc
        {
          ULONG  StructureType;                 // indicates which of the fol-
                                                // lowing structures is used
          tACECallbackGlobalStruc       Global;
          tACECallbackArchiveStruc      Archive;
          tACECallbackArchivedFileStruc ArchivedFile;
          tACECallbackRealFileStruc     RealFile;
        } tACERequestCallbackProcStruc;


      //-----------------------------------------------------------------------
      // Question constants are passed to the RequestCallbackProc callback
      // function to request further data.
      // RequestCallbackProc may return ACE_CALLBACK_RETURN_OK,
      // ACE_CALLBACK_RETURN_NO or ACE_CALLBACK_RETURN_CANCEL.
      //-----------------------------------------------------------------------

        #define ACE_CALLBACK_REQUEST_REGISTER           0x300

                                // Global.UserAV has to be set
                                // to continue registration process;
                                // not used in ACL
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_GLOBAL
                                // operations:
                                //   ACERegister


        #define ACE_CALLBACK_REQUEST_MARKASSOLID        0x320

                                // ArchiveHeader damaged,
                                // set solid flag for the new archive?
                                // (in case of doubt return yes!)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_REQUEST_CHANGEVOLUME       0x321
                                // Asks for permission to process next volume.
                                // If operation is ACE_CALLBACK_OPERATION_ADD
                                // then a new volume will be created.
                                // The application may change the name
                                // of the archive by modifying
                                // ArchiveData->ArchiveName
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEDelete
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEList
                                //   ACETest
                                //   ACEExtract


        #define ACE_CALLBACK_REQUEST_ARCHIVEEXISTS      0x322
                                // Asks whether to overwrite a file with
                                // the same name as the archive.
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEDelete
                                //   ACEAdd
                                //   ACESetComments
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_REQUEST_OVERWRITE          0x340

                                // Overwrite existing file?
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEAdd
                                //   ACEExtract


        #define ACE_CALLBACK_REQUEST_DELARCHIVEDSYSFILE 0x341

                                // Delete rdonly/hidden/system file
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEDelete


        #define ACE_CALLBACK_REQUEST_ADDBROKENFILE      0x342

                                // repair function found file with
                                // broken header, add file?
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACERepair


        #define ACE_CALLBACK_REQUEST_PASSWORD           0x343

                                // password required; attention: may be
                                // decryption _and_ encryption; but  passwords
                                // can be different --> better changing
                                // passwords at StateCallbackProc
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_REQUEST_OVERWRITESYSFILE   0x344

                                // Overwrite rdonly/hidden/system file
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEAdd
                                //   ACEExtract


        #define ACE_CALLBACK_REQUEST_MOVEDELREALSYSFILE 0x360

                                // Delete rdonly/hidden/system file
                                // (move to archive operation)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_REALFILE
                                // operations:
                                //   ACEAdd


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //=================---  Part 2.8: state callback function  ---===============
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  // Declaration of ACEStateCallbackProc() parameter and explanation of
  // callback state codes.
  //---------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // Union parameter used at ACEStateCallbackProc().
      //-----------------------------------------------------------------------

        typedef union sACEStateCallbackProcStruc
        {
          ULONG  StructureType;                 // indicates which of the fol-
                                                // lowing structures is used
          tACECallbackArchiveStruc      Archive;
          tACECallbackArchivedFileStruc ArchivedFile;
          tACECallbackArchivedFileStruc RealFile;
          tACECallbackProgressStruc     Progress;
          tACECallbackCRCCheckStruc     CRCCheck;
        } tACEStateCallbackProcStruc;


      //-----------------------------------------------------------------------
      // Calls to (*StateCallbackProc)() with ACE_CALLBACK_STATE values in the
      // Code field are made to enable the application to show the progress of
      // an operation.
      //-----------------------------------------------------------------------

        #define ACE_CALLBACK_STATE_STARTARCHIVE         0x400

                                // procession of archive is about to begin
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVE
                                // operations:
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACERepair
                                //   ACESetComments
                                //   ACEEncryptFiles
                                //   ACEAddSFX
                                //   ACEAddAV
                                //   ACELock
                                //   ACEAddRecoveryRecord


        #define ACE_CALLBACK_STATE_STARTFILE            0x410

                                // procession of file is about to begin
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEList
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACERepair
                                //   ACESetComments
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_STATE_ENDNOCRCCHECK        0x411

                                // end of file procession
                                // (no CRC chceck for this operation)
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_ARCHIVEDFILE
                                // operations:
                                //   ACEList
                                //   ACEDelete
                                //   ACEAdd
                                //   ACERepair
                                //   ACESetComments
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_STATE_PROGRESS             0x420

                                // informs about the progress of a file
                                // operation
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_PROGRESS
                                // operations:
                                //   ACEDelete
                                //   ACETest
                                //   ACEExtract
                                //   ACEAdd
                                //   ACERepair
                                //   ACEEncryptFiles


        #define ACE_CALLBACK_STATE_ENDCRCCHECK          0x430

                                // end of file procession, CRC-check
                                // result is passed
                                //---------------------------------------------
                                // structure type:
                                //   ACE_CALLBACK_TYPE_CRCCHECK
                                // operations:
                                //   ACETest
                                //   ACEExtract
                                //   ACEDelete
                                //   ACEAdd




//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//ออออออออ=====---  Part 3: Functions supported by UNAceV2.Dll ---====ออออออออออออ
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

  //---------------------------------------------------------------------------
  //
  // UnAceV2.Dll supports the following functions:
  //
  //   ACEInitDll
  //   ACEReadArchiveData
  //   ACEList
  //   ACETest
  //   ACEExtract
  //
  // First of all - before any other function is called - the Dll has to be
  // initialized by ACEInitDll(). Using this function the application has
  // to set temporary directory, key file path, comment buffer and
  // callback function pointers.
  // (callback function pointers can be set to NULL for first-try-runs)
  //
  // See description of each function for details about their task, input and
  // output. Return codes of those functions are listed in part 3.1.
  //
  //---------------------------------------------------------------------------
  //
  // Contents:
  //   Part 3.1: ACE.DLL function return codes
  //   Part 3.2: functions and parameter structures
  //
  //---------------------------------------------------------------------------


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //=============---  Part 3.1: ACE.DLL function return codes  ---=============
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

      //-----------------------------------------------------------------------
      // These error codes are returned by the ACE.DLL-functions. The meanings
      // of the codes are the same, as they are for the exit codes of ACE.EXE.
      //-----------------------------------------------------------------------

        #define ACE_ERROR_NOERROR       0   // no error; operation succesful
        #define ACE_ERROR_MEM           1   // insufficient memory
        #define ACE_ERROR_FILES         2   // no files specified
        #define ACE_ERROR_FOUND         3   // specified archive not found
        #define ACE_ERROR_FULL          4   // disk full
        #define ACE_ERROR_OPEN          5   // could not open file
        #define ACE_ERROR_READ          6   // read error
        #define ACE_ERROR_WRITE         7   // write error
        #define ACE_ERROR_CLINE         8   // invalid command line
        #define ACE_ERROR_CRC           9   // CRC error
        #define ACE_ERROR_OTHER         10  // other error
        #define ACE_ERROR_EXISTS        11  // file already exists
        #define ACE_ERROR_USER          255 // user break (application
                                            // returned cancel code at
                                            // callback function)


      //-----------------------------------------------------------------------
      // These error codes are returned by the ACE.DLL-functions. They are not
      // used by ACE.EXE yet.
      //-----------------------------------------------------------------------

        #define ACE_ERROR_PARAM       128 // might be used later


  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  //============---  Part 3.2: functions and parameter structures ---==========
  //อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  ACEInitDll  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Initializes ACE dynamic link library. Has to be called before any
    // other function call. May be called more than one time.
    //-------------------------------------------------------------------------
    // Input :
    // Output:
    //-------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // ACEInitDll() parameter structure.
      //-----------------------------------------------------------------------

        typedef struct sACEInitDllStruc
        {
          tACEGlobalDataStruc
                GlobalData;
          CHAR  Reserved[64];         // has to be filled with zeroes
        } tACEInitDllStruc,
         *pACEInitDllStruc;


      //-----------------------------------------------------------------------
      // ACEInitDll() function declaration.
      //-----------------------------------------------------------------------

        INT __stdcall ACEInitDll(pACEInitDllStruc DllData);


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  ACEReadArchiveData  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Tests a file whether it is an archive or not and reads out the archive
    // data.
    //-------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // ACEReadArchiveData() parameter structure.
      //-----------------------------------------------------------------------

        typedef struct sACEReadArchiveDataStruc
        {
          pACEArchiveDataStruc        // if this pointer is NULL, the
                ArchiveData;          // file passed to ACEReadArchiveData
                                      // is no archive; otherwise it points
                                      // to a tACEArchiveDataStruc structure
                                      // that contains information about the
                                      // archive
          CHAR  Reserved[64];         // has to be filled with zeroes
        } tACEReadArchiveDataStruc,
         *pACEReadArchiveDataStruc;


      //-----------------------------------------------------------------------
      // ACEReadArchiveData() function declaration.
      //-----------------------------------------------------------------------

        INT __stdcall ACEReadArchiveData(LPSTR ArchiveName,
                                         pACEReadArchiveDataStruc ArchiveData);


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  ACEList  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Passes the specified files in the archive to StateCallbackProc().
    //-------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // ACEList() parameter structure.
      //-----------------------------------------------------------------------

        typedef struct sACEListStruc
        {
          tACEFilesStruc              // specifies files to be listed;
                Files;                // see tACEFilesStruc structure
          CHAR  Reserved[64];         // has to be filled with zeroes
        } tACEListStruc,
         *pACEListStruc;


      //-----------------------------------------------------------------------
      // ACEList() function declaration.
      //-----------------------------------------------------------------------

        INT __stdcall ACEList(LPSTR ArchiveName, pACEListStruc List);


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  ACETest  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Tests specified files in archive.
    //-------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // ACETest() parameter structure.
      //-----------------------------------------------------------------------

        typedef struct sACETestStruc
        {
          tACEFilesStruc              // specifies files to test;
                Files;                // see tACEFilesStruc structure
          LPSTR DecryptPassword;      // zero-terminated string,
                                      // case-sensitive (maxlen=56)
          CHAR  Reserved[64];         // has to be filled with zeroes
        } tACETestStruc,
         *pACETestStruc;


      //-----------------------------------------------------------------------
      // ACETest() function declaration.
      //-----------------------------------------------------------------------

        INT __stdcall ACETest(LPSTR ArchiveName, pACETestStruc Test);


    //ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ  ACEExtract  ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
    // Extracts specified  files.
    //-------------------------------------------------------------------------

      //-----------------------------------------------------------------------
      // ACEExtract() parameter structure.
      //-----------------------------------------------------------------------

        typedef struct sACEExtractStruc
        {
          tACEFilesStruc                // specifies files to extract;
                  Files;                // see tACEFilesStruc structure
          LPSTR   DestinationDir;       // directory to extract files to
          BOOL    ExcludePath;          // extract files without path
          LPSTR   DecryptPassword;      // password for decryption (if files
                                        // are encrypted);
                                        // zero-terminated string,
                                        // case-sensitive (maxlen=56)
          CHAR    Reserved[64];         // has to be filled with zeroes
        } tACEExtractStruc,
         *pACEExtractStruc;


      //-----------------------------------------------------------------------
      // ACEExtract() function declaration.
      //-----------------------------------------------------------------------

        INT __stdcall ACEExtract(LPSTR ArchiveName,
                                 pACEExtractStruc Extract);


#ifdef __cplusplus
 }
#endif
#endif

