//--- K.I.LIB ---
// kl_reg.h : registry and ini-file operation

#ifndef AFX_KIREGKEY_H__4FD5E1B3_B8FE_45B3_B19E_3D30407C94BA__INCLUDED_
#define AFX_KIREGKEY_H__4FD5E1B3_B8FE_45B3_B19E_3D30407C94BA__INCLUDED_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// レジストリ操作＆ini操作

class kiRegKey
{
public: //-- 外向きインターフェイス --------------------------

	// 開く＆閉じる
	bool open( HKEY parent, LPCTSTR keyname, REGSAM access = KEY_ALL_ACCESS );
	bool create( HKEY parent, LPCTSTR keyname, REGSAM access = KEY_ALL_ACCESS );
	void close()
		{
			if( m_hKey )
				RegCloseKey( m_hKey );
		}

	// サブキーが存在するや否や
	bool exist( LPCTSTR keyname )
		{
			HKEY k;
			if( ERROR_SUCCESS==RegOpenKeyEx( m_hKey,keyname,0,KEY_READ,&k ) )
			{
				RegCloseKey( k );
				return true;
			}
			return false;
		}
	// HKEYへキャスト
	operator HKEY() const
		{
			return m_hKey;
		}

	// 値を得る
	bool get( LPCTSTR valname, DWORD* val );
	bool get( LPCTSTR valname, BYTE* val, DWORD siz );
	bool get( LPCTSTR valname, kiStr* val );

	// 値を設定
	bool set( LPCTSTR valname, DWORD val );
	bool set( LPCTSTR valname, BYTE* val, DWORD siz );
	bool set( LPCTSTR valname, LPCTSTR val );

	// 削除
	bool del( LPCTSTR valname );
	bool delSubKey( LPCTSTR keyname );

public: //-- 内部処理 -----------------------------------

	kiRegKey()
		{
			m_hKey = NULL;
		}

	virtual ~kiRegKey()
		{
			close();
		}

private:

	HKEY m_hKey;
	static bool delSubKeyRecursive( HKEY k, LPCTSTR n );
};

class kiIniFile
{
public: //-- 外向きインターフェイス --------------------------

	// iniファイル名を設定
	void setFileName( const char* ini, bool exepath=true );
	void setSection( const char* section )
		{ m_CurSec = section; }

	// 読み込み
	// ※ 注意！getStrの返値は内部バッファな為、
	// ※　　　 呼び出し直後以外は内容を保証しない。
	int getInt( const char* key, int defval );
	bool getBool( const char* key, bool defval );
	const char* getStr( const char* key, const char* defval );

	// 書き込み
	bool putStr( const char* key, const char* val );
	bool putInt( const char* key, int val );
	bool putBool( const char* key, bool val );

private: //-- 内部処理 -----------------------------------

	kiPath m_FileName;
	kiStr m_CurSec;
	char m_StrBuf[256];
};

#endif
