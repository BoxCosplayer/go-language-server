// We want to convert URI -> a path that we can use
// All incoming URIs should be file:/// URIs

#include "headers/path-normalisation.h"

#include <cctype>
#include <filesystem>
#include <system_error>

namespace
{
    bool is_hex(char c)
    {
        // Is character a valid hexcode char?
        return (c >= '0' && c <= '9') ||
               (c >= 'a' && c <= 'f') ||
               (c >= 'A' && c <= 'F');
    }

    unsigned hex_value(char c)
    {
        // hex -> digit
        if (c >= '0' && c <= '9')
            return static_cast<unsigned>(c - '0');
        if (c >= 'a' && c <= 'f')
            return static_cast<unsigned>(10 + (c - 'a'));
        return static_cast<unsigned>(10 + (c - 'A'));
    }

    bool percent_decode(const std::string &in, std::string &out)
    {
        // In URIs, hex codes are encoded then denoted via % signs
        // %AA -> %1010
        out.clear();
        out.reserve(in.size());

        for (size_t i = 0; i < in.size(); ++i)
        {
            char c = in[i];
            if (c != '%')
            {
                out.push_back(c);
                continue;
            }

            if (i + 2 >= in.size())
                return false;

            char h1 = in[i + 1];
            char h2 = in[i + 2];
            if (!is_hex(h1) || !is_hex(h2))
                return false;

            unsigned byte = (hex_value(h1) << 4U) | hex_value(h2);
            out.push_back(static_cast<char>(byte));
            i += 2;
        }

        return true;
    }

    void slash_to_backslash(std::string &s)
    {
        for (char &c : s)
        {
            if (c == '/')
                c = '\\';
        }
    }

    void backslash_to_slash(std::string &s)
    {
        for (char &c : s)
        {
            if (c == '\\')
                c = '/';
        }
    }

    char upper_hex_digit(unsigned v)
    {
        // nibble -> hex
        return static_cast<char>((v < 10) ? ('0' + v) : ('A' + (v - 10)));
    }

    bool is_unreserved(unsigned char c)
    {
        // true  if text
        // false if special character, used by URIs as per RFC
        return std::isalnum(c) ||
               c == '-' || c == '.' || c == '_' || c == '~';
    }

    bool is_sub_delim(unsigned char c)
    {
        // characters that to not need to be hex-encoded
        switch (c)
        {
        case '!':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ';':
        case '=':
            return true;
        default:
            return false;
        }
    }

    bool should_encode_path_char(unsigned char c)
    {
        // RFC 3986 path allows pchar + '/'
        return !(is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '@' || c == '/');
    }

    void percent_encode_path(const std::string &in, std::string &out)
    {
        out.clear();
        out.reserve(in.size());

        for (unsigned char c : in)
        {
            if (!should_encode_path_char(c))
            {
                out.push_back(static_cast<char>(c));
                continue;
            }

            out.push_back('%');
            out.push_back(upper_hex_digit((c >> 4U) & 0xFU));
            out.push_back(upper_hex_digit(c & 0xFU));
        }
    }

    bool scheme_is_file(const std::string &scheme)
    {
        // Checking scheme of URI if it is file or not
        // Should always be file
        if (scheme.size() != 4)
            return false;

        return std::tolower(static_cast<unsigned char>(scheme[0])) == 'f' &&
               std::tolower(static_cast<unsigned char>(scheme[1])) == 'i' &&
               std::tolower(static_cast<unsigned char>(scheme[2])) == 'l' &&
               std::tolower(static_cast<unsigned char>(scheme[3])) == 'e';
    }

    bool is_drive_path(const std::string &path)
    {
        // RFC-style file URI path on Windows: /C:/folder/file.cpp
        return path.size() >= 3 &&
               path[0] == '/' &&
               std::isalpha(static_cast<unsigned char>(path[1])) &&
               path[2] == ':';
    }
} // namespace

