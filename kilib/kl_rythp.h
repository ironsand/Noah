//--- K.I.LIB ---
// kl_rythp.h : interpretor for simple script langauage 'Rythp'

#ifndef AFX_KIRYTHPVM_H__4F3C28A9_7EFE_4605_A149_2C0B9A9236E5__INCLUDED_
#define AFX_KIRYTHPVM_H__4F3C28A9_7EFE_4605_A149_2C0B9A9236E5__INCLUDED_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// kiVar : Rythp用のVariant変数型。ほとんどただのkiStr。

class kiVar : public kiStr
{
public:
	kiVar() : kiStr(20) {}
	explicit kiVar( const char* s ) : kiStr( s, 20 ){}
	explicit kiVar( const kiStr& s ) : kiStr( s, 20 ){}
	explicit kiVar( const kiVar& s ) : kiStr( s, 20 ){}
	void operator = ( const char* s ){ kiStr::operator =(s); }

	int getInt();
	kiVar& quote();
	kiVar& unquote();
};

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// kiRythpVM : 最小限Rythp。実用には、派生して独自の exec_function を実装すべし

class kiRythpVM
{
public:
	kiRythpVM();
	virtual ~kiRythpVM() {}

public:
	// eval
	void eval( char* str, kiVar* ans=NULL );

protected:
	// 引数を適当にevalや変数置き換えをした形で取得
	void getarg( char* a, bool b, kiVar* arg );

	// function実行。[ bool=処理したか？、 name=function名、 a,b,c=引数、 r=返値 ]
	virtual bool exec_function( const kiVar& name,
		const CharArray& a, const BoolArray& b,int c, kiVar* r );

private:
	// 変数
	kiVar ele[256];

	// パラメータ分割
	static char* split_tonext( char* p );
	static char* split_toend( char* p );
	static bool split( char* buf, CharArray& argv, BoolArray& argb, int& argc );
};

#endif
