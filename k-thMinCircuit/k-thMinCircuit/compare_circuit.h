#pragma once
#include "circuit.h"
class compare_circuit :
    public circuit
{
    /*
    * The implementation of "strictly less" and "strictly larger".
    */
public:
    /*
    * n = the length of two inputs to be compared.
    */
    compare_circuit(int n);
};
class less_circuit :
    public circuit
{
    /*
    * The implementation of "strictly less".
    */
public:
    /*
    * n = the length of two inputs to be compared.
    */
    less_circuit(int n);
};

void test_comparison();

std::vector<bool> compare(std::vector<bool> x[], int n);

void test_less();
