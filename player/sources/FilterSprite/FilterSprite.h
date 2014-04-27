//
//  FilterSprite.h
//  game
//
//  过滤特定颜色的SPRITE
//  Created by mrj on 3/11/14.
//
//

#ifndef __game__FilterSprite__
#define __game__FilterSprite__

#include "cocos2d.h"
USING_NS_CC;

class FilterSprite : public CCSprite {
private:
    int			m_keep_a_Location;
    
    // 保留alpha
    CC_SYNTHESIZE(int, m_keep_Alpha, keepAlpha);
public:
    FilterSprite();
    virtual ~FilterSprite();
    
    static FilterSprite* create(const char *pszFileName);
    
protected:
    virtual void initShader();
    
    virtual void update(float delta);
};

#endif /* defined(__game__FilterSprite__) */
