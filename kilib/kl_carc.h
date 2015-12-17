//--- K.I.LIB ---
// kl_carc.h : handling "common archivers' dll"

#ifndef AFX_KIARCDLLRAW_H__C94DE2A0_4292_49CE_8471_2CAA1340D216__INCLUDED_
#define AFX_KIARCDLLRAW_H__C94DE2A0_4292_49CE_8471_2CAA1340D216__INCLUDED_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// 統合アーカイバDLL共通の定義

// FNAME_MAX
#if !defined(FNAME_MAX32)
#define FNAME_MAX32	512
#define	FNAME_MAX	FNAME_MAX32
#else
#if !defined(FNAME_MAX)
#define	FNAME_MAX	128
#endif
#endif

// CHECKARCHIVE
#if !defined(CHECKARCHIVE_RAPID)
#define	CHECKARCHIVE_RAPID		0
#define	CHECKARCHIVE_BASIC		1
#define	CHECKARCHIVE_FULLCRC	2
#endif

// ISARC
#if !defined(ISARC_FUNCTION_START)
#define ISARC_FUNCTION_START			0
#define ISARC							0
#define ISARC_GET_VERSION				1
#define ISARC_GET_CURSOR_INTERVAL		2
#define ISARC_SET_CURSOR_INTERVAL		3
#define ISARC_GET_BACK_GROUND_MODE		4
#define ISARC_SET_BACK_GROUND_MODE		5
#define ISARC_GET_CURSOR_MODE			6
#define ISARC_SET_CURSOR_MODE			7
#define ISARC_GET_RUNNING				8

#define ISARC_CHECK_ARCHIVE				16
#define ISARC_CONFIG_DIALOG				17
#define ISARC_GET_FILE_COUNT			18
#define ISARC_QUERY_FUNCTION_LIST		19
#define ISARC_HOUT						20
#define ISARC_STRUCTOUT					21
#define ISARC_GET_ARC_FILE_INFO			22

#define ISARC_OPEN_ARCHIVE				23
#define ISARC_CLOSE_ARCHIVE				24
#define ISARC_FIND_FIRST				25
#define ISARC_FIND_NEXT					26
#define ISARC_EXTRACT					27
#define ISARC_ADD						28
#define ISARC_MOVE						29
#define ISARC_DELETE					30
#define ISARC_SETOWNERWINDOW			31
#define ISARC_CLEAROWNERWINDOW			32
#define ISARC_SETOWNERWINDOWEX			33
#define ISARC_KILLOWNERWINDOWEX			34

#define ISARC_GET_ARC_FILE_NAME			40
#define ISARC_GET_ARC_FILE_SIZE			41
#define ISARC_GET_ARC_ORIGINAL_SIZE		42
#define ISARC_GET_ARC_COMPRESSED_SIZE	43
#define ISARC_GET_ARC_RATIO				44
#define ISARC_GET_ARC_DATE				45
#define ISARC_GET_ARC_TIME				46
#define ISARC_GET_ARC_OS_TYPE			47
#define ISARC_GET_ARC_IS_SFX_FILE		48
#define ISARC_GET_ARC_WRITE_TIME_EX		49
#define ISARC_GET_ARC_CREATE_TIME_EX	50
#define	ISARC_GET_ARC_ACCESS_TIME_EX	51
#define	ISARC_GET_ARC_CREATE_TIME_EX2	52
#define ISARC_GET_ARC_WRITE_TIME_EX2	53
#define ISARC_GET_FILE_NAME				57
#define ISARC_GET_ORIGINAL_SIZE			58
#define ISARC_GET_COMPRESSED_SIZE		59
#define ISARC_GET_RATIO					60
#define ISARC_GET_DATE					61
#define ISARC_GET_TIME					62
#define ISARC_GET_CRC					63
#define ISARC_GET_ATTRIBUTE				64
#define ISARC_GET_OS_TYPE				65
#define ISARC_GET_METHOD				66
#define ISARC_GET_WRITE_TIME			67
#define ISARC_GET_CREATE_TIME			68
#define ISARC_GET_ACCESS_TIME			69
#define ISARC_GET_WRITE_TIME_EX			70
#define ISARC_GET_CREATE_TIME_EX		71
#define ISARC_GET_ACCESS_TIME_EX		72
#define ISARC_SET_ENUM_MEMBERS_PROC		80
#define ISARC_CLEAR_ENUM_MEMBERS_PROC	81

