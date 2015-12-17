#ifndef AFX_ARCMSC_H__1891072B_44D7_4577_AF05_6D221A2FD0DD__INCLUDED_
#define AFX_ARCMSC_H__1891072B_44D7_4577_AF05_6D221A2FD0DD__INCLUDED_

#include "Archiver.h"
#include "SubDlg.h"

class CArcMsc : public CArchiver
{
public:
	CArcMsc() : CArchiver("") {}

private:
	int v_load()
		{
			set_cmp_ext( "??_" ), add_cmp_mhd( "MSCompress" );
			return aCheck|aMelt|aList|aMeltEach|aCompress;
		}
	int  v_contents( const kiPath& aname, kiPath& dnam ){return aSingleFile;}

	bool v_check( const kiPath& aname );
	int  v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files );
	bool v_list( const arcname& aname, aflArray& files );
	int  v_compress( const kiPath& base, const wfdArray& files, const kiPath& ddir, int method, bool sfx );
	kiStr v_name(const char*) const { return "MSCompress"; }

private:
	bool header( kiFile& fp, unsigned long* siz, char* ext );
	void filename( char* oname, const char* ol, char ext );
	bool do_lzss( kiFile& in, kiFile& out, CArcProgressDlg& dlg );
};

#endif
