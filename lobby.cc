/** 
  * lobby.cc
  *
  * This is a lobby object.  The lobby controls the movement of sockets to the
  * correct editing sessions.  There will be only two messages handled by the 
  * lobby: CREATE and JOIN. The lobby will notify the socket if the CREATE is
  * successful, or fails.  The lobby will also notify the socket if a JOIN
  * fails.
  *
  * Team: CHIJ
  * Members: Charlie Jacobsen, Hutch Jorgensen, Joe Sturzenegger, Ian Noy
  *
  * CS3505 - Spring 2013 
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "lobby.h"

using namespace std;

/* Constructor */
lobby::lobby() : waiting_socks(), ss_sessions()
{
}

/* Destructor */
lobby::~lobby()
{
  // Delete any ss socks in lobby
  for (set<ss_sock *>::iterator it = this->waiting_socks.begin();
       it != this->waiting_socks.end();
       it++) {
    
    (*it)->stop();
    delete *it;

  }

  // Tear down any active sessions
  for (map<string, editing_session *>::iterator it = 
	 this->ss_sessions.begin();
       it != this->ss_sessions.end();
       it++) {

    delete it->second;

  }
}

void lobby::create_ss(string name, string password, ss_sock* ss)
{
  //Need to check if spreadsheet already exists
  try
  {
    spreadsheet::create_empty_ss(name, password);
    
    // Send message to socket saying CREATE OK
    ss->async_write("CREATE OK\nName:" + name + "\nPassword:" + password + "\n",
		    boost::bind(&lobby::handle_write,
				this,
				_1,
				_2));

  }

  catch(spreadsheet_exists& e)
  {
    //Send message to socket saying already exists
    ss->async_write("CREATE FAIL\nName:" + name + "\nSpreadsheet already exists.\n",
		    boost::bind(&lobby::handle_write,
				this,
				_1,
				_2));

  }

  catch(spreadsheet_rw_exception& e)
  {
    //Send message to socket for failure
    ss->async_write("CREATE FAIL\nName:" + name + "\nError creating spreadsheet file.\n",
		    boost::bind(&lobby::handle_write,
				this,
				_1,
				_2));
  }

  // listen for next message
  ss->async_rcv(boost::bind(&lobby::handle_read,
			   this,
			   _1,
			   _2,
			   _3));

}

void lobby::join_ss(string name, string password, ss_sock* ss)
{
  //Check active spreadsheet session
  map<string, editing_session*>::iterator it;
  it = ss_sessions.find(name);
	
  if(it == ss_sessions.end())
  {
    //Session is not active, but check if spreadsheet can be opened
    try
    {
      std::cout << "Creating session!\n" << std::endl;

      //Create spreadsheet object
      spreadsheet* new_ss = new spreadsheet(name, password);

      std::cout << new_ss->get_xml() << std::endl;

      //If no exception thrown create new editing_session
      editing_session *new_session = new editing_session(name, new_ss, this);

      //Add editing session to list of active sessions
      ss_sessions.insert(pair<string, editing_session*>(name, new_session));

      //Send socket to editing session
      new_session->join_session(ss);

      //Remove socket from lobby
      waiting_socks.erase(ss);

      // Return
      return;
      
    }
    catch(spreadsheet_not_exists& e)
    {
      //Spreadsheet has not been created yet, send fail to socket
      ss->async_write("JOIN FAIL\nName:" + name + "\nSpreadsheet does not exist.\n",
		      boost::bind(&lobby::handle_write,
				  this,
				  _1,
				  _2));

      
    }
    catch(spreadsheet_wrong_pword& e)
    {
      //Spreadsheet password is incorrect send, fail to socket
      ss->async_write("JOIN FAIL\nName:" + name + "\nIncorrect spreadsheet password.\n",
		      boost::bind(&lobby::handle_write,
				  this,
				  _1,
				  _2));
    }
    catch(spreadsheet_rw_exception& e)
    {
      //Exception thrown, send fail to socket
      ss->async_write("JOIN FAIL\nName:" + name + "\nError joining spreadsheet fil.\n",
		      boost::bind(&lobby::handle_write,
				  this,
				  _1,
				  _2));
    }

    // listen for next message
    ss->async_rcv(boost::bind(&lobby::handle_read,
			     this,
			     _1,
			     _2,
			     _3));

  }
  else {

      //Spreadsheet is active, attempt join
      int atmpt_join = it->second->attempt_join(name, password, ss);

      if(atmpt_join)
      {
	//Join succeeded so remove sock from lobby
	waiting_socks.erase(ss);
      }

      else
      {
	//Join failed so send fail to client
	ss->async_write("JOIN FAIL\nName:" + name + "\nError joining spreadsheet.\n",
			boost::bind(&lobby::handle_write,
				    this,
				    _1,
				    _2));
	// listen for next message
	ss->async_rcv(boost::bind(&lobby::handle_read,
				 this,
				 _1,
				 _2,
				 _3));

      }
    }
}

