#include "StdAfx.h"

#include "BaseDic.h"
#include "imcStr.h"
#include "imcDirMgr.h"
#include "LuaConvertor.h"
#include "boost/regex.hpp"
#include "boost/nowide/convert.hpp"
#include "boost/bind.hpp"


CLuaConvertor::CLuaConvertor(void)
	: m_bIsNeedBuild(false)
{
	newdatas.clear();
}


CLuaConvertor::~CLuaConvertor(void)
{

}

static bool IsHangul(const std::string& data) // ÀÌ°Å Á» ÀÌ»óÇÑ°¡?
{
	std::wstring widestr = boost::nowide::widen(data);
	boost::wregex re;
	boost::wsmatch whaat;
	std::wstring sre(L"[°¡-ÆR]");

	re.assign(sre, boost::regex_constants::icase);

	if (boost::regex_search(widestr, whaat, re))
		return true;
	return false;
}

bool Match(const boost::match_results<std::string::const_iterator>& match, std::vector<std::string>* pList)
{
	pList->push_back(match.str());
	return true;
}


std::string CLuaConvertor::ConvertSimpleHangul(std::string original_string, bool* m_bIsNeedBuild)
{

	std::string nowstring(original_string.c_str());

	boost::regex re;
	boost::smatch matchstrings;
	//std::string filter("\"[^\"]*[°¡-ÆR][^\"]*\"|\'[^\']*[°¡-ÆR][^\']*\'"); // µû¿ÈÇ¥¿Í Å«µû¿ÈÇ¥ »çÀÌÀÇ(Quoting Sentence) ÇÑ±ÛÀ» Ã£´Â Á¤±Ô½Ä ?? À¯´ÏÄÚµå µµ µÊ?

	//std::string filter("\"[^\"]*[°¡-ÆR][^\"]*\""); //mode 1
	std::string filter("\'[^\']*[°¡-ÆR][^\']*\'"); //mode 2


	re.assign(filter, boost::regex_constants::icase);

// 	if (nowstring == " ")
// 		return "";

	while (true)
	{
		//LOG_LINE("%s", nowstring.c_str());
		if (boost::regex_search(nowstring, matchstrings, re))
		{

			*m_bIsNeedBuild = true;
	
			for (unsigned int i = 0; i < matchstrings.size(); i++)
			{
				std::string temp = matchstrings[i];
				std::string oristr = temp;

				// °Ë»ö¿¡ ÇÊ¿äÇÑ º¯¼öµé ÃÊ±âÈ­.
				int cnt = 1;
				std::string::size_type findOffset2 = 0;

				while (true)
				{
					// °Ë»ö.
					findOffset2 = temp.find("%s", findOffset2);

					// Ã£Áö ¸øÇßÀ» ¶§.
					if (std::string::npos == findOffset2)
					{
						break;
					}
					else
					{
						temp.replace(findOffset2, 2, "%d");
					}
				}

				findOffset2 = 0;

				while (true)
				{
					// °Ë»ö.
					findOffset2 = temp.find("%d", findOffset2);

					// Ã£Áö ¸øÇßÀ» ¶§.
					if (std::string::npos == findOffset2)
					{
						break;
					}
					else
					{
						char text[MAX_PATH] = { NULL };
						imc::StrFormat(text, "{Auto_%d}", cnt);
						temp.replace(findOffset2, 2, text);
						cnt++;
					}
				}




				std::string tempcopy = temp;

				std::wstring widetemp;
				std::wstring engname_wide;
				imc::MultibyteToWide(temp, &widetemp);

				wchar_t buffer[4096];
				engname_wide = imc::MakeEngName(widetemp.c_str(), buffer, sizeof buffer);

				std::string engname_mb;
				imc::WideToMultibyte(engname_wide, &engname_mb);

// 				auto°¡ ÀÖ´Â°¡
// 				std::string::size_type offset = 0;
// 				int count = 0;
// 
// 				offset = engname_mb.find("{Auto_", offset);
// 
// 				LOG_LINE("%d", offset);
// 
// 				if (!(std::string::npos == offset))
// 					count++;




				if (cnt < 2)
				{
					std::string doubleorsingle = engname_mb.substr(0, 1);
// 					if (doubleorsingle == "\"")
// 						engname_mb = "\"Auto_" + engname_mb;
// 					else if (doubleorsingle == "\'")
// 						engname_mb = "\'Auto_" + engname_mb;
// 					else
// 						LOG_LINE("???");

					//engname_mb = "\"Auto_" + engname_mb; //mode1
					engname_mb = "\'Auto_" + engname_mb; //mode2
				}else{
					std::string doubleorsingle = engname_mb.substr(0, 1);
// 					if (doubleorsingle == "\"")
// 						engname_mb = "\"Buto_" + engname_mb;
// 					else if (doubleorsingle == "\'")
// 						engname_mb = "\'Buto_" + engname_mb;
// 					else
// 						LOG_LINE("???");

					//engname_mb = "\"Buto_" + engname_mb;//mode1
					engname_mb = "\'Buto_" + engname_mb; //mode2
				}

				engname_mb.replace(6, 1, "");

				std::string luastring = "ScpArgMsg(" + engname_mb + ")";


				//xml ÀúÀå¿ë
				AUTO_ADDED_DATA newdata;
				std::string testtest = engname_mb.substr(1, engname_mb.length() - 2);

				bool flag = false;
				for (unsigned int i = 0; i < newdatas.size();i++)
				{
					std::string nowstr = newdatas[i].className;
					if (nowstr == testtest)
					{
						flag = true;
					}
				}

				if (flag == false)
				{
					newdata.className = testtest;
					testtest = tempcopy.substr(1, tempcopy.length() - 2);
					newdata.data = testtest;
					newdatas.push_back(newdata);
				}
				

				
				int index = nowstring.find(oristr, 0);

				std::string convertresult = nowstring.replace(index, oristr.length(), luastring);

				
				nowstring = convertresult;
		
			}
		}else{
			break;
		}

	}

	return nowstring;
}

