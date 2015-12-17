// NoahCM.h
//-- CNoahConfigManager -- save / load / modify the setting of 'Noah' --

#ifndef AFX_NOAHCM_H__ACE475C1_D925_4F9E_BDCA_783B921E6FD5__INCLUDED_
#define AFX_NOAHCM_H__ACE475C1_D925_4F9E_BDCA_783B921E6FD5__INCLUDED_

class CNoahConfigManager;

class CNoahConfigDialog : public kiPropSheet
{
public:
	class CCmprPage : public kiPropSheetPage
	{
	public:
		CCmprPage();
	private:
		BOOL onInit();
		bool onOK();
		bool onCancel();
		BOOL CALLBACK proc( UINT msg, WPARAM wp, LPARAM lp );
		void correct( const char* ext, bool first );
		void SetUpToolTip();
	private:
		HWND m_tooltip;
	};
	class CMeltPage : public kiPropSheetPage
	{
	public:
		CMeltPage();
	private:
		BOOL onInit();
		bool onOK();
		BOOL CALLBACK proc( UINT msg, WPARAM wp, LPARAM lp );
		void correct();
	};
	class CWinXPage : public kiPropSheetPage
	{
	public:
		CWinXPage();
	private:
		BOOL onInit();
		BOOL CALLBACK proc( UINT msg, WPARAM wp, LPARAM lp );
		bool onOK();
	};
	class CAssPage : public kiDialog
	{
	public:
		CAssPage( HWND parent );
	private:
		BOOL onInit();
		BOOL CALLBACK proc( UINT msg, WPARAM wp, LPARAM lp );
		bool onOK();
	};
	class CInfoPage : public kiPropSheetPage
	{
	public:
		CInfoPage();
	private:
		BOOL onInit();
	};

public:
	CNoahConfigDialog();

private:
	bool onOK();
	bool onCancel();
	void onApply();
	void onHelp();
	void onCommand( UINT id );
	BOOL onInit();
	void onDrop( HDROP hdrop );
	static void shift_and_button();
};

enum loading_flag
{
	Mode    = 1,
	Melt    = 2,
	Compress= 4,
	Shell   = 8,
	OpenDir =16,
	All     =31,
};

class CNoahConfigManager
{
public: //-- 操作

	void init();
	void load( loading_flag what );
	void save();
	void dialog();

public: //-- 設定項目取得用インターフェイス

	// Section: Mode
	const int     mode()  { return m_Mode; }  // 0:圧専 1:圧優 2:解優 3:解専
	const bool  miniboot(){ return m_MiniBoot; } // 最小化起動？
	const bool  oldver()  { return m_OldVer; }// 古い形式でバージョン表示
	const int   extnum()  { return m_OneExt ? 1 : m_ZeroExt ? 0 : -1; } // 書庫名作成時に拡張子と見なす部分
	const int multiboot_limit() { return m_MbLim; } // 多重起動個数制限値
	// Section: Melt
	const kiPath& mdir()  { return m_MDir; }  // 解凍先
	const bool    mdirsm(){ return m_MDirSm; }// 同じディレクトリに解凍？
	const int     mkdir() { return m_MkDir; } // 0:x 1:file 2:dir 3:o
	const bool    mnonum(){ return m_MNoNum; }// 数字省略
	const char*   kill()  { return m_Kill; }// OFFにする内蔵ルーチン
	// Section: Compress
	const kiPath& cdir()  { return m_CDir; }  // 圧縮先
	const bool    cdirsm(){ return m_CDirSm; }// 同じディレクトリに圧縮？
	const kiStr&  cext()  { return m_CExt; }  // 圧縮形式
	const kiStr&  cmhd()  { return m_CMhd; }  // 圧縮メソッド
	// Section: Shell
	// Section: OpenDir
	const bool    modir() { return m_MODir; } // 解凍後開く？
	const bool    codir() { return m_CODir; } // 圧縮後開く？
	const kiStr&  openby(){ return m_OpenBy; }// 開くプログラム(隠し)

private: //-- 内部変数

	unsigned long m_Loaded;
	kiIniFile m_Ini;
	kiStr m_UserName;
	HINSTANCE m_hNoahXtDLL;
	enum { NOSHL, NOADMIN, SHLOK } m_bShlOK;

	// 設定項目
	int    m_Mode;
	kiPath m_MDir, m_CDir;
	bool   m_MODir,m_CODir,m_MDirSm,m_CDirSm;
	int    m_MkDir;
	int    m_MbLim;
	kiStr  m_CExt;
	kiStr  m_OpenBy;
	kiStr  m_CMhd;
	bool   m_MNoNum;
	bool   m_SCSendTo,m_SCDesktop;
	kiStr  m_Kill;
	bool   m_SECmp, m_SEExt, m_bAsso[13];
	bool   m_MiniBoot;
	bool   m_OldVer;
	bool   m_OneExt, m_ZeroExt;

	// 関数ロード
	FARPROC getProc( const char* name );

public:
	CNoahConfigManager()
		{
			m_hNoahXtDLL = NULL;
		}
	~CNoahConfigManager()
		{
			if( m_hNoahXtDLL )
				::FreeLibrary( m_hNoahXtDLL );
		}
friend class CNoahConfigDialog::CCmprPage;
friend class CNoahConfigDialog::CMeltPage;
friend class CNoahConfigDialog::CWinXPage;
friend class CNoahConfigDialog::CInfoPage;
friend class CNoahConfigDialog::CAssPage;
};

#endif
