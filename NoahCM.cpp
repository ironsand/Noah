// NoahCM.cpp
//-- CNoahConfigManager -- save / load / modify the setting of 'Noah' --

#include "stdafx.h"
#include "resource.h"
#include "NoahApp.h"
#include "NoahCM.h"

//----------------------------------------------//
//---------- INIファイル名の設定など -----------//
//----------------------------------------------//

void CNoahConfigManager::init()
{
	//-- caldix.iniがもしあればDLLインストール先を読み込み。環境変数に追加
	{
		kiIniFile caldini;
		caldini.setFileName( "caldix.ini" );
		caldini.setSection(  "conf" );
		kiStr dll_to = caldini.getStr( "dll", "" );
		if( dll_to.len() != 0 )
		{
			int siz = ::GetEnvironmentVariable( "PATH", NULL, 0 );
			char* buf = new char[siz+1];
			::GetEnvironmentVariable( "PATH", buf, siz+1 );
			::SetEnvironmentVariable( "PATH", (kiStr(buf)+=";")+=dll_to );
			delete [] buf;
		}
	}

	//-- 設定読み込み済みフラグをクリア
	m_Loaded = 0;

	//-- iniファイル名設定
	char usr[256];
	DWORD siz=sizeof(usr);
	if( !::GetUserName( usr, &siz ) )
		ki_strcpy( usr, "Default" );
	m_Ini.setFileName( "Noah.ini" );
	m_Ini.setSection( usr );

	//-- 解凍用設定は全部ロードしておく
	load( Melt );
}

//----------------------------------------------//
//------------ 設定の load & save --------------//
//----------------------------------------------//

typedef bool (WINAPI * XT_IA)();
typedef void (WINAPI * XT_LS)(bool*,bool*);
typedef void (WINAPI * XT_SS)(bool,bool);
typedef void (WINAPI * XT_AS)(bool*);
typedef void (WINAPI * XT_LSEX)(const char*,bool*);
typedef void (WINAPI * XT_SSEX)(const char*,bool);

void CNoahConfigManager::load( loading_flag what )
{
	if( (what & Mode) && !(m_Loaded & Mode) ) //----------- モード
	{
		m_Mode     = m_Ini.getInt( "Mode", 2 ) & 3;
		m_MiniBoot = m_Ini.getBool( "MiniBoot", false );
		m_OneExt   = m_Ini.getBool( "OneExt", false );
		m_ZeroExt  = m_Ini.getBool( "NoExt", false );
		m_MbLim    = max( 1, m_Ini.getInt( "MultiBootLimit", 4 ) );
	}
	if( (what & Melt) && !(m_Loaded & Melt) ) //----------- 解凍
	{
		const char* x = m_Ini.getStr( "MDir", kiPath( kiPath::Dsk ) );
		m_MDirSm = (*x=='@');
		m_MDir   = (*x=='@') ? x+1 : x;
		const int m = m_Ini.getInt( "MkDir", 2 );
		m_MNoNum = ( m>=16 );
		m_MkDir  = ( m&3 );
		m_Kill   = m_Ini.getStr( "Kill", "" );
	}
	if( (what & Compress) && !(m_Loaded & Compress) ) //--- 圧縮
	{
		const char* x = m_Ini.getStr( "CDir", kiPath( kiPath::Dsk ) );
		m_CDirSm = (*x=='@');
		m_CDir   = (*x=='@') ? x+1 : x;
		m_CExt = m_Ini.getStr( "CExt", "zip" );
		m_CMhd = m_Ini.getStr( "CMhd", "7-zip" );
	}
	if( (what & Shell) && !(m_Loaded & Shell) )	//---------- ショートカット
												//---------- 関連づけ
												//---------- シェルエクステンション
	{
		m_OldVer = m_Ini.getBool( "OldAbout", false );

		kiPath SndLink(kiPath::Snd),DskLink(kiPath::Dsk);
		SndLink += "Noah.lnk", DskLink += "Noah.lnk";
		m_SCSendTo = kiSUtil::exist(SndLink);
		m_SCDesktop= kiSUtil::exist(DskLink);

		m_bShlOK = NOSHL;
		m_hNoahXtDLL = kiSUtil::loadLibrary( "NoahXt" );
		if( m_hNoahXtDLL )
		{
			XT_IA Init = (XT_IA)getProc( "Init" );
			m_bShlOK = ( Init() ? SHLOK : NOADMIN );
			XT_LS LoadSE = (XT_LS)getProc( "LoadSE" );
			XT_AS LoadAssoc = (XT_AS)getProc( "LoadAS" );
			XT_LSEX LoadASEx = (XT_LSEX)getProc( "LoadASEx" );
			LoadSE( &m_SECmp, &m_SEExt );
			LoadAssoc( m_bAsso );
			LoadASEx( "7z\0", &m_bAsso[12] );
		}
	}
	if( (what & OpenDir) && !(m_Loaded & OpenDir) ) //------ フォルダオープン
	{
		m_MODir = m_Ini.getBool( "MODir", true );
		m_CODir = m_Ini.getBool( "CODir", true );
		m_OpenBy = m_Ini.getStr( "OpenBy", kiPath(kiPath::Win)+"explorer.exe \"%s\"" );
	}

	m_Loaded |= what;
}

