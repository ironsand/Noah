
#include "stdafx.h"
#include "ArcAce.h"
#include "NoahApp.h"


int CArcAce::v_load()
{
	if( hDLL = kiSUtil::loadLibrary("UnAceV2") )
	{
		if( (aceInit = (FI)::GetProcAddress( hDLL,"ACEInitDll" ))
		 && (aceHead = (FR)::GetProcAddress( hDLL,"ACEReadArchiveData" ))
		 && (aceList = (FL)::GetProcAddress( hDLL,"ACEList" ))
		 && (aceTest = (FT)::GetProcAddress( hDLL,"ACETest" ))
		 && (aceExtr = (FE)::GetProcAddress( hDLL,"ACEExtract" )) )
		{
			tACEInitDllStruc ini;
			ki_memzero( &ini, sizeof(ini) );

			ini.GlobalData.Obj                 = this;
			ini.GlobalData.MaxArchiveTestBytes = 0x2ffFF;
			ini.GlobalData.MaxFileBufSize      = 0x2ffFF;
			ini.GlobalData.InfoCallbackProc    = callback_info;
			ini.GlobalData.ErrorCallbackProc   = callback_error;
			ini.GlobalData.RequestCallbackProc = callback_request;
			ini.GlobalData.StateCallbackProc   = callback_state;
			ini.GlobalData.DecryptPassword     = pwdBuf;

			if( 0 == aceInit( &ini ) )
				return aCheck|aMelt|aList|aMeltEach;
		}

		::FreeLibrary( hDLL );
		hDLL = NULL;
	}
	return 0;
}

bool CArcAce::v_ver( kiStr& str )
{
	str = "UnAceV2.dll  ";

	//-- 存在確認
	char buf[40];
	if( hDLL )
		if( GetVersionInfoStr( "UnAceV2", buf, sizeof(buf) ) )
			str += buf;
		else
			str += "OK!";
	else
		str += "----";

	return true;
}

bool CArcAce::v_check( const kiPath& aname )
{
	//-- 書庫情報を読み込んでみる
	tACEReadArchiveDataStruc dt;
	ki_memzero( &dt, sizeof(dt) );
	return (  0==aceHead( const_cast<char*>((const char*)aname), &dt )
		&& NULL!=dt.ArchiveData );
}

bool CArcAce::v_list( const arcname& aname, aflArray& files )
{
	list = &files;

	//-- リスティング開始！
	tACEListStruc lst;
	ki_memzero( &lst, sizeof(lst) );
	lst.Files.SourceDir   = "";
	lst.Files.FileList    = "*";
	lst.Files.ExcludeList = "";
	lst.Files.FullMatch   = FALSE;

	char anm[MAX_PATH];
	ki_strcpy( anm, aname.basedir );
	ki_strcat( anm, aname.lname );

	return ( 0==aceList( anm, &lst ) );
}

int CArcAce::v_contents( const kiPath& aname, kiPath& dname )
{
	list = NULL;
	aCnt = aUnknown;
	dName = &dname;

	//-- リスティング開始！
	tACEListStruc lst;
	ki_memzero( &lst, sizeof(lst) );
	lst.Files.SourceDir   = "";
	lst.Files.FileList    = "*";
	lst.Files.ExcludeList = "";
	lst.Files.FullMatch   = TRUE;
	if( 0!=aceList( const_cast<char*>((const char*)aname), &lst ) )
		return aMulti;
	return aCnt;
}

int CArcAce::v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files )
{
	pathCheck = new CArcPathCheckDlg;

	//-- 渡すファイルリスト作成
	kiStr lst;
	if( files )
	{
		dlg = NULL;
		for( unsigned int i=0; i!=list->len(); i++ )
			if( (*files)[i].selected )
				lst += (*files)[i].inf.szFileName, lst += '\xd';
	}
	else
		lst = "*", dlg = new CArcProgressDlg( 32768 );

	//-- 展開開始！
	::SetCurrentDirectory( ddir );

	m_aname = aname.lname;
	pwdBuf[0] = -1;

	tACEExtractStruc ex;
	ki_memzero( &ex, sizeof(ex) );
	ex.Files.SourceDir   = "";
	ex.Files.FileList    = const_cast<char*>((const char*)lst);
	ex.Files.ExcludeList = "";
	ex.Files.FullMatch   = files==NULL;
	ex.DestinationDir    = const_cast<char*>((const char*)ddir);
	ex.ExcludePath       = FALSE;
	ex.DecryptPassword   = "";

	char anm[MAX_PATH];
	ki_strcpy( anm, aname.basedir );
	ki_strcat( anm, aname.lname );

	m_bCanceled = false;
	bool ans = ( 0==aceExtr( anm, &ex ) );
	if( dlg && ans )
	{
		dlg->change(NULL,32768);
		::Sleep(100);
	}
	delete dlg;
	delete pathCheck;
	return ans ? 0 : (m_bCanceled?0x8020:0xffff);
}

int __stdcall CArcAce::callback_error( pACEErrorCallbackProcStruc Error )
{
	return ACE_CALLBACK_RETURN_OK;
}

