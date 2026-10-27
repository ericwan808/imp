#include "tcp_sender.hh"
#include "debug.hh"
#include "tcp_config.hh"

#include <algorithm>

using namespace std;

// How many sequence numbers are outstanding?
uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // last byte not yet acked - last byte acked
  return next_seqno_ - acked_;
}

// How many consecutive retransmissions have happened?
uint64_t TCPSender::consecutive_retransmissions() const
{
  return consec_retrans_;
}

void TCPSender::push( const TransmitFunction& transmit )
{

  // ensure we send at least one new seqno if desired
  uint64_t avail_space = max( win_size_, static_cast<uint64_t>( 1 ) ) - sequence_numbers_in_flight();

  // transmit as many messages as possible
  while ( avail_space > 0 ) {

    TCPSenderMessage new_sender_message = make_empty_message();

    // check if we need to send ISN and SYN
    if ( next_seqno_ == 0 ) {
      new_sender_message.SYN = true;
      avail_space -= 1;
    }

    if ( avail_space > 0 ) {
      const string_view payload = reader().peek();
      const size_t payload_size = min( { avail_space, payload.size(), TCPConfig::MAX_PAYLOAD_SIZE } );
      // trim payload if too large
      new_sender_message.payload = string( payload.substr( 0, payload_size ) );

      reader().pop( payload_size );
      avail_space -= new_sender_message.payload.size();
    }

    // send FIN if finished and there is space and no previous FIN sent
    if ( reader().is_finished() && avail_space > 0 && 
      next_seqno_ < reader().bytes_popped() + 2 ) {
      new_sender_message.FIN = true;
      avail_space -= 1;
    }

    // if we have nothing new to say, don't say anything
    if ( new_sender_message.sequence_length() == 0 ) {
      break;
    }

    transmit( new_sender_message );

    // next_seqno_ is now the ackno this message is waiting for
    next_seqno_ += new_sender_message.sequence_length();
    outstanding_.push( new_sender_message );
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  TCPSenderMessage emptyMessage = {
    .seqno = isn_.operator+( next_seqno_ ), .SYN = false, .payload = "", .FIN = false, .RST = input_.has_error() };
  return emptyMessage;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  if ( msg.RST ) {
    input_.set_error();
    return;
  }

  if ( msg.ackno ) {

    const uint64_t new_acked = msg.ackno->unwrap( isn_, acked_ );

    // old seqnos or bogus seqnos
    if ( new_acked < acked_ || new_acked > next_seqno_ ) {
      return;
    }

    // new seqnos, == case excluded to prevent timer reset
    if ( new_acked > acked_ ) {

      acked_ = new_acked;

      while ( !outstanding_.empty() ) {
        if ( outstanding_.front().seqno.unwrap( isn_, acked_ ) + outstanding_.front().sequence_length() > acked_ ) {
          break;
        }
        outstanding_.pop();
      }

      current_RTO_ms_ = initial_RTO_ms_;
      consec_retrans_ = 0;
      last_retrans_ = 0;
    }
  }

  // win_size_ is updated when acked_ is updated
  // as well as when there is no acked_ or new_acked == acked_
  win_size_ = msg.window_size;
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  // if there is nothing outstanding we don't need to keep track of anythingS
  if ( outstanding_.empty() ) {
    return;
  }

  // update time since last retransmission and quit if we don't need to retransmit
  last_retrans_ += ms_since_last_tick;
  if ( last_retrans_ < current_RTO_ms_ ) {
    return;
  }

  // retransmit oldest oustanding message
  transmit( outstanding_.front() );

  // backoff
  if ( win_size_ > 0 ) {
    current_RTO_ms_ *= 2;
  }

  last_retrans_ = 0;
  consec_retrans_ += 1;
}
