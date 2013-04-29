/*
 * spreadsheet.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Created by the sturzenegger!
 *
 * Spreadsheet implementation.
 */

#include <string>
#include <map>
#include <stack>
#include <sstream>
#include <iostream>
#include "spreadsheet.h"
#include "ss_xml.h"
#include "ss_db.h"

void spreadsheet::create_empty_ss(const std::string& name, const std::string& password) {
  
  int id;
  std::map<std::string, std::string> empty_map;
  std::stringstream ss;

  // attempt to insert into db (may throw spreadsheet_exists exc)
  id = ss_db::insert_db(name, password);

  // create filename (without xml extension)
  ss << id;

  // create empty xml file
  save_ss_file(ss.str(), password, empty_map);

}

spreadsheet::spreadsheet(const std::string ss_name, const std::string password)
  throw(spreadsheet_not_exists, spreadsheet_wrong_pword)
{
  int id;
  std::stringstream ss;

  // query the db (could through exc)
  id = ss_db::query_db(ss_name, password);
  
  // remember name and password
  this->password = password;
  this->ss_name  = ss_name;

  // init vnum to 0
  this->version = 0;

  // convert id to string
  ss << id;

  // read in xml  
  this->cell_list = get_cells_from_file(ss.str());

  std::cout << "cells: " << cell_list.size() << std::endl;

  // init undo stacks
  std::stack<std::string> undo_name_stack;
  std::stack<std::string> undo_cellval_stack;
}

// add or change a cell to the spreadsheet
void spreadsheet::update_cell(std::string cell_name, std::string cell_value)
{
    std::string former_value = get_cell_value(cell_name);
    // add cell name to undo name stack
    undo_name_stack.push(cell_name);
    // add cell value to undo value stack
    undo_cellval_stack.push(former_value);
    
    // add cell to list
    cell_list[cell_name] = cell_value;
    version++;
    return;
}
// revert to last change
int spreadsheet::undo(std::string *out_cell_name, std::string *out_cell_val) {

    if (!undo_name_stack.empty())
    {
        // get the cell values on the two stacks
        *out_cell_val = undo_cellval_stack.top();
        *out_cell_name = undo_name_stack.top();
        
        // pop the cell values
        undo_cellval_stack.pop();
        undo_name_stack.pop();
        // if cell value is empty
        if (*out_cell_val == "")
        {
            //remove from the SS map
            cell_list.erase(*out_cell_name);
            
        }
        else
        {
            // update SS map
            cell_list[*out_cell_name] = *out_cell_val;
        }
        version++;

        return 1;

    } else {

      return 0;

    }
}
// save the spreadsheet
void spreadsheet::save_sheet()
{
  int id;
  std::stringstream ss;

  // query db for ss id
  id = ss_db::query_db(ss_name, password);

  // convert id to string
  ss << id;

  save_ss_file(ss.str(), password, cell_list);
  return;
}
//get version number
int spreadsheet::get_version()
{
    return version;
}

// get spreadsheet xml of current state of ss
std::string spreadsheet::get_xml() {

  std::cout << this->cell_list.size() << std::endl;
  return map_to_xml(this->cell_list);

}

// get password
std::string spreadsheet::get_password()
{
    return password;
}
// get spreadsheet name
std::string spreadsheet::get_ss_name()
{
    return ss_name;
}
std::string spreadsheet::get_cell_value(std::string cellname)
{
  for(std::map<std::string, std::string>::iterator ii=cell_list.begin(); ii!=cell_list.end(); ++ii) {

    if ((*ii).first == cellname)
      {
	// if the value has a value return it
	return (*ii).second;
      }
  }

  //other wise return an empty string;
  return "";
}
/*
int main()
{
    map<string, string> cells;
    
    int test_version;
    std::string test_password;
    std::string test_name;
    
    map<string, string> cells_test;
    cells_test["A1"] = "hello";
    cells_test["B2"] = "123";
    cells_test["C3"] = "=B2";
    string password = "test_password";
    string name = "test_name";
    
    spreadsheet test1 = spreadsheet(cells_test, name, password);
    
    test_name = test1.get_ss_name();
    test_password = test1.get_password();
    test_version = test1.get_version();
    
    std::cout << test_name << endl;
    std::cout << "version " << test_version << endl;
    std::cout << test_password << endl;
    
    test1.update_cell("A1", "hi");
    test1.update_cell("B2", "321");
    
    test_name = test1.get_ss_name();
    test_password = test1.get_password();
    test_version = test1.get_version();
    
    std::cout << test_name << endl;
    std::cout << "version "  << test_version << endl;
    std::cout << test_password << endl;
    
    
    cells_test = test1.get_cells();
    for( map<string, string>::iterator ii=cells_test.begin(); ii!=cells_test.end(); ++ii)
    {
        cout << (*ii).first << "," << (*ii).second << endl;
    }
    test1.undo();
    test1.undo();
    test1.undo();
    cells_test = test1.get_cells();
    for( map<string, string>::iterator ii=cells_test.begin(); ii!=cells_test.end(); ++ii)
    {
        cout << (*ii).first << "," << (*ii).second << endl;
    }
    test_name = test1.get_ss_name();
    test_password = test1.get_password();
    test_version = test1.get_version();
    
    std::cout << test_name << endl;
    std::cout << "version "  << test_version << endl;
    std::cout << test_password << endl;

    return 0;
}
*/

