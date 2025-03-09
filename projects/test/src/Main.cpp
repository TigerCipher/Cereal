#include "cereal/Cereal.h"

#include "cereal/JsonParser.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <ostream>


struct TestObject : public cereal::Serializable
{
    std::string apple = "apple";
    bool        flag  = true;

    std::vector<int> nums = { 1, 2, 3, 4, 5 };

    std::array<float, 5> spanArray = { 1.5f, 2.4f, 3.0f, 4.2f, 5.0f };

    std::array<bool, 3> bools = { true, false, true };

    char c = 'b';

    char tilemap[3][3] = {
        { 'a', 'b', 'c' },
        { 'd', 'e', 'f' },
        { 'g', 'h', 'i' }
    };

    int num = 37;

    [[nodiscard]] cereal::JsonObject Serialize() override
    {
        cereal::JsonObject json;
        json.Add("fruit", apple);
        json.Add("flag", flag);

        json.Add("name", "John Doe");
        json.Add("null", nullptr);

        std::span<float> span = spanArray;
        json.Add("span", span);

        // Vectors, arrays, etc must first be converted to a span
        std::span<int> numSpan = nums;

        // You can also add like this
        json["spannum"] = numSpan;
        json["letter"]  = c;

        std::span<bool> boolSpan = bools;
        json["bools"]            = boolSpan;

        // Raw pointers, and there for C srrays and multi-dimensional arrays are not supported and must
        // be converted to a flat span
        std::span<char> flatSpan{ &tilemap[0][0], sizeof(tilemap) };
        json["tilemap"] = flatSpan;

        json["num"] = num;

        return json;
    }

    void Deserialize(const cereal::JsonObject& json) override {}
};

struct Vector2 : public cereal::Serializable
{
    float x = 0.0f;
    float y = 0.0f;

    Vector2() = default;
    Vector2(float xx, float yy) : x(xx), y(yy) {}

    [[nodiscard]] cereal::JsonObject Serialize() override
    {
        cereal::JsonObject json;
        json.Add("x", x);
        json.Add("y", y);
        return json;
    }

    void Deserialize(const cereal::JsonObject& json) override
    {
        x = json["x"];
        y = json["y"];
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector2& obj) { return os << " x: " << obj.x << " y: " << obj.y; }
};

struct Vector3 : public cereal::Serializable
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vector2 pos{ 3.4f, 2.f };

    Vector3() = default;

    [[nodiscard]] cereal::JsonObject Serialize() override
    {
        cereal::JsonObject json;
        json["x"] = x;
        json["y"] = y;
        json["z"] = z;

        json["pos"] = pos.Serialize();

        return json;
    }


    void Deserialize(const cereal::JsonObject& json) override
    {
        x = json["x"];
        y = json["y"];
        z = json["z"];
        pos.Deserialize(json["pos"]);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& obj)
    {
        return os << "x: " << obj.x << " y: " << obj.y << " z: " << obj.z << " pos: " << obj.pos;
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

        auto json      = obj.Serialize();
        auto jsonChild = obj2.Serialize();

        cereal::JsonObject jsonChild2;

        Vector2 pos;
        pos.x = 1.0f;
        pos.y = 2.0f;

        Vector3 vec;
        vec.x = 1.76;
        vec.y = 2.7423432423770929990;
        vec.z = 3.76;

        jsonChild.Add("2dcoord", pos.Serialize());
        jsonChild.Add("3dcoord", vec.Serialize());

        json.Add("child", jsonChild);
        json.Add("child2", jsonChild2);

        int thirdNum = json.GetObject("child").GetSpan<int>("spannum")[2];
        std::cout << "Third number in child: " << thirdNum << std::endl;

        double ySpeed = json.GetObject("child").GetObject("3dcoord").Get<double>("y");
        std::cout << "Y speed: " << ySpeed << std::endl;


        double xSpeed = json["child"]["3dcoord"]["x"];
        std::cout << "X speed: " << xSpeed << std::endl;

        // Not allowed due to json being declared as const
        // json["child"] = obj.Serialize();

        // Allowed because nonconstJson is, well, non const
        json["child"]["3dcoord"]["pos"]["y"] = 7.459f;

        // json.PrintToFile("./sample.json", true, 8); // pretty print with 8 spaces for tabs
        json.PrintToFile("./sample.json", true); // pretty print with default 4 spaces for tabs


        char letterFromChar = json["child"]["letter"];
        int  letterAsInt    = json["child"]["letter"];

        std::cout << "Letter as char: " << letterFromChar << std::endl;
        std::cout << "Letter as int: " << letterAsInt << std::endl;

        int  num       = json["num"];
        char numAsChar = json["num"];

        std::cout << "Num as int: " << num << std::endl;
        std::cout << "Num as char: " << numAsChar << std::endl;

        // show-casing type mismatch error
        std::string z = json["child"]["3dcoord"]["z"];

    } catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        std::ifstream      file("./sample.json");
        std::string        str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        cereal::JsonObject parsedRoot = cereal::Parse(str);

        parsedRoot.PrintToFile("./parsed_output.json", true);

        Vector2 deserialized2dCoord{};
        Vector3 deserialized3dCoord{};

        deserialized2dCoord.Deserialize(parsedRoot["child"]["2dcoord"]);
        deserialized3dCoord.Deserialize(parsedRoot["child"]["3dcoord"]);

        std::cout << "Deserialized 2D Coord: " << deserialized2dCoord << std::endl;
        std::cout << "Deserialized 3D Coord: " << deserialized3dCoord << std::endl;

    } catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}