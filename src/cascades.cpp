#include <iostream>
#include <sstream>
#include "optimizer.hpp"

int main()
{
    stringstream ss;

    ss << "name: Richard," << endl;
    ss << "age: 27," << endl;
    ss << "nationality: British" << endl;
    ss << "name: Will," << endl;
    ss << "age: 40," << endl;
    ss << "nationality: Italian" << endl;

    Cascade<4, 6> c(ss.str(), "name && Will || age && 40 || Italian", 8);
    std::cout << "name: Will evaluates to: " << c.eval("name: Will") << endl;
    std::cout << "name: Richard evaulates to: " << c.eval("name: Richard") << endl;
}
