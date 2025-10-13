#pragma once

namespace Engine
{

// Helper functions for the ECS

/// Adds a component of the requested type to the given object
template<typename T>
T& AddComponent(const SceneObject& object);

/// Removes the component from the given object
template<typename T>
void RemoveComponent(const SceneObject& object);

/// Get the requested component from the given object
template<typename T>
T& GetComponent(const SceneObject& object);

/// Tries to get the requested component from the given object. Returns nullptr if it does not
template<typename T>
T* TryGetComponent(const SceneObject& object);

/// Checks if the given object has the given component
template<typename T>
bool HasComponent(const SceneObject& object);

// Some helper functions for scene hierarchy

/// Sets the parent of an object to a specified object
void ReParent(const SceneObject& parent, const SceneObject& child);

/// Checks if the given child is in the hierarchy of the parent object
bool IsChildOf(const SceneObject& parent, const SceneObject& possibleChild);

/// Checks if the object is visible within the hierarchy
bool IsVisible(const SceneObject& sceneObject);

/// Sets the visibility of the object within the hierarchy
void SetVisible(const SceneObject& sceneObject, bool visible);

/// Gets the global transform matrix of the given object
glm::mat4 GetGlobalTransform(const SceneObject& sceneObject);

// Some functions for default creation of objects and other functionality for objects

/// Creates a scene object with default components
SceneObject CreateSceneObject(const std::string& name = "Object");

/// Marks an object for delete
void DestroySceneObject(const SceneObject& object);

/// Destroys all scene objects that are marked for delete
void DestroyMarkedSceneObjects();

}