void CNoahConfigManager::save()
{
	kiStr tmp;

	//-- モード
	m_Ini.putInt( "Mode", m_Mode );
	//-- 解凍
	tmp = m_MDirSm ? "@" : "", tmp+= m_MDir;
	m_Ini.putStr( "MDir", tmp );
	m_Ini.putInt( "MkDir", m_MkDir+(m_MNoNum?16:0) );
	//-- 圧縮
	tmp = m_CDirSm ? "@" : "", tmp+= m_CDir;
	m_Ini.putStr( "CDir", tmp );
	m_Ini.putStr( "CExt", m_CExt );
	m_Ini.putStr( "CMhd", m_CMhd );
	//-- ショートカット
	kiPath SndLink(kiPath::Snd); SndLink += "Noah.lnk";
	kiPath DskLink(kiPath::Dsk); DskLink += "Noah.lnk";
	if( m_SCSendTo )
	{
		if( !kiSUtil::exist(SndLink) )
			kiSUtil::createShortCut( kiPath(kiPath::Snd), "Noah" );
	}
	else
		::DeleteFile(SndLink);
	if( m_SCDesktop )
	{
		if( !kiSUtil::exist(DskLink) )
			kiSUtil::createShortCut( kiPath(kiPath::Dsk), "Noah" );
	}
	else
		::DeleteFile(DskLink);
	//-- 関連づけ・シェルエクステンション
	if( m_bShlOK )
	{
		XT_SS SaveSE = (XT_SS)getProc( "SaveSE" );
		XT_AS SaveAssoc = (XT_AS)getProc( "SaveAS" );
		XT_SSEX SaveASEx = (XT_SSEX)getProc( "SaveASEx" );
		SaveSE( m_SECmp, m_SEExt );
		SaveAssoc( m_bAsso );
		SaveASEx( "7z\0", m_bAsso[12] );
	}
	//-- フォルダオープン
	m_Ini.putBool("MODir", m_MODir );
	m_Ini.putBool("CODir", m_CODir );
}

void CNoahConfigManager::dialog()
{
	CNoahConfigDialog dlg;
	dlg.createModeless( NULL );

	app()->setMainWnd( &dlg );

	if( dlg.isAlive() )
		kiWindow::msgLoop();
}

FARPROC CNoahConfigManager::getProc( const char* name )
{
	return ::GetProcAddress( m_hNoahXtDLL, name );
}

//----------------------------------------------//
//--------------- ダイアログ関係 ---------------//
//----------------------------------------------//

///////// 初期化 /////////////

#define IDI_LZH 101
#define IDI_ZIP 102
#define IDI_CAB 103
#define IDI_RAR 104
#define IDI_TAR 105
#define IDI_YZ1 106
#define IDI_GCA 107
#define IDI_ARJ 108
#define IDI_BGA 109
#define IDI_ACE 110
#define IDI_OTH 111
#define IDI_JAK 112

