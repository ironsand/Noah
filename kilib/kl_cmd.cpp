//--- K.I.LIB ---
// kl_cmd.h : commandline parser

#include "stdafx.h"
#include "kilib.h"


//------------------------ 文字列のメモリ処理など -----------------------//


kiCmdParser::kiCmdParser( char* cmd, bool ignoreFirst )
{
	m_Buffer = NULL;
	if( cmd )
		doit( cmd, ignoreFirst );
}

kiCmdParser::kiCmdParser( const char* cmd, bool ignoreFirst )
{
	m_Buffer=NULL;
	if( cmd )
	{
		m_Buffer = new char[ ki_strlen(cmd)+1 ];
		ki_strcpy( m_Buffer, cmd );
		doit( m_Buffer, ignoreFirst );
	}
}

kiCmdParser::~kiCmdParser()
{
	delete [] m_Buffer;
}


//---------------------------- 分割の処理 -----------------------------//


void kiCmdParser::doit( char* start, bool ignoreFirst )
{
	char* p=start;
	char endc;
	bool first = true;

	while( *p!='\0' )
	{
		// 余分な空白はスキップ
		while( *p==' ' ) //|| *p=='\t' || *p=='\r' || *p=='\n' )
			p++;

		// " だったら、その旨記録してさらに一個進める
		if( *p=='"' )
			endc='"', p++;
		else
			endc=' ';

		// end-of-text なら終了
		if( *p=='\0' )
			break;

		if( first && ignoreFirst )
			first = false;
		else
		{
			// 引数を保存
			if( *p=='-' )
				m_Switch.add( p );
			else
				m_Param.add( p );
		}

		// 引数の終わりへ…
		while( *p!=endc && *p!='\0' )
			p++;

		// 終わりは'\0'にすることによって、引数を区切る
		if( *p!='\0' )
			*(p++) = '\0';
	}
}