void _GetMaxAttribute(imcXml::CXmlNode elem, const char * attrName, int& maxValue)
{
	for (imcXml::CXmlNode it = elem->FirstChild(); it; it = it->NextSiblingElement())
	{
		if (!it->ToElement())
			continue;

		const char * attrValue = it->ToElement()->Attribute(attrName);
		if (attrValue)
		{
			int intValue = atoi(attrValue);
			maxValue = imc::Max(intValue, maxValue);
		}
		
		_GetMaxAttribute(it->ToElement(), attrName, maxValue);
	}
}

bool IsDuplicateInXml(imcXml::CXmlNode elem, const char * ClassName)
{
	bool revalue = false;

	for (imcXml::CXmlNode it = elem->FirstChild(); it; it = it->NextSiblingElement())
	{
		if (!it->ToElement())
			continue;

		const char * nowValue = it->ToElement()->Attribute("ClassName");

		if (nowValue)
		{
			if (strcmp(ClassName, nowValue) == 0)
			{
				return true;
			}
		}

		revalue = revalue || IsDuplicateInXml(it->ToElement(), ClassName);
	}

	return revalue;
}

int CLuaConvertor::GetMaxAttribute(imcXml::CXmlNode elem, const char * attrName)
{
	int maxValue = 1;
	_GetMaxAttribute(elem, attrName, maxValue);
	return maxValue;
}

void CLuaConvertor::AddClientMessage(imcXml::CXmlDoc& s_messageDoc, const char * category, const char * className, const char * data)
{
	// geXml::InsertNewClass()
// 	if (!s_messageDocInited)
// 	{
// 		imcXml::Load(gepath::GetPath(gepath::DIRTYPE_DATABASE, "xml_lang\\", "clientmessage.xml").c_str(), s_messageDoc);
// 		s_messageDocInited = true;
// 	}

	imcXml::CXmlNode categoryElement = imcXml::GetChildElem(s_messageDoc.RootElement(), "Category", "Name", category);
	if (categoryElement)
	{
		int newClassID = GetMaxAttribute(s_messageDoc.RootElement(), "ClassID") + 1;
		bool dupulci = IsDuplicateInXml(s_messageDoc.RootElement(), className);

		if (dupulci == false)
		{
			imcXml::CXmlNode cls = s_messageDoc.NewElement("Class");
			categoryElement->InsertEndChild(cls);
			cls->SetAttribute("ClassID", newClassID);
			cls->SetAttribute("ClassName", className);
			cls->SetAttribute("Data", data);
		}
	}
}

void CLuaConvertor::SaveClientMessage(imcXml::CXmlDoc& s_messageDoc)
{
	std::string datapath = imc::GetDataPath("", "");
	datapath += "xml_lang\\clientmessage.xml";

	imcXml::ToUTF8Resursively(s_messageDoc.RootElement());
	s_messageDoc.SaveFile(datapath.c_str());
}

void CLuaConvertor::SaveCliMsg()
{
	//imcXml::CXmlDoc s_messageDoc2;
	imcXml::CXmlDoc s_messageDoc2;

 	std::string datapath = imc::GetDataPath("","");
 	datapath += "xml_lang\\clientmessage.xml";

	imcXml::Load(datapath.c_str(), s_messageDoc2);

	for (unsigned int i = 0; i < newdatas.size(); i++)
	{
		
		AddClientMessage(s_messageDoc2, "AutoAdded", newdatas[i].className.c_str(), newdatas[i].data.c_str());
	}

	SaveClientMessage(s_messageDoc2);
}

