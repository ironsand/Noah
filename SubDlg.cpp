
#include "stdafx.h"
#include "NoahApp.h"
#include "SubDlg.h"

int CArcViewDlg::st_nLife;

BOOL CArcViewDlg::onInit()
{
	char cstr[100];
	kiStr str;
	kiPath path;
	SHFILEINFO sfi,lfi;
	HIMAGELIST hImS,hImL;
	kiListView ctrl( this, IDC_FILELIST );
	__int64 filesize_sum = 0;

	//-- ダイアログ一個生成の印
	hello();
	m_bSmallFirst[0] = m_bSmallFirst[1] = m_bSmallFirst[2] =
	m_bSmallFirst[3] = m_bSmallFirst[4] = m_bSmallFirst[5] = true;

	//-- 真ん中に＆前に
	setCenter( hwnd(), app()->mainhwnd() );
	setFront( hwnd() );

	//-- アイコン
	path = m_fname.basedir, path += m_fname.sname;
	hImS = (HIMAGELIST)::SHGetFileInfo( path, 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SMALLICON );
	hImL = (HIMAGELIST)::SHGetFileInfo( path, 0, &lfi, sizeof(lfi), SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_LARGEICON );
	sendMsg( WM_SETICON, ICON_BIG,   (LPARAM)lfi.hIcon );
	sendMsg( WM_SETICON, ICON_SMALL, (LPARAM)sfi.hIcon );

	//-- タイトル
	sendMsg( WM_SETTEXT, 0, (LPARAM)kiPath(m_fname.lname).name() );

	//-- 解凍先
	sendMsgToItem( IDC_DDIR, WM_SETTEXT, 0, (LPARAM)(const char*)m_ddir );

	//-- リスト
	if( !m_pArc->list( m_fname, m_files ) || m_files.len()==0 )
	{
		m_bAble = false;
		ctrl.insertColumn( 0, "", 510 );
		ctrl.insertItem( 0, str.loadRsrc(IDS_NOLIST) );
	}
	else
	{
		m_bAble = ( 0 != (m_pArc->ability() & aMeltEach) );

		ctrl.setImageList( hImL, hImS );
		ctrl.insertColumn( 0, str.loadRsrc(IDS_FNAME),   110 );
		ctrl.insertColumn( 1, str.loadRsrc(IDS_SIZE),    70,  LVCFMT_RIGHT );
		ctrl.insertColumn( 2, str.loadRsrc(IDS_DATETIME),100, LVCFMT_RIGHT );
		ctrl.insertColumn( 3, str.loadRsrc(IDS_RATIO),   55,  LVCFMT_RIGHT );
		ctrl.insertColumn( 4, str.loadRsrc(IDS_METHOD),  50,  LVCFMT_RIGHT );
		ctrl.insertColumn( 5, str.loadRsrc(IDS_PATH),    130 );

		FILETIME ftm;
		SYSTEMTIME stm;

		//-- アイテム
		for( unsigned int i=0,k=0; i!=m_files.len(); i++ )
			if( m_files[i].isfile )
			{
#define			usiz (m_files[i].inf.dwOriginalSize)
#define			csiz (m_files[i].inf.dwCompressedSize)
#define			method (m_files[i].inf.szMode)
#define			date (m_files[i].inf.wDate)
#define			time (m_files[i].inf.wTime)
				path = m_files[i].inf.szFileName;

				// ファイル名
				ctrl.insertItem( k, path.name(),
					(LPARAM)(&m_files[i]), kiSUtil::getSysIcon(path.ext()) );

				// サイズ
				if( usiz == 0xffffffff )
					ctrl.setSubItem( k, 1, "????" );
				else
					ctrl.setSubItem( k, 1, str.setInt( usiz,true ) );

				// 時間
				if( ::DosDateTimeToFileTime( date, time, &ftm )
				 && ::FileTimeToSystemTime( &ftm, &stm ) )
				{
					*cstr=0;
					::GetDateFormat( LOCALE_USER_DEFAULT, 0, &stm,
									 "yy/MM/dd", cstr, sizeof(cstr) );
					str=cstr;
					::GetTimeFormat( LOCALE_USER_DEFAULT, 0, &stm,
									 " HH:mm", cstr, sizeof(cstr) );
					str+=cstr;
					ctrl.setSubItem( k, 2, str );
				}

				// 圧縮率
				filesize_sum += usiz;
				if( usiz==0 )		ctrl.setSubItem( k, 3, "100%" );
				else if( csiz==0 )	ctrl.setSubItem( k, 3, "????" );
				else				ctrl.setSubItem( k, 3, str.setInt( (int)(((__int64)csiz)*100/usiz) )+='%' );

				// メソッド
				ctrl.setSubItem( k, 4, method );

				// パス
				path.beDirOnly();
				ctrl.setSubItem( k, 5, path );

				k++;

#undef			usiz
#undef			csiz
#undef			method
#undef			date
#undef			time
			}

		//-- ドラッグ＆ドロップフォーマット登録
		FORMATETC fmt;
		fmt.cfFormat = CF_HDROP;
		fmt.ptd      = NULL;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex   = -1;
		fmt.tymed    = TYMED_HGLOBAL;
		addFormat( fmt );
	}

	//-- 情報 --
	char tmp[255];
	kiStr full_filename = m_fname.basedir + m_fname.lname;
	__int64 filesize_arc = kiFile::getSize64(full_filename);
	if( filesize_sum==0 ) filesize_sum = 1;
	wsprintf( tmp, kiStr().loadRsrc(IDS_ARCVIEW_MSG),
		m_files.len(),
		(int)(filesize_arc*100 / filesize_sum),
		(const char*)m_pArc->arctype_name(full_filename)
	);
	sendMsgToItem( IDC_STATUSBAR, WM_SETTEXT, 0, (long)tmp );

	if( !m_bAble )
	{
		static const UINT items[] = { IDC_SELECTINV,IDC_REF,IDC_MELTEACH,IDC_SHOW,IDC_DDIR };
		for( int i=0; i!=sizeof(items)/sizeof(UINT); i++ )
			::EnableWindow( item(items[i]), FALSE );
	}

	return FALSE;
}