#define ISARC_FUNCTION_END				81
#endif

// ERROR
#if !defined(ERROR_START)
#define ERROR_START				0x8000
	// warning
#define ERROR_DISK_SPACE		0x8005
#define ERROR_READ_ONLY			0x8006
#define ERROR_USER_SKIP			0x8007
#define ERROR_UNKNOWN_TYPE		0x8008
#define ERROR_METHOD			0x8009
#define ERROR_PASSWORD_FILE		0x800A
#define ERROR_VERSION			0x800B
#define ERROR_FILE_CRC			0x800C
#define ERROR_FILE_OPEN			0x800D
#define ERROR_MORE_FRESH		0x800E
#define ERROR_NOT_EXIST			0x800F
#define ERROR_ALREADY_EXIST		0x8010

#define ERROR_TOO_MANY_FILES	0x8011
	// error
#define ERROR_MAKEDIRECTORY		0x8012
#define ERROR_CANNOT_WRITE		0x8013
#define ERROR_HUFFMAN_CODE		0x8014
#define ERROR_COMMENT_HEADER	0x8015
#define ERROR_HEADER_CRC		0x8016
#define ERROR_HEADER_BROKEN		0x8017
#define ERROR_ARC_FILE_OPEN		0x8018
#define ERROR_NOT_ARC_FILE		0x8019
#define ERROR_CANNOT_READ		0x801A
#define ERROR_FILE_STYLE		0x801B
#define ERROR_COMMAND_NAME		0x801C
#define ERROR_MORE_HEAP_MEMORY	0x801D
#define ERROR_ENOUGH_MEMORY		0x801E
#if !defined(ERROR_ALREADY_RUNNING)
#define ERROR_ALREADY_RUNNING	0x801F
#endif
#define ERROR_USER_CANCEL		0x8020
#define ERROR_HARC_ISNOT_OPENED	0x8021
#define ERROR_NOT_SEARCH_MODE	0x8022
#define ERROR_NOT_SUPPORT		0x8023
#define ERROR_TIME_STAMP		0x8024
#define ERROR_TMP_OPEN			0x8025
#define ERROR_LONG_FILE_NAME	0x8026
#define ERROR_ARC_READ_ONLY		0x8027
#define ERROR_SAME_NAME_FILE	0x8028
#define ERROR_NOT_FIND_ARC_FILE 0x8029
#define ERROR_RESPONSE_READ		0x802A
#define ERROR_NOT_FILENAME		0x802B
#define ERROR_TMP_COPY			0x802C
#define ERROR_EOF				0x802D
#define ERROR_ADD_TO_LARC		0x802E
#define ERROR_TMP_BACK_SPACE	0x802F
#define ERROR_SHARING			0x8030
#define ERROR_NOT_FIND_FILE		0x8031
#define ERROR_LOG_FILE			0x8032
#define	ERROR_NO_DEVICE			0x8033
#define ERROR_GET_ATTRIBUTES	0x8034
#define ERROR_SET_ATTRIBUTES	0x8035
#define ERROR_GET_INFORMATION	0x8036
#define ERROR_GET_POINT			0x8037
#define ERROR_SET_POINT			0x8038
#define ERROR_CONVERT_TIME		0x8039
#define ERROR_GET_TIME			0x803a
#define ERROR_SET_TIME			0x803b
#define ERROR_CLOSE_FILE		0x803c
#define ERROR_HEAP_MEMORY		0x803d
#define ERROR_HANDLE			0x803e
#define ERROR_TIME_STAMP_RANGE	0x803f
#define ERROR_MAKE_ARCHIVE		0x8040

