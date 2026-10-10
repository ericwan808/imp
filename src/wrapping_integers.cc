#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return zero_point.operator+( uint32_t( n ) );
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{

  uint64_t offset = raw_value_ - zero_point.raw_value_; // get first possible absolute seqno

  uint64_t num_loops = checkpoint & 0xFFFFFFFF00000000ULL; // get closest multiple of 2**32
  uint64_t checkpoint_offset = checkpoint & 0x00000000FFFFFFFFULL;

  // if the checkpoint_offset is close to the "top of the 2**32 section"
  // and the offset is close to the "bottom of the 2**32 section"
  // the closest unwrapped value will be in the next "section" up
  if ( checkpoint_offset > offset && 
    checkpoint_offset - offset > 2147483648 &&
    num_loops < 0xFFFFFFFF00000000ULL ) {
    num_loops += 0x0000000100000000ULL;
  }

  // vise versa if the checkpoint_offset is close to the "bottom"
  // TODO: check underflow
  if ( checkpoint_offset < offset && 
    offset - checkpoint_offset > 2147483648 &&
    num_loops > 0 ) {
    num_loops -= 0x0000000100000000ULL;
  }

  return { num_loops + offset }; // get closest valid wrap
}
