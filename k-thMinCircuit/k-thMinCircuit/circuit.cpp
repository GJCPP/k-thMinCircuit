#include "circuit.h"

circuit::circuit(const circuit& C) :
	in(C.in),
	out(C.out) {
	std::map<gate*, const gate*> mapto;
	std::map<const gate*, gate*> mapback;
	std::queue<gate*> que;
	/*
	* Consider the new circuit is on the left, C on the right
	*          mapto
	*       ---------->
	* this               C
	*       <----------
	*         mapback
	*/

	mapto[nullptr] = nullptr;
	mapback[nullptr] = nullptr;

	for (int i(0); i != in.size(); ++i) {
		mapto[in[i]] = C.in[i];
		mapback[C.in[i]] = in[i];
		que.push(in[i]);
	}
	for (int i(0); i != out.size(); ++i) {
		mapto[out[i]] = C.out[i];
		mapback[C.out[i]] = out[i];
		que.push(out[i]);
	}
	while (!que.empty()) {
		gate* now(que.front());
		que.pop();
		const gate* cor = mapto[now];
		for (const gate* right : cor->output) {
			gate* left(nullptr);
			if (mapback.find(right) == mapback.end()) {
				left = mapback[right] = new gate(right->type);
				que.push(left);
			} else {
				left = mapback[right];
			}
			now->output.push_back(left);
		}
		for (int i(0); i != 2; ++i) {
			const gate* right = cor->input[i];
			gate* left(nullptr);
			if (mapback.find(right) == mapback.end()) {
				left = mapback[right] = new gate(right->type);
				que.push(left);
			} else {
				left = mapback[right];
			}
			now->input[i] = left;
		}
	}
}

// Deconstruct all gates in the circuit.
void circuit::clear() {
	std::queue<gate*> que;
	std::set<gate*> set;
	set.insert(nullptr);
	for (gate* g : in) {
		que.push(g);
		set.insert(g);
	}
	for (gate* g : out) {
		que.push(g);
		set.insert(g);
	}
	while (!que.empty()) {
		gate* now(que.front());
		que.pop();
		for (gate* to : now->output) {
			if (set.find(to) == set.end()) {
				set.insert(to);
				que.push(to);
			}
		}
		for (int i(0); i != 2; ++i) {
			if (set.find(now->input[i]) == set.end()) {
				set.insert(now->input[i]);
				que.push(now->input[i]);
			}
		}
	}
	for (gate* g : set) {
		delete g;
	}
	in.clear();
	out.clear();
}

void circuit::moderate_clear() {
	in.clear();
	out.clear();
}

circuit::~circuit() {
	clear();
}

bool circuit::check() const {
	std::queue<const gate*> que;
	std::set<const gate*> set, inout;
	set.insert(nullptr);
	for (const gate* g : out) {
		if (g->type != gate::OUTPUT) return false;
		que.push(g);
		set.insert(g);
		inout.insert(g);
	}
	for (const gate* g : in) {
		if (g->type != gate::INPUT) return false;
		que.push(g);
		set.insert(g);
		inout.insert(g);
	}
	while (!que.empty()) {
		const gate* now(que.front());
		que.pop();
		if (now->check() == false) return false;
		if (inout.find(now) == inout.end()) {
			if (now->type == gate::INPUT || now->type == gate::OUTPUT) {
				return false;
			}
		}
		for (const gate* g : now->output) {
			if (set.find(g) == set.end()) {
				set.insert(g);
				que.push(g);
			}
		}
		for (int i(0); i != 2; ++i) {
			if (set.find(now->input[i]) == set.end()) {
				set.insert(now->input[i]);
				que.push(now->input[i]);
			}
		}
	}
	return true;
}



void circuit::clear_state() {
	std::queue<gate*> que;
	std::set<gate*> set;
	set.insert(nullptr);
	for (gate* g : in) {
		que.push(g);
		set.insert(g);
	}
	while (!que.empty()) {
		gate* now(que.front());
		que.pop();
		now->ready_inputs = 0;
		for (gate* g : now->output) {
			if (set.find(g) == set.end()) {
				set.insert(g);
				que.push(g);
			}
		}
	}
}

