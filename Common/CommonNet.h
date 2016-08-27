#ifndef __COMMON_NET_H__
#define __COMMON_NET_H__

#include "ostype.h"
#include "CommonDef.h"

typedef struct _tagWGDHEAD
{
	int32_t				nParentCmd;
	int32_t				nSubCmd;
	uint32_t			nDataLen;
}WGDHEAD;

typedef struct _tagSendData
{
	net_handle_t		fd;
	PVOID				pData;
	uint32_t			len;
}SendData;

typedef struct _tagLoginData
{
	int8_t szUserName[MAX_NAME_LEN];
	int8_t szPassPort[MAX_NAME_LEN];
	int8_t szReserved[4];
}LoginData;

typedef struct _tagHallSvrInfo
{
	int8_t szIp[MAX_IP_LEN];
	short  port;
	int8_t szReserved[4];
}HallSvrInfo;

typedef struct _tagLoginResponse
{
	int nResponseCode;
	int8_t* pData;
	int     nLen;
	int nReserved[4];
}LoginResponse;

typedef enum _eNumLoginResponse
{
	e_Null_Error = -1,
	e_login_ok = 0,
	e_register_ok = 1,
	e_UserName_notExist,
	e_PassPort_Error,
	e_UserName_hasExist,
	e_PassPort_tooEasy,
	e_Database_Error,
};

#define BASE_REQ_MSG			(10000)
#define BASE_RESPONSE_MSG		(2000000)

#define LR_REQ_LOGIN			(BASE_REQ_MSG+1)
#define LR_REQ_REGISTER			(BASE_REQ_MSG+2)
#define LR_HALLSVR_REG			(BASE_REQ_MSG+3)

#define LR_REP_LOGIN			(BASE_RESPONSE_MSG+1)
#define LR_REP_REGISTER			(BASE_RESPONSE_MSG+2)


#endif
