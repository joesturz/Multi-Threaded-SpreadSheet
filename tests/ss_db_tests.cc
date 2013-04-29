/*
 * ss_db_tests.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * ss_db tests.
 */

#include <iostream>
#include "../ss_db.h"
#include "../exceptions.h"

int main() {

  // init db
  ss_db::init_db();

  // attempt to insert non-existing ss
  ss_db::insert_db("test.ss", "sesame");
  std::cout << "inserted test.ss" << std::endl;

  // try to insert existing ss
  try {

    ss_db::insert_db("test.ss", "sesame");
    std::cout << "FAILED to detect non-existing ss." << std::endl;

  } catch(spreadsheet_exists& e) {

    std::cout << "correctly detected dup ss." << std::endl;
  }

  // add a few ss
  ss_db::insert_db("test1.ss", "sesame2");
  ss_db::insert_db("rm */evil.ss", "open sesame");

  // query for existing ss, right pword
  int id;
  id = ss_db::query_db("rm */evil.ss", "open sesame");
  std::cout << "db hit, id = " << id << std::endl;

  // query for existing ss, wrong pword
  try {

    ss_db::query_db("test1.ss", "wrongpw");
    std::cout << "failed to detect wrong pword" << std::endl;

  } catch(spreadsheet_wrong_pword& e) {

    std::cout << "correctly detected wrong pword" << std::endl;

  }

  // query for non-existing ss
  try {

    ss_db::query_db("hello", "");
    std::cout << "failed to detect non-existing ss" << std::endl;

  } catch(spreadsheet_not_exists& e) {

    std::cout << "correctly detected non-existing ss" << std::endl;

  }

  return 0;
}
