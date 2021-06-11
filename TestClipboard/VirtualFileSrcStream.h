#pragma once
#include <stdint.h>
#include "DataObject.h"

namespace clipboard {

	class FileStream : public IStream
	{
	public:

		FileStream(uint64_t file_size)
			: ref_(1)
		{
			file_size_.QuadPart = file_size;
			current_position_.QuadPart = 0;
		}

		virtual ~FileStream()
		{
			
		}

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

		ULONG STDMETHODCALLTYPE AddRef()
		{
			return InterlockedIncrement(&ref_);
		}

		ULONG STDMETHODCALLTYPE Release()
		{
			ULONG newRef = InterlockedDecrement(&ref_);
			if (newRef == 0)
			{
				delete this;
			}

			return newRef;
		}

		virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*,
			ULARGE_INTEGER*)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Commit(DWORD)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Revert(void)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Clone(IStream **)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead);

		virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);

		virtual HRESULT WINAPI Stat(STATSTG *pstatstg, DWORD grfStatFlag);


	private:
		LONG ref_;
		ULARGE_INTEGER file_size_;
		ULARGE_INTEGER current_position_;
	};

	class VirtualFileSrcStream : public CDataObject
								, public IDataObjectAsyncCapability
	{
	public:
		VirtualFileSrcStream(){}
		~VirtualFileSrcStream();

		void init();

		bool set_to_clipboard();

		IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
		{
			if (IsEqualIID(IID_IDataObjectAsyncCapability, riid)) {
				*ppv = (IDataObjectAsyncCapability*)this;
				AddRef();
				return S_OK;
			}

			/*if (IsEqualIID(IID_IOperationsProgressDialog, riid)) {
				return E_NOINTERFACE;
			}*/
			return CDataObject::QueryInterface(riid, ppv);
		}

		IFACEMETHODIMP_(ULONG) AddRef()
		{
			return CDataObject::AddRef();
		}

		IFACEMETHODIMP_(ULONG) Release()
		{
			return CDataObject::Release();
		}

		IFACEMETHODIMP GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);

		IFACEMETHODIMP QueryGetData(FORMATETC *pformatetc);

		IFACEMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);

		// IDataObjectAsyncCapability
		virtual HRESULT STDMETHODCALLTYPE SetAsyncMode(
			/* [in] */ BOOL fDoOpAsync);

		virtual HRESULT STDMETHODCALLTYPE GetAsyncMode(
			/* [out] */ __RPC__out BOOL *pfIsOpAsync);

		virtual HRESULT STDMETHODCALLTYPE StartOperation(
			/* [optional][unique][in] */ __RPC__in_opt IBindCtx *pbcReserved);

		virtual HRESULT STDMETHODCALLTYPE InOperation(
			/* [out] */ __RPC__out BOOL *pfInAsyncOp);

		virtual HRESULT STDMETHODCALLTYPE EndOperation(
			/* [in] */ HRESULT hResult,
			/* [unique][in] */ __RPC__in_opt IBindCtx *pbcReserved,
			/* [in] */ DWORD dwEffects);

	private:
		uint32_t clip_format_filedesc_ = 0;
		uint32_t clip_format_filecontent_ = 0;
		BOOL	 in_async_op_ = false;

		FileStream	*file_stream_ = nullptr;

	};

	STDAPI VirtualFileSrcStream_CreateInstance(REFIID riid, void **ppv);

};

