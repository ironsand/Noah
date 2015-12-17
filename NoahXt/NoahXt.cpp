// NoahXt.cpp
// -- all of 'NoahXt.dll' (ShellExtension && Configuration of Noah)

#undef   WINVER
#define  WINVER   0x0400
#ifndef  STRICT
#define  STRICT
#endif
#define  INC_OLE2
#undef  _WIN32_IE
#define _WIN32_IE 0x0200

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <lmaccess.h>

// カレントディレクトリを安全なところに移してLoadLibrary
static HMODULE safepathLoadLibrary(LPCTSTR lpFileName)
{
	char original_cur[MAX_PATH], sys[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, original_cur);
	::GetSystemDirectory(sys, MAX_PATH);
	::SetCurrentDirectory(sys);
	HMODULE han = ::LoadLibrary(lpFileName);
	::SetCurrentDirectory(original_cur);
	return han;
}

//-------------------------------------------------------
//-- Noah Config API ------------------------------------
//-------------------------------------------------------



bool WINAPI Init();
void WINAPI LoadSE( bool* a, bool* x );
void WINAPI SaveSE( bool a, bool x );
void WINAPI LoadAS( bool asso[] );
void WINAPI SaveAS( bool asso[] );
void WINAPI LoadASEx( const char* ext, bool* x );
void WINAPI SaveASEx( const char* ext, bool x );



//-------------------------------------------------------
//-- グローバル変数 -------------------------------------
//-------------------------------------------------------



char g_szNoah[MAX_PATH]; // Where is Noah ?
char  g_szDLL[MAX_PATH]; // What's my name ?
bool             g_bJpn; // am I in Japanese Mode ?
bool             g_isNT; // is Windows NT/2000 ?
int              g_cRef; // reference counter
bool         g_bChanged; // association changed ?



//-------------------------------------------------------
//-- GUID {953AFAE9-C2A9-4674-9811-D7E281B001E1} --------
//-------------------------------------------------------



static const GUID CLSID_NoahXt =
	{ 0x953afae9, 0xc2a9, 0x4674, { 0x98, 0x11, 0xd7, 0xe2, 0x81, 0xb0, 0x1, 0xe1 } };
static const char* ProgID_NoahXt = "NoahXt";



//--------------------------------------------------------
//-- シェルエクステンション・本体 --------------------------
//--------------------------------------------------------



class noahXt : public IContextMenu, IShellExtInit
{
public:
	noahXt()						{ m_szDir[0]=0; m_pDataObj=NULL; m_cRef=0L; g_cRef++; }
	~noahXt()						{ if( m_pDataObj ) m_pDataObj->Release(); g_cRef--; }
	STDMETHODIMP_(ULONG) AddRef()	{ return (++m_cRef); }
	STDMETHODIMP_(ULONG) Release()	{ if( --m_cRef )return m_cRef; delete this; return 0L; }
	STDMETHODIMP QueryInterface( REFIID riid, void** ppv )
		{
			*ppv = NULL;
			AddRef();
			if( IsEqualIID( riid, IID_IUnknown ) || IsEqualIID( riid, IID_IShellExtInit ) )
				*ppv = (IShellExtInit*)this;
			else if( IsEqualIID( riid, IID_IContextMenu ) )
				*ppv = (IContextMenu*)this;
			else
			{
				Release();
				return E_NOINTERFACE;
			}
			return NOERROR;
		}
	STDMETHODIMP Initialize( const ITEMIDLIST* pF, IDataObject* pD, HKEY )
		{
			::SHGetPathFromIDList( pF, m_szDir );
			if( m_pDataObj )m_pDataObj->Release();
			if( pD )	(m_pDataObj=pD)->AddRef();
			return NOERROR;
		}

#define CMPR_CMD_E ("Com&press Here")
#define EXTR_CMD_E ("E&xtract Here")
#define CMPR_CMD   (g_bJpn ? "ここに圧縮(&P)" : CMPR_CMD_E)
#define EXTR_CMD   (g_bJpn ? "ここに解凍(&X)" : EXTR_CMD_E)
#define CMPR_HLP   (g_bJpn ? "ファイルをNoahで圧縮します。" : "Compress These Files By Noah")
#define EXTR_HLP   (g_bJpn ? "ファイルをNoahで展開" : "Extract Files By Noah")