#define icon_is(_x) { if( mycnf().m_hNoahXtDLL ) setIcon( ::LoadIcon( mycnf().m_hNoahXtDLL, MAKEINTRESOURCE(_x) ) ); }
	CNoahConfigDialog::CCmprPage::CCmprPage() : kiPropSheetPage( IDD_CMPCFG ) icon_is( IDI_ACE )
	CNoahConfigDialog::CMeltPage::CMeltPage() : kiPropSheetPage( IDD_MLTCFG ) icon_is( IDI_LZH )
	CNoahConfigDialog::CWinXPage::CWinXPage() : kiPropSheetPage( IDD_WINCFG ) icon_is( IDI_YZ1 )
	CNoahConfigDialog::CInfoPage::CInfoPage() : kiPropSheetPage( IDD_INFCFG ) icon_is( IDI_GCA )
#undef icon_is

CNoahConfigDialog::CNoahConfigDialog()
{
	//-- [icon] Noahのプロパティ
	m_Header.dwFlags |= PSH_PROPTITLE | PSH_USEICONID;
	m_Header.pszIcon = MAKEINTRESOURCE( IDI_MAIN );
	m_Header.pszCaption = "Noah";

	//-- アクセラレータをセット
	loadAccel( IDR_ACCEL );

	//-- ページをどかどかと追加
	m_Pages.add( new CCmprPage );
	m_Pages.add( new CMeltPage );
	m_Pages.add( new CWinXPage );
	m_Pages.add( new CInfoPage );
}

BOOL CNoahConfigDialog::onInit()
{
	//-- DnD ON, 前面へ
	::DragAcceptFiles( hwnd(), TRUE );
	setFront( hwnd() );
	return FALSE;
}

///////// 各種コマンド /////////////

void CNoahConfigDialog::onCommand( UINT id )
{
	//-- アクセラレータトラップ
	if( id == IDA_HELP )		onHelp();
	else if( id == IDA_MYDIR )	myapp().open_folder( kiPath( kiPath::Exe ) );
}

void CNoahConfigDialog::onHelp()
{
	kiPath exepos( kiPath::Exe );

	//-- exeと同じ箇所にあるmanual.htmを起動
	kiPath hlp(exepos); hlp+="manual.htm";
	if( kiSUtil::exist(hlp) )
		::ShellExecute( hwnd(), NULL, hlp, NULL, NULL, SW_MAXIMIZE );
	else
	{
		//-- 無ければreadme.txtを
		hlp=exepos; hlp+="readme.txt";
		if( kiSUtil::exist(hlp) )
			::ShellExecute( hwnd(), NULL, hlp, NULL, NULL, SW_SHOWDEFAULT );
	}
}

void CNoahConfigDialog::onDrop( HDROP hdrop )
{
	//-- ダイアログへのドラッグ＆ドロップ
	sendOK2All();

	//-- 途中で邪魔にならないように、消える
	::ShowWindow( hwnd(), SW_HIDE );

	char str[MAX_PATH];
	StrArray reallist;
	cCharArray dummy;

	unsigned long max = ::DragQueryFile( hdrop, 0xffffffff, NULL, 0 );
	for( unsigned int i=0; i!=max; i++ )
	{
		::DragQueryFile( hdrop, i, str, MAX_PATH );
		reallist.add( kiStr(str) );
	}
	for( i=0; i!=max; i++ )
		dummy.add( (const char*)reallist[i] );
	myapp().do_files( dummy, NULL );

	// 復帰
	::DragFinish( hdrop );
	::ShowWindow( hwnd(), SW_SHOW );
}

///////// 終了時処理など /////////////

void CNoahConfigDialog::shift_and_button()
{
	if( app()->keyPushed( VK_SHIFT ) )
	{
		app()->setMainWnd( NULL );
		myapp().do_cmdline();
	}
}

bool CNoahConfigDialog::onOK()
{
	onApply();
	::PostQuitMessage( 0 );
	return true;
}

void CNoahConfigDialog::onApply()
{
	mycnf().save();
	shift_and_button();
}

bool CNoahConfigDialog::onCancel()
{
	sendOK2All();
	::ShowWindow( hwnd(), SW_HIDE );
	shift_and_button();
	::PostQuitMessage( 0 );
	return true;
}

///////// 圧縮設定・解凍設定の共通部分 /////////////

static void dirinit( kiDialog* dlg, bool same, bool open, const char* dir )
{
	dlg->sendMsgToItem( same ? IDC_DDIR1 : IDC_DDIR2 , BM_SETCHECK, TRUE );
	if( open )
		dlg->sendMsgToItem( IDC_ODIR , BM_SETCHECK, TRUE );
	dlg->sendMsgToItem( IDC_DDIR , WM_SETTEXT , 0, (LPARAM)dir );
}

