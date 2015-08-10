#include <windows.h>


HANDLE m_RecvThreadHandle = NULL; 
HINSTANCE m_DllhInstDll = NULL;
CHAR* m_recvBuf = NULL;
DWORD m_RecvBytes = 0;
volatile BOOL m_IsClose = FALSE;
HWND m_RecvHwnd = NULL;
UINT m_RecvMessageNumber = 0;
HANDLE m_SerialHandle = NULL;
BOOL m_SerialOpen = FALSE;

#define BUF_SIZE 1024*1024

#define FUNC_OK 1
#define FUNC_FAILED -1


DWORD WINAPI ThreadProc(LPVOID pParam){

	DWORD retValue = FUNC_FAILED;
	DWORD bResult = 0;
    DWORD dwErr = 0;
	OVERLAPPED ReadOver={0};
    DWORD hMask;
    COMSTAT comstat;
	DWORD Flag = 0;
	DWORD res = 0;

	try{
		//申请读取空间
		if (m_recvBuf == NULL){
			m_recvBuf = new CHAR[BUF_SIZE+1];
		}
		//创建异步信号
		memset(&ReadOver,0x00,sizeof(OVERLAPPED));
		ReadOver.Internal = 0;
		ReadOver.InternalHigh = 0;
		ReadOver.Offset = 0;
		ReadOver.OffsetHigh = 0;
		ReadOver.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		//清空串口数据
		PurgeComm(m_SerialHandle,PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
		

		//::PostMessage(m_RecvHwnd,m_RecvMessageNumber,NULL,NULL);

		for (; !m_IsClose ;){
			
			bResult = WaitCommEvent(m_SerialHandle,&hMask,&ReadOver);
			if (m_SerialOpen){
				if (!bResult){
					switch (dwErr = GetLastError())
					{
					case ERROR_IO_PENDING:{
						Flag ^= 1;
											}break;
					default:{
						retValue = FUNC_FAILED;
						goto ErrorNext;
							}break;
					}
				}
				else{
					bResult = ClearCommError(m_SerialHandle,&dwErr,&comstat);
					if (comstat.cbInQue == 0)
						continue;
				}

				res = WaitForSingleObject(ReadOver.hEvent,5000);
				if (res == WAIT_TIMEOUT){
					continue;
				}
				Flag = 0;
				GetCommMask(m_SerialHandle,&hMask);
				if (hMask & EV_RXCHAR){
					m_RecvBytes = 0;
					if (!ReadFile(m_SerialHandle,m_recvBuf,BUF_SIZE,&m_RecvBytes,&ReadOver)){
						switch (dwErr = GetLastError())
						{
						case ERROR_IO_PENDING:
							Flag = 0; break;
						default:
							goto ErrorNext;
						}
					}
					else{
						Flag = 1;
					}
					if (Flag & 1 && m_RecvBytes != 0){
						m_recvBuf[m_RecvBytes] = '\0';
						for (DWORD i = 0; i < m_RecvBytes; i++){
							if (m_recvBuf[i] == 0x00)
								m_recvBuf[i] = ' ';
						}
						::PostMessage(m_RecvHwnd,m_RecvMessageNumber,(WPARAM)&m_RecvBytes,(LPARAM)m_recvBuf);
						Sleep(1000);
					}
				}
ErrorNext:
				Flag=0;
			}
		}
		retValue = FUNC_OK;
	}
	catch(...){
		retValue = FUNC_FAILED;
	}
	if (m_recvBuf != NULL){
		delete[] m_recvBuf;
		m_recvBuf = NULL;
	}
	CloseHandle(ReadOver.hEvent);
	//::PostMessage(m_RecvHwnd,m_RecvMessageNumber,NULL,NULL);
	return retValue;
}

#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) INT Opencom(CHAR *Com,HWND hwnd,UINT MessageNumber){
		INT retValue = FUNC_FAILED;
		COMMTIMEOUTS TimeOuts;
		DCB SerialConfig = {0};
		try{
			//打开串口
			m_SerialHandle = CreateFile(Com,
				GENERIC_READ|GENERIC_WRITE,
				0,NULL,OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
				NULL);
			if (m_SerialHandle == INVALID_HANDLE_VALUE){
				goto ERROR_RET;
			}
			m_SerialOpen = TRUE;
			//设定缓冲区
			SetupComm(m_SerialHandle,BUF_SIZE,BUF_SIZE);
			//设定超时系统
			TimeOuts.ReadIntervalTimeout = MAXDWORD;
			TimeOuts.ReadTotalTimeoutMultiplier = 0;
			TimeOuts.ReadTotalTimeoutConstant = 0;
			TimeOuts.WriteTotalTimeoutMultiplier = 100;
			TimeOuts.WriteTotalTimeoutConstant = 500;
			if (!SetCommTimeouts(m_SerialHandle,&TimeOuts)){
				goto CLOSE_SERIALHANDLE;
			}
			//设置串口的配置
			if (!GetCommState(m_SerialHandle,&SerialConfig)){
				goto CLOSE_SERIALHANDLE;
			}
			SerialConfig.DCBlength = sizeof(DCB);
			SerialConfig.BaudRate = CBR_115200;
			SerialConfig.fBinary = TRUE;
			SerialConfig.fParity = NOPARITY;
			SerialConfig.StopBits = ONESTOPBIT;
			SerialConfig.ByteSize = 8;
			SerialConfig.fRtsControl = 0;
			if (!SetCommState(m_SerialHandle,&SerialConfig)){
				goto CLOSE_SERIALHANDLE;
			}
			//清空缓冲区
			PurgeComm(m_SerialHandle,PURGE_TXCLEAR|PURGE_RXCLEAR);
			//设置监听事件
			if (!SetCommMask(m_SerialHandle,EV_RXCHAR)){
				goto CLOSE_SERIALHANDLE;
			}
			//设定接收窗口的信息
			m_RecvHwnd = hwnd;
			m_RecvMessageNumber = MessageNumber;
			//创建接收线程
			m_RecvThreadHandle = CreateThread(NULL,
				0,ThreadProc,NULL,0,NULL);
			if (m_RecvThreadHandle == INVALID_HANDLE_VALUE){
				goto CLOSE_SERIALHANDLE;
			}
			return FUNC_OK;
		}
		catch (...){
			goto ERROR_RET;
		}
CLOSE_SERIALHANDLE:
		CloseHandle(m_SerialHandle);
		m_SerialHandle = INVALID_HANDLE_VALUE;
ERROR_RET:
		retValue = FUNC_FAILED;
		return retValue;
	}

	__declspec(dllexport) INT WriteComm(CHAR* buf,DWORD WriteSize){
		INT retValue = FUNC_FAILED;
		OVERLAPPED WriteOver = {0};
		BOOL bWrite = FALSE;
		DWORD factWriteBytes = 0;
		DWORD res = 0;
		try{
			if (m_SerialHandle == INVALID_HANDLE_VALUE || m_SerialOpen == FALSE){
				goto RETURN_VALUE;
			}
			buf[WriteSize] = '\0';
			WriteOver.Offset = 0;
			WriteOver.OffsetHigh = 0;
			WriteOver.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
			bWrite = WriteFile(m_SerialHandle,buf,WriteSize,&factWriteBytes,&WriteOver);
			if (bWrite){
				goto RETURN_VALUE;
			}
			else if (GetLastError() == ERROR_IO_PENDING){
				WaitForSingleObject(WriteOver.hEvent,2000);
			}
			else{
				retValue = FUNC_FAILED;
				goto RETURN_VALUE;
			}
			retValue = FUNC_OK;
		}
		catch(...){
			retValue = FUNC_FAILED;
		}
RETURN_VALUE:
		CloseHandle(WriteOver.hEvent);
		return retValue;
	}

	__declspec(dllexport) INT CloseComm(){
		INT retValue = FUNC_FAILED;
		try{
			//先判断线程是否在运行
			if (m_RecvThreadHandle != INVALID_HANDLE_VALUE){
				m_IsClose = TRUE;
				WaitForSingleObject(m_RecvThreadHandle,INFINITE);
				CloseHandle(m_RecvThreadHandle);
				m_RecvThreadHandle = INVALID_HANDLE_VALUE;
			}
			if (m_SerialHandle != INVALID_HANDLE_VALUE){
				CloseHandle(m_SerialHandle);
				m_SerialOpen = FALSE;
				m_SerialHandle = INVALID_HANDLE_VALUE;
			}
			m_RecvHwnd = NULL;
			m_RecvMessageNumber = 0;
			m_RecvBytes = 0;
			retValue = FUNC_OK;
		}
		catch (...){
			retValue = FUNC_FAILED;
		}
		return retValue;
	}

#ifdef __cplusplus
}
#endif

BOOL WINAPI DllMain(HINSTANCE hInstDll,DWORD fdwReason,LPVOID lpvReserved){

	m_DllhInstDll = hInstDll;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:{
		m_RecvThreadHandle = INVALID_HANDLE_VALUE;
		m_recvBuf = NULL;
		m_IsClose = FALSE;
		m_RecvHwnd = NULL;
		m_RecvMessageNumber = 0;
		m_SerialHandle = INVALID_HANDLE_VALUE;
		m_SerialOpen = FALSE;
		m_RecvBytes = 0;
							}break;
	case DLL_PROCESS_DETACH:{
		CloseComm();
							}break;
	}

	return TRUE;
}