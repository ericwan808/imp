#include "socket.hh"

using namespace std;

int main()
{
  // construct an Internet or user datagram here, and send using a RawSocket
  string datagram;

  datagram += 0b0100'0101;    // version and IHL
  datagram += string( 7, 0 ); // rest of first two lines

  datagram += 64;             // TTL
  datagram += 5;              // protocol
  datagram += string( 6, 0 ); // rest of next two lines

  // destination address
  datagram += char( 10 );
  datagram += char( 144 );
  datagram += char( 0 );
  datagram += char( 86 );

  datagram += "Hello!";

  RawSocket {}.send( datagram, Address { "10.144.0.86" } );
}
