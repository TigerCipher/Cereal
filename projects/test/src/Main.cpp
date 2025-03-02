#include "cereal/Cereal.h"

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

template<>
std::string cereal::Serialize<std::vector<int>>(const std::vector<int>& obj)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < obj.size(); ++i)
    {
        oss << cereal::Serialize(obj[i]);
        if (i != obj.size() - 1)
        {
            oss << ", ";
        }
    }
    oss << "]";
    return oss.str();
}

int main()
{
    std::string      apple = "apple";
    bool             flag  = true;
    std::vector<int> nums  = { 1, 2, 3, 4, 5 };

    std::unordered_map<std::string_view, std::string> jsonMap;

    jsonMap["fruit"] = cereal::Serialize(apple);
    jsonMap["flag"]  = cereal::Serialize(flag);
    jsonMap["array"] = cereal::Serialize(nums);

    std::string jsonStr = cereal::SerializeJson(jsonMap);

    std::cout << jsonStr << std::endl;

}