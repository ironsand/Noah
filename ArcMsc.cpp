
#include "stdafx.h"
#include "ArcMsc.h"
#include "NoahApp.h"

bool CArcMsc::header( kiFile& fp, unsigned long* siz, char* ext )
{
	// 読み出す
	unsigned char p[14];
	if( 14!=fp.read(p,14) )
		return false;

	// ヘッダ構造：SZDD芋'3A [ext3rd] [orisiz(dword)]
	static const unsigned char head[9]={ 0x53,0x5A,0x44,0x44,0x88,0xF0,0x27,0x33,0x41 };//="SZDD芋'3A";

	for( int i=0; i!=9; i++ )
		if( p[i]!=head[i] )
			return false;

	if( ext )
		*ext = (char)p[9];

	DWORD x = p[10]+(p[11]<<8)+(p[12]<<16)+(p[13]<<24);
	if( siz )
		*siz=x;

	// 原理的に、16倍という圧縮率は出ない
	return (x <= fp.getSize()*16);
}

void CArcMsc::filename( char* oname, const char* ol, char ext )
{
	// コピー
	ki_strcpy( oname, ol );

	// 最後が '_' なら消しておく。
	bool bIs_=false;
	for( char* p=oname; *p; p=kiStr::next(p) ) 
		bIs_ = (*p=='_');
	if( bIs_ )
		*(--p)='\0';


	if( ext )
	{
		// 拡張子復元
		*p++ = ext;
		*p   = '\0';
	}
	else
	{
		// 拡張子自動補完
		const char* x = kiPath::ext(oname);
		if( ki_strlen(x)==2 )
		{
			::CharLower(oname);

				 if( 0==ki_strcmp(x,"ex") )(*p++)='e';
			else if( 0==ki_strcmp(x,"co") )(*p++)='m';
			else if( 0==ki_strcmp(x,"sc") )(*p++)='r';

			else if( 0==ki_strcmp(x,"dl") )(*p++)='l';
			else if( 0==ki_strcmp(x,"oc") )(*p++)='x';
			else if( 0==ki_strcmp(x,"dr") )(*p++)='v';
			else if( 0==ki_strcmp(x,"vx") )(*p++)='d';
			else if( 0==ki_strcmp(x,"38") )(*p++)='6';
			else if( 0==ki_strcmp(x,"sy") )(*p++)='s';
			else if( 0==ki_strcmp(x,"cp") )(*p++)='l';
			else if( 0==ki_strcmp(x,"li") )(*p++)='b';
			else if( 0==ki_strcmp(x,"tt") )(*p++)='f';

			else if( 0==ki_strcmp(x,"ch") )(*p++)='m';
			else if( 0==ki_strcmp(x,"hl") )(*p++)='p';
			else if( 0==ki_strcmp(x,"cn") )(*p++)='t';
			else if( 0==ki_strcmp(x,"da") )(*p++)='t';

			else if( 0==ki_strcmp(x,"tx") )(*p++)='t';
			else if( 0==ki_strcmp(x,"wr") )(*p++)='i';
			else if( 0==ki_strcmp(x,"wa") )(*p++)='v';
			else if( 0==ki_strcmp(x,"mi") )(*p++)='d';
			else if( 0==ki_strcmp(x,"rm") )(*p++)='i';
			else if( 0==ki_strcmp(x,"bm") )(*p++)='p';
			else if( 0==ki_strcmp(x,"rl") )(*p++)='e';
			else if( 0==ki_strcmp(x,"cu") )(*p++)='r';
			else if( 0==ki_strcmp(x,"do") )(*p++)='c';
			else if( 0==ki_strcmp(x,"ic") )(*p++)='o';
			else if( 0==ki_strcmp(x,"re") )(*p++)='g';
			else if( 0==ki_strcmp(x,"rt") )(*p++)='f';
			else if( 0==ki_strcmp(x,"ht") )(*p++)='m';
			else						   (*p++)='#';

			*p = '\0';
		}
	}
}

bool CArcMsc::v_check( const kiPath& aname )
{
	kiFile fp;
	if( fp.open( aname ) )
		return header( fp,NULL,NULL );
	return false;
}

bool CArcMsc::v_list( const arcname& aname, aflArray& files )
{
	kiPath fname(aname.basedir); fname+=aname.lname;

	arcfile x;
	kiFile fp;
	char ext;
	if( !fp.open( fname ) || !header( fp, &x.inf.dwOriginalSize, &ext ) )
		return false;
	ki_strcpy( x.inf.szMode, "-msc-" );
	filename( x.inf.szFileName, aname.lname, ext );
	x.inf.dwCompressedSize = fp.getSize();
	x.isfile = true;

	// TODO: 日時をどうする？

	files.add( x );
	return true;
}

