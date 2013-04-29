/*
 * CHIJ
 * CS3505, Spring 2013
 *
 * Tests for parser.h and message.h procedures
 * and classes.
 */

#include "testhdr.h"
#include "../parser.h"

using namespace testutil;

void test1() {

  // create empty change msg
  change_msg m;
  
  my_assert("1", 1, "");

}

void test2() {

  // parse, line empty
  try {

    parse_req_line("");

    // failed
    my_assert("2", 0, "Failed to throw exception.");

  } catch (parsing_exception& e) {

    my_assert("2", 1, "");

  }
}

void test3() {

  // parse create
  msg *m = NULL;
  m = parse_req_line("CREATE");
  my_assert("3a", m != NULL, "m is null.");
  my_assert("3b", m->mtype() == create_msg_type, "Wrong message type.");
  
  if (m) delete m;

}

void test4() {

  // parse join
  msg *m = NULL;
  m = parse_req_line("jOiN");
  my_assert("4a", m != NULL, "m is null.");
  my_assert("4b", m->mtype() == join_msg_type, "Wrong message type.");

  if (m) delete m;

}

void test5() {

  // parse name with empty line
  try {

    parse_name("");
    
    my_assert("5", 0, "Failed to throw exc.");

  } catch(parsing_exception& ex) {

    my_assert("5", 1, "");

  }
}

void test6() {

  // parse name
  string n;
  n = parse_name("Name:test.ss");
  my_assert("6", n.compare("test.ss") == 0, "Wrong name.");

}

void test7() {

  // parse name with wrong line
  try {

    parse_name("hello");

    my_assert("7", 0, "Failed to throw exc.");

  } catch (parsing_exception& e) {

    my_assert("7", 1, "");

  }
}

void test8() {

  // parse password with empty line
  try {

    parse_password("");
    
    my_assert("8", 0, "Failed to throw exc.");

  } catch(parsing_exception& ex) {

    my_assert("8", 1, "");

  }
}

void test9() {

  // parse password
  string n;
  n = parse_password("Password:sesame");
  my_assert("9", n.compare("sesame") == 0, "Wrong password.");

}

void test10() {

  // parse password with wrong line
  try {

    parse_password("hello");

    my_assert("10", 0, "Failed to throw exc.");

  } catch (parsing_exception& e) {

    my_assert("10", 1, "");

  }
}

void test11() {

  // parse version with empty line
  try {

    parse_version("");
    
    my_assert("11", 0, "Failed to throw exc.");

  } catch(parsing_exception& ex) {

    my_assert("11", 1, "");

  }
}

void test12() {

  // parse password
  int n;
  n = parse_version("Version:25");
  my_assert("12", n == 25, "Wrong version.");

}

void test13() {

  // parse version with wrong line
  try {

    parse_password("hello");

    my_assert("13", 0, "Failed to throw exc.");

  } catch (parsing_exception& e) {

    my_assert("13", 1, "");

  }
}

void test14() {

  // parse length with empty line
  try {

    parse_length("");
    
    my_assert("14", 0, "Failed to throw exc.");

  } catch(parsing_exception& ex) {

    my_assert("14", 1, "");

  }
}

void test15() {

  // parse length
  int n;
  n = parse_length("Length:980983");
  my_assert("15", n == 980983, "Wrong length.");

}

void test16() {

  // parse version with wrong line
  try {

    parse_length("hello");

    my_assert("16", 0, "Failed to throw exc.");

  } catch (parsing_exception& e) {

    my_assert("16", 1, "");

  }
}

void test17() {

  // parse cell with empty line
  try {

    parse_cell("");
    
    my_assert("17", 0, "Failed to throw exc.");

  } catch(parsing_exception& ex) {

    my_assert("17", 1, "");

  }
}

void test18() {

  // parse cell
  string n;
  n = parse_cell("Cell:A1");
  my_assert("18", n.compare("A1") == 0, "Wrong cell name.");

}

void test19() {

  // parse version with wrong line
  try {

    parse_cell("hello");

    my_assert("19", 0, "Failed to throw exc.");

  } catch (parsing_exception& e) {

    my_assert("19", 1, "");

  }
}