// URI -> local file path
// e.g. file:///[absolute-path]/utils/headers/JSON-decode.h?q#h -> utils\headers\JSON-decode.h?q#h

bool uriToPath(URI &uri, std::string &out)
{
    // Should only be pointing to files
    if (!scheme_is_file(uri.Scheme))
        return false;

    // Query/fragment indicate a non-filesystem resource identity.
    if (uri.Query.has_value() || uri.Fragment.has_value())
        return false;

    std::string decoded_path;
    if (!percent_decode(uri.Path, decoded_path))
        return false;

    std::string path;

    // UNC/network form: file://server/share/path
    if (uri.Authority.has_value() &&
        !uri.Authority->empty() &&
        *uri.Authority != "localhost")
    {
        if (decoded_path.empty() || decoded_path[0] != '/')
            return false;

        path = "\\\\";
        path += *uri.Authority;
        path += decoded_path;

        if (uri.Query.has_value())
        {
            std::string q = *uri.Query;
            path += "?";
            path += q;
        }
        if (uri.Fragment.has_value())
        {
            std::string f = *uri.Fragment;
            path += "#";
            path += f;
        }
        slash_to_backslash(path);
        out = path;
        return true;
    }

    // Local drive form: /C:/folder/file.cpp -> C:\folder\file.cpp
    if (is_drive_path(decoded_path))
        decoded_path.erase(0, 1);

    slash_to_backslash(decoded_path);
    path = decoded_path;

    out = path;
    return true;
}

// filepath -> URI
// e.g. utils\headers\JSON-decode.h?q#h -> file:////[absolute-path]/utils/headers/JSON-decode.h?q#h

bool pathToURI(std::string &filepath, URI &out)
{
    if (filepath.empty())
        return false;

    std::string raw = filepath;
    std::optional<std::string> query = std::nullopt;
    std::optional<std::string> fragment = std::nullopt;

    // Allow optional suffixes (?query#fragment) if caller stores URI extras in the same string.
    size_t fragment_pos = raw.find('#');
    if (fragment_pos != std::string::npos)
    {
        fragment = raw.substr(fragment_pos + 1);
        raw.erase(fragment_pos);
    }

    size_t query_pos = raw.find('?');
    if (query_pos != std::string::npos)
    {
        query = raw.substr(query_pos + 1);
        raw.erase(query_pos);
    }

    if (raw.empty())
        return false;

    std::error_code ec;
    std::filesystem::path absolute_path = std::filesystem::absolute(std::filesystem::path(raw), ec);
    if (ec)
        return false;

    std::string native_path = absolute_path.string();
    if (native_path.empty())
        return false;

    URI uri;
    uri.Scheme = "file";
    uri.Query = query;
    uri.Fragment = fragment;

    // UNC/network form: \\server\share\folder\file.cpp -> file://server/share/folder/file.cpp
    if (native_path.size() >= 2 && native_path[0] == '\\' && native_path[1] == '\\')
    {
        size_t host_start = 2;
        size_t host_end = native_path.find_first_of("\\/", host_start);
        if (host_end == std::string::npos || host_end == host_start)
            return false;

        uri.Authority = native_path.substr(host_start, host_end - host_start);
        std::string unc_path = native_path.substr(host_end);
        backslash_to_slash(unc_path);

        if (unc_path.empty() || unc_path[0] != '/')
            return false;

        percent_encode_path(unc_path, uri.Path);
        out = uri;
        return true;
    }

    uri.Authority = std::nullopt;

    std::string normalized_path = native_path;
    backslash_to_slash(normalized_path);

    // Local drive form: C:\folder\file.cpp -> /C:/folder/file.cpp
    if (normalized_path.size() >= 2 &&
        std::isalpha(static_cast<unsigned char>(normalized_path[0])) &&
        normalized_path[1] == ':')
    {
        normalized_path.insert(normalized_path.begin(), '/');
    }

    percent_encode_path(normalized_path, uri.Path);

    out = uri;
    return true;
}
