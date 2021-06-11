#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <stdint.h>

namespace clipboard {
class DropSource : public IDropSource
{
public:
	DropSource(){}
	~DropSource(){}
	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
	{
		static const QITAB qit[] = {
			QITABENT(DropSource, IDropSource),
			{ 0 },
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&ref_);
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		long ref = InterlockedDecrement(&ref_);
		if (0 == ref)
		{
			delete this;
		}
		return ref;
	}

	// IDropSource
	virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(
		/* [annotation][in] */
		_In_  BOOL fEscapePressed,
		/* [annotation][in] */
		_In_  DWORD grfKeyState);

	virtual HRESULT STDMETHODCALLTYPE GiveFeedback(
		/* [annotation][in] */
		_In_  DWORD dwEffect);

protected:
	long ref_ = 1;
};

HRESULT DoDragDrop(uint32_t ok_effect/* = DROPEFFECT_COPY*/);

}	// namespace clipboard



