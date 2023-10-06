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
	}
}

// Deconstruct all gates in the circuit.
void circuit::clear() {
	std::queue<gate*> que;
	std::unordered_set<gate*> set;
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

void circuit::check() const {
	std::queue<const gate*> que;
	std::unordered_set<const gate*> set, setin, setout;
	set.insert(nullptr);
	for (const gate* g : out) {
		setout.insert(g);
	}
	for (const gate* g : in) {
		if (g->type != gate::INPUT) throw "Input gate is not of type INPUT.";
		que.push(g);
		set.insert(g);
		setin.insert(g);
	}
	while (!que.empty()) {
		const gate* now(que.front());
		que.pop();
		now->check();
		for (const gate* g : now->output) {
			if (set.find(g) == set.end()) {
				set.insert(g);
				que.push(g);
			}
		}
		if (now->type != gate::INPUT) {
			for (int i(0); i != 2; ++i) {
				if (now->type == gate::NOT && i) break;
				bool flag = false;
				for (gate* g : now->input[i]->output) {
					if (g == now) {
						flag = true;
						break;
					}
				}
				if (!flag) throw "A gate is not marked as output by its input gate.";
			}
		}
	}
}



void circuit::clear_state() {
	std::queue<gate*> que;
	std::unordered_set<gate*> set;
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
	// clear_state();
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
			} else if (g->ready_inputs == 1 && g->type == gate::NOT) {
				g->val = !g->input[0]->val;
				que.push(g);
			}
		}
	}
	std::vector<bool> ret;
	for (gate* g : out) {
		ret.push_back(g->val);
	}
	return ret;
}

int circuit::size() const {
	int sz = 0;
	std::queue<gate*> que;
	std::unordered_set<gate*> set;
	for (gate* g : in) {
		que.push(g);
		set.insert(g);
	}
	while (!que.empty()) {
		gate* now(que.front());
		que.pop();
		if (now->type != gate::INPUT && now->type != gate::NOT) {
			++sz;
		}
		for (gate* g : now->output) {
			if (set.find(g) == set.end()) {
				que.push(g);
				set.insert(g);
			}
		}
	}
	return sz;
}

void circuit::print() const {
	int next_num(1);
	std::map<const gate*, int> hash;
	std::queue<const gate*> que;
	std::unordered_set<const gate*> set;
	for (auto g : in) {
		hash[g] = next_num++;
		que.push(g);
		set.insert(g);
	}
	for (auto g : out) hash[g] = next_num++;
	while (!que.empty()) {
		const gate* now(que.front());
		que.pop();

		std::cout << hash[now] << " (" << now->name() << "=" << now->val << ") --> ";

		for (auto g : now->output) {
			if (set.find(g) == set.end()) {
				if (hash.find(g) == hash.end()) hash[g] = next_num++;
				que.push(g);
				set.insert(g);
			}
			std::cout << hash[g] << " ";
		}
		bool found(false);
		for (auto g : out) {
			if (g == now) {
				found = true;
				break;
			}
		}
		if (found) std::cout << "OUTPUT";
		else if (now->output.empty()) std::cout << "VOID";
		std::cout << std::endl;

	}
}

void circuit::remove_void() {
	int next_num(1);
	std::map<const gate*, int> hash;
	std::queue<const gate*> que;
	std::queue<const gate*> voidque;
	std::unordered_set<const gate*> set;
	for (auto g : in) {
		hash[g] = next_num++;
		que.push(g);
		set.insert(g);
	}
	for (auto g : out) hash[g] = next_num++;
	while (!que.empty()) {
		const gate* now(que.front());
		que.pop();

		for (auto g : now->output) {
			if (set.find(g) == set.end()) {
				if (hash.find(g) == hash.end()) hash[g] = next_num++;
				que.push(g);
				set.insert(g);
			}
		}

		if (now->output.empty()) {
			bool found(false);
			for (auto g : out) {
				if (g == now) {
					found = true;
					break;
				}
			}
			int rank(hash[now]);
			if (!found) voidque.push(now);
		}
	}
	while (!voidque.empty()) {
		const gate* now(voidque.front());
		voidque.pop();
		int rank = hash[now];
		std::vector<gate*> ingate;
		if (now->type == gate::INPUT) throw "INPUT gate not used.";
		if (now->input[0] == nullptr) throw "Left input wire not connected.";
		else ingate.push_back(now->input[0]);
		if (now->input[1] == nullptr) {
			if (now->type != gate::NOT) throw "Right input wire not connected.";
		} else ingate.push_back(now->input[1]);

		for (gate* g : ingate) {
			bool found(false);
			for (auto itr(g->output.begin()); itr != g->output.end(); ++itr) {
				if (*itr == now) {
					found = true;
					g->output.erase(itr);
					break;
				}
			}
			if (!found) throw "Not in the output gate list.";
			if (g->output.empty()) {
				bool isoutput(false);
				for (auto gout : out) {
					if (g == gout) {
						isoutput = true;
						break;
					}
				}
				if (!isoutput) voidque.push(g);
			}
		}

		delete now;
	}
}

