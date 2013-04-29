/*
 * msg.h
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * A msg represents a message received from a spreadsheet
 * client. It contains the lines received so far, and
 * encapsulates some of the logic for parsing.
 *
 * There are 6 derived classes:
 *
 *   -- create_msg
 *   -- join_msg
 *   -- change_msg
 *   -- undo_msg
 *   -- save_msg
 *   -- leave_msg
 *
 * corresponding to the different possible messages rcv'd
 * from the client.
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>
#include "exceptions.h"

using std::string;
using std::vector;

enum msg_type {

  create_msg_type,
  join_msg_type,
  change_msg_type,
  undo_msg_type,
  save_msg_type,
  leave_msg_type

};

class msg
{
 public:

  // constructor
  msg();

  // public methods
  virtual msg_type mtype() = 0;
  virtual void add_line(const string &line);
  virtual int  need_more_hdr();
  virtual int  need_more_body();

  virtual void parse_hdr() 
    throw(parsing_exception);

  virtual const string to_string();

  // public ivars
  int hdr_is_parsed;
  string ssname;

 protected:
  // protected ivar
  vector<string> header_lines;
  
};

class create_msg: public msg
{
 public:

  // methods
  virtual msg_type mtype() { return create_msg_type; }
  virtual int  need_more_hdr() { return this->header_lines.size() < 3; }
  virtual void parse_hdr() 
    throw(parsing_exception);

  // extra ivar
  string password;
};

class join_msg: public msg
{
 public:

  // methods
  virtual msg_type mtype() { return join_msg_type; }
  virtual int  need_more_hdr() { return this->header_lines.size() < 3; }
  virtual void parse_hdr() 
    throw(parsing_exception);

  // extra ivar
  string password;

};

class change_msg: public msg
{
 public:

  // methods
  virtual msg_type mtype() { return change_msg_type; }
  virtual void add_line(const string& line);
  virtual int  need_more_hdr() { return this->header_lines.size() < 5; }
  virtual int  need_more_body() { return this->content.length() < this->length; }
  virtual void parse_hdr() 
    throw(parsing_exception);

  virtual const string to_string();

  // extra ivars
  int version;
  string cell;
  int length;
  string content;

};

class undo_msg: public msg
{
 public:

  virtual msg_type mtype() { return undo_msg_type; }
  virtual int  need_more_hdr() { return this->header_lines.size() < 3; }
  virtual void parse_hdr() 
    throw(parsing_exception);

  // extra ivars
  int version;

};

class save_msg: public msg
{
 public:
  virtual msg_type mtype() { return save_msg_type; }
  virtual int  need_more_hdr() { return this->header_lines.size() < 2; }
};

class leave_msg: public msg
{
 public:
  virtual msg_type mtype() { return leave_msg_type; }
  virtual int  need_more_hdr() { return this->header_lines.size() < 2; }

};

#endif
