#ifndef _PROPERTY_H
#define _PROPERTY_H

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace Configuration
{
    class Property
    {
        private:
            std::string _name;  // The property name
            std::string _value; // The property value

        public:
            Property(const std::string& name, const std::string& value)
                : _name(name), _value(value) {};

            const std::string& getPropertyName() const;
            const std::string& getPropertyValue() const;
    };
    
    typedef std::shared_ptr<Property> Property_ptr;

    class PropertyGroup
    {
        private:
            // A map where each key is the property name and value
            // a shared pointer to a property.
            std::unordered_map<std::string, Property_ptr> _properties;
            std::string _name;

        public:
            PropertyGroup(const std::string& name): _name(name) {};
            ~PropertyGroup() = default;

            void addProperty(Property_ptr prop);
            const std::string& getGroupName();
            Property_ptr getProperty(const std::string& name);
    };

    typedef std::shared_ptr<PropertyGroup> PropertyGroup_ptr;
}

#endif