#include "stdafx.h"

#include "imcd3d9.h"

#include "App.h"
#include "AppObject.h"

namespace app
{

CShowObject::CShowObject()
{
	imc::MatrixIdentity(&m_worldTM);

	m_scale			= 5.0f;
	m_color			= 0xAAAAAAAA;
	m_blinkType		= BLINK_COLOR;

	m_isUpRatio		= TRUE;
	m_cycleTime		= 1;
	m_processTime	= 0.0f;

	m_pVertexBuffer	= NULL;
	m_pIndexBuffer	= NULL;	

	m_isVisible		= TRUE;
}

CShowObject::~CShowObject()
{	
	if ( m_pVertexBuffer ) {
		imcd3d9::GetDevice()->DestroyVertexBuffer(m_pVertexBuffer);
		m_pVertexBuffer = NULL;
	}
	if ( m_pIndexBuffer ) {
		imcd3d9::GetDevice()->DestroyIndexBuffer(m_pIndexBuffer);
		m_pIndexBuffer = NULL;
	}
}

void CShowObject::Create()
{
	imcd3d9::IDevice* pDev = imcd3d9::GetDevice();
	IMC_ASSERT(pDev);

	m_pVertexBuffer = pDev->CreateVertexBuffer(imcd3d9::PATCHNVERTEX, 6, TRUE);
	IMC_ASSERT(m_pVertexBuffer);

	imcd3d9::PatchNVertex* vertex = NULL;
	if ( m_pVertexBuffer->Lock((void**)&vertex) == TRUE ) 
	{
		vertex[0].z = 1.0f;		vertex[0].y = 0.0f;		vertex[0].x = 0.0f;
		vertex[0].nz = 1.0f;	vertex[0].ny = 0.0f;	vertex[0].nx = 0.0f;

		vertex[1].z = 0.0f;		vertex[1].y = 0.5f;		vertex[1].x = 0.0f;
		vertex[1].nz = 0.0f;	vertex[1].ny = 1.0f;	vertex[1].nx = 0.0f;

		vertex[2].z = 0.0f;		vertex[2].y = 0.0f;		vertex[2].x = 0.5f;
		vertex[2].nz = 0.0f;	vertex[2].ny = 0.0f;	vertex[2].nx = 1.0f;

		vertex[3].z = -0.5f;	vertex[3].y = 0.0f;		vertex[3].x = 0.0f;
		vertex[3].nz = -1.0f;	vertex[3].ny = 0.0f;	vertex[3].nx = 0.0f;

		vertex[4].z = 0.0f;		vertex[4].y = -0.5f;	vertex[4].x = 0.0f;
		vertex[4].nz = 0.0f;	vertex[4].ny = -1.0f;	vertex[4].nx = 0.0f;

		vertex[5].z = 0.0f;		vertex[5].y = 0.0f;		vertex[5].x = -0.5f;
		vertex[5].nz = 0.0f;	vertex[5].ny = 0.0f;	vertex[5].nx = -1.0f;

		m_pVertexBuffer->UnLock();
	}

	m_pIndexBuffer = pDev->CreateIndexBuffer(24, TRUE);
	IMC_ASSERT(m_pIndexBuffer);

	DWORD* indices = NULL;
	if (m_pIndexBuffer->Lock(&indices) == TRUE)
	{
		indices[0] = 1;		indices[1] = 5;		indices[2] = 3;
		indices[3] = 1;		indices[4] = 0;		indices[5] = 5;
		indices[6] = 1;		indices[7] = 2;		indices[8] = 0;
		indices[9] = 1;		indices[10] = 3;	indices[11] = 2;

		indices[12] = 4;	indices[13] = 3;	indices[14] = 5;
		indices[15] = 4;	indices[16] = 5;	indices[17] = 0;
		indices[18] = 4;	indices[19] = 0;	indices[20] = 2;
		indices[21] = 4;	indices[22] = 2;	indices[23] = 3;

		m_pIndexBuffer->UnLock();
	}
}

void CShowObject::Update(float elapsedTime)
{	
	m_processTime += elapsedTime;

	if (m_processTime > m_cycleTime)
	{
		m_isUpRatio		= !m_isUpRatio;
		m_processTime	= 0.0f;			
	}
}

void CShowObject::Render()
{
	if (m_pVertexBuffer == NULL || m_pIndexBuffer == NULL || m_isVisible == FALSE)
		return;

	imcd3d9::IDevice* pDev = imcd3d9::GetDevice();
	IMC_ASSERT(pDev);

	imc::matrix44 finalTM = m_worldTM;

	DWORD color			= m_color;
	float scaleOffset	= m_scale;	

	float ratio = min(1.0f, m_processTime / m_cycleTime);
	if (m_isUpRatio)
		ratio = 1 - ratio;
	
	switch (m_blinkType)
	{

	case BLINK_COLOR:
		{
			BYTE a = imc::GetAlphaFromColor(color);
			BYTE r = imc::GetRedFromColor(color);
			BYTE g = imc::GetGreenFromColor(color);
			BYTE b = imc::GetBlueFromColor(color);
			
			r += (BYTE)(ratio * (255 - r));
			g += (BYTE)(ratio * (255 - g));
			b += (BYTE)(ratio * (255 - b));

			color = (a << 24) + (r << 16) + (g << 8) + b;
		}
		break;

	case BLINK_SCALE:
		{
			scaleOffset += ratio * 2.0f;
		}
		break;
	}

	imc::matrix44 scaleTM;
	imc::MatrixScaling(&scaleTM, scaleOffset, scaleOffset, scaleOffset);
	imc::MatrixMultiply(&finalTM, &scaleTM, &m_worldTM);

	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);	
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDev->SetRenderState(D3DRS_TEXTUREFACTOR, color);

	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2); 
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2); 
	pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);	

	pDev->SetWorldTM(&finalTM);
	pDev->SetVertexFormat(imcd3d9::PATCHNVERTEX);

	pDev->SetVertexStream(0, m_pVertexBuffer, 0, imcd3d9::GetVertexSize(imcd3d9::PATCHNVERTEX));
	pDev->SetIndexStream(m_pIndexBuffer);
	pDev->SetTexture(0, NULL);

	pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 8);

	pDev->SetWorldTM(&imc::IdentityTM);	
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); 
	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); 	
}