static void dirok( kiDialog* dlg, bool& same, bool& open, kiPath& dir )
{
	same = ( BST_CHECKED==dlg->sendMsgToItem( IDC_DDIR1, BM_GETCHECK ) );
	open = ( BST_CHECKED==dlg->sendMsgToItem( IDC_ODIR, BM_GETCHECK ) );
	static char str[MAX_PATH];
	dlg->sendMsgToItem( IDC_DDIR, WM_GETTEXT, MAX_PATH, (LPARAM)str );
	dir = str;
}

static bool dirdlg( kiDialog* dlg, UINT msg, WPARAM wp )
{
	if( msg==WM_COMMAND && LOWORD(wp)==IDC_REF )
	{
		kiSUtil::getFolderDlgOfEditBox(
			dlg->item(IDC_DDIR), dlg->hwnd(), kiStr().loadRsrc(IDS_CHOOSEDIR) );
		return true;
	}
	return false;
}

///////// 圧縮設定 /////////////

BOOL CNoahConfigDialog::CCmprPage::onInit()
{
	// 圧縮先フォルダ
	dirinit( this, mycnf().cdirsm(), mycnf().codir(), mycnf().cdir() );

	// 動作モード
	sendMsgToItem( IDC_MODE1 + mycnf().mode(), BM_SETCHECK, TRUE );

	// 圧縮形式
	correct( mycnf().cext(), true );
	int ind=sendMsgToItem( IDC_CMPMHD, CB_FINDSTRINGEXACT, -1, (LPARAM)(const char*)mycnf().cmhd() );
	if( ind!=CB_ERR )
		sendMsgToItem( IDC_CMPMHD, CB_SETCURSEL, ind );

	// ツールチップ
	m_tooltip = ::CreateWindowEx(
		0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd(), NULL, app()->inst(), NULL );
	SetUpToolTip();

	::SetFocus(hwnd());
	return TRUE;
}

bool CNoahConfigDialog::CCmprPage::onOK()
{
	// 圧縮先フォルダ
	dirok( this, mycnf().m_CDirSm, mycnf().m_CODir, mycnf().m_CDir );

	// 動作モード
	for( int i=0; i!=4; i++ )
		if( BST_CHECKED==sendMsgToItem( IDC_MODE1 + i, BM_GETCHECK ) )
			{ mycnf().m_Mode = i; break; }

	// 圧縮形式
	char str[200]="";
	sendMsgToItem( IDC_CMPEXT, CB_GETLBTEXT, sendMsgToItem( IDC_CMPEXT, CB_GETCURSEL ), (LPARAM)str );
	if( *str )
	{
		mycnf().m_CExt = str;
		sendMsgToItem( IDC_CMPMHD, CB_GETLBTEXT, sendMsgToItem( IDC_CMPMHD, CB_GETCURSEL ), (LPARAM)str );
		mycnf().m_CMhd = str;
	}

	onCancel(); // 終了処理
	return true;
}

bool CNoahConfigDialog::CCmprPage::onCancel()
{
	// 終了処理
	::DestroyWindow( m_tooltip );
	return true;
}

void CNoahConfigDialog::CCmprPage::SetUpToolTip()
{
	char ext[200]="";
	sendMsgToItem( IDC_CMPEXT, CB_GETLBTEXT, sendMsgToItem( IDC_CMPEXT, CB_GETCURSEL ), (LPARAM)ext );
	char mhd[200]="";
	sendMsgToItem( IDC_CMPMHD, CB_GETLBTEXT, sendMsgToItem( IDC_CMPMHD, CB_GETCURSEL ), (LPARAM)mhd );

	TOOLINFO ti = {sizeof(TOOLINFO)};
	ti.uFlags   = TTF_SUBCLASS;
	{
		ti.uId      = 0;
		ti.hwnd     = item(IDC_CMPEXT);
		::GetClientRect( item(IDC_CMPEXT), &ti.rect );
		SendMessage( m_tooltip, TTM_DELTOOL, 0, (LPARAM)&ti );
		if( *ext )
		{
			ti.lpszText = ext;
			SendMessage( m_tooltip, TTM_ADDTOOL, 0, (LPARAM)&ti );
		}
	}
	{
		ti.uId      = 1;
		ti.hwnd     = item(IDC_CMPMHD);
		::GetClientRect( item(IDC_CMPMHD), &ti.rect );
		SendMessage( m_tooltip, TTM_DELTOOL, 0, (LPARAM)&ti );
		if( *mhd )
		{
			ti.lpszText = mhd;
			SendMessage( m_tooltip, TTM_ADDTOOL, 0, (LPARAM)&ti );
		}
	}
}

