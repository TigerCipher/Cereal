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
#include <iomanip>
#include <ostream>
#include <sstream>
#include <unordered_map>
#include <variant>

#ifndef CEREAL_FLT_PRECISION
    #define CEREAL_FLT_PRECISION std::numeric_limits<float>::digits10
#endif

#ifndef CEREAL_DBL_PRECISION
    #define CEREAL_DBL_PRECISION std::numeric_limits<double>::digits10
#endif

namespace cereal
{

template<typename T>
std::string SerializeItem(const T& obj)
{
    std::ostringstream oss;
    oss << obj;
    return oss.str();
}

template<>
inline std::string SerializeItem<std::string>(const std::string& obj)
{
    return "\"" + obj + "\"";
}

template<>
inline std::string SerializeItem<char>(const char& obj)
{
    return "\"" + std::string(1, obj) + "\"";
}

template<>
inline std::string SerializeItem<bool>(const bool& obj)
{
    return obj ? "true" : "false";
}

template<>
inline std::string SerializeItem<float>(const float& obj)
{
    std::ostringstream oss;
    oss << std::setprecision(CEREAL_FLT_PRECISION) << obj;
    return oss.str();
}

template<>
inline std::string SerializeItem<double>(const double& obj)
{
    std::ostringstream oss;
    oss << std::setprecision(CEREAL_DBL_PRECISION) << obj;
    return oss.str();
}


template<typename T>
std::string SerializeItem(const std::string_view key, const T& value)
{
    const std::string jsonValue = SerializeItem(value);
    return "\"" + std::string(key) + "\": " + jsonValue;
}

template<typename T, size_t N>
std::string SerializeArray(const std::array<T, N> arr)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < N; ++i)
    {
        oss << cereal::SerializeItem(arr[i]);
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
        oss << cereal::SerializeItem(vec[i]);
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
        oss << cereal::SerializeItem(key, value);
        if (i != map.size() - 1)
        {
            oss << ", ";
        }
        ++i;
    }
    oss << "}";
    return oss.str();
}

template<typename T>
struct Serializer
{
    static std::string Serialize(const T& obj) { return SerializeItem(obj); }
};

template<typename T>
struct Serializer<std::vector<T>>
{
    static std::string Serialize(const std::vector<T>& obj) { return SerializeVector(obj); }
};

template<typename T>
struct Serializer<std::unordered_map<std::string, T>>
{
    static std::string Serialize(const std::unordered_map<std::string, T>& obj) { return SerializeMap(obj); }
};

template<typename... Ts>
struct Serializer<std::variant<Ts...>>;

template<typename T>
std::string Serialize(const T& obj)
{
    return Serializer<T>::Serialize(obj);
}

template<typename... Ts>
struct Serializer<std::variant<Ts...>>
{
    static std::string Serialize(const std::variant<Ts...>& obj)
    {
        std::ostringstream oss;
        std::visit([&oss](const auto& val) { oss << Serialize(val); }, obj);
        return oss.str();
    }
};

} // namespace cereal