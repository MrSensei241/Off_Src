#include "stdAfx.h"

#include "imcd3d9.h"
#include "imcFont.h"
#include "imcUIRender.h"
#include "imcSkin.h"
#include "imcUI.h"
#include "imcUIControls.h"

#include "Script.h"
#include "MsgHandlerUsr.h"

#include "Script.h"

#include "app.h"

void app::CreateTestFrame_EditBox()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "ButtonFrame", -100, 100, 400, 200, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->SetTitleName("ButtonFrame");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);

		ui::CreateObject(ui::CLSID_EDITBOX, "EditBox1", 10, 10, 350, 10, pFrame);
		ui::CreateObject(ui::CLSID_EDITBOX, "EditBox2", 10, 30, 350, 20, pFrame);
		ui::CreateObject(ui::CLSID_EDITBOX, "EditBox3", 10, 60, 350, 30, pFrame);
		ui::CreateObject(ui::CLSID_EDITBOX, "EditBox4", 10, 100, 350, 40, pFrame);

		pFrame->ShowWindow(TRUE);
	}
}

void app::CreateTestFrame_Button()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "ButtonFrame", -100, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->SetTitleName("ButtonFrame");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);

		ui::IObject* pButton = ui::CreateObject(ui::CLSID_BUTTON, "TextButton", 10, 10, 100, 40, pFrame);		
		pButton->SetTextAlign(ui::CENTER_HORZ, ui::CENTER_VERT);
		pButton->SetText("{s16}{#FFAAAA}버튼 테스트");

		pButton = ui::CreateObject(ui::CLSID_BUTTON, "ImageButton", 10, 80, 100, 40, pFrame);		
		pButton->SetTextAlign(ui::LEFT, ui::CENTER_VERT);
		pButton->SetText("{s16}{#AAFFAA}버튼 테스트");

		ui::Get_IButton(pButton)->SetImage("icon_mus_anklecut");

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_RadioButton()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "ButtonFrame", -100, 100, 400, 100, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->SetTitleName("ButtonFrame");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);

		ui::IObject* pButton = ui::CreateObject(ui::CLSID_RADIOBUTTON, "TextButton", 10, 10, 350, 80, pFrame);
		pButton->SetText("{s24}{img icon_mus_anklecut 32 32}{ol}라디오{/} 버튼{/}테스트");		

		pFrame->ShowWindow(TRUE);
	}
}

void app::CreateTestFrame_List()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "ListFrame", 0, 100, 250, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::CENTER_HORZ, ui::TOP);		
		ui::Get_IFrame(pFrame)->SetTitleName("List Test");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);

		ui::IObject* pListBox = ui::CreateObject(ui::CLSID_LISTBOX, "List", 0, 0, 210, 250, pFrame);		

		for (int i = 0; i < 100; ++i)
		{
			char itemID[256];
			char itemCaption[256];

			int randIndex = rand()%10000;
			imc::StrFormat(itemID, "item%d", i);
			imc::StrFormat(itemCaption, "{#FFAAFF}%d{/} 번 아이템", i + randIndex);

			ui::Get_IListBox(pListBox)->AddItem(itemCaption);
		}

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_DropList()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "ListFrame", -100, 100, 250, 100, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);		
		ui::Get_IFrame(pFrame)->SetTitleName("{ds}DropList Test");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);

		ui::IObject* pDropList = ui::CreateObject(ui::CLSID_DROPLIST, "DropList",00, 10, 80, 30, pFrame);
		pDropList->SetGravity(ui::CENTER_HORZ, ui::TOP);

		pDropList->SetTextAlign(ui::CENTER_HORZ, ui::CENTER_VERT);

		ui::Get_IDropList(pDropList)->SetVisibleLine(3);

		for (int i = 0; i < 10; ++i)
		{
			char itemID[256];
			char itemCaption[256];

			int randIndex = rand()%10000;
			imc::StrFormat(itemID, "item%d", i);
			imc::StrFormat(itemCaption, "{ds}{#FFAAFF}%d{/} 번 아이템", i + randIndex);

			ui::Get_IDropList(pDropList)->AddItem(itemID, itemCaption);
		}

		pFrame->ShowWindow(TRUE);
	}
}

