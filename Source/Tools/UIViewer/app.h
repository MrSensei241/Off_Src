#pragma once

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define MAX_TESTFRAME	100

class CWordBreaker : public ui::IWordBreaker 
{
	char * GetNextChar(char * pText);
};

void SetItemToSlot(ui::IObject* pSlot, int nItem);

struct HOTKEY
{
	int				activeKey;
	char			activeChar;
	BOOL			useAlt;
	BOOL			useCtrl;
	BOOL			useShift;
	
	std::string		frameName;
};

class app 
{
public:

	void					Init(HINSTANCE hInst);
	void					Exit();
	void					Run();

	void					ToggleSleepMode();

	void					TestGlobalImageItem(int x, int y);

private:

	ATOM					_MyRegisterClass();	

	BOOL					_InitInstance();
	BOOL					_InitBaseLib();
	BOOL					_InitEtcLib();

	// NOTE : 애드온 프레임 
	void					CreateListFrame();	

	// NOTE : 개별 개체 테스트
	void					CreateTestFrame_EditBox();
	void					CreateTestFrame_List();
	void					CreateTestFrame_DropList();
	void					CreateTestFrame_Button();
	void					CreateTestFrame_RadioButton();
	void					CreateTestFrame_SlideBar();
	void					CreateTestFrame_Picture();
	void					CreateTestFrame_StaticText();
	void					CreateTestFrame_RichText();
	void					CreateTestFrame_Tree();
	void					CreateTestFrame_TextView();
	void					CreateTestFrame_GroupBox();
	void					CreateTestFrame_Tab();
	void					CreateTestFrame_Slot();
	void					CreateTestFrame_Gauge();		 
	void					CreateTestFrame_ControlSet();
	void					CreateTestFrame_FlashControl();

	// NOTE : 프레임 기능 테스트
	void					CreateTestFrame_TextFrame();
	void					CreateTestFrame_ContextMenuFrame();
	void					CreateTestFrame_MessageBoxFrame();

	void					CreateTestFrame_FrameInstant();
	void					CreateTestFrame_ImageItem();
	void					CreateTestFrame_FrameResize();
	void					CreateTestFrame_FrameMove();
	void					CreateTestFrame_FrameFade();

	// NOTE : UI 기능 테스트
	void					CreateTestFrame_ToolTip();

	// NOTE : Skin 기능 테스트
	void					CreateTestFrame_SkinBorder();

	void					Update();
	void					Render();	

	// NOTE : 툴
	void					RenderHelpInfo();

	HWND					m_hWnd;
	HINSTANCE				m_hInstance;
	int						m_wndWidth;
	int						m_wndHeight;

	BOOL					m_isSleepMode;

	char					szTitle[16];			
	char					szWindowClass[16];	
	
	ui::IMessageHandler*	m_pMsgHandler;			
	CWordBreaker			m_wordBreaker;
 
 	ui::IObject*			m_pFrame[MAX_TESTFRAME];
};

inline void app::ToggleSleepMode()
{
	m_isSleepMode = !m_isSleepMode;
}
