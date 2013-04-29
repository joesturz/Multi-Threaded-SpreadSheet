/*
 * ss_db.h
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Simple spreadsheet info database front end. All methods
 * are static.
 */

#ifndef SS_DB_H
#define SS_DB_H

#include <string>
#include "exceptions.h"

class ss_db {

 public:

  //
  // init_db
  // -------
  //
  // Initializes database for spreadsheet file
  // and password information.
  //
  // Returns 0 on fail, non-zero on success;
  //
  // This static method should be called when
  // the server is first initialized.
  //
  static int init_db();

  //
  // insert_db
  // ---------
  //
  // Attempts to insert spreadsheet name and
  // password into db. Returns ss id.
  //
  // Throws spreadsheet_exists exception if
  // a ss of that name is already in the db.
  //
  static int insert_db(const std::string& name, const std::string& password) 
    throw(spreadsheet_exists);

  //
  // query_db
  // --------
  //
  // Attempts to find ss name in db. Checks that
  // password matches.
  //
  // If it finds ss name and the password matches,
  // returns the id for the ss, as an int. The ss
  // xml file should be stored as "<id>.xml".
  //
  // If the ss name is not in the db, throws a
  // spreadsheet_not_exists exception.
  //
  // If the password does not match, throws a
  // spreadsheet_wrong_pword exception.
  //
  static int query_db(const std::string& name, const std::string& password)
    throw(spreadsheet_not_exists, spreadsheet_wrong_pword);

};

#endif
