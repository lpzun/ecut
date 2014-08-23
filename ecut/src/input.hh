/*
 * input.h
 *
 *  Created on: Oct 17, 2012
 *      Author: lpzun
 */

#ifndef __INPUT_HH__
#define __INPUT_HH__

#include <stdlib.h>
#include <string>
#include <list>

#include "functions.hh"

// A general class to organize command line input. Arguments with parameters are supported.
// As much of input validity testing as possible is done.

class Argument {
public:
	const string name;
	const string meaning;
	const List<string> range;

	string value;

	static const string prefix; // keyword args and boolean switches have a prefix
	static const string value_separator; // the string separating values in range specifications, e.g. ','

	static bool starts_with_prefix(const string& name);

	void set_value(const string& value); // if range empty, return value, otherwise return value found using function below
	string value_from_prefix(const string& prefix) const; // if <prefix> is prefix of some unique value in <range>, return value

	inline Argument(const string& name, const string& meaning,
			const List<string>& range, const string& default_value) :
			name(name), meaning(meaning), range(range) {
		set_value(default_value);
	}
	inline Argument(const string& name, const string& meaning) :
			name(name), meaning(meaning), range(
					List<string>("false" + value_separator + "true",
							value_separator)), value("false") {
	}

	inline bool is_switch() const {
		return range.size() == 2 && range.front() == "false"
				&& range.back() == "true";
	}
	inline bool is_keyword() const {
		return (!is_switch()) && starts_with_prefix(name);
	}
	inline bool is_keyless() const {
		return !(is_switch() || is_keyword());
	}

	inline operator bool() const {
		__SAFE_ASSERT__ (is_switch());
		return value == "true";
	}
};

class Argument_by_Name {
private:
	const string& name;
public:
	inline Argument_by_Name(const string& name) :
			name(name) {
	}
	inline bool operator()(const Argument& arg) const {
		return arg.name == name;
	}
};

class Input {
private:
	static const string help_switch_short; // string indicating that the short usage message should be printed, e.g. "-h"
	static const string help_switch_long; // string indicating that the long  usage message should be printed, e.g. "-H"
	static const string help_message;

	typedef list<Argument>::const_iterator Const_iterator;
	typedef list<Argument>::iterator Iterator;

	list<Argument> arguments;

	inline Const_iterator find(const string& name) const {
		return find_if(arguments.begin(), arguments.end(),
				Argument_by_Name(name));
	}
	inline Iterator find(const string& name) {
		return find_if(arguments.begin(), arguments.end(),
				Argument_by_Name(name));
	}

	Iterator next_keyless_arg();

	ushort name_width;

public:
	// Range "" will be treated as "no range", default_value "" as "no default"
	void add_argument(const string& name, const string& meaning = "",
			const string& range_s = "", const string& default_value = "");
	void add_switch(const string& name, const string& meaning = "");

	inline Input() :
			name_width(0) {
		add_switch(help_switch_short,
				"print short help message and return to caller, ignoring rest of line");
		add_switch(help_switch_long,
				"print long  help message and return to caller, ignoring rest of line");
	}

	struct Help {
	};

	void print_usage_short(const string& prog_name, cushort& indent = 0,
			ostream& out = cout) const;
	void print_usage_long(const string& prog_name, cushort& indent = 0,
			ostream& out = cout) const;

	void get_command_line(const int argc, const char* const * const argv); // args supplied as by main
	void get_command_line(const string& prog_name, const string& args); // args supplied as a single input string
	void get_command_line(const string& prog_name, const List<string>& args); // args supplied as a list of strings

	void test_all_args_set() const;

	void reset_switches(); // clear all switches to their default "false". Needed since switches cannot be set to false

	void print_command_line(cushort& indent = 0, ostream& out = cout) const;

	string arg_value(const string& name) const; // for any arg

	inline bool arg2bool(const string& name) const {
		const Argument& arg = *find(name);
		__SAFE_ASSERT__ (arg.is_switch());
		return bool(arg);
	} // for switches
	inline long arg2long(const string& name) const {
		return atol(arg_value(name).c_str());
	}
};

#endif
