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

#include "Serializer.h"
#include "Json.h"

namespace cereal
{

class Serializable
{
public:
    Serializable()                                         = default;
    virtual ~Serializable()                                = default;
    Serializable(const Serializable& other)                = default;
    Serializable(Serializable&& other) noexcept            = default;
    Serializable& operator=(const Serializable& other)     = default;
    Serializable& operator=(Serializable&& other) noexcept = default;

    [[nodiscard]] virtual std::shared_ptr<JsonObject> Serialize() = 0;
};

} // namespace cereal