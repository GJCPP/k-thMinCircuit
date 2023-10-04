#pragma once
#include "circuit.h"

/*
* Input: n bits + n bits + 1 selection bit
* Output: if the selection bit is 0, output the first n bits; otherwise the second.
*/
class selector :
    public circuit
{
public:
    selector(int n);
};

void test_selector();
