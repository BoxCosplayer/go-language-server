#include <cctype>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <string>
#include <optional>

/*
JSON deserialisation (decoding a message)

Messages (Incoming or outgoing) can be 1 of 4 types:

Requests:
{json
    "jsonrpc": "2.0",
    "id": int
    "method": string // "TypeOfTask/Task"
    "params": {
        ...
    }
}

Notifications:
{json
    "jsonprc": "2.0",
    "method": string // "TypeOfTask/Task"
    "params": {
        ...
    }
}

Batches:
{json
    [message1, message2, message3, ...]
}
// NOTE: batches are sorted automatically by `byte-stream-to-json.cpp`

Response:
{json
    "jsonrpc":
    "result": string;
    "error": string;
}

struct incomingMessage
{
    float jsonrpc;
    int id = "null";
    std::string method;
    std::string params_json = "";
    std::string result = null;
    std::string error = null;
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

    // Reads JSON sting value
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
        // If the starting character was a closing '"', then the cursor is at EoS
        // so it fails the check after "++i;"
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

    // Jump cursor to 1 character after the value
    bool skip_value(const std::string &s, size_t &i)
    {
        // If there's whitespace at the cursor, ignore it
        skip_ws(s, i);
        if (i >= s.size())
            return false;

        char c = s[i];

        // Advance the cursor past the value
        switch (c)
        {
        // Start (or end) of a string
        case '"':
        {
            std::string tmp;
            return parse_string(s, i, tmp);
        }
        // Start of block
        case '{':
        {
            ++i;
            skip_ws(s, i);
            // If cursor is at end of block...
            if (i < s.size() && s[i] == '}')
            {
                ++i;
                return true;
            }
            while (i < s.size())
            {
                // Atteomt to parse the key
                std::string key;

                // If theres no key
                if (!parse_string(s, i, key))
                    return false;

                // If there is key, parse_string advanced cursor to the :
                skip_ws(s, i);

                // Fail if there is no separator key
                if (i >= s.size() || s[i] != ':')
                    return false;
                ++i;

                // Consume the value as normal
                if (!skip_value(s, i))
                    // If the value parsing fails, then float the fail up the stack
                    return false;

                // Check to ensure the end of block character
                // is in string
                // Cursor is at end of a value + whitespace here
                skip_ws(s, i);
                if (i >= s.size())
                    return false;

                // If there are more key-value pairs, go again
                if (s[i] == ',')
                {
                    ++i;
                    skip_ws(s, i);
                    continue;
                }

                // If at end of block, skip finished
                if (s[i] == '}')
                {
                    ++i;
                    return true;
                }
                // If havent hit end of block yet, fail
                return false;
            }
            // If bounds exceeded, fail
            return false;
        }
        // Start of array
        case '[':
        {
            // Skip to next value
            ++i;
            skip_ws(s, i);

            // If array is empty
            if (i < s.size() && s[i] == ']')
            {
                ++i;
                return true;
            }
            // For every value in bounds...
            while (i < s.size())
            {
                // If value/block is malformed, fail
                if (!skip_value(s, i))
                    return false;

                skip_ws(s, i);
                if (i >= s.size())
                    return false;

                // If there is another value in array, continue
                if (s[i] == ',')
                {
                    ++i;
                    skip_ws(s, i);
                    continue;
                }
                // Correct end of block check
                if (s[i] == ']')
                {
                    ++i;
                    return true;
                }
                // Malformed array, missing or in valid separator / ']'
                return false;
            }
            // Bounds has been exceeded, fail
            return false;
        }

        // Check for any common non-string terms
        case 't':
            return parse_literal(s, i, "true");
        case 'f':
            return parse_literal(s, i, "false");
        case 'n':
            return parse_literal(s, i, "null");
        default:
            break;
        }

        // Not a string, not any special block-type characters
        // not any non-string terms, assume it is a number
        std::string token;
        return parse_number_token(s, i, token);
    }

    // Extracts value into outstring
    bool extract_raw_value(const std::string &s, size_t &i, std::string &out)
    {
        skip_ws(s, i);
        size_t start = i;
        if (!skip_value(s, i))
            return false;
        out = s.substr(start, i - start);
        return true;
    }
} // namespace

// Generic message, before determining which type it is
struct incomingMessage
{
    float jsonrpc; // Required in all incoming messages, so no default
    std::optional<int> id = std::nullopt;
    std::optional<std::string> method = std::nullopt;
    std::optional<std::string> params_json = std::nullopt;
    std::optional<std::string> result = std::nullopt;
    std::optional<std::string> error = std::nullopt;
};

bool storeMessage(const std::string &json, incomingMessage &out)
{
    // Extracts info from {"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}
    // into the struct defined above
    const std::string &s = json;
    size_t i = 0;
    skip_ws(s, i);
    if (i >= s.size())
        return false;

    // This character indicates a batch, which is handled by byte-stream-to-json.cpp
    if (s[i] == '[')
        return false;

    // Ensures that the json is valid
    if (s[i] != '{')
        return false;
    ++i;

    incomingMessage msg{};
    bool has_jsonrpc = false;

    while (i < s.size())
    {
        skip_ws(s, i);
        if (i >= s.size())
            return false;

        // When the end json character is detected, message is read
        if (s[i] == '}')
        {
            ++i;
            break;
        }

        // Should be expected key:val pairs from now

        // If no key, not valid json
        std::string key;
        if (!parse_string(s, i, key))
            return false;

        // Key processed, value next
        skip_ws(s, i);

        // Check for separator character
        if (i >= s.size() || s[i] != ':')
            return false;
        ++i;
        skip_ws(s, i);

        // Cursor is currently at value[0]
        if (key == "jsonrpc")
        {
            if (i >= s.size())
                return false;

            // If value is a string
            if (s[i] == '"')
            {
                std::string v;
                if (!parse_string(s, i, v))
                    return false;

                // If value isnt a float, error out
                if (!parse_float_str(v, msg.jsonrpc))
                    return false;
            }
            // Value is a 'number'
            else
            {
                std::string v;
                if (!parse_number_token(s, i, v))
                    return false;

                // If value isnt a float, error out
                if (!parse_float_str(v, msg.jsonrpc))
                    return false;
            }
            has_jsonrpc = true;
        }
        else if (key == "id")
        {
            // Value is either "null" or an int
            if (!parse_literal(s, i, "null"))
            {
                int id = 0;
                if (!parse_number_int(s, i, id))
                    return false;
                msg.id = id;
            }
        }
        else if (key == "method")
        {
            // Method must be a string
            std::string method;
            if (!parse_string(s, i, method))
                return false;
            msg.method = method;
        }
        else if (key == "params")
        {
            // Params could be anything
            std::string params = "";
            if (!extract_raw_value(s, i, params))
                return false;
            msg.params_json = params;
        }
        else
        {
            // We don't care about this value (at least for now)
            if (!skip_value(s, i))
                return false;
        }

        skip_ws(s, i);
        if (i >= s.size())
            return false;

        // Continue if there are more values, otherwise end of block reached
        if (s[i] == ',')
        {
            ++i;
            continue;
        }
        if (s[i] == '}')
        {
            ++i;
            break;
        }
        return false;
    }

    skip_ws(s, i);
    // Check to see if the content-length fails, and the json string has extra characters
    if (i != s.size())
        return false;

    // Message parsed, but doesnt have all the required parameters (just jsonprc for now)
    if (!has_jsonrpc)
        return false;

    // Message stored
    out = msg;
    return true;
}
