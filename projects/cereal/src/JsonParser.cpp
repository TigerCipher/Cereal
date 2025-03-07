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
// File Name: JsonParser
// Date File Created: 03/06/2025
// Author: Matt
//
// ------------------------------------------------------------------------------

#include "JsonParser.h"

#include <sstream>
#include <cctype>
#include <format>
#include <iostream>

namespace cereal
{

namespace
{

std::istringstream _stream;

std::string ParseString();
JsonObject  ParseObject();
JsonValue   ParseArray();
JsonValue   ParseNumber();
JsonValue   ParseLiteral();


void SkipWhitespace()
{
    while (std::isspace(_stream.peek()))
    {
        _stream.ignore();
    }
}

JsonValue ParseValue()
{
    SkipWhitespace();
    const char ch = static_cast<char>(_stream.peek());
    std::cout << "Parsing value starting with: " << ch << std::endl; // Debugging output
    if (ch == '\"')
        return ParseString();
    if (ch == '{')
        return ParseObject();
    if (ch == '[')
        return ParseArray();
    if (std::isdigit(ch) || ch == '-')
        return ParseNumber();
    if (std::isalpha(ch))
        return ParseLiteral();
    throw std::runtime_error(std::format("Unexpected character while parsing value: {}" , ch));
}

std::string ParseString()
{
    char firstChar = _stream.get(); // Get the first character
    if (firstChar != '\"')
    {
        throw std::runtime_error("Expected opening quote for string, got: " + std::string(1, firstChar));
    }

    std::string result;
    char        ch;

    while (_stream.get(ch))
    {
        std::cout << "Reading char: " << ch << std::endl; // Debugging output

        if (ch == '\"')
            return result; // Closing quote found, return string

        if (ch == '\\')
        { // Handle escaped characters
            if (!_stream.get(ch))
                throw std::runtime_error("Invalid escape sequence");
            switch (ch)
            {
            case 'n': result += '\n'; break;
            case 't': result += '\t'; break;
            case 'r': result += '\r'; break;
            case 'b': result += '\b'; break;
            case 'f': result += '\f'; break;
            case '\\': result += '\\'; break;
            case '\"': result += '\"'; break;
            default: throw std::runtime_error("Invalid escape sequence");
            }
        } else
        {
            result += ch;
        }
    }

    throw std::runtime_error("Unterminated string");
}


JsonValue ParseNumber()
{
    std::string num;
    while (std::isdigit(_stream.peek()) || _stream.peek() == '.' || _stream.peek() == '-')
    {
        num += static_cast<char>(_stream.get());
    }
    std::cout << "Parsed number: " << num << std::endl; // Debugging output
    return std::stod(num);
}

JsonValue ParseLiteral()
{
    std::string literal;
    while (std::isalpha(_stream.peek()))
    {
        literal += static_cast<char>(_stream.get());
    }
    if (literal == "true")
        return (uint8_t)1;
    if (literal == "false")
        return (uint8_t) 0;
    if (literal == "null")
        return nullptr;
    throw std::runtime_error("Invalid literal: " + literal);
}

JsonObject ParseObject()
{
    if (_stream.get() != '{')
        throw std::runtime_error("Expected '{'");

    JsonObject obj;
    SkipWhitespace();

    while (_stream.peek() != '}')
    {
        SkipWhitespace();
        std::string key = ParseString();
        std::cout << "Key: " << key << std::endl; // Debugging output
        SkipWhitespace();
        if (_stream.get() != ':')
            throw std::runtime_error("Expected ':' after key");

        SkipWhitespace();

        JsonValue value = ParseValue();
        obj.Add(key, std::move(value));

        SkipWhitespace();

        if (_stream.peek() == ',')
            _stream.get();
        else
            break;
    }

    if (_stream.get() != '}')
        throw std::runtime_error("Expected '}'");
    return obj;
}

template<typename T>
JsonValue ParseTypedArray(const JsonValue& firstElement)
{
    std::vector<T> tempValues;
    tempValues.push_back(std::get<T>(firstElement)); // Store first parsed value

    while (true)
    {
        SkipWhitespace();

        // If next character is ']', break (end of array)
        if (_stream.peek() == ']')
        {
            _stream.get(); // Consume ']'
            break;
        }

        // Expect a comma before reading the next value
        if (_stream.get() != ',')
        {
            throw std::runtime_error("Expected ',' between array elements");
        }

        SkipWhitespace();

        // Parse the next value
        JsonValue nextValue = ParseValue();

        // Ensure type consistency
        if (!std::holds_alternative<T>(nextValue))
        {
            throw std::runtime_error("JSON array contains mixed types, which is not supported.");
        }

        tempValues.push_back(std::get<T>(nextValue));
    }

    return std::span<T>(tempValues); // Convert vector to span
}


template<>
JsonValue ParseTypedArray<bool>(const JsonValue& firstElement)
{
    std::vector<uint8_t> tempValues;
    tempValues.push_back(std::get<uint8_t>(firstElement));

    while (true)
    {
        SkipWhitespace();

        // If next character is ']', break (end of array)
        if (_stream.peek() == ']')
        {
            _stream.get(); // Consume ']'
            break;
        }

        // Expect a comma before reading the next value
        if (_stream.get() != ',')
        {
            throw std::runtime_error("Expected ',' between array elements");
        }

        SkipWhitespace();

        // Parse the next value
        JsonValue nextValue = ParseValue();

        // Ensure type consistency
        if (!std::holds_alternative<uint8_t>(nextValue))
        {
            throw std::runtime_error("JSON array contains mixed types, which is not supported.");
        }

        tempValues.push_back(std::get<uint8_t>(nextValue));
    }

    // Convert vector<bool> to vector<uint8_t> and return a span
    // auto boolStorage = std::make_shared<std::vector<uint8_t>>(std::move(tempValues));
    return std::span<uint8_t>(tempValues);
}



JsonValue ParseArray()
{
    if (_stream.get() != '[')
        throw std::runtime_error("Expected '['");

    SkipWhitespace();

    // If array is empty, return an empty span
    if (_stream.peek() == ']')
    {
        _stream.get();           // Consume ']'
        return std::span<int>(); // Defaulting to an empty span of some type
    }

    // Parse first value to determine the type
    JsonValue firstValue = ParseValue();

    SkipWhitespace();

    // Delegate parsing based on the first value type
    return std::visit(
        [](auto&& firstElement) -> JsonValue {
            using T = std::decay_t<decltype(firstElement)>;

            if constexpr (std::is_same_v<T, int>)
                return ParseTypedArray<int>(firstElement);
            else if constexpr (std::is_same_v<T, bool>) // bool is a special case
                return ParseTypedArray<bool>(firstElement);
            else if constexpr (std::is_same_v<T, double>)
                return ParseTypedArray<double>(firstElement);
            else if constexpr (std::is_same_v<T, float>)
                return ParseTypedArray<float>(firstElement);
            else if constexpr (std::is_same_v<T, unsigned char>) // seems to need to be unsigned char but we want char?
                return ParseTypedArray<unsigned char>(firstElement);
            else if constexpr (std::is_same_v<T, std::string>)
                return ParseTypedArray<std::string>(firstElement);
            else if constexpr (std::is_same_v<T, JsonObject>)
                return ParseTypedArray<JsonObject>(firstElement);
            else
                throw std::runtime_error(std::format("Unsupported array type: {}", GetTypeName<T>()));
        },
        firstValue);
}


} // anonymous namespace

JsonObject Parse(const std::string& json)
{
    _stream.clear();
    _stream.str(json);

    SkipWhitespace();

    return ParseObject();
}


} // namespace cereal