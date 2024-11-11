#include "stdafx.h"

namespace Em2DFrame
{
	enum ErrorType
	{
		eNoProblem = 0,
		eInvalidData,
		eUnableReadFile,
		eHasPossibilityInfiniteLoop,

		ErrorTypeCount,
	};
}

static std::list<std::string> s_listFileName;
static std::list<std::string> s_vecErrorFileList[Em2DFrame::ErrorTypeCount];

Em2DFrame::ErrorType Check2DFrameFile(const char* strFilePath)
{
	Em2DFrame::ErrorType emErrorType = Em2DFrame::eNoProblem;

	imc3d::MOTION_2D_FRAME* p2DFrame = imc3d::Get2DFrameTimeTest(strFilePath, reinterpret_cast<uint32_t*>(&emErrorType));
	if (emErrorType != Em2DFrame::eNoProblem)
		return emErrorType;

	imc3d::MOTION_2D motion2D;
	motion2D.Reset();

	motion2D.m_maxFrame = p2DFrame->size();
	motion2D.m_playSpeed = 1.0;
	motion2D.m_animPlaySpeed = 1.0;

	int nLoopCount = 5;
	float fMaxTime = 30.f * motion2D.m_maxFrame * nLoopCount;

	for (uint32_t j = 0; j < 30; ++j)
	{
		motion2D.Reset();

		float fTime = 0.f;
		while (fTime >= fMaxTime)
		{
			float fElapsedTime = 1.f / (10 * j);

			bool isError = false;
			motion2D.Update(p2DFrame, fElapsedTime, &isError);

			if (isError == true)
				return Em2DFrame::eHasPossibilityInfiniteLoop;

			fTime += fElapsedTime;
		}
	}

	return Em2DFrame::eNoProblem;
}

void SearchDirectory(const char* strSearchPath)
{
	std::string strFullPath = strSearchPath;
	strFullPath.append("\\*.*");

	WIN32_FIND_DATA findData;
	HANDLE hDirInfo = ::FindFirstFile(strFullPath.c_str(), &findData);

	if (hDirInfo != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string strChildPath = strSearchPath;
			strChildPath.append("\\");
			strChildPath.append(findData.cFileName);

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (strcmp(findData.cFileName, ".") && strcmp(findData.cFileName, ".."))
				{
					SearchDirectory(strChildPath.c_str());
				}
			}
			else
			{
				char drive[_MAX_DRIVE];
				char dir[_MAX_DIR];
				char fname[_MAX_FNAME];
				char ext[_MAX_EXT];
				::_splitpath_s(strChildPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

				if (imc::StrIsSame(ext, ".xsmtime") == true)
				{
					s_listFileName.emplace_back(strChildPath.c_str());
				}
			}
		} while (FindNextFile(hDirInfo, &findData));
	}

	FindClose(hDirInfo);
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		std::cout << "경로 탐색 중입니다..." << std::endl << std::endl;

		for (int i = 1; i < argc; ++i)
		{
			WIN32_FIND_DATA findData;
			HANDLE hDirInfo = ::FindFirstFile(argv[i], &findData);

			if (hDirInfo != INVALID_HANDLE_VALUE)
			{
				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					SearchDirectory(argv[i]);
				}
				else
				{
					char drive[_MAX_DRIVE];
					char dir[_MAX_DIR];
					char fname[_MAX_FNAME];
					char ext[_MAX_EXT];
					::_splitpath_s(argv[i], drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

					if (imc::StrIsSame(ext, ".xsmtime") == true)
					{
						s_listFileName.emplace_back(argv[i]);
					}
				}
			}

			FindClose(hDirInfo);
		}

		if (s_listFileName.empty() == true)
		{
			std::cout << "검사할 파일이 존재하지 않습니다." << std::endl;
		}
		else
		{
			std::cout << "검사 대상 수 : " << s_listFileName.size() << std::endl;
			std::cout << "검사 시작" << std::endl;

			for (auto& strFileName : s_listFileName)
			{
				Em2DFrame::ErrorType emErrorType = Check2DFrameFile(strFileName.c_str());
				s_vecErrorFileList[emErrorType].emplace_back(strFileName.c_str());
			}

			std::cout << "검사 종료" << std::endl << std::endl;

			std::cout << "결과 : 총 " << s_listFileName.size() << " 개 중" << std::endl;
			std::cout << "정상 : " << s_vecErrorFileList[Em2DFrame::eNoProblem].size() << std::endl;
			std::cout << "비정상 : " << s_vecErrorFileList[Em2DFrame::eInvalidData].size() << std::endl;
			std::cout << "읽기 실패 : " << s_vecErrorFileList[Em2DFrame::eUnableReadFile].size() << std::endl;
			std::cout << "무한루프 가능성 : " << s_vecErrorFileList[Em2DFrame::eHasPossibilityInfiniteLoop].size() << std::endl << std::endl;

			char strPath[MAX_PATH];
			::GetModuleFileName(nullptr, strPath, MAX_PATH);

			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];
			::_splitpath_s(strPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

			time_t now = time(0); //현재 시간을 time_t 타입으로 저장
			tm tstruct;
			localtime_s(&tstruct, &now);

			char buf[80];
			strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);

			std::string strResultPath = drive;
			strResultPath.append(dir);
			strResultPath.append("Error2DFrameList_");
			strResultPath.append(buf);
			strResultPath.append(".xml");

			std::cout << "테스트 결과 출력 및 XML 파일로 저장 중..." << std::endl;
			std::cout << "경로 : " << strResultPath.c_str() << std::endl << std::endl;
			Sleep(500);

			imcXml::CXmlDoc doc;
			imcXml::InitDocument(doc);

			imcXml::CXmlNode root = doc.NewElement("Error2DFrameList");
			doc.InsertEndChild(root);

			std::string strCategoryArr[] =
			{
				"NoProblem",
				"InvalidData",
				"UnableReadFile",
				"HasPossibilityInfiniteLoop",
			};

			for (uint32_t i = Em2DFrame::eInvalidData; i < Em2DFrame::ErrorTypeCount; ++i)
			{
				std::cout << "-------------------------------------------------" << std::endl;
				std::cout << "ErrorType : " << strCategoryArr[i] << std::endl;

				if (s_vecErrorFileList[i].empty() == true)
				{
					std::cout << "None" << std::endl;
				}
				else
				{
					uint32_t nIdx = 0;
					for (auto& strFileName : s_vecErrorFileList[i])
					{
						std::cout << ++nIdx << ". " << strFileName.c_str() << std::endl;

						imcXml::CXmlNode childNode = imcXml::CreateChild(root, strCategoryArr[i].c_str());
						childNode->SetAttribute("FileName", strFileName.c_str());
					}
				}

				std::cout << std::endl;
			}
			std::cout << "-------------------------------------------------" << std::endl << std::endl;

			if (doc.SaveFile(strResultPath.c_str()) == true)
			{
				std::cout << "XML 저장 완료" << std::endl << std::endl;
			}
			else
			{
				std::cout << "XML 저장 실패" << std::endl << std::endl;
			}

			std::cout << "데이터에 이상이 있을 경우, 그래픽팀에 확인 요청해주세요." << std::endl << std::endl;
		}
	}
	else
	{
		std::cout << "검사하려는 파일(*.xsmtime) 또는 폴더를 프로그램에 드래그&드랍 해주세요" << std::endl << std::endl;
	}

	system("pause");

	return 0;
}