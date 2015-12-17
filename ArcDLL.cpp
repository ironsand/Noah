
#include "stdafx.h"
#include "ArcDLL.h"
#include "NoahApp.h"

int CArcDLL::v_load()
{
	// DLLの存在確認
	if( !dll.exist() )
		return 0;

	// 圧縮メソッドリストを設定
	DWORD cp = set_cmpr_mhd();

	// Abiltyを返す
	return (m_Ecmd ? aCheck|aMelt : 0) | (m_Xcmd ? aList|aMeltEach : 0) | cp | (m_Scmd ? aSfx : 0);
}

int CArcDLL::v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files )
{
	// 出力先をカレントに
	::SetCurrentDirectory( ddir );

	// 解凍コマンド
	kiStr cmd = ( !files || files->len()==0 ) ? m_Ecmd : m_Xcmd;

	// 書庫名
	cmd += " \"", cmd += aname.basedir, cmd += aname.lname, cmd += "\" \"";
	// 出力先ディレクトリ
	cmd += ddir, cmd += '"';

	// ( もしあれば )ファイルリスト
	if( files )
		for( unsigned int i=0; i!=files->len(); i++ )
			if( (*files)[i].selected )
				cmd += " \"",
				decorate_add_melt( cmd, (*files)[i].inf.szFileName ),
				cmd += '"';

	// コマンド実行！
	return dll.cmd( cmd );
}

