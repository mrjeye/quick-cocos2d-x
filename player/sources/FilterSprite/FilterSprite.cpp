//
//  FilterSprite.cpp
//  game
//
//  Created by mrj on 3/11/14.
//
//

#include "FilterSprite.h"

FilterSprite::FilterSprite() {
    m_keep_a_Location = 0;
    
    m_keep_Alpha = 250;
}

FilterSprite::~FilterSprite() {
    
}

FilterSprite* FilterSprite::create(const char *pszFileName) {
    FilterSprite* sp = new FilterSprite();
	sp->initWithFile(pszFileName);
	sp->autorelease();
	sp->initShader();
    
    sp->autorelease();
    
    return sp;
}

void  FilterSprite::initShader() {
	CCGLProgram* progrom = new CCGLProgram();
    
	char buff[2048] = {0};
	unsigned long size = 0;
	const char* c = (char*)CCFileUtils::sharedFileUtils()->getFileData("FilterSprite.fsh", "r", &size);
    
	memcpy(buff, c, size);
    
	progrom->initWithVertexShaderByteArray(ccPositionTextureA8Color_vert,buff);
	this->setShaderProgram(progrom);
	progrom->addAttribute(kCCAttributeNamePosition,kCCVertexAttrib_Position);
	progrom->addAttribute(kCCAttributeNameTexCoord,kCCVertexAttrib_TexCoords);
	progrom->link();
	progrom->updateUniforms();
    
	m_keep_a_Location = glGetUniformLocation(progrom->getProgram(), "keep_a");
    
	progrom->use();
	scheduleUpdate();
    
    progrom->release();
}

void FilterSprite::update(float delta) {
	getShaderProgram()->use();
    
	float ka = this->getkeepAlpha() / 255.f;
    
	glUniform1f(m_keep_a_Location, ka);
}