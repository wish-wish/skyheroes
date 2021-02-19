
local MainScene = class("MainScene", cc.load("mvc").ViewBase)

local GameCards = require("apps.mapheart.views.GameCards")
local bit=require("bytepack.BitOpr")
local ba=require("bytepack.ByteArray")
local bav=require("bytepack.ByteArrayVarint")
local ws=require("apps.mapheart.views.WebSocket")

MainScene.addheart=false;
MainScene.delta_time=0;
MainScene.hearts={};

function MainScene:onCreate()
    self.cards=GameCards:new();
    self:addhearts();
    self:helloworld();
    --self:clickSprite();
    ws:testProto();
    self:packTest();
end

function MainScene:testByteArray()
    local bya=ba.new();
    bya:writeUInt(10);
end

function MainScene:packTest()
    print("-----packTest------------------------------Start")
    local bpack=string.pack
    local bunpack=string.unpack
    
    local function hex(s)
        s=string.gsub(s,"(.)",function (x) return string.format("%02X",string.byte(x)) end)
        return s
    end
    
    local a=bpack("Ab8","\027Lua",5*16+1,0,1,4,4,4,8,0)
    print(hex(a),string.len(a))
    print(bunpack(a,"bA3b8"));
    
    local b=string.dump(hex)
    local b=string.sub(b,1,string.len(a))
    print(a==b,string.len(b))
    print(bunpack(b,"bA3b8"))
    
    local i=314159265 
    local f="<I>I=I"
    local a=bpack(f,i,i,i)
    print(hex(a))
    print(bunpack(a,f))
    
    local i=3.14159265 
    local f="<d>d=d"
    local a=bpack(f,i,i,i)
    print(hex(a))
    print(bunpack(a,f))

    print("-----packTest------------------------------End")
end

function MainScene:addhearts()
    self.count=0;
    local onecount,max_count=3,3000;--一帧增加个数,最大精灵数
    --local bn=ccs.BatchNode:create();
    local tex=cc.TextureCache:getInstance():addImage("heart/heart1.png");
    local bn=cc.SpriteBatchNode:createWithTexture(tex);
    self:addChild(bn);
    ---[[
    self:scheduleUpdate(function(delta)
        self.delta_time=self.delta_time+delta;
        if math.modf(self.delta_time)>0 then
            --self.cards:testCards();
            self.delta_time=self.delta_time-math.modf(self.delta_time);
        end
        if MainScene.addheart then
            if self.count<max_count  then
                self.count=self.count+onecount;
                for i=1,onecount do
                    local sp=rad.SpriteWithHue:createWithTexture(tex):setScale(0.1);
                    sp:setHue(2 * 3.1415926535 * math.random( 0,10 )/10)
                    sp._start=cc.p(math.random(0,display.width),math.random(120,display.height-120));
                    sp:setPosition(sp._start);
                    self:addChild(sp);
                    --bn:addChild(sp);--使用批次
                    --sp:runAction(cc.RepeatForever:create(cc.OrbitCamera:create(math.random(1,10)/10,100,0,0,90,0,0)));
                    ---[[
                    local rand_time=math.random(2,10)/3;
                    local move=cc.MoveBy:create(rand_time,cc.p(display.width+100,0));
                    local rotate=cc.RotateBy:create(rand_time,360);    
                    local seq=cc.Sequence:create(cc.Spawn:create(move,rotate),cc.Place:create(cc.p(-100,sp._start.y)));
                    sp:runAction(cc.RepeatForever:create(seq));
                    --]]
                    table.insert(MainScene.hearts,sp);
                end
            end
        else
            for i=1,table.getn(MainScene.hearts) do
                MainScene.hearts[i]:removeFromParent();
            end
            MainScene.hearts={};
            self.count=0;
        end 
    end);
    --]]
end

