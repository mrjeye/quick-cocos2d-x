//
//  NetworkUtil.h
//  game
//
//  Created by mrj on 12/17/13.
//
//

#ifndef __game__NetworkUtil__
#define __game__NetworkUtil__

#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC_EXT;
USING_NS_CC;

#include "WebSocket.h"

struct SocketResponse {
    
};

//class SocketUtilDelegate {
//    
//public:
//    virtual void onResponse(const SocketResponse& response) {};
//};

class NetworkUtil : public WebSocket::Delegate {
protected:
    WebSocket* m_webSocket;
    
    std::vector<int> m_funIDs;
public:
    bool init();
    
    static NetworkUtil* getInstance();
    
    static bool connect();
    static void sendRequest(std::string data, int nFunID);
    
protected:
    bool _connect();
    void _sendRequest(std::string data, int nFunID);
    
protected:
    // delegate
    virtual void onOpen(WebSocket* ws);
    virtual void onMessage(WebSocket* ws, const WebSocket::Data& data);
    virtual void onClose(WebSocket* ws);
    virtual void onError(WebSocket* ws, const WebSocket::ErrorCode& error);
};

#endif /* defined(__game__NetworkUtil__) */
