/*
 * exceptions.h
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Exceptions used in application.
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
 
#include <exception>

class parsing_exception: public std::exception
{
 public:
  const char *message;
  virtual const char* what() const throw()
  {
    return this->message;
  }
};

class spreadsheet_exists: public std::exception { };

class spreadsheet_not_exists: public std::exception { };

class spreadsheet_wrong_pword: public std::exception { };

class spreadsheet_rw_exception: public std::exception
{
 public:
  const char *message;
  virtual const char* what() const throw()
  {
    return this->message;
  }
};


#endif
