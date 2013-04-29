/*
 * sock_tests.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Tests for ss_sock.
 */

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <pthread.h>
#include <sstream>
#include <iostream>
#include "testhdr.h"
#include "../ss_sock.h"

using boost::asio::ip::tcp;

int port = 4000;



/***** Callbacks ******/

typedef void (* srv_cb)(ss_sock *s);
typedef void (* clt_cb)(tcp::socket *s);




/***** Server and Client Setup Code *****/

void handle_accept(boost::asio::io_service *ios,
		   tcp::acceptor *acptr, 
		   tcp::socket *sock, 
		   srv_cb cb,
		   const boost::system::error_code& e) {

  if (!e) {

    // create ss_sock
    ss_sock *s = new ss_sock(sock);

    // fire handler
    cb(s);

    // setup new sock
    tcp::socket *sock = new tcp::socket(*ios);

    // begin accepting a conn
    acptr->async_accept(*sock,
			boost::bind(&handle_accept,
				    ios,
				    acptr,
				    sock,
				    cb,
				    boost::asio::placeholders::error));

  } else {

    std::cerr << "Error: " << e << "\n";

    exit(1);

  }

}

void* setup_server(void *_cb) {

  srv_cb cb = (srv_cb)_cb;

  try {

    // create io service
    boost::asio::io_service *io_service = new boost::asio::io_service();

    // setup endpoint
    tcp::endpoint endpoint(tcp::v4(), port);

    // setup acceptor
    tcp::acceptor *acceptor = new tcp::acceptor(*io_service, endpoint);

    // setup new sock
    tcp::socket *sock = new tcp::socket(*io_service);

    // begin accepting a conn
    acceptor->async_accept(*sock,
			   boost::bind(&handle_accept,
				       io_service,
				       acceptor,
				       sock,
				       cb,
				       boost::asio::placeholders::error));

    // signal caller with NULL
    cb(NULL);

    // start io service
    io_service->run();

    // ended; free acceptor and io_service
    delete acceptor;
    delete io_service;

  }
  catch (std::exception& e)  {

    std::cerr << "Exception: " << e.what() << "\n";
    
  }
}

void* setup_client(void *_cb) {

  clt_cb cb = (clt_cb)_cb;

  try {

    // create io service
    boost::asio::io_service io_service;

    // setup new sock
    tcp::socket *sock = new tcp::socket(io_service);

    // convert port to string
    std::stringstream ss;
    ss << port;

    // lookup
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), "localhost", ss.str());
    tcp::resolver::iterator iterator = resolver.resolve(query);

    // connect
    boost::asio::connect(*sock, iterator);

    // fire callback
    cb(sock);

    // start io service
    io_service.run();

  } catch (std::exception& e) {

    std::cerr << "Exception: " << e.what() << std::endl;

  }
}









namespace test1ns {

  pthread_t srv_thread;
  pthread_t clt_thread;

  void clt_connected(tcp::socket *s) {

    std::cout << "Client connected!" << std::endl;

    exit(0);

  }

  void srv_connected(ss_sock *s) {

    if (!s) {

      // server now accepting connections

      // setup client on another thread
      pthread_create(&clt_thread, NULL, &setup_client, 
		     (void *)&clt_connected);

    } else {

      std::cout << "Server connected!" << std::endl;

      pthread_join(clt_thread, NULL);

      delete s;

    }
  }

  void run() {

    pthread_create(&srv_thread, NULL, &setup_server, 
		   (void *)&srv_connected);

    pthread_join(srv_thread, NULL);

  }

}

void test1() {

  test1ns::run();

}



int main() {

  test1();

  return 0;
}