void test20() {

  // CREATE test

  // add header line
  msg *m;
  m = parse_req_line("CREATE");
  m->add_line("CREATE");

  // cast
  create_msg *cm;
  cm = dynamic_cast<create_msg *>(m);

  // chk
  my_assert("20a", cm->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  cm->add_line("Name:test.ss");

  // chk
  my_assert("20b", cm->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  cm->add_line("Password:sesame");

  // chk
  my_assert("20c", !cm->need_more_hdr(), "Should not have needed more header lines.");

  // attempt to parse
  cm->parse_hdr();

  // chk vals
  my_assert("20d", cm->ssname.compare("test.ss") == 0, "Wrong ss name.");
  my_assert("20e", cm->password.compare("sesame") == 0, "Wrong password.");
}

void test21() {

  // CHANGE test

  // add header line
  msg *m;
  m = parse_req_line("CHANGE");
  m->add_line("CHANGE");

  // cast
  change_msg *cm;
  cm = dynamic_cast<change_msg *>(m);

  // chk
  my_assert("21a", cm->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  cm->add_line("Name:test.ss");

  // chk
  my_assert("21b", cm->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  cm->add_line("Version:21");

  // chk
  my_assert("21c", cm->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  cm->add_line("Cell:A1");

  // chk
  my_assert("21d", cm->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  cm->add_line("Length:10");

  // chk
  my_assert("21e", !cm->need_more_hdr(), "Should not have needed more header lines.");

  // attempt to parse
  cm->parse_hdr();

  my_assert("21f", cm->hdr_is_parsed, "Header should be parsed.");
  my_assert("21g", cm->need_more_body(), "Should have needed more body.");

  // add body
  cm->add_line("0123456789");

  // chk
  my_assert("21h", !cm->need_more_body(), "Should not have needed more body.");

  // chk vals
  my_assert("21i", cm->ssname.compare("test.ss") == 0, "Wrong ss name.");
  my_assert("21j", cm->version == 21, "Wrong vnum.");
  my_assert("21k", cm->cell.compare("A1") == 0, "Wrong cell name.");
  my_assert("21l", cm->length == 10, "Wrong length.");
  my_assert("21m", cm->content.compare("0123456789") == 0, "Wrong content.");

}

void test22() {

  // UNDO test

  // add header line
  msg *m;
  m = parse_req_line("UNDO");
  m->add_line("UNDO");

  // cast
  undo_msg *um;
  um = dynamic_cast<undo_msg *>(m);

  // chk
  my_assert("22a", um->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  um->add_line("Name:test.ss");

  // chk
  my_assert("22b", um->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  um->add_line("Version:35");

  // chk
  my_assert("22c", !um->need_more_hdr(), "Should not have needed more header lines.");

  // attempt to parse
  um->parse_hdr();

  // chk vals
  my_assert("22d", um->ssname.compare("test.ss") == 0, "Wrong ss name.");
  my_assert("22e", um->version == 35, "Wrong version.");
}

void test23() {

  // SAVE test

  // add header line
  msg *m;
  m = parse_req_line("SAVE");
  m->add_line("SAVE");

  // cast
  save_msg *sm;
  sm = dynamic_cast<save_msg *>(m);

  // chk
  my_assert("23a", sm->need_more_hdr(), "Should have needed more header lines.");

  // add header line
  sm->add_line("Name:test.ss");

  // chk
  my_assert("23b", !sm->need_more_hdr(), "Should not have needed more header lines.");

  // attempt to parse
  sm->parse_hdr();

  // chk vals
  my_assert("23c", sm->ssname.compare("test.ss") == 0, "Wrong ss name.");
}

void test24() {

  // LEAVE test

  // add header line
  msg *m;
  m = parse_req_line("LEAVE");
  m->add_line("LEAVE");

  // cast
  leave_msg *lm;
  lm = dynamic_cast<leave_msg *>(m);

  // chk
  my_assert("24a", !lm->need_more_hdr(), "Should not have needed more header lines.");

  // attempt to parse (not necessary)
  lm->parse_hdr();

}

int main() {

  test1();

  test2();

  test3();
  
  test4();
  
  test5();

  test6();
  
  test7();

  test8();

  test9();

  test10();

  test11();

  test12();

  test13();

  test14();

  test15();

  test16();

  test17();

  test18();

  test19();

  test20();

  test21();

  test22();

  test23();

  test24();

  return 0;
}
