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
  uint32_t next_hop_num = next_hop.ipv4_numeric();

  update_arp_requests(next_hop_num);

  // update arp_requests_[next_hop_num] and ip_to_eth_[next_hop_num] if necessary
  if(ip_to_eth_.contains(next_hop_num)){ // if we've received an arp in the past

      EthernetHeader header {.dst = ip_to_eth_[next_hop_num], .src  = ethernet_address_, .type = EthernetHeader::TYPE_IPv4};
      EthernetFrame frame {.header = header, .payload = serialize( dgram )};
      transmit(frame);
      return;

  }

  arp_requests_[next_hop_num].waiting_datagrams.push_back(dgram);

  if (!arp_requests_[next_hop_num].sent || cur_time_ - arp_requests_[next_hop_num].sent_time >= 5000){
    ARPMessage ARPrequest;
    ARPrequest.opcode = ARPMessage::OPCODE_REQUEST;

    ARPrequest.sender_ethernet_address = ethernet_address_;
    ARPrequest.sender_ip_address = ip_address_.ipv4_numeric();

    ARPrequest.target_ip_address = next_hop_num;

    EthernetHeader header {.dst = ETHERNET_BROADCAST, .src = ethernet_address_, .type= EthernetHeader::TYPE_ARP};
    EthernetFrame frame {.header = header, .payload = serialize(ARPrequest)};
    transmit(frame);

    arp_requests_[next_hop_num].sent_time = cur_time_;
    arp_requests_[next_hop_num].sent = true;
  }
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  EthernetHeader header = frame.header;

  if (header.dst != ethernet_address_ && header.dst != ETHERNET_BROADCAST){
    return ;
  }

  if (header.type == EthernetHeader::TYPE_ARP){
    ARPMessage arp;
    if(parse(arp, frame.payload)){

      uint32_t learned_ip = arp.sender_ip_address;

      update_arp_requests(learned_ip);

      ip_to_eth_[learned_ip] = arp.sender_ethernet_address;
      arp_requests_[learned_ip].received_time = cur_time_;

      list<InternetDatagram>& queued_dgrams = arp_requests_[learned_ip].waiting_datagrams;

      while(!queued_dgrams.empty()){
        send_datagram(queued_dgrams.front(), Address::from_ipv4_numeric(learned_ip));
        queued_dgrams.pop_front();
      }

      if(arp.opcode == ARPMessage::OPCODE_REQUEST && arp.target_ip_address == ip_address_.ipv4_numeric()){
        ARPMessage ARPreply;
        ARPreply.opcode = ARPMessage::OPCODE_REPLY;

        ARPreply.sender_ethernet_address = ethernet_address_;
        ARPreply.sender_ip_address = ip_address_.ipv4_numeric();
        ARPreply.target_ethernet_address = arp.sender_ethernet_address;
        ARPreply.target_ip_address = learned_ip;

        EthernetHeader outHeader {.dst = arp.sender_ethernet_address, .src = ethernet_address_, .type= EthernetHeader::TYPE_ARP};
        EthernetFrame outframe {.header = outHeader, .payload = serialize(ARPreply)};
        transmit(outframe);
      
      }

    }
  }

  if (header.type == EthernetHeader::TYPE_IPv4){
   InternetDatagram internet_datagram;
   if ( parse( internet_datagram, frame.payload ) ) {
    datagrams_received_.push(internet_datagram);
   }
  }
}

void NetworkInterface::update_arp_requests(uint32_t addr){

    if(ip_to_eth_.contains(addr) && cur_time_ - arp_requests_[addr].received_time >= 30000){
      arp_requests_[addr].sent = false;
      ip_to_eth_.erase(addr);
      arp_requests_[addr].waiting_datagrams.clear();
    }

    if(arp_requests_[addr].sent && cur_time_ - arp_requests_[addr].sent_time >= 5000){
      arp_requests_[addr].waiting_datagrams.clear();
    }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  cur_time_ += ms_since_last_tick;
}
