
#include "stdafx.h"
#include "ArcB2e.h"
#include "resource.h"
#include "NoahApp.h"

//----------------- ArcB2eクラス全体的な処理 ------------------------------

char CArcB2e::st_base[MAX_PATH];
int  CArcB2e::st_life=0;
CArcB2e::CB2eCore* CArcB2e::rvm=NULL;

const char* CArcB2e::init_b2e_path()
{
	kiPath dir( kiPath::Exe );
	ki_strcpy( st_base, dir+="b2e\\" );
	return st_base;
}

CArcB2e::CArcB2e( const char* scriptname ) : CArchiver( scriptname )
{
	st_life++;
	exe = NULL;
	m_LstScr = m_DcEScr = m_EncScr =
	m_DecScr = m_SfxScr = m_LoadScr= m_ScriptBuf = NULL;
}

CArcB2e::~CArcB2e()
{
	if( !(--st_life) )
		delete rvm;
	delete [] m_ScriptBuf;
}

//------------------- スクリプト側にあまり関係しない部分 -------------------------

bool CArcB2e::v_ver( kiStr& str )
{
	if( !exe )
		return false;
	exe->ver( str );

	kiStr tmp;
	for( int i=0,e=m_subFile.len(); i<e; ++i )
	{
		str += "\r\n";
		CArcModule(m_subFile[i]).ver( tmp );
		str += tmp;
	}
	return true;
}

bool CArcB2e::v_check( const kiPath& aname )
{
	return exe ? exe->chk( aname ) : false;
}

int CArcB2e::v_contents( const kiPath& aname, kiPath& dname )
{
	return exe ? exe->cnt( aname, dname ) : aUnknown;
}

//------------------- スクリプトを読み込み＆eval( load: ) -------------------

bool CArcB2e::load_module( const char* name )
{
	exe = new CArcModule( name, m_usMode );
	return exe->exist();
}

int CArcB2e::v_load()
{
	//-- 拡張スクリプトファイルを開く
	kiStr fname( st_base ); fname += mlt_ext();
	kiFile fp;
	if( fp.open( fname ) )
	{
		//-- ファイル全体を読み込み
		unsigned int ln=fp.getSize();
		m_ScriptBuf = new char[ ln+1 ];
		ln = fp.read( (unsigned char*)m_ScriptBuf, ln );
		m_ScriptBuf[ ln ] = '\0';

		//-- section毎に切り分ける
		bool pack1,chk=false;
		for( char* p=m_ScriptBuf; *p; p++ )
		{
			switch( *p )
			{
			case 'c': case 'd': case 'e': case 'l': case 's':
				if( ki_memcmp(p,"load:",5) )
					*p='\0', m_LoadScr = (p+=4)+1;
				else if( ki_memcmp(p,"encode:",7) )
					*p='\0', m_EncScr = (p+=6)+1, pack1=false;
				else if( ki_memcmp(p,"encode1:",8) )
					*p='\0', m_EncScr = (p+=7)+1, pack1=true;
				else if( ki_memcmp(p,"decode:",7) )
					*p='\0', m_DecScr = (p+=6)+1;
				else if( ki_memcmp(p,"sfx:",4) )
					*p='\0', m_SfxScr = (p+=3)+1, m_SfxDirect=false;
				else if( ki_memcmp(p,"sfxd:",5) )
					*p='\0', m_SfxScr = (p+=4)+1, m_SfxDirect=true;
				else if( ki_memcmp(p,"check:",6) )
					*p='\0', (p+=5), chk=true;
				else if( ki_memcmp(p,"decode1:",8) )
					*p='\0', m_DcEScr = (p+=7);
				else if( ki_memcmp(p,"list:",5) )
					*p='\0', m_LstScr = (p+=4);
			}
			while( *p && *p!='\n' && *p!='\r' )
				p++;
			if( *p=='\0' )
				break;
		}

		//-- [load:]を実行！
		if( m_LoadScr )
		{
			//-- RythpVM 起動
			if( !rvm )
				rvm = new CB2eCore;

			//-- 初期化
			m_Result=0;
			rvm->setPtr( this,mLod );

			//-- 実行
			rvm->eval( m_LoadScr );

			//-- 結果
			if( m_Result==0 )
				return (m_DecScr?aMelt|(m_DcEScr?aList|aMeltEach:0)|(chk?aCheck:0):0)
					 | (m_EncScr?aCompress|(pack1?0:aArchive)|(m_SfxScr?aSfx:0):0);
		}
	}
	return 0;
}

