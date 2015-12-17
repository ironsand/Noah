//--- K.I.LIB ---
// kl_wnd.cpp : window information manager

#include "stdafx.h"
#include "kilib.h"


//-------- Windowを作成時に HWND に kiWindow* をセットするための処理 -------//


kiWindow* kiWindow::st_pCurInit = NULL;
    HHOOK kiWindow::st_hHook    = NULL;

void kiWindow::init()
{
	// CreateWindow 用フック設置
	st_hHook = ::SetWindowsHookEx( WH_CBT, &CBTProc, NULL, ::GetCurrentThreadId() );
}

void kiWindow::finish()
{
	// CreateWindow 用フック解除
	::UnhookWindowsHookEx( st_hHook );
}

LRESULT CALLBACK kiWindow::CBTProc( int code, WPARAM wp, LPARAM lp )
{
	if( code == HCBT_CREATEWND )
	{
		if( st_pCurInit )
		{
			// k.i.lib のウインドウが CreateWindow された場合
			st_pCurInit->setHwnd( (HWND)wp );
			::SetWindowLong( (HWND)wp, GWL_USERDATA, (LONG)st_pCurInit );
			st_pCurInit = NULL;
		}
		else
			::SetWindowLong( (HWND)wp, GWL_USERDATA, 0 );
	}

	return ::CallNextHookEx( st_hHook, code, wp, lp );
}

void kiWindow::detachHwnd()
{
	::SetWindowLong( hwnd(), GWL_USERDATA, 0 );
	if( this == app()->mainwnd() )
		app()->setMainWnd( NULL );
	setHwnd( NULL );
}


//------------ Window にまつわるエトセトラな処理 (static) ---------------//


bool kiWindow::loopbreaker = false;

void kiWindow::msg()
{
	for( MSG msg; ::PeekMessage( &msg,NULL,0,0,PM_REMOVE ); )
		::TranslateMessage( &msg ), ::DispatchMessage( &msg );
}

void kiWindow::msgLoop( msglooptype type )
{
	kiWindow* wnd;
	MSG msg;
	while( !loopbreaker &&
		  type==GET ?  ::GetMessage( &msg,NULL,0,0 )
					: ::PeekMessage( &msg,NULL,0,0,PM_REMOVE ) )
	{
		if( wnd = app()->mainwnd() )
		{
			if( wnd->m_hAccel )
				if( ::TranslateAccelerator( wnd->hwnd(), wnd->m_hAccel, &msg ) )
					continue;
			if( msg.message!=WM_CHAR && wnd->isDlgMsg( &msg ) )
				continue;
		}
		::TranslateMessage( &msg ), ::DispatchMessage( &msg );
	}
	loopbreaker = false;
}

void kiWindow::setFront( HWND wnd )
{
	const OSVERSIONINFO& v = app()->osver();

	// Win2000 以上 or Win98 以上
	if( ( v.dwPlatformId==VER_PLATFORM_WIN32_NT && v.dwMajorVersion>=5 )
	 || ( v.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS &&
							v.dwMajorVersion*100+v.dwMinorVersion>=410 ) )
	{
		DWORD pid;
		DWORD th1 = ::GetWindowThreadProcessId( ::GetForegroundWindow(), &pid );
		DWORD th2 = ::GetCurrentThreadId();
		::AttachThreadInput( th2, th1, TRUE );
		::SetForegroundWindow( wnd );
		::AttachThreadInput( th2, th1, FALSE );
		::BringWindowToTop( wnd );
	}
	else  // 古いWin
		::SetForegroundWindow( wnd );

	// Special Thanks To kazubon !! ( the author of TClock )
}

void kiWindow::setCenter( HWND wnd, HWND rel )
{
	RECT rc,pr;
	::GetWindowRect( wnd, &rc );

	if( rel )
		::GetWindowRect( rel, &pr );
	else
		::SystemParametersInfo( SPI_GETWORKAREA, 0, &pr, 0 );

	::SetWindowPos( wnd, 0,
		pr.left + ( (pr.right-pr.left)-(rc.right-rc.left) )/2,
		pr.top  + ( (pr.bottom-pr.top)-(rc.bottom-rc.top) )/2,
		0, 0, SWP_NOSIZE|SWP_NOZORDER );
}


//------------------ Windowベースクラスとしての処理 ----------------------//


kiWindow::kiWindow()
{
	m_hWnd = NULL;
	m_hAccel = NULL;
	app()->shellInit();
}

kiWindow::~kiWindow()
{
	if( m_hWnd && ::IsWindow( m_hWnd ) )
	{
		::SetWindowLong( m_hWnd, GWL_USERDATA, 0 );
		::DestroyWindow( m_hWnd );
	}
}

