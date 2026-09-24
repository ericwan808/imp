#include "byte_stream.hh"
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), closed(false), total_pushed(0), total_popped(0), size(0), buffer(""){}

// Push data to stream, but only as much as available capacity allows.
void Writer::push( string data )
{
  if(this->is_closed()){
    return;
  }

  uint64_t avail = this->available_capacity();
  if (data.size() > avail){
    this->buffer.append(data.substr(0, avail));
    this->size += avail;
    this->total_pushed += avail;
  } else {
    this->buffer.append(data);
    this->size += data.size();
    this->total_pushed += data.size();
  }
}

// Signal that the stream has reached its ending. Nothing more will be written.
void Writer::close()
{
  this->closed = true;
}

// Has the stream been closed?
bool Writer::is_closed() const
{
  return this->closed;
}

// How many bytes can be pushed to the stream right now?
uint64_t Writer::available_capacity() const
{
  return this->capacity_ - this->size;
}

// Total number of bytes cumulatively pushed to the stream
uint64_t Writer::bytes_pushed() const
{
  return this->total_pushed;
}

// Peek at the next bytes in the buffer -- ideally as many as possible.
// It's not required to return a string_view of the *whole* buffer, but
// if the peeked string_view is only one byte at a time, it will probably force
// the caller to do a lot of extra work.
string_view Reader::peek() const
{
  return this->buffer;
}

// Remove `len` bytes from the buffer.
void Reader::pop( uint64_t len )
{

  if (len > this->size){
    this->buffer = "";
    this->total_popped += this->size;
    this->size = 0;
    
  } else {
    this->buffer.erase(0, len);
    this->total_popped += len;
    this->size -= len;
  }
}

// Is the stream finished (closed and fully popped)?
bool Reader::is_finished() const
{
  return this->closed && this->size == 0;
}

// Number of bytes currently buffered (pushed and not popped)
uint64_t Reader::bytes_buffered() const
{
  return this->size;
}

// Total number of bytes cumulatively popped from stream
uint64_t Reader::bytes_popped() const
{
  return this->total_popped;
}
