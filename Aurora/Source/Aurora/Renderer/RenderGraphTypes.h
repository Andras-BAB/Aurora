#pragma once
#include <cstdint>
#include <string>

#include "Texture.h"

namespace Aurora {

	using GraphResourceID = uint32_t;
	constexpr GraphResourceID INVALID_RESOURCE_ID = 0xFFFFFFFF;

	struct GraphTextureDesc {
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		std::string_view Name = "Unnamed_Texture";
	};

	struct GraphBufferDesc {
		uint32_t Size = 0;
		std::string_view Name = "Unnamed_Buffer";
	};

}
