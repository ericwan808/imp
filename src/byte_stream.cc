#include "byte_stream.hh"
#include "debug.hh"

#include <algorithm>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity )
{
  // immediately allocate space for the full buffer so string.replace does not throw an error
  buffer.resize( capacity, '0' );
}

// Push data to stream, but only as much as available capacity allows.
void Writer::push( string data )
{
  // check whether stream is closed, exit if it is
  if ( is_closed() ) {
    return;
  }

  const uint64_t tail = total_pushed % capacity_;

  // only push up to capacity
  const uint64_t pushed_size = available_capacity() < data.size() ? available_capacity() : data.size();

  // if we need to wrap to the beginning of the ring, segment_to_end will be less than the pushed_size
  const uint64_t segment_to_end = ( capacity_ - tail ) < pushed_size ? ( capacity_ - tail ) : pushed_size;

  // we push from the tail to the end of the ring or the entire pushed_size
  buffer.replace( tail, segment_to_end, data, 0, segment_to_end );

  // if we need to wrap around, we push the remaining data from the start of the ring
  // on the other hand, if segment_to_end == pushed_size, this line will do nothing
  buffer.replace( 0, pushed_size - segment_to_end, data, segment_to_end, pushed_size - segment_to_end );

  // update total_pushed
  total_pushed += pushed_size;
}

// Signal that the stream has reached its ending. Nothing more will be written.
void Writer::close()
{
  closed = true;
}

// Has the stream been closed?
bool Writer::is_closed() const
{
  return closed;
}

// How many bytes can be pushed to the stream right now?
uint64_t Writer::available_capacity() const
{
  // total_pushed - total_popped is the current amount of bytes in the buffer
  return capacity_ - ( total_pushed - total_popped );
}

// Total number of bytes cumulatively pushed to the stream
uint64_t Writer::bytes_pushed() const
{
  return total_pushed;
}

// Peek at the next bytes in the buffer -- ideally as many as possible.
// It's not required to return a string_view of the *whole* buffer, but
// if the peeked string_view is only one byte at a time, it will probably force
// the caller to do a lot of extra work.
string_view Reader::peek() const
{

  string_view peeked_string;

  const uint64_t size = bytes_buffered();
  const uint64_t head = total_popped % capacity_;

  if ( size < capacity_ - head ) {
    // if the queue does not wrap around, we can just read it from the head to the tail
    peeked_string = string_view( buffer ).substr( head, size );
  } else {
    // if the queue wraps, we read from the head to the end of the ring
    // this will require at most one other peek for the remaining buffer
    peeked_string = string_view( buffer ).substr( head, capacity_ - head );
  }

  return peeked_string;
}

// Remove `len` bytes from the buffer.
void Reader::pop( uint64_t len )
{
  const uint64_t size = bytes_buffered();

  // we can only pop up to the current buffer size
  const uint64_t popped_size = size < len ? size : len;

  // update total_popped
  total_popped += popped_size;
}

// Is the stream finished (closed and fully popped)?
bool Reader::is_finished() const
{
  return closed && ( bytes_buffered() == 0 );
}

// Number of bytes currently buffered (pushed and not popped)
uint64_t Reader::bytes_buffered() const
{
  return total_pushed - total_popped;
}
// Total number of bytes cumulatively popped from stream
uint64_t Reader::bytes_popped() const
{
  return total_popped;
}
