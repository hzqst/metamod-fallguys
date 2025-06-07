#pragma once

#include <extdll.h>			// always
#include <meta_api.h>		// of course
#include <edict.h>
#include <const.h>
#include <enginecallback.h>

class CBaseEntity;

class EHANDLE
{
private:
	edict_t* m_pent;
	int		m_serialnumber;
public:
	EHANDLE()
	{
		m_pent = nullptr;
		m_serialnumber = 0;
	}

	EHANDLE(edict_t * pEnt)
	{
		if (pEnt)
		{
			m_pent = pEnt;
			if (m_pent)
				m_serialnumber = m_pent->serialnumber;
			else
				m_serialnumber = 0;
		}
		else
		{
			m_pent = NULL;
			m_serialnumber = 0;
		}
	}

	edict_t* Get() const
	{
		if (m_pent)
		{
			if (m_pent->serialnumber == m_serialnumber)
				return m_pent;
			else
				return nullptr;
		}
		return nullptr;
	}
	edict_t* Set(edict_t* pent)
	{
		m_pent = pent;
		if (pent)
			m_serialnumber = m_pent->serialnumber;
		return pent;
	}

	operator bool() const
	{
		return Get() != NULL;
	}

	operator edict_t *() const
	{
		return Get();
	}

	edict_t* operator = (edict_t* pEnt)
	{
		if (pEnt)
		{
			m_pent = pEnt;
			if (m_pent)
				m_serialnumber = m_pent->serialnumber;
		}
		else
		{
			m_pent = NULL;
			m_serialnumber = 0;
		}
		return pEnt;
	}
	edict_t* operator ->() const
	{
		return Get();
	}
};
