// ReSharper disable CppInconsistentNaming
#pragma once

#include "entt/entt.hpp"


namespace cereal
{
class JSONOutputArchive;
class JSONInputArchive;
}


namespace Helios
{
// =============================================================
// Main class for the entity component system
// Handles everything to do with components
// Also holds the registry
// It is a singleton, as it makes it easier to access
// =============================================================

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

	std::vector<std::function<void( entt::snapshot_loader&, cereal::JSONInputArchive& )>> serializedComponentsInput =
			{};
	std::vector<std::function<void( entt::snapshot&, cereal::JSONOutputArchive& )>> serializedComponentsOutput = {};
	std::vector<std::function<void( const entt::entity& )>>                         inspectableComponents      = {};
	std::vector<std::function<void( const entt::entity& )>>                         addableComponents          = {};
	std::unordered_set<std::type_index>                                             serializedComponentTypes   = {};

private:

	entt::sigh<void( float )> m_engineSystemSignal;
	entt::sigh<void( float )> m_editorSystemSignal;
	entt::sigh<void( float )> m_gameSystemSignal;

	entt::sink<entt::sigh<void( float )>> m_engineSystemSink{m_engineSystemSignal};
	entt::sink<entt::sigh<void( float )>> m_editorSystemSink{m_editorSystemSignal};
	entt::sink<entt::sigh<void( float )>> m_gameSystemSink{m_gameSystemSignal};

	bool m_engineUsed = false, m_editorUsed = false, m_gameUsed = false;

	std::stringstream m_snapshot;
	entt::registry    m_registry = {};
};


/// Shorthand for ease of use
using ECS = EntityComponentSystem;

// =============================================================
// Base classes for components
// =============================================================

namespace Components
{
	// TODO(Quillan): Maybe add serializable as an option? Since now not all components are being registered to prevent serialization of those
	enum ComponentFlags : uint8_t
	{
		NONE        = 0,
		INSPECTABLE = 1 << 0,
		// Component shows up in the inspector if the selected entity has this component
		ADDABLE = 1 << 1 // Component can be added and removed from entity using the inspector
	};


	/// Empty struct used for shorthand
	struct DefaultComponent
	{};


	/// Base class for all components
	template <typename T>
	struct Component
	{
		Component() = default;

		/// Non default constructor. Registers the component to the ECS to be used later.
		/// @note Should only ever be called via the macro REGISTER_COMPONENT()
		explicit Component( const std::string& name, const int flags = ComponentFlags::NONE )
		{
			ECS* ecs = ECS::Get();

			// Skip duplicates
			if (const auto& types = ecs->serializedComponentTypes;
				types.contains(std::type_index(typeid(T))))
				return;

			ecs->serializedComponentTypes.insert(std::type_index(typeid(T)));

			ecs->serializedComponentsInput.push_back([]( entt::snapshot_loader& s, cereal::JSONInputArchive& archive )
			{
				s.get<T>(archive);
			});
			ecs->serializedComponentsOutput.push_back([]( const entt::snapshot& s, cereal::JSONOutputArchive& archive )
			{
				s.get<T>(archive);
			});
			if (flags & ComponentFlags::INSPECTABLE)
			{
				ecs->inspectableComponents.push_back([name, flags]( const entt::entity& e )
				{
					if (T* c = ECS::Registry()->try_get<T>(e))
					{
						if (flags & ComponentFlags::ADDABLE)
						{
							const bool inspecting = ImGui::CollapsingHeader(name.c_str(),
							                                                ImGuiTreeNodeFlags_DefaultOpen |
							                                                ImGuiTreeNodeFlags_SpanAvailWidth |
							                                                ImGuiTreeNodeFlags_AllowOverlap);
							ImGui::SameLine();
							ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::GetFontSize() * 1.75f);
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
							if (ImGui::Button((ICON_TRASH_CAN"##DeleteComp_" + name).c_str()))
								ECS::Registry()->remove<T>(e);
							ImGui::PopStyleColor();
							if (inspecting)
							{
								ImGui::PushID(c);
								c->Inspector();
								ImGui::PopID();
							}
						}
						else
						{
							if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
							{
								ImGui::PushID(c);
								c->Inspector();
								ImGui::PopID();
							}
						}
					}
				});
			}

			if (flags & ComponentFlags::ADDABLE)
			{
				ecs->addableComponents.push_back([name]( const entt::entity& e )
				{
					const ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
					const auto   btnSize  = ImVec2(
							std::max(ImGui::GetContentRegionAvail().x, textSize.x + ImGui::GetFontSize()),
							textSize.y * 1.5f);

					if (!ECS::Registry()->try_get<T>(e))
					{
						if (ImGui::Button(name.c_str(), btnSize))
						{
							ECS::Registry()->emplace<T>(e);
							ImGui::CloseCurrentPopup();
						}
					}
					else
					{
						ImGui::BeginDisabled();
						ImGui::Button(name.c_str(), btnSize);
						ImGui::EndDisabled();
					}
				});
			}
		}


		virtual ~Component() = default;

		/// Override if INSPECTABLE flag is set when registering component
		virtual void Inspector() {}
	};


	/// Shorthand for component base class
	using BaseComponent = Component<DefaultComponent>;
}}


/// This macro must be placed after any class that inherits BaseComponent to make it serializable, addable and inspectable (if set)
#define REGISTER_COMPONENT(TYPE, NAME, FLAGS) \
    namespace \
    { \
        using namespace Helios::Components; \
        Helios::Components::Component<TYPE> reg_##TYPE {NAME, FLAGS}; \
    }