	// 右クリックメニューへ追加
	STDMETHODIMP QueryContextMenu( HMENU h, UINT i, UINT id, UINT idLast, UINT flag )
		{
			if( (flag&0x000F)!=CMF_NORMAL && !(flag&CMF_VERBSONLY) && !(flag&CMF_EXPLORE) )
				return NOERROR;

			// レジストリから設定読み込み
			// できれば、ここでm_bEXTに関しては拡張子判定を行いたいところ…(^^;
			LoadSE( &m_bCMP, &m_bEXT );

			if( m_bCMP ) ::InsertMenu( h, i++, MF_STRING|MF_BYPOSITION, id++, CMPR_CMD );
			if( m_bEXT ) ::InsertMenu( h, i++, MF_STRING|MF_BYPOSITION, id++, EXTR_CMD );
			return MAKE_HRESULT( SEVERITY_SUCCESS, 0, id );
		}
	// コマンド実行
	STDMETHODIMP InvokeCommand( LPCMINVOKECOMMANDINFO lpcmi )
		{
			if( 0==HIWORD(lpcmi->lpVerb) )
				switch( filter_cmd( LOWORD(lpcmi->lpVerb) ) )
				{
				case 0: return operation( "-a" );
				case 1: return operation( "-x" );
				}
			return E_INVALIDARG;
		}
	// ヘルプ文字列など
	STDMETHODIMP GetCommandString( UINT cmd, UINT flag, UINT*, LPSTR pszName, UINT cchMax )
		{
			cmd = filter_cmd( cmd );
			if( cmd==2 )
				return E_FAIL;
			switch( flag )
			{
			case GCS_HELPTEXT:	::lstrcpyn( pszName, cmd==0 ? CMPR_HLP : EXTR_HLP, cchMax ); break;
			case GCS_VERB:		::lstrcpyn( pszName, cmd==0 ? CMPR_CMD_E : EXTR_CMD_E, cchMax ); break;
			}
			return NOERROR;
		}
private:
	// コマンドIDを compress=0, melt=1, else=2 にフィルタリング
	UINT filter_cmd( UINT i )
		{
			if( m_bCMP )if( m_bEXT )return (i<=1 ? i : 2);
						else		return (i==0 ? 0 : 2);
			else		if( m_bEXT )return (i==0 ? 1 : 2);
						else		return 2;
		}
	// Noah.exeへ渡す処理 "Compress Here" or "Extract Here"
	STDMETHODIMP operation( const char* opt )
		{
			STGMEDIUM md;
			FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
			if( SUCCEEDED( m_pDataObj->GetData( &fe, &md ) ) )
			{
				const HDROP& hDrop = (HDROP)md.hGlobal;
				const int num = ::DragQueryFile( hDrop, 0xffffffff, NULL, 0 );
				if( num )
				{
					// main command
					char* cmd = new char[10 + MAX_PATH * (num+2)];
					::lstrcpy( cmd, g_szNoah );
					::lstrcat( cmd, " " );
					::lstrcat( cmd, opt );
					// destdir
					::lstrcat( cmd, " \"-D" );
					::lstrcat( cmd, m_szDir );
					::lstrcat( cmd, "\"" );
					// filelist
					char str[MAX_PATH];
					for( int i=0; i!=num; i++ )
					{
						::DragQueryFile( hDrop, i, str, sizeof(str) );
						::lstrcat( cmd, " \"" );
						::lstrcat( cmd, str );
						::lstrcat( cmd, "\"" );
					}
					// call 'Noah'
					::WinExec( cmd, SW_SHOWDEFAULT );
					delete [] cmd;
				}
				::ReleaseStgMedium( &md );
			}
			return NOERROR;
		}
private:
	ULONG        m_cRef;
	IDataObject* m_pDataObj;
	char		 m_szDir[MAX_PATH];
	bool         m_bCMP, m_bEXT;
};