void circuit::save(std::ostream& stream) {
	int next_num(1);
	std::map<const gate*, int> hash;
	std::queue<const gate*> que;
	std::unordered_set<const gate*> set;
	for (auto g : in) {
		hash[g] = next_num++;
		que.push(g);
		set.insert(g);
	}
	for (auto g : out) hash[g] = next_num++;
	while (!que.empty()) {
		const gate* now(que.front());
		que.pop();
		stream << hash[now] << " " << now->type_name() << " --> ";
		for (auto g : now->output) {
			if (set.find(g) == set.end()) {
				if (hash.find(g) == hash.end()) hash[g] = next_num++;
				que.push(g);
				set.insert(g);
			}
			stream << hash[g] << " ";
		}
		bool found(false);
		for (auto g : out) {
			if (g == now) {
				found = true;
				break;
			}
		}
		if (found) stream << "OUTPUT";
		else if (now->output.empty()) stream << "VOID";
		std::cout << std::endl;
	}
}

void circuit::copy_from(const circuit& C) {
	clear();
	*this = circuit(C);
}

circuit::circuit(int fanin, int fanout)
	: in(fanin, nullptr), out(fanout, nullptr)
{
	for (int i(0); i != fanin; ++i) in[i] = new gate(gate::INPUT);
	for (int i(0); i != fanout; ++i) out[i] = nullptr;
}

gate::gate()
	: input{}, output{}, type(END_OF_TYPE), ready_inputs(0), val(0)
{
}

gate::gate(gate_type t)
	: input{}, output{}, type(t), ready_inputs(0), val(0) {
}

void gate::check() const {
	if (type < 0 && type >= END_OF_TYPE) throw "Unknown gate.";
	switch (type)
	{
	case INPUT:
		if (input[0] != nullptr || input[1] != nullptr) throw "Input gate missing.";
			if (output.empty()) throw "Input gate not used.";
		break;
	case NOT:
		if (input[0] == nullptr || input[1] != nullptr) throw "NOT gate should have only one input.";
		break;
	default:
		if (input[0] == nullptr || input[1] == nullptr) throw "Input gate missing.";
		break;
	}
}

void gate::concat(gate* g) {
	if (g == nullptr) throw "Trying to connect to NULL.";
	gate* tem = nullptr;
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
		if (input[0] == g) throw "Trying to concat two same gates as input.";
		if (type == NOT) throw "NOT gate can only have one input.";
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
	if (type == INPUT) throw "Invalid gate concatenation.";
	concat(g1);
	concat(g2);
}

void gate::disconnect(gate* g) {
	if (input[0] == g) input[0] = nullptr;
	else if (input[1] == g) input[1] = nullptr;
	else throw "Nothing to be disconnected.";
}

void gate::init(gate* arr[], int sz, gate_type type) {
	for (int i(0); i != sz; ++i) {
		arr[i] = new gate(type);
	}
}

void gate::name(const std::string& newname) {
	nm = newname;
}

std::string gate::name() const {
	switch (type) {
	case gate::NOT:
		return "NOT" + nm;
	case gate::AND:
		return "AND" + nm;
	case gate::OR:
		return "OR" + nm;
	case gate::XOR:
		return "XOR" + nm;
	case gate::INPUT:
		return "INPUT" + nm;
	case gate::END_OF_TYPE:
		return "END_OF_TYPE" + nm;
	default:
		throw "Invaild gate type.";
	}
	throw "WTF have you done???";
	return "";
}

std::string gate::type_name() const {
	switch (type) {
	case gate::NOT:
		return "NOT";
	case gate::AND:
		return "AND";
	case gate::OR:
		return "OR";
	case gate::XOR:
		return "XOR";
	case gate::INPUT:
		return "INPUT";
	case gate::END_OF_TYPE:
		return "END_OF_TYPE";
	default:
		throw "Invaild gate type.";
	}
	throw "WTF have you done???";
	return "";
}


void demo_circuit() {
	circuit C;
	gate* gand(new gate(gate::XOR));
	gate* gin[2] = { new gate(gate::INPUT), new gate(gate::INPUT) };
	C.out.push_back(gand);
	C.in.push_back(gin[0]);
	C.in.push_back(gin[1]);
	gand->concat(gin[0]), gand->concat(gin[1]);
	C.check();
	for (int i(0); i != 2; ++i) {
		for (int j(0); j != 2; ++j) {
			std::cout << C.eval({ bool(i), bool(j) })[0] << std::endl;
		}
	}
}