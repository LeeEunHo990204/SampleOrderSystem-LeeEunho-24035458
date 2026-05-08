#pragma once
#include <string>
#include <vector>
#include "../model/Sample.h"
#include "../util/JsonValue.h"

class SampleRepository {
public:
    explicit SampleRepository(const std::string& filePath);

    std::vector<Sample> loadAll();
    void saveAll(const std::vector<Sample>& samples);

private:
    std::string filePath_;

    Sample    fromJson(const JsonValue& obj) const;
    JsonValue toJson(const Sample& s)        const;
};
