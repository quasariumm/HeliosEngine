#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <nlohmann/json.hpp>

#include "components/resource_types.hpp"

namespace nlohmann {

#pragma region GLM
// Serialization for glm::vec3
template <>
struct adl_serializer<glm::vec3> {
	static void to_json(json& j, const glm::vec3& v) {
		j = json::array({v.x, v.y, v.z});
	}

	static void from_json(const json& j, glm::vec3& v) {
		v.x = j.at(0).get<float>();
		v.y = j.at(1).get<float>();
		v.z = j.at(2).get<float>();
	}
};

// Serialization for glm::vec4
template <>
struct adl_serializer<glm::vec4> {
	static void to_json(json& j, const glm::vec4& v) {
		j = json::array({v.x, v.y, v.z, v.w});
	}

	static void from_json(const json& j, glm::vec4& v) {
		v.x = j.at(0).get<float>();
		v.y = j.at(1).get<float>();
		v.z = j.at(2).get<float>();
		v.w = j.at(3).get<float>();
	}
};

// Serialization for glm::quat
template <>
struct adl_serializer<glm::quat> {
	static void to_json(json& j, const glm::quat& q) {
		j = json::array({q.w, q.x, q.y, q.z});
	}

	static void from_json(const json& j, glm::quat& q) {
		q.w = j.at(0).get<float>();
		q.x = j.at(1).get<float>();
		q.y = j.at(2).get<float>();
		q.z = j.at(3).get<float>();
	}
};
#pragma endregion

#pragma region Helios

// Resource handles
template <typename T>
struct adl_serializer<Helios::Resources::ResourceHandle<T>> {
	static void to_json(json& j, const Helios::Resources::ResourceHandle<T>& v) {
		j["Path"] = v.Path();
	}

	static void from_json(const json& j, Helios::Resources::ResourceHandle<T>& v) {
		v = Helios::Resources::LoadResource<T>(j.at("Path"));
	}
};

#pragma endregion

}