#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if (message.RST){
    reassembler_.reader().set_error();
    return;
  }

  if ( message.SYN ) {
    zero_point_ = message.seqno;
  }

  if (!zero_point_){
    return;
  }
  uint64_t first_index = message.seqno.unwrap( *zero_point_, reassembler_.writer().bytes_pushed() );
  reassembler_.insert( first_index + static_cast<int>(message.SYN) - 1, message.payload, message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  uint16_t adjusted_window = reassembler_.writer().available_capacity() < UINT16_MAX ? 
    static_cast<uint16_t>(reassembler_.writer().available_capacity()) : UINT16_MAX;
  TCPReceiverMessage new_reciever_message { .ackno = nullopt, .window_size = adjusted_window, .RST = false };
  if (zero_point_){
    uint64_t raw_seqno = reassembler_.writer().bytes_pushed() + 1 + static_cast<int>(reassembler_.writer().is_closed());
    new_reciever_message.ackno = Wrap32::wrap(raw_seqno, *zero_point_);
  }

  if (reassembler_.reader().has_error()){
    new_reciever_message.RST = true;
  }

  return new_reciever_message;
}