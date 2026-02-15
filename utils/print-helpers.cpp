#include "headers/print-helpers.h"

namespace
{
    void printIndent(int depth, std::ostream &out)
    {
        for (int i = 0; i < depth; ++i)
            out << "  ";
    }
}

namespace print_helpers
{
    void printObject(const std::map<std::string, ParameterValue> &object, int depth, std::ostream &out)
    {
        out << "{\n";
        for (std::map<std::string, ParameterValue>::const_iterator it = object.begin(); it != object.end(); ++it)
        {
            printIndent(depth + 1, out);
            out << it->first << ": ";
            printParameterValue(it->second, depth + 1, out);
            out << "\n";
        }
        printIndent(depth, out);
        out << "}";
    }

    void printArray(const std::vector<ParameterValue> &array, int depth, std::ostream &out)
    {
        out << "[\n";
        for (size_t i = 0; i < array.size(); ++i)
        {
            printIndent(depth + 1, out);
            printParameterValue(array[i], depth + 1, out);
            if (i + 1 < array.size())
                out << ",";
            out << "\n";
        }
        printIndent(depth, out);
        out << "]";
    }

    void printParameterValue(const ParameterValue &value, int depth, std::ostream &out)
    {
        switch (value.type)
        {
        case ParameterType::Null:
            out << "null";
            break;
        case ParameterType::Boolean:
            out << (value.bool_value ? "true" : "false");
            break;
        case ParameterType::Number:
            out << value.number_value;
            break;
        case ParameterType::String:
            out << "\"" << value.string_value << "\"";
            break;
        case ParameterType::Object:
            printObject(value.object_value, depth, out);
            break;
        case ParameterType::Array:
            printArray(value.array_value, depth, out);
            break;
        }
    }

    void printParameterTree(const ParameterTree &tree, std::ostream &out)
    {
        printObject(tree.fields, 0, out);
    }
}
