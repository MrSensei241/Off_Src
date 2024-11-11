#ifndef __PTCLAPP_H__
#define __PTCLAPP_H__

#define DEFAULT_FILE_TEXTURELIST "textureInfo.xml"

struct camController
{
	FLOAT distance, latitude, longitude;
	imc::vector3 eyePos;
	imc::vector3 atPos;

	camController() : distance(0), latitude(0), longitude(0)
	{}

	void Init()
	{	
		distance	= 100.0f;
		longitude   = 180.0f;
		latitude    = -8.0f;
		eyePos		= imc::vector3( 0.0f, 20.0f, -200.0f );
		atPos		= imc::vector3( 0.0f, 0.0f, 0.0f );
	}
};

enum ActiveMode
{
	AM_NONE = 0,
	AM_LINEAR,
	AM_CIRCUIT,
	AM_ROTATION,
	AM_SPLINE,
};

enum EFFECT_LEVEL
{
	EL_CATEGORY = 0,
	EL_EFFECT,
	EL_LAYER,
	EL_COMPONENT,
	EL_COLORBLEND,
	EL_TRANSFORM,
	EL_EMITTER,
};

namespace app
{
	BOOL	Init(HWND hWnd, int nWndWidth,int nWndHeight);
	void	Exit();
	void	Run();

	void	StopAllParticle();
	void	Play();	

	void	PrevEffect();
	void	NextEffect();
	void	SetEffect(int effectIndex);
	void	SetEffect(const char* effectName);
	void	SetLayer(int layerIndex);
	void	SetLayer(const char* layerName);
	int		GetEffectIndex();
	int		GetLayerIndex();

	void	CopyLayer(imcEffect::ILayer* pInfo);
	imcEffect::ILayer* PasteLayer();
	imcEffect::ILayer* CreateNewLayer(const char* layerName);

	BOOL	SaveEffect_Xml();
	BOOL	SaveEffect_Bin();
	BOOL	LoadEffect_Bin();

	void	Toggle_ShowUI();
	BOOL	IsInitialized();
	void	Pause(BOOL bPause);

	camController*	 GetCamCtrl();
	imcd3d9::Camera* GetCamera();
	void			ApplyCamera();
	void			FowardCamera(float fDistance);

	// Grid
	BOOL	GridToggle();

	// 파티클 동작 설정
	void	SetMode(ActiveMode mode);
	void	AddSpd(float fSpd		=	1.0f);
	void	AddRadius(float fRadius	=	10.0f);	

	//배경 설정
	void	SetBGColor(COLORREF rgb);
};

#endif