//-- クラス工場 -------------------------------------------

class noahXtClassFactory : public IClassFactory
{
public:
	noahXtClassFactory()			{ m_cRef = 0L; g_cRef++; }
	~noahXtClassFactory()			{ g_cRef--; }
	STDMETHODIMP_(ULONG) AddRef()	{ return (++m_cRef); }
	STDMETHODIMP_(ULONG) Release()	{ if( --m_cRef )return m_cRef; delete this; return 0L; }
	STDMETHODIMP LockServer( BOOL )	{ return NOERROR; }
	STDMETHODIMP QueryInterface( REFIID riid, void** ppv )
		{
			*ppv = NULL;
			AddRef();
			if( IsEqualIID( riid, IID_IUnknown ) || IsEqualIID( riid, IID_IClassFactory ) )
			{
				*ppv = (IClassFactory*)this;
				return NOERROR;
			}
			Release();
			return E_NOINTERFACE;
		}
	STDMETHODIMP CreateInstance( IUnknown* pOuter, REFIID riid, void** ppvObj )
		{
			*ppvObj = NULL;
			if( pOuter )
				return CLASS_E_NOAGGREGATION;
			noahXt* pXt = new noahXt;
			return pXt ? pXt->QueryInterface( riid, ppvObj ) : E_OUTOFMEMORY;
		}
private:
	ULONG m_cRef;
};

//-- システム向けAPI ---------------------------------------

extern "C" int APIENTRY
DllMain( HINSTANCE inst, DWORD why, LPVOID reserved )
{
	if( why==DLL_PROCESS_ATTACH )
	{
		::GetModuleFileName( inst, g_szDLL, sizeof(g_szDLL) );
		::lstrcpy( g_szNoah, g_szDLL );
		for( char *p=g_szNoah,*y=g_szNoah-1; *p; p=::CharNext(p) )
			if( *p=='\\' )
				y=p;
		::lstrcpy( y+1, "Noah.exe" );
		::GetShortPathName( g_szNoah, g_szNoah, MAX_PATH );

		OSVERSIONINFO osVer;
		osVer.dwOSVersionInfoSize = sizeof(osVer);
		::GetVersionEx( &osVer );
		g_isNT = ( osVer.dwPlatformId == VER_PLATFORM_WIN32_NT );

		g_bJpn = (::GetUserDefaultLangID()==0x0411);
		g_cRef = 0;
		g_bChanged = false;
	}
	else if( why==DLL_PROCESS_DETACH )
	{
		if( g_bChanged )
			::SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL );
	}
	return TRUE;
}

STDAPI DllCanUnloadNow()
{
    return ( g_cRef==0 ? S_OK : S_FALSE );
}

STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, void** ppvOut )
{
	*ppvOut = NULL;
	if( IsEqualIID( rclsid, CLSID_NoahXt ) )
	{
		noahXtClassFactory* p = new noahXtClassFactory;
		return p->QueryInterface( riid, ppvOut );
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}



//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------


//-- kiRegKey from K.I.LIB -------------------------//
//------ 'not using kiStr && REG_SZ only' version --//


class kiRegKey
{
public:
	kiRegKey()
		{ m_hKey = NULL; }
	~kiRegKey()
		{ close(); }
	operator HKEY() const
		{ return m_hKey; }
	bool open( HKEY parent, LPCTSTR keyname, REGSAM access )
		{ close(); return (ERROR_SUCCESS == ::RegOpenKeyEx( parent, keyname, 0, access, &m_hKey )); }
	bool create( HKEY parent, LPCTSTR keyname, REGSAM access )
		{ close(); DWORD x; return (ERROR_SUCCESS == ::RegCreateKeyEx( parent, keyname, 0, REG_NONE, REG_OPTION_NON_VOLATILE, access, NULL, &m_hKey, &x )); }
	void close()
		{ if( m_hKey ) ::RegCloseKey( m_hKey ); }
	static bool exist( HKEY parent, LPCTSTR keyname )
		{ HKEY k; if( ERROR_SUCCESS!=::RegOpenKeyEx( parent,keyname,0,KEY_READ,&k ) ) return false; ::RegCloseKey( k ); return true; }
	bool set( LPCTSTR valname, LPCTSTR val )
		{ return (ERROR_SUCCESS == ::RegSetValueEx( m_hKey, valname, 0, REG_SZ, (BYTE*)val, ::lstrlen(val)+1 )); }
	bool get( LPCTSTR valname, char* val, DWORD siz )
		{ return ( ERROR_SUCCESS == RegQueryValueEx( m_hKey, valname, NULL, NULL, (BYTE*)val, &siz )); }
	bool del( LPCTSTR valname )
		{ return (ERROR_SUCCESS == RegDeleteValue( m_hKey, valname )); }
	static void delSubKey( HKEY k, LPCTSTR n )
	{
		if( !g_isNT )
		{
			::RegDeleteKey( k, n );
			return;
		}

		// NT系では、サブキーのあるキーは消せないので再帰的に。
		// しかもEnum中にDeleteはできない上にvector<string>等も
		// 封印しているためわけのわからんコードになってます。

		// 消したいキーを開く
		HKEY k2;
		while( ERROR_SUCCESS == ::RegOpenKeyEx( k,n,0,KEY_ENUMERATE_SUB_KEYS|KEY_SET_VALUE,&k2 ) )
		{
			// １個目の子キー名を取得
			char buf[200];
			DWORD bs = sizeof(buf);
			if( ERROR_SUCCESS == ::RegEnumKeyEx( k2,0,buf,&bs,NULL,NULL,NULL,NULL ) )
			{
				// あればそれを削除
				delSubKey( k2, buf );
				::RegCloseKey( k2 );
			}
			else
			{
				// なければ、消したいキーを消せる
				::RegCloseKey( k2 );
				::RegDeleteKey( k, n );
				return;
			}
		}
	}
private:
	HKEY m_hKey;
};



//--------------------------------------------------------
//-- Noahの設定用APIなど ----------------------------------
//--------------------------------------------------------



char g_szAsCmd[MAX_PATH+10];
char g_szAsIcon[MAX_PATH+10];


// 管理者権限判別
//-- IsAdmin() : very very thanks! to ardry, the author of 'meltice'.

bool IsAdmin()
{
	bool isadmin=false;

	//-- 9x対策のため、動的にDLLロード
	HINSTANCE hInstDll = safepathLoadLibrary( "NetAPI32" );
	if( !hInstDll )
		return false;

	//-- NetUserGetLocalGroupes API 取得
	typedef NET_API_STATUS (NET_API_FUNCTION *PNETUSRGETLCLGRP)(LPCWSTR,wchar_t *,DWORD,DWORD,VOID*,DWORD,LPDWORD,LPDWORD);
	PNETUSRGETLCLGRP pNetUserGetLocalGroups = (PNETUSRGETLCLGRP)::GetProcAddress(hInstDll, "NetUserGetLocalGroups");
	if( !pNetUserGetLocalGroups )
	{
		::FreeLibrary( hInstDll );
		return false;
	}

	//-- ユーザー名取得
	char    userA[256];
	wchar_t userW[256];
	DWORD   tmp = 256;
	::GetUserName( userA, &tmp );
	::MultiByteToWideChar( CP_ACP, 0, userA, -1, userW, 255 );

	//-- 本筋
	LOCALGROUP_USERS_INFO_0* pBuf;
	DWORD entry;
	char buf[256];

	if( 0 == pNetUserGetLocalGroups(NULL,userW,0,0,(BYTE**)&pBuf,-1,&entry,&tmp) )
	{
		for( unsigned int i=0; i<entry; i++ )
		{
			::WideCharToMultiByte( CP_ACP, 0, pBuf[i].lgrui0_name, -1, buf, 256, NULL, NULL );

			if( 0 == ::lstrcmp( buf, "Administrators" ) )
			{
				isadmin=true;
				break;
			}
		}

		//-- メモリ解放
		typedef NET_API_STATUS (NET_API_FUNCTION * PNETAPIBUFFERFREE)(void*);
		PNETAPIBUFFERFREE pNetApiBufferFree = (PNETAPIBUFFERFREE)::GetProcAddress( hInstDll, "NetApiBufferFree" );
		if( pNetApiBufferFree )
			pNetApiBufferFree( pBuf );
	}

	//-- DLL解放
	::FreeLibrary( hInstDll );
	return isadmin;
}

// レジストリへの書き込み権限判定…うまくいかないらしい
//
//bool IsRegWritable()
//{
//	HKEY key;
//	if( ERROR_SUCCESS != // 適当なクラスキーへの書き込み権限を調べる
//		::RegOpenKeyEx( HKEY_CLASSES_ROOT, "ttffile", 0, KEY_WRITE, &key ) )
//		return false;
//	::RegCloseKey( key );
//	return true;
//}

//** bool Init()
//**
//**   設定画面用に起動するときは最初にコレを呼ぶこと。
//**   false が返ってきたときは、諸事情により利用できないことを示す。
bool WINAPI Init()
{
	::wsprintf( g_szAsIcon, "%s,%%d", g_szDLL );
	::wsprintf( g_szAsCmd , "%s -x \"%%1\"", g_szNoah );

	if( g_isNT )
		if( !IsAdmin() )
//			if( !IsRegWritable() )
				return false;

	return true;
}

//** void LoadSE( bool* a, bool* x )
//**
//**   シェルエクステンションの設定を返す。
//**   a: [ここに圧縮]がONか否か  x: [ここに解凍]がONか否か
void WINAPI LoadSE( bool* a, bool* x )
{
	*a = kiRegKey::exist( HKEY_CLASSES_ROOT, "CLSID\\{953AFAE9-C2A9-4674-9811-D7E281B001E1}\\CShl" );
	*x = kiRegKey::exist( HKEY_CLASSES_ROOT, "CLSID\\{953AFAE9-C2A9-4674-9811-D7E281B001E1}\\MShl" );
}

//** void SaveSE( bool a, bool x )
//**
//**   シェルエクステンションの設定を保存する。
//**   a: [ここに圧縮]がONか否か  x: [ここに解凍]がONか否か
void WINAPI SaveSE( bool a, bool x )
{
	kiRegKey key, key2;

	if( !a && !x )
	{
		kiRegKey::delSubKey( HKEY_CLASSES_ROOT, "CLSID\\{953AFAE9-C2A9-4674-9811-D7E281B001E1}" );
		kiRegKey::delSubKey( HKEY_CLASSES_ROOT, "Folder\\shellex\\DragDropHandlers\\NoahXt" );
		kiRegKey::delSubKey( HKEY_CLASSES_ROOT, "Drive\\shellex\\DragDropHandlers\\NoahXt" );
		if( key.open( HKEY_CLASSES_ROOT, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", KEY_SET_VALUE ) )
			key.del( "{953AFAE9-C2A9-4674-9811-D7E281B001E1}" );
	}
	else
	{
		// CLSID 登録
		key.create( HKEY_CLASSES_ROOT, "CLSID\\{953AFAE9-C2A9-4674-9811-D7E281B001E1}", KEY_WRITE );
			key.set( "", ProgID_NoahXt );
			key2.create( key, "InprocServer32", KEY_WRITE );
				key2.set( "", g_szDLL );
				key2.set( "ThreadingModel", "Apartment" );
		if(a)key2.create( key, "CShl", KEY_READ );
		else kiRegKey::delSubKey( key, "CShl" );
		if(x)key2.create( key, "MShl", KEY_READ );
		else kiRegKey::delSubKey( key, "MShl" );

		// Folder の DnD Handler として登録
		key.create( HKEY_CLASSES_ROOT, "Folder\\shellex\\DragDropHandlers\\NoahXt", KEY_WRITE );
			key.set( "", "{953AFAE9-C2A9-4674-9811-D7E281B001E1}" );
		// Drive の DnD Handler として登録
		key.create( HKEY_CLASSES_ROOT, "Drive\\shellex\\DragDropHandlers\\NoahXt", KEY_WRITE );
			key.set( "", "{953AFAE9-C2A9-4674-9811-D7E281B001E1}" );

		// NT系用に、Approved List に書いておく
		if( g_isNT && key.open( HKEY_CLASSES_ROOT, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", KEY_SET_VALUE ) )
			key.set( "{953AFAE9-C2A9-4674-9811-D7E281B001E1}", ProgID_NoahXt );
	}
}

void asso_on( const char* ext, const int no );
void asso_off( const char* ext, const int no );
bool is_asso_on( const char* ext );

enum { A_BEGIN, LZH=0, ZIP, CAB, RAR, TAR, YZ1, GCA, ARJ, BGA, ACE, CPT, JAK, A_END,
       OTH=A_END, SvnZ=CPT };
static const char* ext_list[] = {
	"lzh\0lzs\0lha\0",
	"zip\0",
	"cab\0",
	"rar\0",
	"tar\0tgz\0tbz\0taz\0gz\0bz2\0z\0xz\0lzma\0",
	"yz1\0",
	"gca\0",
	"arj\0",
	"gza\0bza\0",
	"ace\0",
	"cpt\0",
	"jak\0",
};

//** void LoadAS( bool asso[] )
//**
//**   標準の関連付けの設定を返す。
//**   LZH=0, ZIP, CAB, RAR, TAR, YZ1, GCA, ARJ, BGA, ACE, CPT, JAK
void WINAPI LoadAS( bool asso[] )
{
	for( int i=A_BEGIN; i<A_END; i++ )
		asso[i] = is_asso_on( ext_list[i] );
}

//** void SaveAS( bool asso[] )
//**
//**   標準の関連付けの設定を保存。
//**   LZH=0, ZIP, CAB, RAR, TAR, YZ1, GCA, ARJ, BGA, ACE, CPT, JAK
void WINAPI SaveAS( bool asso[] )
{
	for( int i=A_BEGIN; i<A_END; i++ )
	{
		int icon_type = i;
		if( i==CPT ) icon_type = OTH; // v3.195: cptは、"その他"アイコン

		if( asso[i] )
			asso_on( ext_list[i], icon_type );
		else
			asso_off( ext_list[i], icon_type );
	}
}

//** void LoadASEx( const char* ext, bool* x )
//**
//**   指定した拡張子がNoahに関連付けられているかどうかを返す
void WINAPI LoadASEx( const char* ext, bool* x )
{
	*x = is_asso_on( ext );
}

//** void SaveASEx( const char* ext, bool x )
//**
//**   指定した拡張子をNoahに関連付けたり解除したり
void WINAPI SaveASEx( const char* ext, bool x )
{
	int icon_type = OTH;                         // デフォルトは"その他"アイコン
	if( 0==lstrcmp(ext,"7z") ) icon_type = SvnZ; // v3.195: 7zに限り、7z専用アイコン

	if( x )	asso_on(  ext, icon_type );
	else	asso_off( ext, icon_type );
}

//---------------------------------------------------------------


#define step(_x)    (_x+=::lstrlen(_x)+1)
#define MltCmd      (g_bJpn ? "解凍(&E)" : "&Extract")
#define JntCmd      (g_bJpn ? "結合(&E)" : "Combin&e")
#define MltTyp      (g_bJpn ? "書庫(%s)" : "Archive(%s)")
#define JntTyp      (g_bJpn ? "分割ファイル(%s)" : "RipperedFile(%s)")
#define CmdName(_n) (_n==JAK ? JntCmd : MltCmd)
#define TypName(_n) (_n==JAK ? JntTyp : MltTyp)

static void recover_zip()
{
	if( kiRegKey::exist( HKEY_CLASSES_ROOT, "CompressedFolder" ) )
	{
		kiRegKey key, key2;
		if( key.create( HKEY_CLASSES_ROOT, ".zip", KEY_WRITE ) )
		{
			key.set( "", "CompressedFolder" );
			if( key2.create( key, "ShellNew", KEY_WRITE ) )
				key2.set( "NullFIle", "" );
		}
	}
}
static void recover_cab()
{
	if( kiRegKey::exist( HKEY_CLASSES_ROOT, "CLSID\\{0CD7A5C0-9F37-11CE-AE65-08002B2E1262}" ) )
	{
		kiRegKey key;
		if( key.create( HKEY_CLASSES_ROOT, ".cab", KEY_WRITE ) )
			key.set( "", "CLSID\\{0CD7A5C0-9F37-11CE-AE65-08002B2E1262}" );
	}
}

void asso_on( const char* ext, const int no )
{
	if( is_asso_on( ext ) )
		return;
	g_bChanged = true;

	kiRegKey key, key2, key3, key4;
	char str[500],asc[20]="NoahXt.";
	::lstrcpy( asc+7, ext );

	for( const char* p=ext; *p; step(p) )
	{
		//-- "HKCR/.lzh" = "NoahXt.lzh", "HKCR/.lzs" = "NoahXt.lzh" ...
		str[0]='.', ::lstrcpy( str+1, p );
		if( key.create( HKEY_CLASSES_ROOT, str, KEY_WRITE ) )
		{
			key.set( "", asc );
			kiRegKey::delSubKey( key, "ShellNew" );
		}
	}

	if( key.create( HKEY_CLASSES_ROOT, asc, KEY_WRITE ) )
	{
		//-- "HKCR/NoahXt.lzh" = "書庫( lzh )"
		::wsprintf( str, TypName(no), ext );
		key.set( "", str );
		key.del( "EditFlags" );

		if( key2.create( key, "DefaultIcon", KEY_WRITE ) )
		{
			//-- "HKCR/NoahXt.lzh/DefaultIcon" = "...Noah.exe, 1"
			::wsprintf( str, g_szAsIcon, no );
			key2.set( "", str );
		}

		if( key2.create( key, "Shell", KEY_WRITE ) )
		{
			//-- "HKCR/NoahXt.lzh/Shell" = "Open"
			key2.set( "", "Open" );
			if( key3.create( key2, "Open", KEY_WRITE ) )
			{
				//--  "HKCR/NoahXt.lzh/Shell/Open" = "解凍(&X)"
				key3.set( "", CmdName(no) );
				if( key4.create( key3, "Command", KEY_WRITE ) )
					//--  "HKCR/NoahXt.lzh/Shell/Open/Command" = "...Noah.exe -x "%1""
					key4.set( "", g_szAsCmd );
			}
		}
	}
}

void asso_off( const char* ext, const int no )
{
	if( !is_asso_on( ext ) )
		return;
	g_bChanged = true;

	//-- delete "HKCR/NoahXt.lzh"
	char str[20] = "NoahXt.";
	::lstrcpy( str+7, ext );
	kiRegKey::delSubKey( HKEY_CLASSES_ROOT, str );

	//-- delete "HKCR/.lzh" "HKCR/.lzs" ...
	for( const char* p=ext; *p; step(p) )
	{
		str[0]='.', ::lstrcpy( str+1, p );
		kiRegKey::delSubKey( HKEY_CLASSES_ROOT, str );
	}

	switch( no )
	{
	// CAB, ZIP to Windows Default Association
	case CAB: recover_cab(); break;
	case ZIP: recover_zip(); break;
	}
}

bool is_asso_on( const char* ext )
{
	//-- "HKCR/.lzh" exists ?
	char str[20] = ".";
	::lstrcpy( str+1, ext );
	kiRegKey key;
	if( !key.open( HKEY_CLASSES_ROOT, str, KEY_READ ) )
		return false;
	if( !key.get( "", str, 20 ) )
		return false;

	//-- the value of "HKCR/.lzh" is "NoahXt.lzh" ?
	char asc[20] = "NoahXt.";
	::lstrcpy( asc+7, ext );
	return ( 0==::lstrcmp( str, asc ) );
}
