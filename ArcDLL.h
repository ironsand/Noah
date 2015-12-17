#ifndef AFX_ARCDLL_H__911C109D_A40E_4722_A6F7_E3ACB8C2D2E3__INCLUDED_
#define AFX_ARCDLL_H__911C109D_A40E_4722_A6F7_E3ACB8C2D2E3__INCLUDED_

#include "Archiver.h"

class CArcDLL : public CArchiver
{
public:
	CArcDLL(
		const char*  Dllname,	// DLL名
		const char*  Exts,		// 対応拡張子
		const char*  Ecmd,		// 解凍用コマンド
		const char*  Xcmd,		// 部分解凍用コマンド
		unsigned int Wild=1,	// ワイルドカードの形式( 0=no(*) 1=* 2=*.* )
		const char*  Scmd=NULL )// SFX変換用コマンド
		: dll( Dllname ), CArchiver( Exts ), m_Ecmd( Ecmd ),
			m_Xcmd( Xcmd ), m_Wild( Wild ), m_Scmd( Scmd ) {}

protected: //-- 派生してもよし
	virtual DWORD set_cmpr_mhd()
		{ return 0; }
	virtual const char* get_cmpr_ext( int method )
		{ return cmp_ext(); }
	virtual const char* get_cmpr_cmd( int method )
		{ return ""; }
	virtual void decorate_add_cmpr( kiStr& lst, const char* fname );
	virtual void decorate_add_melt( kiStr& lst, const char* fname );

protected: //-- 内部処理
	CArcModule dll;
	bool v_ver( kiStr& str ){ dll.ver(str); return true; }
	bool v_check( const kiPath& aname ){ return dll.chk(aname); }
	int  v_contents( const kiPath& aname, kiPath& dname ){ return dll.cnt(aname,dname,m_Wild==2?"*.*":"*"); }
	bool v_list( const arcname& aname, aflArray& files ){ return dll.lst_dll(aname,files,m_Wild==2?"*.*":"*"); }

	int  v_load();
	int  v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files );
	int  v_compress( const kiPath& base, const wfdArray& files, const kiPath& ddir, int method, bool sfx );
	kiStr v_name(const char*) const { return dll.name(); }

private:
	const char *m_Ecmd, *m_Xcmd, *m_Scmd;
	unsigned int m_Wild;
	int  cmpr( int mhd, kiStr& aname,const kiPath& to,const kiStr& lst );
	bool arc2sfx( const kiStr& aname );
};

//------------------------------------------------------------//

class CArcDLL_DotSlash : public CArcDLL
{
	// "./-" や "./@" とすることでとりあえず無理矢理圧縮する形式
	virtual void decorate_add_cmpr( kiStr& lst, const char* fname );

protected:
	CArcDLL_DotSlash(
		const char*  Dllname,
		const char*  Exts,
		const char*  Ecmd,
		const char*  Xcmd,
		unsigned int Wild=1,
		const char*  Scmd=NULL )
		: CArcDLL( Dllname, Exts, Ecmd, Xcmd, Wild, Scmd ) {}
};

//------------------------------------------------------------//

class CArcRar : public CArcDLL
{
public:
	CArcRar() : CArcDLL(
		"Unrar32.dll",
		"rar.",
		"-x",
		"-x -s -y --" ) {}

private:
	kiStr v_name(const char*) const { return "RAR"; }
	// 特別に派生
	bool v_check( const kiPath& aname );
	int  v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files );
};

//------------------------------------------------------------//

class CArcUnZip : public CArcDLL
{
public:
	CArcUnZip() : CArcDLL(
		"UnZip32.dll",
		"zip.jar.",
		"-x",
		"-x --i -o" ) {}
private:
	kiStr v_name(const char*) const { return "ZIP"; }
	// 特別に派生
	int  v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files );
};

//------------------------------------------------------------//

class CArcLzh : public CArcDLL
{
public:
	CArcLzh() : CArcDLL(
		"Unlha32.dll",
		"lzh.lha.lzs.",
		"x -a1 -jf0",
		"x -a1 -jf0 -jyo -p1 -n -gm --",
		0,
		"s -gw3 -x1" ) {}
private:
	kiStr v_name(const char*) const { return "LZH"; }
	DWORD set_cmpr_mhd();
	const char* get_cmpr_cmd( int method );
	void decorate_add_cmpr( kiStr& lst, const char* fname );
	void decorate_add_melt( kiStr& lst, const char* fname );
};

//------------------------------------------------------------//

class CArcZip : public CArcDLL
{
public:
	CArcZip() : CArcDLL(
		"Zip32j.dll",
		"",
		NULL,
		NULL,
		1,
		"--sfx" ) {}
private:
	DWORD set_cmpr_mhd();
	const char* get_cmpr_cmd( int method );
	void decorate_add_cmpr( kiStr& lst, const char* fname );
};

//------------------------------------------------------------//

class CArcCab : public CArcDLL_DotSlash
{
public:
	CArcCab() : CArcDLL_DotSlash(
		"Cab32.dll",
		"cab.",
		"-x",
		"-x -i -o",
		1,
		"-f" ) {}
private:
	kiStr v_name(const char*) const { return "CAB"; }
	DWORD set_cmpr_mhd();
	const char* get_cmpr_cmd( int method );
};

//------------------------------------------------------------//

