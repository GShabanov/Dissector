/***************************************************************************************\
*   File:                                                                              *
*       transport.h                                                                    *
*                                                                                      *
*   Abstract:                                                                          *
*       transport IPC connect                                                          *
*                                                                                      *
*   Author:                                                                            *
*       G Shabanov          11-Mar-2021                                                *
*                                                                                      *
*   Revision History:                                                                  *
/***************************************************************************************/
// $Id: $
#pragma once
#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

class ITransportDatagram
{
public:
    enum EDirection {
        clientToServer = 0,
        serverToClient = 1,
    };

private:
    typedef struct _MESSAGE_BUFFER
    {
        EDirection   direction;


    } MESSAGE_BUFFER, *PMESSAGE_BUFFER;

    PMESSAGE_BUFFER  m_messageBuffer;

public:
    ITransportDatagram(PVOID memory)
    { 
        m_messageBuffer = (PMESSAGE_BUFFER)memory;
    }

    EDirection GetDirection() {
        return m_messageBuffer->direction;
    }

    void SetDirection(EDirection direction) {
        m_messageBuffer->direction = direction;
    }


    friend class CTransportServer;
};

typedef struct _TRANSPORT_BINDING_CONTEXT {
    HANDLE    m_hServerEvent;
    HANDLE    m_hClientEvent;
    HANDLE    m_hTerminateEvent;

    HANDLE    m_hSharedMapping;
} TRANSPORT_BINDING_CONTEXT, *PTRANSPORT_BINDING_CONTEXT;


class CTransportServer
{
private:
    TRANSPORT_BINDING_CONTEXT m_BindingCtxLocal;
    TRANSPORT_BINDING_CONTEXT m_BindingCtxRemote;
public:
    CTransportServer()
    {
        memset(&m_BindingCtxLocal, 0, sizeof(TRANSPORT_BINDING_CONTEXT));
        memset(&m_BindingCtxRemote, 0, sizeof(TRANSPORT_BINDING_CONTEXT));
    }

    ITransportDatagram &operator() (const ITransportDatagram &datagram);

    BOOL  BindToClient(const HANDLE  hClientProcess);
    BOOL  WaitForDatagram() const;
    BOOL  SendDatagram(const ITransportDatagram &datagram);
    BOOL  GetDataGram(ITransportDatagram &datagram);
};

class CClientServer
{
private:
    TRANSPORT_BINDING_CONTEXT m_BindingCtx;
public:
    CClientServer(const TRANSPORT_BINDING_CONTEXT &context)
    {
        memcpy(&m_BindingCtx, &context, sizeof(TRANSPORT_BINDING_CONTEXT));
    }

    ITransportDatagram &operator() (const ITransportDatagram &datagram);

    BOOL  BindToClient(const HANDLE  hClientProcess);
    BOOL  WaitForDatagram() const;
    BOOL  SendDatagram(const ITransportDatagram &datagram);
    BOOL  GetDataGram(ITransportDatagram &datagram);
};


#endif // __TRANSPORT_H__
