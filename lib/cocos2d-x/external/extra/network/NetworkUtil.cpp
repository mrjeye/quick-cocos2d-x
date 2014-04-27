//
//  NetworkUtil.cpp
//  game
//
//  Created by mrj on 12/17/13.
//
//

#include "NetworkUtil.h"
#include "network/WebSocket.h"
#include "cocos2dx_extra.h"
#include "CSContentJsonDictionary.h"

#include "CCCrypto.h"

#define kMaxDataLen 1024

#define DEBUG_NETWORK_UTIL true

static CCLuaValue convertToLuaDict(const CSJson::Value& root);
static CCLuaValue convertToLuaDict(const char* root);

bool NetworkUtil::init() {
    m_webSocket = NULL;
    
    m_currentSerialNumber = 1;
    m_signKey = "";
    
    m_scheduler = new CCScheduler();
    
    m_scheduler->scheduleSelector(schedule_selector(NetworkUtil::onTick), this, 1.0f, 0, .0f, false);
    CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(m_scheduler, 0, false);
    m_scheduler->release();
    
    return true;
}

static NetworkUtil* gSocketUtil = NULL;
NetworkUtil* NetworkUtil::getInstance() {
    if (NULL == gSocketUtil) {
        gSocketUtil = new NetworkUtil();
        
        gSocketUtil->init();
        
//        gSocketUtil->connect();
    }
    
    return gSocketUtil;
}

bool NetworkUtil::connect() {
    return NetworkUtil::getInstance()->_connect();
}

void NetworkUtil::sendRequest(const char* serviceName, const char* requestData, LUA_FUNCTION nFunID) {
    NetworkUtil::getInstance()->_sendRequest(serviceName, requestData, nFunID);
}

void NetworkUtil::sendRequest(const char* serviceName, const char* requestData/* , LUA_FUNCTION nFunID */) {
    NetworkUtil::sendRequest(serviceName, requestData, 0);
}

bool NetworkUtil::_connect() {
    if (m_webSocket != NULL && WebSocket::kStateOpen == m_webSocket->getReadyState()) {
        return true;
    }
    
    m_webSocket = new WebSocket();
    
    return m_webSocket->init(*this, "ws://115.182.52.252:8080/jiyu/ws");
}

void NetworkUtil::_sendRequest(const char* serviceName, const char* requestData, LUA_FUNCTION nFunID) {
    if (m_webSocket->getReadyState() != WebSocket::State::kStateOpen) {
        sendEvent(NetworkEventOnError, CCLuaValue::stringValue("Server not connected"));
        
        // just call lua callback!!
        CCLuaStack* luaStack = CCLuaEngine::defaultEngine()->getLuaStack();
        
        luaStack->pushCCLuaValue(convertToLuaDict("{\"_r\":\"-1\",\"_m\":\"服务器尚未连接\"}"));
        
        luaStack->executeFunctionByHandler(nFunID, 1);
        
        return;
    }
    
    int rsn = m_currentSerialNumber++;  // request sn.
    
    if (0 != nFunID) {
        m_funIDs.insert(std::pair<SERIAL_NUMBER, LUA_FUNCTION>(rsn, nFunID));
    }
    
    std::string _requestData = requestData;
    size_t last = 0;
    size_t index = _requestData.find_first_of("&",last);
    
    std::vector<std::string> params;
    while (index != std::string::npos) {
        params.push_back(_requestData.substr(last,index-last));
        last = index+1;
        index = _requestData.find_first_of("&",last);
    }
    
    if (index - last > 0) {
        params.push_back(_requestData.substr(last,index-last));
    }
    
    cs::CSJsonDictionary* _dictData = new cs::CSJsonDictionary();
    
    // require params
    _dictData->insertItem("n", serviceName);
    char _bsn[64] = {0};
    sprintf(_bsn, "%d", rsn);
    _dictData->insertItem("s", _bsn);
    //////////////
    
    cs::CSJsonDictionary _reqParams;
    for (int index = 0; index < params.size(); index ++) {
        std::string paramItem = params[index];
        size_t eIndex = paramItem.find_first_of("=", 0);
        if (eIndex == std::string::npos) {
            // ignore invalid param item.
            
            continue;
        }
        
        std::string _key = paramItem.substr(0, eIndex);
        std::string _value = paramItem.substr(eIndex + 1, paramItem.length());
        
        _reqParams.insertItem(_key.c_str(), _value.c_str());
    }
    
    _dictData->insertItem("p", &_reqParams);
    
    std::string __requestData = _dictData->getDescription();
    
    // -- 解析器
    CSJson::Value root;
    CSJson::Reader reader;
    CSJson::FastWriter fastWriter;
    ///////////
    
    reader.parse(__requestData, root);
    __requestData = fastWriter.write(root); // 请求数据(未加入MD5签名前)
    __requestData = __requestData.substr(0, __requestData.size() - 1);  // ignore the last space
    
    // add sign key
    __requestData = m_signKey + __requestData;
    
    std::string sign = cocos2d::extra::CCCrypto::MD5String((void*)__requestData.c_str(), (int)__requestData.length());
    
    _dictData->insertItem("m", sign.c_str());
    
    reader.parse(_dictData->getDescription(), root);
    __requestData = fastWriter.write(root); // 请求数据(已加入MD5签名)
    __requestData = __requestData.substr(0, __requestData.size() - 1);
    
#if DEBUG_NETWORK_UTIL
    CCLog("NetworkUtil --> SENDDING: ==========\n%s\n==========", _dictData->getDescription().c_str());
#endif
    
    delete _dictData;
    
    m_webSocket->send(__requestData.c_str());
}

void NetworkUtil::onOpen(WebSocket* ws) {
    CCLOG("SocketUtil:: onOpen\n");
    
    sendEvent(NetworkEventOnOpen);
}

