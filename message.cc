/*
 * message.cc
 *
 * CHIJ
 * CS 3505, Spring 2013
 *
 * Implementation of message classes.
 */

#include <iostream>
#include <sstream>
#include "message.h"
#include "parser.h"


/*** msg implementation ***/

msg::msg() {

  this->hdr_is_parsed = 0;

}

void msg::add_line(const string& line) {

  this->header_lines.push_back(line);

}

void msg::parse_hdr() throw(parsing_exception) {

  parsing_exception ex;

  if (this->need_more_hdr()) {

    ex.message = "Called parsing header, but need more lines!";
    throw ex;

  }

  if (this->mtype() != leave_msg_type) {

    // parse spreadsheet name (may throw parsing exc)
    this->ssname = parse_name(this->header_lines[1]);

  }

  this->hdr_is_parsed = 1;

}

int msg::need_more_hdr() { return 0; }

int msg::need_more_body() { return 0; }

const string msg::to_string() {

  int i;
  std::stringstream ss;

  for (i = 0; i < this->header_lines.size(); i++) {

    ss << this->header_lines[i] << "\n";

  }

  return ss.str();

}

/** create_msg implementation **/
void create_msg::parse_hdr() throw(parsing_exception) {

  // call base method
  this->msg::parse_hdr();

  // parse password (may throw exc)
  this->password = parse_password(this->header_lines[2]);

}

/** join_msg implementation **/
void join_msg::parse_hdr() throw(parsing_exception) {

  // call base method
  this->msg::parse_hdr();

  // parse password (may throw exc)
  this->password = parse_password(this->header_lines[2]);

}

/** change_msg implementation **/
void change_msg::add_line(const string& line) {

  if (this->need_more_hdr()) {

    // call base
    this->msg::add_line(line);

  } else {

    // header full; reading in content
    this->content.append(line);

  }
}

void change_msg::parse_hdr() throw(parsing_exception) {

  // call base
  this->msg::parse_hdr();

  // parse version
  this->version = parse_version(this->header_lines[2]);

  // parse cell
  this->cell = parse_cell(this->header_lines[3]);

  // parse length
  this->length = parse_length(this->header_lines[4]);

  // content comes next...

}

const string change_msg::to_string() {

  // call base, and add content
  return this->msg::to_string() + this->content + "\n";

}

/** undo_msg implementation **/
void undo_msg::parse_hdr() throw(parsing_exception) {

  // call base
  this->msg::parse_hdr();

  // parse version
  this->version = parse_version(this->header_lines[2]);

}


