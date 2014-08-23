/*
 * functions.h
 *
 *  Created on: Oct 17, 2012
 *      Author: lpzun
 */

#ifndef __FUNCTIONS_HH__
#define __FUNCTIONS_HH__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <string>
#include <sstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <cctype>
#include <list>
#include <set>
#include <vector>
#include <map>
#include <queue>
#include <utility>
#include <iterator>
#include <algorithm>

using std::string;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;

using std::list;
using std::set;
using std::vector;
using std::map;
using std::queue;

typedef const short cshort;
typedef const int cint;
typedef const long clong;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef const unsigned short cushort;
typedef const unsigned int cuint;
typedef const unsigned long culong;

namespace CONTROL {
// If in is empty, return false. Otherwise, return true and append
// string from current pointer until but excluding eol or eof to line
bool getline(istream& in, string& line, const char& eol = '\n');

// Tip: can read a file line by line like this:
// string line;
// while (CONTROL::getline(in, line = ""))
//   <process line>;

inline void clear(istream& in) {
	string line;
	getline(in, line);
} // skip until eol or eof

// remove "// .*" style comments
void remove_comments(istream& in, ostream& out, const string& comment);
void remove_comments(istream& in, const string& filename,
		const string& comment);
void remove_comments(const string& in, string& out, const string& comment);

// Print message on out and wait for <RETURN>. Excess characters are removed
inline void wait_return(const string& message = "Please press <RETURN>: ",
		ostream& out = cout) {
	out << message;
	clear(cin);
}

// Print message on out and wait for 'y' or 'n' answer. Return true iff 'y'
bool wait_yes_no(const string& message = "", ostream& out = cout);

// When writing a piece of software, distinguish between different two types of runtime tests:

// 1. Those that check integrity of user input. Such tests should always be executed.
//    Use Error class below to indicate failure (to allow for clean exception handling).
//    Generally, do not turn those checks off. They only happen during reading of input.
//    The reading is most likely slower than the checking. "Input" includes command line, stdin,
//    file input, etc. The program has no way of knowing where these data come from.

// 2. Those that check program invariants and thus verify whether the program is correct.
//    Use the __SAFE_... macros below for those tests. A simple "crash" due to a failed assert is enough.
//    Once the program is stable, these tests can be turned off by undefining the macro.

// It seems unwise to have every function test its input for sanity
// before succeeding. Often, functions are called with computed data, not user input,
// so the check is a waste if the program is correct. Use __SAFE_ASSERT__ instead.

inline void warning(const string& message = "(internal)", ostream& out = cout) {
	out << "warning: " << message << endl;
}

struct Error {
	string message;
	short code;
	inline Error(const string& message, cshort& code = -1) :
			message(message), code(code) {
	}    // must provide some message
	inline void print(ostream& out = cout) const {
		out << "Error: " << message << endl;
	} // print on cout by default (cerr may be diverted)
	inline void print_exit(ostream& out = cout) const {
		print(out);
		exit(code);
	}
};
}

class Timing {
	static ulong stamp;

public:
	static inline void start_time() {
		stamp = time(0);
	}
	static inline void start_time(ostream& out) {
		out << "starting time" << endl;
		start_time();
	}
	static inline ulong take_time() {
		return time(0) - stamp;
	}
	static inline ulong reset_time() {
		culong result = take_time();
		start_time();
		return result;
	}
	static inline void wait_seconds(cushort& seconds) {
		reset_time();
		do
			; while (take_time() < seconds);
	}
};

// Evaluate assertion if __SAFE_COMPUTATION__ is defined. If not, __SAFE_ASSERT__ results in an empty statement ";"
// Careful: in macro definition, NO SPACE between macro name and formal arg list!
#ifdef __SAFE_COMPUTATION__
#define __SAFE_COMPUTE__(stmt) stmt
#define __SAFE_ASSERT__(cond) assert(cond)  // an abbreviation for: __SAFE_COMPUTE__ (assert cond)
#else
#define __SAFE_COMPUTE__(stmt)
#define __SAFE_ASSERT__(cond)
#endif

