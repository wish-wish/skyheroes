--author:cjoy
local GameCards=class("GameCards")
local bit=require("bytepack.BitOpr")
local ba=require("bytepack.ByteArray")
local bav=require("bytepack.ByteArrayVarint")

--五小牛＞炸弹牛＞五花牛＞四花牛＞牛牛＞有牛＞无牛

local poker_values={
    --服务端
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,--方块 A - K
    0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,--梅花 A - K
    0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,--红桃 A - K
    0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,--黑桃 A - K
    0x41,0x42
    ,--单牌
    0x11,0x21,0x31,0x41,0x51,0x61,0x71,0x81,0x91,0xA1,0xB1,0xC1,0xD1,--方块 A - K
    0x12,0x22,0x32,0x42,0x52,0x62,0x72,0x82,0x92,0xA2,0xB2,0xC2,0xD2,--梅花 A - K
    0x13,0x23,0x33,0x43,0x53,0x63,0x73,0x83,0x93,0xA3,0xB3,0xC3,0xD3,--红桃 A - K
    0x14,0x24,0x34,0x44,0x54,0x64,0x74,0x84,0x94,0xA4,0xB4,0xC4,0xD4,--黑桃 A - K
    0xE1,0xE2
    ,--计分值
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0A,0x0A,0x0A,--方块 A - K
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0A,0x0A,0x0A,--梅花 A - K
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0A,0x0A,0x0A,--红桃 A - K
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0A,0x0A,0x0A,--黑桃 A - K
    0x0A,0x0A
    ,--计花值
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,--方块 A - K
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,--梅花 A - K
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,--红桃 A - K
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,--黑桃 A - K
    0x01,0x01    
    ,--图片
    "Red_A","Red_2","Red_3","Red_4","Red_5","Red_6","Red_7","Red_8","Red_9","Red_10","Red_J","Red_Q","Red_K",--方块 A - K
    "Black_A","Black_2","Black_3","Black_4","Black_5","Black_6","Black_7","Black_8","Black_9","Black_10","Black_J","Black_Q","Black_K",--梅花 A - K
    "Red_A","Red_2","Red_3","Red_4","Red_5","Red_6","Red_7","Red_8","Red_9","Red_10","Red_J","Red_Q","Red_K",--红桃 A - K
    "Black_A","Black_2","Black_3","Black_4","Black_5","Black_6","Black_7","Black_8","Black_9","Black_10","Black_J","Black_Q","Black_K",--黑桃 A - K    
    "Black_K","Red_K"--无大小王，用黑桃k和红桃k替代
};

local pic_prifx="Img_Card";
local pic_card="Img_BackPoker";
local pic_back="Img_CardBack";
local spell_prifx="Img_SpellTen";
local poker_pictures={
    "Img_CardBlock",
    "Img_CardGrassFlower",
    "Img_CardRedHeart",
    "Img_CardSpade",
    "Img_CardSpade",--王
    "Img_CardBlock01",
    "Img_CardGrassFlower_01",
    "Img_CardRedHeart_01",
    "Img_CardSpade_01",
    "Img_CardSpade_01"--王
}

local poker_times={
      1--散牌、10带1～10带6
     ,2--10带7~10带9
     ,3--双十
     ,4--炸弹
     ,5--五花
     ,8--五小
};

GameCards.TEN_ERR   = -1--无此牌型
GameCards.TEN_WUNIU = 0 --无牛,散牌
GameCards.TEN_NIU1  = 1 --牛1，10带1
GameCards.TEN_NIU2  = 2 --牛2，10带2
GameCards.TEN_NIU3  = 3 --牛3，10带3
GameCards.TEN_NIU4  = 4 --牛4，10带4
GameCards.TEN_NIU5  = 5 --牛5，10带5
GameCards.TEN_NIU6  = 6 --牛6，10带6
GameCards.TEN_NIU7  = 7 --牛7，10带7
GameCards.TEN_NIU8  = 8 --牛8，10带8
GameCards.TEN_NIU9  = 9 --牛9，10带9
GameCards.TEN_NN    = 10 --牛牛，双十
--GameCards.TEN_XWN   = 11 --小王牛
--GameCards.TEN_DWN   = 12 --大王牛
--GameCards.TEN_SHN   = 13 --四花牛(银牛)
GameCards.TEN_WHN   = 14 --五花牛(金牛)
GameCards.TEN_ZD    = 15 --炸弹
GameCards.TEN_WXN   = 16 --五小牛（<5,<=10）

GameCards.userBets={};
GameCards.SpellPrefix="Img_SpellTen";
GameCards.SpellTenFmt={
"SP_%02d"
,"01_%02d","02_%02d","03_%02d","04_%02d","05_%02d","06_%02d","07_%02d","08_%02d","09_%02d"
,"SS_%02d"
,"XW_%02d","DW_%02d","SH_%02d"
,"WH_%02d"
,"ZD_%02d"
,"WX_%02d"
};

