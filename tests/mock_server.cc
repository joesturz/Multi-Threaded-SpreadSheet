/*
 * mock_server.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Listens for connection req on port 4000. Once connected,
 * wraps socket in ss_sock, and listens for commands from
 * a client.
 */

#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <pthread.h>
#include "../ss_sock.h"
#include "../message.h"

using boost::asio::ip::tcp;

// global thread
pthread_t prompt_thread;

void snd(ss_sock *s, ss_sock_err e) {

  if (e != no_err) {

    // some error
    std::cerr << "\nSnd error: " << e << std::endl;

    // delete s
    delete s;

    // read char to hang
    char c;
    std::cin >> c;

    // exit
    exit(0);

  }
}

void* run_loop(void *_s) {

  ss_sock *s = (ss_sock *)_s;

  while (1) {

    string str;

    while (1) {

      // print prompt
      std::cout << ">> ";

      // wait for input
      string line;
      std::getline(std::cin, line);

      if (line.length() == 0) {

	s->async_write(str, &snd);
	break;	

      } else {

	str += line + "\n";

      }

    }
  }
}

void rcv(ss_sock *s, msg *m, ss_sock_err e) {

  if (e == no_err) {

    // print msg
    std::cout << std::endl
	      << m->to_string()
	      << std::endl;

    // print prompt
    std::cout << ">> ";
    std::cout.flush();

    // begin rcv'ing again
    s->async_rcv(&rcv);

  } else {

    // some error
    std::cerr << "\nRcv error: " << e << std::endl;

    // delete s
    delete s;

    // read char to hang
    char c;
    std::cin >> c;

    // exit
    exit(0);

  }
}


void handle_accept(tcp::socket *sock, const boost::system::error_code& e) {

  if (!e) {

    // notify
    std::cout << "\nClient connected!\n";

    // create ss_sock
    ss_sock *s = new ss_sock(sock);

    // start listening
    s->start();
    
    // begin receiving msgs
    s->async_rcv(&rcv);

    // start prompt loop on different thread
    pthread_create(&prompt_thread, NULL, &run_loop, (void *)s);

  } else {

    std::cerr << "Socket Error: " << e << std::endl;

    delete sock;

    exit(1);

  }
}

int main() {

  try
  {
    // create io service
    boost::asio::io_service io_service;

    // setup endpoint
    tcp::endpoint endpoint(tcp::v4(), 4000);

    // setup acceptor
    tcp::acceptor acceptor(io_service, endpoint);

    // setup new sock
    tcp::socket *sock = new tcp::socket(io_service);

    // begin accepting a conn
    acceptor.async_accept(*sock,
			  boost::bind(&handle_accept, 
				      sock,
				      boost::asio::placeholders::error));

    // start io service
    io_service.run();

  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
