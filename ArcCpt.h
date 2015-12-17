#ifndef AFX_ARCCPT_H__B5D13025_2DA9_4489_8BCB_ACB84B908A84__INCLUDED_
#define AFX_ARCCPT_H__B5D13025_2DA9_4489_8BCB_ACB84B908A84__INCLUDED_

#include "Archiver.h"
#include "SubDlg.h"

class CArcCpt : public CArchiver
{
public:
	CArcCpt() : CArchiver("cpt.bin.") {}

private:
	int  v_load(){return aCheck|aMelt|aList|aMeltEach;}
	bool v_check( const kiPath& aname );
	int  v_contents( const kiPath& aname, kiPath& dname );
	int  v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files );
	bool v_list( const arcname& aname, aflArray& files );
	kiStr v_name(const char*) const { return "CPT"; }

	CArcProgressDlg* pdlg;

//-- cpt archive operations ----------------

	unsigned int m_nMacBinOffset;
	unsigned int m_nIndexPos;
	unsigned int dataULen, dataCLen;

	kiFile cpt,out;
	unsigned char tmp[270]; // temp

	bool read_main_hdr();
	bool check_index_crc();
	bool operation_for_each( bool o_list=true, aflArray* files=NULL );
	bool recurse( bool o_list, aflArray* files, kiPath& path, int base, int num );
	void cptmelt( bool isRL );

//-- lzh - rle -------------------------------

#define CIRCSIZE 8192
	class sf_entry
	{
	public:
		int Value;
		int BitLength;
	};
	class node
	{
	public:
		int flag, byte;
		node *one, *zero;
	};

	void cpt_outch( unsigned char ch );
	BYTE cpt_outstat,cpt_savechar;
	void cpt_rle_lzh();
	int cpt_get6bits();
	int cpt_getbit();
	void cpt_readHuff(int size,node* Hufftree);

	int gethuffbyte(node* l_nodelist);
#define SLACK 6
	node cpt_Hufftree[512 + SLACK],cpt_LZlength[128 + SLACK],cpt_LZoffs[256 + SLACK];
	unsigned char cpt_LZbuff[CIRCSIZE];
	unsigned int cpt_LZptr;
	unsigned long cpt_newbits;
	int cpt_bitsavail;
	int cpt_blocksize;
};

#endif
