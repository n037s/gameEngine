#include "ObjectMemberHolder.h"

// Get values templates
template<>
bool Value::getValue() const
{
	return std::get<bool>(data);
}
template<>
int Value::getValue() const
{
	return std::get<int>(data);
}
template<>
float Value::getValue() const
{
	return std::get<float>(data);
}
template<>
size_t Value::getValue() const
{
	return std::get<size_t>(data);
}
template<>
std::string Value::getValue() const
{
	return std::get<std::string>(data);
}
template<>
point2D Value::getValue() const
{
	return std::get<point2D>(data);
}
template<>
size2D Value::getValue() const
{
	return std::get<size2D>(data);
}
template<>
Color Value::getValue() const
{
	return std::get<Color>(data);
}
template<>
FontPtr Value::getValue() const
{
	return std::get<FontPtr>(data);
}

void Value::serialize(std::ostream& out)
{
	writeData(out, data.index());
	switch (data.index())
	{
	case BOOL:
		writeData(out, getValue<bool>());
		break;
	case INT:
		writeData(out, getValue<int>());
		break;
	case FLOAT:
		writeData(out, getValue<float>());
		break;
	case SIZE_T:
		writeData(out, getValue<size_t>());
		break;
	case STRING:
		writeData(out, getValue<std::string>());
		break;
	case POINT2D:
	{
		point2D pt = getValue<point2D>();
		writeData(out, pt.y);
		writeData(out, pt.x);
		break;
	}
	case SIZE2D:
	{
		size2D size = getValue<size2D>();
		writeData(out, size.h);
		writeData(out, size.w);
		break;
	}
	case COLOR:
	{
		Color color = getValue<Color>();
		writeData(out, color.a);
		writeData(out, color.b);
		writeData(out, color.g);
		writeData(out, color.r);
		break;
	}
	case FONT:
	{
		FontPtr font = getValue<FontPtr>();
		writeData(out, font->getTextAlignement().verticalAlignement);
		writeData(out, font->getTextAlignement().horizontalAlignement);
		writeData(out, font->getPolicySize());
		writeData(out, font->getFontID());
		break;
	}
	}
}

void Value::deserialize(std::istream& in)
{
	size_t type = readData<size_t>(in);
	switch (type)
	{
	case BOOL:
		data = readData<bool>(in);
		break;
	case INT:
		data = readData<int>(in);
		break;
	case FLOAT:
		data = readData<float>(in);
		break;
	case SIZE_T:
		data = readData<size_t>(in);
		break;
	case STRING:
		data = readData<std::string>(in);
		break;
	case POINT2D:
		data = point2D(readData<float>(in), readData<float>(in));
		break;
	case SIZE2D:
		data = size2D(readData<float>(in), readData<float>(in));
		break;
	case COLOR:
		data = Color(readData<unsigned char>(in), readData<unsigned char>(in), 
				readData<unsigned char>(in), readData<unsigned char>(in));
		break;
	case FONT:
		TextAlignement txtAlignement = { readData<Alignement>(in), readData<Alignement>(in) };
		data = std::make_shared<Font>(readData<std::string>(in), readData<float>(in), txtAlignement);
		break;
	}
}

Value Value::readNextValue(std::istream& in)
{
	Value item("/0");
	item.deserialize(in);
	return item;
}

std::ostream& operator<<(std::ostream& out, const Value& value)
{
	switch (value.data.index())
	{
	case BOOL:
		out << (value.getValue<bool>())?"true":"false";
		break;
	case INT:
		out << value.getValue<int>();
		break;
	case FLOAT:
		out << value.getValue<float>();
		break;
	case SIZE_T:
		out << value.getValue<size_t>();
		break;
	case STRING:
		out << value.getValue<std::string>();
		break;
	case POINT2D:
		out << value.getValue<point2D>().x << ";" << value.getValue<point2D>().y;
		break;
	case SIZE2D:
		out << value.getValue<size2D>().w << "x" << value.getValue<size2D>().h;
		break;
	case COLOR:
		out << static_cast<int>(value.getValue<Color>().r) << ";" << static_cast<int>(value.getValue<Color>().g) << ";" <<
			static_cast<int>(value.getValue<Color>().b) << ";" << static_cast<int>(value.getValue<Color>().a);
		break;
	case FONT:
		out << "Font";
		break;
	}
	
	return out;
}

ObjectMemberHolder::ObjectMemberHolder()
{
	m_list = std::map<std::string, Value*>();
}

ObjectMemberHolder::~ObjectMemberHolder()
{
	for (auto& pair : m_list) {
		if (pair.second)
			delete pair.second;
	}
	m_list.clear();
}

void ObjectMemberHolder::serialize(std::ostream& out)
{
	Value memberCount = Value(m_list.size());
	memberCount.serialize(out);

	for (auto it = m_list.begin(); it != m_list.end(); ++it)
	{
		Value(it->first).serialize(out);
		it->second->serialize(out);
	}
}

void ObjectMemberHolder::deserialize(std::istream& in)
{
	size_t memberCount = Value::readNextValue(in).getValue<size_t>();;
	for (size_t i = 0; i < memberCount; ++i)
	{
		std::string memberName = Value::readNextValue(in).getValue<std::string>();
		std::cout << "    [" << memberName << "]" << std::endl;
		Value* memberValue = new Value("\0");
		memberValue->deserialize(in);
		std::cout << "        value = " << *memberValue << std::endl;
		m_list[memberName] = memberValue;
	}
}


template<typename T>
inline T Value::readData(std::istream& in)
{
	T output;
	in.read(reinterpret_cast<char*>(&output), sizeof(output));
	return output;
}

template<>
std::string Value::readData(std::istream& in)
{
	size_t string_length = 0;
	in.read(reinterpret_cast<char*>(&string_length), sizeof(string_length));

	std::string output(string_length, '\0');
	in.read(&output[0], string_length);

	return output;
}

template<typename T>
void Value::writeData(std::ostream& out, T typed_data)
{
	out.write(reinterpret_cast<char*>(&typed_data), sizeof(typed_data));
}

template<>
void Value::writeData(std::ostream& out, std::string string)
{
	size_t string_length = string.size();
	out.write(reinterpret_cast<char*>(&string_length), sizeof(string_length));
	out.write(string.data(), string_length);
}