//--- K.I.LIB ---
// kl_carc.cpp : handling "common archivers' dll"

#include "stdafx.h"
#include "kilibext.h"


//------------------------ load/unload 制御 ----------------------//


kiArcDLLRaw::kiArcDLLRaw( const char* dllname )
	: m_DllPath( dllname ), not_loaded_yet( true )
{
	ki_memzero( m_Proc, ISARC_FUNCTION_END*sizeof(FARPROC) );
	f_VSb = NULL;
}

kiArcDLLRaw::~kiArcDLLRaw()
{
	unload();
}

bool kiArcDLLRaw::load()
{
	not_loaded_yet = false;

	m_hDLL = kiSUtil::loadLibrary( m_DllPath );
	if( !m_hDLL )
		return false;

	// DLLの名前部分を切り出し
	char str[MAX_PATH], *p;
	const char *x, *y, *z;
	for( x=y=m_DllPath; *x!='\0'; x=kiStr::next(x) )
		if( *x=='\\' || *x=='/' )
			y = x + 1;
	for( p=str, z=y; (*z!='.' && *z!='3' && *z!='\0'); p++, z++ )
		*p = *z;
	*p = '\0';

	// コマンド送りAPIを取得。ccで始まるAPI名かも?
	f_Cmd = ::GetProcAddress( m_hDLL, str );
	if( f_Cmd )
		m_DllNameBody = str;
	else
	{
		f_Cmd = ::GetProcAddress( m_hDLL, "ccCommand" );
		if( f_Cmd )
			m_DllNameBody = "cc";
		else
		{
			if( str[0]=='7' && (f_Cmd=::GetProcAddress(m_hDLL,"SevenZip")) )
				m_DllNameBody = "SevenZip";
			else
				return false;
		}
	}

	return true;
}

void kiArcDLLRaw::unload()
{
	if( !not_loaded_yet && m_hDLL )
	{
		ki_memzero( m_Proc, ISARC_FUNCTION_END*sizeof(FARPROC) );
		::FreeLibrary( m_hDLL );
		not_loaded_yet = true;
	}
}

bool kiArcDLLRaw::isAlive()
{
	if( not_loaded_yet )
		load();

	return (m_hDLL != NULL);
}

FARPROC kiArcDLLRaw::getProc( const char* procname )
{
	kiStr funcName = (const char*)m_DllNameBody;
	funcName += procname;
	return ::GetProcAddress( m_hDLL, funcName );
}

//----------------------------- APIのラッパ群 --------------------------//


int kiArcDLLRaw::command( const HWND wnd, LPCSTR cmd, LPSTR buf, const DWORD siz )
{
	if( not_loaded_yet )
		if( !load() )
			return ERROR_NOT_SUPPORT;

	// コマンド関数は必ずロードされている

	typedef int (WINAPI * CARC_CMD)(const HWND,const char*,char*,const DWORD);
	return ((CARC_CMD)f_Cmd)( wnd, cmd, buf, siz );
}

WORD kiArcDLLRaw::getVer()
{
	if( not_loaded_yet )
		if( !load() )
			return 0;

	if( !f_Ver )
		f_Ver = getProc( "GetVersion" );

	typedef WORD (WINAPI * CARC_VER)(void);
	return f_Ver ? ((CARC_VER)f_Ver)() : 0;
}

WORD kiArcDLLRaw::getVerSub()
{
	if( not_loaded_yet )
		if( !load() )
			return 0;

	if( !f_VSb )
		f_VSb = getProc( "GetSubVersion" );

	typedef WORD (WINAPI * CARC_VER)(void);
	return f_VSb ? ((CARC_VER)f_VSb)() : 0;
}

BOOL kiArcDLLRaw::check( LPCSTR filename, const int mode )
{
	if( not_loaded_yet )
		if( !load() )
			return FALSE;

	if( !f_Chk )
		f_Chk = getProc( "CheckArchive" );

	typedef BOOL (WINAPI * CARC_CHK)(const char*,const int);
	return f_Chk ? ((CARC_CHK)f_Chk)( filename, mode ) : FALSE;
}

int kiArcDLLRaw::getArcType( LPCSTR filename )
{
	if( not_loaded_yet )
		if( !load() )
			return FALSE;

	if( !f_Gat )
		f_Gat = getProc( "GetArchiveType" );

	typedef BOOL (WINAPI * CARC_GAT)(const char*);
	return f_Gat ? ((CARC_GAT)f_Gat)( filename ) : 0;
}

HARC kiArcDLLRaw::openArc( const HWND wnd, LPCSTR arcname, const DWORD flag )
{
	if( not_loaded_yet )
		if( !load() )
			return NULL;

	if( !f_Opn )
		f_Opn = getProc( "OpenArchive" );

	typedef HARC (WINAPI * CARC_OPN)(const HWND,LPCSTR,const DWORD);
	return f_Opn ? ((CARC_OPN)f_Opn)( wnd, arcname, flag ) : NULL;
}

void kiArcDLLRaw::closeArc( HARC arc )
{
	if( not_loaded_yet )
		if( !load() )
			return;

	if( !f_Cls )
		f_Cls = getProc( "CloseArchive" );

	typedef int (WINAPI * CARC_CLS)(HARC);
	if( f_Cls )
		((CARC_CLS)f_Cls)( arc );
}

int kiArcDLLRaw::findfirst( HARC arc, LPCSTR wildname, INDIVIDUALINFO* inf )
{
	if( not_loaded_yet )
		if( !load() )
			return ERROR_NOT_SUPPORT;

	if( !f_Ffs )
		f_Ffs = getProc( "FindFirst" );

	typedef int (WINAPI * CARC_FFS)(HARC,LPCSTR,INDIVIDUALINFO FAR *);
	return f_Ffs ? ((CARC_FFS)f_Ffs)( arc, wildname, inf ) : ERROR_NOT_SUPPORT;
}

int kiArcDLLRaw::findnext( HARC arc, INDIVIDUALINFO* inf )
{
	if( not_loaded_yet )
		if( !load() )
			return ERROR_NOT_SUPPORT;

	if( !f_Fnx )
		f_Fnx = getProc( "FindNext" );

	typedef int (WINAPI * CARC_FNX)(HARC,INDIVIDUALINFO FAR *);
	return f_Fnx ? ((CARC_FNX)f_Fnx)( arc, inf ) : ERROR_NOT_SUPPORT;
}

int kiArcDLLRaw::getAttr( HARC arc )
{
	if( not_loaded_yet )
		if( !load() )
			return ERROR_NOT_SUPPORT;

	if( !f_GAr )
		f_GAr = getProc( "GetAttribute" );

	typedef int (WINAPI * CARC_GAR)(HARC);
	return f_GAr ? ((CARC_GAR)f_GAr)( arc ) : 0;
}

BOOL kiArcDLLRaw::setOwner( HWND wnd )
{
	if( not_loaded_yet )
		if( !load() )
			return ERROR_NOT_SUPPORT;

	if( !f_SOw )
		f_SOw = getProc( "SetOwnerWindow" );

	typedef BOOL (WINAPI * CARC_SOW)(HWND);
	return f_SOw ? ((CARC_SOW)f_SOw)( wnd ) : FALSE;
}

BOOL kiArcDLLRaw::clearOwner()
{
	if( not_loaded_yet )
		if( !load() )
			return ERROR_NOT_SUPPORT;

	if( !f_COw )
		f_COw = getProc( "ClearOwnerWindow" );

	typedef BOOL (WINAPI * CARC_COW)();
	return f_COw ? ((CARC_COW)f_COw)() : FALSE;
}