int CArcB2e::exec_script( const char* scr, scr_mode mode )
{
	//-- 初期化
	m_Result = 0;
	rvm->setPtr( this, mode );

	//-- 実行
	char* script = new char[ki_strlen(scr)+8];
	ki_strcpy( script, "(exec " );
	ki_strcat( script, scr );
	ki_strcat( script, ")" );
	rvm->eval( script );
	delete [] script;

	//-- 結果
	return m_Result;
}

//-------------------- リストアップ eval( list: ) -----------------------

bool CArcB2e::v_list( const arcname& aname, aflArray& files )
{
	//-- スクリプト無しで何とかできるならする。
	if( !exe )
		return false;
	else if( exe->isdll() )
		return exe->lst_dll( aname, files );
	else if( !m_LstScr )
		return false;

//-- リスティングスクリプトに必要なデータ

	// 書庫名
	m_psArc = &aname;
	// ファイルリスト
	m_psAInfo = &files;

//-- 実行！ ---------------------

	return 0==exec_script( m_LstScr, mLst );
}

//-------------------- 展開処理 eval( decode: ) -----------------------

int CArcB2e::v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files )
{
//-- 解凍スクリプトに必要なデータ

	// カレント
	::SetCurrentDirectory( ddir );
	// 書庫名
	m_psArc = &aname;
	// 出力先ディレクトリ
	m_psDir = &ddir;
	// ファイルリスト
	m_psAInfo = files;

//-- 実行！ ---------------------

	return exec_script( files ? m_DcEScr : m_DecScr,
						files ? mDc1     : mDec );
}

//-------------------- 圧縮処理 eval( encode: sfx: ) -----------------------

int CArcB2e::cmpr( const char* scr, const kiPath& base, const wfdArray& files, const kiPath& ddir, const int method )
{
//-- 圧縮スクリプトに必要なデータ

	arcname aname(
		ddir,
		files[0].cAlternateFileName,
		files[0].cFileName );
	int mhd=method+1;

	// カレント
	::SetCurrentDirectory( base );
	// 書庫名
	m_psArc = &aname;
	// ベースディレクトリ
	m_psDir = &base;
	// メソッド
	m_psMhd = &mhd;
	// リスト
	m_psList = &files;

//-- 実行！ --------------------

	return exec_script( scr, mEnc );
}

bool CArcB2e::arc2sfx( const kiPath& temp, const kiPath& dest )
{
//-- SFX変換スクリプトに必要なデータ

	kiFindFile f;
	WIN32_FIND_DATA fd;
	kiPath wild( temp );
	f.begin( wild += "*" );
	if( !f.next( &fd ) )
		return false;
	kiPath from, to, oldname( fd.cFileName );
	arcname aname( temp, fd.cAlternateFileName[0] ? fd.cAlternateFileName : fd.cFileName, fd.cFileName );

	// カレント
	::SetCurrentDirectory( temp );
	// 書庫名
	m_psArc = &aname;
	// ディレクトリ
	m_psDir = &temp;

//-- 実行！ ----------------------

	if( 0x8000<=exec_script( m_SfxScr, mSfx ) )
		return false;

//-- コピー ----------------------

	bool skipped=false, ans=false;
	f.begin( wild );
	while( f.next( &fd ) )
	{
		if( !skipped && oldname == fd.cFileName ) // テンポラリ書庫はコピーしない。
		{
			skipped=true;
			continue;
		}
		from = temp, from += fd.cFileName;
		to   = dest, to   += fd.cFileName;
		if( ::CopyFile( from, to, FALSE ) )
			ans = true;
	}
	return ans;
}

