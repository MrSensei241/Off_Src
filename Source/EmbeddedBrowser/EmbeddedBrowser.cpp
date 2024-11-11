// EmbeddedBrowser.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "EmbeddedBrowser.h"

	Embedded_Browser::Embedded_Browser(RECT rtWnd)
		: webBrowser2(NULL)
		, iComRefCount(0)
		, hWndControl(0)
		, oleObject(NULL)
		, oleInPlaceObject(NULL)
	{
		::SetRect(&rObject, -rtWnd.left, -rtWnd.top, rtWnd.right, rtWnd.bottom);

		// Com 객체를 사용하기 위해서는 CoInitialize(Null)을 호출해야 한다. 
		// CoInitialize를 호출하면 COM 객체의 사용이 끝났을 때 반드시 CoUninitialize를 호출해 줘야 한다.
		// CoInitialize를 n 번 호출할 경우 CoUninitialize도 n 번 호출이 되어야 한다는 의미.
		// 만약 멀티스레드이면, Thread당 CoInitialize / CoUninitialize가 짝이 맞아야 한다.
		// 특별한 이유가 있지 않고서야, CoInitialize를 사용해서는 안된다.

		// 프로그램에서 시작할때에 CoInitialize를 한번만 호출하면 된다고 하지만, 
		// CoUninitialize()이 없다면, 
		// TOS클라이언트에서는 어떤 이유에서인지 CoInitialize가 RPC_E_CHANGED_MODE(0x80010106, -2147417850)를 리턴시킴. (원인불명)

		// CoInitialize()와 OleInitialize() 는 비슷한 것이고, 
		// 해당 클래스는 OleCreate()를 사용함으로서 OleInitialize() 사용이 맞지만,
		// 앞서 어떤 코드에서 CoInitialize()를 쓰고 있기 때문에 CoUninitialize()를 먼저 해줘야 한다.
		// CoUninitialize()를 해주었음으로 OleInitialize()를 해주어도 상관 없지만, 
		// CoUninitialize()으로 했음으로 맞춰서 CoInitialize()를 해줌.E_NOINTERFACE

		CoInitialize(NULL);

		//함수 설명. (만들어진지 오래되었고 최근에는 C#으로 사용이 줄어들어 C++에 대한 정보가 희귀해서 설명해둠.)

		/// <CoInitialize> 
		// 현재 스레드에서 COM library를 초기화 하거나 STA 같은 동시성 모델을 식별함
		// 새 응용프로그램은 CoInitialize대신에 CoInitializeEx를 호출 해야 함

		/// <CoInitializeEx> 
		// 호출 스레드를 사용하는 COM library를 초기화 하고, 스레드의 동시성 모델을 설정하고,
		// 필요한 경우 스레드의 apartment를 생성한다.
		// 멀티스레드인 경우, CoInitialize 대신 사용하고 
		// Thread당 CoInitialize / CoUninitialize가 짝이 맞지 않다면 성능 및 퍼포먼스에 크게 영향이 갈 수 있다.
		// 2번째 인자값은 스레드의 동시성 모델 및 초기화 옵션으로 COINIT 열거형에 있는 값이다.
		//		COINIT_APARTMENTTHREADEDCOINIT와 COINIT_MULTITHREADED 플래그를 같이 사용할 수 없다는 점을
		//		제외한 COINIT 열거형에 있는 값을 모두 조합 할 수 있음

		/// <CoUninitialize > 
		// 현재 스레드의 COM 라이브러리를 닫고, 스레드가 로드된 모든 DLL를 unload 시키며,
		// 다른 스레드 유지하는 리소스를 할당 해제 시키며, 강제로 스레드에 연결하는 모든 RPC를 닫음
		// 현재 코드에서 CoUninitialize()함으로서 다른 로직에서 Com객체를 못얻어오는 사이드 이펙트가 발생 할 수 있다.
	}

	Embedded_Browser::~Embedded_Browser()
	{
		CoUninitialize();

		if (oleInPlaceObject != NULL)
		{
			oleInPlaceObject->Release();
			oleInPlaceObject = NULL;
		}

		if (oleObject != NULL)
		{
			oleObject->Close(OLECLOSE_NOSAVE);
			oleObject->Release();
		}

		if (webBrowser2 != NULL)
		{
			webBrowser2->Quit();
			webBrowser2->Release();
			webBrowser2 = NULL;
		}
	}

	HRESULT Embedded_Browser::Init(HWND _hWndParent)
	{
		hWndParent = _hWndParent;
		HRESULT hr = CreateBrowser();
		return hr;
	}


	HRESULT Embedded_Browser::CreateBrowser()
	{
		// TOS클라이언트에서 CoInitialize 없이 OleCreate를 시도하면, 
		// E_NOINTERFACE(0x80004002, -2147467262)를 리턴시키고 객체를 만들지를 못한다.
		// 여기서 CoCreateInstance를 사용하는 이유는 OleCreate의 매개변수에 this를 넣는 것과 같은 이유이며,
		// 해당 Embedded_Browser 클래스가 재정의된 객체로 이벤트 후킹이나 클라이언트와의 연결을 하기 위한 조치이다.

		HRESULT hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (void**)&webBrowser2);
		if (FAILED(hr))
		{
			MessageBox(NULL, _T("CoCreateInstance() failed"), _T("Error"), MB_ICONERROR);
			DestroyWindow(hWndParent);
			return hr;
		}

		// Query WebBrowser for IOleObject pointer
		hr = webBrowser2->QueryInterface(IID_IOleObject, (void**)&oleObject);
		if (FAILED(hr))
		{
			MessageBox(NULL, _T("webBrowser2->QueryInterface() failed"), _T("Error"), MB_ICONERROR);
			DestroyWindow(hWndParent);
			return hr;
		}

		// Set client site
		hr = oleObject->SetClientSite(this);
		if (FAILED(hr))
		{
			MessageBox(NULL, _T("oleObject->SetClientSite() failed"), _T("Error"), MB_ICONERROR);
			DestroyWindow(hWndParent);
			return hr;
		}

		// In-place activate the WebBrowser control
		RECT rcClient;
		GetClientRect(hWndParent, &rcClient);
		hr = oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, hWndParent, &rcClient);
		if (FAILED(hr))
		{
			MessageBox(NULL, _T("oleObject->DoVerb() failed"), _T("Error"), MB_ICONERROR);
			DestroyWindow(hWndParent);
			return hr;
		}

		// 닫기 관련 이벤트 후킹을 위하여 시도한 방식이다. 
		// 이벤트 후킹 방식은 MFC로는 쉽게 접근 가능하나, 클라이언트에 MFC를 추가 할 수 없고, 
		// ATLAdvise를 이용하여 DWebBrowserEvents2의 재정의된 클래스를 등록하여 이벤트를 후킹하는 방식이 있지만, 
		// 브라우저창이 아닌 다른 윈도우의 페이지로 만들어지는 경우일때에는 DISPID_ONQUIT, DISPID_QUIT 이벤트가 전혀 발생하지 않는다.
		// 그럼으로 닫는 이벤트를 얻기 위하여 oleInPlaceObject의 윈도우에 SetWindowLongPtr(,GWLP_WNDPROC,)으로 Proc을 갈아끼우고.
		// 갈아끼운 Proc이 Close외 에는 필요 없지만, 다른 메세지들을 처리해야함으로 기존 Proc도 작동하게 함.
		// DWebBrowserEvents2와 ATLAdvise를 이용하는 방법은 링크를 남겨둠.
		// http://lab.gamecodi.com/board/zboard.php?id=GAMECODILAB_QnA_etc&page=9&sn1=&divpage=1&sn=off&ss=on&sc=on&select_arrange=hit&desc=desc&no=2115
		// 그러나 현재 구현된 방식은 기존Proc을 스태틱 처리함으로서 문제있는 방식이다. 이는 후에 고쳐야될 문제임.
		HWND hWnd = GetControlWindow();
		if (hWnd != 0)
		{
			OldOleComProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(OleComProc)));
			if (OldOleComProc == NULL)
			{
				MessageBox(NULL, _T("Cannot Changed oleObject Proc"), _T("Error"), MB_ICONERROR);
				DestroyWindow(hWndParent);
				return S_OK;
			}
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(OldOleComProc));
		}

		return hr;
	}

	RECT Embedded_Browser::PixelToHiMetric(const RECT& _rc)
	{
		static bool s_initialized = false;
		static int s_pixelsPerInchX, s_pixelsPerInchY;
		if (!s_initialized)
		{
			HDC hdc = ::GetDC(0);
			s_pixelsPerInchX = ::GetDeviceCaps(hdc, LOGPIXELSX);
			s_pixelsPerInchY = ::GetDeviceCaps(hdc, LOGPIXELSY);
			::ReleaseDC(0, hdc);
			s_initialized = true;
		}

		RECT rc;
		rc.left = MulDiv(2540, _rc.left, s_pixelsPerInchX);
		rc.top = MulDiv(2540, _rc.top, s_pixelsPerInchY);
		rc.right = MulDiv(2540, _rc.right, s_pixelsPerInchX);
		rc.bottom = MulDiv(2540, _rc.bottom, s_pixelsPerInchY);
		return rc;
	}

	void Embedded_Browser::SetRect(const RECT& _rc)
	{
		rObject = _rc;

		{
			RECT hiMetricRect = PixelToHiMetric(rObject);
			SIZEL sz;
			sz.cx = hiMetricRect.right - hiMetricRect.left;
			sz.cy = hiMetricRect.bottom - hiMetricRect.top;
			oleObject->SetExtent(DVASPECT_CONTENT, &sz);
		}

		if (oleInPlaceObject != 0)
		{
			oleInPlaceObject->SetObjectRects(&rObject, &rObject);
		}
	}

	// ----- Control methods -----
	void Embedded_Browser::GoBack()		{ this->webBrowser2->GoBack(); }
	void Embedded_Browser::GoForward()	{ this->webBrowser2->GoForward(); }
	void Embedded_Browser::Refresh()	{ this->webBrowser2->Refresh(); }
	HRESULT Embedded_Browser::Navigate(NaviVariant* navVariants)
	{
		if (webBrowser2 == NULL)
			return S_OK;
		
		webBrowser2->put_Silent(VARIANT_TRUE);

		// Navigate2는 해당 URL을 탐험한다는 의미로 해당URL에 따라 웹페이지를 띄우게 한다.
		HRESULT hrIE = webBrowser2->Navigate2(&navVariants->vtUrl, &navVariants->vtFlags, &navVariants->vtTaget, &navVariants->vtPostdata, &navVariants->vtHeader);
		//HRESULT hrIE = webBrowser2->Navigate(navVariants->vtUrl.bstrVal, &navVariants->vtFlags, &navVariants->vtTaget, &navVariants->vtPostdata, &navVariants->vtHeader);
		
		// 해당 페이지를 보이게 하려면 꼭 put_Visible(VARIANT_TRUE)를 통해서만 가능하다.
		webBrowser2->put_Visible(VARIANT_TRUE);
		return hrIE;
	}

	// ----- IUnknown -----
	HRESULT STDMETHODCALLTYPE Embedded_Browser::QueryInterface(REFIID riid, void**ppvObject)
	{
		if (riid == __uuidof(IUnknown))		{ (*ppvObject) = static_cast<IOleClientSite*>(this); }
		else if (riid == __uuidof(IOleInPlaceSite)){ (*ppvObject) = static_cast<IOleInPlaceSite*>(this); }
		else	{ return E_NOINTERFACE; }
		AddRef();
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE Embedded_Browser::AddRef(void)
	{
		iComRefCount++;
		return iComRefCount;
	}

	ULONG STDMETHODCALLTYPE Embedded_Browser::Release(void)
	{
		iComRefCount--;
		return iComRefCount;
	}

	// ---------- IOleWindow ----------
	HRESULT STDMETHODCALLTYPE Embedded_Browser::GetWindow(__RPC__deref_out_opt HWND *phwnd)
	{
		(*phwnd) = hWndParent;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::ContextSensitiveHelp(BOOL fEnterMode) { return E_NOTIMPL; }

	// ---------- IOleInPlaceSite ----------
	HWND Embedded_Browser::GetControlWindow()
	{
		if (hWndControl != 0)
			return hWndControl;

		if (oleInPlaceObject == 0)
			return 0;

		oleInPlaceObject->GetWindow(&hWndControl);
		return hWndControl;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::CanInPlaceActivate(void) { return S_OK; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::OnInPlaceActivate(void)
	{
		OleLockRunning(oleObject, TRUE, FALSE);
		oleObject->QueryInterface(&oleInPlaceObject);
		oleInPlaceObject->SetObjectRects(&rObject, &rObject);

		return S_OK;

	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::OnUIActivate(void)	{ return S_OK; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::GetWindowContext(
		__RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
		__RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
		__RPC__out LPRECT lprcPosRect,
		__RPC__out LPRECT lprcClipRect,
		__RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
		HWND hwnd = hWndParent;

		(*ppFrame) = NULL;
		(*ppDoc) = NULL;
		(*lprcPosRect).left = rObject.left;
		(*lprcPosRect).top = rObject.top;
		(*lprcPosRect).right = rObject.right;
		(*lprcPosRect).bottom = rObject.bottom;
		*lprcClipRect = *lprcPosRect;

		lpFrameInfo->fMDIApp = false;
		lpFrameInfo->hwndFrame = hwnd;
		lpFrameInfo->haccel = NULL;
		lpFrameInfo->cAccelEntries = 0;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::Scroll(SIZE scrollExtant)		{ return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::OnUIDeactivate(BOOL fUndoable)	{ return S_OK; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::OnInPlaceDeactivate(void)
	{
		hWndControl = 0;
		oleInPlaceObject = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::DiscardUndoState(void)	{ return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::DeactivateAndUndo(void)	{ return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::OnPosRectChange(__RPC__in LPCRECT lprcPosRect)	{ return S_OK; }

	// ---------- IOleClientSite ----------
	HRESULT STDMETHODCALLTYPE Embedded_Browser::SaveObject(void)	{ return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::GetMoniker(
		DWORD dwAssign,
		DWORD dwWhichMoniker,
		__RPC__deref_out_opt IMoniker **ppmk)
	{
		if ((dwAssign == OLEGETMONIKER_ONLYIFTHERE) &&
			(dwWhichMoniker == OLEWHICHMK_CONTAINER))
			return E_FAIL;

		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::GetContainer(__RPC__deref_out_opt IOleContainer **ppContainer)
	{
		return E_NOINTERFACE;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::ShowObject(void) { return S_OK; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::OnShowWindow(BOOL fShow) { return S_OK; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::RequestNewObjectLayout(void)	{ return E_NOTIMPL; }

	// ----- IStorage -----

	HRESULT STDMETHODCALLTYPE Embedded_Browser::CreateStream(
		__RPC__in_string const OLECHAR *pwcsName,
		DWORD grfMode,
		DWORD reserved1,
		DWORD reserved2,
		__RPC__deref_out_opt IStream **ppstm)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::OpenStream(
		const OLECHAR *pwcsName,
		void *reserved1,
		DWORD grfMode,
		DWORD reserved2,
		IStream **ppstm)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::CreateStorage(
		__RPC__in_string const OLECHAR *pwcsName,
		DWORD grfMode,
		DWORD reserved1,
		DWORD reserved2,
		__RPC__deref_out_opt IStorage **ppstg)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::OpenStorage(
		__RPC__in_opt_string const OLECHAR *pwcsName,
		__RPC__in_opt IStorage *pstgPriority,
		DWORD grfMode,
		__RPC__deref_opt_in_opt SNB snbExclude,
		DWORD reserved,
		__RPC__deref_out_opt IStorage **ppstg)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::CopyTo(
		DWORD ciidExclude,
		const IID *rgiidExclude,
		__RPC__in_opt  SNB snbExclude,
		IStorage *pstgDest)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::MoveElementTo(
		__RPC__in_string const OLECHAR *pwcsName,
		__RPC__in_opt IStorage *pstgDest,
		__RPC__in_string const OLECHAR *pwcsNewName,
		DWORD grfFlags)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::Commit(DWORD grfCommitFlags)	{ return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::Revert(void) { return E_NOTIMPL; }

	HRESULT STDMETHODCALLTYPE Embedded_Browser::EnumElements(
		DWORD reserved1,
		void *reserved2,
		DWORD reserved3,
		IEnumSTATSTG **ppenum)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::DestroyElement(__RPC__in_string const OLECHAR *pwcsName)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::RenameElement(
		__RPC__in_string const OLECHAR *pwcsOldName,
		__RPC__in_string const OLECHAR *pwcsNewName)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::SetElementTimes(
		__RPC__in_opt_string const OLECHAR *pwcsName,
		__RPC__in_opt const FILETIME *pctime,
		__RPC__in_opt const FILETIME *patime,
		__RPC__in_opt const FILETIME *pmtime)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::SetClass(__RPC__in REFCLSID clsid) { return S_OK; }
	HRESULT STDMETHODCALLTYPE Embedded_Browser::SetStateBits(DWORD grfStateBits, DWORD grfMask)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Embedded_Browser::Stat(__RPC__out STATSTG *pstatstg, DWORD grfStatFlag)
	{
		return E_NOTIMPL;
	}
			
	LRESULT CALLBACK OleComProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		if (msg == WM_CLOSE)
		{
			HWND hWndParent = GetParent(hwnd);
			if (hWndParent != 0)
			{
				DestroyWindow(hWndParent);
				return 0;
			}
		}

		if (OldOleComProc == NULL)
		{
			return DefWindowProc(hwnd, msg, wp, lp);
		}
		return CallWindowProc(OldOleComProc, hwnd, msg, wp, lp);
	}
