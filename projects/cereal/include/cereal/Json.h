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

#include "JsonException.h"
#include "Serializer.h"

#include <unordered_map>
#include <vector>
#include <memory>
#include <variant>
#include <span>
#include <typeinfo>

#pragma region Preprocessor Macros

#ifdef _DEBUG

    #if defined(__GNUG__) // GCC / Clang
        #include <cxxabi.h>
        #include <cstdlib>

        #define DEMANGLE_NAME(name)                                                                                              \
            int         status    = 0;                                                                                           \
            char*       demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);                                        \
            std::string result    = (status == 0) ? demangled : name;                                                            \
            free(demangled);                                                                                                     \
            return result

    #elif defined(_MSC_VER) // MSVC
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
        #include <dbghelp.h>


inline std::string DemangleTypeName(const char* name)
{
    if (char demangled[1024]; UnDecorateSymbolName(name, demangled, sizeof(demangled), UNDNAME_COMPLETE))
    {
        return std::string(demangled);
    }
    return name; // If demangling fails, return raw name
}

        #define DEMANGLE_NAME(name)                                                                                              \
            if (char demangled[1024]; UnDecorateSymbolName(name, demangled, sizeof(demangled), UNDNAME_COMPLETE))                \
            {                                                                                                                    \
                return std::string(demangled);                                                                                   \
            }                                                                                                                    \
            return name // If demangling fails, return raw name

    #else // Fallback
        #define DEMANGLE_NAME(name) return name
    #endif
#else
    #define DEMANGLE_NAME(name) return name
#endif

template<typename T>
std::string GetTypeName()
{
    DEMANGLE_NAME(typeid(T).name());
}


#define MAP_TYPE_HELPER(T)  std::unordered_map<std::string, T>
#define SPAN_TYPE_HELPER(T) std::span<T>
#define VEC_TYPE_HELPER(T)  std::vector<T>
#define GET_TYPE(x)         x

#define APPLY_MACRO_TO_TYPE(macro, type) macro(type)

// clang-format off
#define TYPE_VARIANTS(macro, T)                              \
    APPLY_MACRO_TO_TYPE(macro, T),                           \
    APPLY_MACRO_TO_TYPE(macro, std::shared_ptr<T>)           \

#define APPLY_MACRO(macro)                                   \
    TYPE_VARIANTS(macro, int),                               \
    TYPE_VARIANTS(macro, bool),                              \
    TYPE_VARIANTS(macro, double),                            \
    TYPE_VARIANTS(macro, float),                             \
    TYPE_VARIANTS(macro, char),                              \
    TYPE_VARIANTS(macro, std::string),                       \
    TYPE_VARIANTS(macro, JsonObject)
// clang-format on


#define TYPES APPLY_MACRO(MAP_TYPE_HELPER), APPLY_MACRO(SPAN_TYPE_HELPER), APPLY_MACRO(GET_TYPE), APPLY_MACRO(VEC_TYPE_HELPER)

#define VARIANT std::variant<TYPES, std::nullptr_t>

#pragma endregion Preprocessor Macros

namespace cereal
{

class JsonObject;
using JsonValue = VARIANT;

template<>
std::string Serialize<std::shared_ptr<JsonObject>>(const std::shared_ptr<JsonObject>& obj);

class JsonObject
{
public:
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

    template<typename T>
    [[nodiscard]] const T& Get(const std::string& key) const
    {
        const auto it = mValues.find(key);
        if (it == mValues.end())
        {
            THROW_JSON_EXCEPTION("Key not found in JsonObject: " + key);
        }

        if (!std::holds_alternative<T>(it->second))
        {
            const std::string expectedType = GetTypeName<T>();

            const std::string actualType =
                std::visit([]<typename P>(const P&) -> std::string { return GetTypeName<std::decay_t<P>>(); }, it->second);

            THROW_JSON_EXCEPTION("Type mismatch: Expected '" + expectedType + "', but found '" + actualType +
                                 "' for key: " + key);
        }

        return std::get<T>(mValues.at(key));
    }

    template<typename T>
    [[nodiscard]] const std::span<T>& GetSpan(const std::string& key) const
    {
        return Get<std::span<T>>(key);
    }

    [[nodiscard]] const std::shared_ptr<JsonObject>& GetObjectPtr(const std::string& key) const
    {
        return Get<std::shared_ptr<JsonObject>>(key);
    }

    [[nodiscard]] const JsonObject& GetObject(const std::string& key) const { return *GetObjectPtr(key); }

#pragma region Json Proxy

    class JsonProxy
    {
    public:
        JsonProxy(JsonValue& value, const std::string& key) : mValue(value), mKey(key) {}

        template<typename T>
        JsonProxy& operator=(const T& value)
        {
            mValue = value;
            return *this;
        }

        template<typename T>
        operator T() const
        {
            if (!std::holds_alternative<T>(mValue))
            {
                const std::string expectedType = GetTypeName<T>();

                 return std::visit(
                    [&]<typename AT>(AT&& value) -> T {
                        using ActualType = std::decay_t<AT>;

                        if constexpr (std::is_arithmetic_v<T> && std::is_arithmetic_v<ActualType>)
                        {
                            // Convert to the expected type instead of throwing
                            return static_cast<T>(value);
                        } else if constexpr (std::is_integral_v<T> && std::is_same_v<ActualType, char>)
                        {
                            return static_cast<T>(value); // Convert char to ASCII int
                        }
                        else if constexpr (std::is_same_v<T, char> && std::is_integral_v<ActualType>)
                        {
                            if (value < 0 || value > 255)
                                throw JsonException("Invalid conversion: Cannot convert number " + std::to_string(value) +
                                                     " to char");
                            return static_cast<char>(value);
                        }
                        else
                        {
                            // If conversion isn't possible, throw an exception
                            const std::string actualType = GetTypeName<ActualType>();
                            throw JsonException("Type mismatch: Expected '" + expectedType + "', but found '" + actualType +
                                                 "' for key: " + mKey);
                        }
                    },
                    mValue);
            }
            return std::get<T>(mValue);
        }

        JsonProxy operator[](const std::string& key) const
        {
            return JsonProxy(std::get<std::shared_ptr<JsonObject>>(mValue)->mValues[key], key);
        }

    private:
        JsonValue&         mValue;
        const std::string& mKey;
    };

#pragma endregion Json Proxy

    JsonProxy operator[](const std::string& key) { return JsonProxy(mValues[key], key); }

    const JsonProxy operator[](const std::string& key) const
    {
        const auto it = mValues.find(key);
        if (it == mValues.end())
        {
            THROW_JSON_EXCEPTION("Key not found in JsonObject: " + key);
        }
        return JsonProxy(const_cast<JsonValue&>(it->second), key);
    }

private:
    static std::string Indent(const size_t level, const size_t indentSize = 4) { return std::string(level * indentSize, ' '); }

private:
    std::unordered_map<std::string, JsonValue> mValues;
};

template<>
inline std::string Serialize<std::shared_ptr<JsonObject>>(const std::shared_ptr<JsonObject>& obj)
{
    return obj->ToString();
}


} // namespace cereal