void NetworkUtil::onMessage(WebSocket* ws, const WebSocket::Data& data) {
    char* respBuff = new char[data.len + 1];
    memset(respBuff, 0, sizeof(data.len));
    strcpy(respBuff, data.bytes);
    
#if DEBUG_NETWORK_UTIL
    CCLog("NetworkUtil --> ON MESSAGE: ==========\n%s\n==========", respBuff);
#endif
    
    // -- 解析器
    CSJson::Value root;
    CSJson::Reader reader;
    
    do {
        if (!reader.parse(respBuff, root)) {
            // parse response error.
            
            break;
        }
        
        CCLuaValue responseValue =  convertToLuaDict(root["p"]);
        
        sendEvent(NetworkEventOnMessage, responseValue);
        
        // find callback funID
        CSJson::Value s = root["s"];
        if (!s.isString() || s.asString().length() <= 0) {
            // error sn.
            
            break;
        }
        
        SERIAL_NUMBER rsn = atoi(s.asCString());
        if (rsn <= 0) {
            // bad sn.
            
            break;
        }
        
        LUA_FUNCTION _callbackFuncID = 0;
        
        std::map<SERIAL_NUMBER, LUA_FUNCTION>::iterator iter;
        
        for(iter = m_funIDs.begin(); iter != m_funIDs.end(); iter++) {
            if (iter->first == rsn) {
                _callbackFuncID = iter->second;
                
                break;
            }
        }
        
        if (_callbackFuncID <= 0) {
            // unknown function ID.
            
            break;
        }
        
        // just call lua callback!!
        CCLuaStack* luaStack = CCLuaEngine::defaultEngine()->getLuaStack();
        
        luaStack->pushCCLuaValue(responseValue);
        
        luaStack->executeFunctionByHandler(_callbackFuncID, 1);
        
        // remove when response successed.
        m_funIDs.erase(_callbackFuncID);
    } while ( 0 );
}

// convert to lua dict
static CCLuaValue convertToLuaDict(const char* root) {
    CSJson::Reader reader;
    CSJson::Value jRoot;
    
    if (!reader.parse(root, jRoot)) {
        const char* msg_tmplate = "__ERROR_PARSE_JSON__(\n%s\n)";
        
        unsigned long mblen = strlen(root) + strlen(msg_tmplate);
        char* mbuff = new char[mblen];
        memset(mbuff, 0, mblen);
        
        sprintf(mbuff, msg_tmplate, root);
        
        return CCLuaValue::stringValue(mbuff);
    }
    
    return convertToLuaDict(jRoot);
}

static CCLuaValue convertToLuaDict(const CSJson::Value& root) {
    // std::map<std::string, CCLuaValue>
    CCLuaValue lResult;
    ////////
    
    switch (root.type()) {
        case CSJson::nullValue:
            lResult = CCLuaValue::floatValue(false);
            
            break;
        case CSJson::intValue:
        case CSJson::uintValue:
            lResult = CCLuaValue::intValue(root.asInt());
            
            break;
        case CSJson::realValue:
            lResult = CCLuaValue::intValue(root.asFloat());
            
            break;
        case CSJson::stringValue:
            lResult = CCLuaValue::stringValue(root.asString());
            
            break;
        case CSJson::booleanValue:
            lResult = CCLuaValue::booleanValue(root.asBool());
            
            break;
        case CSJson::arrayValue: {
            CCLuaValueArray rResultArr;
            for (int aIndex = 0; aIndex < root.size(); aIndex ++) {
                rResultArr.push_back(convertToLuaDict(root));
            }
            
            lResult = CCLuaValue::arrayValue(rResultArr);
            
            break;
        }
        case CSJson::objectValue:
            CCLuaValueDict iValueDict;
            
            CSJson::Value::Members members = root.getMemberNames();
            for (int index = 0; index < members.size(); index ++) {
                std::string key = members[index];
                
                iValueDict.insert(std::pair<std::string, CCLuaValue>(key, convertToLuaDict(root[key])));
            }
            
            lResult = CCLuaValue::dictValue(iValueDict);
            
            break;
    }
    
    return lResult;
}

void NetworkUtil::onClose(WebSocket* ws) {
    CCLOG("SocketUtil:: onClose\n");
    
    sendEvent(NetworkEventOnClose);
}

void NetworkUtil::onError(WebSocket* ws, const WebSocket::ErrorCode& error) {
    std::string eMsg;
    
    switch (error) {
        case WebSocket::kErrorTimeout:
            eMsg = "Timeout";
            
            break;
        case WebSocket::kErrorConnectionFailure:
            eMsg = "Connection failure";
            
            break;
        case WebSocket::kErrorUnknown:
        default:
            eMsg = "Unknown";
            
            break;
    }
    
    CCLOG("SocketUtil:: onError(%s)\n", eMsg.c_str());
    
    sendEvent(NetworkEventOnError, CCLuaValue::stringValue(eMsg));
}

void NetworkUtil::sendEvent(NetworkEvent event) {
    sendEvent(event, CCLuaValue::stringValue(""));
}

void NetworkUtil::sendEvent(NetworkEvent event, const CCLuaValue& data) {
    for (int index = ((int)m_globalDelegates.size() - 1); index >= 0; index --) {
        CCLuaEngine::defaultEngine()->getLuaStack()->pushInt(event);
        CCLuaEngine::defaultEngine()->getLuaStack()->pushCCLuaValue(data);
        
        int ret = CCLuaEngine::defaultEngine()->getLuaStack()->executeFunctionByHandler(m_globalDelegates[index], 2);
        if (0 != ret) {
            removeGlobalDelegate(m_globalDelegates[index]);
        }
    }
}

void NetworkUtil::onTick() {
    
}
