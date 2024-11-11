#pragma once

namespace app
{

#define DEFAULT_ACTION_SPEED	150.0f
#define	DEFAULT_TARGET_SPEED	50.0f

enum BlinkType
{
	BLINK_NONE	= 0,	
	BLINK_COLOR,
	BLINK_SCALE	
};

class CShowObject
{
public:

	CShowObject();
	virtual		~CShowObject();

	virtual	void	Create();

	virtual	void	SetTM(imc::matrix44& worldTM);
	imc::matrix44&	GetTM();
	
	void			SetBlinkType(BlinkType type);
	void			SetColor(DWORD color);

	void			SetVisible(BOOL isVisible);
	void			ToggleVisible();
	BOOL			IsVisible();	

	virtual	void	Update(float elapsedTime);
	virtual void	Render();

protected:

	imc::matrix44	m_worldTM;

	float			m_scale;	
	DWORD			m_color;
	BlinkType		m_blinkType;
	BOOL			m_isVisible;

	BOOL			m_isUpRatio;
	float			m_cycleTime;
	float			m_processTime;

	imcd3d9::IVertexBuffer*	m_pVertexBuffer;
	imcd3d9::IIndexBuffer*	m_pIndexBuffer;	
};

inline void	CShowObject::SetBlinkType(BlinkType type)
{
	m_blinkType = type;
}

inline void CShowObject::SetColor(DWORD color)
{
	m_color = color;
}

inline void CShowObject::SetTM(imc::matrix44& worldTM)
{
	m_worldTM = worldTM;
}

inline imc::matrix44& CShowObject::GetTM()
{
	return m_worldTM;
}

inline void	CShowObject::SetVisible(BOOL isVisible)
{
	m_isVisible = isVisible;
}

inline BOOL	CShowObject::IsVisible()
{
	return m_isVisible;
}

inline void CShowObject::ToggleVisible()
{
	m_isVisible = !m_isVisible;
}


class CDummyObject : public CShowObject
{
public:

	enum ACTIONTYPE
	{
		ACTIONTYPE_STOP = 0,
		ACTIONTYPE_SCALE,
		ACTIONTYPE_SPIN,
		ACTIONTYPE_ROTATE,
		ACTIONTYPE_CHASE,	
		ACTIONTYPE_RANDOM
	};

	CDummyObject();
	virtual			~CDummyObject();

	virtual	void	Update(float elapsedTime);

	void			SetActionType(ACTIONTYPE moveType);
	ACTIONTYPE		GetActionType();
	const char*		GetActionTypeName();

	void			SetActionSpeed(float speed);
	float			GetActionSpeed();

	void			SetTarget(CShowObject* pTarget);

protected:

	CShowObject*	m_pTarget;

	ACTIONTYPE		m_actionType;
	float			m_actionSpeed;
	float			m_rotateRadius;

};

inline void CDummyObject::SetActionType(ACTIONTYPE moveType)
{
	imc::MatrixIdentity(&m_worldTM);

	if (m_actionType != moveType)
	{
		m_actionType = moveType;
	}
}

inline CDummyObject::ACTIONTYPE CDummyObject::GetActionType()
{
	return m_actionType;
}

inline void CDummyObject::SetActionSpeed(float speed)
{
	m_actionSpeed = speed;
}

inline float CDummyObject::GetActionSpeed()
{
	return m_actionSpeed;
}

inline void	CDummyObject::SetTarget(CShowObject* pTarget)
{
	m_pTarget = pTarget;
}


class CTargetObject : public CShowObject
{
public:

	CTargetObject();
	virtual			~CTargetObject();

	virtual	void	Update(float elapsedTime);

	void			SetRandomMoving(BOOL isRandomMoving);
	BOOL			IsRandomMoving();

	void			SetMoveSpeed(float speed);
	float			GetMoveSpeed();

	void			SetBasePos(float x, float y, float z);
	imc::vector3&	GetBasePos();

protected:

	imc::vector3	m_basePos;
	imc::vector3	m_offsetPos;

	BOOL			m_isRandomMoving;
	float			m_moveSpeed;

	imc::vector3	m_moveDir;
	float			m_moveProcessTime;
	float			m_moveCycleTime;
	
};

inline void CTargetObject::SetRandomMoving(BOOL isRandomMoving)
{
	m_isRandomMoving = isRandomMoving;
}

inline BOOL CTargetObject::IsRandomMoving()
{
	return m_isRandomMoving;
}

inline void	CTargetObject::SetMoveSpeed(float speed)
{
	m_moveSpeed = speed;
}

inline float CTargetObject::GetMoveSpeed()
{
	return m_moveSpeed;
}

inline void	CTargetObject::SetBasePos(float x, float y, float z)
{
	m_basePos = imc::vector3(x, y, z);
}

inline imc::vector3& CTargetObject::GetBasePos()
{
	return m_basePos;
}


};
