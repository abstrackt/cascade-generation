#include <iostream>
#include "optimizer.hpp"

int main()
{
    Cascade<4, 3> c("lorem ipisum\ndolor sit amet\nconsectetur adipiscing elit", "lorem && ipsu || adipis", 3);
    std::cout << "dolor sit evaluates to: " << c.eval("dolor sit") << endl;
    std::cout << "lorem ipsum evaluates to: " << c.eval("lorem ipsum") << endl;
}
