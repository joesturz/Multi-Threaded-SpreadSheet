/*
 * ss_xml.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Created by Joe Sturzenegger on 4/16/13.
 *
 *  xml spec:
 *
 * <?xml version="1.0" encoding="utf-8"?>
 * <spreadsheet>
 *   <cell>
 *     <name>A1</name>
 *     <contents>33</name>
 *   </cell>
 * </spreadsheet>
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "ss_xml.h"

using namespace std;

// saves a file given a filename, a password and a map of cells
int save_ss_file(const string& filename, const string& password, map<string, string> cells)
{
  ofstream outputFile;
  string fname;

  fname = filename + ".xml";
  outputFile.open(fname.c_str());
    
  // add xml hdr
  outputFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
  outputFile << "<spreadsheet>" << endl;
    
  // add cells to xml
  for( map<string, string>::iterator ii=cells.begin(); ii!=cells.end(); ++ii) {

    outputFile << "<cell>" << endl;
    outputFile << "<name>" << endl;
    outputFile << (*ii).first << endl;
    outputFile << "</name>" << endl;
    outputFile << "<contents>" << endl;
    outputFile << (*ii).second << endl;
    outputFile << "</contents>" << endl;
    outputFile << "</cell>" << endl;

  }
  outputFile << "</spreadsheet>" << endl;
    
  outputFile.close();
  return 0;
}

std::string map_to_xml(std::map<std::string, std::string> cells) {

  stringstream ss;

  // add xml hdr
  ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
  ss << "<spreadsheet>";

  // add cells to xml
  for(map<string, string>::iterator ii=cells.begin(); ii!=cells.end(); ++ii) {

    ss << "<cell>";
    ss << "<name>";
    ss << (*ii).first;
    ss << "</name>";
    ss << "<contents>";
    ss << (*ii).second;
    ss << "</contents>";
    ss << "</cell>";

  }
  ss << "</spreadsheet>";
    
  // return string
  return ss.str();
}

// adds all the cells in the file to the map
map<string, string> get_cells_from_file(const string& filename)
{
  string fname;

  fname = filename + ".xml";
  ifstream in;
  in.open(fname.c_str());

  string word;
  string password;
  string name;
  string contents;
  map<string, string> cells;
  while (true)
    {
      in >> word;
      if (word == "<cell>")
        {
	  in >> word; // name header
	  in >> name; // name
	  in >> word; // name footer
	  in >> word; // contents header

	  getline(in, contents);
	  while (contents.length() == 0) {

	    getline(in, contents);

	  }

	  cells[name] = contents; // add cell to map
            
	  //cout << name << "," << contents << endl;
            
        }
      if (word == "</spreadsheet>")
        {
	  std::cout << cells.size() << std::endl;
	  in.close();            
	  return cells;
        }
    }


}

// main used for testing purposes

/*
int main()
{
    map<string, string> cells;
    map<string, string> cells_test;
    cells["A1"] = "hello";
    cells["B2"] = "123";
    cells["C3"] = "=B2";
    string filename, password;
    cout << "Enter the filename: ";
    cin >> filename;
    cout << "Enter the password: ";
    cin >> password;
    save_ss_file(filename, password, cells);
    string pw = get_password_from_file(filename);
    cout << pw << endl;
    cells_test = get_cells_from_file(filename);
    for( map<string, string>::iterator ii=cells.begin(); ii!=cells.end(); ++ii)
    {
        cout << (*ii).first << "," << (*ii).second << endl;
    }
    return 0;
}
*/
