#include "reassembler.hh"
#include "debug.hh"

#include <algorithm>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Writer writer = output_.writer();
  //  check for exceeding capacity
  //  equivalent to total_popped + capacity_
  uint64_t unacceptable_index = output_.writer().bytes_pushed() + output_.writer().available_capacity();

  // insert new segment if any part of it could be pushed
  if ( first_index < unacceptable_index ) {
    StreamSegment newSegment = { first_index, is_last_substring, data };

    list<StreamSegment>::iterator iter = segments.begin();

    while ( iter != segments.end() && iter->index < first_index ) {
      ++iter;
    }
    segments.insert( iter, newSegment );
  }

  // after each new insertion, try to push to the writer
  push_until_gap();
}

void Reassembler::push_until_gap(){

  uint64_t data_size;
  uint64_t start_byte;

  // check if segment with lowest index can be pushed to byte_stream
  while ( segments.size() > 0 && segments.front().index <= output_.writer().bytes_pushed() ) {

    data_size = segments.front().data.size();

    // get index of first byte in segment's data that needs to be pushed
    start_byte = output_.writer().bytes_pushed() - segments.front().index;

    // if the next byte of data is already after this segment, we skip it
    if ( start_byte <= data_size ) {

      // overcapacity is handled by push itself
      output_.writer().push( segments.front().data.substr( start_byte ) );

      // check that all bytes are pushed before closing
      if ( segments.front().is_last_substring && 
        output_.writer().bytes_pushed() >= data_size + segments.front().index ) {
        output_.writer().close();
      }
    }

    // remove segment once processed
    segments.pop_front();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  uint64_t count = 0;
  uint64_t last_counted_index = 0;

  uint64_t unacceptable_index = output_.writer().bytes_pushed() + output_.writer().available_capacity();

  uint64_t start_index;
  uint64_t end_index;

  for ( StreamSegment segment : segments ) {

    if ( last_counted_index >= segment.index + segment.data.size() ) {
      continue;
    }

    end_index = unacceptable_index < segment.index + segment.data.size() ? unacceptable_index
                                                                         : segment.index + segment.data.size();
    start_index = segment.index < last_counted_index ? last_counted_index : segment.index;

    if ( end_index <= start_index ) {
      continue;
    }

    count += end_index - start_index;
    last_counted_index = end_index;
  }

  // debug( "unimplemented count_bytes_pending() called" );
  return count;
}