#define ERROR_END	ERROR_MAKE_ARCHIVE
#define ERROR_BUF_TOO_SMALL		0x8041 /**/
#endif

// CONFIG
#if !defined(UNPACK_CONFIG_MODE)
#define	UNPACK_CONFIG_MODE	1
#define	PACK_CONFIG_MODE	2
#endif

// OPENARCHIVE
#if !defined(EXTRACT_FOUND_FILE)
#define M_INIT_FILE_USE			0x00000001L
#define M_REGARDLESS_INIT_FILE	0x00000002L
#define M_NO_BACKGROUND_MODE	0x00000004L
#define M_NOT_USE_TIME_STAMP	0x00000008L
#define M_EXTRACT_REPLACE_FILE	0x00000010L
#define M_EXTRACT_NEW_FILE		0x00000020L
#define M_EXTRACT_UPDATE_FILE	0x00000040L
#define M_CHECK_ALL_PATH		0x00000100L
#define M_CHECK_FILENAME_ONLY	0x00000200L
#define M_CHECK_DISK_SIZE		0x00000400L
#define M_REGARDLESS_DISK_SIZE	0x00000800L
#define M_USE_DRIVE_LETTER		0x00001000L
#define M_NOT_USE_DRIVE_LETTER	0x00002000L
#define M_INQUIRE_DIRECTORY		0x00004000L
#define M_NOT_INQUIRE_DIRECTORY 0x00008000L
#define M_INQUIRE_WRITE			0x00010000L
#define M_NOT_INQUIRE_WRITE		0x00020000L
#define M_CHECK_READONLY		0x00040000L
#define M_REGARDLESS_READONLY	0x00080000L
#define M_REGARD_E_COMMAND		0x00100000L
#define M_REGARD_X_COMMAND		0x00200000L
#define M_ERROR_MESSAGE_ON		0x00400000L
#define M_ERROR_MESSAGE_OFF		0x00800000L
#define M_BAR_WINDOW_ON			0x01000000L
#define M_BAR_WINDOW_OFF		0x02000000L
#define M_CHECK_PATH			0x04000000L
#define M_RECOVERY_ON			0x08000000L

#define M_MAKE_INDEX_FILE		0x10000000L
#define M_NOT_MAKE_INDEX_FILE	0x20000000L
#define EXTRACT_FOUND_FILE		0x40000000L
#define EXTRACT_NAMED_FILE		0x80000000L
#endif

// ATTRIBUTE
#ifndef FA_RDONLY
#define FA_RDONLY       0x01
#define FA_HIDDEN       0x02
#define FA_SYSTEM       0x04
#define FA_LABEL        0x08
#define FA_DIREC        0x10
#define FA_ARCH         0x20
#endif
#ifndef FA_ENCRYPTED
#define FA_ENCRYPTED    0x40
#endif

// STRUCTURES
#if defined(__BORLANDC__)
#pragma option -a-
#else
#pragma pack(1)
#endif

typedef	HGLOBAL	HARCHIVE;

#ifndef ARC_DECSTRACT
#define ARC_DECSTRACT
typedef	HGLOBAL	HARC;

typedef struct {
	DWORD	dwOriginalSize;
	DWORD	dwCompressedSize;
	DWORD	dwCRC;
	UINT	uFlag;
	UINT	uOSType;
	WORD	wRatio;
	WORD	wDate;
	WORD	wTime;
	char	szFileName[FNAME_MAX32 + 1];
	char	dummy1[3];
	char	szAttribute[8];
	char	szMode[8];
} INDIVIDUALINFO, FAR *LPINDIVIDUALINFO;

typedef struct {
	DWORD	dwFileSize;
	DWORD	dwWriteSize;
	char	szSourceFileName[FNAME_MAX32 + 1];
	char	dummy1[3];
	char	szDestFileName[FNAME_MAX32 + 1];
	char	dummy[3];
} EXTRACTINGINFO, FAR *LPEXTRACTINGINFO;

