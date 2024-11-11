// stdafx.h : ìì£¼ ì‚¬ìš©í•˜ì§€ë§Œ ìì£¼ ë³€ê²½ë˜ì§€ëŠ” ì•ŠëŠ”
// í‘œì¤€ ì‹œìŠ¤í…œ í¬í•¨ íŒŒì¼ ë° í”„ë¡œì íŠ¸ ê´€ë ¨ í¬í•¨ íŒŒì¼ì´
// ë“¤ì–´ ìˆëŠ” í¬í•¨ íŒŒì¼ì…ë‹ˆë‹¤.

#pragma once

#undef _SECURE_SCL	//Warning»èÁ¦. 
#define _SECURE_SCL 0
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <windows.h>
#include <mmSystem.h>
#include <zlib.h>
#include <map>
#include <algorithm>


//ÇöÀç ÄÚµå ÆäÀÌÁö(949)¿¡¼­ Ç¥½ÃÇÒ ¼ö ¾ø´Â ¹®ÀÚ°¡ ÆÄÀÏ¿¡ µé¾î ÀÖ½À´Ï´Ù. 
//µ¥ÀÌÅÍ°¡ ¼Õ½ÇµÇÁö ¾Ê°Ô ÇÏ·Á¸é ÇØ´ç ÆÄÀÏÀ» À¯´ÏÄÚµå Çü½ÄÀ¸·Î ÀúÀåÇÏ½Ê½Ã¿À.
#pragma warning (disable:4819)

using namespace std;