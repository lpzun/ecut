//============================================================================
// Name        : explores.cpp
// Author      : Peizun Liu
// Version     :
// Copyright   : The copyright belongs to CAV group of CCIS at NEU
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>

#include "functions.hh"
#include "input.hh"

using namespace std;

class Thread_State {
public:
	ushort shared;
	ushort local;

	inline Thread_State(cushort& shared, cushort& local);
	inline Thread_State();

	ostream& to_stream(ostream& out = cout) const;
};

typedef pair<ushort, Set<ushort> > Thread_State_Combo;

bool operator <(const Thread_State& t1, const Thread_State& t2) {
	cshort compare_shareds = COMPARE::compare(t1.shared, t2.shared);
	if (compare_shareds != 0)
		return compare_shareds == -1;
	cshort compare_locals = COMPARE::compare(t1.local, t2.local);
	return compare_locals == -1;
}

inline bool horizontal(const Thread_State& t1, const Thread_State& t2) {
	return t1.shared == t2.shared;
}
inline bool vertical(const Thread_State& t1, const Thread_State& t2) {
	return t1.local == t2.local;
}

inline ostream& operator <<(ostream& out, const Thread_State& t) {
	return t.to_stream(out);
}

inline ostream& Thread_State::to_stream(ostream& out) const {
	return out << shared << " " << local;
}

class State {
public:
	ushort shared;
	Vector<ushort> locals;

	ushort depth; // depth where state was first reached

	static ushort n; // number of threads
	static ushort s; // number of shared states
	static ushort l; // number of local states

	inline State(const Thread_State& t) :
			shared(t.shared), locals(vector<ushort>(n, t.local), " "), depth(0) {
	}

	ostream& to_stream(ostream& out = cout, const string& sep = "|") const;
};

ushort State::n;
ushort State::s;
ushort State::l;

inline ostream& operator <<(ostream& out, const State& g) {
	return g.to_stream(out);
}

ostream& State::to_stream(ostream& out, const string& sep) const {
	return (out << shared << sep << locals << " (depth " << depth << ")");
}

inline bool operator ==(const State& g1, const State& g2) {
	return g1.shared == g2.shared && g1.locals == g2.locals;
}
inline bool operator !=(const State& g1, const State& g2) {
	return !(g1 == g2);
}

bool operator <(const State& g1, const State& g2) {
	cshort compare_shareds = COMPARE::compare(g1.shared, g2.shared);
	if (compare_shareds != 0)
		return compare_shareds == -1;
	cshort compare_locals = COMPARE::compare_container(g1.locals, g2.locals);
	return compare_locals == -1;
}

inline Thread_State::Thread_State(cushort& shared, cushort& local) :
		shared(shared), local(local) {
#ifdef __SAFE_COMPUTATION__
	assert (shared < State::s);
	assert (local < State::l);
#endif
}

inline Thread_State::Thread_State() :
		shared(Random::integer(0, State::s - 1)), local(Random::integer(0, State::l - 1)) {
}

void print(const map<Thread_State, set<Thread_State> >& adjacency_list, ostream& out = cout) {
	out << State::s << " " << State::l << endl;
	for (map<Thread_State, set<Thread_State> >::const_iterator pair = adjacency_list.begin(), end =
			adjacency_list.end(); pair != end; ++pair) {
		const Thread_State& t = pair->first;
		const Set<Thread_State>& successors = pair->second;
		__SAFE_ASSERT__ (! successors.empty());
		for (Set<Thread_State>::const_iterator succ = successors.begin(), end = successors.end(); succ != end; ++succ)
			out << t << " -> " << (*succ) << endl;
	}
}

void add_successor(const Thread_State& t, set<Thread_State>& successors) {
	while (true) {
		Thread_State succ; // random thread state
		if (succ != t && !successors.count(succ)) {
			successors.insert(succ);
			break;
		}
	}
}

