// ReSharper disable CppInconsistentNaming
#pragma once

#include "entt/entt.hpp"
#include "serialization/serializer.hpp"


namespace cereal
{
class JSONOutputArchive;
class JSONInputArchive;
}


namespace Engine
{
// =================================================================
// Main class for the entity component system
// Handles everything to do with components and updating systems
// It is a singleton, this is because it gets called by macros!
// =================================================================

/// Entity component system
class EntityComponentSystem
{
public:

	EntityComponentSystem() = default;


	static EntityComponentSystem* Get()
	{
		static EntityComponentSystem system;
		return &system;
	}


	static entt::registry* Registry() { return &Get()->m_registry; }

	// ======================
	//        Systems
	// ======================

	void UpdateEngineSystem( const float dt ) const
	{
		if (!m_engineUsed)
			return;
		m_engineSystemSignal.publish(dt);
	}


	void UpdateEditorSystem( const float dt ) const
	{
		if (!m_editorUsed)
			return;
		m_editorSystemSignal.publish(dt);
	}


	void UpdateGameSystem( const float dt ) const
	{
		if (!m_gameUsed)
			return;
		m_gameSystemSignal.publish(dt);
	}


	/**
	* Assign a function to be called on update in both editor and runtime
	 * @tparam Candidate Reference to the function to be assigned
	 */
	template <auto Candidate>
	void AssignEngineSystem()
	{
		m_engineSystemSink.connect<Candidate>();
		m_engineUsed = true;
	}


	/**
	 * Assign a function to be called on update in both editor and runtime
	 * @tparam Candidate Reference to the function to be assigned
	 * @tparam Type The type of the given instance
	 * @param instance The instance of a class if needed
	 */
	template <auto Candidate, typename Type>
	void AssignEngineSystem( Type* instance )
	{
		m_engineSystemSink.connect<Candidate>(instance);
		m_engineUsed = true;
	}


	/**
	* Assign a function to be called on editor time update.
	 * @tparam Candidate Reference to the function to be assigned
	 */
	template <auto Candidate>
	void AssignEditorSystem()
	{
		m_editorSystemSink.connect<Candidate>();
		m_editorUsed = true;
	}


	/**
	 * Assign a function to be called on editor time update.
	 * @tparam Candidate Reference to the function to be assigned
	 * @tparam Type The type of the given instance
	 * @param instance The instance of a class if needed
	 */
	template <auto Candidate, typename Type>
	void AssignEditorSystem( Type* instance )
	{
		m_editorSystemSink.connect<Candidate>(instance);
		m_editorUsed = true;
	}


	/**
	 * Assign a function to be called on runtime update.
	 * @tparam Candidate Reference to the function to be assigned
	 */
	template <auto Candidate>
	void AssignGameSystem()
	{
		m_gameSystemSink.connect<Candidate>();
		m_gameUsed = true;
	}


	/**
	 * Assign a function to be called on runtime update.
	 * @tparam Candidate Reference to the function to be assigned
	 * @tparam Type The type of the given instance
	 * @param instance The instance of a class if needed
	 */
	template <auto Candidate, typename Type>
	void AssignGameSystem( Type* instance )
	{
		m_gameSystemSink.connect<Candidate>(instance);
		m_gameUsed = true;
	}


	// ======================
	//       Snapshots
	// ======================

	/**
	 * Save the current state of the registry to 'snapshot' variable
	 * snapshot variable can later be used for saving and loading from file
	 */
	void SaveSnapshot();

	/**
	 * Load snapshot into registry from 'snapshot' variable
	 * @note overwrites current state of the registry completely
	 */
	void LoadSnapshot();

	// ======================
	//       Variables
	// ======================

	std::vector<std::function<void( const SceneObject&, Serialization::Serializer& )>> serializeComponents		= {};
	std::vector<std::function<void( const SceneObject&, Serialization::Serializer&, entt::registry& )>> deserializeComponents		= {};
	std::vector<std::function<void( const entt::entity& )>>              inspectableComponents      = {};
	std::vector<std::function<void( const entt::entity& )>>              addableComponents          = {};
	std::unordered_set<std::type_index>                                  serializedComponentTypes   = {};

private:

	entt::sigh<void( float )> m_engineSystemSignal;
	entt::sigh<void( float )> m_editorSystemSignal;
	entt::sigh<void( float )> m_gameSystemSignal;

	entt::sink<entt::sigh<void( float )>> m_engineSystemSink{m_engineSystemSignal};
	entt::sink<entt::sigh<void( float )>> m_editorSystemSink{m_editorSystemSignal};
	entt::sink<entt::sigh<void( float )>> m_gameSystemSink{m_gameSystemSignal};

	bool m_engineUsed = false, m_editorUsed = false, m_gameUsed = false;

	nlohmann::json m_snapshot = "";
	entt::registry m_registry = {};
};

/// Shorthand for ease of use
using ECS = EntityComponentSystem;

}


