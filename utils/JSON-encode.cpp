#include "JSON-encode.h"
#include <cctype>
#include <string>

/*
JSON serialisation (encoding a message)

Outgoing messages are the same as incomingMessages, check notes at the top of utils\JSON-decode.cpp
I dont need to differentiate what type of outgoing message, just any message will do with this format

struct outgoingMessage
{
    float jsonrpc
    std::optional<int>
    std::optional<std::string> method
    std::optional<std::string> params_json
    std::optional<std::string> result
    std::optional<std::string> error
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

    // Reads JSON string value while validating escapes and control characters
    bool parse_string(const std::string &s, size_t &i)
    {
        if (i >= s.size() || s[i] != '"')
            return false;
        ++i;

        while (i < s.size())
        {
            unsigned char c = static_cast<unsigned char>(s[i++]);

            if (c == '"')
                return true;

            // Reject unescaped control characters
            if (c < 0x20)
                return false;

            if (c != '\\')
                continue;

            if (i >= s.size())
                return false;

            char esc = s[i++];
            switch (esc)
            {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                break;
            case 'u':
            {
                if (i + 4 > s.size())
                    return false;
                for (int j = 0; j < 4; ++j)
                {
                    unsigned char h = static_cast<unsigned char>(s[i + j]);
                    if (!std::isxdigit(h))
                        return false;
                }
                i += 4;
                break;
            }
            default:
                return false;
            }
        }

        // If the closing '"' is missing, malformed JSON string
        return false;
    }

    // Verifier tool to check value of keyword
    // Used for "true", "false" and "null"
    bool parse_literal(const std::string &s, size_t &i, const char *literal)
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

    // Parse number when one is expected
    // Implements JSON number grammar:
    // -? (0|[1-9][0-9]*) (.[0-9]+)? ([eE][+-]?[0-9]+)?
    bool parse_number_token(const std::string &s, size_t &i)
    {
        if (i >= s.size())
            return false;

        if (s[i] == '-')
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

        // Check fraction
        if (i < s.size() && s[i] == '.')
        {
            ++i;
            if (i >= s.size() || !std::isdigit(static_cast<unsigned char>(s[i])))
                return false;
            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
                ++i;
        }

        // Check exponent
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

        return true;
    }

    // Jump cursor to 1 character after the value
    bool skip_value(const std::string &s, size_t &i)
    {
        skip_ws(s, i);
        if (i >= s.size())
            return false;

        char c = s[i];

        // Advance the cursor past the value
        switch (c)
        {
        // Start (or end) of a string
        case '"':
            return parse_string(s, i);

        // Start of block
        case '{':
        {
            ++i;
            skip_ws(s, i);

            if (i < s.size() && s[i] == '}')
            {
                ++i;
                return true;
            }

            while (i < s.size())
            {
                // Parse key string first
                if (!parse_string(s, i))
                    return false;

                skip_ws(s, i);
                if (i >= s.size() || s[i] != ':')
                    return false;
                ++i;

                if (!skip_value(s, i))
                    return false;

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

        // Start of array
        case '[':
        {
            ++i;
            skip_ws(s, i);

            if (i < s.size() && s[i] == ']')
            {
                ++i;
                return true;
            }

            while (i < s.size())
            {
                if (!skip_value(s, i))
                    return false;

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

        // Check for common non-string terms
        case 't':
            return parse_literal(s, i, "true");
        case 'f':
            return parse_literal(s, i, "false");
        case 'n':
            return parse_literal(s, i, "null");
        default:
            break;
        }

        // Assume number if not string/object/array/literal
        return parse_number_token(s, i);
    }

    // Ensure a string is exactly one valid JSON value
    bool is_complete_json_value(const std::string &s)
    {
        size_t i = 0;
        skip_ws(s, i);
        if (i >= s.size())
            return false;

        if (!skip_value(s, i))
            return false;

        skip_ws(s, i);
        return i == s.size();
    }

    // Appends the separator before the next field
    // Handles first-field logic and comma placement
    void append_field_prefix(std::string &out, bool &has_field)
    {
        if (has_field)
            out.push_back(',');
        has_field = true;
    }

    // Writes a JSON-escaped string into the output buffer
    // Example output: "textDocument/completion"
    void append_escaped_json_string(const std::string &value, std::string &out)
    {
        static const char hex[] = "0123456789ABCDEF";

        out.push_back('"');
        for (unsigned char c : value)
        {
            switch (c)
            {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\b':
                out += "\\b";
                break;
            case '\f':
                out += "\\f";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if (c < 0x20)
                {
                    out += "\\u00";
                    out.push_back(hex[(c >> 4) & 0x0F]);
                    out.push_back(hex[c & 0x0F]);
                }
                else
                {
                    out.push_back(static_cast<char>(c));
                }
                break;
            }
        }
        out.push_back('"');
    }

    // Validates combinations of optional fields for strict JSON-RPC shapes
    bool has_valid_message_shape(const incomingMessage &msg)
    {
        const bool has_id = msg.id.has_value();
        const bool has_method = msg.method.has_value();
        const bool has_params = msg.params_json.has_value();
        const bool has_result = msg.result.has_value();
        const bool has_error = msg.error.has_value();

        // result and error are mutually exclusive
        if (has_result && has_error)
            return false;

        // params only belongs to requests/notifications
        if (has_params && !has_method)
            return false;

        // method cannot be mixed with response payload
        if (has_method && (has_result || has_error))
            return false;

        const bool is_request = has_id && has_method && !has_result && !has_error;
        const bool is_notification = !has_id && has_method && !has_result && !has_error;
        const bool is_success_response = has_id && !has_method && !has_params && has_result && !has_error;
        const bool is_error_response = has_id && !has_method && !has_params && !has_result && has_error;

        return is_request || is_notification || is_success_response || is_error_response;
    }

    bool serialiseMessage(const incomingMessage &msg, std::string &out_json)
    {
        // Protocol version is always required and must be exactly 2.0
        if (msg.jsonrpc != 2.0f)
            return false;

        // Reject any invalid message combinations before building output
        if (!has_valid_message_shape(msg))
            return false;

        // Validate raw JSON fragments before writing them into body
        if (msg.params_json.has_value() && !is_complete_json_value(*msg.params_json))
            return false;
        if (msg.result.has_value() && !is_complete_json_value(*msg.result))
            return false;
        if (msg.error.has_value() && !is_complete_json_value(*msg.error))
            return false;

        // Build into local buffer so caller output stays untouched on failure
        std::string body;

        // Change to match typical "small message" size
        body.reserve(128);
        body.push_back('{');

        bool has_field = false;

        // Always include protocol version first
        append_field_prefix(body, has_field);
        append_escaped_json_string("jsonrpc", body);
        body.push_back(':');
        append_escaped_json_string("2.0", body);

        // Stable field order for deterministic output
        if (msg.id.has_value())
        {
            append_field_prefix(body, has_field);
            append_escaped_json_string("id", body);
            body.push_back(':');
            body += std::to_string(*msg.id);
        }

        if (msg.method.has_value())
        {
            append_field_prefix(body, has_field);
            append_escaped_json_string("method", body);
            body.push_back(':');
            append_escaped_json_string(*msg.method, body);
        }

        if (msg.params_json.has_value())
        {
            append_field_prefix(body, has_field);
            append_escaped_json_string("params", body);
            body.push_back(':');
            body += *msg.params_json;
        }

        if (msg.result.has_value())
        {
            append_field_prefix(body, has_field);
            append_escaped_json_string("result", body);
            body.push_back(':');
            body += *msg.result;
        }

        if (msg.error.has_value())
        {
            append_field_prefix(body, has_field);
            append_escaped_json_string("error", body);
            body.push_back(':');
            body += *msg.error;
        }

        body.push_back('}');
        out_json = body;
        return true;
    }

} // namespace

bool serialiseLspPacket(const incomingMessage &msg, std::string &out_packet)
{
    std::string json_body;
    if (!serialiseMessage(msg, json_body))
        return false;

    std::string packet;
    packet.reserve(json_body.size() + 96);
    packet += "Content-Length: ";
    packet += std::to_string(json_body.size());
    packet += "\r\n";
    packet += "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n";
    packet += "\r\n";
    packet += json_body;

    out_packet = packet;
    return true;
}