int CArcB2e::v_compress( const kiPath& base, const wfdArray& files, const kiPath& ddir, int method, bool sfx )
{
	const char* theScript = m_EncScr;

	if( sfx )
	{
		if( m_SfxDirect )
			theScript = m_SfxScr;
		else
		{
			kiPath tmp;
			myapp().get_tempdir( tmp );

			// テンポラリへ圧縮
			int ans = cmpr( m_EncScr, base, files, tmp, method );
			if( ans < 0x8000 )
				// テンポラリに落ちてるファイルをSFXに変換＆コピー！
				ans = (arc2sfx( tmp, ddir ) ? 0 : 0x8020);

			// カレントを戻しておかないと削除できない…(;_;)
			::SetCurrentDirectory( base );
			tmp.remove();
			return ans;
		}
	}

	// 出力先へ普通に圧縮
	return cmpr( theScript, base, files, ddir, method );
}

//-----------------------------------------------------------------//
//-------------------- RythpVMの方の実務 --------------------------//
//-----------------------------------------------------------------//

bool CArcB2e::CB2eCore::exec_function( const kiVar& name, const CharArray& a, const BoolArray& b, int c, kiVar* r )
{
	bool processed = false;

	if( m_mode==mLod ){ //**ロード時専用functions****************************
		if( name=="name" ){
			processed=true;

			//---------------------------//
			//-- (name module_filename)--//
			//---------------------------//
			if( c>=2 )
			{
				x->m_usMode = false;
				if( c>=3 )
				{
					getarg( a[2],b[2],&t );
					x->m_usMode = ( t=="us" );
				}

				getarg( a[1],b[1],&t );
				if( x->load_module(t) )
					*r = "exec";
				else
					*r = "", x->m_Result=0xffff;
			}

		}else if( name=="type" ){
			processed=true;

			//-----------------------------------//
			//-- (type ext method1 method2 ...)--//
			//-----------------------------------//
			for( int i=1; i<c; i++ )
			{
				getarg( a[i],b[i],&t );
				if( i==1 )
					x->set_cmp_ext( t );
				else
				{
					const char* ptr=t;
					x->add_cmp_mhd( *ptr=='*' ? ptr+1 : ptr, *ptr=='*' );
				}
			}
		}else if( name=="use" ){
			processed=true;

			//-------------------------------//
			//-- (use module1 module2 ...) --//
			//-------------------------------//
			for( int i=1; i<c; i++ )
			{
				getarg( a[i],b[i],&t );
				x->m_subFile.add( t );
			}
		}
	}else{//************ ロード時には使えないfunctions *********************
		if( ki_memcmp( (const char*)name, "arc", 3 ) ){
			processed=true;

			//---------------------------//
			//-- (arc[+-].xxx [slfrd]) --//
			//---------------------------//
			arc( ((const char*)name)+3, a, b, c, r );

		}else if( ki_memcmp( (const char*)name, "list", 4 ) ){
			processed=true;

			//----------------------------//
			//-- (list[\*|\*.*] [slfn]) --//
			//----------------------------//
			list( ((const char*)name)+4, a, b, c, r );

		}else if( name=="method" ){
			processed=true;

			//-------------------//
			//-- (method [no]) --//
			//-------------------//
			if( c>=2 )
			{
				getarg( a[1],b[1],&t );
				*r = t.getInt()==*x->m_psMhd ? "1" : "0";
			}
			else
				r->setInt( *x->m_psMhd );

		}else if( name=="dir" ){
			processed=true;

			//-----------//
			//-- (dir) --//
			//-----------//
			*r = (x->m_psDir ? *x->m_psDir : (const char*)"");

		}else if( name=="del" ){
			processed=true;

			//-------------------//
			//-- (del filenam) --//
			//-------------------//
			if( c>=2 )
			{
				getarg( a[1],b[1],&t );
				::DeleteFile( kiPath( t.unquote() ) );
			}

		}else if( ki_memcmp( (const char*)name, "resp", 4 )
			||	  ki_memcmp( (const char*)name, "resq", 4 ) ){
			processed=true;

			//----------------------------//
			//-- (resp[@|-o] (list a)) ---//
			//----------------------------//
			resp( name[3]=='p', ((const char*)name)+4, a, b, c, r );

		}else if( name=="cd" ){
			processed=true;

			//-------------------//
			//-- (cd directory)--//
			//-------------------//
			if( c>=2 )
			{
				getarg( a[1],b[1],&t );
				::SetCurrentDirectory( t.unquote() );
			}

		}else if( name=="cmd" || name=="xcmd" ){
			processed=true;

			//----------------------------//
			//-- (cmd command line ...)---//
			//-- (xcmd command line ...)--//
			//----------------------------//
			if( name[0]=='x' && c<2 )
				x->m_Result = 0xffff;
			else
			{
				CArcModule* xxx = x->exe;
				kiVar       cmd;
				int         i=1;

				if( name[0] == 'x' )
				{
					kiVar mm;
					getarg( a[i],b[i],&mm );
					i++;
					xxx = new CArcModule( mm, x->m_usMode );
				}
				for( ; i<c; i++ )
					getarg( a[i],b[i],&t ), cmd+=t, cmd+=' ';

				bool m = (mycnf().miniboot() || m_mode==mDc1);
				x->m_Result = xxx->cmd( cmd, m );
				r->setInt( x->m_Result );

				if( name[0] == 'x' )
					delete xxx;
			}
		}else if( name=="scan" || name=="xscan" ){
			processed=true;

			//----------------------------------------//
			//-- (scan BL BSL EL SL dx cmd...) -------//
			//-- (xscan BL BSL EL SL dx CMD cmd...) --//
			//----------------------------------------//
			if( c<6 || (name[0]=='x'&&c<7) )
				x->m_Result = 0xffff;
			else
			{
				CArcModule* xxx = x->exe;

				kiVar BL, EL;
				getarg( a[1],b[1],&BL );
				getarg( a[2],b[2],&t );
				int BSL = t.getInt();
				getarg( a[3],b[3],&EL );
				getarg( a[4],b[4],&t );
				int SL = t.getInt();
				getarg( a[5],b[5],&t );
				int dx = t.getInt();

				int i=6;
				if( name[0] == 'x' )
				{
					kiVar mm;
					getarg( a[i],b[i],&mm );
					i++;
					xxx = new CArcModule( mm, x->m_usMode );
				}

				kiVar cmd;
				for( ; i<c; ++i )
					getarg( a[i],b[i],&t ), cmd+=t, cmd+=' ';

				x->m_Result = xxx->lst_exe(
					cmd, *const_cast<aflArray*>(x->m_psAInfo),
					BL, BSL, EL, SL, dx ) ? 0 : -1;

				if( name[0] == 'x' )
					delete xxx;
			}
		}else if( name=="input" ){
			processed=true;

			//-------------------------//
			//-- (input MSG DEFUALT) --//
			//-------------------------//
			kiVar msg, defval;
			if( c>=2 )
				getarg( a[1],b[1],&msg );
			if( c>=3 )
				getarg( a[2],b[2],&defval );
			input( msg, defval, r );
		}else if( name=="size" ){
			processed=true;

			//---------------------//
			//-- (size FILENAME) --//
			//---------------------//
			if( c>=2 )
			{
				kiVar fnm;
				getarg( a[1],b[1],&fnm );
				r->setInt( kiFile::getSize( fnm.unquote() ) );
			}
		}else if( name=="is_file" ){
			processed=true;

			//---------------------//
			//-- (is_file) --------//
			//---------------------//
			if( c==1 )
				*r = (x->m_psList->len()==1
					  && !kiSUtil::isdir( (*x->m_psList)[0].cFileName )) ? "1" : "0";
		}else if( name=="is_folder" ){
			processed=true;

			//---------------------//
			//-- (is_folder) ------//
			//---------------------//
			if( c==1 )
				*r = (x->m_psList->len()==1
					  && kiSUtil::isdir( (*x->m_psList)[0].cFileName )) ? "1" : "0";
		}else if( name=="is_multiple" ){
			processed=true;

			//---------------------//
			//-- (is_multiple) ----//
			//---------------------//
			if( c==1 )
				*r = x->m_psList->len()>1 ? "1" : "0";
		}else if( name=="find" ){
			processed=true;

			//---------------------//
			//-- (find FILENAME) --//
			//---------------------//
			if( c>=2 )
			{
				kiVar fnm;
				getarg( a[1],b[1],&fnm );
				char buf[MAX_PATH];
				if( 0==::SearchPath( NULL,fnm.unquote(),NULL,MAX_PATH,buf,NULL ) )
					*r = "";
				else
					*r = buf, r->quote();
			}
		}
	}

	return processed ? true : kiRythpVM::exec_function(name,a,b,c,r);
}

