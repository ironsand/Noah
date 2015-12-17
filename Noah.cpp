// Noah.cpp
// -- entrypoint etc for 'Noah'

#include "stdafx.h"
#include "NoahApp.h"
#include "resource.h"

#if _MSC_VER >= 1300
extern "C" { int _afxForceEXCLUDE; }
extern "C" BOOL WINAPI _imp__IsDebuggerPresent() { return FALSE; }
#endif

// プロセス個数制限ゾーン
class ProcessNumLimitZone
{
	HANDLE m_han;
public:
	ProcessNumLimitZone(int Max, const char* name)
		: m_han( ::CreateSemaphore( NULL, Max, Max, name ) )
	{
		if( m_han )
			::WaitForSingleObject( m_han, INFINITE );
		else
			kiSUtil::msgLastError("CreateSemaphore Failed");
	}
	~ProcessNumLimitZone()
	{
		if( m_han )
		{
			::ReleaseSemaphore( m_han, 1, NULL );
			::CloseHandle( m_han );
		}
	}
};

//----------------------------------------------//
//--------- Noah のエントリポイント ------------//
//----------------------------------------------//

void kilib_create_new_app()
{
	//-- kilib にアプリケーションを設定
	new CNoahApp;
}

void CNoahApp::run( kiCmdParser& cmd )
{
	//-- 初期化
	m_cnfMan.init();
	m_arcMan.init();

	//-- コマンドラインパラメータ保持
	m_pCmd = &cmd;

	//-- 「ファイル名が渡されてない or Shift押し起動」なら設定画面表示
	if( cmd.param().len()==0 || keyPushed(VK_SHIFT) )
	{
		//-- Load-INI ( 全部 )
		m_cnfMan.load( All );
		//-- 設定画面表示
		m_cnfMan.dialog();
	}
	else
	{
		//-- 圧縮解凍などの作業
		do_cmdline( true );
	}

	//-- 終了処理
	m_tmpDir.remove();
}

//----------------------------------------------//
//------------- 圧縮/解凍 の 作業 --------------//
//----------------------------------------------//

bool CNoahApp::is_writable_dir( const kiPath& path )
{
	// 要するに、CDROM/DVDROM を切りたい。
	// FDD, PacketWriteなDisk を切るのはあきらめる。

	// RAMDISK, REMOTE, FIXED, UNKNOWN なディスクは書き込み可能と見なす
	UINT drv = path.getDriveType();
	if( drv==DRIVE_REMOVABLE || drv==DRIVE_CDROM )
	{
		// 素Win95では使えない関数なのでDynamicLoad
		typedef BOOL (WINAPI*pGDFSE)( LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER );
		pGDFSE pGetDiskFreeSpaceEx
			= (pGDFSE) ::GetProcAddress( ::GetModuleHandle("kernel32.dll"), "GetDiskFreeSpaceExA" );
		if( pGetDiskFreeSpaceEx )
		{
			// 空き容量が0なら、書き込み不可とみなす
			ULARGE_INTEGER fs, dummy;
			pGetDiskFreeSpaceEx( path, &dummy, &dummy, &fs );
			if( fs.QuadPart == 0 )
				return false;
		}
	}
	return true;
}

void CNoahApp::do_cmdline( bool directcall )
{
	do_files( m_pCmd->param(), &m_pCmd->option(), !directcall );
}

