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

void app::CreateTestFrame_FrameResize()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "ResizeFrame", -100, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::RIGHT, ui::TOP);		
		ui::Get_IFrame(pFrame)->SetTitleName("리사이징");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);
		ui::Get_IFrame(pFrame)->SetMinSize(100, 100);
		ui::Get_IFrame(pFrame)->SetMaxSize(500, 500);

		ui::IObject* pText1 = ui::CreateObject(ui::CLSID_RICHTEXT, "Text1", 5, 5, 300, 20, pFrame);
		pText1->SetTextAlign(ui::CENTER_HORZ, ui::CENTER_VERT);
		pText1->EnableRatioWidth(TRUE);
		pText1->SetText("Resize Test ----------");

		ui::IObject* pText2 = ui::CreateObject(ui::CLSID_BUTTON, "Text2", -5, 5, 80, 20, pFrame);
		pText2->SetGravity(ui::RIGHT, ui::TOP);
		pText2->SetText("{s12}RightAlign");

		ui::IObject* pPic1 = ui::CreateObject(ui::CLSID_PICTURE, "Picture1", 10, 30, 200, 250, pFrame);
		ui::Get_IPicture(pPic1)->SetImage("icon_com_gatherplant");
		ui::Get_IPicture(pPic1)->SetEnableStretch(TRUE);

		ui::IObject* pPic2 = ui::CreateObject(ui::CLSID_PICTURE, "Picture2", 20, 0, 160, 60, pFrame);
		pPic2->SetSnap(pPic1, ui::AS_RIGHT, ui::AS_TOP);
		ui::Get_IPicture(pPic2)->SetImage("icon_com_gatherplant");
		ui::Get_IPicture(pPic2)->SetEnableStretch(TRUE);

		ui::IObject* pPic3 = ui::CreateObject(ui::CLSID_PICTURE, "Picture3", 0, 20, 40, 60, pFrame);
		pPic3->SetSnap(pPic2, ui::AS_BOTTOM, ui::AS_LEFT);
		ui::Get_IPicture(pPic3)->SetImage("icon_com_gatherplant");
		ui::Get_IPicture(pPic3)->SetEnableStretch(TRUE);

		ui::IObject* pPic4 = ui::CreateObject(ui::CLSID_PICTURE, "Picture4", 10, 0, 40, 120, pFrame);
		pPic4->SetSnap(pPic3, ui::AS_RIGHT, ui::AS_TOP);		
		ui::Get_IPicture(pPic4)->SetImage("icon_com_gatherplant");
		ui::Get_IPicture(pPic4)->SetEnableStretch(TRUE);

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_FrameMove()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "MovingFrame", 0, 0, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::CENTER_HORZ, ui::CENTER_VERT);
		ui::Get_IFrame(pFrame)->SetTitleName("프레임이동");
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);
		ui::Get_IFrame(pFrame)->SetMinSize(100, 100);
		ui::Get_IFrame(pFrame)->SetMaxSize(500, 500);
		ui::Get_IFrame(pFrame)->EnableSnapClient(TRUE);
		ui::Get_IFrame(pFrame)->EnableSnapFrame(TRUE);

		ui::IObject* pText1 = ui::CreateObject(ui::CLSID_RICHTEXT, "Text1", 5, 5, 300, 200, pFrame);
		pText1->SetTextAlign(ui::CENTER_HORZ, ui::CENTER_VERT);
		pText1->EnableRatioWidth(TRUE);
		pText1->SetText("Frame Moving Test ----------");

		pFrame->ShowWindow(TRUE);
	}
}	

void app::CreateTestFrame_FrameFade()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "FadeFrame", -100, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->ShowWindow(TRUE);
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);		
		ui::Get_IFrame(pFrame)->SetTitleName("페이딩 테스트");		
	}
}

void app::CreateTestFrame_ImageItem()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "imageItemFrame", -100, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->ShowWindow(TRUE);
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->SetTitleName("이미지아이템 테스트");

		ui::CreateImageItem("Image1", 0, 0, "1");
	}
}

void app::CreateTestFrame_FrameInstant()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "InstantFrame", -100, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->ShowWindow(TRUE);
		pFrame->SetGravity(ui::RIGHT, ui::TOP);
		ui::Get_IFrame(pFrame)->EnableCloseButton(TRUE);		
		ui::Get_IFrame(pFrame)->EnableInstanceMode(TRUE);
	}
}

void app::CreateTestFrame_ToolTip()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "tooltipTestFrame", 0, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::CENTER_HORZ, ui::TOP);
		ui::Get_IFrame(pFrame)->ShowFrame(TRUE);
		ui::Get_IFrame(pFrame)->ShowTitleBar(FALSE);		
		ui::Get_IFrame(pFrame)->SetBlend(70);
		ui::Get_IFrame(pFrame)->SetMoveInTitleBar(FALSE);
		ui::Get_IFrame(pFrame)->EnableResize(TRUE);

		ui::IObject* pPic = ui::CreateObject(ui::CLSID_PICTURE, "Picture", 0, 0, 64, 64, pFrame);		
		//pPic->EnableHitTest(FALSE);
		ui::Get_IPicture(pPic)->SetImage("icon_com_gatherplant");
		ui::Get_IPicture(pPic)->SetEnableStretch(TRUE);		

		pPic->SetTooltipType("common");
		pPic->SetTooltipData("image", "icon_com_gatherplant");
		pPic->SetTooltipData("name", "icon_com_gatherplant");
		pPic->SetTooltipData("comment", "icon_com_gatherplant");		

		pFrame->ShowWindow(TRUE);
	}	
}

void app::CreateTestFrame_SkinBorder()
{
	ui::IObject* pFrame = ui::CreateObject(ui::CLSID_FRAME, "skinBorderTestFrame", 0, 100, 400, 300, NULL);
	if (pFrame != NULL)
	{
		pFrame->SetGravity(ui::CENTER_HORZ, ui::TOP);		
		pFrame->ShowWindow(TRUE);				

		ui::IObject* pPic = ui::CreateObject(ui::CLSID_PICTURE, "Picture", 0, 0, 360, 260, pFrame);
		ui::Get_IPicture(pPic)->SetImage("icon_com_gatherplant");
		ui::Get_IPicture(pPic)->SetEnableStretch(TRUE);
	}
}