//--- K.I.LIB ---
// kl_str.h : string classes for K.I.LIB

#ifndef AFX_KISTR_H__1932CA2C_ACA6_4606_B57A_ACD0B7D1D35B__INCLUDED_
#define AFX_KISTR_H__1932CA2C_ACA6_4606_B57A_ACD0B7D1D35B__INCLUDED_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// kiStr : 単純文字列

class kiStr
{
friend void kilib_startUp();

private: //-- グローバルな初期化処理など ---------------------

	static void init();

public: //-- 外向きインターフェイス --------------------------

	// 2byte文字の処理を高速化(したような気分)
	static char* next( char* p )
		{ return p+st_lb[(*p)&0xff]; }
	static const char* next( const char* p )
		{ return p+st_lb[(*p)&0xff]; }
	static bool isLeadByte( char c )
		{ return st_lb[c&0xff]==2; }

	// 初期化
	kiStr( int start_size = 100 );
	kiStr( const char* s, int min_size = 100 );
	explicit kiStr( const kiStr& s );

	// 演算子
	kiStr& operator = ( const kiStr& );
	kiStr& operator = ( const char* s );
	kiStr& operator += ( const char* s );
	kiStr& operator += ( char c );
	bool operator == ( const char* s ) const;
	bool isSame( const char* s )       const;
	operator const char*()             const;
	int len()                          const;
	void lower()
		{ ::CharLower(m_pBuf); }
	void upper()
		{ ::CharUpper(m_pBuf); }
	kiStr& setInt( int n, bool cm=false );
	void replaceToSlash() {
		for(char* p=m_pBuf; *p; p=next(p))
			if(*p=='\\')
				*p='/';
	}

	// リソースからロード
	kiStr& loadRsrc( UINT id );

	kiStr& removeTrailWS();

protected: //-- 派生クラス向け -----------------------------

	char* m_pBuf;
	int   m_ALen;

private: //-- 内部処理 -------------------------------------

	static char st_lb[256];

public:

	virtual ~kiStr();
};

inline const kiStr operator+(const kiStr& x, const kiStr& y)
	{ return kiStr(x) += y; }
inline const kiStr operator+(const char* x, const kiStr& y)
	{ return kiStr(x) += y; }
inline const kiStr operator+(const kiStr& x, const char* y)
	{ return kiStr(x) += y; }

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// kiPath : パス特有の処理用関数付き文字列

class kiPath : public kiStr
{
public: //-- 外向きインターフェイス --------------------------

	// 初期化
	kiPath() : kiStr( MAX_PATH ){}
	explicit kiPath( const char* s ) : kiStr( s, MAX_PATH ){}
	explicit kiPath( const kiStr& s ) : kiStr( s, MAX_PATH ){}
	explicit kiPath( const kiPath& s ) : kiStr( s, MAX_PATH ){}
	kiPath( int nPATH, bool bs = true ) : kiStr( MAX_PATH )
		{
			beSpecialPath( nPATH );
			if( nPATH != Exe_name )
				beBackSlash( bs );
		}

	// operator
	void operator = ( const char* s ){ kiStr::operator =(s); }

	// 特殊パス取得
	void beSpecialPath( int nPATH );
	enum { Win=0x1787, Sys, Tmp, Prg, Exe, Cur, Exe_name,
			Snd=CSIDL_SENDTO, Dsk=CSIDL_DESKTOP, Doc=CSIDL_PERSONAL };

	// 短いパス
	void beShortPath();

	// 最後のバックスラッシュ制御
	void beBackSlash( bool add );

	// ディレクトリ名のみ
	bool beDirOnly();
	// ファイル名except拡張子全部
	void getBody( kiStr& str ) const;
	// ファイル名except拡張子一つ
	void getBody_all( kiStr& str ) const;

	// 複数階層mkdir
	void mkdir();
	// 複数階層rmdir
	void remove();

	// ドライブタイプ
	UINT getDriveType() const;
	// 同じディレクトリにあるかどうか
	bool isInSameDir(const char* r) const;

	// [static] ディレクトリ情報を含まない、ファイル名のみ抽出
	static const char* name( const char* str );
	// [static] 最後の拡張子。無ければNULL
	static const char* ext( const char* str );
	// [static] 拡張子全部。無ければNULL
	static const char* ext_all( const char* str );
	// [static] \ / で終わるか否か
	static bool endwithyen( const char* str );

	// non-static-ver
	const char* name() const
		{ return name(m_pBuf); }
	const char* ext() const
		{ return ext(m_pBuf); }
	const char* ext_all() const
		{ return ext_all(m_pBuf); }
};

#endif
