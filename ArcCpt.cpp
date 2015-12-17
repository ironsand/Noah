
#include "stdafx.h"
#include "ArcCpt.h"
#include "NoahApp.h"

//--- CArchiver --------------------------------------------------

bool CArcCpt::v_check( const kiPath& aname )
{
	bool ans = ( cpt.open( aname ) && read_main_hdr() && check_index_crc() );
	cpt.close();
	return ans;
}

bool CArcCpt::v_list( const arcname& aname, aflArray& files )
{
	::SetCurrentDirectory( aname.basedir );

	bool ans = ( cpt.open( aname.lname ) && read_main_hdr() && operation_for_each( true, &files ) );
	cpt.close();
	return ans;
}

int CArcCpt::v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files )
{
	::SetCurrentDirectory( aname.basedir );
	if( !cpt.open( aname.lname ) )
		return 0xffff;

	::SetCurrentDirectory( ddir );
	bool ans = ( read_main_hdr() && operation_for_each( false, const_cast<aflArray*>(files) ) );
	cpt.close();
	return ans ? 0 : 0x8020;
}

int CArcCpt::v_contents( const kiPath& aname, kiPath& dname )
{
	int ans=aUnknown;
	if( cpt.open( aname ) && read_main_hdr() )
	{
		if( cpt.read( tmp, 7 ) )
		{
			WORD EntryNum = (tmp[4]<<8)  + tmp[5];
			if( EntryNum == 1 )
				ans = aSingleFile;
			else
			{
				cpt.read( tmp, tmp[6] );
				tmp[0] = cpt.getc();
				if( tmp[0] & 0x80 ) // Folder
				{
					unsigned long size = (tmp[0]&0x3f) + 2;
					if( size == cpt.read( tmp+1, size ) )
						if( EntryNum == 1 + (tmp[size-1]<<8) + tmp[size] )
						{
							dname = "";
							tmp[ 1+tmp[0] ] = '\0';
							for( char* pp=(char*)tmp+1; *pp; pp=kiStr::next(pp) )
							{
								if( kiStr::isLeadByte(*pp) )
									dname += *pp, dname += *(pp+1);
								else if( *pp<' ' || *pp>'~' )
									dname += '_';
								else switch( *pp )
								{
									case '\\': case '/': case ':': case '*': 
									case '?': case '\"': case '<': case '>': case '|':
										dname += '_';
									default:
										dname += *pp;
								}
							}

							ans = aSingleDir;
						}
				}
			}
		}
	}
	cpt.close();
	return ans;
}

//--- CRC ----------------------------------------------------------

static unsigned long crctbl[256] = { 1 };

static void init_crc_table()
{
	if( crctbl[0]==1 ) // uninitialized
		for( unsigned long c,n=0; n!=256; n++ )
		{
			c = n;
			for( unsigned long k=8; k; k-- )
				c = (c&1) ? ((0xedb88320L)^(c>>1)) : (c>>1);
			crctbl[n] = c;
		}
}

static unsigned long crc( unsigned long c, unsigned char* cp,int cnt )
{
	while( cnt-- )
		c = (c>>8)^crctbl[(c&0xff)^*cp++];
	return c;
}

//--- cpt ----------------------------------------------------------

bool CArcCpt::read_main_hdr()
{
	//-- 先頭のバイトは MagicNumber : 0x01 なはず。
	m_nMacBinOffset = 0;
	if( 8 != cpt.read( tmp, 8 ) )
		return false;
	if( tmp[0] != 1 )
	{
		if(  0  != tmp[0]
		 || 120 != cpt.read( tmp, 120 )
		 ||  8  != cpt.read( tmp,  8  )
		 || tmp[0] != 1 ) // MacBinスキップ
			return false;
		m_nMacBinOffset = 128;
	}

	//-- indexまで跳ぶ
	cpt.seek( (tmp[4]<<24) + (tmp[5]<<16) + (tmp[6]<<8) + (tmp[7]) - 8 );
	return true;
}

