#pragma once

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

}
