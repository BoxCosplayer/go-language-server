#include "utils/byte-stream-to-json.h"
#include "utils/JSON-decode.h"
#include "utils/JSON-encode.h"
#include <iostream>
#include <string>
#include <istream>
#include <fcntl.h>
#include <io.h>

int main()
{
    _setmode(_fileno(stdin), _O_BINARY); // preserve \r\n on windows systems, where \r\n\r\n >> \n\n
    std::string json;
    std::cout << "Message recieved" << std::endl;
    while (read_lsp_message(std::cin, json))
    {
        std::cout << "Headers Valid" << std::endl;
        incomingMessage msg;
        if (!storeMessage(json, msg))
        {
            std::cout << "Message Invalid" << std::endl;
            continue;
        }

        // std::cout << "Message Valid" << std::endl;

        // std::cout << "jsonrpc Version: " << msg.jsonrpc << std::endl;

        // if (msg.id.has_value())
        //     std::cout << "ID: " << *msg.id << std::endl;
        // else
        //     std::cout << "No ID" << std::endl;

        // if (msg.method.has_value())
        //     std::cout << "Method: " << *msg.method << std::endl;
        // else
        //     std::cout << "No Method" << std::endl;

        // if (msg.params_json.has_value())
        //     std::cout << "Params: " << *msg.params_json << std::endl;
        // else
        //     std::cout << "No Parameters" << std::endl;

        // if (msg.result.has_value())
        //     std::cout << "Result: " << *msg.result << std::endl;
        // else
        //     std::cout << "No Results" << std::endl;

        // if (msg.error.has_value())
        //     std::cout << "Error(s): " << *msg.error << std::endl;
        // else
        //     std::cout << "No Errors" << std::endl;

        std::string serialisedMessage;
        if (serialiseLspPacket(msg, serialisedMessage))
        {
            std::cout << "\r\nEncoded Message: \r\n\r\n"
                      << serialisedMessage << std::endl;
        }
        else
        {
            std::cout << "Something went wrong encoding the message" << std::endl;
        }
    }
    return 0;
}