bool CArcCpt::check_index_crc()
{
	bool folder;
	unsigned int size;

	//-- CRC(DWORD), EntryNum(WORD), CommentLen(BYTE)
	if( !cpt.read( tmp, 7 ) )
		return false;

	init_crc_table();

	DWORD     CRC = (tmp[0]<<24) + (tmp[1]<<16) + (tmp[2]<<8) + tmp[3];
	WORD EntryNum = (tmp[4]<<8)  + tmp[5];
	if( tmp[6] != cpt.read( tmp+7, tmp[6] ) )
		return false;
	DWORD chk_crc = crc( 0xffffffff, tmp+4, 3+tmp[6] );

	//-- 全ヘッダを走査してCRC計算
	for( WORD i=0; i!=EntryNum; i++ )
	{
		tmp[0] = cpt.getc();

		if( tmp[0] & 0x80 )	folder = true,  size = (tmp[0]&0x3f) + 2;
		else				folder = false, size = (tmp[0]) + 45;
		if( size != cpt.read( tmp+1, size ) ) return false;

		chk_crc = crc( chk_crc, tmp, 1+size );
    }

	return CRC == chk_crc;
}

bool CArcCpt::operation_for_each( bool o_list, aflArray* files )
{
	//-- Entry数を得る
	if( !cpt.read( tmp, 7 ) )
		return false;
	WORD EntryNum = (tmp[4]<<8)  + tmp[5];
	cpt.read( tmp, tmp[6] );

	//-- ダイアログ準備
	pdlg = files ? NULL : new CArcProgressDlg( EntryNum );

	//-- 操作
	kiPath path;
	m_nIndexPos = cpt.tell();
	bool ans = recurse( o_list, files, path, 0, EntryNum );
	delete pdlg;
	return ans;
}

bool CArcCpt::recurse( bool o_list, aflArray* files, kiPath& path, int base, int num )
{
	bool folder;
	unsigned int size;
	kiPath pthtmp;

	for( int i=0; i<num; i++ )
	{
		cpt.seekTo( m_nIndexPos );

	//-- tmpへindex読み込み

		tmp[0] = cpt.getc();
		if( tmp[0] & 0x80 )	folder = true,  size = (tmp[0]&0x3f) + 2;
		else				folder = false, size = (tmp[0]) + 45;
		if( size != cpt.read( tmp+1, size ) ) return false;
		m_nIndexPos = cpt.tell(); // 次のIndexの位置を記憶

	//-- ヘッダ解析

		//   0: filename_len (BYTE)
		// 1-n: filename     (with no '\0')
		char filename[MAX_PATH];
		ki_memcpy( filename, tmp+1, tmp[0] );
		filename[ *tmp ] = '\0';
		for( char* pp=filename; *pp; pp=kiStr::next(pp) )
		{
			if( kiStr::isLeadByte(*pp) )
				continue;
			if( *pp<' ' || *pp>'~' )
				*pp = '_';
			else switch( *pp )
			{
				case '\\': case '/': case ':': case '*': 
				case '?': case '\"': case '<': case '>': case '|':
					*pp = '_';
			}
		}
		// filenameが2個以上の.のみからなっていたら_に書き換え
		{
			int dots = 0;
			char* pp;
			for( pp=filename; *pp; pp=kiStr::next(pp) )
				if( *pp == '.' ) { ++dots; }
				else { dots=-1; break; }
			if( dots >= 2 )
				for( pp=filename; *pp; ++pp )
					*pp = '_';
		}

	//-- ダイアログ処理

		pthtmp = path, pthtmp += filename;

		if( pdlg )
		{
			pdlg->change( pthtmp, base+i+1 );
			if( !pdlg->msgloop() )
				return false;
		}

	//-- フォルダ処理

		if( folder )
		{
			unsigned long fldlen = (tmp[size-1]<<8) + tmp[size];
			pthtmp += '\\';

			if( o_list )
			{
				files->forcelen( base+i+1 );
				ki_strcpy( (*files)[base+i].inf.szFileName, pthtmp );
				(*files)[base+i].isfile = false;
			}

			if( !recurse( o_list, files, pthtmp, base+i+1, fldlen ) )
				return false;
			i += fldlen;
		}

	//-- ファイル処理

		else
		{
			const unsigned char* hdr = tmp + (size+1) - 80;

			unsigned long rsrcSkip; bool lzhFlag;
			if( (hdr[68]<<24) + (hdr[69]<<16) + (hdr[70]<<8) + hdr[71] != 0 )
			{
				// もし dataForkが存在するならば
				dataULen = (hdr[68]<<24) + (hdr[69]<<16) + (hdr[70]<<8) + hdr[71];
				dataCLen = (hdr[76]<<24) + (hdr[77]<<16) + (hdr[78]<<8) + hdr[79];
				rsrcSkip = (hdr[72]<<24) + (hdr[73]<<16) + (hdr[74]<<8) + hdr[75];
				lzhFlag  = (hdr[63]&4) !=0;
			}
			else
			{
				// dataForkが存在しないならば
				dataULen = (hdr[64]<<24) + (hdr[65]<<16) + (hdr[66]<<8) + hdr[67];
				dataCLen = (hdr[72]<<24) + (hdr[73]<<16) + (hdr[74]<<8) + hdr[75];
				rsrcSkip = 0;
				lzhFlag  = (hdr[63]&2) !=0;
			}

			if( o_list ) // リストへ加える
			{
				files->forcelen( base+i+1 );
				ki_strcpy( (*files)[base+i].inf.szFileName, pthtmp );
				ki_strcpy( (*files)[base+i].inf.szMode, lzhFlag ? "rle+lzh" : "rle" );
				(*files)[base+i].inf.dwCompressedSize = dataCLen;
				(*files)[base+i].inf.dwOriginalSize = dataULen;
				(*files)[base+i].isfile = true;
			}
			else if( !files || (*files)[base+i].selected )
			{
				if( !(hdr[63] & 1) )
				{
					pthtmp.mkdir();
					cpt.seekTo( (hdr[36]<<24) + (hdr[37]<<16) + (hdr[38]<<8) + hdr[39] +
								rsrcSkip + m_nMacBinOffset ); // filepos + rsrcCLen + (0 | 128)
					if( out.open( pthtmp, false ) )
					{
						cptmelt( lzhFlag );
						out.close();
					}
				}
			}
		}
    }

	return true;
}