void kiWindow::loadAccel( UINT id )
{
	m_hAccel = ::LoadAccelerators( app()->inst(), MAKEINTRESOURCE(id) );
}


//---------------- スタンドアロンのWindowの処理 ---------------------//

// …未完成…

//---------------------------- Dialog -----------------------------//


kiDialog::kiDialog( UINT id )
{
	m_Rsrc = id;
}

void kiDialog::doModal( HWND parent )
{
	setState( true );
	preCreate( this );

	::DialogBoxParam( app()->inst(), MAKEINTRESOURCE(m_Rsrc),
						parent, commonDlg, (LPARAM)this );
}

void kiDialog::createModeless( HWND parent )
{
	setState( false );
	preCreate( this );

	::CreateDialogParam( app()->inst(), MAKEINTRESOURCE(m_Rsrc),
						parent, commonDlg, (LPARAM)this );

	::ShowWindow( hwnd(), SW_SHOW );
	::UpdateWindow( hwnd() );
}

void kiDialog::end( UINT endcode )
{
	setEndCode( endcode );

	if( isModal() )
		::EndDialog( hwnd(), getEndCode() );
	else
		::DestroyWindow( hwnd() );
}

BOOL kiDialog::commonDlg( HWND dlg, UINT msg, WPARAM wp, LPARAM lp )
{
	// kiDialog インターフェイスへのポインタを取得
	kiDialog* ptr = (kiDialog*)::GetWindowLong( dlg, GWL_USERDATA );
	if( !ptr ) return FALSE;

	// WM_INITDIALOG なら onInit を呼ぶ
	if( msg == WM_INITDIALOG )
		return ptr->onInit();

	// OK / Cancel 処理
	else if( msg == WM_COMMAND )
	{
		switch( LOWORD(wp) )
		{
		case IDOK:
			if( ptr->onOK() )
				ptr->end( IDOK );
			return TRUE;
		case IDCANCEL:
			if( ptr->onCancel() )
				ptr->end( IDCANCEL );
			return TRUE;
		}
	}

	// 普通のメッセージ
	BOOL ans = ptr->proc( msg, wp, lp );

	// WM_DESTORY ならウインドウハンドル切り離し
	if( msg == WM_DESTROY )
		ptr->detachHwnd();

	return ans;
}


//------------------------ PropertySheet -------------------------//


kiPropSheet* kiPropSheet::st_CurInitPS = NULL;

kiPropSheet::kiPropSheet() : kiDialog( 0 )
{
	ki_memzero( &m_Header, sizeof(m_Header) );
	m_Header.dwSize      = sizeof(m_Header);
	m_Header.dwFlags     |=PSH_USECALLBACK | PSH_PROPSHEETPAGE;
	m_Header.pfnCallback = main_initProc;
	m_Header.hInstance   = app()->inst();
	m_Header.nStartPage  = 0;
}

void kiPropSheet::begin()
{
	int l = m_Pages.len();
	PROPSHEETPAGE* ppsp = new PROPSHEETPAGE[ l ];
	ki_memzero( ppsp, sizeof(PROPSHEETPAGE)*l );

	for( int i=0; i<l; i++ )
	{
		ppsp[i].dwSize      = sizeof( PROPSHEETPAGE );
		ppsp[i].hInstance   = app()->inst();
		ppsp[i].pfnCallback = page_initProc;
		ppsp[i].pfnDlgProc  = page_cmmnProc;
		ppsp[i].dwFlags     = PSP_USECALLBACK | PSP_HASHELP;
		m_Pages[i]->setInfo( ppsp+i );
	}

	m_Header.ppsp   = ppsp;
	m_Header.nPages = l;

	st_CurInitPS = this;
	PropertySheet( &m_Header );
	delete [] ppsp;
}

void kiPropSheet::doModal( HWND parent )
{
	m_Header.dwFlags &= (~PSH_MODELESS);
	setState( true );
	begin();
}

void kiPropSheet::createModeless( HWND parent )
{
	m_Header.dwFlags |= PSH_MODELESS;
	setState( false );
	begin();
}

void kiPropSheet::end( UINT endcode )
{
	// 終了コードセット
	setEndCode( endcode );

	// サブクラス化解除
	::SetWindowLong( hwnd(), GWL_WNDPROC, (LONG)m_DefProc );

	// 終了
	if( isModal() ) // サブクラス化解除してるので、再度 end が呼ばれることはないはず。
		::PostMessage( hwnd(), WM_COMMAND, IDCANCEL, 0 );
	else
		::DestroyWindow( hwnd() );

	// WM_DESTROY時相当の動作
	detachHwnd();
}