int CArcMsc::v_melt( const arcname& aname, const kiPath& ddir, const aflArray* files )
{
	kiFile fp;
	kiPath fname(aname.basedir); fname+=aname.lname;
	kiPath oname(ddir);
	char tmp[MAX_PATH];

	// ヘッダ読み込み
	char ext;
	unsigned long alllen;
	if( !fp.open( fname ) || !header(fp,&alllen,&ext) )
		return 0xffff;
	fp.close();
	filename( tmp, aname.lname, ext );
	oname += tmp;

	// 書庫・出力先を開く
	OFSTRUCT of;
	of.cBytes = sizeof(of);
	int FROM = ::LZOpenFile( const_cast<char*>((const char*)fname),&of,OF_READ );
	int TO   = ::LZOpenFile( const_cast<char*>((const char*)oname),&of,OF_WRITE|OF_CREATE );
	if( FROM<0 || TO<0 )
		return 0xffff;
	// 解凍
	bool ans = (0<=::LZCopy( FROM,TO ));
	// 終了
	::LZClose( TO );
	::LZClose( FROM );
	return ans?0:0xffff;
}

int CArcMsc::v_compress( const kiPath& base, const wfdArray& files, const kiPath& ddir, int method, bool sfx )
{
	::SetCurrentDirectory( base );

	// 元ファイルを開く
	kiFile in;
	if( !in.open( files[0].cFileName ) )
		return 0xffff;

	// ヘッダ情報・圧縮先ファイル名
	char h_Ext3 = '\0';
	unsigned long h_Len = in.getSize();

	char aname[MAX_PATH];
	ki_strcpy( aname, files[0].cFileName );
	for( char *x=aname, *last=aname; *x; x=kiStr::next(x) )
		last = x;
	if( !IsDBCSLeadByte(*last) )
		h_Ext3=*last;
	*last = '_', *(last+1) = '\0';

	// 圧縮先開く
	::SetCurrentDirectory( ddir );

	// 圧縮先を開く
	kiFile out;
	if( !out.open( aname, false ) )
		return 0xffff;

	// タイムスタンプコピー
	FILETIME ct, at, mt;
	::GetFileTime(in.getHandle(), &ct, &at, &mt);
	::SetFileTime(out.getHandle(), &ct, &at, &mt);

	// ヘッダ書き込み
	unsigned char head[14]={ 0x53,0x5A,0x44,0x44,0x88,0xF0,0x27,0x33,0x41,(unsigned char)h_Ext3,
							 (unsigned char)(h_Len&0xff),(unsigned char)((h_Len>>8)&0xff),
							 (unsigned char)((h_Len>>16)&0xff),(unsigned char)((h_Len>>24)&0xff) };
	out.write( head, 14 );

	// 圧縮作業
	CArcProgressDlg dlg( h_Len, true );
	dlg.change( files[0].cFileName );
	if( !do_lzss( in, out, dlg ) )
	{
		out.close();
		::DeleteFile(aname);
		return 0x8020;
	}
	return 0;
}


//-- 12bit LZSS -----------------------------------------------//


#define N 4096 // slide窓のサイズ 2^12 bytes
#define F 18   // 最長一致長 2^(16-12)+2 bytes

static unsigned char window[N+F-1];
static int dad[N+1], lson[N+1], rson[N+257];
static int matchpos, matchlen;

static void init_tree()
{
	//-- 木を初期化

	int i;
	for( i=N+1; i<=N+256; i++ ) // root: 0x00 -- 0xff
		rson[i] = N;
	for( i=0; i<N ; i++ )
		 dad[i] = N;
}

static void insert_node( int r )
{
	//-- [位置r]から始まる[列str]を木に登録
	unsigned char* str = window + r;
	rson[r] = lson[r] = N;

	//-- ついでに一致長・位置も記録する
	matchlen = 2;

	// 一文字目でrootを選ぶ
	int i, p = N+1+str[0], cmp=1;
	for(; ;)
	{
		if( cmp >= 0 )
		{
			// 右に進む
			if( rson[p] != N )
				p = rson[p];
			// 右にはもうnodeがないのでそこに登録して終了
			else
			{
				rson[p] = r;
				 dad[r] = p;
				return;
			}
		}
		else
		{
			// 左に進む
			if( lson[p] != N )
				p = lson[p];
			// 左にはもうnodeがないのでそこに登録して終了
			else
			{
				lson[p] = r;
				 dad[r] = p;
				return;
			}
		}

		// 現在のnodeとstrを比較( i==一致長 )
		for( i=1; i<F; i++ )
			if( cmp = str[i] - window[p+i] )
				break;

		// 今まで見つけたものの中で最長だったら記憶
		if( i > matchlen )
		{
			matchpos = p;
			if( (matchlen=i) == F )
				break;
		}
	}

	// [位置p]の列と長さFで一致した場合、ここに来る
	// p の在ったところを r で置き換える

	 dad[r] =  dad[p];
	lson[r] = lson[p];
	rson[r] = rson[p];
	 dad[lson[p]] = r;
	 dad[rson[p]] = r;

	if( rson[dad[p]]==p )
		rson[dad[p]] = r;
	else
		lson[dad[p]] = r;

	dad[p] = N;
}

