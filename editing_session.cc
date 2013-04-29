/** 
  * editing_session.cc
  *
  * This is an editing session object.  This is the main object for which 
  * spreadsheet manipulation takes place.  There is a list of sockets that
  * are working on the spreadsheet object and the session is listening for 
  * incoming commands.  The editing session will evaluate if the command is
  * valid and send messages back to the sockets.
  *
  * Team: CHIJ
  * Members: Charlie Jacobsen, Hutch Jorgensen, Joe Sturzenegger, Ian Noy
  *
  * CS3505 - Spring 2013 
 */

#include "editing_session.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

// Constructor
editing_session::editing_session(std::string name, spreadsheet* s, lobby* const l) : name(name), s(s), l(l)
{
}

// Destructor
editing_session::~editing_session() {

  // Delete any sockets in session
  for (std::list<ss_sock *>::iterator it = this->socket_list.begin();
       it != this->socket_list.end();
       it++) {

    (*it)->stop();
    delete *it;

  }

  // Delete spreadsheet obj
  delete this->s;

}

// Join
void editing_session::join_session(ss_sock* ss)
{
  // Add socket to list
  socket_list.push_back(ss);

  std::cout << "Joined session!\n" << std::endl;

  // Convert xml file to string
  string file = s->get_xml();

  // Get version
  string version = boost::lexical_cast<string>(s->get_version());
  
  // Get length
  string length = boost::lexical_cast<string>(file.length());

  // Send message to socket for join success
  ss->async_write("JOIN OK\nName:" + name + "\nVersion:" + version + "\nLength:"
		 + length + "\n" + file + "\n",
		 boost::bind(&editing_session::handle_write,
			     this,
			     _1,
			     _2));

  // Start listening on this socket
  ss->async_rcv(boost::bind(&editing_session::handle_read,
			   this,
			   _1,
			   _2,
			   _3));    

  std::cout << "file: " << file << std::endl;

  std::cout << "Sent response!\n" << std::endl;

}

// Attempt Join
int editing_session::attempt_join(std::string name, std::string password, ss_sock* ss)
{
  // Socket is attempting to join the session
  //Need to check if password matches
  string passcode = s->get_password();
  
  if(passcode.compare(password) == 0)
  {
    // Password matches so add socket to session
    socket_list.push_back(ss);

    // Convert xml file to string
    string file = s->get_xml();

    // Get version
    string version = boost::lexical_cast<string>(s->get_version());
  
    // Get length
    string length = boost::lexical_cast<string>(file.length());

    // Send message to socket for join success
    ss->async_write("JOIN OK\nName:" + name + "\nVersion:" + version + "\nLength:"
		    + length + "\n" + file + "\n",
		    boost::bind(&editing_session::handle_write,
				this,
				_1,
				_2));

    // Start listening on this socket
    ss->async_rcv(boost::bind(&editing_session::handle_read,
			      this,
			      _1,
			      _2,
			      _3));       

    return 1;
  }

  else
  {
    // Password does not match
    return 0;
  }
}
  
// Read input from sockets
void editing_session::handle_read(ss_sock* s, msg* m, ss_sock_err e)
{
  // check for error
  if (e == no_err) {

    std::cout << "rcv'd msg in session!\n" << std::endl;

    // no error; determine message type

    switch (m->mtype()) {

    case create_msg_type:

      // cast
      create_msg *cm;
      cm = dynamic_cast<create_msg *>(m);

      // fire fail
      s->async_write("CREATE FAIL\nName:" + cm->ssname + "\nAlready connected to spreadsheet.\n",
		     boost::bind(&editing_session::handle_write,
				 this,
				 _1,
				 _2));

      
      break;

    case join_msg_type:

      // cast
      join_msg *jm;
      jm = dynamic_cast<join_msg *>(m);
 
      // fire fail
      s->async_write("JOIN FAIL\nName:" + cm->ssname + "\nAlready connected to spreadsheet.\n",
		     boost::bind(&editing_session::handle_write,
				 this,
				 _1,
				 _2));
      

      break;

    case change_msg_type:

      // cast
      change_msg *chm;
      chm = dynamic_cast<change_msg *>(m);

      // dispatch change msg to handle version checking
      process_change_msg(chm, s);

      break;

    case undo_msg_type:

      // cast
      undo_msg *um;
      um = dynamic_cast<undo_msg *>(m);

      //dispatch undo msg to handle checking
      process_undo_msg(um, s);

      break;

    case save_msg_type:

      // cast
      save_msg *sm;
      sm = dynamic_cast<save_msg *>(m);

      //dispatch save msg to handle save
      process_save_msg(sm, s);

      break;

    case leave_msg_type:
      
      // Remove socket from the list
      process_leave_msg(s);
      
      return;

    default:
      std::cerr << "Error: unrecognized message type! Should not be here...\n" << std::endl;
      break;
    }

    // listen for next message
    s->async_rcv(boost::bind(&editing_session::handle_read,
			     this,
			     _1,
			     _2,
			     _3));    

  } else {

    this->handle_close(s);

  }
}

// Send message to sockets
void editing_session::handle_write(ss_sock* s, ss_sock_err e)
{
  std::cout << "sent message!\n" << std::endl;

  if (e != no_err) {

    // close sock
    this->handle_close(s);

  }

}

// Function to process leave message
void editing_session::process_leave_msg(ss_sock* ss)
{
  // Remove client from the session
  this->socket_list.remove(ss);

  // Send client back to lobby
  l->join(ss);

  // If this is the last client leaving session then save spreadsheet
  if(this->socket_list.empty())
    {
      // save spreadsheet
      this->s->save_sheet();

      // notify lobby
      l->close_session(this);
    }
}

