/*
 * ss_tests.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Tests for spreadsheet class.
 */

#include "../ss_db.h"
#include "../spreadsheet.h"
#include "../exceptions.h"

void test1() {

  spreadsheet::create_empty_ss("test.ss", "sesame");

}

void test2() {

  spreadsheet::create_empty_ss("test.ss", "sesame");
  spreadsheet s("test.ss", "sesame");

}

int main() {
  
  // init database
  ss_db::init_db();

  //test1();

  test2();

  return 0;
}
