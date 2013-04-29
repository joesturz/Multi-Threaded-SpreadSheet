/*
 * testhdr.h
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 */

#ifndef TESTHDR_H
#define TESTHDR_H

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

namespace testutil {

  void my_assert(const string& label, int test, const string& failmsg) {

    if (test) {

      cout << "Test "
	   << label
	   << " passed."
	   << endl;

    } else {

      cout << "Test "
	   << label
	   << " failed. "
	   << failmsg
	   << endl;
  
    }
  }

  void assert_eq_int(const string& label, int expected, int actual) {

    if (expected == actual) {

      cout << "Test "
	   << label
	   << " passed."
	   << endl;

    } else {

      cout << "Test "
	   << label
	   << " failed."
	   << "Expected <"
	   << expected
	   << ">. Actual <"
	   << actual
	   << ">."
	   << endl;
    }

  }

}

#endif
