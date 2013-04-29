/*
 * parser.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Parsing procedure implementations.
 */

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "parser.h"

msg* parse_req_line(const string& line) throw(parsing_exception) {

  string line_cpy;
  parsing_exception ex;
  msg *ret;

  // confirm line is non-empty
  if (line.length() == 0) {

    // fail
    ex.message = "Line is empty!";
    throw ex;

  }

  // convert line to lower case, for uniformity
  line_cpy = boost::to_lower_copy(line);
  
  // determine request type
  if (line_cpy.compare("create") == 0) {

    ret = new create_msg;

  } else if (line_cpy.compare("join") == 0) {

    ret = new join_msg;

  } else if (line_cpy.compare("change") == 0) {

    ret = new change_msg;

  } else if (line_cpy.compare("undo") == 0) {

    ret = new undo_msg;

  } else if (line_cpy.compare("save") == 0) {

    ret = new save_msg;

  } else if (line_cpy.compare("leave") == 0) {

    ret = new leave_msg;

  } else {

    // failed to match
    ex.message = "Unrecognized initial line in message!";
    throw ex;

  }

  return ret;
}


string parse_name(const string& line) throw(parsing_exception) {

  parsing_exception ex;

  // confirm line is non-empty
  if (line.length() == 0) {

    // fail
    ex.message = "Empty line when parsing Name: header field.";
    throw ex;

  }

  // extract name using regex

  // pattern
  const char* pattern = "Name:(.*)";

  // regex obj
  boost::regex name_regex(pattern);

  // extract
  boost::match_results<std::string::const_iterator> m;
  if (boost::regex_search(line.begin(), line.end(), m, name_regex)) {

    return m[1];

  } else {

    // failed to match
    ex.message = "Could not read line as a Name: header field.";
    throw ex;

  }
}

string parse_password(const string& line) throw(parsing_exception) {

  parsing_exception ex;

  // confirm line is non-empty
  if (line.length() == 0) {

    // fail
    ex.message = "Empty line when parsing Password: header field.";
    throw ex;

  }

  // extract name using regex

  // pattern
  const char* pattern = "Password:(.*)";

  // regex obj
  boost::regex password_regex(pattern);

  // extract
  boost::match_results<std::string::const_iterator> m;
  if (boost::regex_search(line.begin(), line.end(), m, password_regex)) {

    return m[1];

  } else {

    // failed to match
    ex.message = "Could not read line as a Password: header field.";
    throw ex;

  }
}

int parse_version(const string& line) throw(parsing_exception) {

  parsing_exception ex;

  // confirm line is non-empty
  if (line.length() == 0) {

    // fail
    ex.message = "Empty line when parsing Version: header field.";
    throw ex;

  }

  // extract name using regex

  // pattern
  const char* pattern = "Version:(.*)";

  // regex obj
  boost::regex version_regex(pattern);

  // extract
  boost::match_results<std::string::const_iterator> m;
  if (boost::regex_search(line.begin(), line.end(), m, version_regex)) {

    // attempt to parse as integer
    try {

      return boost::lexical_cast<int>(m[1]);

    } catch(boost::bad_lexical_cast const&) {

      ex.message = "Could not cast version as an integer.";
      throw ex;

    }

  } else {

    // failed to match
    ex.message = "Could not read line as a Password: header field.";
    throw ex;

  }
}

string parse_cell(const string& line)  throw(parsing_exception) {

  parsing_exception ex;

  // confirm line is non-empty
  if (line.length() == 0) {

    // fail
    ex.message = "Empty line when parsing Cell: header field.";
    throw ex;

  }

  // extract name using regex

  // pattern
  const char* pattern = "Cell:(.*)";

  // regex obj
  boost::regex cell_regex(pattern);

  // extract
  boost::match_results<std::string::const_iterator> m;
  if (boost::regex_search(line.begin(), line.end(), m, cell_regex)) {

    return m[1];

  } else {

    // failed to match
    ex.message = "Could not read line as a Cell: header field.";
    throw ex;

  }

}

int parse_length(const string& line) throw(parsing_exception) {

  parsing_exception ex;

  // confirm line is non-empty
  if (line.length() == 0) {

    // fail
    ex.message = "Empty line when parsing Length: header field.";
    throw ex;

  }

  // extract name using regex

  // pattern
  const char* pattern = "Length:(.*)";

  // regex obj
  boost::regex length_regex(pattern);

  // extract
  boost::match_results<std::string::const_iterator> m;
  if (boost::regex_search(line.begin(), line.end(), m, length_regex)) {

    // attempt to parse as integer
    try {

      return boost::lexical_cast<int>(m[1]);

    } catch(boost::bad_lexical_cast const&) {

      ex.message = "Could not cast length as an integer.";
      throw ex;

    }

  } else {

    // failed to match
    ex.message = "Could not read line as a Length: header field.";
    throw ex;

  }
}
