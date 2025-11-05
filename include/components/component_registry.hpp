#pragma once
#include <typeindex>
#include "core/ecs.hpp"
#include "imgui.h"
#include "serialization/serializer.hpp"
#include "tools/icon_defines.hpp"

namespace Engine::Components
{
	enum ComponentFlags
	{
		NONE			= 0,
		SERIALIZABLE	= 1 << 0,	// Component serializable values get saved to snapshot and files
		INSPECTABLE		= 1 << 1,	// Component shows up in the inspector if the selected entity has this component
		ADDABLE			= 1 << 2	// Component can be added and removed from entity using the inspector
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
			ECS* ecs = Engine::ECS::Get();

			// Skip duplicates
			if (const auto& types = ecs->serializedComponentTypes;
				types.contains(std::type_index(typeid(T))))
				return;

			ecs->serializedComponentTypes.insert(std::type_index(typeid(T)));

			if (flags & SERIALIZABLE)
			{
				ecs->serializeComponents.push_back([](const SceneObject& object, Serialization::Serializer& serializer)
				{
					T* comp = TryGetComponent<T>(object);
					if (comp != nullptr)
						serializer.Serialize(*comp);
				});

				ecs->deserializeComponents.push_back([](const SceneObject& object, Serialization::Serializer& serializer, entt::registry& buffer)
				{
					if (serializer.Contains<T>())
					{
						T& c = buffer.emplace<T>(object);
						serializer.Deserialize(c);
					}
				});
			}

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
}

/// Use to register a component
#define COMPONENT(TYPE, DISPLAY_NAME, FLAGS) \
	namespace Engine::Components \
	{ \
		struct TYPE; \
		inline Component<TYPE> reg_##TYPE {DISPLAY_NAME, FLAGS}; \
	} \
	struct Engine::Components::TYPE final : Engine::Components::BaseComponent

/// Alternate way of registering components, mainly useful for components that inherit a different base component
#define REGISTER_COMPONENT(TYPE, NAME, FLAGS) \
namespace \
{ \
using namespace Engine::Components; \
Engine::Components::Component<TYPE> reg_##TYPE {NAME, FLAGS}; \
}