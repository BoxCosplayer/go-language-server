#pragma once

#include "parameter-extraction.h"

#include <map>
#include <ostream>
#include <vector>

namespace print_helpers
{
    void printObject(const std::map<std::string, ParameterValue> &object, int depth, std::ostream &out);
    void printArray(const std::vector<ParameterValue> &array, int depth, std::ostream &out);
    void printParameterValue(const ParameterValue &value, int depth, std::ostream &out);
    void printParameterTree(const ParameterTree &tree, std::ostream &out);
}
