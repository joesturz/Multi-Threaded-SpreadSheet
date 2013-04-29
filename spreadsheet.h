#ifndef SPREADSHEET_H
#define SPREADSHEET_H
#include <string>
#include <map>
#include <stack>
#include "exceptions.h"

class spreadsheet
{
 public:

  // 
  // create_empty_ss
  // ---------------
  //
  // Creates an empty ss xml file with name / password.
  //
  // Throws a spreadsheet_exists exception if the
  // file already exists.
  //
  // Throws a spreadsheet_rw_exception if error
  // writing empty file.
  //
  static void create_empty_ss(const std::string& name, const std::string& password);

  //
  // constructor
  // -----------
  //
  // Checks if ss exists, and validates password.
  //
  // If ss exists and password okay, reads in ss xml
  // and populates ss model for later use.
  //
  // If ss does not exist, throws spreadsheet_not_exists exc.
  //
  // If ss does exist but wrong pword, throws spreadsheet_wrong_pword exc.
  //
  spreadsheet(const std::string ss_name, const std::string password)
    throw(spreadsheet_not_exists, spreadsheet_wrong_pword);

  // add or change a cell to the spreadsheet
  void update_cell(std::string cell_name, std::string cell_value);

  // revert to last change
  int undo(std::string *out_cell_name, std::string *out_cell_val);

  // save the spreadsheet
  void save_sheet();

  // get version number
  int get_version();

  // get spreadsheet xml of current state of ss
  std::string get_xml();

  // get password
  std::string get_password();

  // get spreadsheet name
  std::string get_ss_name();

 private:

  // list of cells
  std::map<std::string, std::string> cell_list;
  // spreadsheet name;
  std::string ss_name;
  // spreadsheet password
  std::string password;
  // saved stack cellnames that have been changed
  std::stack<std::string> undo_name_stack;
  // saved stack cellnames that have been changed
  std::stack<std::string> undo_cellval_stack;
  // spreadsheet version
  int version;
  // get cells value
  std::string get_cell_value(std::string cellname);
    
};

#endif
