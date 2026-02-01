#include <iostream>
#include <string>
#include <istream>
#include <fcntl.h>
#include <io.h>

// from utils/byte-stream-to-json.cpp
bool read_lsp_message(std::istream &in, std::string &out_json);

int main()
{
    _setmode(_fileno(stdin), _O_BINARY); // preserve \r\n on windows systems, where \r\n\r\n >> \n\n
    std::string json;
    while (read_lsp_message(std::cin, json))
    {
        std::cout << json << std::endl;
    }
    return 0;
}