function GameCards:ctor()
	
end

function GameCards:clearBets()
    self.userBets={};
end

function GameCards:addBets(data)
    --[[--成功
    {k = "wChairID", t = "word"},						--用户位置
	{k = "cbJettonArea", t = "byte"},					--筹码区域
	{k = "lJettonScore", t = "score"},					--加注数目	
	{k = "bIsAndroid", t = "bool"},						--是否机器人
	{k = "bAndroid", t = "bool"},						--机器标识
    ]]
    --[[--失败
    {k = "wPlaceUser", t = "word"},						--下注玩家
	{k = "lJettonArea", t = "byte"},					--下注区域
	{k = "lPlaceScore", t = "score"},					--当前下注
    {k = "bReason", t = "byte"},					    --失败原因
    ]]    
    local userbets=self:getUserBets(data.wChairID);
    table.insert(userbets.bets,data);
    userbets.total=userbets.total+data.lJettonScore;
    userbets.secs[data.cbJettonArea]=userbets.secs[data.cbJettonArea]+data.lJettonScore;
    return userbets;
end

function GameCards:getUserBets(chairid)
    for k,v in pairs(self.userBets) do
        if v.wcharid==chairid then
            return v;
        end
    end
    local userbets={wcharid=chairid,total=0,secs={0,0,0,0},results={0,0,0,0},coins={1,1,1,1},bets={}};
    table.insert(self.userBets,userbets);
    return userbets;
end

function GameCards:getCardIndex(val)--牌索引
    for i=1,54 do
        if poker_values[i]==val then
            return i
        end
    end
    return -1;--error
end

function GameCards:getCardFlower(val)--颜色
    return bit.band(val, 0xF0)/16
end

function GameCards:testCards()
    local cards=self:randomTensCards();
    local utype,_ucards,utimers,umax=self:getCardTypes(cards);
    print(string.format("%02x:%02d,%02x:%02d,%02x:%02d,%02x:%02d,%02x:%02d---type=%d,timers=%d"
        ,_ucards[1].sort_val,_ucards[1].count_val,_ucards[2].sort_val,_ucards[2].count_val
        ,_ucards[3].sort_val,_ucards[3].count_val,_ucards[4].sort_val,_ucards[4].count_val
        ,_ucards[5].sort_val,_ucards[5].count_val,utype,utimers));
end

function GameCards:randomTensCards()
    local cards=self:randomCards(5,52);
    return cards;
end

function GameCards:randomCards(count,max)
    local function isInList(list,card)
        for i,v in pairs(list) do
            if card==v then
                return true;
            end
        end
        return false;
    end
    if count>max then count=max end
    local cards={};
    local indexs={};
    for i=1,count do
        local index=math.random(1,max);
        while isInList(indexs,index) do
            index=math.random(1,max);
        end
        table.insert(indexs,index);
        table.insert(cards,poker_values[index]);
    end
    return cards;
end

function GameCards:getCardPicture(val)--图片
    local val=tonumber(val);
    local flower=self:getCardFlower(val);
    local index=self:getCardIndex(val);
    local picobj={};
    picobj.back="Poker/Img_BackPoker.png";
    picobj.card="Poker/Img_CardBack.png";
    picobj.flower=string.format("Poker/%s.png",poker_pictures[flower+1]);
    picobj.tinyflower=string.format("Poker/%s.png",poker_pictures[flower+1+5]);
    picobj.val=string.format("%s/%s%s.png","Poker",pic_prifx,poker_values[index+54*4]);
    return picobj;
end

function GameCards:getCardPic(data)--图片    
    local picobj={};
    picobj.back="Poker/Img_BackPoker.png";
    picobj.card="Poker/Img_CardBack.png";    
    if data.index>0 then
        picobj.flower=string.format("Poker/%s.png",poker_pictures[data.hua_val+1]);
        picobj.tinyflower=string.format("Poker/%s.png",poker_pictures[data.hua_val+1+5]);
        picobj.val=string.format("%s/%s%s.png","Poker",pic_prifx,poker_values[data.index+54*4]);
    end
    return picobj;
end

