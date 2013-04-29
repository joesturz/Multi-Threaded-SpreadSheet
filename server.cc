/**
  * server.cc
  *
  * This is the server object.  A server creates a lobby, accepts incoming 
  * connection requests, and passes the sockets to the lobby.
  *
  * Team: CHIJ
  * Members: Charlie Jacobsen, Hutch Jorgensen, Joe Sturzenegger, Ian Noy
  *
  * CS3505 - Spring 2013 
 */

#include "server.h"

using boost::asio::ip::tcp;

// Constructor
server::server(boost::asio::io_service *io_service,
	       tcp::endpoint *endpoint,
	       tcp::acceptor *acceptor): 
  io_service(io_service),
  endpoint(endpoint),
  acceptor(acceptor) { }

// Destructor
server::~server() {

  // delete lobby
  delete this->l;

  // delete acceptor
  delete this->acceptor;

  // delete io service
  delete this->io_service;
}

void server::start() {

  // Initialize lobby
  this->l = new lobby();

  // Start accepting clients...

  // Create tcp socket
  tcp::socket *sock = new tcp::socket(*this->io_service);

  // Begin accepting client connections
  this->acceptor->async_accept(*sock,
			       boost::bind(&server::handle_accept, 
					   this, 
					   sock,
					   boost::asio::placeholders::error));

  // start io service event loop
  this->io_service->run();
}

/* Method to start handling client connections */
void server::handle_accept(tcp::socket *sock,
			   const boost::system::error_code& error) {
  if (!error)
  {
    // create a spreadsheet socket
    ss_sock *ss = new ss_sock(sock);

    // start listening for msgs
    ss->start();

    // Add ss_sock to the lobby
    l->join(ss);

    // Create tcp socket
    tcp::socket *sock = new tcp::socket(*this->io_service);

    // Begin accepting client connections
    acceptor->async_accept(*sock,
			   boost::bind(&server::handle_accept, 
				       this, 
				       sock,
				       boost::asio::placeholders::error));
  }
  else
  {
    delete sock;
  }
}

void server::stop() {

  // cancel all ops on acceptor
  this->acceptor->cancel();

  // close acceptor
  this->acceptor->close();

  // stop io service
  this->io_service->stop();

}

 

