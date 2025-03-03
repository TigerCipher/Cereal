// ------------------------------------------------------------------------------
//
// Cereal
// Copyright 2025 Matthew Rogers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// File Name: Json.h
// Date File Created: 03/02/2025
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Serializer.h"

#include <unordered_map>
#include <vector>
#include <memory>
#include <variant>



#define VEC_TYPE_HELPER(T) std::vector<T>
#define MAP_TYPE_HELPER(T) std::unordered_map<std::string, T>

#define VEC_TYPES                                                                                                                \
    VEC_TYPE_HELPER(int), VEC_TYPE_HELPER(bool), VEC_TYPE_HELPER(double), VEC_TYPE_HELPER(float), VEC_TYPE_HELPER(std::string),  \
        VEC_TYPE_HELPER(std::shared_ptr<JsonObject>), VEC_TYPE_HELPER(JsonObject), VEC_TYPE_HELPER(char)

#define MAP_TYPES                                                                                                                \
    MAP_TYPE_HELPER(int), MAP_TYPE_HELPER(bool), MAP_TYPE_HELPER(double), MAP_TYPE_HELPER(float), MAP_TYPE_HELPER(std::string),  \
        MAP_TYPE_HELPER(std::shared_ptr<JsonObject>), MAP_TYPE_HELPER(JsonObject), MAP_TYPE_HELPER(char)

#define VARYING_TYPES int, bool, double, float, char, std::string, std::shared_ptr<JsonObject>, JsonObject

#define VARIANT std::variant<VARYING_TYPES, VEC_TYPES, MAP_TYPES, std::nullptr_t>


namespace cereal
{

class JsonObject;

template<>
std::string Serialize<std::shared_ptr<JsonObject>>(const std::shared_ptr<JsonObject>& obj);

class JsonObject
{
public:
    using JsonValue = VARIANT;

    JsonObject()                             = default;
    JsonObject(const JsonObject&)            = default;
    JsonObject& operator=(const JsonObject&) = default;

    void Add(const std::string& key, JsonValue value);

    void PrintToFile(const std::string_view filename, bool pretty = false, int indentSize = 4) const;

    [[nodiscard]] std::string ToString(bool pretty = false, int indentLevel = 0, int indentSize = 4) const;

    friend std::ostream& operator<<(std::ostream& os, const JsonObject& obj)
    {
        os << obj.ToString();
        return os;
    }

    [[nodiscard]] const std::unordered_map<std::string, JsonValue>& GetValues() const { return mValues; }

    JsonValue& operator[](const std::string& key) { return mValues[key]; }

    template<typename T>
    [[nodiscard]] const T& Get(const std::string& key) const
    {
        if (!mValues.contains(key))
        {
            throw std::runtime_error("Key not found in JsonObject");
        }

        return std::get<T>(mValues.at(key));
    }

    template<typename T>
    [[nodiscard]] const std::vector<T>& GetVector(const std::string& key) const
    {
        return Get<std::vector<T>>(key);
    }

    [[nodiscard]] const std::shared_ptr<JsonObject>& GetObjectPtr(const std::string& key) const
    {
        return Get<std::shared_ptr<JsonObject>>(key);
    }

    [[nodiscard]] const JsonObject& GetObject(const std::string& key) const { return *GetObjectPtr(key); }

private:
    static std::string Indent(size_t level, size_t indentSize = 4) { return std::string(level * indentSize, ' '); }

private:
    std::unordered_map<std::string, JsonValue> mValues;
};

template<>
inline std::string Serialize<std::shared_ptr<JsonObject>>(const std::shared_ptr<JsonObject>& obj)
{
    return obj->ToString();
}


} // namespace cereal