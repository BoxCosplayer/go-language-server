#include "headers/parameter-extraction.h"

#include <cctype>
#include <cstdlib>

namespace
{
    void skip_ws(const std::string& s, size_t& i)
    {
        while (i < s.size())
        {
            char c = s[i];
            if (c != ' ' && c != '\n' && c != '\r' && c != '\t')
                break;
            ++i;
        }
    }

    bool parse_hex4(const std::string& s, size_t& i, unsigned& code)
    {
        if (i + 4 > s.size())
            return false;

        code = 0;
        for (int j = 0; j < 4; ++j)
        {
            char h = s[i + static_cast<size_t>(j)];
            unsigned v = 0;
            if (h >= '0' && h <= '9')
                v = static_cast<unsigned>(h - '0');
            else if (h >= 'a' && h <= 'f')
                v = static_cast<unsigned>(10 + (h - 'a'));
            else if (h >= 'A' && h <= 'F')
                v = static_cast<unsigned>(10 + (h - 'A'));
            else
                return false;
            code = (code << 4U) | v;
        }

        i += 4;
        return true;
    }

    bool parse_string(const std::string& s, size_t& i, std::string& out)
    {
        if (i >= s.size() || s[i] != '"')
            return false;
        ++i;

        out.clear();

        while (i < s.size())
        {
            char c = s[i++];

            if (c == '"')
                return true;

            if (c == '\\')
            {
                if (i >= s.size())
                    return false;

                char esc = s[i++];
                switch (esc)
                {
                case '"':
                case '\\':
                case '/':
                    out.push_back(esc);
                    break;
                case 'b':
                    out.push_back('\b');
                    break;
                case 'f':
                    out.push_back('\f');
                    break;
                case 'n':
                    out.push_back('\n');
                    break;
                case 'r':
                    out.push_back('\r');
                    break;
                case 't':
                    out.push_back('\t');
                    break;
                case 'u':
                {
                    unsigned code = 0;
                    if (!parse_hex4(s, i, code))
                        return false;

                    // Keep this parser ASCII-safe for now.
                    if (code <= 0x7F)
                        out.push_back(static_cast<char>(code));
                    else
                        out.push_back('?');
                    break;
                }
                default:
                    return false;
                }
                continue;
            }

            if (static_cast<unsigned char>(c) < 0x20)
                return false;

            out.push_back(c);
        }

        return false;
    }

    bool parse_literal(const std::string& s, size_t& i, const char* literal)
    {
        size_t j = 0;
        while (literal[j] != '\0')
        {
            if (i + j >= s.size() || s[i + j] != literal[j])
                return false;
            ++j;
        }
        i += j;
        return true;
    }

    bool parse_number(const std::string& s, size_t& i, double& out)
    {
        size_t start = i;

        if (i < s.size() && s[i] == '-')
            ++i;

        if (i >= s.size())
            return false;

        if (s[i] == '0')
        {
            ++i;
        }
        else if (s[i] >= '1' && s[i] <= '9')
        {
            ++i;
            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
                ++i;
        }
        else
        {
            return false;
        }

        if (i < s.size() && s[i] == '.')
        {
            ++i;
            if (i >= s.size() || !std::isdigit(static_cast<unsigned char>(s[i])))
                return false;
            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
                ++i;
        }

        if (i < s.size() && (s[i] == 'e' || s[i] == 'E'))
        {
            ++i;
            if (i < s.size() && (s[i] == '+' || s[i] == '-'))
                ++i;
            if (i >= s.size() || !std::isdigit(static_cast<unsigned char>(s[i])))
                return false;
            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
                ++i;
        }

        std::string token = s.substr(start, i - start);
        char* end = nullptr;
        out = std::strtod(token.c_str(), &end);
        return end == token.c_str() + token.size();
    }

    bool parse_value(const std::string& s, size_t& i, ParameterValue& out);

    bool parse_array(const std::string& s, size_t& i, std::vector<ParameterValue>& out)
    {
        if (i >= s.size() || s[i] != '[')
            return false;
        ++i;

        out.clear();
        skip_ws(s, i);

        if (i < s.size() && s[i] == ']')
        {
            ++i;
            return true;
        }

        while (i < s.size())
        {
            ParameterValue value;
            if (!parse_value(s, i, value))
                return false;
            out.push_back(value);

            skip_ws(s, i);
            if (i >= s.size())
                return false;

            if (s[i] == ',')
            {
                ++i;
                skip_ws(s, i);
                continue;
            }

            if (s[i] == ']')
            {
                ++i;
                return true;
            }

            return false;
        }

        return false;
    }

    bool parse_object(const std::string& s, size_t& i, std::map<std::string, ParameterValue>& out)
    {
        if (i >= s.size() || s[i] != '{')
            return false;
        ++i;

        out.clear();
        skip_ws(s, i);

        if (i < s.size() && s[i] == '}')
        {
            ++i;
            return true;
        }

        while (i < s.size())
        {
            std::string key;
            if (!parse_string(s, i, key))
                return false;

            skip_ws(s, i);
            if (i >= s.size() || s[i] != ':')
                return false;
            ++i;

            ParameterValue value;
            if (!parse_value(s, i, value))
                return false;

            out[key] = value;

            skip_ws(s, i);
            if (i >= s.size())
                return false;

            if (s[i] == ',')
            {
                ++i;
                skip_ws(s, i);
                continue;
            }

            if (s[i] == '}')
            {
                ++i;
                return true;
            }

            return false;
        }

        return false;
    }

    bool parse_value(const std::string& s, size_t& i, ParameterValue& out)
    {
        skip_ws(s, i);
        if (i >= s.size())
            return false;

        char c = s[i];

        if (c == '{')
        {
            out = ParameterValue{};
            out.type = ParameterType::Object;
            return parse_object(s, i, out.object_value);
        }

        if (c == '[')
        {
            out = ParameterValue{};
            out.type = ParameterType::Array;
            return parse_array(s, i, out.array_value);
        }

        if (c == '"')
        {
            out = ParameterValue{};
            out.type = ParameterType::String;
            return parse_string(s, i, out.string_value);
        }

        if (c == 't')
        {
            if (!parse_literal(s, i, "true"))
                return false;
            out = ParameterValue{};
            out.type = ParameterType::Boolean;
            out.bool_value = true;
            return true;
        }

        if (c == 'f')
        {
            if (!parse_literal(s, i, "false"))
                return false;
            out = ParameterValue{};
            out.type = ParameterType::Boolean;
            out.bool_value = false;
            return true;
        }

        if (c == 'n')
        {
            if (!parse_literal(s, i, "null"))
                return false;
            out = ParameterValue{};
            out.type = ParameterType::Null;
            return true;
        }

        double num = 0.0;
        if (!parse_number(s, i, num))
            return false;

        out = ParameterValue{};
        out.type = ParameterType::Number;
        out.number_value = num;
        return true;
    }
} // namespace

bool extractParameters(const std::string& params_json, ParameterTree& out)
{
    size_t i = 0;
    skip_ws(params_json, i);

    std::map<std::string, ParameterValue> fields;
    if (!parse_object(params_json, i, fields))
        return false;

    skip_ws(params_json, i);
    if (i != params_json.size())
        return false;

    out.fields = fields;
    return true;
}