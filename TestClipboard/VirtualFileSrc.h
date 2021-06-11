#pragma once
#include <stdint.h>
#include "DataObject.h"

namespace clipboard {

	class VirtualFileSrc : public CDataObject
	{
	public:
		VirtualFileSrc(){}
		~VirtualFileSrc(){}

		void init();

		bool set_to_clipboard();

		IFACEMETHODIMP GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);

		IFACEMETHODIMP QueryGetData(FORMATETC *pformatetc);

		IFACEMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);

	private:
		uint32_t clip_format_filedesc_ = 0;
		uint32_t clip_format_filecontent_ = 0;

	};

	STDAPI VirtualFileSrc_CreateInstance(REFIID riid, void **ppv);

};	// namespace clipboard

