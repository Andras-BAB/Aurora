#pragma once

#include <wrl.h>
#include <stdexcept>
#include <sstream>
#include "Aurora/Core/Log.h"

// Helper macro to HRESULT error locations
#ifdef AU_DEBUG
#define ThrowOnFail(hRes) MS::ThrowIfFailed(hRes, __FILE__, __LINE__, __func__)
#else
#define ThrowOnFail(hRes) if (FAILED(hRes)) throw MS::HrException(hRes)
#endif

namespace MS {

	// set output to UTF-8
	//SetConsoleOutputCP(CP_UTF8);
	// set input to UTF-8-ra
	// SetConsoleCP(CP_UTF8);

	// Converts the string to wstring 
	inline std::wstring stow(const std::string& string) {
		//return std::wstring(string.c_str(), string.c_str() + strlen(string.c_str()));

		if (string.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &string[0], static_cast<int>(string.size()), nullptr, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &string[0], static_cast<int>(string.size()), &wstrTo[0], size_needed);
		return wstrTo;
	}
	// Converts wstring/wchar_t* to string
	inline std::string wtos(const wchar_t* wchar) {
		std::wstring ws(wchar);

		if (ws.empty()) return std::string();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &ws[0], static_cast<int>(ws.size()), nullptr, 0, nullptr, nullptr);

		std::string strTo(size_needed, 0);

		WideCharToMultiByte(CP_UTF8, 0, &ws[0], static_cast<int>(ws.size()), &strTo[0], size_needed, nullptr, nullptr);

		return strTo;

		//return std::string(ws.begin(), ws.end());
	}
	inline std::string wtos(const std::wstring& ws) {
		//return std::string(ws.begin(), ws.end());
		
		if (ws.empty()) return std::string();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &ws[0], static_cast<int>(ws.size()), nullptr, 0, nullptr, nullptr);

		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &ws[0], static_cast<int>(ws.size()), &strTo[0], size_needed, nullptr, nullptr);

		return strTo;
	}

	// Helpers for ComPtr and HRESULT check
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	inline std::string HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return {s_str};
	}

	class HrException : public std::runtime_error
	{
	public:
		HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_Hr(hr) {  }
		HRESULT Error() const { return m_Hr; }
	private:
		const HRESULT m_Hr;
	};

	void ThrowIfFailed(HRESULT hr, const char* file, uint32_t line, const char* func);

}