#ifdef __VERBOSE__
#define __VERBOSE_COMPUTE__(stmt) stmt
#define __CERR__(stuff) std::cerr << stuff  // print only in verbose mode. Use for debugging messages
#else
#define __VERBOSE_COMPUTE__(stmt)
#define __CERR__(stuff)
#endif

namespace COMPARE {
short compare(clong& x, clong& y);

// Returns 0 if equal, 1 if x > y, -1 if x < y. (think: sign(x-y))
// Motivation: containers have "bool operator <" defined, but this only returns "<" or ">=".
// When you need to distinguish between all three possibilities, compare is more efficient.
// This function was not called "compare" to avoid confusion with the above compare function
template<class T>
short compare_container(const T& x, const T& y) {
	typename T::const_iterator xi = x.begin(), yi = y.begin(), x_end = x.end(),
			y_end = y.end();
	while (true) {
		if (xi == x_end && yi == y_end) // x == y
			return 0;
		else if (xi == x_end)                // x < y
			return -1;
		else if (yi == y_end)                // y < x
			return +1;
		else if (*xi < *yi)                  // x < y
			return -1;
		else if (*yi < *xi)                  // y < x
			return +1;
		++xi, ++yi;
	}
	throw CONTROL::Error("COMPARE::compare: internal");
}

struct No_Number {
};
ulong string2num(const string& s);
}

namespace LOGARITHM {
// Returns, in log(i) time, a pair that contains floor(log_b(i)) (first) and ceil(log_b(i)) (second).
std::pair<ushort, ushort> floor_ceil_log(culong& i, cushort& base);

inline ushort floor_log(culong& i, cushort& base) {
	return floor_ceil_log(i, base).first;
} // floor(log(i))
inline ushort ceil_log(culong& i, cushort& base) {
	return floor_ceil_log(i, base).second;
} // ceil (log(i))

// Returns (log_base(i) \in N).
inline bool is_power(culong& i, cushort& base) {
	std::pair<ushort, ushort> p = floor_ceil_log(i, base);
	return p.first == p.second;
}

// Returns base^exp, for exp >= 0.
// T can be an integral or floating type, for example
// power((unsigned long long) 2, 10)
// power((long double) 10.3, 20)
template<class T>
T power(const T& base, cushort& exp) {
	T result = 1;
	for (ushort i = 0; i < exp; ++i)
		result *= base;
	return result;
}

// convert number in base b, given as vector of digits, into a decimal unsigned long
ulong n_ary2decimal(const vector<ushort>& v, cushort& base); // n-ary number to decimal
vector<ushort> decimal2n_ary(culong& l, cushort& base, cushort& size = 0); // decimal to n-ary. Result is vector of given size (min if 0)

bool inc(vector<bool>& bv);
}

namespace PPRINT {
typedef enum {
	LEFTJUST, RIGHTJUST, CENTERED
} Alignment;

// Convert object x into a string, padded with fill character as necessary to achieve target width.
// stringstream::operator<< must be defined for object x.
// If width = 0, just return string version of x
template<class T>
string widthify(const T& x, cushort& width = 0, const Alignment& c = CENTERED,
		const char& fill = ' ') {
	std::ostringstream os;
	os << x;
	string s = os.str();

	ushort n = s.size();
	if (n >= width)
		return s;
	ushort addlength = width - n;
	string result;
	switch (c) {
	case LEFTJUST:
		result = s + string(addlength, fill);
		break;
	case RIGHTJUST:
		result = string(addlength, fill) + s;
		break;
	case CENTERED:
		result = (
				addlength % 2 == 0 ?
						string(addlength / 2, fill) + s
								+ string(addlength / 2, fill) :
						string((addlength - 1) / 2, fill) + s
								+ string((addlength + 1) / 2, fill));
		break;
	}
	return result;
}

// Output string s, but with <sep> inserted as tabulator every tab character. Never at left or right end of s.
// Never if either left or right neighbor of tabulator position is ' '
string tabularize(const string& s, const string& sep = ",", cushort& tab = 3);

// Convert object x into a string using sprintf and the format string supplied (which must include the %).
// sprintf must be defined for object x (i.e. mainly basic types, such as numeric ones).
// If output is expected to be very long, provide sufficient length argument
template<class T>
string formatString(const T& x, const string& format, cushort& length = 10) {
	char* s = new char[length];
	snprintf(s, length, format.c_str(), x);
	string result = s;
	delete s;
	return result;
}

string hourize(culong& seconds); // given 20, produces "20s", given 80, produces "1:20m", given 3700, produces "1:01:40h"
inline string varname(const string& prefix = "") {
	static ushort varname_counter = 0;
	return prefix + "_" + widthify(varname_counter++);
}
inline string plural(culong& n, const string& name,
		const string& suffix = "s") {
	return widthify(n) + " " + name + (n == 1 ? "" : suffix);
}

// If firstTime is true, sets it to false, otherwise outputs connective. In any case appends common.
// Useful to print strings of expressions of the form
// x[0] + x[1] + ... + x[n-1] as follows:
// bool firstTime = true;
// for (ushort i = 0; i < n; ++i)
//   cout << PPRINT::firstTimeOrNot(firstTime, " + ", string("x[") + widthify(i) + "]");
string firstTimeOrNot(bool& firstTime, const string& connective,
		const string& common);
}

