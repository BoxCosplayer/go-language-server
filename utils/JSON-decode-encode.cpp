#include <cctype>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <string>

// JSON deserialisation (decoding a message)

// Messages can be 1 of 3 types:

// Requests:
//{json
//     "jsonrpc": "2.0",
//     "id": int
//     "method": string // "TypeOfTask/Task"
//     "params": {
//         ...
//     }
// }

// Notifications:
// {json
//     "jsonprc": "2.0",
//     "method": string // "TypeOfTask/Task"
//     "params": {
//         ...
//     }
// }

// Batches:
// {json
//     [message1, message2, message3, ...]
// }
// NOTE: batches are sorted automatically by `byte-stream-to-json.cpp`

/*
struct message
{
    float jsonrpc;
    int id;
    bool has_id;
    std::string method;
    std::string params_json;
};
*/

namespace
{
    // Moves cursor past whitespace / special characters
    void skip_ws(const std::string &s, size_t &i)
    {
        while (i < s.size())
        {
            char c = s[i];
            if (c != ' ' && c != '\n' && c != '\r' && c != '\t')
                break;
            ++i;
        }
    }

    // Reads JSON input
    bool parse_string(const std::string &s, size_t &i, std::string &out)
    {
        // If cursor is after the EoS
        // OR
        // If cursor isnt at a ", fail
        if (i >= s.size() || s[i] != '"')
            return false;
        ++i;
        out.clear();

        // While cursor is within bounds of string
        while (i < s.size())
        {
            // Read the current character and increment the cursor
            char c = s[i];
            i++;

            // If string had ended
            if (c == '"')
                return true;

            // Handle special characters
            if (c == '\\')
            {
                // Ignore if its last character of string
                if (i >= s.size())
                    return false;
                char esc = s[i++];
                switch (esc)
                {
                // Decodes JSON escape sequences -> ASCII escapes
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
                // Case Unicode character is present -- analyse next 4 characters
                case 'u':
                {
                    if (i + 4 > s.size())
                        return false;
                    unsigned code = 0;

                    // Converts hex value to ascii
                    for (int j = 0; j < 4; ++j)
                    {
                        char h = s[i + j];
                        unsigned v = 0;
                        if (h >= '0' && h <= '9')
                            v = static_cast<unsigned>(h - '0');
                        else if (h >= 'a' && h <= 'f')
                            v = static_cast<unsigned>(10 + (h - 'a'));
                        else if (h >= 'A' && h <= 'F')
                            v = static_cast<unsigned>(10 + (h - 'A'));
                        else
                            return false;
                        code = (code << 4) | v;
                    }
                    i += 4;

                    // If it is not valid ascii, then append '?'
                    if (code <= 0x7F)
                        out.push_back(static_cast<char>(code));
                    else
                        out.push_back('?');
                    break;
                }
                // If its none of those, we can't handle that special character
                // Fail
                default:
                    return false;
                }
                // go to next character
                continue;
            }
            // If not special character, append as usual
            out.push_back(c);
        }
        // If the closing character of the json isnt present within the string, fail
        return false;
    }

    // Verifier tool to check value of keyword
    // Used for "true", "false" and "null"
    bool parse_literal(const std::string &s, size_t &i, const char *literal)
    {
        size_t len = std::strlen(literal);
        if (s.compare(i, len, literal) == 0)
        {
            i += len;
            return true;
        }
        return false;
    }

    // Parse number when one is expected; used as framework for int-checking or float-checking
    bool parse_number_token(const std::string &s, size_t &i, std::string &out)
    {
        size_t start = i;
        // skip signage
        if (i < s.size() && (s[i] == '-' || s[i] == '+'))
            ++i;

        bool has_digits = false;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
        {
            ++i;
            has_digits = true;
        }

        // Check if fraction
        if (i < s.size() && s[i] == '.')
        {
            ++i;
            bool has_frac = false;
            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
            {
                ++i;
                has_frac = true;
            }
            has_digits = has_digits || has_frac;
        }

        if (!has_digits)
            return false;

        // out = number parsed
        out = s.substr(start, i - start);
        return true;
    }

    // Parse number, when expecting an int
    bool parse_number_int(const std::string &s, size_t &i, int &out)
    {
        std::string token;

        // If its not a number, return false
        // If it is, token == number
        if (!parse_number_token(s, i, token))
            return false;

        // If its a decimal or has exponent, return false
        // Explicitly doesn't handle exponents.
        if (token.find_first_of(".eE") != std::string::npos)
            return false;

        long long value = 0;
        size_t pos = 0;

        // Checks singage
        bool neg = false;
        if (!token.empty() && (token[0] == '-' || token[0] == '+'))
        {
            neg = token[0] == '-';
            pos = 1;
        }

        // For each digit in the token, move it into the 'value'
        for (; pos < token.size(); ++pos)
        {
            value = value * 10 + static_cast<long long>(token[pos] - '0');
            // If its too big, then return false
            if (value > static_cast<long long>(INT_MAX) + 1)
                return false;
        }
        if (neg)
            value = -value;
        // If its small after being signed
        if (value < INT_MIN || value > INT_MAX)
            return false;
        out = static_cast<int>(value);
        return true;
    }

    // Parse number, when expecting an float
    bool parse_float_str(const std::string &s, float &out)
    {
        if (s.empty())
            return false;

        // Converts string to double -- NOT casting
        char *end = nullptr;
        const char *cstr = s.c_str();
        double value = std::strtod(cstr, &end);

        // If we didn't consume the whole string, it's not a valid number
        if (end != cstr + s.size())
            return false;

        // Store as float
        out = static_cast<float>(value);
        return true;
    }
} // namespace