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
// File Name: Serializer.h
// Date File Created: 03/02/2025
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <array>
#include <ostream>
#include <sstream>
#include <unordered_map>
#include <variant>

#define SERIALIZE_VECTOR_IMPL(T)                                                                                                 \
    template<>                                                                                                                   \
    inline std::string Serialize<std::vector<T>>(const std::vector<T>& obj)                                                      \
    {                                                                                                                            \
        return SerializeVector(obj);                                                                                             \
    }

#define SERIALIZE_MAP_IMPL(T)                                                                                                    \
    template<>                                                                                                                   \
    inline std::string Serialize<std::unordered_map<std::string, T>>(const std::unordered_map<std::string, T>& obj)              \
    {                                                                                                                            \
        return SerializeMap(obj);                                                                                                \
    }

namespace cereal
{

template<typename T>
std::string Serialize(const T& obj)
{
    std::ostringstream oss;
    oss << obj;
    return oss.str();
}

template<>
inline std::string Serialize<std::string>(const std::string& obj)
{
    return "\"" + obj + "\"";
}

template<>
inline std::string Serialize<bool>(const bool& obj)
{
    return obj ? "true" : "false";
}


template<typename T>
std::string Serialize(const std::string_view key, const T& value)
{
    const std::string jsonValue = Serialize(value);
    return "\"" + std::string(key) + "\": " + jsonValue;
}

template<typename T, size_t N>
std::string SerializeArray(const std::array<T, N> arr)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < N; ++i)
    {
        oss << cereal::Serialize(arr[i]);
        if (i != N - 1)
        {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}

template<typename T>
std::string SerializeVector(const std::vector<T>& vec)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        oss << cereal::Serialize(vec[i]);
        if (i != vec.size() - 1)
        {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}

template<typename T>
std::string SerializeMap(const std::unordered_map<std::string, T>& map)
{
    std::ostringstream oss;
    oss << "{";
    size_t i = 0;
    for (const auto& [key, value] : map)
    {
        oss << cereal::Serialize(key, value);
        if (i != map.size() - 1)
        {
            oss << ", ";
        }
        ++i;
    }
    oss << "}";
    return oss.str();
}

template<typename... Ts>
inline std::string Serialize(const std::variant<Ts...>& var)
{
    std::ostringstream oss;
    std::visit([&oss](const auto& val) { oss << Serialize(val); }, var);
    return oss.str();
}

SERIALIZE_VECTOR_IMPL(int)
SERIALIZE_VECTOR_IMPL(bool)
SERIALIZE_VECTOR_IMPL(float)
SERIALIZE_VECTOR_IMPL(double)
SERIALIZE_VECTOR_IMPL(std::string)

SERIALIZE_MAP_IMPL(int)
SERIALIZE_MAP_IMPL(bool)
SERIALIZE_MAP_IMPL(float)
SERIALIZE_MAP_IMPL(double)
SERIALIZE_MAP_IMPL(std::string)

} // namespace cereal