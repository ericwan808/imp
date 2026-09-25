#include "byte_stream.hh"
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), closed(false), total_pushed(0), total_popped(0), 
  size(0), buffer(""), head(0), tail(0){
    this->buffer.resize(capacity);
  }

// Push data to stream, but only as much as available capacity allows.
void Writer::push( string data )
{
  if(this->is_closed()){
    return;
  }

  uint64_t pushed_size = this->available_capacity() < data.size() ? this->available_capacity() : data.size();

  uint64_t segment_to_end = (this->capacity_ - this->tail) < pushed_size ? (this->capacity_ - this->tail) : pushed_size;

  auto buf_iter = this->buffer.begin();
  auto data_iter = data.begin();

  this->buffer.replace(buf_iter + this->tail, buf_iter+this->tail+segment_to_end, data_iter, data_iter+segment_to_end);
  this->buffer.replace(buf_iter, buf_iter+pushed_size-segment_to_end, data_iter+segment_to_end, data_iter+pushed_size);

  //this->buffer.append(data.begin(), data.begin() + pushed_size);

  this->tail = (this->tail + pushed_size)%capacity_;
  this->size += pushed_size;
  this->total_pushed += pushed_size;
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
  string peeked_string;
  if(this->tail > this->head){
    peeked_string = this->buffer.substr(this->head, this->tail);
  } else {
    peeked_string = this->buffer.substr(this->head, this->capacity_);
    peeked_string.append(this->buffer.substr(0, this->tail));
  }
  return peeked_string;
}

// Remove `len` bytes from the buffer.
void Reader::pop( uint64_t len )
{

  uint64_t popped_size = this -> size < len ? this->size: len;

  this->head = (this->head + popped_size)%this->capacity_;
  // this->buffer.erase(0, popped_size);

  this->total_popped += popped_size;
  this->size -= popped_size;
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
