#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "component_registry.hpp"

#include <visit_struct/visit_struct_intrusive.hpp>

// Include is necessary for certain types to be serializable!
#include "serialization/serializer_types.hpp"

COMPONENT(Transform, ICON_AXIS_ARROW" Transform", SERIALIZABLE | INSPECTABLE)
{
	Transform()
	{
		position	= glm::vec3(0.0f);
		rotation	= glm::identity<glm::quat>();
		scale		= glm::vec3(1.0f);
	}

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
		const glm::quat rot = glm::angleAxis(glm::radians(degrees), glm::normalize(axis));
		rotation            = rot * rotation;
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
			const glm::mat4 modelTranslate = glm::translate(glm::mat4(1.0f), position);
			const glm::mat4 modelRotate    = glm::mat4_cast(rotation);
			const glm::mat4 modelScale     = glm::scale(glm::mat4(1.0f), scale);
			transform                      = modelTranslate * modelRotate * modelScale;

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

	friend class visitable;

	void MarkDirty() const
	{
		dirty = true;
	}

	entt::delegate<void()> onModified{};
	bool                   onModifiedUsed = false;

	BEGIN_VISITABLES(Transform);
	VISITABLE(glm::vec3, position);
	VISITABLE(glm::quat, rotation);
	VISITABLE(glm::vec3, scale);
	END_VISITABLES;

	mutable glm::mat4 transform = glm::identity<glm::mat4>();

	mutable bool dirty = true;
};

COMPONENT(SceneObjectInfo, "SceneObject", SERIALIZABLE)
{
	SceneObjectInfo() = default;

	explicit SceneObjectInfo( const std::string& objectName ) { name = objectName; }

	std::string name = "Object";
};

VISITABLE_STRUCT(Helios::Components::SceneObjectInfo, name);

COMPONENT(ParentObject, "Parent Object", SERIALIZABLE)
{
	void RemoveChild( const SceneObject child )
	{
		children.erase(std::ranges::find(children, child));
	}

	void AddChild( const SceneObject child )
	{
		children.push_back(child);
	}

	std::vector<SceneObject> children;
};

VISITABLE_STRUCT(Helios::Components::ParentObject, children);


COMPONENT(ChildObject, "Child Object", SERIALIZABLE)
{
	SceneObject parent = entt::null;
};

VISITABLE_STRUCT(Helios::Components::ChildObject, parent);

namespace Helios::Components
{

// Add to object to mark for delete
struct DeleteMarker {};
// Add to object to hide from renderer
struct Hidden {};

}