LRESULT CALLBACK kiPropSheet::main_cmmnProc( HWND dlg, UINT msg, WPARAM wp, LPARAM lp )
{
	kiPropSheet* ptr = (kiPropSheet*)::GetWindowLong( dlg, GWL_USERDATA );
	if( !ptr )
		return 0;

	// まずデフォルトの処理
	LRESULT result = ::CallWindowProc( ptr->m_DefProc, dlg, msg, wp, lp );

	// ×ボタンはキャンセル扱い
	if( msg==WM_SYSCOMMAND && wp==SC_CLOSE )
		::PostMessage( dlg, WM_COMMAND, IDCANCEL, 0 );

	// コマンド処理
	else if( msg==WM_COMMAND )
	{
		switch( LOWORD(wp) )
		{
		case IDOK:
			if( ptr->onOK() )
				ptr->end( IDOK );
			return TRUE;
		case IDCANCEL:
			if( ptr->onCancel() )
				ptr->end( IDCANCEL );
			return TRUE;
		case IDAPPLY:
			ptr->onApply();
			break;
		case ID_KIPS_HELP:
			ptr->onHelp();
			break;
		default:
			ptr->onCommand( LOWORD(wp) );
			break;
		}
	}

	// ドラッグ＆ドロップ
	else if( msg==WM_DROPFILES )
		ptr->onDrop( (HDROP)wp );

	return result;
}

struct DLGTEMPLATEEX
{
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;
};

int CALLBACK kiPropSheet::main_initProc( HWND dlg, UINT msg, LPARAM lp )
{
	if( msg == PSCB_PRECREATE )
	{
		// スタイルを指すDWORDのアドレスを取得
		DWORD* pst = ( 0xffff==((DLGTEMPLATEEX*)lp)->signature ) ? 
						&(((DLGTEMPLATEEX*)lp)->style) : &(((DLGTEMPLATE*)lp)->style);
		// ヘルプボタンを消して最小化ボタンを付ける
		(*pst) &= ~DS_CONTEXTHELP;
		(*pst) |=  WS_MINIMIZEBOX;

		preCreate( st_CurInitPS );
    }
	else if( msg == PSCB_INITIALIZED )
	{
		// 何故か出来てしまう余計なメニューを削除
		HMENU sysm = ::GetSystemMenu( dlg, FALSE );
		::DeleteMenu( sysm, SC_SIZE, MF_BYCOMMAND );
		::DeleteMenu( sysm, SC_MAXIMIZE, MF_BYCOMMAND );

		// 起動時はウインドウを必ず前面へ
		setFront( dlg );

		//サブクラス化する
		st_CurInitPS->m_DefProc = (WNDPROC)::SetWindowLong( dlg, GWL_WNDPROC, (LONG)main_cmmnProc );
		st_CurInitPS->onInit();
	}
	return 0;
}

BOOL kiPropSheet::page_cmmnProc( HWND dlg, UINT msg, WPARAM wp, LPARAM lp )
{
	kiPropSheetPage* ptr = (kiPropSheetPage*)::GetWindowLong( dlg, GWL_USERDATA );
	if( !ptr )
		return FALSE;

	// ここで、共通処理
	switch( msg )
	{
	case WM_INITDIALOG:
		return ptr->onInit();

	case WM_NOTIFY:
		switch( ((NMHDR*)lp)->code )
		{
		case PSN_APPLY:
			ptr->onOK();
			return TRUE;
		}
		break;

	case WM_COMMAND:
		if( lp )
			switch( HIWORD(wp) )
			{
			case BN_CLICKED:
			if((HWND)lp==::GetFocus())
			case EN_CHANGE:
			case CBN_SELCHANGE:
				PropSheet_Changed( ptr->parent()->hwnd(), dlg );
			}
		break;

	case WM_DESTROY:
		BOOL ans=ptr->proc( msg, wp, lp );
		ptr->detachHwnd();
		return ans;
	}

	return ptr->proc( msg, wp, lp );
}

UINT CALLBACK kiPropSheet::page_initProc( HWND dlg, UINT msg, LPPROPSHEETPAGE ppsp )
{
	if( msg == PSPCB_CREATE )
		preCreate( (kiWindow*)(ppsp->lParam) );
	return TRUE;
}

void kiPropSheetPage::setInfo( PROPSHEETPAGE* p )
{
	p->pszTemplate = MAKEINTRESOURCE( getRsrcID() );
	p->lParam      = (LPARAM)this;

	if( m_hIcon )
	{
		p->dwFlags|= PSP_USEHICON;
		p->hIcon = m_hIcon;
	}
}