class CArcTar : public CArcDLL_DotSlash
{
public:
	CArcTar() : CArcDLL_DotSlash(
		"Tar32.dll",
		"tar.tgz.tbz.gz.bz2.z.taz.rpm.deb.ar.xz.lzma.",
		"-xvf",
		"--check-all-path=1 --display-dialog=0 -xvf",
		2 ) {}
private:
	#define ARCHIVETYPE_NORMAL 0
	#define ARCHIVETYPE_TAR 1
	#define ARCHIVETYPE_TARGZ 2
	#define ARCHIVETYPE_TARZ 3
	#define ARCHIVETYPE_GZ 4
	#define ARCHIVETYPE_Z 5
	#define ARCHIVETYPE_TARBZ2 6
	#define ARCHIVETYPE_BZ2	7

	#define ARCHIVETYPE_CPIO	32
	#define ARCHIVETYPE_CPIOGZ	32+4
	#define ARCHIVETYPE_CPIOZ	32+5
	#define ARCHIVETYPE_CPIOBZ2	32+7

	#define ARCHIVETYPE_AR		48
	#define ARCHIVETYPE_ARGZ	48+4
	#define ARCHIVETYPE_ARZ		48+5
	#define ARCHIVETYPE_ARBZ2	48+7
	kiStr v_name(const char* an) const { switch(dll.arctype(an)) {
		case ARCHIVETYPE_GZ:     return "GZ";
		case ARCHIVETYPE_Z:      return "Z";
		case ARCHIVETYPE_BZ2:    return "BZ2";
		case ARCHIVETYPE_TARGZ:  return "TAR+GZ";
		case ARCHIVETYPE_TARZ:   return "TAR+Z";
		case ARCHIVETYPE_TARBZ2: return "TAR+BZ2";
		case ARCHIVETYPE_CPIOGZ: return kiStr("rpm").isSame(kiPath::ext(an)) ? "RPM" : "CPIO+GZ";
		case ARCHIVETYPE_CPIOZ:  return "CPIO+Z";
		case ARCHIVETYPE_CPIOBZ2:return "CPIO+BZ2";
		case ARCHIVETYPE_ARGZ:   return "AR+GZ";
		case ARCHIVETYPE_ARZ:    return "AR+Z";
		case ARCHIVETYPE_ARBZ2:  return "AR+BZ2";
		case ARCHIVETYPE_AR:     return "AR";
		case ARCHIVETYPE_CPIO:   return "CPIO";
		default:                 return "TAR";
	}}
	const char* get_cmpr_ext( int method );
	DWORD set_cmpr_mhd();
	const char* get_cmpr_cmd( int method );
};

//------------------------------------------------------------//

class CArcBga : public CArcDLL_DotSlash
{
public:
	CArcBga() : CArcDLL_DotSlash(
		"Bga32.dll",
		"gza.bza.",
		"x -a",
		"x -a -i -o",
		0,
		"s" ) {}
private:
	kiStr v_name(const char* an) const { return dll.arctype(an)==2 ? "BZA" : "GZA"; }
	const char* get_cmpr_ext( int method );
	DWORD set_cmpr_mhd();
	const char* get_cmpr_cmd( int method );
};

//------------------------------------------------------------//

class CArcYz1 : public CArcDLL
{
public:
	CArcYz1() : CArcDLL(
		"Yz1.dll",
		"yz1.",
		"x",
		"x -y -z -i2 --",
		0,
		"s" ) {}
private:
	kiStr v_name(const char*) const { return "YZ1"; }
	DWORD set_cmpr_mhd();
	const char* get_cmpr_cmd( int method );
};

//------------------------------------------------------------//

class CArcArj : public CArcDLL
{
public:
	CArcArj() : CArcDLL
		( "Unarj32j.dll", "arj.", "x -jyc", "x -y -p -hm -i -!" ) {}

private:
	kiStr v_name(const char*) const { return "ARJ"; }
};

//------------------------------------------------------------//

class CArcGca : public CArcDLL
{
public:
	CArcGca() : CArcDLL
		( "UnGCA32.dll", "gca.", "e -sx1", "ex -xx1 -sx0 -yx0" ) {}

private:
	kiStr v_name(const char*) const { return "GCA"; }
};

//------------------------------------------------------------//

class CArc7z : public CArcDLL_DotSlash
{
public:
	CArc7z() : CArcDLL_DotSlash(
		"7-zip32.dll",
		"7z.zip.jar.",
		"x --",
		"x -y -hide --",
		1,
		"" ) {}
private:
	kiStr v_name(const char* an) const { return dll.arctype(an)==1 ? "ZIP" : "7Z"; }
	DWORD set_cmpr_mhd();
	const char* get_cmpr_cmd( int method );
	const char* get_cmpr_ext( int method );
	virtual int v_compress( const kiPath& base, const wfdArray& files, const kiPath& ddir, int method, bool sfx );
	bool m_SfxMode;
};

//------------------------------------------------------------//

class CArc7zZip : public CArcDLL_DotSlash
{
public:
	CArc7zZip() : CArcDLL_DotSlash(
		"7-zip32.dll",
		"",NULL,NULL,1,"" ) {}
private:
	DWORD set_cmpr_mhd();
	const char* get_cmpr_cmd( int method );
	virtual bool v_ver( kiStr& ){return false;}//CArc7zに任せる
	virtual int v_compress( const kiPath& base, const wfdArray& files, const kiPath& ddir, int method, bool sfx );
};

//------------------------------------------------------------//

#endif
