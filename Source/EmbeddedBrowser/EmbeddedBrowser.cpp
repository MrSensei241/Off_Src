// EmbeddedBrowser.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
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

		// Com ��ü�� ����ϱ� ���ؼ��� CoInitialize(Null)�� ȣ���ؾ� �Ѵ�. 
		// CoInitialize�� ȣ���ϸ� COM ��ü�� ����� ������ �� �ݵ�� CoUninitialize�� ȣ���� ��� �Ѵ�.
		// CoInitialize�� n �� ȣ���� ��� CoUninitialize�� n �� ȣ���� �Ǿ�� �Ѵٴ� �ǹ�.
		// ���� ��Ƽ�������̸�, Thread�� CoInitialize / CoUninitialize�� ¦�� �¾ƾ� �Ѵ�.
		// Ư���� ������ ���� �ʰ���, CoInitialize�� ����ؼ��� �ȵȴ�.

		// ���α׷����� �����Ҷ��� CoInitialize�� �ѹ��� ȣ���ϸ� �ȴٰ� ������, 
		// CoUninitialize()�� ���ٸ�, 
		// TOSŬ���̾�Ʈ������ � ������������ CoInitialize�� RPC_E_CHANGED_MODE(0x80010106, -2147417850)�� ���Ͻ�Ŵ. (���κҸ�)

		// CoInitialize()�� OleInitialize() �� ����� ���̰�, 
		// �ش� Ŭ������ OleCreate()�� ��������μ� OleInitialize() ����� ������,
		// �ռ� � �ڵ忡�� CoInitialize()�� ���� �ֱ� ������ CoUninitialize()�� ���� ����� �Ѵ�.
		// CoUninitialize()�� ���־������� OleInitialize()�� ���־ ��� ������, 
		// CoUninitialize()���� �������� ���缭 CoInitialize()�� ����.E_NOINTERFACE

		CoInitialize(NULL);

		//�Լ� ����. (��������� �����Ǿ��� �ֱٿ��� C#���� ����� �پ��� C++�� ���� ������ ����ؼ� �����ص�.)

		/// <CoInitialize> 
		// ���� �����忡�� COM library�� �ʱ�ȭ �ϰų� STA ���� ���ü� ���� �ĺ���
		// �� �������α׷��� CoInitialize��ſ� CoInitializeEx�� ȣ�� �ؾ� ��

		/// <CoInitializeEx> 
		// ȣ�� �����带 ����ϴ� COM library�� �ʱ�ȭ �ϰ�, �������� ���ü� ���� �����ϰ�,
		// �ʿ��� ��� �������� apartment�� �����Ѵ�.
		// ��Ƽ�������� ���, CoInitialize ��� ����ϰ� 
		// Thread�� CoInitialize / CoUninitialize�� ¦�� ���� �ʴٸ� ���� �� �����ս��� ũ�� ������ �� �� �ִ�.
		// 2��° ���ڰ��� �������� ���ü� �� �� �ʱ�ȭ �ɼ����� COINIT �������� �ִ� ���̴�.
		//		COINIT_APARTMENTTHREADEDCOINIT�� COINIT_MULTITHREADED �÷��׸� ���� ����� �� ���ٴ� ����
		//		������ COINIT �������� �ִ� ���� ��� ���� �� �� ����

		/// <CoUninitialize > 
		// ���� �������� COM ���̺귯���� �ݰ�, �����尡 �ε�� ��� DLL�� unload ��Ű��,
		// �ٸ� ������ �����ϴ� ���ҽ��� �Ҵ� ���� ��Ű��, ������ �����忡 �����ϴ� ��� RPC�� ����
		// ���� �ڵ忡�� CoUninitialize()�����μ� �ٸ� �������� Com��ü�� �������� ���̵� ����Ʈ�� �߻� �� �� �ִ�.
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
		// TOSŬ���̾�Ʈ���� CoInitialize ���� OleCreate�� �õ��ϸ�, 
		// E_NOINTERFACE(0x80004002, -2147467262)�� ���Ͻ�Ű�� ��ü�� �������� ���Ѵ�.
		// ���⼭ CoCreateInstance�� ����ϴ� ������ OleCreate�� �Ű������� this�� �ִ� �Ͱ� ���� �����̸�,
		// �ش� Embedded_Browser Ŭ������ �����ǵ� ��ü�� �̺�Ʈ ��ŷ�̳� Ŭ���̾�Ʈ���� ������ �ϱ� ���� ��ġ�̴�.

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

		// �ݱ� ���� �̺�Ʈ ��ŷ�� ���Ͽ� �õ��� ����̴�. 
		// �̺�Ʈ ��ŷ ����� MFC�δ� ���� ���� �����ϳ�, Ŭ���̾�Ʈ�� MFC�� �߰� �� �� ����, 
		// ATLAdvise�� �̿��Ͽ� DWebBrowserEvents2�� �����ǵ� Ŭ������ ����Ͽ� �̺�Ʈ�� ��ŷ�ϴ� ����� ������, 
		// ������â�� �ƴ� �ٸ� �������� �������� ��������� ����϶����� DISPID_ONQUIT, DISPID_QUIT �̺�Ʈ�� ���� �߻����� �ʴ´�.
		// �׷����� �ݴ� �̺�Ʈ�� ��� ���Ͽ� oleInPlaceObject�� �����쿡 SetWindowLongPtr(,GWLP_WNDPROC,)���� Proc�� ���Ƴ����.
		// ���Ƴ��� Proc�� Close�� ���� �ʿ� ������, �ٸ� �޼������� ó���ؾ������� ���� Proc�� �۵��ϰ� ��.
		// DWebBrowserEvents2�� ATLAdvise�� �̿��ϴ� ����� ��ũ�� ���ܵ�.
		// http://lab.gamecodi.com/board/zboard.php?id=GAMECODILAB_QnA_etc&page=9&sn1=&divpage=1&sn=off&ss=on&sc=on&select_arrange=hit&desc=desc&no=2115
		// �׷��� ���� ������ ����� ����Proc�� ����ƽ ó�������μ� �����ִ� ����̴�. �̴� �Ŀ� ���ľߵ� ������.
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

		// Navigate2�� �ش� URL�� Ž���Ѵٴ� �ǹ̷� �ش�URL�� ���� ���������� ���� �Ѵ�.
		HRESULT hrIE = webBrowser2->Navigate2(&navVariants->vtUrl, &navVariants->vtFlags, &navVariants->vtTaget, &navVariants->vtPostdata, &navVariants->vtHeader);
		//HRESULT hrIE = webBrowser2->Navigate(navVariants->vtUrl.bstrVal, &navVariants->vtFlags, &navVariants->vtTaget, &navVariants->vtPostdata, &navVariants->vtHeader);
		
		// �ش� �������� ���̰� �Ϸ��� �� put_Visible(VARIANT_TRUE)�� ���ؼ��� �����ϴ�.
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
