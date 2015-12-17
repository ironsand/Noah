// NoahAM.cpp
//-- control many archiver routines --

#include "stdafx.h"
#include "resource.h"
#include "NoahApp.h"
#include "NoahAM.h"
#include "ArcDLL.h"
#include "ArcAce.h"
#include "ArcMsc.h"
#include "ArcB2e.h"
#include "ArcCpt.h"

//----------------------------------------------//
//------ 実働部隊のデータで初期化しておく ------//
//----------------------------------------------//

void CNoahArchiverManager::init()
{
	const char* kl = mycnf().kill();
	static int dead[128];
	while( *kl ) dead[ 0x7f & (*(kl++)) ] = 1;

	// 初期対応形式
	if( !dead['L'] )	m_AList.add( new CArcLzh );
	if( !dead['7'] )	m_AList.add( new CArc7z ),
						m_AList.add( new CArc7zZip );
	if( !dead['Z'] )	m_AList.add( new CArcUnZip );
	if( !dead['z'] )	m_AList.add( new CArcZip );
	if( !dead['T'] )	m_AList.add( new CArcTar );
	if( !dead['C'] )	m_AList.add( new CArcCab );
	if( !dead['R'] )	m_AList.add( new CArcRar );
	if( !dead['A'] )	m_AList.add( new CArcArj );
	if( !dead['B'] )	m_AList.add( new CArcBga );
	if( !dead['Y'] )	m_AList.add( new CArcYz1 );
	if( !dead['G'] )	m_AList.add( new CArcGca );
	if( !dead['a'] )	m_AList.add( new CArcAce );
	if( !dead['M'] )	m_AList.add( new CArcMsc );
	if( !dead['c'] )	m_AList.add( new CArcCpt );

	// 拡張スクリプトロード
	char prev_cur[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, prev_cur);
	::SetCurrentDirectory( CArcB2e::init_b2e_path() );
	kiFindFile find;
	find.begin( "*.b2e" );
	WIN32_FIND_DATA fd;
	for( int t=0; find.next(&fd); t++ )
		m_AList.add( new CArcB2e(fd.cFileName) );
	m_b2e = (t>1);
	::SetCurrentDirectory(prev_cur);
}

//----------------------------------------------//
//------------ ファイルリストを記憶 ------------//
//----------------------------------------------//

unsigned long CNoahArchiverManager::set_files( const cCharArray& files )
{
	//-- クリア
	m_FName.empty();
	m_BasePathList.empty();

	//-- 基底パスを取得( 出来るだけ利用範囲を広げるため、8.3形式で )
	if( files.len() != 0 )
	{
		char spath[MAX_PATH];
		m_BasePath =
			( 0!=::GetShortPathName( files[0], spath, MAX_PATH ) )
			? spath : "";
		if( !m_BasePath.beDirOnly() )
		{
			m_BasePath.beSpecialPath( kiPath::Cur );
			m_BasePath.beBackSlash( true );
		}
	}

	//-- 短いファイル名と長いのを両方取得しておく
	m_FName.alloc( files.len() );
	m_BasePathList.alloc( files.len() );
	for( unsigned int i=0,c=0; i!=files.len(); i++ )
		if( kiFindFile::findfirst( files[i], &m_FName[c] ) )
		{
			if( m_FName[c].cAlternateFileName[0] == '\0' )
				::lstrcpy(m_FName[c].cAlternateFileName,m_FName[c].cFileName);
			m_BasePathList[c] = files[i];
			if( !m_BasePathList[c].beDirOnly() )
			{
				m_BasePathList[c].beSpecialPath( kiPath::Cur );
				m_BasePathList[c].beBackSlash( true );
			}
			++c;
		}
	m_FName.forcelen( c );
	m_BasePathList.forcelen( c );
	return c;
}

//----------------------------------------------//
//--- ファイルリストに解凍ルーチンを割り当て ---//
//----------------------------------------------//

