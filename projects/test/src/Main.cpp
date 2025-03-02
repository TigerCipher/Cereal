#include "cereal/Cereal.h"

#include <iostream>
#include <sstream>
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

    std::string appleStr = cereal::Serialize("fruit", apple);
    std::string flagStr  = cereal::Serialize("flag", flag);
    std::string numsStr  = cereal::Serialize("array", nums);

    std::cout << appleStr << std::endl;
    std::cout << flagStr << std::endl;
    std::cout << numsStr << std::endl;
}