#pragma once
#include "Aurora/Core/Log.h"

#include <fstream>
#include <string>
#include <vector>

namespace Aurora::Utils {
	inline std::vector<char> ReadBinaryFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			AU_CORE_ERROR("Failed to open file: {}", filename);
		}

		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
}