// 指定された拡張子に対応しているルーチンを線形探索
CArchiver* CNoahArchiverManager::fromExt( const char* ext )
{
	kiStr tmp = ext;
	tmp.lower();

	for( unsigned int i=0; i!=m_AList.len(); i++ )
		if( m_AList[i]->extCheck( tmp ) 
		 && (m_AList[i]->ability() & aMelt) )
			return m_AList[i];
	return NULL;
}

bool CNoahArchiverManager::map_melters( int mode ) // 1:cmp 2:mlt 3:must_mlt
{
	// クリア
	m_Melters.empty();

#define attrb (m_FName[ct].dwFileAttributes)
#define lname (m_FName[ct].cFileName)
#define sname (m_FName[ct].cAlternateFileName[0]==0 ? m_FName[ct].cFileName : m_FName[ct].cAlternateFileName)

	kiPath fnm;
	const char* ext;
	for( unsigned int ct=0, bad=0; ct!=file_num(); ct++ )
	{
//		fnm = m_BasePath, fnm += sname;
		fnm = m_BasePathList[ct], fnm += sname;

		//-- 0byteファイル / ディレクトリは弾く
		if( !(attrb & FILE_ATTRIBUTE_DIRECTORY) && 0!=kiFile::getSize( fnm, 0 ) )
		{
			//-- まず対応拡張子かどうかで候補Ａを一つ選出
			CArchiver* x = fromExt( ext=kiPath::ext(lname) );

			//-- 候補Ａで、ファイル内容によるチェック
			if( x && x->check( fnm ) )
			{
				m_Melters.add( x );
				continue;
			}

			//-- 候補Ａが内容チェック不可なものだったらそれを使う
			if( x && !(x->ability() & aCheck) )
			{
				m_Melters.add( x );
				continue;
			}

			//-- 候補Ａがダメなら、その他の内容チェック可能なルーチン全てで試す
			if( mode!=1 || 0==ki_strcmpi( "exe", ext ) )
			{
				for( unsigned long j=0; j!=m_AList.len(); j++ )
					if( m_AList[j]!=x && m_AList[j]->check( fnm ) )
					{
						m_Melters.add( m_AList[j] );
						break;
					}
				if( m_Melters.len() == ct+1 )
					continue;
			}
		}

		//-- チェックの結果、解凍不能でしたとさ
		if( mode!=3 )
			return false; //-- 解凍専用モードでなければ終了
		m_Melters.add( NULL ), bad++;
	}
#undef sname
#undef lname
#undef attrb

	return (ct!=bad);
}

//----------------------------------------------//
//--- ファイルリストに圧縮ルーチンを割り当て ---//
//----------------------------------------------//

bool CNoahArchiverManager::map_compressor( const char* ext, const char* method, bool sfx )
{
	int m;
	m_Method = -1;
	m_Sfx    = sfx;

	for( unsigned int i=0; i!=m_AList.len(); i++ )
		if( -1 != (m=m_AList[i]->cancompressby(ext,method,sfx)) )
			if( m!=-2 ) // 完全一致
			{
				m_Compressor = m_AList[i];
				m_Method = m;
				break;
			}
			else if( m_Method == -1 ) // 形式名のみ一致した最初のモノ
			{
				m_Compressor = m_AList[i];
				m_Method = m_AList[i]->cmp_mhd_default();
			}
	return (m_Method != -1);
}

//----------------------------------------------//
//------------ バージョン情報文字列 ------------//
//----------------------------------------------//

void CNoahArchiverManager::get_version( kiStr& str )
{
	kiStr tmp;
	for( unsigned int i=0; i!=m_AList.len(); i++ )
		if( m_AList[i]->ver( tmp ) )
			str+=tmp, str+="\r\n";
}

//----------------------------------------------//
//--------------- 圧縮形式リスト ---------------//
//----------------------------------------------//