CDummyObject::CDummyObject()
{
	m_actionType	= ACTIONTYPE_STOP;
	m_actionSpeed	= DEFAULT_ACTION_SPEED;
	m_rotateRadius	= 30.0f;

	m_pTarget		= NULL;
}

CDummyObject::~CDummyObject()
{
}

void CDummyObject::Update(float elapsedTime)
{
	CShowObject::Update(elapsedTime);

	static float angle = 0;
	static float spinAngle = 0;		

	static float scale = 1.0f;
	static BOOL	scaleUp = TRUE; 

	switch(m_actionType)
	{
	case ACTIONTYPE_STOP :
		angle		= 0;
		spinAngle	= 0;
		scale		= 1.0f;
		imc::MatrixIdentity(&m_worldTM);
		break;

	case ACTIONTYPE_SCALE:
		{
			if (scaleUp == TRUE)
			{
				scale += (m_actionSpeed * 0.01f * elapsedTime);

				if (scale >= 2.0f)
				{
					scale	= 2.0f;
					scaleUp = FALSE;
				}				
			}
			else
			{
				scale -= (m_actionSpeed * 0.01f * elapsedTime);

				if (scale <= 0.0f)
				{
					scale	= 0.0f;
					scaleUp = TRUE;
				}
			}

			imc::MatrixScaling(&m_worldTM, scale, scale, scale);
		}
		break;

	case ACTIONTYPE_SPIN:		
		{
			imc::MatrixRotationY(&m_worldTM, imc::DegToRad(spinAngle));
			spinAngle += elapsedTime * m_actionSpeed;
		}

		break;

	case ACTIONTYPE_ROTATE:		
		{
			imc::MatrixRotationY(&m_worldTM, imc::DegToRad(spinAngle));

			float x = m_rotateRadius * sinf(imc::DegToRad(spinAngle));
			float z = m_rotateRadius * cosf(imc::DegToRad(spinAngle));

			m_worldTM._41 = x;
			m_worldTM._42 = 0.0f;
			m_worldTM._43 = z;

			spinAngle += elapsedTime * m_actionSpeed;
		}

		break;

	case ACTIONTYPE_CHASE:
		{	
			if (m_pTarget != NULL)
			{
				imc::vector3 targetPos(m_pTarget->GetTM()._41, m_pTarget->GetTM()._42, m_pTarget->GetTM()._43);
				imc::vector3 pos(m_worldTM._41, m_worldTM._42, m_worldTM._43);
				imc::vector3 dir = targetPos - pos;

				if (abs(imc::Vec3Length(&dir)) < m_actionSpeed * elapsedTime)
				{
					m_worldTM._41 = targetPos.x;
					m_worldTM._42 = targetPos.y;
					m_worldTM._43 = targetPos.z;
				}
				else
				{				
					imc::Vec3Normalize(&dir, &dir);

					imc::MatrixLookAtLH(&m_worldTM, &imc::vOrigin, &dir, &imc::vUp);
					imc::MatrixInverse(&m_worldTM, 0, &m_worldTM);	
					dir *= (m_actionSpeed * elapsedTime);

					m_worldTM._41 = pos.x + dir.x;
					m_worldTM._42 = pos.y + dir.y;
					m_worldTM._43 = pos.z + dir.z;
				}
			}
		}
		break;

	case ACTIONTYPE_RANDOM:
		{
			static float processTime = 0.0f;
			processTime += elapsedTime;

			// NOTE : 0.2초 단위로 위치가 변경
			if (processTime >= 0.2f)
			{
				processTime = 0.0f;

				imc::vector3 randomPos;
				randomPos.x = (rand() % 4000 - 2000) * 0.01f;
				randomPos.y = (rand() % 4000 - 2000) * 0.01f;
				randomPos.z = (rand() % 4000 - 2000) * 0.01f;

				m_worldTM._41 = randomPos.x;
				m_worldTM._42 = randomPos.y;
				m_worldTM._43 = randomPos.z;

				angle += elapsedTime * m_actionSpeed;
			}
		}
		break;
	}
}

