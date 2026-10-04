#include "reassembler.hh"
#include "debug.hh"

#include <algorithm>

using namespace std;

/*
 * Insert a new substring to be reassembled into a ByteStream.
 *   `first_index`: the index of the first byte of the substring
 *   `data`: the substring itself
 *   `is_last_substring`: this substring represents the end of the stream
 *   `output`: a mutable reference to the Writer
 *
 * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
 * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
 * learns the next byte in the stream, it should write it to the output.
 *
 * If the Reassembler learns about bytes that fit within the stream's available capacity
 * but can't yet be written (because earlier bytes remain unknown), it should store them
 * internally until the gaps are filled in.
 *
 * The Reassembler should discard any bytes that lie beyond the stream's available capacity
 * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
 *
 * The Reassembler should close the stream after writing the last byte.
 */
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

  // check if first segment can be pushed to byte_stream until can't anymore
  uint64_t data_size;
  uint64_t start_byte;
  uint64_t pushed_size;

  while ( segments.size() > 0 && segments.front().index <= output_.writer().bytes_pushed() ) {

    data_size = segments.front().data.size();

    // get index of first byte in data that needs to be pushed
    start_byte = output_.writer().bytes_pushed() - segments.front().index;

    // if the next byte of data is already after this segment, we skip it
    if ( start_byte <= data_size ) {

      // check for capacity constraints before pushing
      pushed_size = min( data_size - start_byte, output_.writer().available_capacity() );

      output_.writer().push( segments.front().data.substr( start_byte, pushed_size ) );

      // need to check that all bytes of a "last_substring" are pushed before closing
      if ( segments.front().is_last_substring && start_byte + pushed_size == data_size ) {
        output_.writer().close();
      }
    }

    // remove segment once processed
    segments.pop_front();
  }
  // debug( "unimplemented insert({}, {}, {}) called", first_index, data, is_last_substring );
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
