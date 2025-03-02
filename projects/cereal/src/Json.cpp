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

void JsonObject::Add(const std::string& key, JsonValue value)
{
    mValues[key] = std::move(value);
}

std::string JsonObject::ToString() const
{
    std::ostringstream oss;

    if (mIsSingleValue)
    {
        oss << Serialize(mValue);
        return oss.str();
    }

    oss << "{";
    bool first = true;

    for (const auto& [key, value] : mValues)
    {
        if (!first)
        {
            oss << ", ";
        }
        oss << "\"" << key << "\": ";
        std::visit([&oss](const auto& v) { oss << Serialize(v); }, value);
        first = false;
    }

    oss << "}";
    return oss.str();
}


} // namespace cereal