void CArcB2e::CB2eCore::arc( const char* opt, const CharArray& a, const BoolArray& b,int c, kiVar* r )
{
	//---------------------------//
	//-- (arc[+-].xxx [slfrd]) --//
	//---------------------------//

	// デフォルトオプション設定
	const char* anm=x->m_psArc->lname;
	enum{ full, nam, dir } part=full;
	if( m_mode==mSfx )	part=nam; // sfx

	// 指定があれば上書
	if( c>=2 )
	{
		getarg( a[1],b[1],&t );
		for( const char* p=t; *p; p++ )
			switch(*p)
			{
			case 's': anm=x->m_psArc->sname; break;
			case 'l': anm=x->m_psArc->lname; break;
			case 'f': part=full; break;
			case 'n': part=nam;  break;
			case 'd': part=dir;  break;
			}
	}

	// ディレクトリ部分
	*r = (part==nam ? (const char*)"" : x->m_psArc->basedir);

	// 名前部分
	if( part != dir )
	{
		if( *opt=='\0' || *opt=='+' )
		{
			// (arc)       : anmをそのまま返す
			*r += anm;
			// (arc+XXX)   : anmXXXを返す
			if( *opt=='+' )
				*r += (opt+1);
		}
		else
		{
			const char* ext = kiPath::ext(anm);
			const char* add = "";
			if( opt[0]=='-' && opt[1]=='.' )
			{
				// (arc-.XXX) : 末尾が.XXXだったら削除。
				//            : そうでなければ後ろに.decompressed
				if( 0!=ki_strcmpi( ext, opt+2 ) )
					ext = anm + ki_strlen(anm), add = ".decompressed";
			}
			else if( opt[1]!='\0' )
			{
				// (arc.XXX)  : 最後の拡張子を.XXXに取り替え
				add = opt;
				switch(mycnf().extnum())
				{
				case 0: ext = anm + ::lstrlen(anm);break; 
				case 1: ext = kiPath::ext(anm);    break;
				default:ext = kiPath::ext_all(anm);break;
				}
			}
			else
			{
				// (arc.)     : 拡張子を全て取り除く
				switch(mycnf().extnum())
				{
				case 0: ext = anm + ::lstrlen(anm);break; 
				case 1: ext = kiPath::ext(anm);    break;
				default:ext = kiPath::ext_all(anm);break;
				}
			}
			if( *ext )
				ext--;

			char buf[MAX_PATH];
			ki_memcpy( buf, anm, ext-anm );
			buf[ ext-anm ] = '\0';
			*r += buf;
			*r += add;
		}

		// 必要ならくくる
		if( part==full )
			r->quote();
	}
}

