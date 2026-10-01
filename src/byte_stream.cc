#include "byte_stream.hh"
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ): capacity_( capacity )
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

  // only push up to capacity
  const uint64_t pushed_size = available_capacity() < data.size() ? available_capacity() : data.size();

  // if we need to wrap to the beginning of the ring, segment_to_end will be less than the pushed_size
  const uint64_t segment_to_end
    = ( capacity_ - tail ) < pushed_size ? ( capacity_ - tail ) : pushed_size;

  const auto buf_iter = buffer.begin();
  const auto data_iter = data.begin();

  // we push from the tail to the end of the ring or the entire pushed_size
  buffer.replace(
    buf_iter + tail, buf_iter + tail + segment_to_end, data_iter, data_iter + segment_to_end );

  // if we need to wrap around, we push the remaining data from the start of the ring
  // on the other hand, if segment_to_end == pushed_size, this line will do nothing
  buffer.replace(
    buf_iter, buf_iter + pushed_size - segment_to_end, data_iter + segment_to_end, data_iter + pushed_size );

  // update tail, size, and total_pushed
  tail = ( tail + pushed_size ) % capacity_;
  size += pushed_size;
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
  return capacity_ - size;
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
  // we can only pop up to the current buffer size
  const uint64_t popped_size = size < len ? size : len;

  // "free" the popped space by simply moving the head of the queue
  head = ( head + popped_size ) % capacity_;

  // update size and total_popped
  size -= popped_size;
  total_popped += popped_size;
}

// Is the stream finished (closed and fully popped)?
bool Reader::is_finished() const
{
  return closed && size == 0;
}

// Number of bytes currently buffered (pushed and not popped)
uint64_t Reader::bytes_buffered() const
{
  return size;
}

// Total number of bytes cumulatively popped from stream
uint64_t Reader::bytes_popped() const
{
  return total_popped;
}