function GameCards:sortCards(cardDatas)--排序
    local sortVals={}
    for i,val in pairs(cardDatas) do
        local card={};
        if val>0 then        
            card.val=val;--服务器牌值
            local index=self:getCardIndex(val);
            card.index=index;
            card.hua_val = bit.band(val, 0xF0)/16;
            card.card_val=bit.band(val, 0x0F);--牌值A~K
            card.sort_val=poker_values[index+54];--排序值
            card.count_val=poker_values[index+108];--计数值1~10
            card.hua_count=poker_values[index+162];--是否j,q,k,王
        else
            card.val=val;
            card.index=0;
            card.hua_val =0;
            card.card_val=0;
            card.sort_val=0;
            card.count_val=0;
            card.hua_count=0;
        end
        table.insert(sortVals,card);
    end
    table.sort(sortVals,function(it1,it2)
        return it1.sort_val<it2.sort_val 
    end);
    return sortVals;
end

function GameCards:compareCards(ucards,zcards) --牌型比较，服务器值
    local utype,_ucards,utimers,umax=self:getCardTypes(ucards);
    local ztype,_zcards,ztimers,zmax=self:getCardTypes(zcards);
    if utype==ztype then
        return umax.sort_val-zmax.sort_val>0,utype,utimers,_ucards,umax,ztype,ztimers,_zcards,zmax;
    else
        return utype-ztype>0,utype,utimers,_ucards,umax,ztype,ztimers,_zcards,zmax;
    end
end

function GameCards:getCardTypes(cardDatas) --获取牌型
    local sortVals=self:sortCards(cardDatas);
    if not self:checkError(cardDatas) then    
        return self:getNiuTypes(sortVals);
    else
        return self.TEN_ERR,sortVals,0,sortVals[5];
    end
end

function GameCards:checkError(cardDatas)
    for k,v in pairs(cardDatas) do
        if v<0x01 or v>0x42 then
            return true
        elseif v>0x0D and v<0x11 then
            return true
        elseif v>0x1D and v<0x21 then
            return true
        elseif v>0x2D and v<0x31 then
            return true
        elseif v>0x3D and v<0x41 then
            return true
        end
    end
    if #cardDatas~=5 then
        return true
    end
    return false;    
end

function GameCards:getNiuTypes(sortVals)--获取牌型
    local sum=0;
    for k,card in pairs(sortVals) do
        sum=sum+card.count_val;
    end    
    if sum<=10 and self:isWuXiaoNiu(sortVals) then--五小
        return self.TEN_WXN,sortVals,poker_times[6],sortVals[5];
    end
    if self:isWuHuaNiu(sortVals) then--五花
        return self.TEN_WHN,sortVals,poker_times[5],sortVals[5];
    end
    if self:isZhaDan1(sortVals) then--炸弹
        return self.TEN_ZD,sortVals,poker_times[4],sortVals[3];
    end
    for i=1,#sortVals do
        for j=i+1,#sortVals do
            if math.mod(sum-sortVals[i].count_val-sortVals[j].count_val,10)==0 then
                local count=sortVals[i].count_val+sortVals[j].count_val;
                local val=math.mod(count,10);
                local tmp={};
                for k=1,#sortVals do if k~=i and k~=j then table.insert(tmp,sortVals[k]); end end
                local max=sortVals[5];
                sortVals[1],sortVals[2],sortVals[3],sortVals[4],sortVals[5]=tmp[1],tmp[2],tmp[3],sortVals[i],sortVals[j];
                if val==0 then
                    return self.TEN_NN,sortVals,poker_times[3],max;--双十
                else
                    if val>6 then
                        return val,sortVals,poker_times[2],max;--十带7～十带9
                    else
                        return val,sortVals,poker_times[1],max;--十带1～十带6
                    end
                end
            end
        end
    end    
    return self.TEN_WUNIU,sortVals,poker_times[1],sortVals[5];--散牌
end

function GameCards:isWuXiaoNiu(sortVals)--8--五小
    local isWuXiao=true;
    for i=1,#sortVals do
        if sortVals[i].count_val>=5 then
            isWuXiao=false;
        end
    end
    return isWuXiao;
end

function GameCards:isWuHuaNiu(sortVals)--5--五花
    local isWuHuaNiu=true;
    for i=1,#sortVals do
        if sortVals[i].hua_count<1 then
            isWuHuaNiu=false;
        end
    end
    return isWuHuaNiu;
end

function GameCards:isZhaDan(sortVals)--4--炸弹
    local count=1;
    local card_val=sortVals[1].card_val;
    for i=2,#sortVals do
        if card_val==sortVals[i].card_val then
            count=count+1;
        else
            if count>4 then
                return true;
            else
                count=1;
                card_val=sortVals[1].card_val;
            end
        end
    end
    if count>4 then
        return true;
    else
        return false;
    end
end

function GameCards:isZhaDan1(sortVals)--4--炸弹
    if sortVals[1].card_val==sortVals[4].card_val 
        or sortVals[2].card_val==sortVals[5].card_val then
            return true
    end
    return false;
end

function GameCards:dtor()
	
end

return GameCards;