int  CArcDLL::v_compress( const kiPath&	  base,
						  const wfdArray& files,
						  const kiPath&   ddir,
						  const int       method,
						  const bool      sfx )
{
	// 基底はカレント
	::SetCurrentDirectory( base );

	// ファイル名リスト作成
	kiStr lst;
	for( unsigned int i=0; i!=files.len(); i++ )
	{
		lst += '"';
		decorate_add_cmpr( lst, files[i].cFileName );
		if( files[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			if( m_Wild==1 )
				lst += "\\*";
			else if( m_Wild==2 )
				lst += "\\*.*";
		lst += "\" ";
	}

	// 書庫名作成

	// Ver 3.14 -- フォルダなら拡張子を除かない
	//  b2eの方は今更どーしようもないので、ここだけ変更
	// Ver 3.19 -- 削る拡張子は一個だけ
	kiStr aname;
	if( files[0].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
	{
		aname = kiPath::name( files[0].cFileName );
	}
	else
	{
		switch(mycnf().extnum())
		{
		case 0: aname = files[0].cFileName;break; 
		case 1: kiPath(files[0].cFileName).getBody_all(aname);break;
		default:kiPath(files[0].cFileName).getBody(aname);break;
		}
	}


	// 圧縮！
	if( sfx )
	{
		kiPath tmp;
		myapp().get_tempdir( tmp );

		kiPath tsfx(tmp);  tsfx+=aname; tsfx+=".exe";
		kiPath rsfx(ddir); rsfx+=aname; rsfx+=".exe";

		int ans = cmpr( method, aname, tmp, lst );
		if( ans < 0x8000 )
		{
			ans = 0x8020;
			::SetCurrentDirectory( tmp );
			if( arc2sfx( aname ) )
				if( ::CopyFile( tsfx, rsfx, FALSE ) )
					ans = 0;
			// カレントを戻しておかないとあとで削除できない…
			::SetCurrentDirectory( base );
		}

		tmp.remove();
		return ans;
	}
	else
		return cmpr( method, aname, ddir, lst );
}

int CArcDLL::cmpr( int mhd, kiStr& aname,const kiPath& to,const kiStr& lst )
{
	aname += '.', aname+=get_cmpr_ext(mhd);

	kiStr cmd = get_cmpr_cmd(mhd);
	cmd += " \"";
	cmd += to;
	cmd += aname;
	cmd += "\" ";
	cmd += lst;
	return dll.cmd( cmd );
}

bool CArcDLL::arc2sfx( const kiStr& aname )
{
	kiStr cmd=m_Scmd; cmd+=" \"", cmd+=aname, cmd+='"';
	return 0x8000>dll.cmd( cmd );
}

//----------------------------------------------------------------------//

void CArcDLL::decorate_add_cmpr( kiStr& lst, const char* fname )
{
	lst += fname;
}

void CArcDLL::decorate_add_melt( kiStr& lst, const char* fname )
{
	lst += fname;
}

void CArcDLL_DotSlash::decorate_add_cmpr( kiStr& lst, const char* fname )
{
	// 先頭 - や @ 対策に、.\\ でエスケープする
	if( *fname=='-' || *fname=='@' )
		lst += ".\\";
	lst += fname;
}

void CArcLzh::decorate_add_cmpr( kiStr& lst, const char* fname )
{
	// UNLHA32.DLL にはこの問題回避用のオプション -gb が。
	if( *fname=='-' )
		lst += "-gb";
	lst += fname;
}

void CArcLzh::decorate_add_melt( kiStr& lst, const char* fname )
{
	// UNLHA32.DLL にはこの問題回避用のオプション -gb が。
	if( *fname=='-' )
		lst += "-gb";
	lst += fname;
}

void CArcZip::decorate_add_cmpr( kiStr& lst, const char* fname )
{
	// 先頭 - や @ 対策に、[-] [@] と置換する
	if( *fname=='-' || *fname=='@' )
		lst+='[', lst+=*fname++, lst+=']';

	// 途中の [ を [[] に置換する
	while( *fname )
		if( kiStr::isLeadByte(*fname) )
		{
			lst += *fname++, lst += *fname++;
		}
		else
		{
			lst += *fname;
			if( *fname++=='[' )
				lst += "[]";
		}
}

//----------------------------------------------------------------------//

DWORD CArcLzh::set_cmpr_mhd()
{
	set_cmp_ext( "lzh" );
	add_cmp_mhd( "lh5" ), add_cmp_mhd( "lh6" ), add_cmp_mhd( "lh7" );
	return aCompress|aArchive;
}

const char* CArcLzh::get_cmpr_cmd( int method )
{
	static char cmd[] = "a -d -jso1 -- -jm2";
	cmd[ sizeof(cmd)-2 ] = (char)('2'+method);
	return cmd;
}

//----------------------------------------------------------------------//

DWORD CArcZip::set_cmpr_mhd()
{
	set_cmp_ext( "zip" );
	add_cmp_mhd( "store" ),
	add_cmp_mhd( "normal", true ), add_cmp_mhd( "password" );
	return aCompress|aArchive;
}

const char* CArcZip::get_cmpr_cmd( int method )
{
	switch( method )
	{
	case 0: return "-rS0";
	case 2: return "-rS9e";
	}
	return "-rS9";
}

//----------------------------------------------------------------------//

DWORD CArcCab::set_cmpr_mhd()
{
	set_cmp_ext( "cab" );
	add_cmp_mhd( "MSZIP" ), add_cmp_mhd( "LZX21",true );
	return aCompress|aArchive;
}

const char* CArcCab::get_cmpr_cmd( int method )
{
	return method==0 ? "-a -r -mz" : "-a -r -ml:21";
}

//----------------------------------------------------------------------//

DWORD CArcTar::set_cmpr_mhd()
{
	set_cmp_ext( "tar" );
	add_cmp_mhd( "normal" ), add_cmp_mhd( "gzip",true ), add_cmp_mhd( "bzip2" );
	add_cmp_mhd( "xz" ), add_cmp_mhd( "lzma" );
	return aCompress|aArchive;
}

const char* CArcTar::get_cmpr_ext( int method )
{
	switch( method )
	{
	case 1: return "tgz";
	case 2: return "tbz";
	case 3: return "tar.xz";
	case 4: return "tar.lzma";
	}
	return "tar";
}

const char* CArcTar::get_cmpr_cmd( int method )
{
	switch( method )
	{
	case 1: return "-cvz9 --";
	case 2: return "-cvB --";
	case 3: return "-cvJ9 --";
	case 4: return "-cv --lzma=9 --";
	}
	return "-cv --";
}

//----------------------------------------------------------------------//

DWORD CArcBga::set_cmpr_mhd()
{
	set_cmp_ext( "bga" );
	add_cmp_mhd( "gzip" ), add_cmp_mhd( "bzip2",true );
	return aCompress|aArchive;
}

const char* CArcBga::get_cmpr_ext( int method )
{
	return method==0 ? "gza" : "bza";
}

const char* CArcBga::get_cmpr_cmd( int method )
{
	return "a -r -a";
}

//----------------------------------------------------------------------//

DWORD CArcYz1::set_cmpr_mhd()
{
	set_cmp_ext( "yz1" );
	add_cmp_mhd( "normal" ), add_cmp_mhd( "password" );
	return aCompress|aArchive;
}

const char* CArcYz1::get_cmpr_cmd( int method )
{
	return method==0 ? "c --" : "c -p --";
}

//----------------------------------------------------------------------//

bool CArcRar::v_check( const kiPath& aname )
{
	// Unrar.dll のバグ？のため、ファイルハンドルが解放されないことが
	// あるので、自前のチェックルーチンを用いる
	// # Subset of XacRett #39

	bool ans=false;
	kiFile rar;
	if( !rar.open(aname) )
		return false;

	unsigned char mark[10];
	if( 10!=rar.read(mark,10) )
		return false;

	if( mark[0]==0x52 && mark[1]==0x45 && mark[2]==0x7e && mark[3]==0x5e )
		return true;
	else if( mark[0]==0x52 && mark[1]==0x61 &&
			 mark[2]==0x72 && mark[3]==0x21 &&
			 mark[4]==0x1a && mark[5]==0x07 &&
			 mark[6]==0x00 && mark[9]==0x73 )
		return true;
	else
	{
		unsigned char* mem=new unsigned char[0x20000];
		int siz = rar.read( mem, 0x1FFF0 );

		for( unsigned char* p=mem; p<mem+siz-9; p++ )
		{
			if( *p!=0x52 )continue;
			if( p[1]==0x45 && p[2]==0x7e && p[3]==0x5e &&
				mem[18]==0x52 && mem[19]==0x53 &&
				mem[20]==0x46 && mem[21]==0x58)
				{ ans=true; break; }

			if( p[1]==0x61 && p[2]==0x72 && p[3]==0x21 &&
				p[4]==0x1a && p[5]==0x07 && p[6]==0x00 &&
				p[9]==0x73 )
				{ ans=true; break; }
		}
		delete [] mem;
    }

	return ans;
}

int CArcRar::v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files )
{
	// Unrarはコマンド指定でダイアログを消せないので、
	// OwnerWindowを指定する

	if( files && files->len() )
		dll.own( app()->mainhwnd() );

	int ans = CArcDLL::v_melt( aname, ddir, files );

	if( files && files->len() )
		dll.fre();

	return ans;
}

int CArcUnZip::v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files )
{
	// UnZip32.dllが妙なフォルダを作り出すバグ対策

	int ans = CArcDLL::v_melt( aname, ddir, files );
	dll.unload();
	return ans;
}

//----------------------------------------------------------------------//

DWORD CArc7z::set_cmpr_mhd()
{
	set_cmp_ext( "7z" );
	add_cmp_mhd( "LZMA", true );
	add_cmp_mhd( "LZMA(std)" );
	add_cmp_mhd( "LZMA(fast)" );
	add_cmp_mhd( "PPMd" );
	return aCompress|aArchive;
}

const char* CArc7z::get_cmpr_cmd( int method )
{
	if( m_SfxMode )
		switch( method )
		{
			case 0:  return "a -t7z -sfx -m0=LZMA -r0 -mx=9 --";
			case 1:  return "a -t7z -sfx -m0=LZMA -r0 -mx=5 --";
			case 2:  return "a -t7z -sfx -m0=LZMA -r0 -mx=1 --";
			default: return "a -t7z -sfx -m0=PPMd -r0 -mx=9 --";
		}
	else
		switch( method )
		{
			case 0:  return "a -t7z -m0=LZMA -r0 -mx=9 --";
			case 1:  return "a -t7z -m0=LZMA -r0 -mx=5 --";
			case 2:  return "a -t7z -m0=LZMA -r0 -mx=1 --";
			default: return "a -t7z -m0=PPMd -r0 -mx=9 --";
		}
}

int CArc7z::v_compress( const kiPath& base, const wfdArray& files, const kiPath& ddir, int method, bool sfx )
{
	m_SfxMode = sfx; // 処理を横取りしてSFXモードを記憶
	return CArcDLL::v_compress(base,files,ddir,method,false);
}

const char* CArc7z::get_cmpr_ext( int method )
{
	return m_SfxMode ? "exe" : "7z";
}

//----------------------------------------------------------------------//

DWORD CArc7zZip::set_cmpr_mhd()
{
	set_cmp_ext( "zip" );
	add_cmp_mhd( "7-zip", true );
	return aCompress|aArchive;
}

const char* CArc7zZip::get_cmpr_cmd( int method )
{
	return "a -tzip -r0 -mx=9 --";
}

int CArc7zZip::v_compress( const kiPath& base, const wfdArray& files, const kiPath& ddir, int method, bool sfx )
{
	// 処理を横取りして通常圧縮
	return CArcDLL::v_compress(base,files,ddir,method,false);
}

