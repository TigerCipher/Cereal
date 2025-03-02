#include "cereal/Cereal.h"

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>


struct TestObject : public cereal::Serializable
{
    std::string         apple = "apple";
    bool                flag  = true;
    std::vector<int>    nums  = { 1, 2, 3, 4, 5 };
    std::array<bool, 3> bools = { true, false, true };

    [[nodiscard]] std::unique_ptr<cereal::JsonObject> Serialize() const override
    {
        std::unique_ptr<cereal::JsonObject> obj = std::make_unique<cereal::JsonObject>();
        obj->Add("fruit", std::make_unique<cereal::JsonString>(apple));
        obj->Add("flag", std::make_unique<cereal::JsonBool>(flag));

        std::unique_ptr<cereal::JsonArray> numsArray = std::make_unique<cereal::JsonArray>();
        for (const auto& num : nums)
        {
            numsArray->Add(std::make_unique<cereal::JsonNumber>(num));
        }

        obj->Add("array", std::move(numsArray));

        std::unique_ptr<cereal::JsonArray> boolsArray = std::make_unique<cereal::JsonArray>();
        for (const auto& b : bools)
        {
            boolsArray->Add(std::make_unique<cereal::JsonBool>(b));
        }

        obj->Add("bools", std::move(boolsArray));

        return std::move(obj);
    }
};


int main()
{
    TestObject obj;
    TestObject obj2;
    obj2.apple = "orange";
    obj2.flag  = false;
    obj2.nums  = { 5, 4, 3, 2, 1 };

    auto jsonRoot = obj.Serialize();
    auto jsonChild = obj2.Serialize();
    jsonRoot->Add("child", std::move(jsonChild));

    std::cout << jsonRoot->ToString() << std::endl;
}