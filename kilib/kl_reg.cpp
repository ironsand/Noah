//--- K.I.LIB ---
// kl_reg.h : registry and ini-file operation

#include "stdafx.h"
#include "kilib.h"


//---------------------------- openån ----------------------------//


bool kiRegKey::open( HKEY parent, LPCTSTR keyname, REGSAM access )
{
	return (ERROR_SUCCESS == RegOpenKeyEx( parent,
		keyname, 0, access, &m_hKey ));
}

bool kiRegKey::create( HKEY parent, LPCTSTR keyname, REGSAM access )
{
	DWORD x;
	return (ERROR_SUCCESS == RegCreateKeyEx( parent,
		keyname, 0, REG_NONE, REG_OPTION_NON_VOLATILE, access, NULL, &m_hKey, &x ));
}


//------------------------- query/setån ----------------------------//


bool kiRegKey::get( LPCTSTR valname, DWORD* val )
{
	DWORD x=4;
	return (ERROR_SUCCESS == RegQueryValueEx( m_hKey,
		valname, NULL, NULL, (BYTE*)val, &x ));
}

bool kiRegKey::get( LPCTSTR valname, BYTE* val, DWORD siz )
{
	return (ERROR_SUCCESS == RegQueryValueEx( m_hKey,
		valname, NULL, NULL, val, &siz ));
}

bool kiRegKey::get( LPCTSTR valname, kiStr* val )
{
	static char dat[2048];
	DWORD x = 2048;
	if( ERROR_SUCCESS == RegQueryValueEx( m_hKey,
		valname, NULL, NULL, (BYTE*)dat, &x ))
	{
		*val = dat;
		return true;
	}
	return false;
}

bool kiRegKey::set( LPCTSTR valname, DWORD val )
{
	return (ERROR_SUCCESS == RegSetValueEx( m_hKey,
		valname, 0, REG_DWORD, (BYTE*)&val, 4 ));
}

bool kiRegKey::set( LPCTSTR valname, BYTE* val, DWORD siz )
{
	return (ERROR_SUCCESS == RegSetValueEx( m_hKey,
		valname, 0, REG_BINARY, (BYTE*)val, siz ));
}

bool kiRegKey::set( LPCTSTR valname, LPCTSTR val )
{
	return (ERROR_SUCCESS == RegSetValueEx( m_hKey,
		valname, 0, REG_SZ,	(BYTE*)val, ki_strlen(val)+1 ));
}


//--------------------------- deleteån ----------------------------//


bool kiRegKey::del( LPCTSTR valname )
{
	return (ERROR_SUCCESS == RegDeleteValue( m_hKey, valname ));
}

bool kiRegKey::delSubKey( LPCTSTR keyname )
{
	if( app()->osver().dwPlatformId == VER_PLATFORM_WIN32_NT )
		return delSubKeyRecursive( m_hKey, keyname );
	else
		return (ERROR_SUCCESS == RegDeleteKey( m_hKey, keyname ));
}

bool kiRegKey::delSubKeyRecursive( HKEY k, LPCTSTR n )
{
	HKEY k2;
	if( ERROR_SUCCESS!=RegOpenKeyEx( k,n,0,KEY_READ,&k2 ) )
		return false;

	bool ans = true;
	static char buf[2048];
	DWORD bs = sizeof(buf);

	for( int i=0;
		ERROR_SUCCESS==RegEnumKeyEx( k2,i,buf,&bs,NULL,NULL,NULL,NULL );
		i++ )
	{
		if( !delSubKeyRecursive( k2,buf ) )
			ans = false;
		bs = sizeof(buf);
	}
	
	RegCloseKey( k2 );
	RegDeleteKey( k,n );
	return ans;
}

//--------------------------- ini:èâä˙âª ----------------------------//


void kiIniFile::setFileName( const char* ini, bool exepath )
{
	if( !exepath )
		m_FileName = "";
	else
	{
		m_FileName.beSpecialPath( kiPath::Exe );
		m_FileName.beBackSlash( true );
	}
	m_FileName += ini;
}


//--------------------------- ini:readån ----------------------------//


int kiIniFile::getInt( const char* key, int defval )
{
	return ::GetPrivateProfileInt( m_CurSec, key, defval, m_FileName );
}

bool kiIniFile::getBool( const char* key, bool defval )
{
	return (0 != ::GetPrivateProfileInt( m_CurSec,
						key, defval?1:0, m_FileName ) );
}

const char* kiIniFile::getStr( const char* key, const char* defval )
{
	::GetPrivateProfileString( m_CurSec, key, defval,
					m_StrBuf, sizeof(m_StrBuf), m_FileName );
	return m_StrBuf;
}


//--------------------------- ini:writeån ----------------------------//


bool kiIniFile::putStr( const char* key, const char* val )
{
	return (FALSE != ::WritePrivateProfileString(
					m_CurSec, key, val, m_FileName ) );
}

bool kiIniFile::putInt( const char* key, int val )
{
	::wsprintf( m_StrBuf, "%d", val );
	return putStr( key, m_StrBuf );
}

bool kiIniFile::putBool( const char* key, bool val )
{
	return putStr( key, val ? "1" : "0" );
}
