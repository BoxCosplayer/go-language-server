#include "utils/headers/byte-stream-to-json.h"
#include "utils/headers/JSON-decode.h"
#include "utils/headers/JSON-encode.h"
#include "utils/headers/parameter-extraction.h"
#include "utils/headers/print-helpers.h"
#include "utils/headers/logger.h"
#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <io.h>

int main()
{
    _setmode(_fileno(stdin), _O_BINARY); // preserve \r\n on windows systems, where \r\n\r\n >> \n\n

    // Start Logger
    std::string logDir = "logs";
    std::string logFile;
    if (!initialiseLogger(logDir, logFile))
    {
        std::cout << "Logfile could not be created." << std::endl;
    }
    else
    {
        logEvent(logFile, "Server process started", LogEventType::Lifecycle, LogSeverity::Info);
    }

    std::string json;
    std::cout << "Message recieved" << std::endl;
    logEvent(logFile, "Waiting for LSP messages on stdin", LogEventType::Lifecycle, LogSeverity::Info);

    while (read_lsp_message(std::cin, json))
    {
        std::cout << "Headers Valid" << std::endl;
        logEvent(logFile, "Received packet with valid LSP headers", LogEventType::Internal, LogSeverity::Info);

        Message msg;
        if (!storeMessage(json, msg))
        {
            std::cout << "Message Invalid" << std::endl;
            logEvent(logFile, "Message body failed JSON-RPC validation", LogEventType::Internal, LogSeverity::Warning);
            continue;
        }

        std::cout << "Message Valid" << std::endl;
        std::ostringstream messageSummary;
        messageSummary << "Decoded message jsonrpc=" << msg.jsonrpc;
        if (msg.id.has_value())
            messageSummary << ", id=" << *msg.id;
        else
            messageSummary << ", id=<none>";

        if (msg.method.has_value())
            messageSummary << ", method=" << *msg.method;
        else
            messageSummary << ", method=<none>";

        LogEventType eventType = msg.method.has_value() ? LogEventType::Request : LogEventType::Response;
        logEvent(logFile, messageSummary.str(), eventType, LogSeverity::Info);

        std::cout << "jsonrpc Version: " << msg.jsonrpc << std::endl;

        if (msg.id.has_value())
            std::cout << "ID: " << *msg.id << std::endl;
        else
            std::cout << "No ID" << std::endl;

        if (msg.method.has_value())
            std::cout << "Method: " << *msg.method << std::endl;
        else
            std::cout << "No Method" << std::endl;

        if (msg.params_json.has_value())
        {
            std::cout << "Params: " << *msg.params_json << std::endl;
            ParameterTree params;
            if (extractParameters(*msg.params_json, params))
            {
                std::cout << "Layered Params: ";
                print_helpers::printParameterTree(params, std::cout);
                std::cout << std::endl;
                logEvent(logFile, "Params parsed into ParameterTree", LogEventType::Request, LogSeverity::Info);
            }
            else
            {
                std::cout << "Params could not be parsed into layered struct" << std::endl;
                logEvent(logFile, "Params present but could not be parsed into ParameterTree", LogEventType::Request, LogSeverity::Warning);
            }
        }
        else
            std::cout << "No Parameters" << std::endl;

        if (msg.result.has_value())
            std::cout << "Result: " << *msg.result << std::endl;
        else
            std::cout << "No Results" << std::endl;

        if (msg.error.has_value())
            std::cout << "Error(s): " << *msg.error << std::endl;
        else
            std::cout << "No Errors" << std::endl;

        std::string serialisedMessage;
        if (serialiseLspPacket(msg, serialisedMessage))
        {
            std::cout << "\r\nEncoded Message: \r\n\r\n"
                << serialisedMessage << std::endl;
            logEvent(logFile, "Message serialised into LSP packet", LogEventType::Response, LogSeverity::Info);
        }
        else
        {
            std::cout << "Something went wrong encoding the message" << std::endl;
            logEvent(logFile, "Failed to serialise message into LSP packet", LogEventType::Internal, LogSeverity::Error);
        }
    }

    logEvent(logFile, "Input stream closed or unreadable; server loop exiting", LogEventType::Lifecycle, LogSeverity::Info);

    return 0;
}