// The "main" function should call Random::set_seed
class Random {
public:
	static inline ulong set_seed() {
		ulong seed = time(0);
		srand(seed);
		return seed;
	} // seed varies each time
	static inline void set_seed(culong& seed) {
		srand(seed);
	}                        // repeat sequences by reusing the seed value

	static long integer(const long& min, const long& max); // in [min,max]. The bounds can be negative
	static float real(const float& min, const float& max); // in [min,max] or close to the bounds
														   // "double" is precision meaningless for a random number!
	static inline bool bit() {
		return integer(0, 1);
	}
};

// Generic container class
template<class T>
class Container {
protected:
	string sep; // default delimiter for stream output

public:
	inline Container(const string& sep) :
			sep(sep) {
	}

	ostream& to_stream(const T& c, ostream& out, const string& sep) const; // generic stream output

	// Given an iterator x, return next container element WITHOUT changing x (i.e. no ++ on original x).
	static inline typename T::const_iterator succ(
			typename T::const_iterator x) {
		return ++x;
	} // receives x as copy, not reference
	static inline typename T::const_reverse_iterator succ(
			typename T::const_reverse_iterator x) {
		return ++x;
	} // receives x as copy, not reference
};

template<class T>
ostream& Container<T>::to_stream(const T& c, ostream& out,
		const string& sep) const {
	for (typename T::const_iterator begin = c.begin(), end = c.end(), e = begin;
			e != end; ++e) {
		if (e != begin)
			out << sep;
		out << *e;
	}
	return out;
}

template<class T>
class Vector_base: public vector<T>, public Container<vector<T> > {
public:
	typedef Container<vector<T> > Container_type;
	inline Vector_base(const string& sep = "\n") :
			vector<T>(), Container_type(sep) {
	}
	inline Vector_base(const vector<T>& v, const string& sep = "\n") :
			vector<T>(v), Container_type(sep) {
	}
	inline Vector_base(const std::size_t& n, const string& sep = "\n") :
			vector<T>(n), Container_type(sep) {
	}
	inline Vector_base(const std::size_t& n, const T& x, const string& sep) :
			vector<T>(n, x), Container_type(sep) {
	}

	// Default arguments didn't work for the following definitions
	inline ostream& to_stream() const {
		return Container_type::to_stream(*this, cout, this->sep);
	}
	inline ostream& to_stream(ostream& out) const {
		return Container_type::to_stream(*this, out, this->sep);
	}
	inline ostream& to_stream(ostream& out, const string& sep) const {
		return Container_type::to_stream(*this, out, sep);
	}
};

template<class T>
class Vector: public Vector_base<T> {
public:
	inline Vector(const string& sep = "\n") :
			Vector_base<T>(sep) {
	}
	inline Vector(const vector<T>& v, const string& sep = "\n") :
			Vector_base<T>(v, sep) {
	}
	inline Vector(const std::size_t& n, const string& sep = "\n") :
			Vector_base<T>(n, sep) {
	}
	inline Vector(const std::size_t& n, const T& x, const string& sep = "\n") :
			Vector_base<T>(n, x, sep) {
	}
};

