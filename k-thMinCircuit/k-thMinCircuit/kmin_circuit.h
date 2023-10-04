#pragma once
#include "circuit.h"
#include "bitadder_circuit.h"
#include "compare_circuit.h"
#include "adder_circuit.h"
#include "int_adder.h"
#include "selector.h"

#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>

std::vector<bool> kmin(std::vector<bool> x[], int n, int k);
void test_kmin();

void test_kmin_circuit();

/*
* We now meet the climax: the k-th min circuit, the finale of this project.
* The method to compute k-th min is illustrated in kmin function.
* 
* n is the number of values, l is the length of each value.
* the input is: n many l-bit inputs + log(n)-bit k + log(n)-bit 0.
*/
class kmin_circuit
	: public circuit
{
public:
	kmin_circuit(int n, int l);
};