bool CArcViewDlg::onOK()
{
	setdir();
	m_pArc->melt( m_fname, m_ddir );
	myapp().open_folder( m_ddir, 1 );
	kiSUtil::switchCurDirToExeDir(); // 念のため
	return onCancel();
}

bool CArcViewDlg::onCancel()
{
	::SetCurrentDirectory( m_fname.basedir );
	m_tdir.remove();
	if( kiSUtil::exist(m_tdir) )
	{
		kiStr tmp(600);
		if( IDNO==app()->msgBox( tmp.loadRsrc(IDS_EXECUTING), NULL, MB_YESNO|MB_DEFBUTTON2 ) )
			return false;
	}

	kiListView(this,IDC_FILELIST).setImageList( NULL, NULL );
	byebye();
	return true;
}

bool CArcViewDlg::giveData( const FORMATETC& fmt, STGMEDIUM* stg, bool firstcall )
{
	if( firstcall )
		if( 0x8000<=m_pArc->melt( m_fname, m_tdir, &m_files ) )
			return false;

	unsigned int i;
	BOOL fWide = (app()->osver().dwPlatformId==VER_PLATFORM_WIN32_NT);
	kiArray<kiPath> lst;
	kiPath tmp;
	int flen = 0;
	wchar_t wbuf[600];

	for( i=0; i!=m_files.len(); i++ )
		if( m_files[i].selected )
		{
			tmp = m_tdir;
			tmp += m_files[i].inf.szFileName;

			lst.add( tmp );
			if( fWide )
				flen += (::MultiByteToWideChar( CP_ACP, 0, tmp, -1, wbuf, 600 )+1)*2;
			else
				flen += (tmp.len()+1);
		}

	HDROP hDrop = (HDROP)::GlobalAlloc( GHND, sizeof(DROPFILES)+flen+1 );

	DROPFILES* dr = (DROPFILES*)::GlobalLock( hDrop );
	dr->pFiles = sizeof(DROPFILES);
	dr->pt.x   = dr->pt.y = 0;
	dr->fNC    = FALSE;
	dr->fWide  = fWide;

	char* buf = (char*)(&dr[1]);
	for( i=0; i!=lst.len(); i++ )
	{
		if( fWide )
		{
			flen = ::MultiByteToWideChar( CP_ACP, 0, lst[i], -1, wbuf, 600 );
			ki_memcpy( buf, wbuf, flen*2 );
			for( int k=0; k!=flen; k++ )
				if( ((wchar_t*)buf)[k] == '/' )
					((wchar_t*)buf)[k] = '\\';
			buf += flen*2;
		}
		else
		{
			ki_strcpy( buf,lst[i] );
			for( int k=0; k!=lst[i].len(); k++ )
				if( buf[k] == '/' )
					buf[k] = '\\';
			buf += lst[i].len() + 1;
		}
	}
	*buf=0;
	if( fWide )
		buf[1]='\0';

	::GlobalUnlock( hDrop );

	stg->hGlobal        = hDrop;
	stg->tymed          = TYMED_HGLOBAL;
	stg->pUnkForRelease = NULL;
	return true;
}

