#include "bitadder_circuit.h"

/*
* This function computes the number of bits needed to represent n in binary.
*/
int _count_bits(int n) {
	int ret(1);
	while (n > 1) {
		n >>= 1;
		++ret;
	}
	return ret;
}

bitadder_circuit::bitadder_circuit(int n)
	: circuit(n, _count_bits(n))
{
	if (n == 1) throw "Cannot create a vacuous bitadder.";
	if (n == 2) {
		// do it by hand.
		gate* gand(new gate(gate::AND)), * gxor(new gate(gate::XOR));
		gand->concat(in[0], in[1]);
		gxor->concat(in[0], in[1]);
		out[0] = gxor;
		out[1] = gand;
		return;
	}
	if (n == 3) {
		// reduce to full adder.
		adder_circuit C;
		for (int i(0); i != 3; ++i) in[i] = C.in[i];
		for (int i(0); i != 2; ++i) out[i] = C.out[i];

		C.moderate_clear();
		// this prevents ~circuit from destroying the above gates.
		return;
	}
	bitadder_circuit C1(n / 2), C2(n - n / 2);
	// Caution: C1 might fewer output gates than C2 (by one)
	// E.g. 7 -> 3 + 4, 3 can be represent in two bits, yet 4 needs three.
	// Yet since n >= 4, C1 has at least two bits.

	// First, create a half adder manually
	gate *gxor = new gate(gate::XOR), *gand = new gate(gate::AND);
	gxor->concat(C1.out[0], C2.out[0]);
	gand->concat(C1.out[0], C2.out[0]);
	out[0] = gxor;

	// Second, use full adder with carry bit
	gate* carry = gand;
	int i(1);
	for (; i != C2.out.size(); ++i) {
		if (i < C1.out.size()) {
			adder_circuit adder;
			adder.in[0]->concat(C1.out[i]);
			adder.in[1]->concat(C2.out[i]);
			adder.in[2]->concat(carry);
			out[i] = adder.out[0];
			carry = adder.out[1];
			adder.moderate_clear();
		} else {
			// construct half adder
			gate* gxor = new gate(gate::XOR), * gand = new gate(gate::AND);
			gxor->concat(carry, C2.out[i]);
			gand->concat(carry, C2.out[i]);
			out[i] = gxor;
			carry = gand;
		}
	}
	if (i != out.size()) {
		// This means that it's possible that carry != 0
		out[i] = carry;
	}

	// Last, transfer the input of C1 C2 to **this**
	int ind = 0;
	for (; ind != C1.in.size(); ++ind) {
		in[ind] = C1.in[ind];
	}
	for (int j(0); j != C2.in.size(); ++ind, ++j) {
		in[ind] = C2.in[j];
	}
	C1.moderate_clear();
	C2.moderate_clear();
}

void demo_bitadder() {
	int n = 7;
	bitadder_circuit C(n);
	std::vector<bool> vec(n, 0);
	bool wronged = false;
	for (int i(0); i != 8; ++i) {
		int testval = rand() % (1 << n);
		int val = testval;
		for (int j(0); j != n; ++j) {
			vec[j] = (val & 1);
			val >>= 1;
		}
		auto output = C.eval(vec);
		int v = 0, ans = 0;
		for (int i : vec) ans += i;
		for (int j(output.size() - 1); j != -1; --j) v = ((v << 1) | output[j]);

		std::cout << ans << " | ";
		for (int j(0); j != n; ++j) std::cout << vec[j] << ' ';
		std::cout << ": ";
		for (int j(output.size() - 1); j != -1; --j) std::cout << output[j] << ' ';
		std::cout << "| " << v << std::endl;
		wronged = true;
	}
}
