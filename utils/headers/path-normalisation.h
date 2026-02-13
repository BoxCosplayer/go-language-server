#pragma once
#include <string>
#include <optional>

struct URI
{
    std::string Scheme;

    // Represents Network paths
    std::optional<std::string> Authority = std::nullopt;

    std::string Path;

    // Represents parts of files, where appropriate
    std::optional<std::string> Query = std::nullopt;
    std::optional<std::string> Fragment = std::nullopt;
};

bool uriToPath(URI &uri, std::string &out);
bool pathToURI(std::string &filepath, URI &out);
