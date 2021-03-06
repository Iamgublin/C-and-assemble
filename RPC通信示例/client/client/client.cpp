// client.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"..\..\server\server\rpc_h.h"
#include"..\..\server\server\rpc_c.c"
#include"..\..\server\server\commondef.h"

#pragma comment(lib, "Rpcrt4.lib")

#define RPC_REMOTE_IP L"192.168.56.129"
#define RPC_PORT_GUID L"{0AD3C2E4-C14D-48E1-9CAF-502CFD189EE0}"

int main()
{
    RPC_STATUS Status = RPC_S_OK;
    TCHAR * pszStringBinding = NULL;

#ifndef LISTEN_REMOTE

    Status = RpcStringBindingCompose(
        NULL,
        (RPC_WSTR)L"ncalrpc",
        NULL,
        (RPC_WSTR)RPC_PORT_GUID,
        NULL,
        (RPC_WSTR*)&pszStringBinding);
    if (RPC_S_OK != Status || !pszStringBinding) {
        return 0;
    }
    if (RPC_S_OK != RpcBindingFromStringBinding((RPC_WSTR)pszStringBinding, &RpcService_Binding)) {
        return 0;
    }

#else // !LISTEN_REMOTE

    RpcStringBindingCompose(NULL, RPC_WSTR(L"ncacn_ip_tcp"), RPC_WSTR(RPC_REMOTE_IP) /*NULL*/, RPC_WSTR(L"13521"), NULL, (RPC_WSTR*)&pszStringBinding);

    RpcBindingFromStringBinding((RPC_WSTR)pszStringBinding, &RpcService_Binding);
    if (RPC_S_OK != Status || !pszStringBinding) {
        return 0;
    }

#endif
    Param a = {};
    a.Type = 1;

    RpcTryExcept
    {
        HRESULT lRet = RpcCall(&a);
        printf("[client] SyncCall OK");
    }
    RpcExcept(1)
    {
         printf( "RPC Exception %d\n", RpcExceptionCode() );
    }
    RpcEndExcept

    RPC_ASYNC_STATE *asynState = new RPC_ASYNC_STATE;
    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    RpcAsyncInitializeHandle(asynState, sizeof(RPC_ASYNC_STATE));

    asynState->UserInfo = NULL;
    asynState->NotificationType = RpcNotificationTypeEvent;
    asynState->u.hEvent = hEvent;

    //After it is notified that the server has sent a reply, 
    //the client calls RpcAsyncCompleteCall with the asynchronous handle so that it can receive the reply.
    //When RpcAsyncCompleteCall has completed successfully, 
    //the Reply parameter points to a buffer that contains the return value of the remote function.
    //Any buffers supplied by the client program as[out] or [in, out] parameters to the asynchronous remote function contain valid data.
    RpcTryExcept
    {
        BOOL bRet = FALSE;
        RpcAsyncCall(asynState,&a);
        WaitForSingleObject(hEvent,INFINITE);
        printf("[client] AsyncCall End");

        RpcAsyncCompleteCall(asynState, &bRet);

    }
    RpcExcept(1)
    {
        printf("RPC Exception %d\n", RpcExceptionCode());
    }
    RpcEndExcept

    delete asynState;
    CloseHandle(hEvent);
    RpcStringFree((RPC_WSTR*)&pszStringBinding);
    RpcBindingFree(&RpcService_Binding);
    return 0;
}

void __RPC_FAR* __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_USER  midl_user_free(void __RPC_FAR *ptr)
{
    free(ptr);
}

