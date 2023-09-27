#include "kmin_algorithm.h"

/*
* This is the k-th min algorithm we are going to implement.
* It's in essence a "binary" search, but it is written in a form that is intuitive to write a circuit.
* I.e. in the circuit, you start from the MSB, and process bit by bit towards LSB.
* You maintain a "dead" bit, to indicate if a vector is still participating / possible to be the k-th min.
* 
* NOTE: in my implementation of circuit, NOT gate can be used freely. So I do not much mind about NOT operation.
*/
std::vector<bool> kmin(std::vector<bool> x[], int n, int k) {
	int length = x[0].size();
	for (int i(1); i != n; ++i) {
		if (x[i].size() != length) throw "Input of different length";
	}
	if (k < 1 || k > n) throw "k out of range.";
	std::vector<bool> ret;
	std::vector<bool> dead(n, false);
	int strict_less = 0;
	for (int i(0); i != length; ++i) {
		int cnt = 0;
		for (int j(0); j != n; ++j) {
			int v = (x[j][i] || dead[j]);
			if (v == 0) ++cnt;
		}
		if (cnt + strict_less < k) {
			ret.push_back(1);
			strict_less += cnt;
		} else {
			ret.push_back(0);
		}
		for (int j(0); j != n; ++j) {
			dead[j] = (dead[j] || (x[j][i] != ret.back()));
		}
	}
	return ret;
}

void test_kmin() {
	const int n = 100, l = 32;
	bool flag = false;
	std::vector<int> x(n, 0);
	std::vector<bool> bx[n];
	for (int i(0); i != n; ++i) bx[i].resize(l, false);
	for (int _(0); _ != 20; ++_) {
		int k = rand() % n + 1;
		for (int i(0); i != n; ++i) {
			x[i] = rand();
			for (int j(0); j != l; ++j) {
				bx[i][l - j - 1] = ((x[i] >> j) & 1);
			}
		}
		auto ans = kmin(bx, n, k);
		int ians = 0;
		for (int i(0); i != l; ++i) {
			ians += ((ans[i] ? 1 : 0) << (l - i - 1));
		}
		std::sort(x.begin(), x.end());
		if (x[k - 1] != ians) {
			std::cout << "Wronged." << std::endl;
			flag = true;
			break;
		}
	}
	if (!flag) {
		std::cout << "Passed." << std::endl;
	}
}
