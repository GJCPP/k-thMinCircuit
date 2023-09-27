#pragma once
#include "circuit.h"
#include "adder_circuit.h"

class bitadder_circuit :
    public circuit
{
/*
* This adder adds n bits, and represents the result in binary ($\lceil \log n \rceil + 1$ bits).
* It is based on divide-and-conquer method, to minimize circuit size.
* The output will be big endian: the first bit is MSB.
* 
* On input n = 2^k, the size of this circuit satisfies
*             F[k] = 2 * F[k - 1] + 5 * k - 3
*             F[1] = 2
*             F[k] = 9 * 2^{k-1} - 5 * k + 3
*/
public:
    bitadder_circuit(int n);
};

void demo_bitadder();
