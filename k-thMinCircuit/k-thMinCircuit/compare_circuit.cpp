#include "compare_circuit.h"

compare_circuit::compare_circuit(int n)
	: circuit(2 * n, 2)
{
	std::vector<gate*> val[2];
	for (int i(0); i != n; ++i) val[0].push_back(in[i]);
	for (int i(n); i != 2 * n; ++i) val[1].push_back(in[i]);
	gate* larger = new gate(gate::AND);
	gate* lesser = new gate(gate::AND);
	std::vector<gate*> nval[2];
	for (int i(0); i != n; ++i) {
		nval[0].push_back(new gate(gate::NOT));
		nval[1].push_back(new gate(gate::NOT));
	}
	nval[0][0]->concat(val[0][0]);
	nval[1][0]->concat(val[1][0]);
	larger->concat(val[0][0], nval[1][0]);
	lesser->concat(val[1][0], nval[0][0]);
	for (int i(1); i != n; ++i) {
		gate* newval[2];
		gate* newand[2];
		gate* newlarger(new gate(gate::OR)), *newlesser(new gate(gate::OR));
		gate::init(newval, 2, gate::OR);
		gate::init(newand, 2, gate::AND);

		newval[0]->concat(val[0][i], larger);
		newval[1]->concat(val[1][i], lesser);
		val[0][i] = newval[0], val[1][i] = newval[1];
		nval[0][i]->concat(val[0][i]);
		nval[1][i]->concat(val[1][i]);

		newand[0]->concat(val[0][i], nval[1][i]);
		newand[1]->concat(val[1][i], nval[0][i]);
		newlarger->concat(larger, newand[0]);
		newlesser->concat(lesser, newand[1]);

		larger = newlarger;
		lesser = newlesser;
	}
	out[0] = lesser;
	out[1] = larger;
}

void test_comparison() {
	const int n = 100;
	bool wronged(false);
	std::vector<bool> x(n, false), y(n, false);
	compare_circuit C(n);
	std::cout << C.size();
	C.check();
	for (int _(0); _ != 20; ++_) {
		int vx(0), vy(0);
		for (int i(0); i != n; ++i) {
			x[i] = rand() % 2;
			y[i] = rand() % 2;
			vx = (vx << 1) + x[i];
			vy = (vy << 1) + y[i];
		}
		std::vector<bool> input, input_1[2];
		for (int i(0); i != n; ++i) input.push_back(x[i]), input_1[0].push_back(x[i]);
		for (int i(0); i != n; ++i) input.push_back(y[i]), input_1[1].push_back(y[i]);

		auto output = C.eval(input);
		//C.print();
		auto output_1 = compare(input_1, n);
		if (output[0] != output_1[0] || output[1] != output_1[1]) {
			wronged = true;
		}
	}
	if (wronged) std::cout << "Compare circuit wronged." << std::endl;
	else std::cout << "Compare circuit passed." << std::endl;
}

std::vector<bool> compare(std::vector<bool> x[], int n) {
	bool larger(false), lesser(false);
	std::vector<bool> val[2];
	for (int i(0); i != n; ++i) {
		val[0].push_back(x[0][i]);
		val[1].push_back(x[1][i]);
	}
	if (val[0][0] && !val[1][0]) larger = true;
	if (val[1][0] && !val[0][0]) lesser = true;
	for (int i(1); i != n; ++i) {
		bool newval[2]{ false,false };
		newval[0] = (larger || val[0][i]);
		newval[1] = (lesser || val[1][i]);
		val[0][i] = newval[0], val[1][i] = newval[1];
		bool newand[2]{ false,false };
		if (val[0][i] && !val[1][i]) newand[0] = true;
		if (val[1][i] && !val[0][i]) newand[1] = true;
		larger = (larger || newand[0]);
		lesser = (lesser || newand[1]);
	}
	return { lesser, larger };
}

less_circuit::less_circuit(int n)
	: circuit(2 * n, 1)
{
	std::vector<gate*> gval;
	for (int i(0); i != n; ++i) {
		gate* gxor = new gate(gate::XOR);
		gxor->concat(in[i], in[i + n]);
		gval.push_back(gxor);
	}
	for (int i(1); i != n; ++i) {
		gate* gor = new gate(gate::OR);
		gor->concat(gval[i - 1], gval[i]);
		gval[i] = gor;
	}
	std::vector<gate*> new_gval;
	new_gval.push_back(gval[0]);
	for (int i(1); i != n; ++i) {
		gate* gxor = new gate(gate::XOR);
		gxor->concat(gval[i - 1], gval[i]);
		new_gval.push_back(gxor);
	}
	gval = std::move(new_gval);
	for (int i(0); i != n; ++i) {
		gate* gand = new gate(gate::AND);
		gand->concat(gval[i], in[i + n]);
		gval[i] = gand;
	}
	for (int i(1); i != n; ++i) {
		gate* gor = new gate(gate::OR);
		gor->concat(gval[i - 1], gval[i]);
		gval[i] = gor;
	}
	out[0] = gval[n - 1];
}

void test_less() {
	const int n = 100;
	bool wronged(false);
	std::vector<bool> x(n, false), y(n, false);
	less_circuit C(n);
	std::cout << C.size();
	C.check();
	for (int _(0); _ != 20; ++_) {
		int vx(0), vy(0);
		for (int i(0); i != n; ++i) {
			x[i] = rand() % 2;
			y[i] = rand() % 2;
			vx = (vx << 1) + x[i];
			vy = (vy << 1) + y[i];
		}
		std::vector<bool> input, input_1[2];
		for (int i(0); i != n; ++i) input.push_back(x[i]), input_1[0].push_back(x[i]);
		for (int i(0); i != n; ++i) input.push_back(y[i]), input_1[1].push_back(y[i]);

		auto output = C.eval(input);
		//C.print();
		auto output_1 = compare(input_1, n);
		if (output[0] != output_1[0]) {
			wronged = true;
			C.print();
			std::cout << std::endl;
		}
	}
	if (wronged) std::cout << "Compare circuit wronged." << std::endl;
	else std::cout << "Compare circuit passed." << std::endl;
}