void lobby::join(ss_sock* ss)
{

  std::cout << "Client joined!" << std::endl;

  //Add sock to lobby
  waiting_socks.insert(ss);

  //Start listening for client messages
  ss->async_rcv(boost::bind(&lobby::handle_read,
			    this,
			    _1,
			    _2,
			    _3));  
}

// socket, message, error
void lobby::handle_read(ss_sock *s, msg *m, ss_sock_err e) {

  // check for error
  if (e == no_err) {

    std::cout << "rcv'd msg!\n" << std::endl;

    // no error; determine message type

    switch (m->mtype()) {

    case create_msg_type:

      // cast
      create_msg *cm;
      cm = dynamic_cast<create_msg *>(m);

      // dispatch info to create_ss procedure
      create_ss(cm->ssname, cm->password, s);
      
      break;

    case join_msg_type:

      // cast
      join_msg *jm;
      jm = dynamic_cast<join_msg *>(m);

      // dispatch info to join_ss procedure
      join_ss(jm->ssname, jm->password, s);

      break;

    case change_msg_type:

      // cast
      change_msg *chm;
      chm = dynamic_cast<change_msg *>(m);

      // fire fail
      s->async_write("CHANGE FAIL\nName:" + chm->ssname + "\nNot connected to any spreadsheet.\n",
		     boost::bind(&lobby::handle_write,
				 this,
				 _1,
				 _2));

      break;

    case undo_msg_type:

      // cast
      undo_msg *um;
      um = dynamic_cast<undo_msg *>(m);

      // fire fail
      s->async_write("UNDO FAIL\nName:" + um->ssname + "\nNot connected to any spreadsheet.\n",
		     boost::bind(&lobby::handle_write,
				 this,
				 _1,
				 _2));
      break;

    case save_msg_type:

      // cast
      save_msg *sm;
      sm = dynamic_cast<save_msg *>(m);

      // fire fail
      s->async_write("SAVE FAIL\nName:" + sm->ssname + "\nNot connected to any spreadsheet.\n",
		     boost::bind(&lobby::handle_write,
				 this,
				 _1,
				 _2));
      break;

    case leave_msg_type:

      break;

    default:
      std::cerr << "Error: unrecognized message type! Should not be here...\n" << std::endl;
      break;
    }
    

  } else {

    // remove from waiting clients
    this->waiting_socks.erase(s);

    // delete ss sock
    delete s;

  }
}

void lobby::handle_write(ss_sock *s, ss_sock_err e) {

  // check for error
  if (e != no_err) {

    // remove from waiting clients
    this->waiting_socks.erase(s);

    // delete ss sock
    delete s;

  }

}

void lobby::close_session(editing_session * const es) {

  // remove session from map
  ss_sessions.erase(es->get_name());

  // delete editing session
  delete es;
}
