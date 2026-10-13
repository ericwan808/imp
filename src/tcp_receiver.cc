#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // if this is the first time getting a SYN, set the ISN
  if ( message.SYN && !zero_point_) {
    zero_point_ = message.seqno;
  }

  // abort if other side sends RST or we are missing the ISN
  if (message.RST || !zero_point_){
    reassembler_.reader().set_error();
    return;
  }

  // unwrap seqno to obtain absolute seqno
  uint64_t abs_seqno = message.seqno.unwrap( *zero_point_, reassembler_.writer().bytes_pushed() )
    + static_cast<int>(message.SYN);
  
  // SYN takes up one absolute seqno but no stream indexes
  uint64_t stream_index = abs_seqno - 1;
  
  reassembler_.insert( stream_index, message.payload, message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  // window size is at most UINT16_MAX
  uint16_t adjusted_window = reassembler_.writer().available_capacity() < UINT16_MAX ? 
    static_cast<uint16_t>(reassembler_.writer().available_capacity()) : UINT16_MAX;

  // default is no ackno
  TCPReceiverMessage new_reciever_message { .ackno = nullopt, .window_size = adjusted_window, .RST = false };

  if (zero_point_){
    // +1 for SYN and +1 for FIN if stream is closed
    uint64_t abs_seqno = reassembler_.writer().bytes_pushed() + 1 + static_cast<int>(reassembler_.writer().is_closed());
    new_reciever_message.ackno = Wrap32::wrap(abs_seqno, *zero_point_);
  }

  // if an error has occured ask to RST
  if (reassembler_.reader().has_error()){
    new_reciever_message.RST = true;
  }

  return new_reciever_message;
}