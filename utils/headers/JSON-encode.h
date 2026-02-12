#pragma once
#include <string>
#include "JSON-decode.h"

bool serialiseLspPacket(const Message &msg, std::string &out_packet);
