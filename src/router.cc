#include "router.hh"
#include "debug.hh"

#include <iostream>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  Router::ForwardingRule new_rule = {
    route_prefix,
    prefix_length,
    next_hop,
    interface_num
  };

  auto it = routing_table_.begin();
  while(it != routing_table_.end() && it->prefix_length >= prefix_length){
    ++it;
  }
  routing_table_.insert(it, new_rule);
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{
  for(shared_ptr<NetworkInterface> interf : interfaces_){

    while(!(interf->datagrams_received().empty())){

      InternetDatagram datagram = interf->datagrams_received().front();
      interf->datagrams_received().pop();

      IPv4Header header = datagram.header;
      if (datagram.header.ttl <= 1){
        continue;
      }
      datagram.header.ttl--;
      datagram.header.compute_checksum();

      uint32_t dst = header.dst;
      auto best_match = match_routes(dst);
      if(best_match == routing_table_.end()){
        continue;
      }
      shared_ptr<NetworkInterface> next_interface = interface(best_match->interface_num);

      if(best_match->next_hop){
        next_interface->send_datagram(datagram, *(best_match->next_hop));
      } else {
        next_interface->send_datagram(datagram, Address::from_ipv4_numeric(dst));
      }
    }
  }
}

vector<Router::ForwardingRule>::const_iterator Router::match_routes(uint32_t route){
  auto it = routing_table_.begin();
  while(it != routing_table_.end()){
    if(match(route, it->route_prefix, it->prefix_length)){
      break;
    }
    ++it;
  }
  return it;
}

bool Router::match(uint32_t a, uint32_t b, uint8_t prefix){ 
  if (prefix == 0) return true;

  uint32_t mask = 0xFFFFFFFF << (32 - prefix);
  return ((a & mask) == (b & mask));
}