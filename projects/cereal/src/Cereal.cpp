#include "pch.h"
#include "Cereal.h"

namespace cereal
{

std::string SerializeJson(const std::unordered_map<std::string_view, std::string>& jsonMap)
{
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& [key, value] : jsonMap)
    {
        if (!first)
        {
            oss << ", ";
        }
        oss << "\"" << key << "\": " << value;
        first = false;
    }
    oss << "}";
    return oss.str();
}


} // namespace cereal