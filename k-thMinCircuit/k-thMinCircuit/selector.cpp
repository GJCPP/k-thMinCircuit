#include "selector.h"

selector::selector(int n)
	: circuit(2 * n + 1, n)
{
	gate* gnot = new gate(gate::NOT);
	gnot->concat(in[2 * n]);
	for (int i(0); i != n; ++i) {
		gate* gand[2];
		gate::init(gand, 2, gate::AND);
		gate* gor(new gate(gate::OR));
		gand[0]->concat(in[i], gnot);
		gand[1]->concat(in[i + n], in[2 * n]);
		gor->concat(gand[0], gand[1]);
		out[i] = gor;
	}
}

void test_selector() {
	const int n(20);
	selector C(n);
	int val[2][n];
	bool wrong = false;
	for (int _(0); _ != 20; ++_) {
		std::vector<bool> input;
		for (int i(0); i != n; ++i) {
			val[0][i] = rand() % 2;
			val[1][i] = rand() % 2;
		}
		for (int i(0); i != n; ++i) input.push_back(val[0][i]);
		for (int i(0); i != n; ++i) input.push_back(val[1][i]);
		bool selection(rand() % 2);
		input.push_back(selection);
		auto ret = C.eval(input);
		for (int i(0); i != n; ++i) {
			if (selection) {
				if (ret[i] != val[1][i]) {
					wrong = true;
				}
			} else {
				if (ret[i] != val[0][i]) {
					wrong = true;
				}
			}
		}
	}
	if (wrong) std::cout << "test_selector: wrong." << std::endl;
	else std::cout << "test_selector: passed." << std::endl;
}