static void selfR(
	const char* writedir, const char* fullpath, bool lfn, kiVar* r )
{
	kiFindFile       f;
	WIN32_FIND_DATA fd;
	f.begin( kiStr(fullpath) += "\\*" );

	kiVar t, t2, t3;
	while( f.next(&fd) )
	{
		t = writedir;
		t+= '\\';
		t+= (lfn ? fd.cFileName : fd.cAlternateFileName);
		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			t2 = t;
			t  = "";
			t3 = fullpath;
			t3+= '\\';
			t3+= (lfn ? fd.cFileName : fd.cAlternateFileName);
			selfR( t2, t3, lfn, &t );
		}
		else
		{
			if( lfn )
				t.quote();
		}
		*r += t;
		*r += ' ';
	}
}

void CArcB2e::CB2eCore::list( const char* opt, const CharArray& a, const BoolArray& b,int c, kiVar* r )
{
	//---------------------------//
	//-- (list[r|\*.*] [slfn]) --//
	//---------------------------//

	if( m_mode!=mEnc ) // 解凍の場合
	{
		*r = "";

		for( unsigned int i=0; i!=x->m_psAInfo->len(); i++ )
			if( (*x->m_psAInfo)[i].selected )
			{
				// - で始まるヤツ対策をするか？
				t = (*x->m_psAInfo)[i].inf.szFileName;
				t.quote();
				*r += t;
				*r += ' ';
			}
	}
	else // 圧縮の場合
	{
		// デフォルトオプション設定
		bool lfn=true;
		enum{ full, nam } part=nam;
		// 指定があれば上書
		if( c>=2 )
		{
			getarg( a[1],b[1],&t );
			for( const char* p=t; *p; p++ )
				switch(*p)
				{
				case 's': lfn=false; break;
				case 'l': lfn=true;  break;
				case 'f': part=full; break;
				case 'n': part=nam;  break;
				}
		}
		// 自前で再帰リストアップを行うか否か
		bool selfrecurse = (*opt=='r');

		// ディレクトリ名の後ろに付け足すもの。
		if( *opt=='\\' || *opt=='/' )
			opt++;

		// リストアップ
		kiVar t2,t3;
		*r = "";
		for( unsigned int i=0; i!=x->m_psList->len(); i++ )
		{
			// ファイル名部分
			t = ( part==full ? *x->m_psDir : (const char*)"");
			t += lfn ? (*x->m_psList)[i].cFileName : (*x->m_psList)[i].cAlternateFileName;

			if( selfrecurse && ((*x->m_psList)[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// セルフ再帰
				t2 = t;
				t  = "";
				t3 = *x->m_psDir;
				t3+= lfn ? (*x->m_psList)[i].cFileName : (*x->m_psList)[i].cAlternateFileName;
				selfR( t2, t3, lfn, &t );
			}
			else
			{
				// ノーマル処理
				if( *opt && ((*x->m_psList)[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					t += '\\', t += opt;
				if( lfn )
					t.quote();
			}
			*r += t;
			*r += ' ';
		}
	}

	r->removeTrailWS();
}

void CArcB2e::CB2eCore::resp( bool needq, const char* opt, const CharArray& a, const BoolArray& b,int c, kiVar* r )
{
	//-----------------------------//
	//-- (resp[@|-o] (list) ...) --//
	//-----------------------------//

	// レスポンスファイル名作成
	kiPath rspfile;
	myapp().get_tempdir(rspfile);
	rspfile += "filelist";

	// オプションと結合して返す
	*r  = opt;
	*r += rspfile;

	// ファイルへ書き込み
	kiFile fp;
	if( !fp.open( rspfile,false ) )
		return;

	kiVar tmp;
	for( int i=1; i<c; i++ )
	{
		// fpへ各引数をsplitしながら書き込む
		getarg( a[i],b[i],&tmp );

		for( const char *s,*p=tmp; *p; p++ )
		{
			// 余分な空白はスキップ
			while( *p==' ' )
				p++;
			if( *p=='\0' )
				break;

			// 引数の終わりへ…
			s=p;
			for( int q=0; *p!='\0' && (*p!=' ' || (q&1)!=0); p++ )
				if( *p=='"' )
					q++;

			// "のつじつま合わせ一号
			if( !needq && *s=='"' )
			{
				s++;
				if( p!=s && *(p-1)=='"' )
					p--;
			}

			fp.write( s, p-s );
			fp.write( "\r\n", 2 );

			// "のつじつま合わせ二号
			if( *p=='"' )
				p++;
			if( *p=='\0' )
				break;
		}
	}
}

void CArcB2e::CB2eCore::input( const char* msg, const char* defval, kiVar* r )
{
	struct CB2eInputDlg : public kiDialog
	{
		const char* msg;
		const char* def;
		kiVar*      res;

		CB2eInputDlg( const char* m, const char* d, kiVar* r )
			: kiDialog( IDD_PASSWORD ), msg(m), def(d), res(r) {}
		BOOL onInit()
			{
				sendMsgToItem( IDC_EDIT, WM_SETTEXT, 0, (LPARAM)def );
				sendMsgToItem( IDC_MESSAGE, WM_SETTEXT, 0, (LPARAM)msg );
				::ShowWindow( item(IDC_MASK), SW_HIDE );
				::ShowWindow( item(IDCANCEL), SW_HIDE );
				::EnableWindow( item(IDC_MASK), FALSE );
				::EnableWindow( item(IDCANCEL), FALSE );
				::SetFocus( item(IDC_EDIT) );
				return TRUE;
			}
		bool onOK()
			{
				char* buf = new char[32768];
				sendMsgToItem( IDC_EDIT, WM_GETTEXT, 32768, (LPARAM)buf );
				*res = buf;
				delete [] buf;
				return true;
			}
	};

	CB2eInputDlg d( msg, defval, r );
	d.doModal( app()->mainhwnd() );
}