int main(const int argc, const char * const * const argv) {

	try {

		Input I;

		I.add_argument("-f", "input file (if given, don't specify s,l,e)", "", "X");
		I.add_argument("n", "number of threads");
		I.add_argument("s", "number of shared states (i.e. 1 shared variable with range [0..(s-1)])", "", "0");
		I.add_argument("l", "number of local  states", "", "0");
		I.add_argument("e", "number of thread transitions (no self loops; enter 0 to get info on maximum value)", "",
				"0");
		I.add_argument("-init-shared", "initial value of shared variable", "", "0");
		I.add_argument("-init-local", "initial value of local  variable", "", "0");
		I.add_switch("-globals", "whether to print the reached global states");
		I.add_switch("-threads", "whether to print the reached thread states");
		I.add_switch("-combos", "whether to print the reached thread state combos");
		I.add_switch("-adj-list", "whether to print the adjacency list");
		I.add_switch("-cmd-line", "whether to print the command line");
		I.add_switch("-all", "whether to print all of the above");

		try {
			I.get_command_line(argc, argv);
		} catch (Input::Help) {
			return 0;
		}

		const bool all = I.arg2bool("-all");
		const bool globals = I.arg2bool("-globals") || all;
		const bool threads = I.arg2bool("-threads") || all;
		const bool combos = I.arg2bool("-combos") || all;

		if (I.arg2bool("-cmd-line") || all)
			I.print_command_line(0);

		map<Thread_State, set<Thread_State> > adjacency_list;

		const string filename = I.arg_value("-f");
		const ushort init_shared = I.arg2long("-init-shared");
		const ushort init_local = I.arg2long("-init-local");
		State::n = atoi(I.arg_value("n").c_str());

		if (filename == "X") { // make random structure
			State::s = atoi(I.arg_value("s").c_str());
			State::l = atoi(I.arg_value("l").c_str());
			ushort e = atol(I.arg_value("e").c_str());
			culong T = State::s * State::l; // number of thread states
			culong e_max = T * (T - 1);
			if (e == 0) { // if there is no transition, print the following message and return 0
				cout << "maximum number of thread transitions in this configuration: " << e_max << endl;
				return 0;
			}
			assert(e <= e_max);
			Random::set_seed();
			Thread_State init(init_shared, init_local);
			add_successor(init, adjacency_list[init]); // initial state must have a successor
			for (ulong i = 1; i < e; ++i) {
				__CERR__ ("currently adding edge " << i+1 << endl);
				while (true) {
					Thread_State t; // random thread state
					set<Thread_State>& successors = adjacency_list[t]; // must be created unless exists
					if (!successors.count(t)) {
						add_successor(t, successors);
						break;
					} else if (successors.size() < T - 1) { // t does not already point to every other thread state
						add_successor(t, successors);
						break;
					}
				}
			}

			ofstream last("Examples/last.ttd");
			print(adjacency_list, last);
		} else {
			ifstream orig(filename.c_str());
			CONTROL::remove_comments(orig, "/tmp/tmp.ttd.no_comment", "#");
			orig.close();
			ifstream in("/tmp/tmp.ttd.no_comment");
			in >> State::s >> State::l;
			ushort s1, l1, s2, l2;
			string sep;
			while (in) {
				in >> s1 >> l1 >> sep >> s2 >> l2;
				__SAFE_ASSERT__ (sep == "->");
				adjacency_list[Thread_State(s1, l1)].insert(Thread_State(s2, l2));
			}
		}

		if (I.arg2bool("-adj-list") || all) {
			cout << "Adjacency list:" << endl;
			print(adjacency_list);
		}

		culong N = State::s * LOGARITHM::power(culong(State::l), State::n); // number of global states: l^n
		culong T = State::s * State::l;          // number of thread states: s*l
		culong C = State::s * (LOGARITHM::power(culong(2), State::l) - 1); // number of thread state combos: s*(2^l-1)

		Thread_State thread_init(init_shared, init_local);
		State init(thread_init);
		// cout << "Initial state: " << init << endl;

		set<State> reached; //reachable states set
		queue<set<State>::const_iterator> unexpanded; //what's this?
		reached.insert(init);
		unexpanded.push(reached.begin());
		ushort current_depth = 0;
		while (!unexpanded.empty()) {
			set<State>::const_iterator gg = unexpanded.front();
			unexpanded.pop();
			const State& g = *gg;
			const ushort& g_depth = g.depth;

			if (g_depth != current_depth) { // we reached a new depth
				__SAFE_ASSERT__ (current_depth == g_depth - 1); //
				__CERR__ ("all states of depth at most " << PPRINT::widthify(current_depth,3,PPRINT::RIGHTJUST) << " " << "have been expanded; "); //
				__CERR__ ("|unexpanded| = " << PPRINT::widthify(unexpanded.size() + 1,7,PPRINT::RIGHTJUST) << endl); // "+1" since g not yet expanded
				current_depth = g_depth;
			}

			cushort& shared = g.shared;
			for (ushort i = 0; i < State::n; ++i) {
				const Thread_State t(shared, g.locals[i]);
				if (adjacency_list.find(t) != adjacency_list.end()) {
					const set<Thread_State>& successors = adjacency_list[t];
					for (set<Thread_State>::const_iterator t_ssucc = successors.begin(), end = successors.end();
							t_ssucc != end; ++t_ssucc) {
						const Thread_State& t_succ = *t_ssucc;
						State succ = g;
						succ.shared = t_succ.shared;
						succ.locals[i] = t_succ.local;
						sort(succ.locals.begin(), succ.locals.end()); // symmetry reduction
						succ.depth = g_depth + 1;
						pair<set<State>::const_iterator, bool> pair = reached.insert(succ);
						if (pair.second) { // succ is new
							unexpanded.push(pair.first);
						}
					}
				}
			}
		}

		cout << endl << PPRINT::plural(reached.size(), "state") << " reached (out of " << N << " conceivable)"
				<< (globals ? ":" : "") << endl;

		// go through the globals and extract reached thread states and combos
		map<Thread_State, ushort> reached_thread_states;
		map<Thread_State_Combo, ushort> reached_thread_state_combos;
		for (ushort d = 0; d <= current_depth; ++d)
			for (set<State>::const_iterator gg = reached.begin(), end = reached.end(); gg != end; ++gg) {
				const State& g = *gg;
				if (g.depth == d) {
					if (globals)
						cout << g << endl;
					ushort shared = g.shared;
					Thread_State_Combo c(shared, Set<ushort>(" "));
					Set<ushort>& combo_set = c.second;
					for (ushort i = 0; i < State::n; ++i) {
						cushort& local = g.locals[i];
						const Thread_State t(shared, local);
						pair<Thread_State, ushort> p(t, d);
						reached_thread_states.insert(p);
						combo_set.insert(local);
					}
					pair<Thread_State_Combo, ushort> p(c, d);
					reached_thread_state_combos.insert(p);
				}
			}

		cout << endl;

		cout << PPRINT::plural(reached_thread_state_combos.size(), "thread state combo") << " reached (out of " << C
				<< " conceivable)" << (combos ? ":" : "") << endl;
		for (ushort d = 0; d <= current_depth; ++d) {
			bool depth_found = false;
			for (map<Thread_State_Combo, ushort>::const_iterator cc = reached_thread_state_combos.begin(), end =
					reached_thread_state_combos.end(); cc != end; ++cc)
				if (cc->second == d)
					if (combos) {
						depth_found = true;
						cout << cc->first.first << "|" << cc->first.second << " (depth = " << setw(3) << d << ")"
								<< endl;
					}
			if (combos && !depth_found)
				cout << "    (depth = " << setw(3) << d << ")" << endl;
		}

		cout << endl;

		cout << PPRINT::plural(reached_thread_states.size(), "thread state") << " reached (out of " << T
				<< " conceivable)" << (threads ? ":" : "") << endl;
		for (ushort d = 0; d <= current_depth; ++d) {
			bool depth_found = false;
			for (map<Thread_State, ushort>::const_iterator tt = reached_thread_states.begin(), end =
					reached_thread_states.end(); tt != end; ++tt)
				if (tt->second == d)
					if (threads) {
						depth_found = true;
						cout << tt->first << " (depth = " << setw(3) << d << ")" << endl;
					}
			if (threads && !depth_found)
				cout << "    (depth = " << setw(3) << d << ")" << endl;
		}

		cout << endl;

		// Checking for thread states reachable passively in the future.
		bool passive_found = false;

		// checking for candidate triples
		for (map<Thread_State, ushort>::const_iterator pair = reached_thread_states.begin(), end =
				reached_thread_states.end(); pair != end; ++pair) {
			const Thread_State& t = pair->first;
			const set<Thread_State>& successors = adjacency_list[t];
			for (set<Thread_State>::const_iterator uu = successors.begin(), end = successors.end(); uu != end; ++uu) {
				const Thread_State& u = *uu;
				__SAFE_ASSERT__ (reached_thread_states.find(u) != reached_thread_states.end()); // sanity
				if (t.shared != u.shared) { // not horizontal (it may be vertical, though)
					for (ushort local = 0; local < State::l; ++local) {
						if (local != u.local) {           // follows from p != u
							const Thread_State v(t.shared, local);
							const Thread_State p(u.shared, local); // the passively reached thread state
							if (reached_thread_states.find(v) != reached_thread_states.end() && //     v     must     be reachable
									reached_thread_states.find(p) == reached_thread_states.end()) { // candidate must not be reachable
								passive_found = true;
								cout << "candidate triple found: " << "t = " << t << ", u = " << u << ", v = " << v
										<< ", p = " << p << endl;
							}
						}
					}
				}
			}
		}

		if (!passive_found)
			cout << " *** no candidate triple found *** (which means cutoff)" << endl;

		return 0;
	}

	catch (const CONTROL::Error& error) {
		error.print_exit();
	} catch (...) {
		CONTROL::Error("main: unknown exception occurred").print_exit();
	}
}
