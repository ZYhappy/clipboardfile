#include "VirtualFileSrc.h"
#include <Windows.h>
#include <ShlObj.h>

namespace clipboard {

#define FAKE_FILE_SIZE 512

	void VirtualFileSrc::init()
	{
		clip_format_filedesc_ = RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
		clip_format_filecontent_ = RegisterClipboardFormat(CFSTR_FILECONTENTS);

	}

	bool VirtualFileSrc::set_to_clipboard()
	{
		return SUCCEEDED(::OleSetClipboard(this));
	}

	STDMETHODIMP VirtualFileSrc::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
	{
		ZeroMemory(pmedium, sizeof(*pmedium));

		HRESULT hr = DATA_E_FORMATETC;
		if (pformatetcIn->cfFormat == clip_format_filedesc_)
		{
			if (pformatetcIn->tymed & TYMED_HGLOBAL)
			{
				uint32_t file_count = 1;
				UINT cb = sizeof(FILEGROUPDESCRIPTOR) +	(file_count - 1) * sizeof(FILEDESCRIPTOR);
				HGLOBAL h = GlobalAlloc(GHND | GMEM_SHARE, cb);
				if (!h) {
					hr = E_OUTOFMEMORY;
				}
				else {
					FILEGROUPDESCRIPTOR* pGroupDescriptor =	(FILEGROUPDESCRIPTOR*)::GlobalLock(h);
					if (pGroupDescriptor)
					{
						FILEDESCRIPTOR* pFileDescriptorArray = (FILEDESCRIPTOR*)((LPBYTE)pGroupDescriptor + sizeof(UINT));
						pGroupDescriptor->cItems = file_count;

						for (uint32_t index = 0; index < file_count; ++index) {
							wcsncpy_s(pFileDescriptorArray[index].cFileName, _countof(pFileDescriptorArray[index].cFileName), L"TestFakeFile.txt", _TRUNCATE);
							pFileDescriptorArray[index].dwFlags = FD_FILESIZE | FD_ATTRIBUTES;
							pFileDescriptorArray[index].nFileSizeLow = FAKE_FILE_SIZE;
							pFileDescriptorArray[index].nFileSizeHigh = 0;
							pFileDescriptorArray[index].dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
						}

						::GlobalUnlock(h);

						pmedium->hGlobal = h;
						pmedium->tymed = TYMED_HGLOBAL;
						hr = S_OK;
					}
					
				}
			}
		}else if (pformatetcIn->cfFormat == clip_format_filecontent_)
		{
			if (pformatetcIn->tymed & TYMED_HGLOBAL)
			{
				HGLOBAL h = GlobalAlloc(GHND | GMEM_SHARE, FAKE_FILE_SIZE);
				if (!h) {
					hr = E_OUTOFMEMORY;
				}
				else {
					uint8_t *p_data = (uint8_t*)::GlobalLock(h);
					if (p_data)
					{
						
						memset(p_data, 'A', FAKE_FILE_SIZE);
						::GlobalUnlock(h);

						pmedium->hGlobal = h;
						pmedium->tymed = TYMED_HGLOBAL;
						hr = S_OK;
					}

				}
			}
		}
		else if (SUCCEEDED(_EnsureShellDataObject()))
		{
			hr = _pdtobjShell->GetData(pformatetcIn, pmedium);
		}
		
		return hr;
	}


	STDMETHODIMP VirtualFileSrc::QueryGetData(FORMATETC *pformatetc)
	{
		HRESULT hr = S_FALSE;
		if (pformatetc->cfFormat == clip_format_filedesc_ ||
			pformatetc->cfFormat == clip_format_filecontent_)
		{
			hr = S_OK;
		}
		else if (SUCCEEDED(_EnsureShellDataObject()))
		{
			hr = _pdtobjShell->QueryGetData(pformatetc);
		}
		
		return hr;
	}

	STDMETHODIMP VirtualFileSrc::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
	{
		*ppenumFormatEtc = NULL;
		HRESULT hr = E_NOTIMPL;
		if (dwDirection == DATADIR_GET)
		{
			FORMATETC rgfmtetc[] =
			{
				{ clip_format_filedesc_, NULL, DVASPECT_CONTENT,  -1, TYMED_HGLOBAL },
				{ clip_format_filecontent_, NULL, DVASPECT_CONTENT,  -1, TYMED_HGLOBAL },

			};
			hr = SHCreateStdEnumFmtEtc(ARRAYSIZE(rgfmtetc), rgfmtetc, ppenumFormatEtc);
		}
		return hr;
	}

	STDAPI VirtualFileSrc_CreateInstance(REFIID riid, void **ppv)
	{
		*ppv = NULL;
		VirtualFileSrc *p = new VirtualFileSrc();
		p->init();
		HRESULT hr = p ? S_OK : E_OUTOFMEMORY;
		if (SUCCEEDED(hr))
		{
			hr = p->QueryInterface(riid, ppv);
			p->Release();
		}
		return hr;
	}
};