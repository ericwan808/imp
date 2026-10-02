#include "reassembler.hh"
#include "debug.hh"

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
  // check for exceeding capacity
  // equivalent to total_popped + capacity_
  uint64_t unacceptable_index = 
    output_.writer().bytes_pushed() + output_.writer().available_capacity();

  // insert new segment if any part of it could be pushed
  if (first_index < unacceptable_index) {
    StreamSegment newSegment = {first_index, data};

    // sorted backwards based on index for easy popping
    for (int i = segments.size(); i >= 0; i--){
      if (i == 0){
        segments.insert(segments.begin() + i, newSegment);
        continue;
      }
      if (first_index < segments[i-1].index){
        segments.insert(segments.begin() + i, newSegment);
        continue;
      }
    }
  }

  // check if first segment can be pushed to byte_stream until can't anymore
  string pushed_string;
  while (segments[-1].index <= output_.writer().bytes_pushed()){
    
    // cut off front portion that is already pushed
    pushed_string = segments[-1].contents
    .substr(output_.writer().bytes_pushed() - segments[-1].index);
    
    // byte_stream will cut it off back portion if its too long
    output_.writer().push(pushed_string);
    segments.pop_back();
  }

  if ( is_last_substring ) {
    output_.writer().close();
  }
  debug( "unimplemented insert({}, {}, {}) called", first_index, data, is_last_substring );
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  debug( "unimplemented count_bytes_pending() called" );
  return {};
}
