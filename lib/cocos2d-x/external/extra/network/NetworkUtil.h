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
#include "CCLuaValue.h"
#include <algorithm>

typedef int SERIAL_NUMBER;

enum NetworkEvent {
    NetworkEventOnOpen = 1,
    NetworkEventOnClose = 2,
    NetworkEventOnError = 3,
    NetworkEventOnMessage = 4
};

//class SocketUtilDelegate {
//    
//public:
//    virtual void onResponse(const SocketResponse& response) {};
//};

class NetworkUtil : public CCObject, public WebSocket::Delegate {
private:
    CCScheduler* m_scheduler;
    
protected:
    WebSocket* m_webSocket;
    
    std::map<SERIAL_NUMBER, LUA_FUNCTION> m_funIDs;
    
    // 网络请求通知(onOpen/onMessage/onClose/onError)
    std::vector<LUA_FUNCTION> m_globalDelegates;
    
    std::string m_signKey;   // sign key
    
    int m_currentSerialNumber;
public:
    bool init();
    
    static NetworkUtil* getInstance();
    
    // connect
    static bool connect();
    
    // send data
    static void sendRequest(const char* serviceName, const char* requestData, LUA_FUNCTION nFunID);
    static void sendRequest(const char* serviceName, const char* requestData/* , LUA_FUNCTION nFunID */);
    
    static inline LUA_FUNCTION addGlobalDelegate(LUA_FUNCTION nFunID) {
        NetworkUtil::getInstance()->m_globalDelegates.push_back(nFunID);
        
        return nFunID;
    }
    
    static inline bool removeGlobalDelegate(int nFunID) {
        bool ret = false;
        
        std::vector<LUA_FUNCTION>& delegates = NetworkUtil::getInstance()->m_globalDelegates;
        
        std::vector<LUA_FUNCTION>::iterator fRes = std::find(delegates.begin(), delegates.end(), nFunID);
        if (fRes != delegates.end()) {
            delegates.erase(fRes);
            
            ret = true;
        }
        
        return ret;
    }
    
    // set sign_key
    static inline void setSignKey(const char* signKey) {
        NetworkUtil::getInstance()->m_signKey = signKey;
    }
    
protected:
    bool _connect();
    void _sendRequest(const char* serviceName, const char* requestData, LUA_FUNCTION nFunID);
    
protected:
    // delegate
    virtual void onOpen(WebSocket* ws);
    virtual void onMessage(WebSocket* ws, const WebSocket::Data& data);
    virtual void onClose(WebSocket* ws);
    virtual void onError(WebSocket* ws, const WebSocket::ErrorCode& error);
    
    void sendEvent(NetworkEvent event);
    void sendEvent(NetworkEvent event, const CCLuaValue& data);
private:
    // schedule
    void onTick();
};

#endif /* defined(__game__NetworkUtil__) */