BOOL CALLBACK CArcViewDlg::proc( UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg )
	{
	//-- メインウインドウ指定 ---------------------
	case WM_ACTIVATE:
		if( LOWORD(wp)==WA_ACTIVE || LOWORD(wp)==WA_CLICKACTIVE )
		{
			app()->setMainWnd( this );
			return TRUE;
		}
		break;

	//-- リサイズ関連の処理 ---------------------
	case WM_GETMINMAXINFO:
		{
			RECT self,child;
			::GetWindowRect( hwnd(), &self );
			::GetWindowRect( item(IDC_REF), &child );
			POINT& sz = ((MINMAXINFO*)lp)->ptMinTrackSize;
			sz.x = child.right - self.left + 18;
			sz.y = child.bottom - self.top + 100;
		}
		return TRUE;
	case WM_SIZE:
		if( wp!=SIZE_MAXHIDE && wp!=SIZE_MINIMIZED )
		{
			RECT self,ref,child,sbar;
			::GetWindowRect( hwnd(), &self );
			::GetWindowRect( item(IDC_REF), &ref );
			::GetWindowRect( item(IDC_FILELIST), &child );
			::GetClientRect( item(IDC_STATUSBAR), &sbar );

			::SetWindowPos( item(IDC_FILELIST), NULL, 0, 0,
				LOWORD(lp),
				(self.bottom-ref.bottom)-(child.top-ref.bottom)
				-(sbar.bottom-sbar.top)-10,
				SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER );

			::GetClientRect( hwnd(), &self );
			::SetWindowPos( item(IDC_STATUSBAR), NULL, sbar.left,
				self.bottom - (sbar.bottom-sbar.top),
				0, 0, SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOZORDER );
		}
		break;

	case WM_NOTIFY:
		if( wp==IDC_FILELIST && m_bAble )
		{
			NMHDR* phdr=(NMHDR*)lp;
			if( phdr->code==LVN_BEGINDRAG || phdr->code==LVN_BEGINRDRAG )
			{
				if( setSelection() )
					kiDropSource::DnD( this, DROPEFFECT_COPY );
				return TRUE;
			}
			else if( phdr->code==LVN_COLUMNCLICK )
				DoSort( ((NMLISTVIEW*)lp)->iSubItem );
			else if( phdr->code==NM_DBLCLK )
				sendMsg( WM_COMMAND, IDC_SHOW );
			else if( phdr->code==NM_RCLICK )
			{
				if( setSelection() )
					DoRMenu();
			}
		}
		break;

	case WM_COMMAND:
		switch( LOWORD(wp) )
		{
		case IDC_SELECTINV: // 選択反転
			{
				LVITEM item;
				item.mask = LVIF_STATE;
				item.stateMask = LVIS_SELECTED;
				int j,m=sendMsgToItem( IDC_FILELIST, LVM_GETITEMCOUNT );
				for( j=0; j!=m; j++ )
				{
					item.state = ~sendMsgToItem( IDC_FILELIST, LVM_GETITEMSTATE, j, LVIS_SELECTED );
					sendMsgToItem( IDC_FILELIST, LVM_SETITEMSTATE, j, (LPARAM)&item );
				}
				::SetFocus( this->item(IDC_FILELIST) );
			}
			return TRUE;

		case IDC_REF: // 解凍先設定
			kiSUtil::getFolderDlgOfEditBox( item(IDC_DDIR), hwnd(), kiStr().loadRsrc(IDS_CHOOSEDIR) );
			return TRUE;

		case IDC_MELTEACH: // 一部解凍
			if( setSelection() )
			{
				setdir();
				int result = m_pArc->melt( m_fname, m_ddir, &m_files );
				if( result<0x8000 )
					myapp().open_folder( m_ddir, 1 );
				else if( result != 0x8020 )
				{
					char str[255];
					wsprintf( str, "%s\nError No: [%x]",
						(const char*)kiStr().loadRsrc( IDS_M_ERROR ), result );
					app()->msgBox( str );
				}
				kiSUtil::switchCurDirToExeDir(); // 念のため
			}
			return TRUE;

		case IDC_SHOW: // 表示
			if( setSelection() )
			{
				int assocCnt = hlp_cnt_check();
				if( 0x8000 > m_pArc->melt( m_fname, m_tdir, &m_files ) )
				{
					if( assocCnt != -1 )
						m_files[assocCnt].selected = false;
					for( unsigned i=0; i!=m_files.len(); i++ )
						if( m_files[i].selected )
						{
							kiPath tmp(m_tdir);
							char yen[MAX_PATH];
							ki_strcpy( yen, m_files[i].inf.szFileName );
							for( char* p=yen; *p; p=kiStr::next(p) )
								if( *p=='/' )
									*p = '\\';
							tmp += yen;
							::ShellExecute( hwnd(), NULL, tmp, NULL, m_tdir, SW_SHOWDEFAULT );
						}
				}
				kiSUtil::switchCurDirToExeDir(); // 念のため
			}
			return TRUE;
		}
	}
	return FALSE;
}

