#include "stdafx.h"
#include "DB_Event.h"

DB_Event::DB_Event()
{
}

DB_Event::DB_Event(DB_EVENT_TYPE type) :m_type(type)
{
}

DB_Event::DB_Event(DB_EVENT_TYPE type, int ownerId) :m_type(type), m_ownerId(ownerId)
{
}

DB_Event::DB_Event(DB_EVENT_TYPE type, int ownerId, void* buffer) :m_type(type), m_ownerId(ownerId)
{
	switch (type)
	{
	case EV_GET_PLAYER_INFO:
	case EV_ADD_NEW_USER:
	{
		memcpy(m_buffer, buffer, sizeof(DB::DB_PlayerId));
	}

	break;
	default:
		break;
	}
}

DB_Event::DB_Event(const DB_Event& other) :m_type(other.m_type), m_ownerId(other.m_ownerId)
{
	switch (other.m_type)
	{
	case EV_GET_PLAYER_INFO:
	case EV_ADD_NEW_USER:
	{
		memcpy(m_buffer, other.m_buffer, sizeof(DB::DB_PlayerId));
	}

	break;
	default:
		break;
	}
}

DB_Event::DB_Event(DB_Event&& other) :m_type(other.m_type), m_ownerId(other.m_ownerId)
{
	switch (other.m_type)
	{
	case EV_GET_PLAYER_INFO:
	case EV_ADD_NEW_USER:
	{
		memcpy(m_buffer, other.m_buffer, sizeof(DB::DB_PlayerId));
	}

	break;
	default:
		break;
	}
}

DB_Event& DB_Event::operator=(DB_Event& other)
{
	m_type = other.m_type;
	m_ownerId = other.m_ownerId;
	switch (other.m_type)
	{
	case EV_GET_PLAYER_INFO:
	case EV_ADD_NEW_USER:
	{
		memcpy(m_buffer, other.m_buffer, sizeof(DB::DB_PlayerId));
	}

	break;
	default:
		break;
	}
	return *this;
}

DB_Event& DB_Event::operator=(DB_Event&& other)
{
	m_type = other.m_type;
	m_ownerId = other.m_ownerId;
	switch (other.m_type)
	{
	case EV_GET_PLAYER_INFO:
	case EV_ADD_NEW_USER:
	{
		memcpy(m_buffer, other.m_buffer, sizeof(DB::DB_PlayerId));
	}

	break;
	default:
		break;
	}
	return *this;
}
