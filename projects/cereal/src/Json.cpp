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

void JsonObject::PrintToFile(const std::string_view filename, bool pretty, int indentSize) const
{
    std::ofstream fs(filename.data());
    fs << ToString(pretty, 0, indentSize);
}

std::string JsonObject::ToString(bool pretty, int indentLevel, int indentSize) const
{
    std::ostringstream oss;
    std::string        indent  = pretty ? Indent(indentLevel, indentSize) : "";
    std::string        newLine = pretty ? "\n" : "";

    oss << "{" << newLine;
    bool first = true;

    for (const auto& [key, value] : mValues)
    {
        if (!first)
        {
            oss << ", " << newLine;
        }
        oss << indent << Indent(1, indentSize) << "\"" << key << "\": ";
        std::visit(
            [&oss, pretty, indentLevel, indentSize, this]<typename T>(const T& v) {
                if constexpr (std::is_same_v<std::decay_t<T>, std::shared_ptr<JsonObject>>)
                {
                    oss << v->ToString(pretty, indentLevel + 1, indentSize);
                } else if constexpr (std::is_same_v<T, JsonObject>)
                {
                    oss << v.ToString(pretty, indentLevel + 1, indentSize);
                } else
                {
                    oss << Serialize(v);
                }
            },
            value);
        first = false;
    }

    oss << newLine << indent << "}";
    return oss.str();
}


} // namespace cereal