function MainScene:helloworld()
    ---[[
    local heart=display.newSprite("heart/heart1.png")
        :move(display.width-1024*0.2, display.height - 908*0.2):setScale(0.4)
        :addTo(self)
    --heart.move(display.width-heart:getContentSize().width/2,display.height-heart:getContentSize().height/2);
    -- add HelloWorld label
    cc.Label:createWithSystemFont("千心同屏", "Arial", 40)
        :move(display.width-1024*0.25, display.height - 40)
        :setColor({r = 255,g = 0, b = 0, a = 255})
        :addTo(self)
    --]]
    --[[
    //this.flash_sprite.runAction(cc.repeatForever(cc.orbitCamera(1, 64, 0, 0, 90, 0, 0)));//左右
    //this.flash_sprite.runAction(cc.repeatForever(cc.orbitCamera(1, 64, 0, 270, 90, 0, 0)));//左右
    //this.flash_sprite.runAction(cc.repeatForever(cc.sequence(cc.orbitCamera(1, 64, 0, 0, 90, -90, 0),cc.orbitCamera(1, 64, 0, 270, 90, -90, 0))));//上下
    //this.flash_sprite.runAction(cc.repeatForever(cc.sequence(cc.orbitCamera(1, 64, 0, 0, 90, -90, 0))));//上下
    //this.flash_sprite.runAction(cc.repeatForever(cc.sequence(cc.orbitCamera(1, 64, 0, 270, 90, -90, 0))));//上下
    --]]
    --[[
    local widget = ccui.Layout:Layout();
    widget:setTouchEnabled(true);
    widget:addTouchEventListener(function(sender,type)
        print("attack");
    end);
    widget:move(display.cx,100);
    --widget:setBackGroundColor(cc.Color(255,255,255,255));
    --widget:setBackGroundColorType(ccui.Layout.BG_COLOR_SOLID);
    widget:setBackGroundImage("attack.png",0);
    widget:addTo(self);
    ]]
    local button = ccui.Button:create("heart/heart2.png", "", "", 0);
    button:move(display.right-85,display.top_center.y-73);
    button:addTouchEventListener(function(sender,type)
        --print("attack:"..type);
        if type==2 then
           MainScene.addheart=not MainScene.addheart;
            print("generate:"..tostring(MainScene.addheart));
            --self:sort_test();
        end
    end);
    button:addTo(self);
end

function MainScene:clickSprite()
    local function onTouchBegan(touch, event)
        local target = event:getCurrentTarget()
        local locationInNode = target:convertToNodeSpace(touch:getLocation())
        local s = target:getContentSize()
        local rect = cc.rect(0, 0, s.width, s.height)
        if cc.rectContainsPoint(rect, locationInNode) then
            print(string.format("sprite began... x = %f, y = %f", locationInNode.x, locationInNode.y))
            target:setOpacity(180)
            return true
        end
        return false
    end
    local function onTouchMoved(touch, event)
        local target = event:getCurrentTarget()
        local posX,posY = target:getPosition()
        local delta = touch:getDelta()
        --print("sprite onTouchMoved.."..delta.x);
        local s = target:getContentSize()
        local rect = cc.rect(0, 0, s.width, s.height)
        local locationInNode = target:convertToNodeSpace(touch:getLocation())
        if cc.rectContainsPoint(rect, locationInNode) then
            print("Move in"..delta.x);
        else
            print("Move out"..delta.x);
        end
    end
    local function onTouchEnded(touch, event)
        local target = event:getCurrentTarget()
        local delta = touch:getDelta()
        local s = target:getContentSize()
        local rect = cc.rect(0, 0, s.width, s.height)
        local locationInNode = target:convertToNodeSpace(touch:getLocation())
        if cc.rectContainsPoint(rect, locationInNode) then
            print("in onTouchesEnded.."..delta.x);
        else
            print("out onTouchesEnded.."..delta.x);
        end
        target:setOpacity(255)
    end
    local function onTouchCancel(touch, event)
        local target = event:getCurrentTarget()
        print("sprite onTouchCancel..")
        target:setOpacity(255)
    end
    local sprite=cc.Sprite:create("city/attack.png");
    self:addChild(sprite);
    sprite:move(display.cx-200,100);
    local listener = cc.EventListenerTouchOneByOne:create()
    listener:setSwallowTouches(true)
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
    listener:registerScriptHandler(onTouchCancel,cc.Handler.EVENT_TOUCH_CANCELLED )
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, sprite)
end

function MainScene:ffi( ... )
    --http://luajit.org/extensions.html
    --userdata,lightuserdata--C指针
    --ffi.ct(ffi.cdecl,ffi.ctype,ffi.cdata)
    --ffi.cb
    --ffi.VLA,ffi.VLS(variable-length array,variable-length struct is a struct)
    --ffi.cdef[[
        --def
    --]]
    --ffi.C.*--c func
end

return MainScene
