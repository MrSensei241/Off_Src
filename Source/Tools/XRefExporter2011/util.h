#ifndef __UTIL___H___
#define __UTIL___H___

namespace util
{
	const char* GetFilenameFromFullPath(const char* pFullPath)
	{
		const char* pBegin = pFullPath;
		const char* pIter = pFullPath;

		while (*pIter != NULL)
			++pIter;

		pIter -= 2;
		if (pIter < pBegin)
			return pFullPath;

		do
		{
			if (pIter < pBegin)
				return pFullPath;

			if (*pIter == '\\')
				return ++pIter;

			if (pIter == pBegin)
				return pIter;

			--pIter;
		}
		while (1);
	}
};

#endif