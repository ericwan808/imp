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

  if(!ip_to_eth.contains(Address::ipv4_numeric(next_hop))){
    EthernetHeader header {dst = ip_to_eth_[Address::ipv4_numeric(next_hop)], src  = ethernet_address_, type = TYPE_IPv4};
    EthernetFrame frame {header = header, payload = serialize( dgram )};
    transmit(frame);
    return;
  }


  // if next_hop not in map
  // push to queue
  // send ARP request
  ARPMessage ARPreply;

        ARPreply.opcode = OPCODE_REPLY;

        ARPreply.sender_ethernet_address = ethernet_address_;
        ARPreply.sender_ip_address = Address::ipv4_numeric(ip_address_);

        ARPreply.target_ethernet_address = arp.sender_ethernet_address;
        ARPreply.target_ip_address = arp.sender_ip_address;


        EthernetHeader outHeader {dst = arp.sender_ethernet_address, src = ethernet_address_, type= TYPE_ARP};
        EthernetFrame outframe {header = outHeader, payload = serialize(ARPreply)};
        transmit(outframe);


  datagrams_waiting_.push({dgram, next_hop});
  // return


}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  EthernetHeader header = frame.header;

  if (header.type == TYPE_ARP){
    ARPMessage arp;
    if(parse(arp, frame.payload)){

      if(arp.opcode == OPCODE_REQUEST){
        ARPMessage ARPreply;

        ARPreply.opcode = OPCODE_REPLY;

        ARPreply.sender_ethernet_address = ethernet_address_;
        ARPreply.sender_ip_address = Address::ipv4_numeric(ip_address_);

        ARPreply.target_ethernet_address = arp.sender_ethernet_address;
        ARPreply.target_ip_address = arp.sender_ip_address;


        EthernetHeader outHeader {dst = arp.sender_ethernet_address, src = ethernet_address_, type= TYPE_ARP};
        EthernetFrame outframe {header = outHeader, payload = serialize(ARPreply)};
        transmit(outframe);
      
      } else if (arp.opcode == OPCODE_REPLY){
        ip_to_eth_[arp.sender_ip_address] = arp.sender_ethernet_address;
      }
    }
  }

  if (header.type == TYPE_IPv4){
   InternetDatagram internet_datagram;
   if ( parse( internet_datagram, frame.payload ) ) {
    datagrams_received_.push_back(internet_datagram);
   }
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  debug( "unimplemented tick({}) called", ms_since_last_tick );
}
