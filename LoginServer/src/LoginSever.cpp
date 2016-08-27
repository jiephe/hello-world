#include "LoginSever.h"

CLoginServer::CLoginServer(int8_t ip[], uint16_t port, int nThreadNum)
	:CWgdServer(ip, port, nThreadNum)
{

}

CLoginServer::~CLoginServer()
{

}

int CLoginServer::OnReceivedNotify(net_handle_t fd, void* pData, int len)
{
	WGDHEAD* pHead = (WGDHEAD*)pData;

	printf("Have Receive Data Cmd: %d\n", pHead->nSubCmd);

	switch (pHead->nSubCmd)
	{
	case LR_REQ_LOGIN:
		{
			OnMsgLogin(fd, pData, len);
		}
		break;
	case LR_REQ_REGISTER:
		{
			OnMsgRegister(fd, pData, len);
		}
		break;
	case LR_HALLSVR_REG:
		{
			OnHallSvrReg(fd, pData, len);
		}
		break;
	default:
		CWgdServer::OnReceivedNotify(fd, pData, len);
	}

	return 0;
}

int	CLoginServer::OnMsgLogin(net_handle_t fd, void* pData, int len)
{
	WGDHEAD* pHead = (WGDHEAD*)pData;

	LoginData* pLogin = (LoginData*)(pHead+sizeof(WGDHEAD));

	int nRet = FindInDataBase(pLogin->szUserName, pLogin->szPassPort);
	if (nRet == e_login_ok)
	{
		HallSvrInfo hsi;

		m_HallSvrList_lock.lock();
		int nRand = rand()%m_vecHallSvrList.size();
		HallSvrInfo* pInfo = &(m_vecHallSvrList[nRand]);
		memcpy(&hsi, pInfo, sizeof(HallSvrInfo));
		m_HallSvrList_lock.unlock();

		return SendResponse(fd, nRet, &hsi, sizeof(HallSvrInfo));
	}

	return SendResponse(fd, nRet);
}

int	CLoginServer::OnMsgRegister(net_handle_t fd, void* pData, int len)
{
	WGDHEAD* pHead = (WGDHEAD*)pData;

	LoginData* pLogin = (LoginData*)(pHead+sizeof(WGDHEAD));

	int nRet = WriteToDataBase(pLogin->szUserName, pLogin->szPassPort);

	return SendResponse(fd, nRet);
}

int	CLoginServer::OnHallSvrReg(net_handle_t fd, void* pData, int len)
{
	WGDHEAD* pHead = (WGDHEAD*)pData;

	HallSvrInfo* pLogin = (HallSvrInfo*)(pHead+sizeof(WGDHEAD));

	HallSvrInfo hsi;
	memcpy(&hsi, pLogin, sizeof(HallSvrInfo));

	m_HallSvrList_lock.lock();
	m_vecHallSvrList.push_back(hsi);
	m_HallSvrList_lock.unlock();

	std::string str;
	GetIniPath(str, HALL_SVR_INFO_LIST);
	char szKey[8];
	sprintf_s(szKey, sizeof(szKey), "%d", fd);
	char szIpPort[64];
	sprintf_s(szIpPort, sizeof(szIpPort), "%s|%d", pLogin->szIp, pLogin->port);
	WritePrivateProfileString(DATA_BASE_SECTION, szKey, szIpPort, str.c_str());

	return SendResponse(fd, e_register_ok);
}

int	CLoginServer::FindInDataBase(int8_t* pUserName, int8_t* pPassPort)
{
	std::string str;
	GetIniPath(str, LOGIN_DB_FILE_NAME);

	char szPassPort[MAX_PASSPORT_LEN];
	GetPrivateProfileString(DATA_BASE_SECTION, pUserName, "", szPassPort, sizeof(szPassPort), str.c_str());

	if (!stricmp(szPassPort, ""))
	{
		return e_UserName_notExist;
	}
	else
	{
		if (stricmp(pPassPort, szPassPort))
		{
			return e_PassPort_Error;
		}
		else
		{
			return e_login_ok;
		}
	}

	return e_Null_Error;
}

void CLoginServer::GetIniPath(std::string& dstStr, const char* pszIni)
{
	char szFile[MAX_PATH_LEN];
	GetModuleFileName(NULL, szFile, sizeof(szFile));
	int nLen = strlen(szFile);
	char* pszEnd = szFile + nLen;
	while(*pszEnd != '\\' && nLen > 0)
	{
		pszEnd--;
		nLen--;
	}
	std::string str(szFile);
	str = str.substr(0, nLen);

	dstStr = str + pszIni;
}

int	CLoginServer::WriteToDataBase(int8_t* pUserName, int8_t* pPassPort)
{
	std::string str;
	GetIniPath(str, LOGIN_DB_FILE_NAME);

	char szPassPort[MAX_PASSPORT_LEN];
	GetPrivateProfileString(DATA_BASE_SECTION, pUserName, "", szPassPort, sizeof(szPassPort), str.c_str());
	if (stricmp(szPassPort, ""))
	{
		return e_UserName_hasExist;
	}

	if (strlen(szPassPort) <= 6)
	{
		return e_PassPort_tooEasy;
	}

	WritePrivateProfileString(DATA_BASE_SECTION, pUserName, pPassPort, str.c_str());

	return e_register_ok;
}

int	CLoginServer::SendResponse(net_handle_t fd, int nResponseCode, void* pData, int len)
{
	WGDHEAD wh;
	memset(&wh, 0x0, sizeof(WGDHEAD));
	wh.nParentCmd	= LR_REP_LOGIN;
	wh.nSubCmd      = nResponseCode;
	wh.nDataLen		= len;

// 	LoginResponse lr;
// 	memset(&lr, 0x0, sizeof(LoginResponse));
// 	lr.nResponseCode = nResponseCode;

	int nTotalSize = sizeof(WGDHEAD) + wh.nDataLen;
	char* pSendData = new char[nTotalSize];
	memcpy(pSendData, &wh, sizeof(WGDHEAD));
	if (len > 0)
	{
		memcpy(pSendData+sizeof(WGDHEAD), pData, wh.nDataLen);
	}
	
	return SendNetData(fd, pSendData, nTotalSize);
}