int __stdcall CArcAce::callback_info( pACEInfoCallbackProcStruc Info )
{
	return ACE_CALLBACK_RETURN_OK;
}

int __stdcall CArcAce::callback_request( pACERequestCallbackProcStruc Request )
{
	// パスワード要求＠解凍 ... ArchivedFile
	if( Request->Global.Code == ACE_CALLBACK_REQUEST_PASSWORD
	 && Request->Global.Operation == ACE_CALLBACK_OPERATION_EXTRACT )
	{
		CArcAce* ace = (CArcAce*)(Request->Global.GlobalData->Obj);
		if( ace->pwdBuf[0] == -1 )
		{
			ace->pwdBuf[0]=0;

			CArcPwdDlg pwd( ace->dlg ? ace->dlg->hwnd() : NULL, ace->pwdBuf, 60, ace->m_aname );
			if( IDCANCEL == pwd.getEndCode() )
			{
				ace->m_bCanceled = true;
				return ACE_CALLBACK_RETURN_CANCEL;
			}

			Request->Global.GlobalData->DecryptPassword = ace->pwdBuf;
		}
	}

	if( Request->Global.Code == ACE_CALLBACK_REQUEST_CHANGEVOLUME )
		if( !kiSUtil::exist(Request->Archive.ArchiveData->ArchiveName) )
			return ACE_CALLBACK_RETURN_CANCEL;

	return ACE_CALLBACK_RETURN_OK;
}

int __stdcall CArcAce::callback_state( pACEStateCallbackProcStruc State )
{
	// ファイル処理開始＠解凍orリスト
	if( State->Progress.Code == ACE_CALLBACK_STATE_STARTFILE )
	{
		CArcAce* ptr = (CArcAce*)(State->ArchivedFile.GlobalData->Obj);

		if( State->Progress.Operation == ACE_CALLBACK_OPERATION_LIST )
		{
			if( !ptr->listup( *(State->ArchivedFile.FileData) ) )
				return ACE_CALLBACK_RETURN_CANCEL;
		}
		else if( State->Progress.Operation == ACE_CALLBACK_OPERATION_EXTRACT )
		{
			if( ptr->dlg )
				ptr->dlg->change( State->ArchivedFile.FileData->SourceFileName );
			if( ptr->pathCheck )
				if( containsDangerPath( State->ArchivedFile.FileData->SourceFileName ) )
					if( ! ptr->pathCheck->is_ok_to_extract( State->ArchivedFile.FileData->SourceFileName, ptr->dlg ) )
						return ACE_CALLBACK_RETURN_CANCEL;
		}
	}
	// 進展状況グラフ＠解凍
	else if( State->Progress.Code == ACE_CALLBACK_STATE_PROGRESS )
	{
		CArcAce* ptr = (CArcAce*)(State->ArchivedFile.GlobalData->Obj);

		if( State->Progress.Operation == ACE_CALLBACK_OPERATION_EXTRACT && ptr->dlg )
		{
			unsigned long prs = (unsigned long)State->Progress.ProgressData->TotalProcessedSize;
			unsigned long ttl = (unsigned long)State->Progress.ProgressData->TotalSize;
			ptr->dlg->change( NULL, ttl ? (int)((((__int64)prs)<<15)/ttl) : 10000 );
			if( !ptr->dlg->msgloop() )
			{
				ptr->m_bCanceled = true;
				return ACE_CALLBACK_RETURN_CANCEL;
			}
		}
	}
	// CRCチェック＠解凍
	else if( State->Progress.Code == ACE_CALLBACK_STATE_ENDCRCCHECK )
	{
		if( !State->CRCCheck.CRCOk )
			::DeleteFile( State->CRCCheck.FileData->SourceFileName );
	}

	return ACE_CALLBACK_RETURN_OK;
}

bool CArcAce::listup( tACEFileDataStruc& f )
{
	if( list ) // 全ファイルリスティングモード
	{
		arcfile x;

		ki_strcpy( x.inf.szFileName, f.SourceFileName );
		x.isfile = ( 0== (f.Attributes & 0x10) ); //_A_SUBDIR
		x.inf.dwCompressedSize = (unsigned long)f.CompressedSize;
		x.inf.dwOriginalSize   = (unsigned long)f.Size;
		x.inf.wDate = (unsigned short)(f.Time>>16);
		x.inf.wTime = (unsigned short)(f.Time);

		if( f.Method==0 )		ki_strcpy( x.inf.szMode, "store" );
		else if( f.Method&2 )	ki_strcpy( x.inf.szMode, "ace2" );
		else if( f.Method&1 )	ki_strcpy( x.inf.szMode, "ace1" );
		else					ki_strcpy( x.inf.szMode, "-ace-" );

		list->add( x );
		return true;
	}

	// 書庫内ファイル構成検索モード
	for( const char* p=f.SourceFileName; *p; p=kiStr::next(p) )
		if( *p=='\\' || *p=='/' )
			return true; // ルートのファイルのみ処理対象…

	if( aCnt != aUnknown )
		return false;

	aCnt = (f.Attributes & 0x10) ? aSingleDir : aSingleFile;
	if( aCnt == aSingleDir )
		*dName = f.SourceFileName;

	return true;
}
