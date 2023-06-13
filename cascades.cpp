#include <iostream>
#include "optimizer.hpp"

int main()
{
    Cascade<4, 3> c("lorem ipisum\ndolor sit amet\nconsectetur adipiscing elit", "lorem && ipisu || adipis", 3);
    c.eval("dolor sit");
}
