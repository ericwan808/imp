#include "reassembler.hh"
#include "debug.hh"

#include <algorithm>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  //  check for exceeding capacity
  //  equivalent to total_popped + capacity_
  const uint64_t unacceptable_index = output_.writer().bytes_pushed() + output_.writer().available_capacity();

  // queue new segment if any part of it is within capacity
  if ( first_index < unacceptable_index ) {
    const StreamSegment newSegment
      = { .index = first_index, .is_last_substring = is_last_substring, .data = move( data ) };

    // sort segments by first_index
    auto iter = segments.begin();
    while ( iter != segments.end() && iter->index < first_index ) {
      ++iter;
    }
    segments.insert( iter, newSegment );
  }

  // after each new insertion, try to push to the writer
  push_until_gap();
}

void Reassembler::push_until_gap()
{
  Writer& writer = output_.writer();

  // check if segment with lowest index can be pushed to byte_stream
  while ( !segments.empty() && segments.front().index <= writer.bytes_pushed() ) {
    StreamSegment& segment = segments.front();

    const uint64_t data_size = segment.data.size(); // size of data to be pushed
    const uint64_t start_byte
      = writer.bytes_pushed() - segment.index; // first byte in segment that needs to be pushed

    // if the next byte of data is already after this segment, we skip it
    if ( start_byte <= data_size ) {

      // cut off front overlapped data
      segment.data.erase( 0, start_byte );
      writer.push( move( segment.data ) );

      // check that all bytes are pushed before closing
      if ( segment.is_last_substring && writer.bytes_pushed() >= data_size + segment.index ) {
        writer.close();
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
  uint64_t count = 0;              // total counted bytes
  uint64_t last_counted_index = 0; // last index we've included in the count

  // stop counting after capacity
  const uint64_t unacceptable_index = output_.writer().bytes_pushed() + output_.writer().available_capacity();

  for ( const StreamSegment& segment : segments ) {

    // choose larger of last_counted or start of segment as start of count
    const uint64_t start_index = segment.index < last_counted_index ? last_counted_index : segment.index;

    // choose smaller of unacceptable index or last segment index as end of count
    const uint64_t end_index = unacceptable_index < segment.index + segment.data.size()
                                 ? unacceptable_index
                                 : segment.index + segment.data.size();

    // if the segment we want to count occurs after the unacceptable index we skip it
    // this can also occur if the data size is 0
    if ( end_index <= start_index ) {
      continue;
    }

    count += end_index - start_index;
    last_counted_index = end_index;
  }

  return count;
}