static void delete_node( int p )
{
	//-- [位置p]の要素を木から削除

	if( dad[p] == N ) // 既に木に入ってないのでおしまい
		return;

	int q;

	if( rson[p] == N )
		q = lson[p]; // 唯一の子を上に持ち上げる
	else if( lson[p] == N )
		q = rson[p]; // 唯一の子を上に持ち上げる
	else
	{
		q = lson[p];

		if( rson[q] != N )
		{
			// 左の枝の最右、つまり自分より一つ小さいnodeを持ち上げる
			do
				q = rson[q];
			while( rson[q] != N );

			rson[dad[q]] = lson[q];
			dad[lson[q]] = dad[q];
			lson[q] = lson[p];
			dad[lson[p]] = q;
		}
		rson[q] = rson[p];
		dad[rson[p]] = q;
	}

	dad[q] = dad[p];
	if( rson[dad[p]] == p )
		rson[dad[p]] = q;
	else
		lson[dad[p]] = q;
	dad[p] = N;
}

bool CArcMsc::do_lzss( kiFile& in, kiFile& out, CArcProgressDlg& dlg )
{
	int i, c, len, r, s;
	unsigned char code[17]={0}, mask=1, codeptr=1;

	s = 2;		// s = データ読込位置
	r = N - 16;	// r = 木への挿入位置

	// クリア
	init_tree();
	ki_memset( window+2, ' ', N-F );

	// 先頭18bytes入力
	for( len=0 ; len<F ; len++ )
	{
		if( -1 == (c = in.getc()) )
			break;
		window[r+len] = c;
	}
	window[0] = window[r+16];
	window[1] = window[r+17];
	if( len==0 )
		return true;

	// 木へ挿入
	for( i=F ; i>=0 ; i-- )
		insert_node( r-i );

	// ループ
	unsigned int total_read=18,prgr_read=0;

	do
	{
		if( prgr_read > 5000 )
		{
			dlg.change( NULL, total_read+=prgr_read );
			prgr_read-=5000;
			if( !dlg.msgloop() )
			{
				prgr_read=0xffffffff;
				break;
			}
		}

		if( matchlen > len )
			matchlen=len;

		if( matchlen < 3 )	// 一致なし
		{
			matchlen = 1;
			code[0] |= mask;
			code[codeptr++] = window[r];
		}
		else				// 一致あり
		{
			// [pos&0xff] [pos&0xf00 | len-3]
			code[codeptr++] = (unsigned char)matchpos;
			code[codeptr++] = (unsigned char)(((matchpos>>4)&0xf0) | (matchlen-3));
		}

		if( (mask<<=1)==0 ) // code が 8Block になっていたら出力
		{
			out.write( code, codeptr );
			// コードバッファ初期化
			code[0] = 0;
			codeptr = mask = 1;
		}

		// 出力した分読み込む
		int lastmatchlen = matchlen;
		for( i=0 ; i<lastmatchlen; i++ )
		{
			if( -1 == (c=in.getc()) )
				break;
			prgr_read++;

			// [位置s]に一文字書き込み
			delete_node( s );
			window[s] = c;
			if( s < F-1 ) window[ N+s ] = c;

			s = (s+1) & (N-1); // s++
			r = (r+1) & (N-1); // r++

			// 木に[位置r]のデータを挿入
			insert_node( r );
		}

		// EOF後処理
		while( i++ < lastmatchlen )
		{
			delete_node( s );
			s = (s+1) & (N-1); // s++
			r = (r+1) & (N-1); // r++
			if( --len )
				insert_node( r );
		}

	} while( len > 0 );

	if( prgr_read==0xffffffff )
		return false;

	// 8block境界に揃える
	if( mask != 1 )
	{
		while( mask<<=1 )
			code[codeptr++] = 0;
		out.write( code, codeptr );
	}

	return true;
}

