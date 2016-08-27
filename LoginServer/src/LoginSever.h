#ifndef  _LOGIN_SERVER_H_
#define  _LOGIN_SERVER_H_

#include "WgdServer.h"
#include <string>

#define  DATA_BASE_SECTION		"DataBase"

#define  LOGIN_DB_FILE_NAME		"\\UserList.ini"

#define  HALL_SVR_INFO_LIST		"\\HallSvrList.ini"

class CLoginServer : public CWgdServer
{
public:
	CLoginServer(int8_t ip[], uint16_t port, int nThreadNum);

	virtual ~CLoginServer();

public:
	virtual int			OnReceivedNotify(net_handle_t fd, void* pData, int len);

	virtual int			OnMsgLogin(net_handle_t fd, void* pData, int len);

	virtual int			OnMsgRegister(net_handle_t fd, void* pData, int len);

	virtual int			OnHallSvrReg(net_handle_t fd, void* pData, int len);

	virtual int			SendResponse(net_handle_t fd, int nResponseCode, void* pData = NULL, int len = 0);

	virtual int			FindInDataBase(int8_t* pUserName, int8_t* pPassPort);

	virtual int			WriteToDataBase(int8_t* pUserName, int8_t* pPassPort);

	virtual	void		GetIniPath(std::string& dstStr, const char* pszIni);

private:
	CLock						m_HallSvrList_lock;

	std::vector<HallSvrInfo>	m_vecHallSvrList;
};


#endif