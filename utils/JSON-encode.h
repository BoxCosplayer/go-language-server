#pragma once
#include <string>
#include "JSON-decode.h"

bool serialiseLspPacket(const incomingMessage &msg, std::string &out_packet);
