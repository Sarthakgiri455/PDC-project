#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>

int main()
{
    // Open the JSON file for reading
    std::ifstream ifs("example.json");

    // Parse the JSON file into a Json::Value object
    Json::Value json;
    ifs >> json;

    // Access the data in the JSON object
    std::string name = json["name"].asString();
    int age = json["age"].asInt();

    // Print the data to the console
    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;

    return 0;
}
