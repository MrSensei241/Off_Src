#pragma once
#include "zlib.h"

inline bool Decompress(BYTE* pInBuffer, size_t InBufferSize, BYTE* pOutBuffer, size_t OutBufferSize)
{
	z_stream	stream;

	stream.next_in	= pInBuffer;
	stream.avail_in	= InBufferSize;

	stream.next_out = pOutBuffer;
	stream.avail_out= OutBufferSize;

	stream.total_in = 0;
	stream.total_out = 0;

	stream.zalloc	= Z_NULL;
	stream.zfree	= Z_NULL;

	int result = inflateInit2(&stream, -MAX_WBITS);
	if (result != Z_OK)
		return false;

	result = inflate(&stream, Z_FINISH);
	if (result != Z_STREAM_END) 
		return false;

	result = inflateEnd(&stream);
	if (result != Z_OK)
		return false;

	return true;
}
