#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <string>
class gate;
class circuit {
public:
	circuit() = default;

	/*
	* To construct a circuit that shares a same topology of C
	* It guarantees that the order of input wire is preserved,
	* i.e. the input[0] of a gate in C is input[0] of the corresponding gate in *this.
	* 
	* So is the output vector.
	*/
	circuit(const circuit& C);


	~circuit();

	/*
	* To deconstruct the entire circuit.
	* All gates will be deconstructed.
	*/
	void clear();

	/*
	* This function erase the in\out vector, but not destroy any gate.
	* This prevents the deconstruction of circuit from destroying every gate in it.
	*/
	void moderate_clear();

	/*
	* To check if the circuit is well-formed.
	* 
	*     1. if every input gate is of type INPUT
	*     2. if every common gate is not of type INPUT
	*     3. if each gate is recorded as output by its input gates
	*     4. if each gate (except INPUT) has two input wires connected
	*     5. if each gate (except outputing one) has its output vector non-empty
	* 
	* The last two is done by gate::check()
	* If check fail, it throws exception.
	* Note that, in a well-formed circuit, enumeration of gates can be done by starting from in and traveling by output vector.
	* I.e. the user is response for making sure that there is no loop in the circuit.
	*/
	void check() const;

	/*
	* Clear all "ready_inputs" mark
	* The circuit must be well-formed.
	*/
	void clear_state();

	/*
	* To evaluate, the circuit must be well-formed.
	*/
	std::vector<bool> eval(const std::vector<bool>& input);


	/*
	* Count the size of the circuit.
	* Note that this will not count in NOT gate and INPUT gate.
	*/
	int size() const;


	/*
	* Print the circuit; if it is evaluated, the state will also be printed.
	* Enjoy debugging :)
	*/
	void print() const;


	void copy_from(const circuit& C);

	std::vector<gate*> in, out;
protected:
	/*
	* This construction function init the circuit in and out to appointed number
	*/
	circuit(int fanin, int fanout);
};



/*
* NOTE: Due to the design of ~circuit, you should only use "new gate(...)" to create gate object.
* In particular, do not use new gate[](...).
* You can use gate::init to initialize a gate pointer array.
*/
class gate {
public:
	enum gate_type {
		NOT, AND, OR, XOR, INPUT, END_OF_TYPE
		// for OUTPUT gates, the only non-nullptr wire should be input[0]
	};
	gate();
	gate(gate_type t);

	/*
	* To check if the gate is connected "reasonably":
	*     if the gate (except input gate) has two input wires connected
	*/
	void check() const;
	
	/*
	* Try to connect the gate in parameter as an input gate; try input[0] first, [1] second, FAIL third.
	* If **this** gate is INPUT gate, it will try concat g to all its output gates.
	* So please remind that if **this** gate is INPUT, it will be destroyed.
	*/
	void concat(gate *g);

	void concat(gate* g1, gate* g2);


	/*
	* This function calls operator new to allocate sz many gates.
	*/
	static void init(gate* arr[], int sz, gate_type type);

	/*
	* Return the type of the gate, e.g. "NOT"
	*/
	std::string name() const;


	gate* input[2];
	std::vector<gate*> output;
	int ready_inputs;
	bool val;
	gate_type type;

protected:

	/*
	* Disconnect g from its input gate.
	*/
	void disconnect(gate* g);
};

void demo_circuit();
