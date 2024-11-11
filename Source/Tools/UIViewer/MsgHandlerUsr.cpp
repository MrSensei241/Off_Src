#include "stdAfx.h"

#include "imcd3d9.h"
#include "imcinput.h"
#include "imcIME.h"
#include "imcFont.h"
#include "imcUIRender.h"
#include "imcSkin.h"
#include "imcUI.h"
#include "imcUIControls.h"

#include "Script.h"
#include "MsgHandlerUsr.h"

int CMsgHandlerUsr::Handle(const char * pMessage, void * arg1/* = 0*/, void * arg2/* = 0*/)
{
	ON_MESSAGE(APPM_CLOSE) {
		if (arg1 != NULL)
		{
			ui::IObject * pFrame = (ui::IObject *)arg1;
			ui::CloseFrame(pFrame->GetName());
		}
	}
	return 0;
}