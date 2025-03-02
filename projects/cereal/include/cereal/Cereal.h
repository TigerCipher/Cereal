// ------------------------------------------------------------------------------
//
// Cereal
//    Copyright 2025 Matthew Rogers
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
// File Name: Cereal
// Date File Created: 03/01/2025
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <sstream>

namespace cereal
{

template<typename T>
std::string SerializeItem(const T& obj);

std::string SerializeObject(const std::unordered_map<std::string_view, std::string>& jsonMap);


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
inline std::string SerializeItem<bool>(const bool& obj)
{
    return obj ? "true" : "false";
}


class Serializable
{
public:
    virtual ~Serializable() = default;

    [[nodiscard]] virtual std::unordered_map<std::string_view, std::string> ToJson() const = 0;
};


template<typename T>
std::string SerializeItem(const std::string_view key, const T& value)
{
    const std::string jsonValue = SerializeItem(value);
    return "\"" + std::string(key) + "\": " + jsonValue;
}


template<typename T>
std::string Serialize(const T& obj)
{
    if constexpr (std::is_base_of_v<Serializable, T>)
    {
        return cereal::SerializeObject(obj.ToJson());
    } else
    {
        return cereal::SerializeItem(obj);
    }
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

} // namespace cereal