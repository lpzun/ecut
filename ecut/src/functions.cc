/*
 * functions.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: lpzun
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <utility>

#include "functions.hh"

namespace CONTROL {

bool getline(istream& in, string& line, const char& eol) {
	char c = 0;
	while (in.get(c) ? c != eol : false)
		line += c;
	return c != 0;
}

void remove_comments(istream& in, ostream& out, const string& comment) {
	string line;
	while (getline(in, line = "")) {
		const size_t comment_start = line.find(comment);
		out
				<< (comment_start == string::npos ?
						line : line.substr(0, comment_start)) << endl;
	}
}

void remove_comments(istream& in, const string& filename,
		const string& comment) {
	ofstream out(filename.c_str());
	remove_comments(in, out, comment);
}

void remove_comments(const string& in, string& out, const string& comment) {
	std::istringstream instream(in);
	std::ostringstream outstream;
	remove_comments(instream, outstream, comment);
	out = outstream.str();
}

bool wait_yes_no(const string& message, ostream& out) {
	char c;
	do {
		out << message << " Please enter 'y' or 'n': ";
		cin >> c;
		clear(cin);
		c = toupper(c);
	} while (c != 'Y' && c != 'N');
	return c == 'Y';
}

}

ulong Timing::stamp;

namespace COMPARE {
short compare(clong& x, clong& y) {
	if (x < y)
		return -1;
	if (x > y)
		return +1;
	return 0;
}

ulong string2num(const string& s) {
	ulong l = atol(s.c_str());
	if (PPRINT::widthify(l) != s) // we intepret this as error
		throw No_Number();
	return l;
}
}

namespace LOGARITHM {
std::pair<ushort, ushort> floor_ceil_log(const ulong& i, const ushort& base) {
	__SAFE_ASSERT__ ( i >= 1);__SAFE_ASSERT__ (base >= 2);
	ulong n = i;
	ushort floor = 0;
	bool is_exact_power = true;
	while (n >= base) {
		is_exact_power = is_exact_power && (n % base == 0);
		n /= base; // this is floor(n/base)
		++floor;
	}
	is_exact_power = is_exact_power && (n == 1);
	ushort ceil = (is_exact_power ? floor : floor + 1);
	return std::pair<ushort, ushort>(floor, ceil);
}

ulong n_ary2decimal(const vector<ushort>& v, cushort& base) {
	ulong m = 1;
	ulong result = 0;
	for (vector<ushort>::const_reverse_iterator digit = v.rbegin();
			digit != v.rend(); ++digit) {
		__SAFE_ASSERT__ (*digit < base);
		result += (*digit) * m;
		m *= base;
	}
	return result;
}

vector<ushort> decimal2n_ary(culong& l, cushort& base, cushort& size) {
	cushort min_size = std::max(ceil_log(l + 1, base), ushort(1));
	__SAFE_ASSERT__ (size == 0 || size >= std::max(ceil_log(l + 1, 2), ushort(1)));
	cushort vector_size = (size == 0 ? min_size : size);
	vector<ushort> result;
	result.reserve(vector_size);
	ulong r = l;
	for (ushort b = 0; b < vector_size; ++b) { // just a counter: repeat vector_size times
		ushort digit = r % base;
		result.insert(result.begin(), digit);
		r /= base;
	} // = (r-digit)/base
	__SAFE_ASSERT__ (r == 0);
	return result;
}

bool inc(vector<bool>& bv) {
	for (int b = bv.size() - 1; b >= 0; --b)
		if ((bv[b] = !bv[b]))
			return true;
	return false;
}
}

namespace PPRINT {
string tabularize(const string& s, const string& sep,
		const unsigned short& tab) {
	string result;
	ushort n = s.size();
	for (int i = 0; i < n; ++i) {
		if (i > 0 && (n - i) % tab == 0)
			result += (s[i - 1] == ' ' || s[i] == ' ' ? " " : sep);
		result += s[i];
	}
	return result;
}

string hourize(culong& seconds) {
	if (seconds < 60)
		return widthify(seconds) + "s";
	else if (seconds < 3600) { // 60 <= seconds < 3600
		ushort m = seconds / 60;
		ushort s = seconds % 60;
		return widthify(m) + ":" + widthify(s, 2, RIGHTJUST, '0') + "m";
	} else {                     // 3600 <= seconds < infty
		ushort h = seconds / 3600;
		ushort s = seconds % 3600;
		ushort m = s / 60;
		s = s % 60;
		return widthify(h) + ":" + widthify(m, 2, RIGHTJUST, '0') + ":"
				+ widthify(s, 2, RIGHTJUST, '0') + "h";
	}
}

string firstTimeOrNot(bool& firstTime, const string& connective,
		const string& common) {
	string result = (firstTime ? common : connective + common);
	firstTime = false;
	return result;
}
}

Vector<string>::Vector(const string& source, const string& delim,
		const string& sep) :
		Vector_base<string>(sep) {
	string s = source;
	while (true) {
		size_t start = s.find_first_not_of(delim);
		if (start == string::npos)
			break;
		else {
			size_t end = s.find_first_of(delim, start);
			if (end == string::npos) {
				push_back(s.substr(start));
				break;
			} else {
				push_back(s.substr(start, end - start));
				s = s.substr(end);
			}
		}
	}
}

List<string>::List(const string& source, const string& delim, const string& sep) :
		List_base<string>(sep) {
	string s = source;
	while (true) {
		size_t start = s.find_first_not_of(delim);
		if (start == string::npos)
			break;
		else {
			size_t end = s.find_first_of(delim, start);
			if (end == string::npos) {
				push_back(s.substr(start));
				break;
			} else {
				push_back(s.substr(start, end - start));
				s = s.substr(end);
			}
		}
	}
}

List<string>::const_iterator List<string>::find_from_prefix(
		const string& prefix) const {
	List<string>::const_iterator end = this->end(), result = end;
	for (List<string>::const_iterator word = begin(); word != end; ++word)
		if (word->find(prefix) == 0) { // prefix is found at position 0 in *word
			if (result != end) // already have a match
				throw CONTROL::Error(
						"find_from_prefix: '" + prefix + "' matches both '"
								+ (*result) + "' and '" + (*word) + "'");
			else
				result = word;
		}
	return result;
}

Set<string>::Set(const string& source, const string& delim, const string& sep) :
		Set_base<string>(sep) {
	string s = source;
	while (true) {
		size_t start = s.find_first_not_of(delim);
		if (start == string::npos)
			break;
		else {
			size_t end = s.find_first_of(delim, start);
			if (end == string::npos) {
				insert(s.substr(start));
				break;
			} else {
				insert(s.substr(start, end - start));
				s = s.substr(end);
			}
		}
	}
}

long Random::integer(clong& min, clong& max) {
	__SAFE_ASSERT__ (max >= min);
	ulong size = max - min;
	long r = long(rint((float(rand()) / RAND_MAX) * size));
	return r + min;
}

float Random::real(const float& min, const float& max) {
	__SAFE_ASSERT__ (max >= min);
	float size = std::max(max - min, float(0));
	float r = (float(rand()) / RAND_MAX) * size;
	return r + min;
}