void app::CreateTestFrame_SlideBar()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "SlideBarFrame", -100, 100, 400, 200, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);

		ui::IObject* pObject = ui::CreateObject(ui::CLSID_SLIDEBAR, "SlideBar", 5, 30, 390, 30, pFrame);		
		ui::Get_ISlideBar(pObject)->SetMaxSlideLevel(1000);
		ui::Get_ISlideBar(pObject)->SetMinSlideLevel(200);
		
		pObject = ui::CreateObject(ui::CLSID_RICHTEXT, "RichText", 5, 0, 390, 30, pFrame);

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_Picture()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "pictureFrame", 0, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::CENTER_HORZ, ui::TOP);
		ui::Get_IFrame(pFrame)->ShowFrame(TRUE);
		ui::Get_IFrame(pFrame)->ShowTitleBar(FALSE);		
		ui::Get_IFrame(pFrame)->SetBlend(70);
		ui::Get_IFrame(pFrame)->SetMoveInTitleBar(FALSE);
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		
		ui::IObject* pPic = ui::CreateObject(ui::CLSID_PICTURE, "Picture", 0, 0, 360, 260, pFrame);		
		pPic->EnableHitTest(FALSE);
		ui::Get_IPicture(pPic)->SetImage("icon_com_gatherplant");
		ui::Get_IPicture(pPic)->SetEnableStretch(TRUE);		

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_StaticText()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "statictextFrame", -100, 100, 400, 300, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->SetTitleName("Text");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);

		ui::IObject* pStaticTextObj = ui::CreateObject(ui::CLSID_STATICTEXT, "StaticText", 5, 5, 190, 30, pFrame);
		std::string textSample = "{S30}오{ol}탭프레임{/}!!{nl}";

		textSample += "오 {a ScriptTest A 1} 링크1{/}{a ScriptTest B 2}a{spc120}링크2 시작 {nl}";
		textSample += "오 {img icon_Stn_PhmHand1 20 50} 이미지 태그! {nl} {/}";

		pStaticTextObj->SetText(textSample.c_str());		

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_RichText()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "RichtextFrame", 0, 0, 450, 200, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::CENTER_HORZ, ui::CENTER_VERT);				
		ui::Get_IFrame(pFrame)->SetTitleName("Text");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableReduce(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);

		std::string textSample;

		ui::IObject* pRichTextObj2 = ui::CreateObject(ui::CLSID_RICHTEXT, "RichText2", 20, 5, 120, 30, pFrame);		
		pRichTextObj2->SetGravity(ui::RIGHT, ui::TOP);
		//pRichTextObj2->SetTextAlign(ui::AH_CENTER, ui::AV_MIDDLE);

		//ui::Get_IRichText(pRichTextObj2)->EnableUpdateParent(TRUE);
		//ui::Get_IRichText(pRichTextObj2)->EnableResizeByText(TRUE);
		pRichTextObj2->SetTextFixWidth(FALSE);

		//textSample = "{s18} 이것은 긴 텍스트가 짤릴때 나타나는 증상을 테스트하기 위한 것 입니다.{nl}";
		//textSample = "짧은 텍스트";
		textSample = "Font Variation Test --- 1234 ( !@#$ )";

		pRichTextObj2->SetText(textSample.c_str());

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_Tree()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "TreeFrame", -100, 100, 400, 300, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);		
		ui::Get_IFrame(pFrame)->SetTitleName("Tree");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);

		ui::IObject* pTreeObj = ui::CreateObject(ui::CLSID_TREECONTROL, "Tree", 5, 5, 180, 180, pFrame);		
		
		ui::TREEITEM parentItem = ui::Get_ITreeControl(pTreeObj)->Add("이거슨 너무 긴 텍스트가 들어간 경우");
		ui::TREEITEM childItem1 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우1");
		ui::TREEITEM childItem2 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우2");

		parentItem = ui::Get_ITreeControl(pTreeObj)->Add("이거슨 너무 긴 텍스트가 들어간 경우");
		childItem1 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우1");
		childItem2 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우2");

		parentItem = ui::Get_ITreeControl(pTreeObj)->Add("이거슨 너무 긴 텍스트가 들어간 경우");
		childItem1 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우");
		childItem2 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우");
		childItem2 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우");
		childItem2 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우");
						ui::Get_ITreeControl(pTreeObj)->Add(childItem2, "이것도 너무 긴 텍스트가 들어간 경우");
						ui::Get_ITreeControl(pTreeObj)->Add(childItem2, "이것도 너무 긴 텍스트가 들어간 경우");
		childItem2 = ui::Get_ITreeControl(pTreeObj)->Add(parentItem, "이것도 너무 긴 텍스트가 들어간 경우");
	
		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_TextView()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "TextViewFrame", -100, 100, 400, 300, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);		
		ui::Get_IFrame(pFrame)->SetTitleName("TextView");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);

		ui::IObject* pObj = ui::CreateObject(ui::CLSID_TEXTVIEW, "TextView", 5, 5, 180, 180, pFrame);

		for (int i = 0; i < 10; ++i)
		{
			std::string colorText = "{#";
			std::string text = "이거슨 너무 긴 텍스트가 들어간 경우";
			
			for(int colorIndex = 0; colorIndex < 6; ++colorIndex)
			{
				int randomColorValue = rand()%15;				
				char letter = '0';

				if (randomColorValue > 9)
					letter = 'a' + randomColorValue - 9;
				else
					letter += randomColorValue;

				colorText += letter;
			}

			colorText += "}";
			text = colorText + text;

			ui::Get_ITextView(pObj)->AddText(text.c_str());	
		}

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_GroupBox()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "GroupBoxFrame", -100, 100, 200, 300, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);

		ui::Get_IFrame(pFrame)->SetTitleName("GroupBox");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		//ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);
		ui::Get_IFrame(pFrame)->ShowTitleBar(FALSE);
		ui::Get_IFrame(pFrame)->ShowNoTitleBar(TRUE);

		ui::IObject* pGroupObj = ui::CreateObject(ui::CLSID_GROUPBOX, "GroupBox", 0, 0, 160, 230, pFrame);

		ui::IObject* pTextObj = ui::CreateObject(ui::CLSID_RICHTEXT, "Text", 0, 5, 160, 40, pGroupObj);

		pTextObj->SetTextAlign(ui::CENTER_HORZ, ui::TOP);		
		pTextObj->SetTextFixWidth(TRUE);
		pTextObj->SetText("길드명이 너무 길어서 바위뒤에 숨어도 길드명이 보이는 길드 !!!");

		ui::Get_IGroupBox(pGroupObj)->UpdateData();

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_Tab()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "TabFrame", -100, 100, 200, 300, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->SetTitleName("Tab");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);

		ui::IObject* pTabObj = ui::CreateObject(ui::CLSID_TAB, "Tab", 5, 5, 180, 40, pFrame);

		ui::Get_ITabControl(pTabObj)->SetItemFixWidth(FALSE);
		ui::Get_ITabControl(pTabObj)->AddItem("{img icon_Stn_PhmHand1 20 20}tab{#FFFF00}1{s20}23{/}211");
		ui::Get_ITabControl(pTabObj)->AddItem("{img icon_Stn_PhmHand1 20 20}tab2");

		pFrame->ShowWindow(TRUE);
	}
}

