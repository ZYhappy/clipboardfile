#include "DataObject.h"

namespace clipboard {

	const WCHAR* const c_szText = L"Clipboard Contents";

	static void DuplicateMedium(CLIPFORMAT source_clipformat,
		STGMEDIUM* source,
		STGMEDIUM* destination) {
		switch (source->tymed) {
		case TYMED_HGLOBAL:
			destination->hGlobal =
				static_cast<HGLOBAL>(OleDuplicateData(
					source->hGlobal, source_clipformat, 0));
			break;
		case TYMED_MFPICT:
			destination->hMetaFilePict =
				static_cast<HMETAFILEPICT>(OleDuplicateData(
					source->hMetaFilePict, source_clipformat, 0));
			break;
		case TYMED_GDI:
			destination->hBitmap =
				static_cast<HBITMAP>(OleDuplicateData(
					source->hBitmap, source_clipformat, 0));
			break;
		case TYMED_ENHMF:
			destination->hEnhMetaFile =
				static_cast<HENHMETAFILE>(OleDuplicateData(
					source->hEnhMetaFile, source_clipformat, 0));
			break;
		case TYMED_FILE:
			destination->lpszFileName =
				static_cast<LPOLESTR>(OleDuplicateData(
					source->lpszFileName, source_clipformat, 0));
			break;
		case TYMED_ISTREAM:
			destination->pstm = source->pstm;
			destination->pstm->AddRef();
			break;
		case TYMED_ISTORAGE:
			destination->pstg = source->pstg;
			destination->pstg->AddRef();
			break;
		}
		destination->tymed = source->tymed;
		destination->pUnkForRelease = source->pUnkForRelease;
		if (destination->pUnkForRelease)
			destination->pUnkForRelease->AddRef();
	}

	STDMETHODIMP CDataObject::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
	{
		ZeroMemory(pmedium, sizeof(*pmedium));

		HRESULT hr = DATA_E_FORMATETC;
		if (pformatetcIn->cfFormat == CF_UNICODETEXT)
		{
			if (pformatetcIn->tymed & TYMED_HGLOBAL)
			{
				UINT cb = sizeof(c_szText[0]) * (lstrlen(c_szText) + 1);
				HGLOBAL h = GlobalAlloc(GPTR, cb);
				hr = h ? S_OK : E_OUTOFMEMORY;
				if (SUCCEEDED(hr))
				{
					StringCbCopy((PWSTR)h, cb, c_szText);
					pmedium->hGlobal = h;
					pmedium->tymed = TYMED_HGLOBAL;
				}
			}
		}
		else if (SUCCEEDED(_EnsureShellDataObject()))
		{
			hr = _pdtobjShell->GetData(pformatetcIn, pmedium);
		}
		return hr;
	}

	IFACEMETHODIMP CDataObject::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
	{
		HRESULT hr = _EnsureShellDataObject();
		if (SUCCEEDED(hr))
		{
			hr = _pdtobjShell->SetData(pformatetc, pmedium, fRelease);
		}
		return hr;
	}

	STDMETHODIMP CDataObject::QueryGetData(FORMATETC *pformatetc)
	{
		HRESULT hr = S_FALSE;
		if (pformatetc->cfFormat == CF_UNICODETEXT)
		{
			hr = S_OK;
		}
		else if (SUCCEEDED(_EnsureShellDataObject()))
		{
			hr = _pdtobjShell->QueryGetData(pformatetc);
		}
		return hr;
	}

	STDMETHODIMP CDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
	{
		*ppenumFormatEtc = NULL;
		HRESULT hr = E_NOTIMPL;
		if (dwDirection == DATADIR_GET)
		{
			FORMATETC rgfmtetc[] =
			{
				{ CF_UNICODETEXT,          NULL, 0,  0, TYMED_HGLOBAL },
			};
			hr = SHCreateStdEnumFmtEtc(ARRAYSIZE(rgfmtetc), rgfmtetc, ppenumFormatEtc);
		}
		return hr;
	}

	STDAPI CDataObject_CreateInstance(REFIID riid, void **ppv)
	{
		*ppv = NULL;
		CDataObject *p = new CDataObject();
		HRESULT hr = p ? S_OK : E_OUTOFMEMORY;
		if (SUCCEEDED(hr))
		{
			hr = p->QueryInterface(riid, ppv);
			p->Release();
		}
		return hr;
	}

}	// namespace clipboard

