#include "cereal/Cereal.h"

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>


struct TestObject : public cereal::Serializable
{
    std::string      apple = "apple";
    bool             flag  = true;
    std::vector<int> nums  = { 1, 2, 3, 4, 5 };
    std::array<bool, 3> bools = { true, false, true };

    [[nodiscard]] std::unordered_map<std::string_view, std::string> ToJson() const override
    {
        std::unordered_map<std::string_view, std::string> jsonMap;
        jsonMap["fruit"] = cereal::SerializeItem(apple);
        jsonMap["flag"]  = cereal::SerializeItem(flag);
        jsonMap["array"] = cereal::SerializeVector(nums);
        jsonMap["bools"] = cereal::SerializeArray(bools);
        return jsonMap;
    }
};


int main()
{
    TestObject obj;

    std::string jsonStr2 = cereal::Serialize(obj);
    std::cout << jsonStr2 << std::endl;

    std::vector<TestObject> objects;
    objects.reserve(5);
    objects.emplace_back();
    objects.emplace_back();
    objects.emplace_back();

    std::string jsonStr3 = cereal::SerializeVector(objects);
    std::cout << jsonStr3 << std::endl;

}