void app::CreateTestFrame_TextFrame()
{
	RECT rect = {0, 200, 300, 200};
	ui::IObject* pTextFrame = ui::AddTextFrame("MsgFrame1", rect, ui::CENTER_HORZ);	

	ui::Get_ITextFrame(pTextFrame)->SetTextFont(14, 0xFFFFFFFF);
	ui::Get_ITextFrame(pTextFrame)->SetTextHorzAlign(ui::CENTER_HORZ);
	ui::Get_ITextFrame(pTextFrame)->SetTextLifeTime(2.0f);
	ui::Get_ITextFrame(pTextFrame)->SetTextBlendTime(1.0f);
	
	ui::AddText("MsgFrame1", "{#FF0000} 빨강색 {/}{s20}{#00FF00} 녹색 {/}{/}");	
}

void app::CreateTestFrame_ContextMenuFrame()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "ContextMenuFrame", -100, 100, 200, 300, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);		
		ui::Get_IFrame(pFrame)->SetTitleName("ContextMenuFrame");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);

		ui::IObject* pPicObj = ui::CreateObject(ui::CLSID_PICTURE, "pic", 5, 5, 128, 128, pFrame);
		ui::Get_IPicture(pPicObj)->SetImage("icon_Stn_PhmHand1");
		pPicObj->SetLBtnDownScp("Test_ContextMenuFrame");

		pFrame->ShowWindow(TRUE);
	}
}

void app::CreateTestFrame_MessageBoxFrame()
{	
	ui::MsgBox("메세지 박스 테스트");
}