static unsigned int find( const cCharArray& x, const char* o )
{
	for( unsigned int i=0; i!=x.len(); i++ )
		if( 0==ki_strcmp( x[i], o ) )
			return i;
	return 0xffffffff;
}

static unsigned int find( const StrArray& x, const char* o )
{
	for( unsigned int i=0; i!=x.len(); i++ )
		if( x[i]==o )
			return i;
	return 0xffffffff;
}

void CNoahArchiverManager::get_cmpmethod(
		const char* set,
		int& def_mhd,
		StrArray& mhd_list,
		bool need_ext,
		cCharArray* ext_list )
{
	def_mhd = -1;

	const char* x;
	for( unsigned int i=0; i!=m_AList.len(); i++ )
	{
		if( *(x = m_AList[i]->cmp_ext())=='\0' )
			continue;
		if( need_ext )
		{
			if( -1 == find( *ext_list, x ) )
				ext_list->add( x );
		}
		if( 0 == ki_strcmp( set, x ) )
		{
			if( mhd_list.len()==0 )
			{
				def_mhd = m_AList[i]->cmp_mhd_default();
				for( unsigned int j=0; j!=m_AList[i]->cmp_mhd_list().len(); j++ )
					mhd_list.add( (m_AList[i]->cmp_mhd_list())[j] );
			}
			else
			{
				for( unsigned int j=0; j!=m_AList[i]->cmp_mhd_list().len(); j++ )
					if( -1 == find( mhd_list, (m_AList[i]->cmp_mhd_list())[j] ) )
						mhd_list.add( (m_AList[i]->cmp_mhd_list())[j] );
			}
		}
	}

	if( def_mhd == -1 )
		def_mhd = 0;
}

//----------------------------------------------//
//--------------- 書庫一覧モード ---------------//
//----------------------------------------------//

#include "SubDlg.h"

void CNoahArchiverManager::do_listing( kiPath& destdir )
{
	kiWindow* mptr = app()->mainwnd();
	kiPath ddir;
	int    kMakeDirFlag = mycnf().mkdir();
	bool   rmn = mycnf().mnonum();
	destdir.beBackSlash( true );

	//-- ダイアログの個数カウンタをクリア
	kiArray<CArcViewDlg*> views;
	CArcViewDlg::clear();

	//-- ダイアログ起動
	for( unsigned int i=0; i!=m_FName.len(); i++ )
	{
		if( !m_Melters[i] )
			continue;

		arcname an(
			m_BasePathList[i],
//			m_BasePath,
			m_FName[i].cAlternateFileName[0]==0 ? m_FName[i].cFileName : m_FName[i].cAlternateFileName,
			m_FName[i].cFileName );
		ddir = destdir;

		if( kMakeDirFlag )
			generate_dirname( m_FName[i].cFileName, ddir, rmn );

		CArcViewDlg* x = new CArcViewDlg( m_Melters[i],an,ddir );
		views.add( x );
		x->createModeless( NULL );
	}

	//-- 全部終了するまで待機
	kiWindow::msgLoop( kiWindow::GET );

	//-- お終い
	app()->setMainWnd( mptr );
	for( i=0; i!=views.len(); i++ )
		delete views[i];
}

//----------------------------------------------//
//----------------- 解凍作業 -------------------//
//----------------------------------------------//

