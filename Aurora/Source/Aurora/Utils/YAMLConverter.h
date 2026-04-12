#pragma once

#include <yaml-cpp/yaml.h>
#include "Aurora/Math/Math.h"

namespace YAML {

	template<>
	struct convert<math::Vec2> {
		static Node encode(const math::Vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, math::Vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<math::Vec3> {
		static Node encode(const math::Vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, math::Vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<math::Vec4> {
		static Node encode(const math::Vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, math::Vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<math::Quat> {
		static Node encode(const math::Quat& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, math::Quat& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace YAML {

	inline Emitter& operator<<(Emitter& emitter, const math::Vec2& v) {
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return emitter;
	}

	inline Emitter& operator<<(Emitter& emitter, const math::Vec3& v) {
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return emitter;
	}

	inline Emitter& operator<<(Emitter& emitter, const math::Vec4& v) {
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return emitter;
	}

	inline Emitter& operator<<(Emitter& emitter, const math::Quat& q) {
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq << q.w << q.x << q.y << q.z << YAML::EndSeq;
		return emitter;
	}
}