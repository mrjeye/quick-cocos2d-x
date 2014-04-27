//
//  NetworkUtil.cpp
//  game
//
//  Created by mrj on 12/17/13.
//
//

#include "NetworkUtil.h"
#include "network/WebSocket.h"

bool NetworkUtil::init() {
    m_webSocket = NULL;
    
    return true;
}

static NetworkUtil* gSocketUtil = NULL;
NetworkUtil* NetworkUtil::getInstance() {
    if (NULL == gSocketUtil) {
        gSocketUtil = new NetworkUtil();
        
        gSocketUtil->init();
        
        gSocketUtil->connect();
    }
    
    return gSocketUtil;
}

bool NetworkUtil::connect() {
    return NetworkUtil::getInstance()->_connect();
}

void NetworkUtil::sendRequest(std::string data, int nFunID) {
    NetworkUtil::getInstance()->_sendRequest(data, nFunID);
}

bool NetworkUtil::_connect() {
    if (m_webSocket != NULL && WebSocket::kStateOpen == m_webSocket->getReadyState()) {
        return true;
    }
    
    m_webSocket = new WebSocket();
    
    return m_webSocket->init(*this, "192.168.4.16:8888");
}

void NetworkUtil::_sendRequest(std::string data, int nFunID) {
    printf("SocketUtil:: sendRequest >> %s\n", data.c_str());
    
    m_funIDs.push_back(nFunID);
    
    m_webSocket->send(data);
}

void NetworkUtil::onOpen(WebSocket* ws) {
    printf("SocketUtil:: onOpen\n");
}

void NetworkUtil::onMessage(WebSocket* ws, const WebSocket::Data& data) {
    char* buff = new char[data.len + 1];
    memset(buff, 0, sizeof(data.len));
    strcpy(buff, data.bytes);
    
    printf("SocketUtil:: onMessage >> %s\n", buff);
}

void NetworkUtil::onClose(WebSocket* ws) {
    printf("SocketUtil:: onClose\n");
}

void NetworkUtil::onError(WebSocket* ws, const WebSocket::ErrorCode& error) {
    printf("SocketUtil:: onError(%d)\n", error);
}