BOOL CALLBACK CNoahConfigDialog::CCmprPage::proc( UINT msg, WPARAM wp, LPARAM lp )
{
	if( dirdlg( this, msg, wp ) )
		return TRUE;

	if( msg==WM_COMMAND && HIWORD(wp)==CBN_SELCHANGE && LOWORD(wp)==IDC_CMPEXT )
	{
		char str[200]="";
		sendMsgToItem( IDC_CMPEXT, CB_GETLBTEXT, sendMsgToItem( IDC_CMPEXT, CB_GETCURSEL ), (LPARAM)str );
		if( *str )
			correct( str, false );
		SetUpToolTip();
		return TRUE;
	}
	else if( msg==WM_COMMAND && HIWORD(wp)==CBN_SELCHANGE && LOWORD(wp)==IDC_CMPMHD )
	{
		SetUpToolTip();
	}
	return FALSE;
}

void CNoahConfigDialog::CCmprPage::correct( const char* ext, bool first )
{
	cCharArray extl;
	StrArray mhdl;
	int mhdef;
	myarc().get_cmpmethod( ext, mhdef, mhdl, first, &extl );

	if( first )
		for( unsigned int i=0; i!=extl.len(); i++ )
			sendMsgToItem( IDC_CMPEXT, CB_ADDSTRING, 0, (LPARAM)extl[i] );
	sendMsgToItem( IDC_CMPEXT, CB_SELECTSTRING, 0, (LPARAM)ext );

	sendMsgToItem( IDC_CMPMHD, CB_RESETCONTENT );
	for( unsigned int j=0; j!=mhdl.len(); j++ )
		sendMsgToItem( IDC_CMPMHD, CB_ADDSTRING, 0, (LPARAM)(const char*)mhdl[j] );
	sendMsgToItem( IDC_CMPMHD, CB_SETCURSEL, mhdef );
}


///////// 解凍設定 /////////////

BOOL CNoahConfigDialog::CMeltPage::onInit()
{
	// 解凍先フォルダ
	dirinit( this, mycnf().mdirsm(), mycnf().modir(), mycnf().mdir() );

	// フォルダ自動生成
	if( mycnf().mkdir()!=0 )
		sendMsgToItem( IDC_MKDIR ,BM_SETCHECK, TRUE );
	if( mycnf().mkdir()==1 )
		sendMsgToItem( IDC_MKDIR1,BM_SETCHECK, TRUE );
	if( mycnf().mkdir()==2 )
		sendMsgToItem( IDC_MKDIR2,BM_SETCHECK, TRUE );
	if( mycnf().mnonum() )
		sendMsgToItem( IDC_MKDIR3,BM_SETCHECK, TRUE );
	correct();

	return FALSE;
}

bool CNoahConfigDialog::CMeltPage::onOK()
{
	// 解凍先フォルダ
	dirok( this, mycnf().m_MDirSm, mycnf().m_MODir, mycnf().m_MDir );

	// フォルダ自動生成
	mycnf().m_MNoNum = ( BST_CHECKED==sendMsgToItem( IDC_MKDIR3, BM_GETCHECK ) );
	if( BST_CHECKED!=sendMsgToItem( IDC_MKDIR ,BM_GETCHECK ) )
		mycnf().m_MkDir = 0;
	else
	{
		if( BST_CHECKED==sendMsgToItem( IDC_MKDIR1 ,BM_GETCHECK ) )
			mycnf().m_MkDir = 1;
		else if( BST_CHECKED==sendMsgToItem( IDC_MKDIR2 ,BM_GETCHECK ) )
			mycnf().m_MkDir = 2;
		else
			mycnf().m_MkDir = 3;
	}
	return true;
}

