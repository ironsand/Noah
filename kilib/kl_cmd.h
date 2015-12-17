//--- K.I.LIB ---
// kl_cmd.h : commandline parser

#ifndef AFX_KICMDPARSER_H__843A27E0_5DBF_48AF_A748_FA7F111F699A__INCLUDED_
#define AFX_KICMDPARSER_H__843A27E0_5DBF_48AF_A748_FA7F111F699A__INCLUDED_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// kiCmdParser : コマンド文字列をchar*の配列に分割

class kiCmdParser
{
public: //-- 外向きインターフェイス --------------------------

	// 文字列で初期化
	kiCmdParser( char* cmd, bool ignoreFirst=false );
	kiCmdParser( const char* cmd, bool ignoreFirst=false );

	// スイッチ文字列の配列
	cCharArray& option()
		{ return m_Switch; }

	// スイッチ以外の文字列の配列
	cCharArray& param()
		{ return m_Param; }

private: //-- 内部処理 -----------------------------------

	void doit( char* start, bool ignoreFirst );
	cCharArray m_Param;
	cCharArray m_Switch;
	char* m_Buffer;

public:

	virtual ~kiCmdParser();
};

#endif
