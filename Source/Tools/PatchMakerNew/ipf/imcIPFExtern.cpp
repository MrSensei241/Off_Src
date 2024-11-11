#include "stdafx.h"
//#include "imcCompress.h"
#include "boost/nowide/convert.hpp"

#define SAFE_CLOSE_HANDLE(p)	{ if(p) { ::CloseHandle(p); (p)=NULL; } }


namespace imcCompress {

#define GetDWORD(buf,bit,mask) ((*(DWORD*)(((BYTE*)buf)+((bit)>>3)))>>((bit)&7)&(mask))
#define GetWORD(buf,bit,mask) ((*(WORD*)(((BYTE*)buf)+((bit)>>3)))>>((bit)&7)&(mask))

	int GetBitCount(int n)
	{
		int nBitCount = 0;
		while(n)
			n >>= 1, nBitCount++;
		return nBitCount;
	}

	int BinarySearch(void* pValue, int nVlaueSize, void* pArray, int nCount)
	{
		int nIndex, nResult, nStart = 0, nEnd = nCount-1;
		while(nStart <= nEnd)
		{
			nIndex = (nEnd+nStart)/2;
			if((nResult = memcmp((BYTE*)pArray+nIndex*nVlaueSize, pValue, nVlaueSize)) == 0)
				return nIndex;
			if(nResult > 0)
				nEnd = nIndex-1;
			else
				nStart = nIndex+1;
		}
		return -1;
	}

	bool Compress(BYTE *pSrc, int nSrcLen, BYTE *&pDes, int &nDesLen)
	{
		int nBitsPerSample = 12;
		BYTE by[] = {0x0,0xff};
		void* nRuns = by;
		int nRunCount = 2;
		int nRunSize = 1;

		pDes = (BYTE*)malloc(nSrcLen*2);
		memset(pDes, 0, nSrcLen*2);

		nDesLen = sizeof(DWORD);
		*(DWORD*)pDes = nSrcLen;							// save source length
		*(pDes+nDesLen++) = nBitsPerSample;					// save bits per sample
		*(pDes+nDesLen++) = nRunCount;						// save runs count
		*(pDes+nDesLen++) = nRunSize;						// save run bytes
		memcpy(pDes+nDesLen, nRuns, nRunCount*nRunSize);	// save runs
		nDesLen += nRunCount*nRunSize;
		nDesLen <<= 3; // bytes to bits
		if(nRunCount == 0)
			nRunCount = 256, nRunSize = 1, nRuns = NULL;

		int nBitsPerTypeIndex = GetBitCount(nRunCount-1);
		int nMaxRunLength = (1 << nBitsPerSample)-1, nRunLength, nRunIndex, nByte = 0;
		// loop in the source buffer
		while(nByte < nSrcLen)
			if((nRuns && (nRunIndex = BinarySearch(pSrc+nByte, nRunSize, nRuns, nRunCount)) != -1 &&
				memcmp(pSrc+nByte+nRunSize, (BYTE*)nRuns+nRunIndex*nRunSize, nRunSize) == 0) ||
				(!nRuns && (nRunIndex = *(pSrc+nByte)) == *(pSrc+nByte+1)))
			{	// set bit to 1 to indicate type found
				*(pDes+(nDesLen>>3)) |= 1 << (nDesLen&7);
				*(DWORD*)(pDes+(++nDesLen>>3)) |= nRunIndex << (nDesLen&7);
				nDesLen += nBitsPerTypeIndex;
				// skip the two repeated runs
				nByte += nRunSize*2;
				// get run length - 2 (without the two repeated runs)
				nRunLength = 0;
				while(nRunLength < nMaxRunLength && nByte < nSrcLen && 
					((nRuns && memcmp(pSrc+nByte, (BYTE*)nRuns+nRunIndex*nRunSize, nRunSize) == 0) || (!nRuns && (BYTE)nRunIndex == *(pSrc+nByte))))
					nRunLength++, nByte += nRunSize;
				// save run length and increment destination length by bits per sample
				*(DWORD*)(pDes+(nDesLen>>3)) |= nRunLength << (nDesLen&7);
				nDesLen += nBitsPerSample;
			}
			else	// copy one byte
				*(WORD*)(pDes+(++nDesLen>>3)) |= *(pSrc+nByte++) << (nDesLen&7), nDesLen += 8;
		nDesLen = (nDesLen+7)/8;	// bits to bytes
		pDes = (BYTE*)realloc(pDes, nDesLen);

		return true;
	}

