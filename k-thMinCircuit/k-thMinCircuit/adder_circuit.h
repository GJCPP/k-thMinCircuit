#pragma once
#include "circuit.h"

/*
* This is the implementation of full adder, with carry bit.
* It takes as input two bits, and output two bits.
* input[0] will be XOR of the three inputs, input[1] will be AND of then
* ADDER = two XOR gates + two AND gates + one OR gate
* 
*                |---------|
* input[0] ---->>|         |------->> output[0] (XOR)
* input[1] ---->>|  ADDER  |
* input[2] ---->>|         |------->> output[1] (carry bit)
*                |---------|
*/
class adder_circuit :
    public circuit
{
public:
    adder_circuit();
};

void demo_adder();