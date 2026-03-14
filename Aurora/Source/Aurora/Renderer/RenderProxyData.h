#pragma once

#include "Aurora/Scene/MeshInstance.h"

namespace Aurora {
	struct RenderProxyData {
		uint32_t ObjectID;
		uint32_t SubmeshIndex;
		std::shared_ptr<MeshInstance> Mesh;
		math::Mat4 Transform;
	};
}