void CNoahApp::do_files( const cCharArray& files,
						 const cCharArray* opts,
						 bool  basicaly_ignore )
{
	struct local {
		~local() {kiSUtil::switchCurDirToExeDir(); } // ディレクトリロックしないように
	} _;

	//-- Archiver Manager にファイル名リストを記憶する
	if( 0 == m_arcMan.set_files( files ) )
		return;

	//-- 作業用変数
	enum { unknown, melt, compress }
			whattodo = unknown;
	bool	ctrl_mlt = keyPushed( VK_CONTROL );
	bool	ctrl_cmp = ctrl_mlt;
	bool	alt      = keyPushed( VK_MENU ) || keyPushed( VK_RWIN ) || keyPushed( VK_LWIN );
	const char *cmptype=NULL, *method=NULL;
	kiPath  destdir;
	kiStr tmp(300);

	//-- ( もしあれば )コマンドラインオプションを解釈
	if( opts )
		for( unsigned int i=0; i!=opts->len(); i++ )
			switch( (*opts)[i][1] )
			{
			case 'a':	if( !basicaly_ignore )
			case 'A':		whattodo = compress;	break;

			case 'x':	if( !basicaly_ignore )
			case 'X':		whattodo = melt;		break;

			case 'd':	if( !basicaly_ignore )
			case 'D':		destdir = (*opts)[i]+2;	break;

			case 'w':	if( !basicaly_ignore )
			case 'W':		alt = true;				break;

			case 't':	if( !basicaly_ignore )
			case 'T':		cmptype = (*opts)[i]+2;	break;

			case 'm':	if( !basicaly_ignore )
			case 'M':		method = (*opts)[i]+2;	break;

			case 'c':	if( !basicaly_ignore ) {
			case 'C':		if((*opts)[i][2]!='x') ctrl_cmp = true;
							if((*opts)[i][2]!='a') ctrl_mlt = true;
						break;}
			}

	//-- Load-INI ( 動作モード設定 )
	m_cnfMan.load( Mode );

	//-- 圧縮解凍のどちらを行うか決定する。流れは以下の通り。
	//
	// ・コマンドラインで、圧縮と指定されてれば無条件で圧縮へ
	//
	// ・そうでなければ、まずNoahの動作モード取得
	// 　m0:圧縮専用  m1:圧縮優先  m2:解凍優先  m3:解凍専用
	// 　　コマンドラインで解凍と指定されていれば m3。
	// 　　指定が無ければ、m_cnfMan から読み込み。
	//
	// ・m0 か、'm1でしかもファイルが複数' の時は無条件で圧縮へ
	//
	// ・そうでなければ、解凍ルーチンを割り当ててみる。
	// 　この際、m3 以外のときは一個でも割り当て失敗したらエラー＞圧縮へ
	// 　m3 でも、一個も割り当てられなければエラー。＞処理終了

	if( whattodo != compress )
	{
		int mode = 3;
		if( whattodo != melt )
			mode = m_cnfMan.mode();

		if( mode==0 || ( mode==1 && m_arcMan.file_num()>=2 ) )
			whattodo = compress;
		else
		{
			//-- 解凍ルーチン割り当ててみる
			bool suc = m_arcMan.map_melters( mode );
			if( suc )
				whattodo = melt;
			else
			{
				if( mode != 3 )
					whattodo = compress;
				else
				{
					//-- 解凍専用モードだけど解凍不可！！
					msgBox( tmp.loadRsrc(IDS_M_ERROR) );
					return;
				}
			}
		}
	}

	if( whattodo == melt )
	{
		//-- 解凍設定は既にm_cnfMan.init()でロードされている…

		if( destdir.len()==0 )
		{
			//-- 解凍先ディレクトリ取得
			if( m_cnfMan.mdirsm() )
				if( is_writable_dir(m_arcMan.get_basepath()) )
					destdir = m_arcMan.get_basepath();
			if( destdir.len()==0 )
				destdir = m_cnfMan.mdir();
		}

		//-- 解凍
		if( ctrl_mlt )	m_arcMan.do_listing( destdir );
		else {
			ProcessNumLimitZone zone( mycnf().multiboot_limit(), "LimitterForNoahAtKmonosNet" );
			m_arcMan.do_melting( destdir );
		}
	}
	else
	{
		//-- Load-INI( 圧縮設定 )
		m_cnfMan.load( Compress );

		if( destdir.len()==0 )
		{
			//-- 圧縮先ディレクトリ取得
			if( m_cnfMan.cdirsm() )
				if( is_writable_dir(m_arcMan.get_basepath()) )
					destdir = m_arcMan.get_basepath();
			if( destdir.len()==0 )
				destdir = m_cnfMan.cdir();
		}
		if( !cmptype ) cmptype = m_cnfMan.cext();
		else if( !method ) method = "";
		if( !method  ) method  = m_cnfMan.cmhd();

		//-- 圧縮用ルーチンを割り当て
		if( !m_arcMan.map_compressor( cmptype, method, ctrl_cmp ) )
		{
			//-- 圧縮不能な形式！！
			msgBox( tmp.loadRsrc(IDS_C_ERROR) );
			return;
		}

		//-- 圧縮
		ProcessNumLimitZone zone( mycnf().multiboot_limit(), "LimitterForNoahAtKmonosNet" );
		m_arcMan.do_compressing( destdir, alt );
	}
}

//----------------------------------------------//
//----------------- その他雑用 -----------------//
//----------------------------------------------//

// from= 0:normal 1:melt 2:compress
void CNoahApp::open_folder( const kiPath& path, int from )
{
	if( from==1 || from==2 ) //-- Shellに更新通知
		::SHChangeNotify( SHCNE_UPDATEDIR, SHCNF_PATH, (const void*)(const char*)path, NULL );

	//-- デスクトップだったら開かない
	kiPath dir(path), tmp(kiPath::Dsk,false);
	dir.beBackSlash(false), dir.beShortPath(), tmp.beShortPath();

	if( !tmp.isSame( dir ) )
	{
		//-- Load-INI( フォルダ開き設定 )
		m_cnfMan.load( OpenDir );
		if( (from==1 && !m_cnfMan.modir())
		 || (from==2 && !m_cnfMan.codir()) )
			return;
		
		char cmdline[1000];
		wsprintf( cmdline, m_cnfMan.openby(), (const char*)dir );
		::WinExec( cmdline, SW_SHOWDEFAULT );
	}
}

// 全システム中で一意なテンポラリフォルダを作って返す
void CNoahApp::get_tempdir( kiPath& tmp )
{
	char buf[MAX_PATH];

	if( m_tmpDir.len()==0 )
	{
		::GetTempFileName( kiPath( kiPath::Tmp ), "noa", 0, buf );
		::DeleteFile( buf );
		m_tmpDir = buf;
		m_tmpDir.beBackSlash( true );
		m_tmpDir.mkdir();
		m_tmpID = ::GetCurrentProcessId();
	}

	::GetTempFileName( m_tmpDir, "noa", m_tmpID++, buf );
	::DeleteFile( buf );
	tmp = buf;
	tmp.beBackSlash( true );
	tmp.mkdir();
}
