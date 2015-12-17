//--- K.I.LIB ---
// kl_find.h : FindFirstFile wrapper

#include "stdafx.h"
#include "kilib.h"

#define isDots(p) (*p=='.' && (p[1]=='\0' || (p[1]=='.' && p[2]=='\0')))

bool kiFindFile::findfirst( const char* wild, WIN32_FIND_DATA* pfd )
{
	HANDLE xh = ::FindFirstFile( wild, pfd );
	if( xh==INVALID_HANDLE_VALUE )
		return false;
	while( isDots(pfd->cFileName) )
		if( !::FindNextFile( xh, pfd ) )
		{
			::FindClose( xh );
			return false;
		}
	::FindClose( xh );
	return true;
}

void kiFindFile::close()
{
	first=true;
	if( h!=INVALID_HANDLE_VALUE )
	{
		::FindClose( h ), h=INVALID_HANDLE_VALUE;
	}
}

bool kiFindFile::begin( const char* wild )
{
	close();

	h = ::FindFirstFile( wild, &fd );
	if( h==INVALID_HANDLE_VALUE )
		return false;
	while( isDots(fd.cFileName) )
		if( !::FindNextFile( h, &fd ) )
		{
			close();
			return false;
		}
	return true;
}

bool kiFindFile::next( WIN32_FIND_DATA* pfd )
{
	if( h==INVALID_HANDLE_VALUE )
		return false;
	if( first )
	{
		first = false;
		ki_memcpy( pfd, &fd, sizeof(fd) );
		return true;
	}
	if( !::FindNextFile( h, pfd ) )
		return false;
	while( isDots(fd.cFileName) )
		if( !::FindNextFile( h, pfd ) )
			return false;
	return true;
}

#undef isDots