	bool Decompress(BYTE *pSrc, int nSrcLen, BYTE *&pDes, int &nDesLen)
	{
		if(nSrcLen == 0)
			return true;

		// allocate destination buffer
		nDesLen = *(DWORD*)pSrc;
		pDes = (BYTE*)malloc(nDesLen);
		memset(pDes, 0, nDesLen);

		// copy compression information
		int nSrcIndex = sizeof(DWORD);
		int nBitsPerSample = *(pSrc+nSrcIndex++);
		int nRunCount = *(pSrc+nSrcIndex++);
		int nRunSize = *(pSrc+nSrcIndex++);
		void* nRuns = pSrc+nSrcIndex;
		nSrcIndex += nRunSize*nRunCount;
		nSrcIndex <<= 3; // bytes to bits
		if(nRunCount == 0)
			nRunCount = 256, nRunSize = 1, nRuns = NULL;

		int nBitsPerTypeIndex = GetBitCount(nRunCount-1);
		int nMaxTypeIndex = (1 << nBitsPerTypeIndex)-1;
		int nMaxRunLength = (1 << nBitsPerSample)-1;
		int nDesIndex = 0, nRunLength, nRunIndex, nRun, nByte;

		nSrcLen <<= 3; // bytes to bits
		while(nSrcIndex < nSrcLen-8)
			if((*(pSrc+(nSrcIndex>>3)) >> (nSrcIndex++&7)) & 1)
			{
				nRunIndex = GetDWORD(pSrc, nSrcIndex, nMaxTypeIndex), nSrcIndex += nBitsPerTypeIndex;
				nRunLength = GetDWORD(pSrc, nSrcIndex, nMaxRunLength)+2, nSrcIndex += nBitsPerSample;
				for(nRun = 0; nRun < nRunLength; nRun++)
					for(nByte = 0; nByte < nRunSize; nByte++, nDesIndex += 8)
						*(WORD*)(pDes+(nDesIndex>>3)) |= nRuns ? GetWORD(nRuns+nRunSize*nRunIndex, nByte<<3, 0xff) : (BYTE)nRunIndex;
			}
			else	// copy one byte
				*(WORD*)(pDes+(nDesIndex>>3)) |=  GetWORD(pSrc, nSrcIndex, 0xff), nDesIndex += 8, nSrcIndex += 8;

		return true;
	}

	// compress library
	bool ZLibCompress(BYTE* pInBuffer, size_t InBufferSize, BYTE* pOutBuffer, size_t& OutBufferSize)
	{
		z_stream stream;

		stream.next_in	= pInBuffer;
		stream.avail_in	= InBufferSize;

		stream.next_out = pOutBuffer;
		stream.avail_out= OutBufferSize;

		stream.total_in = 0;
		stream.total_out = 0;

		stream.zalloc	= Z_NULL;
		stream.zfree	= Z_NULL;


		int result = deflateInit2(&stream, 1, Z_DEFLATED, -MAX_WBITS, 9, Z_DEFAULT_STRATEGY);
		if (result != Z_OK)
			return false;

		result = deflate(&stream, Z_FINISH);
		if (result != Z_STREAM_END)
			return false;

		result = deflateEnd(&stream);
		if (result != Z_OK)
			return false;

		OutBufferSize = stream.total_out;
		return true;
	}

	bool ZLibDecompress(BYTE* pInBuffer, size_t InBufferSize, BYTE* pOutBuffer, size_t& OutBufferSize)
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

		OutBufferSize = stream.total_out;

		return true;
	}

};


namespace imcfile {

	//
	typedef std::map <std::string , bool> FILE_EXIST_MAP;
	FILE_EXIST_MAP s_fileExistMap;


bool IsExistInDsk(const char * fileName)
{
	FILE_EXIST_MAP::iterator it = s_fileExistMap.find(fileName);
	if(it != s_fileExistMap.end()) // 없으면 한번더 읽어보자
	{
		return it->second;
	}
	else
	{
		HANDLE hHandle = CreateFileW(boost::nowide::widen(fileName).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if ( hHandle == INVALID_HANDLE_VALUE ) {
			s_fileExistMap.insert(std::make_pair(fileName, false));
			FILE_EXIST_MAP::iterator it = s_fileExistMap.find(fileName);
			return false;
		}

		SAFE_CLOSE_HANDLE(hHandle);
		s_fileExistMap.insert(std::make_pair(fileName, true));
		return true;
	}	
}
}