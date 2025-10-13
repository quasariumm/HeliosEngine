#pragma once

#include "entt/entt.hpp"

namespace cereal
{
    class JSONOutputArchive;
    class JSONInputArchive;
}

namespace Engine
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

    static entt::registry* Registry() { return &Get()->registry; }

    // ======================
    //        Systems
    // ======================

    void UpdateEngineSystem(const float dt) const { if (!engineUsed) return; engineSystemSignal.publish(dt); }
    void UpdateEditorSystem(const float dt) const { if (!editorUsed) return; editorSystemSignal.publish(dt); }
    void UpdateGameSystem  (const float dt) const { if (!gameUsed)   return; gameSystemSignal.publish(dt); }

    /**
    * Assign a function to be called on update in both editor and runtime
     * @tparam Candidate Reference to the function to be assigned
     */
    template<auto Candidate>
    void AssignEngineSystem() { engineSystemSink.connect<Candidate>(); engineUsed = true; }

    /**
     * Assign a function to be called on update in both editor and runtime
     * @tparam Candidate Reference to the function to be assigned
     * @tparam Type The type of the given instance
     * @param instance The instance of a class if needed
     */
    template<auto Candidate, typename Type>
    void AssignEngineSystem(Type* instance) { engineSystemSink.connect<Candidate>(instance); engineUsed = true; }
    
    /**
    * Assign a function to be called on editor time update.
     * @tparam Candidate Reference to the function to be assigned
     */
    template<auto Candidate>
    void AssignEditorSystem() { editorSystemSink.connect<Candidate>(); editorUsed = true; }

    /**
     * Assign a function to be called on editor time update.
     * @tparam Candidate Reference to the function to be assigned
     * @tparam Type The type of the given instance
     * @param instance The instance of a class if needed
     */
    template<auto Candidate, typename Type>
    void AssignEditorSystem(Type* instance) { editorSystemSink.connect<Candidate>(instance); editorUsed = true; }
    
    /**
     * Assign a function to be called on runtime update.
     * @tparam Candidate Reference to the function to be assigned
     */
    template<auto Candidate>
    void AssignGameSystem() { gameSystemSink.connect<Candidate>(); gameUsed = true; }

    /**
     * Assign a function to be called on runtime update.
     * @tparam Candidate Reference to the function to be assigned
     * @tparam Type The type of the given instance
     * @param instance The instance of a class if needed
     */
    template<auto Candidate, typename Type>
    void AssignGameSystem(Type* instance) { gameSystemSink.connect<Candidate>(instance); gameUsed = true; }

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

    std::vector<std::function<void(entt::snapshot_loader&, cereal::JSONInputArchive&)>> serializedComponentsInput = {};
    std::vector<std::function<void(entt::snapshot&, cereal::JSONOutputArchive&)>> serializedComponentsOutput = {};
    std::vector<std::function<void(const entt::entity&)>> inspectableComponents = {};
    std::vector<std::function<void(const entt::entity&)>> addableComponents = {};
    std::unordered_set<std::type_index> serializedComponentTypes = {};

private:
    entt::sigh<void(float)> engineSystemSignal;
    entt::sigh<void(float)> editorSystemSignal;
    entt::sigh<void(float)> gameSystemSignal;

    entt::sink<entt::sigh<void(float)>> engineSystemSink{ engineSystemSignal };
    entt::sink<entt::sigh<void(float)>> editorSystemSink{ editorSystemSignal };
    entt::sink<entt::sigh<void(float)>> gameSystemSink  { gameSystemSignal };

    bool engineUsed = false, editorUsed = false, gameUsed = false;

    std::stringstream snapshot;
    entt::registry registry = {};
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
    NONE = 0,
    INSPECTABLE = 1 << 0, // Component shows up in the inspector if the selected entity has this component
    ADDABLE = 1 << 1 // Component can be added and removed from entity using the inspector
};

/// Empty struct used for shorthand
struct DefaultComponent { };

/// Base class for all components
template <typename T>
struct Component
{
    Component() = default;

    /// Non default constructor. Registers the component to the ECS to be used later.
    /// @note Should only ever be called via the macro REGISTER_COMPONENT()
    explicit Component(const std::string& name, const int flags = ComponentFlags::NONE)
    {
        ECS* ecs = ECS::Get();

        // Skip duplicates
        auto& types = ecs->serializedComponentTypes;
        if (types.find(std::type_index(typeid(T))) != types.end()) return;
        
        ecs->serializedComponentTypes.insert(std::type_index(typeid(T)));
        
        ecs->serializedComponentsInput.push_back([](entt::snapshot_loader& s, cereal::JSONInputArchive& archive) { s.get<T>(archive); });
        ecs->serializedComponentsOutput.push_back([](entt::snapshot& s, cereal::JSONOutputArchive& archive) { s.get<T>(archive); });
        if (flags & ComponentFlags::INSPECTABLE)
        {
            ecs->inspectableComponents.push_back([name, flags](const entt::entity& e)
            {
                T* c = ECS::Get()->Registry()->try_get<T>(e);
                if (c)
                {
                    if (flags & ComponentFlags::ADDABLE)
                    {
                        bool inspecting = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap);
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::GetFontSize() * 1.75f);
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                        if (ImGui::Button((ICON_TRASH_CAN"##DeleteComp_" + name).c_str()))
                            ECS::Get()->Registry()->remove<T>(e);
                        ImGui::PopStyleColor();
                        if (inspecting) c->Inspector();
                    }
                    else
                    {
                        if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                            c->Inspector();
                    }
                }
            });
        }

        if (flags & ComponentFlags::ADDABLE)
        {
            ecs->addableComponents.push_back([name](const entt::entity& e)
            {
                ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
                ImVec2 btnSize = ImVec2(std::max(ImGui::GetContentRegionAvail().x, textSize.x + ImGui::GetFontSize()), textSize.y * 1.5f);
                
                if (!ECS::Get()->Registry()->try_get<T>(e))
                {
                    if (ImGui::Button(name.c_str(), btnSize))
                    {
                        ECS::Get()->Registry()->emplace<T>(e);
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

}

/// This macro must be placed after any class that inherits BaseComponent to make it serializable, addable and inspectable (if set)
#define REGISTER_COMPONENT(TYPE, NAME, FLAGS) \
    namespace \
    { \
        using namespace Engine::Components; \
        Engine::Components::Component<TYPE> reg_##TYPE {NAME, FLAGS}; \
    }
