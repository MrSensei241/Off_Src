#include "stdafx.h"
#include "imcStr.h"
#include "FunctionExtracter.h"
#include "io.h"
#include "imcXml.h"
#include <map>
#include <algorithm>
#include "imcTokenizer.h"

#define MAX_FILE_SIZE 131068

static char g_buf[MAX_FILE_SIZE];

static char funname[2048];
static char params[2048];
static char returns[2048];
static char Description[2048];

/** @function	GetRemainTimeInSec
* @param	CFdActor
* @return	Int
* @description  GetRemainTimeInSec(CFdActor)		// 다음 시간까지 얼마나 남앗는지 초로 구합니다.
**/
namespace imc
{
	FUNCTION_LIST	g_funclist;

	FUNCTION_LIST*	GetFuncInfoList()
	{
		return &g_funclist;
	}

	#define MAX_TOKEN_COUNT 32

	class TempTokenizer
	{
	public:
		void	Init(char * pChar)
		{
			m_Cnt = 0;
			int len = strlen(pChar);
			if (len == 0)
				return;

			*(pChar + len - 1)  = 0;

			m_token[m_Cnt] = pChar;
			m_Cnt = 1;

			char * pLast = pChar + len;
			char * token = pChar;
			bool spc = false;
			while(true)
			{
				if (token >= pLast)
					break;

				if (*(token) == '\"')
					*(token) = '\'';

				if (*(token) ==' ' || *(token) == '\t')
				{
					spc = true;
					token++;
				}
				else
				{
					if (spc)
					{
						spc = false;
						m_token[m_Cnt] = token;
						m_Cnt++;
						if (m_Cnt > MAX_TOKEN_COUNT - 1)
							return;
					} else{
						token ++;
					}
				}
			}
		}

		int Size()
		{
			return m_Cnt;
		}

		char * Get(int i)
		{
			return m_token[i];
		}

		char * GetName()
		{
			return m_token[2];
		}

		char * m_token[MAX_TOKEN_COUNT];
		int	m_Cnt;
	};

	void ExtactFunctions(const char * pFileNamePath, const char * pFileName)
	{
		imc::TempTokenizer	tokens;

			FILE* pFile = fopen(pFileNamePath, "r");
			if (NULL == pFile)
				return;

			bool tagstarted = false;
			while(fgets(g_buf, MAX_FILE_SIZE, pFile))
			{
				if (strstr(g_buf, "@function"))
				{
					tokens.Init(g_buf);

					if (tokens.Size() >= 2)
					{
						strcpy(funname, tokens.GetName());
						tagstarted = true;
						strcpy(params, "");
						strcpy(returns, "");
					}
				}
				else if (tagstarted == true)
				{
					if (strstr(g_buf, "@param"))
					{
						tokens.Init(g_buf);

						if (tokens.Size() >= 2)
						{
							strcat(params, tokens.GetName());
							strcat(params, " ");
						}

					} else if (strstr(g_buf, "@return"))
					{
						tokens.Init(g_buf);

						if (tokens.Size() >= 2)
						{
							strcat(returns, tokens.GetName());
							strcat(returns, " ");
						}
					} else if (strstr(g_buf, "@description"))
					{
						tokens.Init(g_buf);

						if (tokens.Size() >= 2)
						{
							strcpy(Description, tokens.GetName());
						}

						tagstarted = false;

						FUNCTION_INFO* info = new FUNCTION_INFO;
						info->Name = funname;
						*( params + strlen(params) -1 ) = 0;
						info->Params = params;
						*( returns + strlen(returns) -1 ) = 0;
						info->Returns = returns;
						info->Description = Description;
						info->FileName = pFileName;
						g_funclist.AddToTail(info);
					} 
				}
			}
			
			fclose(pFile);
			

			
	}

	void MakeFileHeader(FILE * outfile)
	{
		strcpy(g_buf, 
			"<html>\
			<head>\
			<style type=text/css>\
			.font { font-size:13px;  font-family:돋움체 }\
			</style>\
			</head>\
			\
			<body>\
			<table width=100% class=font border=1>\
			\n");

		fputs(g_buf, outfile);
	}

	void MakeFileTail(FILE * outfile)
	{
		sprintf(g_buf, 
			"</table>\
			</body>\
			</html>\
			\n");

		fputs(g_buf, outfile);
	}

	void MakeTableHeader(FILE* pFile)
	{
		strcpy(g_buf, 
			"		 <tr>\
			<td width=150>Function</td>\
			<td width=100>Param</td>\
			<td width=100>Return</td>\
			<td>설명</td>\
			<td>날짜</td>\
			<td>작업자</td>\
			<td>Revision</td>\
			<td>FileName</td>\
			</tr>\
			\n");

		fputs(g_buf, pFile);
	}

	void MakeTableInfo(FUNCTION_INFO* pInfo, FILE* pFile)
	{
		sprintf(g_buf, 
			"		 <tr>\
			<td width=150><font color=brown>%s</font></td>\
			<td width=100><font color=blue>%s</font></td>\
			<td width=100><font color=blue>%s</font></td>\
			<td>%s</td>\
			<td>%s</td>\
			<td>%s</td>\
			<td>%d</td>\
			<td>%s</td>\
			</tr>\
			\n"
			,
			pInfo->Name.c_str(),
			pInfo->Params.c_str(),
			pInfo->Returns.c_str(),
			pInfo->Description.c_str(),
			pInfo->LastDate.c_str(),
			pInfo->LastEditor.c_str(),
			pInfo->LastRevision,
			pInfo->FileName.c_str()
			);

		fputs(g_buf, pFile);
	}


};