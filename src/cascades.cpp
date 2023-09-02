#include <iostream>
#include <cstdlib>
#include <fstream>
#include "optimizer.hpp"

const int MAX_CLAUSES = 6;
const int FILTER_LENGTH = 4;

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

    auto c = Cascade<FILTER_LENGTH, MAX_CLAUSES>(buf.str(), filter, samples);

    c.print_cascade();

    return 0;
}
