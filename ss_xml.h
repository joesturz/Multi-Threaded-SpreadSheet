/*
 * ss_xml.h
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Created by Joe Sturzenegger on 4/16/13.
 *
 * Saves / reads spreadsheet xml files.
 */

#ifndef SS_XML_H
#define SS_XML_H

#include <map>

// saves a file given a filename, a password and a map of cells
int save_ss_file(const std::string& filename, const std::string& password, 
		 std::map<std::string, std::string> cells);

// convert map to xml string
std::string map_to_xml(std::map<std::string, std::string> cells);


// adds all the cells in the file to the map
std::map<std::string, std::string> get_cells_from_file(const std::string& filename);

#endif
