#include "int_adder.h"

int_adder::int_adder(int n)
	: circuit(2 * n, n)
{
	if (n == 1) {
		gate *gxor = new gate(gate::XOR);
		gxor->concat(in[0], in[1]);
		out[0] = gxor;
		return;
	}
	// First, create a half adder manually
	gate* gxor = new gate(gate::XOR), * gand = new gate(gate::AND);
	gxor->concat(in[n - 1], in[2 * n - 1]);
	gand->concat(in[n - 1], in[2 * n - 1]);
	out[n - 1] = gxor;

	// Second, use full adder with carry bit
	gate* carry = gand;
	int i(n - 2);
	for (; i > 0; --i) {
		adder_circuit adder;
		adder.in[0]->concat(in[i]);
		adder.in[1]->concat(in[i + n]);
		adder.in[2]->concat(carry);
		out[i] = adder.out[0];
		carry = adder.out[1];
		adder.moderate_clear();
	}
	

	// Last, construct 2 xor
	gate* garr[2];
	gate::init(garr, 2, gate::XOR);
	garr[0]->concat(in[i], in[i + n]);
	garr[1]->concat(garr[0], carry);
	out[i] = garr[1];
}

exint_adder::exint_adder(int n)
	: circuit(2 * n, n + 1)
{
	// First, create a half adder manually
	gate* gxor = new gate(gate::XOR), * gand = new gate(gate::AND);
	gxor->concat(in[n - 1], in[2 * n - 1]);
	gand->concat(in[n - 1], in[2 * n - 1]);
	out[n] = gxor;

	// Second, use full adder with carry bit
	gate* carry = gand;
	if (n == 1) {
		out[0] = carry;
		return;
	}
	int i(n - 2);
	for (; i >= 0; --i) {
		adder_circuit adder;
		adder.in[0]->concat(in[i]);
		adder.in[1]->concat(in[i + n]);
		adder.in[2]->concat(carry);
		out[i+1] = adder.out[0];
		carry = adder.out[1];
		adder.moderate_clear();
	}
	out[0] = carry;
}


void test_int_adder() {
	const int length(10);
	int_adder adder(length);
	adder.check();
	bool wrong(false);
	for (int _(0); _ != 20; ++_) {
		int a, b, c;
		do {
			a = rand() % (1 << length);
			b = rand() % (1 << length);
			c = a + b;
		} while (c >= (1 << length));
		std::vector<bool> input;
		for (int i(length - 1); i > -1; --i) {
			input.push_back((a >> i) & 1);
		}
		for (int i(length - 1); i > -1; --i) {
			input.push_back((b >> i) & 1);
		}
		std::vector<bool> ret = adder.eval(input);
		int res = 0;
		for (int i(0); i != length; ++i) {
			res = res * 2 + ret[i];
		}
		std::cout << res << " " << c << std::endl;
		if (res != c) {
			wrong = true;
			std::cout << a << " " << b << std::endl;
			adder.print();
		}
	}
	if (wrong) std::cout << "test_int_adder: failed" << std::endl;
	else std::cout << "test_int_adder: passed" << std::endl;
}

void test_exint_adder() {
	const int length(10);
	exint_adder adder(length);
	adder.check();
	bool wrong(false);
	for (int _(0); _ != 20; ++_) {
		int a, b, c;
		a = rand() % (1 << length);
		b = rand() % (1 << length);
		c = a + b;
		std::vector<bool> input;
		for (int i(length - 1); i > -1; --i) {
			input.push_back((a >> i) & 1);
		}
		for (int i(length - 1); i > -1; --i) {
			input.push_back((b >> i) & 1);
		}
		std::vector<bool> ret = adder.eval(input);
		int res = 0;
		for (int i(0); i <= length; ++i) {
			res = res * 2 + ret[i];
		}
		std::cout << res << " " << c << std::endl;
		if (res != c) {
			wrong = true;
			std::cout << a << " " << b << std::endl;
			adder.print();
		}
	}
	if (wrong) std::cout << "test_int_adder: failed" << std::endl;
	else std::cout << "test_int_adder: passed" << std::endl;
}

