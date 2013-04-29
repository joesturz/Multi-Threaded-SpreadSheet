/*
 * ss_sock.h
 *
 * CHIJ
 * CS3505, Spring 2013
 *
 * Wrapper for a boost socket. Sends and receives
 * spreadsheet protocol messages to/from clients.
 */

#ifndef SS_SOCK_H
#define SS_SOCK_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <deque>
#include "message.h"

//
// ss_sock_err
// -----------
//
// Enumeration of different errors passed to
// receive / send callbacks.
//
enum ss_sock_err {
  no_err,
  sock_closed,
  other_err
};


class ss_sock;

//
// Receive / Send Callback Signatures
// ----------------------------------
//
// rcv_cb     void (ss_sock the_socket, msg *rcvd_msg, ss_sock_err error)
//
// send_cb    void (ss_sock the_socket, ss_sock_err error)
//

typedef boost::function<void(ss_sock *s, msg *m, ss_sock_err e)> rcv_cb;

typedef boost::function<void(ss_sock *s, ss_sock_err e)> send_cb;


class ss_sock
{
  //
  // Important:
  //
  // If ss_sock encounters any kind of socket error
  // when either sending or receiving data, it is
  // permanently closed.
  //
  // (This does not include parsing errors, if the client
  // sends a bad message.)
  //

 public:

  //
  // constructor
  // -----------
  //
  // Initializes state for ss_sock. s should be connected.
  //
  // Important: You must call 'start' to begin listening
  // for messages.
  //  
  ss_sock(boost::asio::ip::tcp::socket *s);

  //
  // destructor
  // ----------
  // 
  // Cleans up buffers, deletes underlying socket.
  //
  ~ss_sock();

  //
  // start
  // -----
  //
  // Causes ss_sock to begin listening for messages.
  //
  void start();

  //
  // stop
  // ----
  //
  // Closes underlying socket. This should be called
  // before calling the destructor.
  //
  void stop();

  //
  // async_rcv
  // ---------
  //
  // ss_sock will check if it has a buffered msg; if so,
  // it fires cb immediately, passing it the msg and
  // no_err.
  //
  // Otherwise, it enqueues cb, and fires it when it rcvs
  // a message and cb comes up in the callback queue.
  //
  // If the socket is closed, cb is fired immediately with
  // sock_closed.
  //
  // If there is an error during message receipt, and cb comes up
  // in the queue, it will be fired with other_err.
  //
  void async_rcv(rcv_cb cb);

  //
  // async_write
  // -----------
  //
  // ss_sock will send str async and enqueue cb in the
  // callback queue. When str is sent, cb will be fired
  // with no_err.
  //
  // If the socket is closed, cb is fired immediately with
  // sock_closed.
  //
  // If there is an error sending, cb will be fired
  // other_err.
  //
  void async_write(const string& str, send_cb cb);
  


 private:

  // Invariant: if msg_queue is non-empty then rcv_cb_queue is empty.

  // private ivars

  // the underlying socket
  boost::asio::ip::tcp::socket *s;

  // buffer for bytes received
  boost::asio::streambuf buff;

  // socket state
  int is_closed;

  // the msg currently being received (if any)
  msg *curr_msg;

  // buffered msgs that are waiting to be rcv'd (waiting for
  // async_rcv to be called)
  std::deque<msg *> msg_queue;

  // enqueued rcv callbacks that are waiting for msgs
  std::deque<rcv_cb> rcv_cb_queue;

  // enqueued send callbacks waiting for their lines to be sent
  std::deque<send_cb> snd_cb_queue;

  // private methods

  // underlying socket rcv callback
  void line_rcvd(const boost::system::error_code& e, 
		 std::size_t bytes_transferred);

  // underlying socket snd callback
  void lines_sent(const boost::system::error_code& e);
  
  // helpers
  void rcv_line();
  void snd_lines(const string& str);
  void on_msg_rcvd();
  void no_op(ss_sock *s, ss_sock_err e);

};

#endif
