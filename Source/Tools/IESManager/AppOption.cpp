#include "stdafx.h"

#include "imcd3d9.h"

#include "imcXml.h"

#include "imc3d.h"
#include "imc3dMotion.h"

#include "imcEffect.h"
#include "imcEffectUtil.h"

#include "App.h"
#include "AppObject.h"

#include "PaneView.h"
#include "PaneEffectList.h"
#include "PaneEmitterList.h"
#include "MainFrm.h"

namespace app
{
static imc::CVector<imcd3d9::ITexture*>		s_bgImageList;
static int									s_selectedBGImageIndex;


void AddBGImage(const char* fileFullPath)
{
	imcd3d9::ITexture* pNewTexture = NULL;
	CRes* pRes = imcresmgr::Get(fileFullPath, RT_TEXTURE);
	if ( pRes != NULL ) 	
		pRes->QueryInterface((void**)&pNewTexture);

// 
// 	IDirect3DTexture9* pNewTexture = NULL;
// 	D3DXCreateTextureFromFileEx(imcd3d9::Get3DDevice(), fileFullPath, D3DX_DEFAULT, D3DX_DEFAULT,
// 								1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
// 								0x00000000, NULL, NULL, &pNewTexture);	

	s_bgImageList.AddToTail(pNewTexture);
}

void LoadBGList()
{
	TiXmlDocument xml;
	if (imcXml::Load(GetConfigXmlPath(), xml) == FALSE)
		return;

	TiXmlNode* node;
	TiXmlElement* root = xml.RootElement();

	for (node = root->FirstChild(); node; node = node->NextSibling()) 
	{		
		if (node->Type() != TiXmlNode::ELEMENT)
			continue;

		TiXmlElement* pChildElement	= node->ToElement();
		const char* childName	= pChildElement->Value();

		if (imc::StrIsSame(childName, "BGImage") == TRUE)
		{
			TiXmlNode *	pBGImageNode = NULL;
			while (pBGImageNode = pChildElement->IterateChildren(pBGImageNode))
			{
				TiXmlElement* pBGImageElement = pBGImageNode->ToElement();

				if (pBGImageNode != NULL)
				{
					char name[128];
					imcXml::GetXmlElemAttr(pBGImageElement, "Name", name);

					char path[256];
					imcXml::GetXmlElemAttr(pBGImageElement, "Path", path);

					std::string fullPath = imc::GetDataPath("", path);

					AddBGImage(fullPath.c_str());
				}
			}
		}
	}

}

void ResetBGList()
{
	for (int i = 0; i < GetBGImageCount(); ++i)
	{		
		imcd3d9::GetDevice()->DestroyTexture(s_bgImageList[i]);
	}

	s_bgImageList.RemoveAll();
}

int	GetBGImageCount()
{
	return s_bgImageList.Count();
}

imcd3d9::ITexture* GetBGImage(int index)
{
	IMC_ASSERT(index >= 0 && index < GetBGImageCount())

	return s_bgImageList[index];
}

void SelectBGImage(int index)
{
	s_selectedBGImageIndex = index;
}

int GetSelectedBGImageIndex()
{
	return s_selectedBGImageIndex;
}

imcd3d9::ITexture* GetSelectedBGImage()
{
	return GetBGImage(s_selectedBGImageIndex);
}

} 