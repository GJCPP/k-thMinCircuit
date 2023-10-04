# k-thMinCircuit
Target: Build a circuit that compute k-th minimum
Since this project is written with Visual Studio, you have to endure the nested directory, i.e. "k-thMinCircuit" :)

## A. Project Structure

The structure of this project is rather straightforward: a header file, along with a cpp file, defines a class. All classes are successions of the class `circuit`, which provides most API you can use to interact with.

The header files and cpp files are well annotated (I think so, at least...), so you can ignore the rest of this markdown file, and open `main.cpp` to try some demo.

## B. Important classes
Below we briefly introduce each class and its functionality.

### I. `circuit` and `gate`

Class `gate` provides "gate", which is a node in the circuit. A gate has at most two inputs, and exactly one output.

The member functions of `gate` are listed below.
1. `check`: check if the gate is connected properly; namely, it should have its two input wires connected to other gate (except NOT gate).
2. `concat`: concatenate one or two input wire(s) to the other gate(s).
3. `name`: return a `std::string` that names the gate, e.g. "AND".
4. `init`: initialize a `gate *` array.

**WARNING**: Due to implementation of `circuit`, every `gate` instance should be generated by C++ operator `new`. This is because the deconstruction function of `circuit` would try to delete every gate in it.

Class `circuit` provides abstraction of a circuit. The (important) member variables are listed below.
1. `std::vector<gate*> in`: the input gate of the circuit. The gate must be of type `gate::INPUT`.
2. `std::vector<gate*> out`: the output gate of the circuit; can be of any type.

Its member functions are listed below.
1. `clear`: this function will `delete` every gate in the circuit. It's equivalent to deconstruction function.
2. `moderate_clear`: this function will disembed every gate in the circuit, but not `delete` them. This is useful when you want to use the circuit as a module of a bigger circuit; you can connect the wires properly to the outer part, and then `moderate_clear` it.
3. `check`: check if the circuit is ready to be evaluated.
4. `eval`: evaluate the circuit with given input.
5. `size`: return the size (number of gates) of the circuit. Note that `gate::NOT` type gates will not be counted in.
6. `print`: print the circuit. You can try `print` after `eval`; it will also print out the intermediate values.

### II. `adder_circuit`

It realizes a full adder: it takes three input, and output their sum (in binary, i.e. 2 bits).

### III. `compare_circuit`

`compare_circuit(n)` takes input $n$, and initializes a circuit that takes as input two $n$-bit string, and outputs two results (lesser / larger).

Note that this circuit is big endian: the first bit should be MSB.

### IV. `int_adder` and `exint_adder`
`int_adder(n)` takes input $n$, and initializes a circuit that adds two $n$-bit integer, ant outputs their sum. This circuit assumes the addition will not overflow, i.e. the output will be $n$-bit integer.

In case where overflow is possible, please use `exint_adder(n)`.

These two circuits are both big endian.

### V. `bitadder_circuit`
`bitadder_circuit(n)` computes the sum of $n$ bits, and output $\lfloor\log n\rfloor + 1$ bits.

This circuit is **small endian**, please note this.
