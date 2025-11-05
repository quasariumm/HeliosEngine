#pragma once

struct ImFont;


namespace Helios::Editor
{
class EditorInterface
{
public:

	EditorInterface() = default;

	~EditorInterface();


	static EditorInterface* Get()
	{
		static EditorInterface instance;
		return &instance;
	}


	static void StartFrame();

	void DrawInterfaces() const;

	static void Render();

	static void EndFrame();


	[[nodiscard]]
	ImFont* GetCodeFont() const { return m_codeFont; }


	std::vector<std::function<void()>>  interfaces{};
	std::unordered_set<std::type_index> interfaceTypes{};


	template <typename T>
	void RegisterInterface() { interfaces.push_back([] { T::Draw(); }); }


	ImFont* m_codeFont{};
};


template <typename T>
struct InterfaceHelper
{
	InterfaceHelper()
	{
		// Skip duplicates
		if (const auto types = EditorInterface::Get()->interfaceTypes;
			types.contains(std::type_index(typeid(T))))
			return;

		EditorInterface::Get()->interfaceTypes.insert(std::type_index(typeid(T)));

		EditorInterface::Get()->RegisterInterface<T>();
	}
};
}


#define REGISTER_INTERFACE(TYPE) \
    namespace { Helios::Editor::InterfaceHelper<TYPE> reg_##TYPE; }
