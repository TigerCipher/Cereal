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
// File Name: Json
// Date File Created: 03/02/2025
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "pch.h"
#include "Json.h"
#include "Serializer.h"

namespace cereal
{

std::string JsonString::ToString() const
{
    return Serialize(mValue);
}

std::string JsonNumber::ToString() const
{
    return Serialize(mValue);
}

std::string JsonBool::ToString() const
{
    return Serialize(mValue);
}

void JsonObject::Add(const std::string& key, std::unique_ptr<JsonValue> value)
{
    if (!value)
    {
        throw std::runtime_error("Failed to add value to JsonObject because value was null");
    }
    auto result = mValues.emplace(key, std::move(value));
    if (!result.second)
    {
        throw std::runtime_error("Failed to add value to JsonObject");
    }
}

std::string JsonObject::ToString() const
{
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& [key, value] : mValues)
    {
        if (!first)
        {
            oss << ", ";
        }
        oss << "\"" << key << "\": " << value->ToString();
        first = false;
    }
    oss << "}";
    return oss.str();
}

void JsonArray::Add(std::unique_ptr<JsonValue> value)
{
    mValues.push_back(std::move(value));
}

std::string JsonArray::ToString() const
{
    std::ostringstream oss;
    oss << "[";

    for (size_t i = 0; i < mValues.size(); ++i)
    {
        oss << mValues[i]->ToString();
        if (i != mValues.size() - 1)
        {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}


} // namespace cereal