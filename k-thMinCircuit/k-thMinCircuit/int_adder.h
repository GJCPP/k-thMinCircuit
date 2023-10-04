#pragma once
#include "circuit.h"
#include "adder_circuit.h"

/*
* Add two n-bit integer.
* It is assumed that the result won't overflow.
* Big-endian.
*/
class int_adder :
    public circuit
{
public:
    int_adder(int n);
};

/*
* Add two n-bit integer, result an (n+1)-bit integer.
*/
class exint_adder :
    public circuit
{
public:
    exint_adder(int n);
};

void test_int_adder();

void test_exint_adder();