BOOL CALLBACK CNoahConfigDialog::CMeltPage::proc( UINT msg, WPARAM wp, LPARAM lp )
{
	if( dirdlg( this, msg, wp ) )
		return TRUE;

	if( msg==WM_COMMAND )
		if( LOWORD(wp)==IDC_MKDIR || LOWORD(wp)==IDC_MKDIR1 )
		{
			correct();
			return TRUE;
		}

	return FALSE;
}

void CNoahConfigDialog::CMeltPage::correct()
{
	BOOL _mk = ( BST_CHECKED==sendMsgToItem( IDC_MKDIR ,BM_GETCHECK ) );
	BOOL _1f = ( BST_CHECKED==sendMsgToItem( IDC_MKDIR1,BM_GETCHECK ) );
	::EnableWindow( ::GetDlgItem(hwnd(),IDC_MKDIR1), _mk );
	::EnableWindow( ::GetDlgItem(hwnd(),IDC_MKDIR2), _mk && !_1f );
	::EnableWindow( ::GetDlgItem(hwnd(),IDC_MKDIR3), _mk );
	if( _1f )
		sendMsgToItem( IDC_MKDIR2, BM_SETCHECK, TRUE );
}

///////// Windows拡張設定 /////////////

BOOL CNoahConfigDialog::CWinXPage::onInit()
{
	if( !mycnf().m_bShlOK )
		for( UINT i=IDC_LZH; i<=IDC_MLT; i++ )
			::EnableWindow( item(i), FALSE );
	else
	{
		for( UINT i=IDC_LZH; i<=IDC_7Z; i++ )
			if( mycnf().m_bAsso[i-IDC_LZH] )
				sendMsgToItem( i, BM_SETCHECK, TRUE );
		if( mycnf().m_SECmp )
			sendMsgToItem( IDC_CMP ,BM_SETCHECK, TRUE );
		if( mycnf().m_SEExt )
			sendMsgToItem( IDC_MLT ,BM_SETCHECK, TRUE );
	}
	if( !mycnf().m_bShlOK || !myarc().b2e_enabled() )
	{
		::ShowWindow( item(IDC_ASS), SW_HIDE );
		::EnableWindow( item(IDC_ASS), FALSE );
	}
	if( mycnf().m_bShlOK!=1 )
		::ShowWindow( item(IDC_NOADMIN), SW_HIDE );
	if( mycnf().m_SCSendTo )
		sendMsgToItem( IDC_SND, BM_SETCHECK, TRUE );
	if( mycnf().m_SCDesktop )
		sendMsgToItem( IDC_DSK, BM_SETCHECK, TRUE );

	return FALSE;
}

bool CNoahConfigDialog::CWinXPage::onOK()
{
	mycnf().m_SCSendTo = ( BST_CHECKED==sendMsgToItem( IDC_SND ,BM_GETCHECK ) );
	mycnf().m_SCDesktop= ( BST_CHECKED==sendMsgToItem( IDC_DSK ,BM_GETCHECK ) );
	for( UINT i=IDC_LZH; i<=IDC_7Z; i++ )
		mycnf().m_bAsso[i-IDC_LZH] = ( BST_CHECKED==sendMsgToItem( i, BM_GETCHECK ) );
	mycnf().m_SECmp = ( BST_CHECKED==sendMsgToItem( IDC_CMP ,BM_GETCHECK ) );
	mycnf().m_SEExt = ( BST_CHECKED==sendMsgToItem( IDC_MLT ,BM_GETCHECK ) );

	return true;
}

BOOL CALLBACK CNoahConfigDialog::CWinXPage::proc( UINT msg, WPARAM wp, LPARAM lp )
{
	if( msg==WM_COMMAND )
		if( LOWORD(wp)==IDC_ASS )
		{
			CAssPage x( hwnd() );
			return TRUE;
		}
	return FALSE;
}

CNoahConfigDialog::CAssPage::CAssPage( HWND parent ) : kiDialog( IDD_ANYASS )
{
	doModal( parent );
}

