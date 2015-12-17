//--- K.I.LIB ---
// kl_file.h : file operations

#ifndef AFX_KIFILE_H__7D126C1E_3E5C_476E_9A4E_81CA8055621D__INCLUDED_
#define AFX_KIFILE_H__7D126C1E_3E5C_476E_9A4E_81CA8055621D__INCLUDED_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
// バイナリファイル操作

class kiFile
{
public: //-- static ----------------------------------------

	// ファイルサイズ取得( 名前, エラー時に返したい値 )
	static unsigned long getSize( const char* fname, unsigned long err=0xffffffff );
	static __int64 getSize64( const char* fname );

public: //-- 外向きインターフェイス --------------------------

	// 開いて閉じて
	bool open( const char* filename, bool read=true, bool create=true );
	void close();

	// 読んで書いて
	unsigned long read( unsigned char* buf, unsigned long len );
	void write( const void* buf, unsigned long len );
	int getc();
	void putc( unsigned char c );

	// シーク
	void seekTo( unsigned long pos )
		{
			if( !m_bReadMode ) flush();
			::SetFilePointer( m_hFile, pos, NULL, FILE_BEGIN );
			if(  m_bReadMode ) flush();
		}
	void seek( long pos )
		{
			if( !m_bReadMode ) flush();
			::SetFilePointer( m_hFile,
				pos-(signed)m_nBufSize+(signed)m_nBufPos, NULL,
				FILE_CURRENT );
			if(  m_bReadMode ) flush();
		}
	unsigned long tell()
		{
			return ::SetFilePointer( m_hFile, 0, NULL, FILE_CURRENT )
					- m_nBufSize + m_nBufPos;
		}

	// 情報取得
	bool isOpened()
		{
			return m_hFile != INVALID_HANDLE_VALUE;
		}
	unsigned long getSize( unsigned long* higher=NULL )
		{
			return ::GetFileSize( m_hFile, higher );
		}
	bool isEOF()
		{
			return (m_nBufPos==0 && m_nBufSize==0);
		}

public: //-- 内部処理 -----------------------------------

	kiFile() : kifile_bufsize( 65536 )
		{
			m_hFile= INVALID_HANDLE_VALUE;
			m_pBuf = new unsigned char[kifile_bufsize];
		}

	virtual ~kiFile()
		{
			close();
			delete [] m_pBuf;
		}

	HANDLE getHandle() const
		{
			return m_hFile;
		}

private:
	const int kifile_bufsize;
	void flush();

	HANDLE m_hFile;
	bool   m_bReadMode;
	unsigned char* m_pBuf;
	unsigned long  m_nBufSize, m_nBufPos;
};

#endif
