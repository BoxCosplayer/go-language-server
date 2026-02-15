#pragma once

#include <map>
#include <string>
#include <vector>

enum class ParameterType
{
    Null,
    Boolean,
    Number,
    String,
    Object,
    Array
};

struct ParameterValue
{
    ParameterType type = ParameterType::Null;

    bool bool_value = false;
    double number_value = 0.0;
    std::string string_value;

    std::map<std::string, ParameterValue> object_value;
    std::vector<ParameterValue> array_value;
};

struct ParameterTree
{
    std::map<std::string, ParameterValue> fields;
};

bool extractParameters(const std::string &params_json, ParameterTree &out);