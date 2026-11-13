#include <iostream>

#include "arp_message.hh"
#include "debug.hh"
#include "ethernet_frame.hh"
#include "exception.hh"
#include "helpers.hh"
#include "network_interface.hh"

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( InternetDatagram dgram, const Address& next_hop )
{
  const uint32_t next_hop_num = next_hop.ipv4_numeric();
 
  update_arp_requests( next_hop_num ); // check if ip is stale

  // if we have a valid ip
  if ( ip_to_eth_.contains( next_hop_num ) ) {

    const EthernetHeader header {
      .dst = ip_to_eth_[next_hop_num], .src = ethernet_address_, .type = EthernetHeader::TYPE_IPv4 };
    const EthernetFrame frame { .header = header, .payload = serialize( dgram ) };
    transmit( frame );
    return;
  }

  // queue datagram to be sent once arp response is recieved
  arp_requests_[next_hop_num].waiting_datagrams.push_back( dgram );

  // if we haven't sent an arp in the past or its been over 5 seconds, send an arp request
  if ( !arp_requests_[next_hop_num].sent || cur_time_ - arp_requests_[next_hop_num].sent_time >= 5000 ) {
    ARPMessage ARPrequest;
    ARPrequest.opcode = ARPMessage::OPCODE_REQUEST;
    ARPrequest.sender_ethernet_address = ethernet_address_;
    ARPrequest.sender_ip_address = ip_address_.ipv4_numeric();
    ARPrequest.target_ip_address = next_hop_num;

    const EthernetHeader header { .dst = ETHERNET_BROADCAST, .src = ethernet_address_, .type = EthernetHeader::TYPE_ARP };
    const EthernetFrame frame { .header = header, .payload = serialize( ARPrequest ) };
    transmit( frame );

    // log arp request
    arp_requests_[next_hop_num].sent_time = cur_time_;
    arp_requests_[next_hop_num].sent = true;
  }
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  const EthernetHeader recv_header = frame.header;

  // not intended for us
  if ( recv_header.dst != ethernet_address_ && recv_header.dst != ETHERNET_BROADCAST ) {
    return;
  }

  if ( recv_header.type == EthernetHeader::TYPE_ARP ) {
    ARPMessage arp;
    if ( parse( arp, frame.payload ) ) {

      const uint32_t learned_ip = arp.sender_ip_address;

      // remove stale arp info
      update_arp_requests( learned_ip );

      // update new arp info
      ip_to_eth_[learned_ip] = arp.sender_ethernet_address;
      arp_requests_[learned_ip].received_time = cur_time_;

      // send any queued datagrams
      list<InternetDatagram>& queued_dgrams = arp_requests_[learned_ip].waiting_datagrams;
      while ( !queued_dgrams.empty() ) {
        send_datagram( queued_dgrams.front(), Address::from_ipv4_numeric( learned_ip ) );
        queued_dgrams.pop_front();
      }

      // send arp reply if asking for our ethernet
      if ( arp.opcode == ARPMessage::OPCODE_REQUEST && arp.target_ip_address == ip_address_.ipv4_numeric() ) {
        ARPMessage ARPreply;
        ARPreply.opcode = ARPMessage::OPCODE_REPLY;
        ARPreply.sender_ethernet_address = ethernet_address_;
        ARPreply.sender_ip_address = ip_address_.ipv4_numeric();
        ARPreply.target_ethernet_address = arp.sender_ethernet_address;
        ARPreply.target_ip_address = learned_ip;

        const EthernetHeader header {
          .dst = arp.sender_ethernet_address, .src = ethernet_address_, .type = EthernetHeader::TYPE_ARP };
        const EthernetFrame arp_frame { .header = header, .payload = serialize( ARPreply ) };
        transmit( arp_frame );
      }
    }
  }


  // received datagram
  if ( recv_header.type == EthernetHeader::TYPE_IPv4 ) {
    InternetDatagram internet_datagram;
    if ( parse( internet_datagram, frame.payload ) ) {
      datagrams_received_.push( internet_datagram );
    }
  }
}

void NetworkInterface::update_arp_requests( uint32_t addr )
{
  // remove old ip and arp info past 30 seconds
  if ( ip_to_eth_.contains( addr ) && cur_time_ - arp_requests_[addr].received_time >= 30000 ) {
    arp_requests_[addr].sent = false;
    ip_to_eth_.erase( addr );
    arp_requests_[addr].waiting_datagrams.clear();
  }

  // remove queued datagrams if no arp reply within 5 seconds
  if ( arp_requests_[addr].sent && cur_time_ - arp_requests_[addr].sent_time >= 5000 ) {
    arp_requests_[addr].waiting_datagrams.clear();
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  cur_time_ += ms_since_last_tick;
}
