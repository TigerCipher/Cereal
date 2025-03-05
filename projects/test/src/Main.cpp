#include "cereal/Cereal.h"

#include <iostream>
#include <vector>


struct TestObject : public cereal::Serializable
{
    std::string          apple     = "apple";
    bool                 flag      = true;
    std::vector<int>     nums      = { 1, 2, 3, 4, 5 };
    std::array<float, 5> spanArray = { 1.5f, 2.4f, 3.0f, 4.2f, 5.0f };
    std::span<float>     span      = spanArray;
    std::span<int>       numSpan   = nums;
    std::array<bool, 3>  bools     = { true, false, true }; // TODO: Add support for std::array
    char                 c         = 'b';

    [[nodiscard]] std::shared_ptr<cereal::JsonObject> Serialize() const override
    {
        cereal::JsonObject json;
        json.Add("fruit", apple);
        json.Add("flag", flag);
        json.Add("nums", nums);
        json.Add("name", "John Doe");
        json.Add("null", nullptr);
        json.Add("span", span);

        // You can also add like this
        json["spannum"] = numSpan;
        json["letter"]  = c;

        return std::make_shared<cereal::JsonObject>(json);
    }
};

struct Vector2 : public cereal::Serializable
{
    float x = 0.0f;
    float y = 0.0f;

    Vector2() = default;
    Vector2(float xx, float yy) : x(xx), y(yy) {}

    [[nodiscard]] std::shared_ptr<cereal::JsonObject> Serialize() const override
    {
        cereal::JsonObject json;
        json.Add("x", x);
        json.Add("y", y);
        return std::make_shared<cereal::JsonObject>(json);
    }
};

struct Vector3 : public cereal::Serializable
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vector2 pos{ 3.4f, 2.f };

    [[nodiscard]] std::shared_ptr<cereal::JsonObject> Serialize() const override
    {
        cereal::JsonObject json;
        json["x"] = x;
        json["y"] = y;
        json["z"] = z;

        json["pos"] = pos.Serialize();

        return std::make_shared<cereal::JsonObject>(json);
    }
};


int main()
{
    try
    {
        TestObject obj;
        TestObject obj2;
        obj2.apple = "orange";
        obj2.flag  = false;
        obj2.nums  = { 5, 4, 3, 2, 1 };

        auto jsonRoot  = obj.Serialize();
        auto jsonChild = obj2.Serialize();

        std::shared_ptr<cereal::JsonObject> jsonChild2;

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
        jsonRoot->Add("child2", jsonChild2);

        std::cout << jsonRoot->ToString() << std::endl;

        int thirdNum = jsonRoot->GetObject("child").GetVector<int>("nums")[2];
        std::cout << "Third number in child: " << thirdNum << std::endl;

        double ySpeed = jsonRoot->GetObject("child").GetObject("3dcoord").Get<double>("y");
        std::cout << "Y speed: " << ySpeed << std::endl;

        const auto& json   = *jsonRoot;
        double     xSpeed = json["child"]["3dcoord"]["x"];
        std::cout << "X speed: " << xSpeed << std::endl;

        // Not allowed due to json being declared as const
        // json["child"] = obj.Serialize();

        // jsonRoot->PrintToFile("./sample.json", true, 8); // pretty print with 8 spaces for tabs
        jsonRoot->PrintToFile("./sample.json", true); // pretty print with default 4 spaces for tabs

        // show-casing type mismatch error
        int z = json["child"]["3dcoord"]["z"];

    } catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 0;
    }
}