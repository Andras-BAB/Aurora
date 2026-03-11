#include "stdafx.h"
#include "MSUtils.h"

namespace MS {
	void ThrowIfFailed(HRESULT hr, const char* file, uint32_t line, const char* func) {
		if (FAILED(hr)) {
			std::stringstream err;
			err << "ERROR:\n\tFILE: " << file << " LINE: " << line << "\n\t" << HrToString(hr);
			AU_CORE_LOG_ERROR(err.str());
			throw HrException(hr);
		}
	}
}
