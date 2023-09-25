#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
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
	*     2. if every output gate is of type OUTPUT
	*     3. if every common gate is not of type INPUT/OUTPUT
	*     4. if each gate (except input and output gate) has two input wires connected
	*     5. if each output gate has exactly input[0] non-nullptr
	*     6. if each gate (except output gate) has its output vector non-empty
	* 
	* The last three is done by gate::check()
	* Return TRUE if the check passes.
	* Note that, in a well-formed circuit, enumeration of gates can be done by starting from in and traveling by output vector
	*/
	void check() const;

	/*
	* Clear all "ready_inputs" mark
	* The circuit must be well-formed.
	*/
	void clear_state();

	/*
	* The circuit must be well-formed
	*/
	std::vector<bool> eval(const std::vector<bool>& input);


	void copy_from(const circuit& C);

	std::vector<gate*> in, out;
protected:
	/*
	* This construction function init the circuit in and out to appointed number
	*/
	circuit(int fanin, int fanout);
};

class gate {
	/*
	* NOTE: Due to the design of ~circuit, you should only use "new gate(...)" to create gate object.
	* In particular, do not use new gate[](...).
	* You can use gate::init to initialize a gate pointer array.
	*/
public:
	enum gate_type {
		AND, OR, XOR, INPUT, END_OT_TYPE
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
	* In particular, if the gate is OUTPUT gate, this function will take its input[0] and delete that gate.
	* If **this** gate is INPUT gate, it will turn concat g to all its output gates.
	* So please remind that if **this** gate is INPUT, it will be destroyed.
	*/
	void concat(gate *g);

	void concat(gate* g1, gate* g2);


	/*
	* This function calls operator new to allocate sz many gates.
	*/
	static void init(gate* arr[], int sz, gate_type type);

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
