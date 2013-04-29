/*
 * parser.h
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Small lib for parsing lines received from spreadsheet clients,
 * in some cases using regular expressions.
 *
 * Contains def for parsing_exception.
 */

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "exceptions.h"
#include "message.h"

// Matches line for create, join, change, etc. If
// line matches, returns a new msg of the right type.
//
// Throws a parsing exception if line cannot be
// matched.
msg* parse_req_line(const string& line) 
  throw(parsing_exception);

// Parses line as "Name:<name>" header line.
//
// Returns <name> as a string.
//
// Throws a parsing exception if line cannot be
// parsed as such.
string parse_name(const string& line)
  throw(parsing_exception);

// Parses line as "Password:<password>" header line.
//
// Returns <password> as a string.
//
// Throws a parsing exception if line cannot be
// parsed as such.
string parse_password(const string& line)
  throw(parsing_exception);

// Parses line as "Version:<version>" header line.
//
// Returns <version> as an int.
//
// Throws a parsing exception if line cannot be
// parsed as such.
int parse_version(const string& line)
  throw(parsing_exception);

// Parses line as "Cell:<cell>" header line.
//
// Returns <cell> as a string.
//
// Throws a parsing exception if line cannot be
// parsed as such.
string parse_cell(const string& line)
  throw(parsing_exception);

// Parses line as "Length:<length>" header line.
//
// Returns <length> as an int.
//
// Throws a parsing exception if line cannot be
// parsed as such.
int parse_length(const string& line)
  throw(parsing_exception);

#endif