void app::CreateTestFrame_Slot()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "SlotFrame", -100, 100, 200, 300, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);		
		ui::Get_IFrame(pFrame)->SetTitleName("Slot");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);

		ui::IObject* pSlotObj = ui::CreateObject(ui::CLSID_SLOT, "Slot", 10, 10, 64, 64, pFrame);

// 		ui::IObject* pNewIcon = ui::CreateIcon("icon_Stn_PhmHand1", NULL, 0, 0);
// 		ui::Get_ISlot(pSlotObj)->SetIcon(pNewIcon);

		pFrame->ShowWindow(TRUE);
	}
}

// NOTE : 컨트롤세트와 커스텀리스트에 대한 테스트

void app::CreateTestFrame_ControlSet()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "ControlSetFrame", 0, 100, 300, 600, NULL);

	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::CENTER_HORZ, ui::TOP);
		ui::Get_IFrame(pFrame)->SetTitleName("ControlSet");	
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);

		ui::IObject* pGroupObj = ui::CreateObject(ui::CLSID_GROUPBOX, "GroupBox", 5, 5, 290, 580, pFrame);

		ui::IControlSetInfo* pInfo = ui::GetControlSetInfo("itemset_Type01");
		if (pInfo != NULL)
		{
			ui::IObject* pPrevItemSet = NULL;

			for (int i = 0; i < 10; ++i)
			{
				char name[36];
				imc::StrFormat(name, "itemSet%d", i);
				int offsetY = 5;				

				ui::IObject* pItemSet = pGroupObj->CreateControlSet("itemset_Type01", name, 5, offsetY);
				
				if (pItemSet != NULL)
				{
					if (pPrevItemSet != NULL)
						pItemSet->SetSnap(pPrevItemSet, ui::AS_BOTTOM, ui::AS_NONE);
					else
						pItemSet->SetGravity(ui::LEFT, ui::TOP);

					std::string iconName;

					switch(rand()%6)
					{
					case 0:	iconName = "icon_wpn_swd_sword1"; break;
					case 1:	iconName = "icon_wpn_sab_sabre1"; break;
					case 2:	iconName = "icon_wpn_spr_spear1"; break;
					case 3:	iconName = "icon_wpn_bul_buljin1"; break;
					case 4:	iconName = "icon_part_mag_part1"; break;
					case 5:	iconName = "icon_part_fgt_part1"; break;
					}

					pItemSet->SetTextByKey("slot", iconName.c_str());
					pItemSet->SetTextByKey("ItemName", name);

					ui::Get_IControlSet(pItemSet)->SetEnableSelect(TRUE);
					ui::Get_IControlSet(pItemSet)->SetSelectGroupName("itemgroup_1");				

					pPrevItemSet = pItemSet;
				}
			}			
		}
		
		ui::Get_IGroupBox(pGroupObj)->UpdateData();

		pFrame->ShowWindow(TRUE);
	}
}

// NOTE : 게이지 테스트
void app::CreateTestFrame_Gauge()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "gaugeFrame", -100, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->ShowWindow(TRUE);
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->SetTitleName("게이지 테스트");

		ui::IObject* pGauge1 = ui::CreateObject(ui::CLSID_GAUGE, "gauge1", 10, 10, 300, 20, pFrame);
		ui::Get_IGauge(pGauge1)->SetPoint(50.0f, 100.0f);
		pGauge1->SetSkinName("hpgauge_24pixel");		
		ui::Get_IGauge(pGauge1)->SetBlink(0.0f, 2.0f, 0xFF330000);
	}
}

// NOTE : 플레쉬 컨트롤 테스트
void	app::CreateTestFrame_FlashControl()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "flashControlFrame", -100, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->ShowFrame(TRUE);
		ui::Get_IFrame(pFrame)->ShowTitleBar(FALSE);		
		ui::Get_IFrame(pFrame)->SetBlend(70);
		ui::Get_IFrame(pFrame)->SetMoveInTitleBar(FALSE);
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);

		ui::IObject* pFlashControl = ui::CreateObject(ui::CLSID_FLASHCONTROL, "FlashControl", -100, 100, 600, 100, pFrame);				
		ui::Get_IFlashControl(pFlashControl)->SetFlashObjectName("flash_a0001");

		pFrame->ShowWindow(TRUE);
	}	
}