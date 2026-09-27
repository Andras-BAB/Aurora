#pragma once

#include <filesystem>

namespace Aurora::Utils {

	// FNV-1a hashing for consistent UUID generation from paths
	constexpr uint64_t HashString(const std::string& str) {
		uint64_t hash = 0xcbf29ce484222325; // FNV offset basis
		for (char c : str) {
			hash ^= static_cast<uint64_t>(c);
			hash *= 0x100000001b3; // FNV prime
		}
		return hash;
	}

	inline std::string CanonicalizeAssetPath(const std::string& path) {
		std::error_code ec;
		std::filesystem::path absolutePath = std::filesystem::absolute(std::filesystem::path(path), ec);
		if (ec) return path;

		std::filesystem::path relativePath = std::filesystem::relative(absolutePath, std::filesystem::current_path(), ec);
		if (ec || relativePath.empty()) return absolutePath.lexically_normal().generic_string();

		return relativePath.lexically_normal().generic_string();
	}

}
