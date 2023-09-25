#include "adder_circuit.h"

adder_circuit::adder_circuit()
	: circuit(3, 2)
{
	gate* gand[2], * gxor[2], * gor;
	gate::init(gand, 2, gate::AND);
	gate::init(gxor, 2, gate::XOR);
	gor = new gate(gate::OR);

	gxor[0]->concat(in[0], in[1]);
	gand[0]->concat(in[0], in[1]);
	gxor[1]->concat(gxor[0], in[2]);
	gand[1]->concat(gxor[0], in[2]);
	gor->concat(gand[0], gand[1]);

	out[0] = gxor[1];
	out[1] = gor;
}

void demo_adder() {
	adder_circuit C;
	C.check();
	for (int i(0); i != 2; ++i) {
		for (int j(0); j != 2; ++j) {
			for (int k(0); k != 2; ++k) {
				auto ret = C.eval({ bool(i), bool(j), bool(k)});
				std::cout << i << j << k << " : " << ret[1] << ret[0] << std::endl;
			}
		}
	}
}