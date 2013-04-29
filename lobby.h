/**
  * lobby.h
  *
  * This is the header for a lobby object. A lobby contains a list of 
  * sockets that are currently waiting to join an editing session as well
  * as a list of active editing sessions.
  *
  * Team: CHIJ
  * Members: Charlie Jacobsen, Hutch Jorgensen, Joe Sturzenegger, Ian Noy
  *
  * CS3505 - Spring 2013 
 */

#ifndef LOBBY_H
#define LOBBY_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <set>
#include <map>
#include "message.h"
#include "ss_sock.h"
#include "spreadsheet.h"
#include "editing_session.h"

// Forward declaration editing session
class editing_session;

class lobby
{
 public:

  // Constructor
  lobby();

  // Destructor
  ~lobby();

  // Method to evaluate a CREATE message
  void create_ss(std::string, std::string, ss_sock*);

  // Method to evaluate a JOIN message
  void join_ss(std::string, std::string, ss_sock*);

  // Join socket to lobby
  void join(ss_sock* ss);

  // Call back method for handling incoming messages from socket
  void handle_read(ss_sock *s, msg *m, ss_sock_err e);

  // Call back method for handing outgoing messages to socket
  void handle_write(ss_sock *s, ss_sock_err e);

  // Close editing session
  void close_session(editing_session * const);

 private:
  // Sockets currently in lobby
  std::set<ss_sock*> waiting_socks;      
  // Active editing_sessions
  std::map<std::string, editing_session *> ss_sessions; 

};

#endif
