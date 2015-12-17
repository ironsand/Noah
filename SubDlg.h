#ifndef AFX_ARCVIEWDLG_H__91EDF9F6_142E_4E25_BCE3_448E937E29D9__INCLUDED_
#define AFX_ARCVIEWDLG_H__91EDF9F6_142E_4E25_BCE3_448E937E29D9__INCLUDED_

#include "NoahApp.h"
#include "Archiver.h"
#include "resource.h"

class CArcViewDlg : public kiDialog, kiDataObject
{
public:
	CArcViewDlg( CArchiver* ptr,arcname& fnm,const kiPath& ddir )
		: kiDialog( IDD_ARCVIEW ), m_pArc( ptr ),
		m_fname( fnm ), m_ddir( ddir )
		{
			AddRef();
			myapp().get_tempdir( m_tdir );
		}

private: //-- ダイアログとしての処理

	BOOL CALLBACK proc( UINT msg, WPARAM wp, LPARAM lp );
	BOOL onInit();
	bool onOK();
	bool onCancel();
	void setdir()
		{
			char str[MAX_PATH];
			sendMsgToItem( IDC_DDIR, WM_GETTEXT, MAX_PATH, (LPARAM)str );
			m_ddir = str;
			m_ddir.beBackSlash( true );
			m_ddir.mkdir();
			m_ddir.beShortPath();
		}
	bool setSelection()
		{
			bool x=false;
			LVITEM it;
			it.mask = (LVIF_PARAM | LVIF_STATE);
			it.iSubItem = 0;
			it.stateMask = LVIS_SELECTED;
			for( it.iItem=0; sendMsgToItem( IDC_FILELIST, LVM_GETITEM, 0, (LPARAM)&it ); it.iItem++ )
				if( ((arcfile*)it.lParam)->selected = (0!=(LVIS_SELECTED&it.state)) )
					x = true;
			return x;
		}
	int hlp_cnt_check();
	bool m_bAble;

private: //-- ドラッグ＆ドロップ処理

	bool giveData( const FORMATETC& fmt, STGMEDIUM* stg, bool firstcall );

private: //-- ソート処理

	void DoSort( int col );
	static int CALLBACK lv_compare( LPARAM p1, LPARAM p2, LPARAM type );
	bool m_bSmallFirst[6];

private: //-- 右クリック

	void DoRMenu();
	void GenerateDirMenu( HMENU m, int& id, StrArray* sx, const kiPath& pth );

private: //-- 解凍作業

	CArchiver* m_pArc;
	arcname m_fname;
	kiPath m_ddir, m_tdir;
	aflArray m_files;

//-- 存在しているウインドウ数管理。
public:	static void clear() { st_nLife=0; }
private:static void hello() { st_nLife++; }
		static void byebye() { if(--st_nLife==0) kiWindow::loopbreak(); }
		static int st_nLife;
};


class CArcPwdDlg : public kiDialog
{
	char *buf;
	const char *nam;
	int siz;
public:
	CArcPwdDlg( HWND h, char* x, int s, const char* n ) : kiDialog( IDD_PASSWORD ), buf(x), siz(s), nam(n)
		{
			doModal( h );
		}
	BOOL onInit()
		{
			sendMsgToItem( IDC_MASK, BM_SETCHECK, BST_CHECKED );
			sendMsgToItem( IDC_EDIT, EM_SETLIMITTEXT, siz );
			sendMsgToItem( IDC_EDIT, EM_SETPASSWORDCHAR, '*' );
			sendMsg( WM_SETTEXT, 0, (LPARAM)nam );
			::SetFocus( item(IDC_EDIT) );
			return TRUE;
		}
	BOOL CALLBACK proc( UINT msg, WPARAM wp, LPARAM lp )
		{
			if( msg==WM_COMMAND && LOWORD(wp)==IDC_MASK )
			{
				if( BST_CHECKED==sendMsgToItem( IDC_MASK, BM_GETCHECK ) )
					sendMsgToItem( IDC_EDIT, EM_SETPASSWORDCHAR, '*' );
				else
					sendMsgToItem( IDC_EDIT, EM_SETPASSWORDCHAR );
				::InvalidateRect( item(IDC_EDIT), NULL, TRUE );
			}
			return FALSE;
		}
	bool onOK()
		{
			sendMsgToItem( IDC_EDIT, WM_GETTEXT, siz, (LPARAM)buf );
			return true;
		}
};

class CArcProgressDlg : public kiDialog
{
	unsigned int bl;
public:
	CArcProgressDlg( unsigned int barlimit, bool packing=false )
		: kiDialog( IDD_PROGRESS ), bl( barlimit )
		{
			createModeless();
			if( bl<0x10000 )
				sendMsgToItem( IDC_BAR, PBM_SETRANGE, 0, MAKELPARAM(0,bl) );
			if( packing )
				sendMsg( WM_SETTEXT, 0, (LPARAM)"Compressing..." );
			kiWindow::setFront( hwnd() );
			msgloop();
		}
	~CArcProgressDlg()
		{
			if( isAlive() )
				end( IDCANCEL );
		}
	bool msgloop()
		{
			kiWindow::msg();
			return isAlive();
		}
	void change( const char* name=NULL, unsigned int pos=0xffffffff )
		{
			if( name ) sendMsgToItem( IDC_FNAME, WM_SETTEXT, 0, (LPARAM)name );
			if( pos!=0xffffffff )
				if( bl<0x10000 )sendMsgToItem( IDC_BAR, PBM_SETPOS, pos );
				else			sendMsgToItem( IDC_BAR, PBM_SETPOS, pos*100/bl );
		}
};


class CArcPathCheckDlg
{
private:
	class TheDlg : public kiDialog
	{
	public:
		TheDlg( const char* path, HWND h )
			: kiDialog(IDD_PATHCHECK), result(CANCEL), path(path) { doModal(h); }
		enum { YES, ALL_YES, CANCEL } result;
	private:
		const char* path;
		BOOL onInit()
		{
			sendMsgToItem( IDC_PATHNAME, WM_SETTEXT, 0, (LPARAM)path );
			return TRUE;
		}
		BOOL CALLBACK proc( UINT msg, WPARAM wp, LPARAM lp )
		{
			if( msg==WM_COMMAND )
				switch( LOWORD(wp) )
				{
				case IDYES:     result = YES;     end(IDOK);     return TRUE;
				case ID_ALLYES: result = ALL_YES; end(IDOK);     return TRUE;
				case ID_ALLNO:  result = CANCEL;  end(IDCANCEL); return TRUE;
				}
			return FALSE;
		}
	};

private:
	bool all_yes;

public:
	CArcPathCheckDlg() : all_yes(false) {}

	bool is_ok_to_extract( const char* path, kiWindow* wnd )
	{
		if( all_yes )
			return true;
		switch( TheDlg(path,wnd?wnd->hwnd():NULL).result )
		{
		case TheDlg::ALL_YES: all_yes = true;
		case TheDlg::YES:     return true;
		}
		return false;
	}
};


#endif
