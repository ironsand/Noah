//--- K.I.LIB ---
// kl_dnd.h : drag and drop operation

#include "stdafx.h"
#include "kilib.h"

//--------------------------------------------------------------------//

bool kiDropSource::DnD( kiDataObject* pObj, DWORD efct, DWORD* pefct )
{
	kiDropSource* pDrpSrc = new kiDropSource;
	pDrpSrc->AddRef();
	pObj->begin();

	DWORD d, *p=(pefct==NULL ? &d : pefct);
	HRESULT hr = ::DoDragDrop( pObj, pDrpSrc, efct, p );

	pDrpSrc->Release();
	return (hr == DRAGDROP_S_DROP);
}

//------------------------ IUnknown Implement -------------------------//

kiDropSource::kiDropSource() : m_cRef( 0L )
{
	app()->shellInit(); // OleInitialize()
}

STDMETHODIMP_(ULONG) kiDropSource::AddRef()
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) kiDropSource::Release()
{
	if( --m_cRef )
		return m_cRef;

	delete this;
	return 0L;
}

STDMETHODIMP kiDropSource::QueryInterface( REFIID riid, void** ppObj )
{
	if( ::IsEqualIID( riid, IID_IUnknown )
	 || ::IsEqualIID( riid, IID_IDropSource ) )
	{
		*ppObj = (void*)this;
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

//------------------------ IDropSource Implement -------------------------//

STDMETHODIMP kiDropSource::GiveFeedback( DWORD dwEffect )
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

STDMETHODIMP kiDropSource::QueryContinueDrag( BOOL keyESC, DWORD keyOther )
{
	if( keyESC )
		return DRAGDROP_S_CANCEL;
	if( !(keyOther&MK_LBUTTON) && !(keyOther&MK_RBUTTON) ) 
		return DRAGDROP_S_DROP;
	return S_OK;
}

//------------------------ IUnknown Implement -------------------------//

kiDataObject::kiDataObject() : m_cRef( 0L ), m_FormatList( 2 )
{
}

STDMETHODIMP_(ULONG) kiDataObject::AddRef()
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) kiDataObject::Release()
{
	if( --m_cRef )
		return m_cRef;

	delete this;
	return 0L;
}

STDMETHODIMP kiDataObject::QueryInterface( REFIID riid, void** ppObj )
{
	if( ::IsEqualIID( riid, IID_IUnknown )
	 || ::IsEqualIID( riid, IID_IDataObject ) )
	{
		*ppObj = (void*)this;
		AddRef();
		return S_OK;
	}

	*ppObj = NULL;
	return E_NOINTERFACE;
}

//------------------------ IDataObject Implement -------------------------//


STDMETHODIMP kiDataObject::GetData( FORMATETC* fmtc, STGMEDIUM* stg )
{
	HRESULT hr = QueryGetData( fmtc );
	if( FAILED(hr) )
		return hr;
	bool res = giveData( *fmtc, stg, m_bFirst );
	m_bFirst = false;
	return res ? S_OK : STG_E_MEDIUMFULL;
}

STDMETHODIMP kiDataObject::QueryGetData( FORMATETC* fmtc )
{
	for( unsigned int i=0; i!=m_FormatList.len(); i++ )
		if( m_FormatList[i].cfFormat == fmtc->cfFormat )
//			if(fmtc & TYMED_HGLOBAL)
				return S_OK;
	return DV_E_FORMATETC;
}

//---------- Enumrator ---------//

class kiDataObject_Enum : public IEnumFORMATETC
{
public:
	kiDataObject_Enum( kiDataObject* p )
		: m_cRef( 0L ), m_pObj( p ), m_nCur( 0L )
		{
			m_pObj->AddRef();
		}
	~kiDataObject_Enum()
		{
			m_pObj->Release();
		}
	STDMETHODIMP_(ULONG) AddRef()
		{
			return ++m_cRef;
		}
	STDMETHODIMP_(ULONG) Release()
		{
			if( --m_cRef )
				return m_cRef;
			delete this;
			return 0L;
		}
	STDMETHODIMP QueryInterface( REFIID riid, void** ppObj )
		{
			if( ::IsEqualIID( riid, IID_IUnknown )
			 || ::IsEqualIID( riid, IID_IEnumFORMATETC ) )
			{
				*ppObj = (void*)this;
				AddRef();
				return S_OK;
			}
			*ppObj = NULL;
			return E_NOINTERFACE;
		}
	STDMETHODIMP Clone( IEnumFORMATETC** ppNew )
		{
			*ppNew = new kiDataObject_Enum( m_pObj );
			((kiDataObject_Enum*)(*ppNew))->m_nCur = m_nCur;
			(*ppNew)->AddRef();
			return S_OK;
		}
	STDMETHODIMP Reset()
		{
			m_nCur = 0;
			return S_OK;
		}
	STDMETHODIMP Skip( ULONG celt )
		{
			m_nCur += celt;
			if( m_pObj->m_FormatList.len() <= m_nCur )
			{
				m_nCur = m_pObj->m_FormatList.len() - 1;
				return S_FALSE;
			}
			return S_OK;
		}
	STDMETHODIMP Next( ULONG celt, FORMATETC* pFmt, ULONG* fetched )
		{
			if( fetched )
				*fetched = 0L;
			if( !pFmt )
				return E_POINTER;
			ULONG i;
			for( i=0; i < celt && m_nCur < m_pObj->m_FormatList.len(); i++,m_nCur++ )
				*pFmt++ = m_pObj->m_FormatList[m_nCur];
			if( fetched )
				*fetched = i;
			return i==celt ? S_OK : S_FALSE;
		}
private:
	ULONG         m_cRef;
	ULONG         m_nCur;
	kiDataObject* m_pObj;
};

STDMETHODIMP kiDataObject::EnumFormatEtc( DWORD drctn, IEnumFORMATETC** ppEnm )
{
	if( !ppEnm )
		return E_INVALIDARG;
	*ppEnm = NULL;
	if( drctn!=DATADIR_GET )
		return E_NOTIMPL;

	(*ppEnm = new kiDataObject_Enum( this ))->AddRef();
	return S_OK;
}