// Function to process save message call
void editing_session::process_save_msg(save_msg* m, ss_sock* ss)
{
  // Perform spreadsheet name check
  if((m->ssname).compare(name) != 0)
    {
      // Socket trying to save wrong spreadsheet send fail to socket
      ss->async_write("SAVE FAIL\nName:" + m->ssname + "\nPerform save on wrong spreadsheet.\n",
		      boost::bind(&editing_session::handle_write,
				  this,
				  _1,
				  _2));
    }
  else
    {
      // Name passes so save spreadsheet
      s->save_sheet();

      // Send message to client
      ss->async_write("SAVE OK\nName:" + m->ssname + "\n",
		      boost::bind(&editing_session::handle_write,
				  this,
				  _1,
				  _2));
    
    }

}

// Function to process undo message call
void editing_session::process_undo_msg(undo_msg* m, ss_sock* ss)
{
  // Perform spreadsheet name check
  if((m->ssname).compare(name) != 0)
    {
      // Socket trying to undo wrong spreadsheet send fail to socket
      ss->async_write("UNDO FAIL\nName:" + m->ssname + "\nPerform undo on wrong spreadsheet.\n",
		      boost::bind(&editing_session::handle_write,
				  this,
				  _1,
				  _2));
    }

  // Perform version check to accept change
  string version = boost::lexical_cast<string>(s->get_version());

  if(m->version != s->get_version())
    {
      // Socket trying to undo with old version, send wait response
      ss->async_write("UNDO WAIT\nName:" + m->ssname + "\nVersion:" + version
		      + "\n",
		      boost::bind(&editing_session::handle_write,
				  this,
				  _1,
				  _2));
    }
  else
    {
      // Version checks out, test if undo is available
    
      // Save exists so perform undo on spreadsheet
      string* cell_name = new string();
      string* cell_val = new string();
      int check = s->undo(cell_name, cell_val);

      // Get new version
      string new_version = boost::lexical_cast<string>(s->get_version());

      if(!check)
	{
	  // Undo not available, so send message to socket
	  ss->async_write("UNDO END\nName:" + m->ssname + "\nVersion:" + new_version
			  + "\n",
			  boost::bind(&editing_session::handle_write,
				      this,
				      _1,
				      _2));
	      
	}

      else
	{
	  // Get length of content
	  string length = boost::lexical_cast<string>(cell_val->length());

	  // Send message to socket for "UNDO"
	  ss->async_write("UNDO OK\nName:" + m->ssname + "\nVersion:" +
			  new_version + "\nCell:" + *cell_name + "\nLength:"
			  + length + "\n" + *cell_val + "\n",
			  boost::bind(&editing_session::handle_write,
				      this,
				      _1,
				      _2));

	  // Send message to other sockets "UPDATE"
	  // Loop through the list of sockets that are in the editing session
	  for(std::list<ss_sock*>::iterator it = socket_list.begin(); 
	      it != socket_list.end(); it++)
	    {
	      // If the iterator is pointing at the ss then dont send message
	      if((*it) != ss)
		{
		  (*it)->async_write("UPDATE \nName:" + m->ssname + "\nVersion:" +
				     new_version + "\nCell:" + *cell_name + "\nLength:" +
				     length + "\n" + *cell_val + "\n",
				     boost::bind(&editing_session::handle_write,
						 this,
						 _1,
						 _2));
      
		}
	    }
	}
    }
 
}

// Function to process change message call
void editing_session::process_change_msg(change_msg* m, ss_sock* ss)
{
  // Perform spreadsheet name check
  if((m->ssname).compare(name) != 0)
  {
    // Socket trying to update wrong spreadsheet send fail to socket
    ss->async_write("CHANGE FAIL\nName:" + m->ssname + "\nUpdating wrong spreadsheet.\n",
		   boost::bind(&editing_session::handle_write,
			       this,
			       _1,
			       _2));
  }

  // Perform version check to accept change
  string version = boost::lexical_cast<string>(s->get_version());

  if(m->version != s->get_version())
  {
    // Socket trying to update with old version, send wait response
    ss->async_write("CHANGE WAIT\nName:" + m->ssname + "\nVersion:" + version
		    + "\n",
		   boost::bind(&editing_session::handle_write,
			       this,
			       _1,
			       _2));
  }
  else
  {
    // Version check passes so update spreadsheet
    s->update_cell(m->cell, m->content);

    // Get new version
    string new_version = boost::lexical_cast<string>(s->get_version());

    // Get content
    string content = m->content;

    // Get length of content
    string length = boost::lexical_cast<string>(content.length());

    // Send message to socket "CHANGE OK"
    ss->async_write("CHANGE OK\nName:" + m->ssname + "\nVersion:" +
		    new_version + "\n",
		    boost::bind(&editing_session::handle_write,
				this,
				_1,
				_2));

    // Send message to other sockets "UPDATE"
    // Loop through the list of sockets that are in the editing session
    for(std::list<ss_sock*>::iterator it = socket_list.begin(); 
	it != socket_list.end(); it++)
    {
      // If the iterator is pointing at the ss then dont send message
      if((*it) != ss)
      {
	(*it)->async_write("UPDATE \nName:" + m->ssname + "\nVersion:" +
			   new_version + "\nCell:" + m->cell + "\nLength:" +
			   length + "\n" + content + "\n",
			   boost::bind(&editing_session::handle_write,
				       this,
				       _1,
				       _2));
      }
    }
  }
}

void editing_session::handle_close(ss_sock* ss) {

  // remove from waiting clients
  this->socket_list.remove(ss);

  // delete ss sock
  delete ss;

  // If this is the last client leaving session then save spreadsheet
  if(this->socket_list.empty())
    {
      // save spreadsheet
      this->s->save_sheet();

      // notify lobby
      l->close_session(this);
    }

}
