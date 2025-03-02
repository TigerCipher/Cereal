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

namespace cereal
{

template<typename T>
std::string Serialize(const T& obj);

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

template<>
inline std::string Serialize<int>(const int& obj)
{
    return std::to_string(obj);
}

class Serializable
{
public:
    virtual ~Serializable() = default;

    [[nodiscard]] virtual std::string Serialize() const = 0;
};

namespace detail
{
template<typename T>
std::string Serialize(const T& obj)
{
    if constexpr (std::is_base_of_v<Serializable, T>)
    {
        return obj.Serialize();
    } else
    {
        return cereal::Serialize(obj);
    }
}
} // namespace detail


template<typename T>
std::string Serialize(const std::string_view key, const T& value)
{
    const std::string jsonValue = Serialize(value);
    return "\"" + std::string(key) + "\": " + jsonValue;
}


} // namespace cereal