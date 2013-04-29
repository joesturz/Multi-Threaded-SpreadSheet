/*
 * main.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Main entry point for server application.
 */

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <stdlib.h>
#include "ss_db.h"
#include "server.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{

  boost::asio::io_service *io_service = NULL;
  tcp::endpoint *endpoint = NULL;
  tcp::acceptor *acceptor = NULL;
  server *s = NULL;

  try
  {
    // Check for command line port
    int port; 

    if(argc > 1)
      {
	port = atoi(argv[1]);
      }
    else
      {
	port = 1984;
      }

    // initialize ss database
    ss_db::init_db();

    // create io service
    io_service = new boost::asio::io_service();

    // create endpoint
    endpoint = new tcp::endpoint(tcp::v4(), port);
    
    // create acceptor
    acceptor = new tcp::acceptor(*io_service,
				 *endpoint);

    // create server
    s = new server(io_service, endpoint, acceptor);

    // start server
    s->start();

    // stop server (something led the io service to quit)
    s->stop();

    // delete server
    delete s;

  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";

    // tear down
    if (s) {

      try { s->stop(); } catch(std::exception& e) { }

      delete s;

    } else {

      // server not created

      // delete all others that are non-NULL

      if (io_service) {

	delete io_service;
	
      }

      if (endpoint) {

	delete endpoint;

      }

      if (acceptor) {

	delete acceptor;

      }
    }
  }

  return 0;
}
