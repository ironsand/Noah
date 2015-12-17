// NoahAM.h
//-- CNoahArchiverManager -- control many archiver routines --

#ifndef AFX_NOAHAM_H__CCE30761_D91B_4570_931A_6C165B19B22F__INCLUDED_
#define AFX_NOAHAM_H__CCE30761_D91B_4570_931A_6C165B19B22F__INCLUDED_

#include "Archiver.h"

class CNoahArchiverManager
{
public: //-- 外向けインターフェイス ------------

	// 初期化
	void init();

	// ファイルリストを記憶
	unsigned long set_files( const cCharArray& files );
	unsigned long file_num() { return m_FName.len(); }
	const kiPath& get_basepath() { return m_BasePath; }
	bool map_melters( int mode );
	bool map_compressor( const char* ext, const char* method, bool sfx );

	// 解凍( 解凍先Dir以外の情報は中で勝手に取得します )
	void do_melting( kiPath& destdir );
	// 閲覧( 解凍先Dir以外の情報は中で勝手に取得します )
	void do_listing( kiPath& destdir );
	// 圧縮
	void do_compressing( kiPath& destdir, bool each );


	// バージョン情報
	void get_version( kiStr& str );
	// 圧縮形式リスト
	void get_cmpmethod( const char* set, int& def_mhd, StrArray& mhd_list, bool need_ext=false, cCharArray* ext_list=NULL );
	// b2eが一個以上読み込まれていればtrue
	bool b2e_enabled() { return m_b2e; }


private: //-- 内部処理 ---------------------------

	// ２重フォルダ解消など
	bool break_ddir( kiPath& dir, bool onlydir );
	CArchiver* fromExt( const char* ext );
	void generate_dirname( const char* src, kiPath& dst, bool rmn );

	kiPath   m_BasePath;
	wfdArray m_FName;
	kiArray<CArchiver*> m_AList;
	kiArray<kiPath> m_BasePathList;
	bool m_b2e;

	// 解凍用割り当て
	kiArray<CArchiver*> m_Melters;
	// 圧縮用割り当て
	CArchiver* m_Compressor;
	int        m_Method;
	bool       m_Sfx;

public:
	~CNoahArchiverManager()
		{
			for( unsigned int i=0; i!=m_AList.len(); i++ )
				delete m_AList[i];
		}
};

#endif