template<>
class Vector<string> : public Vector_base<string> {
public:
	inline Vector(const string& sep = "\n") :
			Vector_base<string>(sep) {
	}
	inline Vector(const vector<string>& v, const string& sep = "\n") :
			Vector_base<string>(v, sep) {
	}
	inline Vector(const std::size_t& n, const string& sep = "\n") :
			Vector_base<string>(n, sep) {
	}
	inline Vector(const std::size_t& n, const string& x, const string& sep) :
			Vector_base<string>(n, x, sep) {
	}
	Vector(const string& source, const string& delim, const string& sep = "\n");
};

template<class T>
class List_base: public list<T>, public Container<list<T> > {
public:
	typedef Container<list<T> > Container_type;
	inline List_base(const string& sep = "\n") :
			list<T>(), Container_type(sep) {
	}
	inline List_base(const list<T>& l, const string& sep = "\n") :
			list<T>(l), Container_type(sep) {
	}

	// Returns an iterator to a duplicate if exists, end() otherwise. Requires bool operator<(T,T)
	typename List_base::const_iterator duplicate() const {
		List_base<T> s = *this;
		s.sort();
		typedef typename List_base<T>::const_iterator const_iterator;
		for (const_iterator ii = s.begin(), end = s.end(); ii != end; ++ii) {
			typename List_base<T>::const_iterator succ = this->succ(ii);
			if (succ == end)
				break;
			if (*ii == *succ)
				return succ;
		}
		return this->end();
	}

	inline ostream& to_stream() const {
		return Container_type::to_stream(*this, cout, this->sep);
	}
	inline ostream& to_stream(ostream& out) const {
		return Container_type::to_stream(*this, out, this->sep);
	}
	inline ostream& to_stream(ostream& out, const string& sep) const {
		return Container_type::to_stream(*this, out, sep);
	}
};

template<class T>
class List: public List_base<T> {
public:
	inline List(const string& sep = "\n") :
			List_base<T>(sep) {
	}
	inline List(const list<T>& l, const string& sep = "\n") :
			List_base<T>(l, sep) {
	}
};

template<>
class List<string> : public List_base<string> {
public:
	inline List(const string& sep = "\n") :
			List_base<string>(sep) {
	}
	inline List(const list<string>& l, const string& sep = "\n") :
			List_base<string>(l, sep) {
	}
	List(const string& source, const string& delim, const string& sep = "\n");

	// If the list contains exactly one string that has the prefix, return iterator to this string.
	// If none, return end(). If more than two, error.
	List<string>::const_iterator find_from_prefix(const string& prefix) const;
};

template<class T>
class Set_base: public set<T>, public Container<set<T> > {
public:
	typedef Container<set<T> > Container_type;
	inline Set_base(const string& sep = "\n") :
			set<T>(), Container_type(sep) {
	}
	inline Set_base(const set<T>& s, const string& sep = "\n") :
			set<T>(s), Container_type(sep) {
	}

	// Sometimes we need the first/last element of a set, e.g. to determine lower/upper bound on the elements
	inline const T& front() const {
		return *(this->begin());
	} // compiler complains without "this->"
	inline const T& back() const {
		return *(this->rbegin());
	}

	inline bool singleton() const {
		if (this->empty())
			return false;
		return succ(this->begin()) == this->end();
	} // avoid "size"

	// random element if non-empty. I return an iterator here since it can be converted into a const reference but not vice versa
	typename Set_base<T>::const_iterator random() const {
		__SAFE_ASSERT__ (! this->empty());
		ulong i = Random::integer(0, this->size() - 1);
		for (typename Set_base<T>::const_iterator x = this->begin(), end =
				this->end(); x != end; ++x, --i)
			if (i == 0)
				return x;
		throw CONTROL::Error("Set_base::random: internal");
	}

