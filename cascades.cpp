#include <iostream>
#include <sstream>
#include "optimizer.hpp"

int main()
{
    stringstream ss;

    ss << "lorem ipsum, quia dolor sit, amet, consectetur, adipisci velit," << endl;
    ss << "sed quia non numquam eius modi tempora incidunt, ut labore et dolore" << endl;
    ss << "magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis" << endl;
    ss << "nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut" << endl;
    ss << "aliquid ex ea commodi consequatur? Quis autem vel eum iure" << endl;
    ss << "reprehenderit, qui in ea voluptate velit esse, quam nihil" << endl;
    ss << "molestiae consequatur, vel illum, qui dolorem eum fugiat," << endl;
    ss << "quo voluptas nulla pariatur" << endl;

    Cascade<4, 6> c(ss.str(), "lorem && ipsu || adipis || ess || vel || qui", 8);
    std::cout << "dolor sit evaluates to: " << c.eval("dolor sit") << endl;
    std::cout << "lorem ipsum evaluates to: " << c.eval("lorem ipsum") << endl;
}