std::vector<bool> circuit::eval(const std::vector<bool>& input) {
	if (in.size() != input.size()) return {}; // invalid input.
	if (out.empty()) return {}; // nothing to output.
	clear_state();
	std::queue<gate*> que;
	for (int i(0); i != in.size(); ++i) {
		gate* g = in[i];
		g->ready_inputs = 2;
		g->val = input[i];
		que.push(g);
	}
	while (!que.empty()) {
		gate* now(que.front());
		que.pop();
		now->ready_inputs = 0;
		for (gate* g : now->output) {
			if (++g->ready_inputs == 2) {
				switch (g->type) {
				case gate::AND:
					g->val = (g->input[0]->val && g->input[1]->val);
					break;
				case gate::OR:
					g->val = (g->input[0]->val || g->input[1]->val);
					break;
				case gate::XOR:
					g->val = (g->input[0]->val ^ g->input[1]->val);
					break;
				default:
					return {}; // ill-formed circuit.
				}
				que.push(g);
			}
		}
	}
	std::vector<bool> ret;
	for (gate* g : out) {
		if (g->ready_inputs != 1) return {};
		ret.push_back(g->input[0]->val);
	}
	return ret;
}

void circuit::copy_from(const circuit& C) {
	clear();
	*this = circuit(C);
}

circuit::circuit(int fanin, int fanout)
	: in(fanin, nullptr), out(fanout, nullptr)
{
	for (int i(0); i != fanin; ++i) in[i] = new gate(gate::INPUT);
	for (int i(0); i != fanout; ++i) out[i] = new gate(gate::OUTPUT);
}

gate::gate()
	: input{}, output{}, type(END_OT_TYPE), ready_inputs(0), val(0)
{
}

gate::gate(gate_type t)
	: input{}, output{}, type(t), ready_inputs(0), val(0) {
}

bool gate::check() const {
	if (type < 0 && type >= END_OT_TYPE) return false;
	switch (type)
	{
	case INPUT:
		if (input[0] != nullptr || input[1] != nullptr) return false;
		if (output.empty()) return false;
		break;
	case OUTPUT:
		if (input[0] == nullptr || input[1] != nullptr) return false;
		if (!output.empty()) return false;
		break;
	default:
		if (input[0] == nullptr || input[1] == nullptr) return false;
		if (output.empty()) return false;
		break;
	}
	return true;
}

void gate::concat(gate* g) {
	if (g == nullptr) throw "Trying to connect to NULL.";
	gate* tem = nullptr;
	if (g->type == gate::OUTPUT) {
		concat(g->input[0]);
		delete g;
		return;
	}
	if (type == gate::INPUT) {
		if (output.empty()) throw "INPUT gate has no output.";
		for (gate* out : output) {
			out->disconnect(this);
			out->concat(g);
		}
		if (tem) delete tem;
		delete this;
		return;
	}
	if (g == nullptr) throw "Trying to connect to NULL.";
	if (input[0] == nullptr) {
		input[0] = g;
		g->output.push_back(this);
		if (tem) delete tem;
		return;
	} else {
		if (type == OUTPUT) throw "OUTPUT gate has only one input.";
		if (input[0] == g) throw "Trying to concat two same gates as input.";
		if (input[1] == nullptr) {
			input[1] = g;
			g->output.push_back(this);
			if (tem) delete tem;
			return;
		}
		throw "No room for more inputs.";
	}
}

void gate::concat(gate* g1, gate* g2) {
	if (input[0] != nullptr || input[1] != nullptr) throw "Input gates already connected.";
	if (type == INPUT || type == OUTPUT) throw "Invalid gate concatenation.";
	concat(g1);
	concat(g2);
}

void gate::disconnect(gate* g) {
	if (input[0] == g) input[0] = nullptr;
	if (input[1] == g) input[1] = nullptr;
	throw "Nothing to be disconnected.";
}

void gate::init(gate* arr[], int sz, gate_type type) {
	for (int i(0); i != sz; ++i) {
		arr[i] = new gate(type);
	}
}


void demo_circuit() {
	circuit C;
	gate* gout(new gate(gate::OUTPUT));
	gate* gand(new gate(gate::XOR));
	gate* gin[2] = { new gate(gate::INPUT), new gate(gate::INPUT) };
	C.out.push_back(gout);
	C.in.push_back(gin[0]);
	C.in.push_back(gin[1]);
	gand->concat(gin[0]), gand->concat(gin[1]);
	gout->concat(gand);
	std::cout << C.check() << std::endl;
	if (C.check()) {
		for (int i(0); i != 2; ++i) {
			for (int j(0); j != 2; ++j) {
				std::cout << C.eval({ bool(i), bool(j) })[0] << std::endl;
			}
		}
	}
}