std::string CLuaConvertor::StartConvert( const std::string& fullfileName )
{
	m_parser.Reset();
	m_exporter.Reset();
	m_bIsNeedBuild = false;

	m_parser.Init(fullfileName);

	imcDic::FIND_DIC_INFO dicInfo;
	std::string datafile(fullfileName);

	dicInfo.dataFile	= datafile;
	dicInfo.mode		= imcDic::DM_MAKE_DIC;
	m_pDic = imcDic::FindDic(dicInfo);

	int index = 0;
	while( m_parser.IsEnd() == false ) 
	{
		std::string senetence = m_parser.GetGlyph();
		senetence = ConvertSimpleHangul(senetence, &m_bIsNeedBuild);
		m_exporter.AddSentence(senetence);
		m_exporter.AddReturn();
	}

	if ( m_bIsNeedBuild ) {
		std::string fileName = fullfileName;
		fileName.erase(fileName.size()-3, fileName.size());
		fileName += "lua";
		LOG_LINE("convert : %s", fileName);
		m_exporter.SaveAS(fileName);
		return fileName;
	}
	return "";
}

bool CLuaConvertor::IsDialogSentnce( const std::string& sentence )
{
	return true;
	enum {
		NUM_DIALOG_FUNCTION = 3,
	};
	static const std::string DLG_FUNCTION[NUM_DIALOG_FUNCTION] = 
	{ "Latent_ShowOkDlg", 
	  "Latent_ShowNextDlg", 
	  "Latent_ShowSelDlg" 
	};

	for ( unsigned int index = 0 ; index < NUM_DIALOG_FUNCTION ; ++index ) {
		if ( sentence.find(DLG_FUNCTION[index]) != std::string::npos ) 
			return true;
	}
	return false;
}

// static bool IsHangul(const std::string& data)
// {
// 	std::wstring u16_str = imc::UTF8ToUTF16(data);
// 	LOG_LINE("%s %d / %s %d", data, data.size(), u16_str, u16_str.size());
// 	if ( data.size() != u16_str.size() )
// 		return true;
// 	return false;
// }



std::string CLuaConvertor::ConvertSentence( std::string sentence, std::string::size_type findOffSet )
{
	std::string::size_type Ã¹µû¿ÈÇ¥ = sentence.find("'", findOffSet);
	if ( Ã¹µû¿ÈÇ¥ != std::string::npos )
	{
		if ( Ã¹µû¿ÈÇ¥ != std::string::npos ) {
			std::string::size_type ³¡µû¿ÈÇ¥ = sentence.find("'", Ã¹µû¿ÈÇ¥+1);
			if ( ³¡µû¿ÈÇ¥ == std::string::npos ) {
				return ConvertSentence(sentence, Ã¹µû¿ÈÇ¥ + 1);
			}
			sentence = ConvertDicID(sentence, Ã¹µû¿ÈÇ¥, ³¡µû¿ÈÇ¥, '\'');
			sentence = ConvertSentence(sentence, ³¡µû¿ÈÇ¥+1);
		}
	}

	std::string::size_type Ã¹½Öµû¿ÈÇ¥ = sentence.find("\"", findOffSet);
	if ( Ã¹½Öµû¿ÈÇ¥ != std::string::npos ) 
	{
		if ( Ã¹½Öµû¿ÈÇ¥ != std::string::npos ) {
			std::string::size_type ³¡½Öµû¿ÈÇ¥ = sentence.find("\"", Ã¹½Öµû¿ÈÇ¥+1);
			if ( ³¡½Öµû¿ÈÇ¥ == std::string::npos ) {
				return ConvertSentence(sentence, Ã¹½Öµû¿ÈÇ¥ + 1);
			}
			sentence = ConvertDicID(sentence, Ã¹½Öµû¿ÈÇ¥, ³¡½Öµû¿ÈÇ¥, '\"');
			sentence = ConvertSentence(sentence, ³¡½Öµû¿ÈÇ¥+1);
		} 
	}

	return sentence;
}

std::string CLuaConvertor::ConvertDicID( std::string sentence, std::string::size_type start, std::string::size_type& end, char findChar )
{
	std::string todic_data;
	todic_data.append(sentence.begin() + start + 1, sentence.begin() + end);
	
	if ( todic_data.empty() ) 
		return sentence;

	if ( IsHangul(todic_data) ) 
	{
		m_bIsNeedBuild = true;

		bool isExist = false;
		const KOR_BASE_DATA& korData = GetBaseDic()->FindDicData(todic_data, &isExist);
		imcDic::SENTENCE_ID dic_id;
		if ( isExist ) {
			dic_id = korData.ID;
		} else {
			//dic_id = GetBaseDic()->GetNextID();
		}
		
		m_pDic->AddSentence(todic_data, dic_id,"xxx","yyy");
		char text[MAX_PATH] = { NULL };
		imc::StrFormat(text, "@dicID_^*$%d$*^", dic_id);
		std::wstring u16_data;
		imc::MultibyteToWide(text, &u16_data);
		std::string u8Text = imc::UTF16ToUTF8(u16_data);

		sentence.erase(start+1, end - start -1);
		sentence.insert(start+1, u8Text.c_str());

 		std::string::size_type findXML = sentence.find(".xml", start + 1);
		end = sentence.find(findChar, findXML);
	}
	return sentence;
}

