#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // wrapping occurs automatically in uint32 addition
  return zero_point.operator+( static_cast<uint32_t>( n ) );
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // get first possible absolute seqno
  const uint64_t offset = raw_value_ - zero_point.raw_value_;

  // get closest multiple of 2**32 and remainder
  uint64_t num_loops = checkpoint & 0xFFFFFFFF00000000ULL;
  const uint64_t checkpoint_offset = checkpoint & 0x00000000FFFFFFFFULL;

  // if the checkpoint_offset is close to the "top of the 2**32 section"
  // and the offset is close to the "bottom of the 2**32 section"
  // the closest unwrapped value will be in the next "section" up
  if ( checkpoint_offset > offset && checkpoint_offset - offset > 2147483648
       && num_loops < 0xFFFFFFFF00000000ULL ) { // prevent overflow
    num_loops += 0x0000000100000000ULL;
  }

  // vise versa if the checkpoint_offset is close to the "bottom"
  if ( checkpoint_offset < offset && offset - checkpoint_offset > 2147483648
       && num_loops > 0 ) { // prevent underflow
    num_loops -= 0x0000000100000000ULL;
  }

  return num_loops + offset; // get closest valid wrap
}