int CArcViewDlg::hlp_cnt_check()
{
	// 一個目の選択済みファイルが .hlp か否か
	for( unsigned i=0; i!=m_files.len(); i++ )
		if( m_files[i].selected )
			break;
	if( i==m_files.len() )
		return -1;
	int x = kiPath::ext(m_files[i].inf.szFileName)-m_files[i].inf.szFileName;
	if( 0!=ki_strcmpi( "hlp", m_files[i].inf.szFileName+x ) )
		return -1;

	// .cnt のファイル名
	char cntpath[FNAME_MAX32];
	ki_strcpy( cntpath, m_files[i].inf.szFileName );
	cntpath[x]='c', cntpath[x+1]='n', cntpath[x+2]='t';

	// .cntも一時的に選択する
	for( i=0; i!=m_files.len(); i++ )
		if( 0==ki_strcmpi( cntpath, m_files[i].inf.szFileName ) )
		{
			if( m_files[i].selected )
				return -1;
			m_files[i].selected = true;
			return i;
		}
	return -1;
}

int CALLBACK CArcViewDlg::lv_compare( LPARAM p1, LPARAM p2, LPARAM type )
{
	bool rev = false;
	if( type>=10000 )
		rev=true, type-=10000;
	int ans = 0;

	INDIVIDUALINFO *a1=&((arcfile*)p1)->inf, *a2=&((arcfile*)p2)->inf;
	switch( type )
	{
	case 0: //NAME
		ans = ::lstrcmp( kiPath::name(a1->szFileName),
			             kiPath::name(a2->szFileName) );
		break;
	case 1: //SIZE
		ans = (signed)a1->dwOriginalSize - (signed)a2->dwOriginalSize;
		break;
	case 2: //DATE,TIME
		ans = (signed)a1->wDate - (signed)a2->wDate;
		if( ans==0 )
			ans = (signed)a1->wTime - (signed)a2->wTime;
		break;
	case 3:{//RATIO
		int cr1, cr2;
		if( a1->dwOriginalSize==0 )        cr1=100;
		else if( a1->dwCompressedSize==0 ) cr1=-1;
		else cr1 = (a1->dwCompressedSize*100)/(a1->dwOriginalSize);
		if( a2->dwOriginalSize==0 )        cr2=100;
		else if( a2->dwCompressedSize==0 ) cr2=-1;
		else cr2 = (int)((((__int64)a2->dwCompressedSize)*100)/(a2->dwOriginalSize));
		ans = cr1 - cr2;
		}break;
	case 4: //METHOD
		ans = ::lstrcmp( a1->szMode, a2->szMode );
		break;
	case 5:{//PATH
		kiPath pt1(a1->szFileName), pt2(a2->szFileName);
		pt1.beDirOnly(), pt2.beDirOnly();
		ans = ::lstrcmp( pt1, pt2 );
		}break;
	}

	return rev ? -ans : ans;
}