BOOL CNoahConfigDialog::CAssPage::onInit()
{
	typedef void (WINAPI * XT_LAX)(const char*,bool*);
	XT_LAX LoadASEx = (XT_LAX)mycnf().getProc( "LoadASEx" );
	static const char* const ext_list[] =
		{ "lzh","zip","cab","rar","tar","yz1","gca","arj","gza","ace","cpt","jak","7z" };

	// b2e検索
	kiPath wild( kiPath::Exe );
	wild += "b2e\\*.b2e";
	kiFindFile find;
	find.begin( wild );

	char* first_dot;
	bool state;
	HWND lst[] = { item(IDC_NASSOC), item(IDC_ASSOC) };

	for( WIN32_FIND_DATA fd; find.next(&fd); )
	{
		// # 付きは圧縮専用
		if( fd.cFileName[0] == '#' )
			continue;

		// 拡張子を切り出し
		::CharLower( fd.cFileName );
		first_dot = const_cast<char*>(kiPath::ext_all(fd.cFileName)-1);
		*first_dot = '\0';

		// 基本形式ならここではねる
		for( int i=0; i<sizeof(ext_list)/sizeof(const char*); i++ )
			if( 0==ki_strcmp( ext_list[i], fd.cFileName ) )
				break;
		if( i != sizeof(ext_list)/sizeof(const char*) )
			continue;

		// 関連づけ済みかどうかチェック
		LoadASEx( fd.cFileName, &state );

		// 適切な方のリストへ追加
		*first_dot = '.';
		*const_cast<char*>(kiPath::ext(fd.cFileName)-1) = '\0';
		::SendMessage( lst[state?1:0], LB_SETITEMDATA,
					   ::SendMessage( lst[state?1:0], LB_ADDSTRING, 0, (LPARAM)fd.cFileName ),
					   state?1:0 );
	}

	return FALSE;
}

BOOL CALLBACK CNoahConfigDialog::CAssPage::proc( UINT msg, WPARAM wp, LPARAM lp )
{
	if( msg==WM_COMMAND )
	{
		char str[300];
		DWORD dat;
		HWND from=item(IDC_NASSOC), to=item(IDC_ASSOC);

		switch( LOWORD(wp) )
		{
		case IDC_DEL:
			from=item(IDC_ASSOC), to=item(IDC_NASSOC);
		case IDC_ADD:{
			int end = ::SendMessage( from, LB_GETCOUNT, 0, 0 );
			for( int i=0; i<end; i++ )
				if( ::SendMessage( from, LB_GETSEL, i, 0 ) )
				{
					// 取得
					::SendMessage( from, LB_GETTEXT, i, (LPARAM)str );
					dat = ::SendMessage( from, LB_GETITEMDATA, i, 0 );
					//　コピー
					::SendMessage( to, LB_SETITEMDATA,
								   ::SendMessage( to, LB_ADDSTRING, 0, (LPARAM)str ),
								   dat );
					// 削除
					::SendMessage( from, LB_DELETESTRING, i, 0 );
					i--, end--;
				}

			}return TRUE;
		}
	}
	return FALSE;
}

static void crack_str( char* p )
{
	for( ; *p; p=kiStr::next(p) )
		if( *p=='.' )
			*p++ = '\0';
	*++p = '\0';
}

bool CNoahConfigDialog::CAssPage::onOK()
{
	typedef void (WINAPI * XT_SAX)(const char*,bool);
	XT_SAX SaveASEx = (XT_SAX)mycnf().getProc( "SaveASEx" );

	char str[301];
	int i, nc = sendMsgToItem( IDC_NASSOC, LB_GETCOUNT ),
	       ac = sendMsgToItem(  IDC_ASSOC, LB_GETCOUNT );

	// 解除
	for( i=0; i<nc; i++ )
		if( sendMsgToItem( IDC_NASSOC, LB_GETITEMDATA, i ) )
		{
			sendMsgToItem( IDC_NASSOC, LB_GETTEXT, i, (LPARAM)str );
			crack_str( str );
			SaveASEx( str, false );
		}
	// 設定
	for( i=0; i<ac; i++ )
		if( !sendMsgToItem( IDC_ASSOC, LB_GETITEMDATA, i ) )
		{
			sendMsgToItem(  IDC_ASSOC, LB_GETTEXT, i, (LPARAM)str );
			crack_str( str );
			SaveASEx( str, true );
		}

	return true;
}

///////// その他設定 /////////////

BOOL CNoahConfigDialog::CInfoPage::onInit()
{
	kiStr ver;
	myarc().get_version( ver );
	sendMsgToItem( IDC_VERSION, WM_SETTEXT, 0, (LPARAM)(const char*)ver );
	return FALSE;
}
