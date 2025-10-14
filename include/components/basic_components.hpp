#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "cereal/archives/json.hpp"
#include "core/ecs.hpp"


namespace glm
{
template <class Archive>
void serialize( Archive& archive, glm::vec3& v )
{
	archive(CEREAL_NVP(v.x), CEREAL_NVP(v.y), CEREAL_NVP(v.z));
}


template <class Archive>
void serialize( Archive& archive, glm::quat& q )
{
	archive(CEREAL_NVP(q.w), CEREAL_NVP(q.x), CEREAL_NVP(q.y), CEREAL_NVP(q.z));
}
}


namespace Engine::Components
{

struct Transform final : BaseComponent
{
	Transform() = default;

	// Setters
	// Position
	void SetPosition( const glm::vec3& new_position )
	{
		MarkDirty();
		position = new_position;
	}


	void OffsetPosition( const glm::vec3& new_position )
	{
		MarkDirty();
		position += new_position;
	}


	// Rotation
	void SetRotation( const glm::quat& new_rotation )
	{
		MarkDirty();
		rotation = new_rotation;
	}


	void SetEulerRotation( const glm::vec3& new_rotation )
	{
		MarkDirty();
		rotation = glm::quat(glm::radians(new_rotation));
	}


	void SetEulerRotationX( const float& degrees )
	{
		MarkDirty();
		glm::vec3 temp = GetEulerRotation();
		temp.x         = glm::radians(degrees);
		rotation       = glm::quat(temp);
	}


	void SetEulerRotationY( const float& degrees )
	{
		MarkDirty();
		glm::vec3 temp = GetEulerRotation();
		temp.y         = glm::radians(degrees);
		rotation       = glm::quat(temp);
	}


	void SetEulerRotationZ( const float& degrees )
	{
		MarkDirty();
		glm::vec3 temp = GetEulerRotation();
		temp.z         = glm::radians(degrees);
		rotation       = glm::quat(temp);
	}


	void OffsetEulerRotationX( const float& degrees )
	{
		MarkDirty();
		glm::vec3 temp = GetEulerRotation();
		temp.x += glm::radians(degrees);
		rotation = glm::quat(temp);
	}


	void OffsetEulerRotationY( const float& degrees )
	{
		MarkDirty();
		glm::vec3 temp = GetEulerRotation();
		temp.y += glm::radians(degrees);
		rotation = glm::quat(temp);
	}


	void OffsetEulerRotationZ( const float& degrees )
	{
		MarkDirty();
		glm::vec3 temp = GetEulerRotation();
		temp.z += glm::radians(degrees);
		rotation = glm::quat(temp);
	}


	void RotateX( const float& degrees ) { RotateAxis({1, 0, 0}, degrees); }
	void RotateY( const float& degrees ) { RotateAxis({0, 1, 0}, degrees); }
	void RotateZ( const float& degrees ) { RotateAxis({0, 0, 1}, degrees); }


	void RotateAxis( const glm::vec3& axis, const float& degrees )
	{
		MarkDirty();
		glm::quat rot = glm::angleAxis(glm::radians(degrees), glm::normalize(axis));
		rotation      = rot * rotation;
	}


	// Scale
	void SetScale( const glm::vec3& new_scale )
	{
		MarkDirty();
		scale = new_scale;
	}


	// Getters
	[[nodiscard]] glm::vec3 GetPosition() const { return position; }
	[[nodiscard]] glm::quat GetRotation() const { return rotation; }
	[[nodiscard]] glm::vec3 GetEulerRotation() const { return glm::eulerAngles(rotation); }
	[[nodiscard]] glm::vec3 GetScale() const { return scale; }

	[[nodiscard]] glm::vec3 GetForward() const { return glm::normalize(GetMatrix()[1]); }
	[[nodiscard]] glm::vec3 GetRight() const { return glm::normalize(glm::cross(GetForward(), {0, 0, 1})); }
	[[nodiscard]] glm::vec3 GetUp() const { return glm::normalize(glm::cross(GetRight(), GetForward())); }


	[[nodiscard]] glm::mat4 GetMatrix() const
	{
		if (dirty)
		{
			glm::mat4 model_translate = glm::translate(glm::mat4(1.0f), position);
			glm::mat4 model_rotate    = glm::mat4_cast(rotation);
			glm::mat4 model_scale     = glm::scale(glm::mat4(1.0f), scale);
			transform                 = model_translate * model_rotate * model_scale;

			dirty = false;
		}
		return transform;
	}


	// Assuming that when a reference is used, that it will be used to change the value

	glm::vec3* PositionRef()
	{
		MarkDirty();
		return &position;
	}


	glm::quat* RotationRef()
	{
		MarkDirty();
		return &rotation;
	}


	glm::vec3* ScaleRef()
	{
		MarkDirty();
		return &scale;
	}


	template <class Archive>
	void save( Archive& ar ) const
	{
		ar(CEREAL_NVP(position), CEREAL_NVP(rotation), CEREAL_NVP(scale));
	}


	template <class Archive>
	void load( Archive& ar )
	{
		MarkDirty();
		ar(CEREAL_NVP(position), CEREAL_NVP(rotation), CEREAL_NVP(scale));
	}


	template <auto Candidate>
	void SubscribeOnModify()
	{
		onModifiedUsed = true;
		onModified.connect<Candidate>();
	}


	template <auto Candidate, typename Type>
	void SubscribeOnModify( Type* instance )
	{
		onModifiedUsed = true;
		onModified.connect<Candidate>(instance);
	}


	void Inspector() override;

private:

	void MarkDirty() const
	{
		dirty = true;
	}


	entt::delegate<void()> onModified{};
	bool                   onModifiedUsed = false;

	glm::vec3 position = glm::vec3(0);
	glm::quat rotation = glm::vec3(0);
	glm::vec3 scale    = glm::vec3(1);

	mutable glm::mat4 transform = glm::identity<glm::mat4>();

	mutable bool dirty = true;
};


REGISTER_COMPONENT(Transform, ICON_AXIS_ARROW" Transform", INSPECTABLE);


struct SceneObjectInfo final : BaseComponent
{
	SceneObjectInfo() = default;

	explicit SceneObjectInfo( const std::string& objectName ) { name = objectName; }

	std::string name = "Object";


	template <typename Archive>
	void serialize( Archive& archive ) { archive(CEREAL_NVP(name)); }
};


REGISTER_COMPONENT(SceneObjectInfo, "SceneObject", NONE);


// Add to object to hide from renderer
struct Hidden
{};


struct ParentObject final : BaseComponent
{
	void RemoveChild( const entt::entity child )
	{
		children.erase(std::ranges::find(children, child));
	}


	void AddChild( const entt::entity child )
	{
		children.push_back(child);
	}


	std::vector<entt::entity> children;
};


struct ChildObject final : BaseComponent
{
	entt::entity parent = entt::null;


	template <typename Archive>
	void serialize( Archive& archive ) { archive(CEREAL_NVP(parent)); }
};


REGISTER_COMPONENT(ChildObject, "Child Object", NONE);


// Add to object to mark for delete
struct DeleteMarker
{};


}
