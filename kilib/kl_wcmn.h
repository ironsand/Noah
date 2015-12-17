//--- K.I.LIB ---
// kl_wcmn.h : windows-common-interface operatin

#ifndef AFX_KIWINCOMMON_H__0686721C_CAFB_4C2C_9FE5_0F482EA6A60B__INCLUDED_
#define AFX_KIWINCOMMON_H__0686721C_CAFB_4C2C_9FE5_0F482EA6A60B__INCLUDED_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// シェルユーティリティークラス

class kiSUtil
{
public:
	// カレントディレクトリ退避
	static void switchCurDirToExeDir();

	// 「フォルダの選択」ダイアログ
	static bool getFolderDlg( char* buf, HWND par, const char* title, const char* def );
	static void getFolderDlgOfEditBox( HWND wnd, HWND par, const char* title );

	// 対応する拡張子用アイコンの、システムイメージリストのインデックスを返す。
	static int getSysIcon( const char* ext );

	// 最後のエラーを表示
	static void msgLastError( const char* msg = NULL );

	// 自分へのショートカット作成
	static void createShortCut( const kiPath& at, const char* name );

	// ファイルが存在する？
	static bool exist( const char* fname );
	static bool isdir( const char* fname );

	// カレントディレクトリを安全なところに移してLoadLibrary
	static HMODULE loadLibrary(LPCTSTR lpFileName)
	{
		char original_cur[MAX_PATH], sys[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, original_cur);
		::GetSystemDirectory(sys, MAX_PATH);
		::SetCurrentDirectory(sys);
		HMODULE han = ::LoadLibrary(lpFileName);
		::SetCurrentDirectory(original_cur);
		return han;
	}
};

#endif
