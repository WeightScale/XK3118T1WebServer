#pragma once

#include <Stream.h>

class StringStream : public Stream
{
public:
	StringStream(String &s)	: _string(s), _position(0) {}
	~StringStream(){}

	// Stream methods
	virtual int available() { return _string.length() - _position; }
	virtual int read() { return _position < _string.length() ? _string[_position++] : -1; }
	virtual int peek() { return _position < _string.length() ? _string[_position] : -1; }
	virtual void flush() {}
	;
	// Print methods
	virtual size_t write(uint8_t c) { _string += (char)c; return 1;};
	unsigned int length() {return _length;};

private:
	String &_string;
	unsigned int _length;
	unsigned int _position;
};