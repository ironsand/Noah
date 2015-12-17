#ifndef AFX_ARCACE_H__84B9CB87_29F7_428D_A3E7_F239213B127C__INCLUDED_
#define AFX_ARCACE_H__84B9CB87_29F7_428D_A3E7_F239213B127C__INCLUDED_

#include "Archiver.h"
#include "SubDlg.h"

#include "unacedef.h"

class CArcAce : public CArchiver
{
public: //-- CArchiver é¿ëï --------------------------------

	CArcAce() : CArchiver("ace."), hDLL( NULL ) {}
	virtual ~CArcAce()
	{
		if(hDLL) 
			::FreeLibrary(hDLL);
	}
private:
	int  v_load();
	bool v_ver( kiStr& str );
	bool v_check( const kiPath& aname );
	int  v_contents( const kiPath& aname, kiPath& dname );
	int  v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files );
	bool v_list( const arcname& aname, aflArray& files );
	kiStr v_name(const char*) const { return "Unace32"; }

private: //-- UnAceV2.DLL ëÄçÏ ------------------------------

	HINSTANCE hDLL;
	static int __stdcall callback_error( pACEErrorCallbackProcStruc Error );
	static int __stdcall callback_request( pACERequestCallbackProcStruc Request );
	static int __stdcall callback_state( pACEStateCallbackProcStruc State );
	static int __stdcall callback_info( pACEInfoCallbackProcStruc Info );
	bool listup( tACEFileDataStruc& f );
	aflArray* list;
	CArcProgressDlg* dlg;
	CArcPathCheckDlg* pathCheck;
	char pwdBuf[100];
	const char* m_aname;
	int aCnt;
	kiPath* dName;
	bool m_bCanceled;

private: //-- UnAce.h ---------------------------------------

	// API
	typedef int (__stdcall *FI)( pACEInitDllStruc );
	typedef int (__stdcall *FR)( LPSTR, pACEReadArchiveDataStruc );
	typedef int (__stdcall *FL)( LPSTR, pACEListStruc );
	typedef int (__stdcall *FT)( LPSTR, pACETestStruc );
	typedef int (__stdcall *FE)( LPSTR, pACEExtractStruc );
	FI aceInit; FR aceHead; FL aceList; FT aceTest; FE aceExtr;
};

#endif
