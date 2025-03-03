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

struct Vector2 : public cereal::Serializable
{
    float x = 0.0f;
    float y = 0.0f;

    [[nodiscard]] std::shared_ptr<cereal::JsonObject> Serialize() const override
    {
        auto json = std::make_shared<cereal::JsonObject>();
        json->Add("x", x);
        json->Add("y", y);
        return json;
    }
};

struct Vector3 : public cereal::Serializable
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    [[nodiscard]] std::shared_ptr<cereal::JsonObject> Serialize() const override
    {
        auto json = std::make_shared<cereal::JsonObject>();
        json->Add("x", x);
        json->Add("y", y);
        json->Add("z", z);
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

    Vector2 pos;
    pos.x = 1.0f;
    pos.y = 2.0f;

    Vector3 vec;
    vec.x = 1.76;
    vec.y = 2.7423432423770929990;
    vec.z = 3.76;

    jsonChild->Add("2dcoord", pos.Serialize());
    jsonChild->Add("3dcoord", vec.Serialize());

    jsonRoot->Add("child", jsonChild);

    std::cout << jsonRoot->ToString() << std::endl;

    int thirdNum = jsonRoot->GetObject("child").GetVector<int>("nums")[2];
    std::cout << "Third number in child: " << thirdNum << std::endl;

    double ySpeed = jsonRoot->GetObject("child").GetObject("3dcoord").Get<double>("y");
    std::cout << "Y speed: " << ySpeed << std::endl;


    // jsonRoot->PrintToFile("./sample.json", true, 8); // pretty print with 8 spaces for tabs
    jsonRoot->PrintToFile("./sample.json", true); // pretty print with default 4 spaces for tabs
}