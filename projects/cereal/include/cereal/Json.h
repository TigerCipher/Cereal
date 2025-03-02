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

namespace cereal
{

class JsonValue
{
public:
    virtual ~JsonValue() = default;
    [[nodiscard]] virtual std::string ToString() const = 0;
};

template<>
inline std::string Serialize<JsonValue>(const JsonValue& obj)
{
    return obj.ToString();
}

class JsonString : public JsonValue
{
public:
    explicit JsonString(std::string value) : mValue(std::move(value)) {}
    [[nodiscard]] std::string ToString() const override;

    [[nodiscard]] constexpr const std::string& Get() const { return mValue; }

private:
    std::string mValue;
};

class JsonNumber : public JsonValue
{
public:
    explicit JsonNumber(const double value) : mValue(value) {}
    [[nodiscard]] std::string ToString() const override;

    [[nodiscard]] constexpr double Get() const { return mValue; }

private:
    double mValue;
};

class JsonBool : public JsonValue
{
public:
    explicit JsonBool(const bool value) : mValue(value) {}
    [[nodiscard]] std::string ToString() const override;

    [[nodiscard]] constexpr bool Get() const { return mValue; }

private:
    bool mValue;
};

class JsonNull : public JsonValue
{
public:
    [[nodiscard]] std::string ToString() const override { return "null"; }
};

class JsonObject : public JsonValue
{
public:
    JsonObject()                             = default;
    JsonObject(const JsonObject&)            = delete;
    JsonObject& operator=(const JsonObject&) = delete;

    void Add(const std::string& key, std::unique_ptr<JsonValue> value);

    [[nodiscard]] std::string ToString() const override;

    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<JsonValue>>& Get() const { return mValues; }

    std::unique_ptr<JsonValue>& operator[](const std::string& key) { return mValues[key]; }

private:
    std::unordered_map<std::string, std::unique_ptr<JsonValue>> mValues;
};

class JsonArray : public JsonValue
{
public:
    JsonArray()                            = default;
    JsonArray(const JsonArray&)            = delete;
    JsonArray& operator=(const JsonArray&) = delete;

    void Add(std::unique_ptr<JsonValue> value);

    [[nodiscard]] std::string ToString() const override;

    [[nodiscard]] const std::vector<std::unique_ptr<JsonValue>>& Get() const { return mValues; }

    std::unique_ptr<JsonValue>& operator[](const size_t index) { return mValues[index]; }

private:
    std::vector<std::unique_ptr<JsonValue>> mValues;
};

} // namespace cereal