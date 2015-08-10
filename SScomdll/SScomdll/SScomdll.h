#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) INT Opencom(CHAR *Com,HWND hwnd,UINT MessageNumber);
__declspec(dllexport) INT WriteComm(CHAR* buf,DWORD WriteSize);
__declspec(dllexport) INT CloseComm();

#ifdef __cplusplus
}
#endif