#include "SampleRepository.h"
#include "../util/JsonParser.h"
#include <fstream>
#include <sstream>

SampleRepository::SampleRepository(const std::string& filePath)
    : filePath_(filePath)
{
}

std::vector<Sample> SampleRepository::loadAll() {
    std::ifstream file(filePath_);
    if (!file) return {};

    std::string text((std::istreambuf_iterator<char>(file)), {});
    if (text.empty()) return {};

    JsonValue root = JsonParser::parse(text);
    std::vector<Sample> result;
    for (const auto& item : root.getArray())
        result.push_back(fromJson(item));
    return result;
}

void SampleRepository::saveAll(const std::vector<Sample>& samples) {
    JsonValue::Array arr;
    for (const auto& s : samples)
        arr.push_back(toJson(s));

    std::ofstream file(filePath_);
    file << JsonParser::serialize(JsonValue(arr));
}

Sample SampleRepository::fromJson(const JsonValue& obj) const {
    std::string id   = obj["id"].getString();
    std::string name = obj["name"].getString();
    double avgTime   = obj["avgProductionTime"].getNumber();
    double yieldRate = obj["yieldRate"].getNumber();
    int    stock     = static_cast<int>(obj["stock"].getNumber());
    return Sample(id, name, avgTime, yieldRate, stock);
}

JsonValue SampleRepository::toJson(const Sample& s) const {
    JsonValue::Object obj;
    obj["id"]                 = JsonValue(s.getId());
    obj["name"]               = JsonValue(s.getName());
    obj["avgProductionTime"]  = JsonValue(s.getAvgProductionTime());
    obj["yieldRate"]          = JsonValue(s.getYieldRate());
    obj["stock"]              = JsonValue(s.getStock());
    return JsonValue(obj);
}
