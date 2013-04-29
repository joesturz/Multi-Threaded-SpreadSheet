/**
  * editing_session.h
  *
  * This is the editing session header.  An editing session contain the name
  * of the spreadsheet that is being edited, the list of sockets editing the
  * spreadsheet, and the spreadsheet object itself.
  *
  * Team: CHIJ
  * Members: Charlie Jacobsen, Hutch Jorgensen, Joe Sturzenegger, Ian Noy
  *
  * CS3505 - Spring 2013 
 */

#ifndef EDITING_SESSION_H
#define EDITING_SESSION_H
#include <boost/bind.hpp>
#include "ss_sock.h"
#include "spreadsheet.h"
#include "message.h"
#include "lobby.h"
#include <list>

// Forward declaration of the lobby class
class lobby;

class editing_session
{

 public:
  // Constructor
  editing_session(std::string, spreadsheet*, lobby* const);

  // Destructor
  ~editing_session();
  
  // Join client 
  void join_session(ss_sock*);

  // Attempt to join client
  int attempt_join(std::string, std::string, ss_sock*);

  // Call back method for handling incoming messages from socket
  void handle_read(ss_sock *s, msg *m, ss_sock_err e);

  // Call back method for handing outgoing messages to socket
  void handle_write(ss_sock *s, ss_sock_err e);

  // Method to process change message
  void process_change_msg(change_msg*, ss_sock*);

  // Method to process undo message
  void process_undo_msg(undo_msg*, ss_sock*);

  // Method to process save message
  void process_save_msg(save_msg*, ss_sock*);

  // Method to process leave message
  void process_leave_msg(ss_sock*);

  // Name accessor
  string get_name() { return this->name; }

  // Handle socket close
  void handle_close(ss_sock* s);

 private:
  std::string name;                //spreadsheet name
  std::list<ss_sock*> socket_list; //list of sockets working in session
  spreadsheet* s;                  //spreadsheet object
  lobby* l;                        //lobby object
};

#endif
