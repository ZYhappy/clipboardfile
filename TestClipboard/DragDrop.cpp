#include "DragDrop.h"
#include "VirtualFileSrcStream.h"

namespace clipboard {

	STDMETHODIMP DropSource::QueryContinueDrag(
		/* [in] */ BOOL fEscapePressed,
		/* [in] */ DWORD grfKeyState)
	{
		if (fEscapePressed)
			return DRAGDROP_S_CANCEL;
		if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON)))
		{
			return DRAGDROP_S_DROP;
		}

		return S_OK;
	}

	STDMETHODIMP DropSource::GiveFeedback(
		/* [in] */ DWORD /*dwEffect*/)
	{
		
		return DRAGDROP_S_USEDEFAULTCURSORS;
	}

	HRESULT DoDragDrop(uint32_t ok_effect)
	{
		DropSource *source = new DropSource();
		VirtualFileSrcStream *src_obj = new VirtualFileSrcStream();
		src_obj->init();
		uint32_t effect = 0;
		HRESULT hr = ::DoDragDrop(src_obj, source, ok_effect, (unsigned long*)&effect);
		source->Release();
		src_obj->Release();
		return hr;
		 
	}

}	// namespace clipboard
