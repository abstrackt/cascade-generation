#include <iostream>
#include <cstdlib>
#include <fstream>
#include "optimizer.hpp"

int main(int argc, char *argv[])
{
    if (argc != 4) {
        std::cout << "Usage: ./cascades <INPUT FILE PATH (jsonlines)> <FILTER STATEMENT> <NUMBER OF SAMPLES>";
        return 0;
    }

    auto path = argv[1];
    auto filter = argv[2];
    auto samples = atoi(argv[3]);

    std::ifstream jsonfile(path);
    std::stringstream buf;
    buf << jsonfile.rdbuf();

    Cascade<6, 4>(buf.str(), filter, samples);

    return 0;
}
