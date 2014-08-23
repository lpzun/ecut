/*
 * input.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: lpzun
 */

#include <string>
#include <list>
#include <algorithm>
#include <iostream>

#include "functions.hh"

#include "input.hh"

const string Argument::prefix = "-";
const string Argument::value_separator = ",";

void Argument::set_value(const string& value) {
	this->value = (range.empty() ? value : value_from_prefix(value));
}

string Argument::value_from_prefix(const string& prefix) const {
	List<string>::const_iterator value = range.find_from_prefix(prefix);
	if (value == range.end())
		throw CONTROL::Error(
				"Input::Argument::value_from_prefix: prefix \"" + prefix
						+ "\" not extendible to value in range of argument \""
						+ name + "\"");
	return *value;
}

bool Argument::starts_with_prefix(const string& name) {
	return (name.size() < prefix.size() ?
			false : name.substr(0, prefix.size()) == prefix);
}

const string Input::help_switch_short = Argument::prefix + "h";
const string Input::help_switch_long = Argument::prefix + "H";
const string Input::help_message = "Use " + Input::help_switch_short
		+ " for basic help, " + Input::help_switch_long + " for more";

void Input::add_argument(const string& name, const string& meaning,
		const string& range_s, const string& default_value) {
	if (find(name) != arguments.end())
		throw CONTROL::Error(
				"Input::add_argument: argument " + name + " already exists");
	const List<string> range(range_s, Argument::value_separator);
	arguments.push_back(Argument(name, meaning, range, default_value));
	name_width = std::max(name_width, ushort(name.size()));
}

void Input::add_switch(const string& name, const string& meaning) {
	if (find(name) != arguments.end())
		throw CONTROL::Error(
				"Input::add_switch: argument " + name + " already exists");
	arguments.push_back(Argument(name, meaning));
	name_width = std::max(name_width, ushort(name.size()));
}

Input::Iterator Input::next_keyless_arg() {
	static Iterator arg = arguments.begin();
	while (arg != arguments.end() && !arg->is_keyless())
		++arg;
	return arg++;
}

void Input::get_command_line(const int argc, const char* const * const argv) {
	list<string> args;
	for (int i = 1; i < argc; args.push_back(argv[i]), ++i)
		;
	get_command_line(argv[0], args);
}

void Input::get_command_line(const string& prog_name, const string& args) {
	List<string> arg_list(args, " \t");
	get_command_line(prog_name, arg_list);
}

void Input::get_command_line(const string& prog_name,
		const List<string>& arg_strs) {
	for (list<string>::const_iterator arg_strp = arg_strs.begin();
			arg_strp != arg_strs.end(); ++arg_strp) {
		const string& arg_str = *arg_strp;
		__SAFE_ASSERT__ (! arg_str.empty()); // ??
		if (arg_str == help_switch_short) {
			print_usage_short(prog_name);
			throw Help();
		} else if (arg_str == help_switch_long) {
			print_usage_long(prog_name);
			throw Help();
		} else if (Argument::starts_with_prefix(arg_str)) {
			Iterator arg = find(arg_str);
			if (arg == arguments.end())
				throw CONTROL::Error(
						"Input::get_command_line: " + arg_str
								+ ": no such keyword argument. "
								+ help_message);
			if (arg->is_switch())
				arg->value = "true";
			else {
				++arg_strp;
				if (arg_strp == arg_strs.end())
					throw CONTROL::Error(
							"Input::get_command_line: no value specified for argument "
									+ arg_str + ". " + help_message);
				arg->set_value(*arg_strp);
			}
		} else {   // keyless argument
			Iterator next_keyless_arg = this->next_keyless_arg();
			if (next_keyless_arg == arguments.end())
				throw CONTROL::Error(
						"Input::get_command_line: no argument left to assign value of "
								+ arg_str + " to. " + help_message);
			next_keyless_arg->set_value(arg_str);
		}
	}
	test_all_args_set();
}

void Input::test_all_args_set() const {
	for (Const_iterator arg = arguments.begin(); arg != arguments.end(); ++arg)
		if (arg->value.empty())
			throw CONTROL::Error(
					"Input::get_command_line: no value specified for argument "
							+ arg->name + ". " + help_message);
}

void Input::print_usage_short(const string& prog_name, cushort& indent,
		ostream& out) const {
	const string i = string(indent, ' ');
	out << i << "Usage:" << " " << prog_name;
	for (Const_iterator arg = arguments.begin(); arg != arguments.end(); ++arg)
		out << " " << arg->name << (arg->is_keyword() ? " <val>" : "");
	out << endl;

	for (Const_iterator arg = arguments.begin(); arg != arguments.end();
			++arg) {
		out << i << "  "
				<< PPRINT::widthify(arg->name, name_width, PPRINT::RIGHTJUST)
				<< " = "
				<< (arg->meaning.empty() ? "(no meaning)" : arg->meaning);
		if (arg->is_switch())
			out << " (a switch)";
		else {
			out << ", with ";
			if (arg->range.empty())
				out << "no specified range";
			else {
				out << "range {";
				for (list<string>::const_iterator x = arg->range.begin();
						x != arg->range.end(); ++x)
					out << " " << (*x);
				out << " }";
			}
			out << " and "
					<< (arg->value.empty() ?
							"no default value" :
							"default value \"" + arg->value + "\"");
		}
		out << endl;
	}
}

void Input::print_usage_long(const string& prog_name, cushort& indent,
		ostream& out) const {
	print_usage_short(prog_name, indent, out);
	const string i = string(indent, ' ');
	out << endl << i << "General information:" << endl << i
			<< "  Types of command line arguments shown in the Usage string:"
			<< endl << i << "  (*) Switches, whose names start with "
			<< Argument::prefix << "." << endl << i
			<< "      These are of type boolean and have default value false."
			<< endl << i
			<< "      Set them to true simply by including the switch on the command line."
			<< endl << i << "  (*) Keyword arguments, whose names start with "
			<< Argument::prefix << " and which are followed by <val>." << endl
			<< i << "      Set them by giving a value following the name."
			<< endl << i
			<< "  (*) Keyless arguments, whose names do not start with "
			<< Argument::prefix << "." << endl << i
			<< "      Set them by giving their value in place of the name."
			<< endl << i
			<< "  Switches and keyword arguments can be set in any order."
			<< endl << i
			<< "  Keyless arguments must be set in the order they appear in the command line."
			<< endl << i
			<< "  If default value was provided at declaration and no value is given at the command line, the default value is used;"
			<< endl << i
			<< "  if no default value was provided at declaration, the argument is mandatory."
			<< endl << i
			<< "  Values of arguments may be abbreviated if unambiguous and if range was provided."
			<< endl << i << "  Argument names may not be abbreviated." << endl;
}

void Input::print_command_line(cushort& indent, ostream& out) const {
	out << string(indent, ' ') << "Values assigned to arguments:" << endl;
	for (Const_iterator arg = arguments.begin(); arg != arguments.end(); ++arg)
		out << string(indent + 2, ' ')
				<< PPRINT::widthify(arg->name, name_width, PPRINT::LEFTJUST)
				<< " with value \"" << arg->value << "\"" << endl;
}

string Input::arg_value(const string& name) const {
	Const_iterator arg = find(name);
	if (find(name) == arguments.end())
		throw CONTROL::Error(
				"Input::arg_value: argument " + name + " does not exist");
	return arg->value;
}

void Input::reset_switches() {
	for (Iterator arg = arguments.begin(); arg != arguments.end(); ++arg)
		if (arg->is_switch())
			arg->value = "false";
}

