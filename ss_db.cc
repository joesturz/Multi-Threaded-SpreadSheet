/*
 * ss_db.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * ss_db implementation.
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include "ss_db.h"

int ss_db::init_db() {

  // create db if it does not exist
  std::fstream fs;

  try {

    // open
    fs.open("ss_db.txt", std::fstream::app);

    // success; close file
    fs.close();

    // return 1
    return 1;    

  } catch(std::exception& e) {

    // failed

    // close file
    fs.close();

    // return 0
    return 0;

  }

}

int ss_db::insert_db(const std::string& name, const std::string& password) 
  throw(spreadsheet_exists) {

  int id = 0;
  std::fstream fs;
  std::string line;


  spreadsheet_exists exc;
  
  // open
  fs.open("ss_db.txt", std::fstream::in);

  // read existing ss records, confirming they do not have
  // the same name

  while (!std::getline(fs, line).eof()) {

    // line contains record id; parse into int
    std::stringstream ss;
    ss.str(line);
    ss >> id;

    // read in ss name
    std::getline(fs, line);

    // compare
    if (line.compare(name) == 0) {

      // ss already in db! 

      // close file
      fs.close();

      //throw exc
      throw exc;

    }

    // otherwise okay; skip over pword
    std::getline(fs, line);

  }

  // close fs
  fs.close();

  fs.open("ss_db.txt", std::fstream::out | std::fstream::app);

  // exited loop; use id++ for new ss record
  id++;

  // append:
  // 
  // id
  // ss name
  // password

  fs << id << std::endl;
  fs << name << std::endl;
  fs << password << std::endl;
    
  // success; close file
  fs.close();

  return id;

}

int ss_db::query_db(const std::string& name, const std::string& password)
  throw(spreadsheet_not_exists, spreadsheet_wrong_pword) {

  int id;
  std::ifstream fs;
  std::string line;

  spreadsheet_not_exists exc1;
  spreadsheet_wrong_pword exc2;  
  
  // open
  fs.open("ss_db.txt", std::ofstream::in);

  // read existing ss records

  while (!std::getline(fs, line).eof()) {

    // line contains record id; parse into int
    std::stringstream ss;
    ss.str(line);
    ss >> id;

    // read in ss name; check if it matches
    std::getline(fs, line);

    // compare
    if (line.compare(name) == 0) {

      // read pword
      std::getline(fs, line);

      if (line.compare(password) == 0) {

	// success!

	// close file
	fs.close();

	//return id
	return id;

      } else {

	// wrong pword

	// close file
	fs.close();

	//throw exc2
	throw exc2;

      }
    }

    // otherwise skip over pword
    std::getline(fs, line);

  }

  // exited loop without finding ss

  // throw exc1
  throw exc1;
}