//-- 解凍処理 ------------------------------------------------

#define	ESC1		0x81
#define	ESC2		0x82
#define NONESEEN	0
#define ESC1SEEN	1
#define ESC2SEEN	2

void CArcCpt::cptmelt( bool isRL )
{
	cpt_outstat = NONESEEN;
	cpt_LZptr = 0;
	cpt_blocksize = 0x1fff0;

	if( isRL )
		cpt_rle_lzh();
	else
		while( dataCLen-- )
			cpt_outch( cpt.getc() );
}

void CArcCpt::cpt_outch(unsigned char ch)
{
	cpt_LZbuff[ cpt_LZptr++ & (CIRCSIZE-1) ] = ch;

	switch( cpt_outstat )
	{
	case NONESEEN:
		if( ch==ESC1 )
			cpt_outstat = ESC1SEEN;
		else
			dataULen--,out.putc( cpt_savechar=ch );
		break;

	case ESC1SEEN:
		if( ch==ESC2 )
			cpt_outstat = ESC2SEEN;
		else
		{
			dataULen--,out.putc( cpt_savechar=ESC1 );
			if( ch!=ESC1 )
			{
				cpt_outstat = NONESEEN;
				dataULen--,out.putc( cpt_savechar=ch );
			}
		}
		break;

	case ESC2SEEN:
		cpt_outstat = NONESEEN;
		if( ch!=0 )
			while( --ch )
				dataULen--,out.putc(cpt_savechar);
		else
		{
			dataULen--,out.putc( ESC1 );
			dataULen--,out.putc( cpt_savechar=ESC2 );
		}
    }
}

void CArcCpt::cpt_rle_lzh()
{
	int block_count;
	unsigned int bptr;
	int Huffchar, LZlength, LZoffs;

	cpt_LZbuff[CIRCSIZE - 3] = 0;
	cpt_LZbuff[CIRCSIZE - 2] = 0;
	cpt_LZbuff[CIRCSIZE - 1] = 0;
	cpt_LZptr = 0;

	while( dataULen!=0 )
	{
		cpt_readHuff(256,cpt_Hufftree);
		cpt_readHuff( 64,cpt_LZlength);
		cpt_readHuff(128,cpt_LZoffs  );
		block_count = 0;
		cpt_newbits = (cpt.getc()<<8);
		cpt_newbits = cpt_newbits | cpt.getc();
		cpt_newbits = cpt_newbits << 16;
		cpt_bitsavail = 16;
		while( block_count<cpt_blocksize && dataULen!=0 )
		{
			if( cpt_getbit() )
			{
				Huffchar = gethuffbyte(cpt_Hufftree);
				cpt_outch((unsigned char)Huffchar);
				block_count += 2;
			}
			else
			{
				LZlength = gethuffbyte(cpt_LZlength);
				LZoffs = gethuffbyte(cpt_LZoffs);
				LZoffs = (LZoffs << 6) | cpt_get6bits();
				bptr = cpt_LZptr - LZoffs;
				while( LZlength-->0 )
					cpt_outch(cpt_LZbuff[bptr++&(CIRCSIZE-1)]);
				block_count += 3;
			}
		}
	}
}