	inline ostream& to_stream() const {
		return Container_type::to_stream(*this, cout, this->sep);
	}
	inline ostream& to_stream(ostream& out) const {
		return Container_type::to_stream(*this, out, this->sep);
	}
	inline ostream& to_stream(ostream& out, const string& sep) const {
		return Container_type::to_stream(*this, out, sep);
	}

private:
	// Assume bv is a bit vector representing a subset of s, convert it into the set of elements
	Set_base<T> subset(const vector<bool>& bv) const {
		__SAFE_ASSERT__ (this->size() == bv.size());
		Set_base<T> subset;
		ushort i = 0;
		for (typename Set_base<T>::const_iterator x = this->begin(), end =
				this->end(); x != end; ++x, ++i)
			if (bv[i])
				subset.insert(subset.end(), *x);
		return subset;
	}

	// Assume bv is a vector of pointers to set elements, convert it into the set of elements
	Set_base<T> subset(vector<typename Set_base<T>::const_iterator>& iv) const {
		ushort size = iv.size();
		__SAFE_ASSERT__ (size <= this->size());
		Set_base<T> subset;
		for (ushort i = 0; i < size; ++i)
			subset.insert(subset.end(), *(iv[i]));
		return subset;
	}

	bool inc(vector<typename Set_base<T>::const_iterator>& iv) const {
		if (iv.empty())
			return false;
		ushort size = iv.size(), last = size - 1;
		long pos = last;
		while (!((pos == last && succ(iv[pos]) != this->end())
				|| (pos < last && succ(iv[pos]) != iv[pos + 1])))
			if (--pos < 0)
				return false;
		++(iv[pos]);
		typename Set_base<T>::const_iterator next = succ(iv[pos++]);
		while (pos < size)
			iv[pos++] = next++;
		return true;
	}

public:
	struct Read_subset {
		virtual void operator ()(const Set_base<T>& t) = 0;
		virtual ~Read_subset() {
		}
	};

	void enumerate(Read_subset& read_subset) const { // all subsets
		vector<bool> bv(this->size(), 0);
		do {
			const Set_base<T> subset = this->subset(bv);
			read_subset(subset);
		} while (LOGARITHM::inc(bv));
	}

	void enumerate(Read_subset& read_subset, cushort& card) const { // subsets of given cardinality
		__SAFE_ASSERT__ (card <= this->size());
		vector<typename Set_base<T>::const_iterator> iv;
		iv.reserve(card);
		typename Set_base<T>::const_iterator x = this->begin();
		ushort count = card;
		while (count > 0) {
			iv.push_back(x++);
			--count;
		}
		do {
			const Set_base<T> subset = this->subset(iv);
			read_subset(subset);
		} while (LOGARITHM::inc(iv));
	}
};

template<class T>
class Set: public Set_base<T> {
public:
	inline Set(const string& sep = "\n") :
			Set_base<T>(sep) {
	}
	inline Set(const set<T>& s, const string& sep = "\n") :
			Set_base<T>(s, sep) {
	}
};

template<>
class Set<string> : public Set_base<string> {
public:
	inline Set(const string& sep = "\n") :
			Set_base<string>(sep) {
	}
	inline Set(const set<string>& s, const string& sep = "\n") :
			Set_base<string>(s, sep) {
	}
	Set(const string& source, const string& delim, const string& sep = "\n");
};

template<class T> inline Set<T> Intersection(const Set<T>& A, const Set<T>& B) {
	Set<T> result;
	std::set_intersection(A.begin(), A.end(), B.begin(), B.end(),
			std::inserter(result, result.begin()));
	return result;
}
template<class T> inline Set<T> Union(const Set<T>& A, const Set<T>& B) {
	Set<T> result;
	std::set_union(A.begin(), A.end(), B.begin(), B.end(),
			std::inserter(result, result.begin()));
	return result;
}
template<class T> inline Set<T> Difference(const Set<T>& A, const Set<T>& B) {
	Set<T> result;
	std::set_difference(A.begin(), A.end(), B.begin(), B.end(),
			std::inserter(result, result.begin()));
	return result;
}
template<class T> inline bool Subset(const Set<T>& A, const Set<T>& B) {
	return Difference(A, B).empty();
}

template<class T> inline ostream& operator <<(ostream& out,
		const Vector<T>& v) {
	return v.to_stream(out);
}
template<class T> inline ostream& operator <<(ostream& out, const List<T>& l) {
	return l.to_stream(out);
}
template<class T> inline ostream& operator <<(ostream& out, const Set<T>& s) {
	return s.to_stream(out);
}

#endif
