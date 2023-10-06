#include "kmin_circuit.h"

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
	const int n = 1000, l = 20;
	bool flag = false;
	std::vector<int> x(n, 0);
	std::vector<bool> bx[n];
	for (int i(0); i != n; ++i) bx[i].resize(l, false);
	for (int _(0); _ != 20; ++_) {
		int k = rand() % n + 1;
		for (int i(0); i != n; ++i) {
			x[i] = rand() % (1 << l);
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


kmin_circuit::kmin_circuit(int n, int l)
	: circuit(n * l + _count_bits(n) + 1, l)
{
	int logn(_count_bits(n));
	gate* zero(in[n * l + logn]);
	std::vector<gate*> k, strict_less; // big endian integer
	std::vector<gate*> dead, val; // boolean array
	k = { in.begin() + n * l, in.begin() + n * l + logn };
	strict_less.resize(logn, zero);
	dead.resize(n, zero);
	val = {in.begin(), in.begin() + n * l};


	for (int i(0); i != l; ++i) {
		bitadder_circuit adder(n);
		std::vector<gate*> cnt;
		for (int j(0); j != n; ++j) {
			gate* gnot(new gate(gate::NOT));
			gnot->concat(val[j * l + i]);
			adder.in[j]->concat(gnot);
		}
		for (int j(adder.out.size() - 1); j >= 0; --j) {
			cnt.push_back(adder.out[j]);
			// Caution : adder is small endian.
		}
		adder.moderate_clear();


		int_adder new_sl(logn); // = strict_less + cnt
		for (int j(0); j != logn; ++j) new_sl.in[j]->concat(strict_less[j]);
		for (int j(0); j != logn; ++j) new_sl.in[j + logn]->concat(cnt[j]);
		std::vector<gate*> sum(std::move(new_sl.out)); // = strict_less + cnt
		for (int j(0); j != logn; ++j) {
			sum[j]->name(std::string("-SUM") + char(i + '0') + char(j + '0'));
		}
		new_sl.moderate_clear();
		assert(sum.size() == logn);


		compare_circuit comp(logn);
		for (int j(0); j != logn; ++j) comp.in[j]->concat(sum[j]);
		for (int j(0); j != logn; ++j) comp.in[j + logn]->concat(k[j]);
		gate* lesser = comp.out[0];
		comp.moderate_clear();


		out[i] = lesser;
		out[i]->name(std::string("-lesser") + char(i+'0'));


		// use leq to select new value for strict_less
		selector sel(logn);
		sel.in[logn * 2]->concat(lesser);
		// if leq == false, remain unchanged
		for (int j(0); j != logn; ++j) sel.in[j]->concat(strict_less[j]);
		// if leq == true, select the "added" value
		for (int j(0); j != logn; ++j) sel.in[j + logn]->concat(sum[j]);
		strict_less = std::move(sel.out);
		sel.moderate_clear();

		if (i != l - 1) {
			for (int j(0); j != n; ++j) {
				gate* gor(new gate(gate::OR)), * gxor(new gate(gate::XOR));
				gxor->concat(val[j * l + i], out[i]);
				gor->concat(dead[j], gxor);
				dead[j] = gor;
				gate* gor2(new gate(gate::OR));
				gor2->concat(val[j * l + i + 1], dead[j]);
				val[j * l + i + 1] = gor2;
			}
		}
	}
	remove_void();
}

void test_kmin_circuit() {
	const int n(100), l(128);
	int logn(_count_bits(n));
	int ik;
	kmin_circuit C(n, l);
	C.check();
	std::vector<bool> val[n];
	std::vector<bool> k;
	bool wrong(false);
	for (int i(0); i != n; ++i) {
		val[i].resize(l, false);
	}
	k.resize(logn, false);
	for (int _(0); _ != 2000; ++_) {
		//std::cout << _ << std::endl;
		for (int i(0); i != n; ++i) {
			for (int j(0); j != l; ++j) {
				val[i][j] = rand() % 2;
			}
		}
		ik = rand() % n + 1;
		std::vector<bool> input;
		for (int i(0); i != n; ++i) {
			for (int j(0); j != l; ++j) {
				input.push_back(val[i][j]);
			}
		}
		for (int i(0); i != logn; ++i) {
			k[i] = ((ik >> (logn - i - 1)) & 1);
			input.push_back(k[i]);
		}
		input.push_back(false);
		auto ret = C.eval(input);
		auto ans = kmin(val, n, ik);
		for (int i(0); i != l; ++i) {
			if (ret[i] != ans[i]) {
				wrong = true;
			}
		}
	}
	if (wrong) std::cout << "test_kmin_circuit: wrong." << std::endl;
	else std::cout << "test_kmin_circuit: passed." << std::endl;
	std::cout << C.size();
}