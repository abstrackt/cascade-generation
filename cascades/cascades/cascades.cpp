#include <iostream>
#include "optimizer.hpp"

int main()
{
    Cascade<4> c("lorem ipsum\ndolor sit amet\nconsectetur adipiscing elit", "lorem && ip || adipis", 3);
    c.eval("dolor sit");
}
