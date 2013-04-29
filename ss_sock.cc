/*
 * ss_sock.cc
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * ss_sock implementation.
 */

#include <boost/bind.hpp>
#include <iostream>
#include "ss_sock.h"
#include "parser.h"

ss_sock::ss_sock(boost::asio::ip::tcp::socket *s) {

  // store ref to boost socket
  this->s = s;

  // null out current msg
  this->curr_msg = NULL;

  // set to 0
  this->is_closed = 0;

}

ss_sock::~ss_sock() {

  // delete underlying socket
  delete this->s;

  // delete buffered msg, if any
  if (this->curr_msg) {
    delete this->curr_msg;
  }

  // delete any enqueued msgs
  while (this->msg_queue.size() > 0) {

    msg *m = this->msg_queue.front();
    this->msg_queue.pop_front();

    delete m;

  }

  std::cout << "sock deleted!\n" << std::endl;

  // other ivars free'd automagically

}

void ss_sock::start() {

  // begin listening for a line on the socket
  this->rcv_line();

}

void ss_sock::stop() {

  /* This doesn't work as it should...

     Currently, we're just deleting the socket
     without closing it.

     Perhaps a newer version of boost would work.
  
  boost::system::error_code ignored;

  if (this->s->is_open()) {

    this->s->shutdown(boost::asio::ip::tcp::socket::shutdown_both,
		      ignored);
    this->s->close(ignored);

  }

  */
}

void ss_sock::rcv_line() {

  boost::asio::async_read_until(*this->s,
				this->buff,
				'\n',
				boost::bind(&ss_sock::line_rcvd,
					    this,
					    boost::asio::placeholders::error,
					    boost::asio::placeholders::bytes_transferred));

}

void ss_sock::line_rcvd(const boost::system::error_code& e, 
			std::size_t bytes_transferred) {

  if (!e) {

    // extract line
    std::istream is(&this->buff);
    std::string line;
    std::getline(is, line);

    // chk for pending msg
    if (!this->curr_msg) {

      // message not currently being recv'd

      // parse line into a new msg
      try {

	this->curr_msg = parse_req_line(line);

      } catch(parsing_exception& e) {

	std::cout << "error: " << line << std::endl;

	// fail; send ERROR to client
	this->async_write("ERROR\n", 
			  boost::bind(&ss_sock::no_op, this, _1, _2));

	// ensure curr_msg is nulled out
	this->curr_msg = NULL;

	// listen for next line
	this->rcv_line();

	// leave
	return;

      }
    }

    // curr_msg is non null ...

    // add line to message
    this->curr_msg->add_line(line);

    // chk header
    if (!this->curr_msg->need_more_hdr()) {

      // chk if header parsed
      if (!this->curr_msg->hdr_is_parsed) {
	
	// parse hdr
	try {

	  this->curr_msg->parse_hdr();

	} catch(parsing_exception& e) {

	  std::cout << "error: " << line << std::endl;

	  // fail; send ERROR to client
	  this->async_write("ERROR\n", 
			    boost::bind(&ss_sock::no_op, this, _1, _2));

	  // delete buffered msg
	  delete this->curr_msg;
	  this->curr_msg = NULL;

	  // listen for next line
	  this->rcv_line();

	  // leave
	  return;

	}
      }

      // chk if we need body
      if (!this->curr_msg->need_more_body()) {

	// message fully rcv'd
	this->on_msg_rcvd();

      }
    }

    // begin listening for another line on the socket
    this->rcv_line();

  } else {

    // mark as closed
    this->is_closed = 1;

    // check if cb waiting
    if (this->rcv_cb_queue.size() > 0) {

      // fire first cb in queue
      rcv_cb cb = this->rcv_cb_queue.front();
      this->rcv_cb_queue.pop_front();

      if (e == boost::asio::error::eof) {

	// client closed connection
	cb(this, NULL, sock_closed);

      } else {

	cb(this, NULL, other_err);

      }
    }


  }
}

void ss_sock::on_msg_rcvd() {

  // chk if cb waiting
  if (this->rcv_cb_queue.size() > 0) {

    // dequeue callback and fire it
    rcv_cb cb = this->rcv_cb_queue.front();
    this->rcv_cb_queue.pop_front();

    cb(this, this->curr_msg, no_err);

    // delete msg
    delete this->curr_msg;
    this->curr_msg = NULL;

  } else {

    // enqueue msg
    this->msg_queue.push_back(this->curr_msg);

    // null out curr_msg
    this->curr_msg = NULL;

  }

}

void ss_sock::async_rcv(rcv_cb cb) {

  msg *m;

  // check if socket is already closed
  if (this->is_closed) {

    cb(this, NULL, sock_closed);
    return;

  }

  // socket is still open ...

  // check if msgs in queue
  if (this->msg_queue.size() > 0) {

    // fire cb immediately

    // pull off first msg
    m = this->msg_queue.front();
    this->msg_queue.pop_front();

    // fire callback
    cb(this, m, no_err);

    // delete msg
    delete m;

  } else {

    // enqueue callback
    
    this->rcv_cb_queue.push_back(cb);

  }
}

void ss_sock::snd_lines(const string& str) {

  boost::asio::async_write(*this->s,
			   boost::asio::buffer(str),
			   boost::bind(&ss_sock::lines_sent,
				       this,
				       boost::asio::placeholders::error));
}

void ss_sock::async_write(const string& str, send_cb cb) {


  // debug
  std::cout << "sending : \n" << str << std::endl;

  // chk if sock closed
  if (this->is_closed) {

    // fire cb immediately
    cb(this, sock_closed);

  } else {

    // enqueue cb
    this->snd_cb_queue.push_back(cb);

    // send str
    this->snd_lines(str);

  }  
}

void ss_sock::no_op(ss_sock *s, ss_sock_err e) { }

void ss_sock::lines_sent(const boost::system::error_code& error) {

  // there will always be a cb (either external or internal)
    
  send_cb cb = this->snd_cb_queue.front();
  this->snd_cb_queue.pop_front();

  if (!error) {

    // fire
    cb(this, no_err);

  } else {

    // mark as closed
    this->is_closed = 1;

    // chk if sock closed
    if (error == boost::asio::error::eof) {

      cb(this, sock_closed);

    } else {

      cb(this, other_err);

    }

  }
}
