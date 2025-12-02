#ifndef OBJECTMEMBERHOLDER_H
#define OBJECTMEMBERHOLDER_H

#include <list>
#include <map>

#include "Geometry.h"
#include "Font.h"
#include "Color.h"

#include <variant>

enum Type
{
	BOOL, 
	INT, 
	FLOAT, 
	SIZE_T, 
	STRING,
	POINT2D, 
	SIZE2D, 
	COLOR, 
	FONT,
};

class Value
{
public:
	Value(bool b) : data(b) {}
	Value(int v) : data(v) {}
	Value(float f) : data(f) {}
	Value(size_t size) : data(size) {}
	Value(const std::string& s) : data(s) {}
	Value(point2D pt) : data(pt) {}
	Value(size2D size) : data(size)	{}
	Value(Color color) : data(color) {}
	Value(FontPtr font) : data(font) {}

	template<typename T>
	T getValue() const;
	friend std::ostream& operator<<(std::ostream& out, const Value& value);

	void serialize(std::ostream& out);
	void deserialize(std::istream& in);

	static Value readNextValue(std::istream& in);

private:
	std::variant<bool, int, float, size_t, std::string, 
		point2D, size2D, Color, FontPtr> data;

	template<typename T>
	void writeData(std::ostream& out, T typed_data);
	template<>
	void writeData<std::string>(std::ostream& out, std::string string);

	template<typename T>
	inline T readData(std::istream& in);
	template<>
	std::string readData(std::istream& in);
};

class ObjectMemberHolder
{
public:
	ObjectMemberHolder();
	~ObjectMemberHolder();

	template<typename T>
	void addMember(std::string member_name, T value) { m_list[member_name] = new Value(value); }

	template<typename U>
	U getMember(std::string member_name) const { 
		U result;
		auto it = m_list.find(member_name);
		if (it != m_list.end())
			result = it->second->getValue<U>();
		return result;
	}

	void serialize(std::ostream& out);
	void deserialize(std::istream& in);

private:
	std::map<std::string, Value*> m_list;
};

#endif // OBJECTMEMBERHOLDER_H