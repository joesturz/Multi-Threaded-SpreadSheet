/**
  * server.h
  *
  * Server header file.  A server is the initial connection point for
  * sockets wishing to connect to a spreadsheet.  The server contains a
  * io_service object, a tcp acceptor object, and a lobby object.
  *
  * Team: CHIJ
  * Members: Charlie Jacobsen, Hutch Jorgensen, Joe Sturzenegger, Ian Noy
  *
  * CS3505 - Spring 2013 
 */

#ifndef SERVER_H
#define SERVER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "lobby.h"
#include "ss_sock.h"

using boost::asio::ip::tcp;

class server
{

 public:

  // Constructor
  server(boost::asio::io_service *io_service,
	 tcp::endpoint *endpoint,
	 tcp::acceptor *acceptor);

  // Destructor
  ~server();

  // Start
  void start();

  // Stop
  void stop();

  // Callback method to handle incoming socket connections
  void handle_accept(tcp::socket *sock, 
		     const boost::system::error_code& error);

 private:
  boost::asio::io_service *io_service;   //Service object
  tcp::endpoint *endpoint;
  tcp::acceptor *acceptor;               //TCP acceptor
  lobby *l;                              //Lobby object

};

#endif
