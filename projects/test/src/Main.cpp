#include "cereal/Cereal.h"

#include <iostream>
#include <vector>


struct TestObject : public cereal::Serializable
{
    std::string         apple = "apple";
    bool                flag  = true;
    std::vector<int>    nums  = { 1, 2, 3, 4, 5 };
    std::array<bool, 3> bools = { true, false, true };

    [[nodiscard]] std::shared_ptr<cereal::JsonObject> Serialize() const override
    {
        auto json = std::make_shared<cereal::JsonObject>();
        json->Add("fruit", apple);
        json->Add("flag", flag);
        json->Add("nums", nums);

        return json;
    }
};


int main()
{
    TestObject obj;
    TestObject obj2;
    obj2.apple = "orange";
    obj2.flag  = false;
    obj2.nums  = { 5, 4, 3, 2, 1 };

    auto jsonRoot  = obj.Serialize();
    auto jsonChild = obj2.Serialize();

    jsonRoot->Add("child", jsonChild);

    std::cout << jsonRoot->ToString() << std::endl;

    int thirdNum = jsonRoot->GetObject("child").GetVector<int>("nums")[2];
    std::cout << "Third number in child: " << thirdNum << std::endl;
}