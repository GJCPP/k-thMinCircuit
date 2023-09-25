#pragma once
#include "circuit.h"
#include "adder_circuit.h"

class bitadder_circuit :
    public circuit
{
/*
* This adder adds n bits, and represents the result in binary ($\lceil \log n \rceil + 1$ bits).
* It is based on divide-and-conquer method, to minimize circuit size.
*/
public:
    bitadder_circuit(int n);
};

void demo_bitadder();
