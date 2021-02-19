
#include "SpriteWithHue.h"

const GLchar* colorRotationShaderBody();
void xRotateMat(float mat[3][3], float rs, float rc);
void yRotateMat(float mat[3][3], float rs, float rc);
void zRotateMat(float mat[3][3], float rs, float rc);
void matrixMult(float a[3][3], float b[3][3], float c[3][3]);
void hueMatrix(GLfloat mat[3][3], float angle);
void premultiplyAlpha(GLfloat mat[3][3], float alpha);


SpriteWithHue::SpriteWithHue()
	:_enable(false),
	_selected(false),
	_callback(nullptr),
	_defineInherite(false)
{

}


SpriteWithHue::~SpriteWithHue()
{
	_callback = nullptr;
}

SpriteWithHue* SpriteWithHue::create(const std::string& filename)
{
    SpriteWithHue *sprite = new (std::nothrow) SpriteWithHue();
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithHue* SpriteWithHue::create(const std::string& filename, const Rect& rect)
{
    SpriteWithHue *sprite = new (std::nothrow) SpriteWithHue();
    if (sprite && sprite->initWithFile(filename, rect))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithHue* SpriteWithHue::createWithTexture(Texture2D *texture)
{
    SpriteWithHue *sprite = new (std::nothrow) SpriteWithHue();
    if (sprite && sprite->initWithTexture(texture))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithHue* SpriteWithHue::createWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
{
    SpriteWithHue *sprite = new (std::nothrow) SpriteWithHue();
    if (sprite && sprite->initWithTexture(texture, rect, rotated))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithHue* SpriteWithHue::createWithSpriteFrame(SpriteFrame *spriteFrame)
{
    SpriteWithHue *sprite = new (std::nothrow) SpriteWithHue();
    if (sprite && spriteFrame && sprite->initWithSpriteFrame(spriteFrame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteWithHue* SpriteWithHue::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
    
#if COCOS2D_DEBUG > 0
    char msg[256] = {0};
    sprintf(msg, "Invalid spriteFrameName: %s", spriteFrameName.c_str());
    CCASSERT(frame != nullptr, msg);
#endif
    
    return createWithSpriteFrame(frame);
}

bool SpriteWithHue::initWithTexture(Texture2D *texture, const Rect &rect, bool rotated)
{
    bool ret = Sprite::initWithTexture(texture, rect, rotated);
    if(ret)
    {
        setupDefaultSettings();
        initShader();
    }
    return ret;
}

bool SpriteWithHue::initWithTexture(Texture2D *texture)
{
    CCASSERT(texture != nullptr, "Invalid texture for sprite");
    
    Rect rect = Rect::ZERO;
    rect.size = texture->getContentSize();
    
    return initWithTexture(texture, rect);
}

bool SpriteWithHue::initWithTexture(Texture2D *texture, const Rect& rect)
{
    return initWithTexture(texture, rect, false);
}

bool SpriteWithHue::initWithSpriteFrame(SpriteFrame *spriteFrame)
{
    CCASSERT(spriteFrame != nullptr, "");
    
    bool bRet = initWithTexture(spriteFrame->getTexture(), spriteFrame->getRect());
    setSpriteFrame(spriteFrame);
    
    return bRet;
}

void SpriteWithHue::setupDefaultSettings()
{
    _hue = 0.0f;
}

void SpriteWithHue::initShader()
{
    auto glprogram = GLProgramCache::getInstance()->getGLProgram("hue_program");
    if(!glprogram)
    {
       glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, shaderBody());
       GLProgramCache::getInstance()->addGLProgram(glprogram, "hue_program");
    }
    auto glprogramstate = GLProgramState::create(glprogram);
    setGLProgramState(glprogramstate);
    updateColor();
}

const GLchar* SpriteWithHue::shaderBody()
{
    return colorRotationShaderBody();
}

void SpriteWithHue::updateColor()
{
    Sprite::updateColor();
    updateColorMatrix();
    updateAlpha();
}

void SpriteWithHue::hueUniformCallback(GLProgram *p, Uniform *u)
{
   glUniformMatrix3fv(u->location, 1, GL_FALSE, (GLfloat*)&_mat);
}

void SpriteWithHue::updateColorMatrix()
{
    hueMatrix(_mat, _hue);
    premultiplyAlpha(_mat, getAlpha());

    getGLProgramState()->setUniformCallback("u_hue", CC_CALLBACK_2(SpriteWithHue::hueUniformCallback, this));
}

void SpriteWithHue::updateAlpha()
{
    getGLProgramState()->setUniformFloat("u_alpha", getAlpha());
}

GLfloat SpriteWithHue::getAlpha()
{
    return _displayedOpacity/255.0f;
}

float SpriteWithHue::getHue()
{
    return _hue;
}

void SpriteWithHue::setHue(float hue)
{
    _hue = hue;
    updateColorMatrix();
}

bool SpriteWithHue::isClick(Touch* touch)
{
	auto camera = Camera::getVisitingCamera();
	return this->hitTest(touch->getLocation(), camera, nullptr);
}

bool SpriteWithHue::hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const
{
	Rect rect;
	rect.size = getContentSize();
	return isScreenPointInRect(pt, camera, getWorldToNodeTransform(), rect, p);
}


void SpriteWithHue::setEnable(bool is)
{
	if (_enable == is)
	{
		return;
	}
	_enable = is;
	if (_enable)
	{
		_touchListener = EventListenerTouchOneByOne::create();
		CC_SAFE_RETAIN(_touchListener);
		_touchListener->setSwallowTouches(false);
		_touchListener->onTouchBegan = CC_CALLBACK_2(SpriteWithHue::onTouchBegan, this);
		_touchListener->onTouchMoved = CC_CALLBACK_2(SpriteWithHue::onTouchMoved, this);
		_touchListener->onTouchEnded = CC_CALLBACK_2(SpriteWithHue::onTouchEnded, this);
		_touchListener->onTouchCancelled = CC_CALLBACK_2(SpriteWithHue::onTouchCancelled, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
		this->setHue(0);
	}
	else
	{
		_eventDispatcher->removeEventListener(_touchListener);
		this->setHue(2 * M_PI * 0.7);
		CC_SAFE_RELEASE_NULL(_touchListener);
	}
}


void SpriteWithHue::setSelect(bool is)
{
	_selected = is; 
	if (_selected)
	{
		this->setHue(2 * M_PI * 0.3);
	}
	else
	{
		this->setHue(0);
	}
}


bool SpriteWithHue::isInherite(Event* event)
{
	if (event->getEventData() != nullptr&&this->isDefineInherite())
	{
		std::string user((char*)event->getEventData());
		if (user.compare("cancel_inherite")==0)
		{
			return false;
		}
	}
	return true;
}

bool SpriteWithHue::onTouchBegan(Touch *touch, Event *aevent)
{
	//CCLOG("SpriteWithHue::onTouchBegan");
	if (!this->isInherite(aevent))
	{
		return false;
	}
	bool ret = true;
	if (_enable&&_callback!=nullptr&&this->isClick(touch))
	{
		this->old_selected = this->isSelect();
		this->setSelect(!this->isSelect());
		ret = _callback(this, 0, touch, aevent);
	}
	return ret;
}


void SpriteWithHue::onTouchMoved(Touch *touch, Event *aevent)
{
	if (!this->isInherite(aevent))
	{
		return ;
	}
	bool ret = true;
	if (_enable&&_callback != nullptr&&this->isClick(touch))
	{
		ret = _callback(this, 1, touch, aevent);
	}
}


void SpriteWithHue::onTouchEnded(Touch *touch, Event *aevent)
{
	if (!this->isInherite(aevent))
	{
		return;
	}
	bool ret = true;
	if (_enable&&_callback != nullptr&&this->isClick(touch))
	{
		this->setSelect(this->old_selected);
		ret = _callback(this, 2, touch, aevent);
	}
}


void SpriteWithHue::onTouchCancelled(Touch *touch, Event *aevent)
{
	if (!this->isInherite(aevent))
	{
		return;
	}
	bool ret = true;
	if (_enable&&_callback != nullptr&&this->isClick(touch))
	{
		this->setSelect(this->old_selected);
		ret = _callback(this, 3, touch, aevent);
	}
}

//shader

const GLchar * colorRotationShaderBody()
{
    return
    "                                                               \n\
    #ifdef GL_ES                                                    \n\
    precision mediump float;                                        \n\
    #endif                                                          \n\
    \n\
    varying vec2 v_texCoord;                                        \n\
    uniform mat3 u_hue;                                             \n\
    uniform float u_alpha;                                          \n\
    \n\
    void main()                                                     \n\
    {                                                               \n\
    vec4 pixColor = texture2D(CC_Texture0, v_texCoord);             \n\
    vec3 rgbColor = u_hue * pixColor.rgb;                           \n\
    gl_FragColor = vec4(rgbColor, pixColor.a * u_alpha);            \n\
    }                                                               \n\
    ";
}

void xRotateMat(float mat[3][3], float rs, float rc)
{
    mat[0][0] = 1.0;
    mat[0][1] = 0.0;
    mat[0][2] = 0.0;
    
    mat[1][0] = 0.0;
    mat[1][1] = rc;
    mat[1][2] = rs;
    
    mat[2][0] = 0.0;
    mat[2][1] = -rs;
    mat[2][2] = rc;
}

void yRotateMat(float mat[3][3], float rs, float rc)
{
    mat[0][0] = rc;
    mat[0][1] = 0.0;
    mat[0][2] = -rs;
    
    mat[1][0] = 0.0;
    mat[1][1] = 1.0;
    mat[1][2] = 0.0;
    
    mat[2][0] = rs;
    mat[2][1] = 0.0;
    mat[2][2] = rc;
}


void zRotateMat(float mat[3][3], float rs, float rc)
{
    mat[0][0] = rc;
    mat[0][1] = rs;
    mat[0][2] = 0.0;
    
    mat[1][0] = -rs;
    mat[1][1] = rc;
    mat[1][2] = 0.0;
    
    mat[2][0] = 0.0;
    mat[2][1] = 0.0;
    mat[2][2] = 1.0;
}

void matrixMult(float a[3][3], float b[3][3], float c[3][3])
{
    int x, y;
    float temp[3][3];
    
    for(y=0; y<3; y++) {
        for(x=0; x<3; x++) {
            temp[y][x] = b[y][0] * a[0][x] + b[y][1] * a[1][x] + b[y][2] * a[2][x];
        }
    }
    for(y=0; y<3; y++) {
        for(x=0; x<3; x++) {
            c[y][x] = temp[y][x];
        }
    }
}

void hueMatrix(GLfloat mat[3][3], float angle)
{
#define SQRT_2      sqrt(2.0)
#define SQRT_3      sqrt(3.0)
    
    float mag, rot[3][3];
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;
    
    // Rotate the grey vector into positive Z
    mag = SQRT_2;
    xrs = 1.0/mag;
    xrc = 1.0/mag;
    xRotateMat(mat, xrs, xrc);
    mag = SQRT_3;
    yrs = -1.0/mag;
    yrc = SQRT_2/mag;
    yRotateMat(rot, yrs, yrc);
    matrixMult(rot, mat, mat);
    
    // Rotate the hue
    zrs = sin(angle);
    zrc = cos(angle);
    zRotateMat(rot, zrs, zrc);
    matrixMult(rot, mat, mat);
    
    // Rotate the grey vector back into place
    yRotateMat(rot, -yrs, yrc);
    matrixMult(rot,  mat, mat);
    xRotateMat(rot, -xrs, xrc);
    matrixMult(rot,  mat, mat);
}

void premultiplyAlpha(GLfloat mat[3][3], float alpha)
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            mat[i][j] *= alpha;
        }
    }
}


MenuItemHue* MenuItemHue::create()
{
	MenuItemHue *ret = new (std::nothrow) MenuItemHue();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool MenuItemHue::initWithHueImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const ccMenuCallback& callback)
{
	Node *normalSprite = nullptr;
	Node *selectedSprite = nullptr;
	Node *disabledSprite = nullptr;

	if (normalImage.size() >0)
	{
		normalSprite = Sprite::create(normalImage);
	}

	if (selectedImage.size() > 0)
	{
		selectedSprite = Sprite::create(selectedImage);
	}
	else
	{
		selectedSprite = SpriteWithHue::create(normalImage);
		((SpriteWithHue*)selectedSprite)->setHue(2 * M_PI * 0.3);
	}

	if (disabledImage.size() > 0)
	{
		disabledSprite = Sprite::create(disabledImage);
	}
	else
	{
		disabledSprite = SpriteWithHue::create(normalImage);
		((SpriteWithHue*)disabledSprite)->setHue(2 * M_PI * 0.7);
	}
	return initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, callback);
}

CoinHue* CoinHue::create(std::string coinfile)
{
	CoinHue *ret = new (std::nothrow) CoinHue();
	if (ret && ret->init())
	{
		ret->m_file = coinfile;
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

void CoinHue::setCoinSprite(std::string coinfile)
{
	this->m_file;
}

bool CoinHue::initWithHueImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const ccMenuCallback& callback)
{
	bool ret=MenuItemHue::initWithHueImage(normalImage, selectedImage, disabledImage, callback);
	Size size = this->_normalImage->getContentSize();
	this->m_hue = SpriteWithHue::create(this->m_file);
	float scale = 0.2;
	this->m_hue->setScale(scale);
	Size si = this->m_hue->getContentSize();
	this->addChild(this->m_hue);
	this->m_hue->setPosition(Vec2(size.width / 2, size.height + si.height* scale / 3));
	return ret;
}