void CNoahArchiverManager::do_melting( kiPath& destdir )
{
	//-- 設定ロード
	const int  kMakeDirFlag = mycnf().mkdir();  // Make Directory Flag( 0:no 1:no1file 2: noddir 3:yes )
	const bool rmn = mycnf().mnonum(); // Remove NuMber ?

	//-- 出力先
	destdir.beBackSlash( true );
	destdir.mkdir(), destdir.beShortPath();

	for( unsigned int i=0; i!=m_FName.len(); i++ )
		if( m_Melters[i] )
		{
			//-- 出力先

			int mk=2; // 0:no 1:yes 2:???
			kiPath ddir( destdir ), dnm;
			if( kMakeDirFlag==0 )
				mk=0;
			else if( kMakeDirFlag==3 )
				mk=1;
			else
			{
				kiPath anm(m_BasePathList[i]);
//				kiPath anm(m_BasePath);
				anm+=m_FName[i].cFileName;
				int c = m_Melters[i]->contents( anm, dnm );
				if( c==aSingleDir || (c==aSingleFile && kMakeDirFlag==1) )
					mk=0; // ２重フォルダ防止処理(強)
				else if( c==aMulti )
					mk=1;
			}
			if( mk )
			{
				generate_dirname( m_FName[i].cFileName, ddir, rmn );
				if( mk==2 && kiSUtil::exist(ddir) )
					mk=1;
				ddir+='\\';
				ddir.mkdir();
				ddir.beShortPath();
			}

			//-- 解凍！

			arcname an( m_BasePathList[i],
//			arcname an( m_BasePath,
				m_FName[i].cAlternateFileName[0]==0 ? m_FName[i].cFileName : m_FName[i].cAlternateFileName,
				m_FName[i].cFileName );
			int result = m_Melters[i]->melt( an, ddir );
			if( result<0x8000 )
			{
				if( mk==2 ) // ２重フォルダ防止処理(弱)
					break_ddir( ddir, kMakeDirFlag==2 );
				else if( mk==0 && dnm.len() ) // ２重フォルダ防止処理(強)
					if( dnm.len()<=1 || dnm[1]!=':' ) // 絶対パスは開かない
						ddir+=dnm, ddir+='\\';
				// 出力先を開くかも
				myapp().open_folder( ddir, 1 );
			}
			else if( result!=0x8020 )
			{
				//エラー！
				char str[255];
				wsprintf( str, "%s\nError No: [%x]",
					(const char*)kiStr().loadRsrc( IDS_M_ERROR ), result );
				app()->msgBox( str );
			}
		}
}

void CNoahArchiverManager::generate_dirname( const char* src, kiPath& dst, bool rmn )
{
	// srcで示された書庫名からディレクトリ名を生成し、
	// dstへ足す。rmn==trueなら末尾の数字も削除

	// 一番左の . と左から二番目の . を探す
	const char *fdot=NULL, *sdot=NULL, *tail;
	for( tail=src; *tail; tail=kiStr::next(tail) )
		if( *tail=='.' )
			sdot=fdot, fdot=tail;

	// .tar.xxx か、.xxx.gz/.xxx.z/.xxx.bz2 なら二つ削除
	if( fdot )
	{
		tail = fdot;
		if( sdot )
			if( 0==::lstrcmpi(fdot,".gz")
			 || 0==::lstrcmpi(fdot,".z")
			 || 0==::lstrcmpi(fdot,".bz2")
			 || (sdot+4==fdot
			 && (sdot[1]=='t'||sdot[1]=='T')
			 && (sdot[2]=='a'||sdot[2]=='A')
			 && (sdot[3]=='r'||sdot[3]=='R')
			))
				tail = sdot;
	}

	// 末尾の数字と'-'と'_'と'.'削除。半角スペースも。
	bool del[256];
	ki_memzero( del, sizeof(del) );
	if( rmn )
	{
		del['-'] = del['_'] = del['.'] = true;
		for( char c='0'; c<='9'; ++c )
			del[c] = true;
	}
	del[' '] = true;

	const char* mjs=NULL;
	for( const char *x=src; x<tail; x=kiStr::next(x) )
		if( !del[(unsigned char)(*x)] )
			mjs = NULL;
		else if( !mjs )
			mjs = x;
	if( mjs && mjs!=src )
		tail = mjs;

	// 空になってしまったら "noahmelt" という名前にしてしまう。
	if( src==tail )
		dst += "noahmelt";
	else
		while( src!=tail )
			dst += *src++;
}