typedef struct {
	EXTRACTINGINFO exinfo;
	DWORD dwCompressedSize;
	DWORD dwCRC;
	UINT  uOSType;
	WORD  wRatio;
	WORD  wDate;
	WORD  wTime;
	char  szAttribute[8];
	char  szMode[8];
} EXTRACTINGINFOEX, *LPEXTRACTINGINFOEX;
#endif

#if !defined(__BORLANDC__)
#pragma pack()
#else
#pragma option -a.
#endif

#if !defined(__BORLANDC__)
#define	_export
#endif

// WindowsMessage
#ifndef WM_ARCEXTRACT
#define	WM_ARCEXTRACT	"wm_arcextract"
#define	ARCEXTRACT_BEGIN		0
#define	ARCEXTRACT_INPROCESS	1
#define	ARCEXTRACT_END			2
#define ARCEXTRACT_OPEN			3
#define ARCEXTRACT_COPY			4
typedef BOOL CALLBACK ARCHIVERPROC(HWND,UINT,UINT,LPEXTRACTINGINFOEX);
typedef ARCHIVERPROC *LPARCHIVERPROC;
#endif

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// 統合アーカイバDLLを非常に薄く覆うレイヤ

class kiArcDLLRaw  
{
public: //-- 外向きインターフェイス --------------------------

	// DLL名で初期化。
	kiArcDLLRaw( const char* dllname );
	kiPath& name()
		{ return m_DllPath; }

	// DLLが生きているかどうか。
	bool isAlive();

	// API のラッパ [ DLLやAPIが無いときの返値 ]

		// バージョンx100 [ 0 ]
		WORD getVer();
		// バージョンサブx100 [ 0 ]
		WORD getVerSub();
		// コマンド送り [ ERROR_NOT_SUPPORT ]
		int command( const HWND wnd, LPCSTR cmd, LPSTR buf, const DWORD siz );
		// 書庫チェック [ FALSE ]
		BOOL check( LPCSTR filename, const int mode );
		// 書庫の種類 [ 0 ]
		int getArcType( LPCSTR filename );

		// 開く [ NULL ]
		HARC openArc( const HWND wnd, LPCSTR arcname, const DWORD flag );
		// 閉じる
		void closeArc( HARC arc );
		// 一つ目を検索 [ ERROR_NOT_SUPPORT ]
		int findfirst( HARC arc, LPCSTR wildname, INDIVIDUALINFO* inf );
		// 二つ目以降を検索 [ ERROR_NOT_SUPPORT ]
		int findnext( HARC arc, INDIVIDUALINFO* inf );
		// 属性取得 [ 0 ]
		int getAttr( HARC arc );

		// オーナー指定 [ FALSE ]
		BOOL setOwner( HWND wnd );
		// オーナー解除 [ FALSE ]
		BOOL clearOwner();

	// ※load系は呼ばなくても内部で何とかするのでできれば使わないこと。
	bool load();
	void unload();

private: //-- 内部処理 -----------------------------------

	HINSTANCE m_hDLL;
	bool not_loaded_yet;

	kiPath m_DllPath;
	kiStr m_DllNameBody;
	FARPROC getProc( const char* procname );

	FARPROC m_Proc[ ISARC_FUNCTION_END ];
	FARPROC f_VSb;
#define f_Cmd m_Proc[ ISARC ]
#define f_Chk m_Proc[ ISARC_GET_VERSION ]
#define f_Gat m_Proc[ 13 ]
#define f_Ver m_Proc[ ISARC_CHECK_ARCHIVE ]
#define f_Opn m_Proc[ ISARC_OPEN_ARCHIVE ]
#define f_Cls m_Proc[ ISARC_CLOSE_ARCHIVE ]
#define f_Ffs m_Proc[ ISARC_FIND_FIRST ]
#define f_Fnx m_Proc[ ISARC_FIND_NEXT ]
#define f_GAr m_Proc[ ISARC_GET_ATTRIBUTE ]
#define f_SOw m_Proc[ ISARC_SETOWNERWINDOW ]
#define f_COw m_Proc[ ISARC_CLEAROWNERWINDOW ]

public:
	virtual ~kiArcDLLRaw();
};

#endif
