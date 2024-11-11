#pragma once

#include <comdef.h>
#include <control.h> 
#include <string>
#include <tchar.h>
#include <atlbase.h>
#include <Exdisp.h>
#include <Windows.h>

	// 임베디드 브라우저 기능의 구현하기 위해서는 Com객체로 접근하여 
	// IWebBrowser2를 띄워야 한다. 
	// 그러나 일반적으로 보안성을 위하여 브라우저창로 그냥 Open하는 것이 아닌, 
	// 하나의 프로세스 내의 창에 브라우저 내용을 Open하는 것이다.

	// Navigate하기 위해서는 해당 구조체를 채워놓아야 한다.
	// 기본적으로 vtUrl만 채워놓으면 됀다.
	struct NaviVariant
	{
		NaviVariant()
		{
			::VariantInit(&vtUrl);
			::VariantInit(&vtTaget);
			::VariantInit(&vtPostdata);
			::VariantInit(&vtHeader);
			::VariantInit(&vtFlags);
		}

		VARIANT vtUrl;
		VARIANT vtTaget;
		VARIANT vtPostdata;
		VARIANT vtHeader;
		VARIANT vtFlags;
	};

	// 닫기이벤트를 가로채기 위한 기존Proc을 갈아끼운 Proc에서 접근할 수 있게 하기 위한 스태틱 글로벌 포인터.
	// 않좋은 구조. 바꿔야할 방식이지만, 이외에 방법이 없음.
	static WNDPROC OldOleComProc = NULL;

	// 닫기이벤트를 가로채기 위한 기존Proc을 갈아끼울 Proc
	LRESULT CALLBACK OleComProc(HWND, UINT, WPARAM, LPARAM);

	// IOleClientSite와 IOleInPlaceSite, IStorage 역할들을 모두 할 수 있게 다중상속하고 있는데, 
	// 나중에 확장하기 위하여 정리할때에 고쳐야 할 부분.
	// 여태 예제들이 모두 다중상속으로 시도하고 있었다. 
	class Embedded_Browser :
		public IOleClientSite,
		public IOleInPlaceSite,
		public IStorage
	{
	public:

		Embedded_Browser(RECT rtWnd);
		virtual ~Embedded_Browser();
		HRESULT Init(HWND _hWndParent);
		HRESULT CreateBrowser();
		RECT PixelToHiMetric(const RECT& _rc);
		virtual void SetRect(const RECT& _rc);

		// ----- Control methods -----
		void GoBack();
		void GoForward();
		void Refresh();
		HRESULT Navigate(NaviVariant* navVariants);

		// ----- IUnknown -----
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void**ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void);
		virtual ULONG STDMETHODCALLTYPE Release(void);

		// ---------- IOleWindow ----------
		virtual HRESULT STDMETHODCALLTYPE GetWindow(__RPC__deref_out_opt HWND *phwnd) override;
		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) override;

		// ---------- IOleInPlaceSite ----------
		virtual HWND GetControlWindow();
		virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void) override;
		virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void) override;
		virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void) override;
		virtual HRESULT STDMETHODCALLTYPE GetWindowContext(
			__RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
			__RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
			__RPC__out LPRECT lprcPosRect,
			__RPC__out LPRECT lprcClipRect,
			__RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo) override;
		virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant) override;
		virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable) override;
		virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void) override;
		virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void) override;
		virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void) override;
		virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(__RPC__in LPCRECT lprcPosRect) override;

		// ---------- IOleClientSite ----------
		virtual HRESULT STDMETHODCALLTYPE SaveObject(void) override;
		virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, __RPC__deref_out_opt IMoniker **ppmk) override;
		virtual HRESULT STDMETHODCALLTYPE GetContainer(__RPC__deref_out_opt IOleContainer **ppContainer) override;
		virtual HRESULT STDMETHODCALLTYPE ShowObject(void) override;
		virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow) override;
		virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void) override;

		// ----- IStorage -----
		virtual HRESULT STDMETHODCALLTYPE CreateStream(
			__RPC__in_string const OLECHAR *pwcsName,
			DWORD grfMode,
			DWORD reserved1,
			DWORD reserved2,
			__RPC__deref_out_opt IStream **ppstm) override;

		virtual HRESULT STDMETHODCALLTYPE OpenStream(
			const OLECHAR *pwcsName,
			void *reserved1,
			DWORD grfMode,
			DWORD reserved2,
			IStream **ppstm) override;

		virtual HRESULT STDMETHODCALLTYPE CreateStorage(
			__RPC__in_string const OLECHAR *pwcsName,
			DWORD grfMode,
			DWORD reserved1,
			DWORD reserved2,
			__RPC__deref_out_opt IStorage **ppstg) override;

		virtual HRESULT STDMETHODCALLTYPE OpenStorage(
			__RPC__in_opt_string const OLECHAR *pwcsName,
			__RPC__in_opt IStorage *pstgPriority,
			DWORD grfMode,
			__RPC__deref_opt_in_opt SNB snbExclude,
			DWORD reserved,
			__RPC__deref_out_opt IStorage **ppstg) override;

		virtual HRESULT STDMETHODCALLTYPE CopyTo(
			DWORD ciidExclude,
			const IID *rgiidExclude,
			__RPC__in_opt  SNB snbExclude,
			IStorage *pstgDest) override;

		virtual HRESULT STDMETHODCALLTYPE MoveElementTo(
			__RPC__in_string const OLECHAR *pwcsName,
			__RPC__in_opt IStorage *pstgDest,
			__RPC__in_string const OLECHAR *pwcsNewName,
			DWORD grfFlags) override;

		virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) override;

		virtual HRESULT STDMETHODCALLTYPE Revert(void) override;

		virtual HRESULT STDMETHODCALLTYPE EnumElements(
			DWORD reserved1,
			void *reserved2,
			DWORD reserved3,
			IEnumSTATSTG **ppenum) override;

		virtual HRESULT STDMETHODCALLTYPE DestroyElement(__RPC__in_string const OLECHAR *pwcsName) override;

		virtual HRESULT STDMETHODCALLTYPE RenameElement(
			__RPC__in_string const OLECHAR *pwcsOldName,
			__RPC__in_string const OLECHAR *pwcsNewName) override;

		virtual HRESULT STDMETHODCALLTYPE SetElementTimes(
			__RPC__in_opt_string const OLECHAR *pwcsName,
			__RPC__in_opt const FILETIME *pctime,
			__RPC__in_opt const FILETIME *patime,
			__RPC__in_opt const FILETIME *pmtime) override;

		virtual HRESULT STDMETHODCALLTYPE SetClass(__RPC__in REFCLSID clsid) override;
		virtual HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits, DWORD grfMask) override;
		virtual HRESULT STDMETHODCALLTYPE Stat(__RPC__out STATSTG *pstatstg, DWORD grfStatFlag) override;

	protected:
		IOleObject* oleObject;					///<임베디드 브라우저 기능의 총괄적인 오브젝트>
		IOleInPlaceObject* oleInPlaceObject;	///<브라우저 내용이 표시되는 부분의 오브젝트.>

		IWebBrowser2* webBrowser2;				///<임베디드 브라우저 기능의 핵심 객체>

		LONG iComRefCount;						///<레퍼런스 카운터>	
		// 사실, 이에 맞춰 재설계해야한다. 
		// 현재는 결제창 하나만 띄우기만 하면 됌으로 무시된 의미없는 내용(코드는 사용되고 있음.)

		RECT rObject;							///<창 내부의 브라우저 크기>

		HWND hWndParent;						///<임베디드 브라우저의 창 HWND>
		HWND hWndControl;						///<브라우저 내용이 표시되는 부분인 오브젝트의 창 HWND>

	};	// End of class Embedded_Browser;
	