const char* CDummyObject::GetActionTypeName()
{
	switch(m_actionType)
	{
	case ACTIONTYPE_STOP: return "정지";	
	case ACTIONTYPE_SPIN: return "제자리 회전";		
	case ACTIONTYPE_ROTATE: return "원 운동";
	case ACTIONTYPE_CHASE: return "타겟 추적";
	case ACTIONTYPE_RANDOM: return "무작위 위치";
	}

	return NULL;
}

CTargetObject::CTargetObject()
{
	m_isRandomMoving	= FALSE;
	m_moveSpeed			= DEFAULT_TARGET_SPEED;
	m_basePos			= imc::vector3(0, 0, 500.0f);
	m_offsetPos			= imc::vector3(0, 0, 0);
	m_moveDir			= imc::vector3(1, 0, 0);
	m_moveProcessTime	= 100000.0f;
	m_moveCycleTime		= 2.0f;

	m_scale				= 10.0f;
}

CTargetObject::~CTargetObject()
{
	
}

void CTargetObject::Update(float elapsedTime)
{
	CShowObject::Update(elapsedTime);

	if (m_isRandomMoving)
	{
		if (m_moveProcessTime >= m_moveCycleTime)
		{
			m_moveDir.x = rand()% 1001 - 500;
			m_moveDir.y = rand()% 1001 - 500;
			m_moveDir.z = rand()% 1001 - 500;
			imc::Vec3Normalize(&m_moveDir, &m_moveDir);

			m_moveProcessTime = 0.0f;
		}

		m_offsetPos += (m_moveDir * m_moveSpeed * elapsedTime);
		m_moveProcessTime += elapsedTime;
	}
	else
	{
		m_offsetPos = imc::vOrigin;
	}
	
	imc::vector3 finalPos = m_basePos + m_offsetPos;
	m_worldTM._41 = finalPos.x;
	m_worldTM._42 = finalPos.y;
	m_worldTM._43 = finalPos.z;
}

} 