void CArcViewDlg::DoSort( int col )
{
	WPARAM p = col + (m_bSmallFirst[col] ? 0 : 10000);
	sendMsgToItem( IDC_FILELIST, LVM_SORTITEMS, p, (LPARAM)lv_compare );
	m_bSmallFirst[col] = !m_bSmallFirst[col];
}

void CArcViewDlg::GenerateDirMenu( HMENU m, int& id, StrArray* sx, const kiPath& pth )
{
	// フォルダ内リストアップ
	kiFindFile ff;
	ff.begin( kiPath(pth)+="*" );
	for( WIN32_FIND_DATA fd; ff.next(&fd); )
		if( fd.cFileName[0]!='.'
		 && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) )
		{
			kiPath fullpath(pth); fullpath+=fd.cFileName;
			const int pID=id;
			MENUITEMINFO mi = { sizeof(MENUITEMINFO) };

			if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				// 再帰的に
				mi.fMask = MIIM_SUBMENU | 0x00000040;// (MIIM_STRING)
				mi.hSubMenu = ::CreatePopupMenu();
				GenerateDirMenu( mi.hSubMenu, id, sx,
					kiPath(kiPath(fullpath)+="\\") );
			}
			else
			{
				const char* ext = kiPath::ext(fd.cFileName);
				if( ::lstrlen(ext) > 4 ) continue;
				if( 0==::lstrcmpi(ext,"lnk") )
					*const_cast<char*>(ext-1) = '\0';
				mi.fMask = MIIM_ID | 0x00000040;// (MIIM_STRING)
				mi.wID = id++;
				sx->add( fullpath );
			}

			mi.dwTypeData = const_cast<char*>((const char*)fd.cFileName);
			mi.cch        = ::lstrlen(fd.cFileName);
			::InsertMenuItem( m, pID, FALSE, &mi );
		}
}

void CArcViewDlg::DoRMenu()
{
	// メニュー作成
	HMENU m = ::CreatePopupMenu();
	POINT pt; ::GetCursorPos( &pt );
	const int IDSTART = 128;

	// フォルダの中身をリストアップしつつメニューに追加
	int id = IDSTART;
	StrArray lst;
	GenerateDirMenu( m, id, &lst, kiPath(CSIDL_SENDTO) );

	// メニュー表示
	id = ::TrackPopupMenu( m,
		TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD|TPM_NONOTIFY,
		pt.x, pt.y, 0, hwnd(), NULL );
	::DestroyMenu( m );

	// 結果処理
	if( id != 0 )
	{
		kiStr cmd;
		if( 0x8000>m_pArc->melt( m_fname, m_tdir, &m_files ) )
		{
			for( UINT i=0; i!=m_files.len(); i++ )
				if( m_files[i].selected )
				{
					cmd += "\"";
					cmd += m_tdir;
					const char* buf = m_files[i].inf.szFileName;
					for( int k=0; buf[k]; ++k )
						cmd += ( buf[k]=='/' ? '\\' : buf[k] );
					cmd += "\" ";
				}
			ShellExecute(hwnd(),NULL,lst[id-IDSTART],cmd,NULL,SW_SHOW);
		}
	}
}

