#pragma once

#include "nlohmann/json.hpp"
#include "visit_struct/visit_struct.hpp"

namespace Engine::Serialization
{

class Serializer
{
public:
	Serializer() = default;
	Serializer(const nlohmann::json& json) { m_buffer = json; }

	/// Serialize the given type to the internal buffer of the serializer
	template<typename Obj>
	std::enable_if_t<visit_struct::traits::is_visitable<Obj>::value, void>
	Serialize(const Obj& object)
	{
		std::string typeName = visit_struct::get_name(object);
		visit_struct::for_each(object, [this, typeName](const char * name, const auto & value)
		{
			m_buffer[typeName][name] = value;
		});
	}

	/// Deserialize the internal buffer into the given object
	template<typename Obj>
	std::enable_if_t<visit_struct::traits::is_visitable<Obj>::value, void>
	Deserialize(Obj& object)
	{
		std::string typeName = visit_struct::get_name(object);
		visit_struct::for_each(object, [this, typeName](const char* name, auto& value)
		{
			value = m_buffer[typeName][name].get<std::decay_t<decltype(value)>>();
		});
	}

	/// Check if the given type exists within the buffer
	template<typename Obj>
	std::enable_if_t<visit_struct::traits::is_visitable<Obj>::value, bool>
	Contains()
	{
		return m_buffer.contains(visit_struct::get_name<Obj>());
	}

	// Versions for when the object is not visitable

	template<typename Obj>
	std::enable_if_t<!visit_struct::traits::is_visitable<Obj>::value, void>
	Serialize(const Obj& object) { }

	template<typename Obj>
	std::enable_if_t<!visit_struct::traits::is_visitable<Obj>::value, void>
	Deserialize(Obj& object) { }

	template<typename Obj>
	std::enable_if_t<!visit_struct::traits::is_visitable<Obj>::value, bool>
	Contains() { return false; }

	nlohmann::json GetBuffer() const { return m_buffer; }

private:
	nlohmann::json m_buffer;
};

}

