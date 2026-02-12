#pragma once
#include <string>
#include <optional>

struct Message
{
    float jsonrpc; // Required in all incoming messages, validation handled via storeMessage()
    std::optional<int> id = std::nullopt;
    std::optional<std::string> method = std::nullopt;
    std::optional<std::string> params_json = std::nullopt;
    std::optional<std::string> result = std::nullopt;
    std::optional<std::string> error = std::nullopt;
};

bool storeMessage(const std::string &json, Message &out);