int CArcCpt::gethuffbyte(node* l_nodelist)
{
	register node *np;
	np = l_nodelist;
	while(np->flag == 0)
		np = cpt_getbit() ? np->one : np->zero;
	return np->byte;
}

void CArcCpt::cpt_readHuff(int size,node* Hufftree)
{
	sf_entry tree_entry[256 + SLACK];
	int tree_entries;
	int tree_MaxLength;
	int treeBytes, i, len;
	sf_entry *ejm1;
	int j;
	sf_entry *entry;
    sf_entry tmp;
    int entries;
    unsigned a, b;
	int codelen, lvlstart, next, parents;
	int tree_count[32];

	treeBytes = cpt.getc();
	if( size<treeBytes*2 )
		return;
	for( i=0; i!=32; i++ )
		tree_count[i] = 0;
	i = 0;
	tree_MaxLength = 0;
	tree_entries = 0;
	while( treeBytes-->0 )
	{
		int c=cpt.getc();
		len = c >> 4;

		if(len != 0)
		{
			if(len > tree_MaxLength)
				tree_MaxLength = len;
			tree_count[len]++;
			tree_entry[tree_entries].Value = i;
			tree_entry[tree_entries++].BitLength = len;
		}
		i++;
		len = c & 0x0f;
		if(len != 0)
		{
			if(len > tree_MaxLength)
				tree_MaxLength = len;
			tree_count[len]++;
			tree_entry[tree_entries].Value = i;
			tree_entry[tree_entries++].BitLength = len;
		}
		i++;
	}

	j = 0;
    for( i=0; i<=tree_MaxLength; i++ )
		j = (j << 1) + tree_count[i];
	j = (1 <<tree_MaxLength) - j;
	for( i=0; i<j; i++ )
	{
		tree_entry[tree_entries].Value = size;
		tree_entry[tree_entries++].BitLength = tree_MaxLength;
	}

	entry = &(tree_entry[0]);
	entries = tree_entries;
    for( i=0; ++i<entries; )
	{
		tmp = entry[i];
		b = tmp.BitLength;
		j = i;
		while((j > 0) && ((a = (ejm1 = &(entry[j - 1]))->BitLength) >= b))
		{
			if((a == b) && (ejm1->Value <= tmp.Value))
				break;
			*(ejm1 + 1) = *ejm1;
			--j;
		}
		entry[j] = tmp;
	}

    i = tree_entries - 1;
    lvlstart = next = size * 2 + SLACK - 1;
    for(codelen = tree_MaxLength; codelen >= 1; --codelen)
	{
		while((i >= 0) && (tree_entry[i].BitLength == codelen))
		{
			Hufftree[next].byte = tree_entry[i].Value;
			Hufftree[next].flag = 1;
			next--;
			i--;
		}
		parents = next;
		if(codelen > 1)
		{
			for(j = lvlstart; j > parents + 1; j-= 2)
			{
				Hufftree[next].one = &(Hufftree[j]);
				Hufftree[next].zero = &(Hufftree[j - 1]);
				Hufftree[next].flag = 0;
				next--;
			}
		}
		lvlstart = parents;
	}
	Hufftree[0].one = &(Hufftree[next + 2]);
	Hufftree[0].zero = &(Hufftree[next + 1]);
	Hufftree[0].flag = 0;
}

int CArcCpt::cpt_get6bits()
{
	int cn,b=(cpt_newbits >> 26) & 0x3f;
	cpt_bitsavail -= 6;
	cpt_newbits <<= 6;
	if(cpt_bitsavail < 16)
	{
		cn = (cpt.getc() << 8);
		cn |= cpt.getc();
		cpt_newbits |= (cn << (16 - cpt_bitsavail));
		cpt_bitsavail += 16;
	}
	return b;
}

int CArcCpt::cpt_getbit()
{
	int b = (cpt_newbits >> 31) & 1;
	cpt_bitsavail--;
	if( cpt_bitsavail<16 )
	{
		cpt_newbits |= (cpt.getc() << 8);
		cpt_newbits |= cpt.getc();
		cpt_bitsavail += 16;
	}
	cpt_newbits <<= 1;
	return b;
}