bool CNoahArchiverManager::break_ddir( kiPath& dir, bool onlydir )
{
// ２重フォルダ or 単一ファイル 状態を解消
//
// 素直に格納ファイル名を一度走査するのが本当なんですが、
// 特に巨大書庫の時速度低下が激しいのと、FindFirst系を
// サポートしたDLLや内蔵エンジン以外に対応できないという
// 欠点があるため、相変わらず Noah 2.xx と同じ手法です。

//-- 中に１個しか入ってないことを確認 -----------------
	char wild[MAX_PATH];
	ki_strcpy( wild, dir );
	ki_strcat( wild, "*.*" );
	kiFindFile find;
	if( !find.begin( wild ) )
		return false;
	WIN32_FIND_DATA fd,fd2,fd3;
	find.next( &fd );
	if( find.next( &fd2 ) )
		return false;
	find.close();
//----------------------------------------------------

//-- to:最終移動先ファイル名。ついでに、カレントDirは消せない問題の回避策 -----
	kiPath to(dir); to.beBackSlash( false ), to.beDirOnly();
	::SetCurrentDirectory( to );
	to += fd.cFileName;
//-------------------------------------------------------------------------

//-- ファイルだった場合 --------------------------------------
	if( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		if( !onlydir )
		{
			// now 現在のファイル名
			kiStr now=dir; now+=fd.cFileName;

			// now -> to 移動
			if( ::MoveFile( now, to ) )
			{
				dir.remove();
				dir.beBackSlash( false ), dir.beDirOnly();
				return true;
			}
		}
	}
//-- フォルダだった場合 ----------------------------------------
	else
	{
		// 'base/aaa/aaa/' だと中のaaaを外にmoveできない。
		// よって、回避策。-> 'base/aaa_noah_tmp_178116/aaa/'

		dir.beBackSlash( false );
		kiFindFile::findfirst( dir, &fd3 );
		kiPath dirx( dir ); dirx+="_noah_tmp_178116";

		if( ::MoveFile( dir, dirx ) )
		{
			// now 現在のファイル名
			kiStr now( dirx ); now+='\\', now+=fd.cFileName;

			// ディレクトリを移動
			if( ::MoveFile( now, to ) )
			{
				dirx.remove();
				dir=to, dir.beBackSlash( true );
				return true;
			}
			else
			{
				// 'base/aaa_noah_tmp_178116/aaa/' -> 'base/aaa/aaa/'
				dir.beDirOnly(), dir+=fd3.cFileName;
				::MoveFile( dirx, dir );
			}
		}

		dir.beBackSlash( true );
	}
//------------------------------------------------------------
	return false;
}

//----------------------------------------------//
//----------------- 圧縮作業 -------------------//
//----------------------------------------------//

void CNoahArchiverManager::do_compressing( kiPath& destdir, bool each )
{
	int result = 0xffff, tr;

	// 出力先を確実に作っておく
	destdir.beBackSlash( true );
	destdir.mkdir();
	destdir.beShortPath();

	// 個別圧縮モードか、Archiving不可の形式なら一個ずつ
	if( each || !(m_Compressor->ability() & aArchive) )
	{
		wfdArray templist;

		for( unsigned int i=0; i!=m_FName.len(); i++ )
		{
			templist.empty();
			templist.add( m_FName[i] );
			tr = m_Compressor->compress( m_BasePath,templist,destdir,m_Method,m_Sfx );
			if( tr<0x8000 || tr==0x8020 )
				result = tr;
		}
	}
	else
		result = m_Compressor->compress( m_BasePath,m_FName,destdir,m_Method,m_Sfx );

	// 開くかも
	if( result<0x8000 )
		myapp().open_folder( destdir, 2 );
	else if( result!=0x8020 )
	{
		//エラー！
		char str[255];
		wsprintf( str, "%s\nError No: [%x]", "Compression Error", result );
		app()->msgBox( str );
	}
}
