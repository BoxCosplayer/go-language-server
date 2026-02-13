#include "headers/byte-stream-to-json.h"

#include <unordered_map>
#include <sstream>

// Each Message is recieved via byte streams
// We want JSON

// Use the headers to convert properly:

// Headers:
// Content-Length: int // measured in bytes
// //\r\n
// Content-Type: string // Should be "application/ide-jsonrpc; charset=utf8" -- charset should always be set to utf-8
// \r\n\r\n

// Parse headers into a map
static std::unordered_map<std::string, std::string>
parse_headers(const std::string &header_block)
{
    std::unordered_map<std::string, std::string> map;
    std::istringstream ss(header_block);
    std::string line;
    while (std::getline(ss, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        std::string::size_type pos = line.find(':');
        if (pos == std::string::npos)
            continue;
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        if (!val.empty() && val[0] == ' ')
            val.erase(0, 1);
        map[key] = val;
    }
    return map;
}

// Reads one LSP message body into out_json. Returns false on EOF/stream error.
bool read_lsp_message(std::istream &in, std::string &out_json)
{
    static std::string buffer;
    char chunk[4096];

    std::size_t sep = std::string::npos;

    // Read header block
    while (true)
    {
        // Read more data if needed
        if (in.good())
        {
            in.read(chunk, sizeof(chunk));
            buffer.append(chunk, static_cast<size_t>(in.gcount()));
        }
        if (buffer.empty() && in.eof())
            return false;

        // Find header/content separator
        sep = buffer.find("\r\n\r\n");
        // Keep reading until header + sep is in the buffer
        if (sep == std::string::npos)
            continue;

        // If the sep is in the buffer (headers are all in the buffer)...
        break;
    }

    // Slice headers, making sure to include final CRLF
    std::string header_block = buffer.substr(0, sep + 2);
    std::unordered_map<std::string, std::string> headers = parse_headers(header_block);

    // Find required length header
    std::unordered_map<std::string, std::string>::iterator it = headers.find("Content-Length");
    if (it == headers.end())
        return false;

    // Body begins after CRLFCRLF, and is "content_len" bytes long
    size_t content_len = static_cast<size_t>(std::stoul(it->second));
    size_t body_start = sep + 4;

    if (content_len > out_json.max_size())
        return false;

    // Read body block
    while (buffer.size() < body_start + content_len)
    {
        // Read more data if needed
        if (in.good())
        {
            in.read(chunk, sizeof(chunk));
            buffer.append(chunk, static_cast<size_t>(in.gcount()));
        }
        if (buffer.size() < body_start + content_len && in.eof())
            // Content was greater than the content_length header
            return false;
    }

    // Extract JSON body by length
    out_json = buffer.substr(body_start, content_len);

    // Remove consumed data from buffer (if multiple messages are queued via batch message)
    buffer.erase(0, body_start + content_len);
    return true;

    // the headers do not need to be memorised from here, so it should be safe to call
    // the json deserialisation functions directly to start decoding the body??
    // but then the file wouldnt be
    // byte-stream -> json,     it would be
    // byte-stream -> message
    // something to think about
}
