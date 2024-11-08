#include "Property.hpp"

const std::string &Configuration::Property::getPropertyName() const
{
    return _name;
}

const std::string& Configuration::Property::getPropertyValue() const
{
    return _value;
}

void Configuration::PropertyGroup::addProperty(Property_ptr prop)
{
    std::string name = prop->getPropertyName();
    _properties.insert(std::make_pair(name, prop));
}

const std::string &Configuration::PropertyGroup::getGroupName()
{
    return _name;
}

Configuration::Property_ptr Configuration::PropertyGroup::getProperty(const std::string &name)
{
    auto it = _properties.find(name);
    if (it == _properties.end()) throw std::runtime_error("Cannot find given key.");
    return it->second;
}
