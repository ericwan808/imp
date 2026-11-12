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

  if(ip_to_eth_.contains(next_hop.ipv4_numeric())){
    EthernetHeader header {.dst = ip_to_eth_[next_hop.ipv4_numeric()], .src  = ethernet_address_, .type = EthernetHeader::TYPE_IPv4};
    EthernetFrame frame {.header = header, .payload = serialize( dgram )};
    transmit(frame);
    return;
  }

  ARPMessage ARPrequest;
  ARPrequest.opcode = ARPMessage::OPCODE_REQUEST;

  ARPrequest.sender_ethernet_address = ethernet_address_;
  ARPrequest.sender_ip_address = ip_address_.ipv4_numeric();

  ARPrequest.target_ip_address = next_hop.ipv4_numeric();

  EthernetHeader header {.dst = ETHERNET_BROADCAST, .src = ethernet_address_, .type= EthernetHeader::TYPE_ARP};
  EthernetFrame frame {.header = header, .payload = serialize(ARPrequest)};
  transmit(frame);


  datagrams_waiting_.push(dgram);

}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  EthernetHeader header = frame.header;

  if (header.dst != ethernet_address_ || header.dst != ETHERNET_BROADCAST){
    return ;
  }

  if (header.type == EthernetHeader::TYPE_ARP){
    ARPMessage arp;
    if(parse(arp, frame.payload)){

      ip_to_eth_[arp.sender_ip_address] = arp.sender_ethernet_address;

      if(arp.opcode == ARPMessage::OPCODE_REQUEST){
        ARPMessage ARPreply;
        ARPreply.opcode = ARPMessage::OPCODE_REPLY;

        ARPreply.sender_ethernet_address = ethernet_address_;
        ARPreply.sender_ip_address = ip_address_.ipv4_numeric();
        ARPreply.target_ethernet_address = arp.sender_ethernet_address;
        ARPreply.target_ip_address = arp.sender_ip_address;

        EthernetHeader outHeader {.dst = arp.sender_ethernet_address, .src = ethernet_address_, .type= EthernetHeader::TYPE_ARP};
        EthernetFrame outframe {.header = outHeader, .payload = serialize(ARPreply)};
        transmit(outframe);
      
      }
      remembered_ips_.push({arp.sender_ip_address, cur_time_});
      // send waiting datagrams that match received ip
    }
  }

  if (header.type == EthernetHeader::TYPE_IPv4){
   InternetDatagram internet_datagram;
   if ( parse( internet_datagram, frame.payload ) ) {
    datagrams_received_.push(internet_datagram);
   }
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  cur_time_ += ms_since_last_tick;

  // forgets old ips
  while(!remembered_ips_.empty() && remembered_ips_.front().time_learned <= cur_time_ - 30000){
    uint32_t removed_ip = remembered_ips_.front().ip_address;
    remembered_ips_.pop();
    ip_to_eth_.erase(removed_ip);
  }
}
