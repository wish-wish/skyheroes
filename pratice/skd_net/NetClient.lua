--Lua 网络

--使用示例，网络先要知道结构，知道发送的命令字和push消息的命令字

--[[
local login=skdProtocols():qryIdProtocol(skdCmds().JSQY_LG_USR_LOGIN);--获取结构
login.m_nProtocolVersion=20111017;--修改数据
login.m_szName="name";
login.m_szPwd="pass";
login.m_szServer="szServer";
local function logincallback(cmdid,data) --回调函数
    if (cmdid==skdCmds().JSQY_LG_SVR_LOGIN) then --login result
        --print("JSQY_LG_SVR_LOGIN="..tostring(data.m_nCheckCode));  
    elseif (cmdid==skdCmds().JSQY_LG_SVR_PT_AUTH) then --auth result
        --print("JSQY_LG_SVR_PT_AUTH="..tostring(data.m_res));  
    elseif (cmdid==skdCmds().JSQY_LG_SVR_CREATE_ROLE_ACK) then --role ack
        print("no roes need created");  
    end
end
self:sendCBData(login,logincallback,"Login");--发送数据
self:registerCBHandler(skdCmds().JSQY_LG_SVR_PT_AUTH,logincallback,"Login");--注册push消息接收
self:registerCBHandler(skdCmds().JSQY_LG_SVR_CREATE_ROLE_ACK,logincallback,"Login");--注册push消息接收
--]]

local NetClient = class("NetClient");
local ByteArray=require("framework.cc.utils.ByteArray");
local ByteArrayVariant=require("framework.cc.utils.ByteArrayVarint");
local SocketTCP=require("framework.cc.net.SocketTCP");
local gscheduler=require("framework.schedulerbs");
local zlib=require("zlib");

local defServer="Game";
local userID=12;
local innerID=12;
local sockets={};
local tagIndex=0;
local enter_game_handler=nil;
local sendDataQueues={};
local packetIndex=1;
local lastHeart=0;

gcb_handlers={};

function NetClient:ctor()
        
end

function NetClient:dispose()
    
end

function NetClient:newTCP(name,ip,port)
    
    local tcpsocket=SocketTCP.new(ip,port,false);
    local netstruct={};
    netstruct.name=name;
    netstruct.ip=ip;
    netstruct.port=port;
    netstruct.socket=tcpsocket;
    netstruct.tagindex=tagIndex;
    tcpsocket:setName(name);
    sockets[name]=netstruct;

    tcpsocket:addEventListener("SOCKET_TCP_CONNECTED",handler(self,self.onStatus), self.tagIndex);
    tcpsocket:addEventListener("SOCKET_TCP_CLOSE", handler(self,self.onStatus), self.tagIndex);
    tcpsocket:addEventListener("SOCKET_TCP_CLOSED", handler(self,self.onStatus), self.tagIndex);
    tcpsocket:addEventListener("SOCKET_TCP_CONNECT_FAILURE", handler(self,self.onStatus), self.tagIndex);
    tcpsocket:addEventListener("SOCKET_TCP_DATA", handler(self,self.onRecvData), self.tagIndex);

    tagIndex=tagIndex+1;
    self.lastHeart = os.time();
end

function NetClient:connectFunc(name,status)    
    local function enterGameCallback(cmdid,data)
        --TODO:
        --[[
        JSQY_SVR_FRESHGIFT	=	0x81B3	--33203	--新手礼包信息
        JSQY_SVR_LEAVEMSG	=	0x81AF	--33199	--< 服务端发送留言的玩家集合
        JSQY_SVR_GET_ALL_TALISMAN	=	0x82DE	--33502	--初始化所有法宝数据
        JSQY_SVR_ROLEINFO_CHANGE	=	0x0043	--67	--< 发送用户信息变化
        JSQY_SVR_ENTERGAME	=	0x0057	--87	--< 服务器返回应答  

        JSQY_SVR_UPDATE_ROLE_PARAM	=	0x824D	--33357	--更新玩家变量
        JSQY_SVR_OFFLINE_EXP	=	0x823A	--33338	--< 返回离线经验        
        JSQY_SVR_CITY	=	0x82A1	--33441	--城战相关
        JSQY_SVR_QUERY_ALLLEFTTIMES	=	0x81C4	--33220	--服务端返回所有副本剩余次数的消息
        JSQY_SVR_DAREHERO_INIT_MAXINDEX	=	0x8271	--33393	--初始化英雄副本最大索引数据
        JSQY_SVR_SEND_FINISH_TASKTARGET_LIST	=	0xEA7A	--60026	--完成的任务列表
        JSQY_SVR_PLAYER_NOTIFY_MESSAGE	=	0x81FB	--33275	--玩家提示信息
        --26251 --未知命令
        ]]
        if cmdid==skdCmds().JSQY_SVR_ENTERGAME then
            skdClient():getCurrCharacter().gameData=data;
            enter_game_handler(cmdid,data);
        elseif cmdid==skdCmds().JSQY_SVR_ROLEINFO_CHANGE then
            
        else
            skdLog():logTFStr("enterGameCallback cmdid %d",cmdid);
        end        
    end
    if status=="SOCKET_TCP_CONNECTED" then        
        if name=="Game" then            
            --enter main game            
            local gatekey="?><!097098$#ASDLJKadgsdg@$#@!j88";                
            local gameopt=skdProtocols():qryIdProtocol(skdCmds().JSQY_USR_ENTERGAME);--获取结构
            gameopt.m_timestamp=os.time();
            local szgatekey=string.format("%d------%s",gameopt.m_timestamp,gatekey);
            gameopt.m_sskey=bs.bsShine:toMD5(szgatekey);
            gameopt.m_reqType=0;
            gameopt.m_txOpenKey="";
            gameopt.m_txPf="";
            gameopt.m_txPfKey="";                
            self:sendCBData(gameopt,enterGameCallback,"Game");--发送数据
            self:registerCBHandler(skdCmds().JSQY_SVR_ROLEINFO_CHANGE,enterGameCallback,"Game");
            self:registerCBHandler(skdCmds().JSQY_SVR_ENTERGAME,enterGameCallback,"Game");
            skdLog():logTFStr("send enter game");
        else
            skdLog():logTFStr("name="..name..";status="..status);
        end
    else
        skdLog():logTFStr("name="..name..";status="..status);
    end        
end

function NetClient:enterGame(ip,port,callback)
    enter_game_handler=callback;
    self:newTCP("Game", ip, port);    
    self:connect("Game",handler(NetClient,NetClient.connectFunc));   
end

function NetClient:connect(name,acbfunc)
    if sockets[name]~=nil then
        local ip=sockets[name].ip;
        local port=sockets[name].port;
        sockets[name].socket:connect(ip, port, false);
        sockets[name].cb_func=acbfunc;
        sockets[name].status="SOCKET_TCP_CONNECTING";
        skdLog():logTFStr("connect to "..name..":"..ip..":"..tostring(port));
    end
end

function NetClient:update(delta)
    --TODO:断线重连
    --TODO:保存链接   
    local function callback(data,status)
        skdLog():logTFStr(status);
    end
    if table.getn(sendDataQueues)>0 then
        local data=sendDataQueues[1];
        if data==nil then
            return;
        end
        if self:getStatus(data.iname)~="SOCKET_TCP_CONNECTED" then
            --主动链接
            self:connect(name,acbfunc);
            return;
        end
        if self:getStatus(data.iname)=="SOCKET_TCP_CONNECTED" then            
            --发送数据
            data.ibody:setPos(1);
            local ret=self:sendCBBuffer(data.icmd,data.ibody,data.ifunc,data.iname);
            if data.acbfunc~=nil then
               data.acbfunc(data.iname,"SOCKET_TCP_CONNECTED");
            end
            if ret==nil then
                ret=-3;
            end
            if ret>0 then
                if data.icmd~=2 then
                    skdLog():logTFStr(string.format("send success %d %d %s",ret,data.icmd,skdProtocols():qryCmdStr(data.icmd)));
                end
                sendDataQueues[1]=nil;
            else
                skdLog():logTFStr(string.format("send fail %d %s ret=%d",data.icmd,data.iname,ret));
            end                    
        end        
    end
    if os.time()-lastHeart>50 then
        lastHeart = os.time();
        if sockets["Game"]~=nil then
            self:sendHeart("Game");
        end
    end
end

local updatehandler=gscheduler.scheduleGlobal(handler(NetClient,NetClient.update), 0.033);

function NetClient:getStatus(name)
    if sockets[name]~=nil then
        return sockets[name].status;
    else
        return nil;
    end
end

function NetClient:stop(name)        
    if self:getStatus(name)=="SOCKET_TCP_CONNECTED" then
        sockets[name].status="SOCKET_TCP_CLOSEING";
        sockets[name].socket:close();
        skdLog():logTFStr("clonse="..name);
    elseif self:getStatus(name)~="SOCKET_TCP_CLOSED" then
        sockets[name].status="SOCKET_TCP_CLOSED";
    end
end

function NetClient:sendHeart(name)
    local function HeartCallback(cmdid,data)
    end
    local heart=skdProtocols():qryIdProtocol(skdCmds().JSQY_USR_KEEPALIVE);--获取结构         
    self:sendCBData(heart,HeartCallback,name);--发送数据
end

function NetClient:setDefaultServer(name)
    defServer=name;
end

function NetClient:setUserID(id)
    userID=id;
end

function NetClient:getSocketCount()
    local count=0;
    for k,v in pairs(sockets) do
        count=count+1;        
    end
    return count;
end

function NetClient:getFirstName(name)
    for k,v in pairs(sockets) do
        return v.name;        
    end    
end

function NetClient:getSocketName(name)
    if name==nil or sockets[name]==nil then
        name=defServer;
    end
    if sockets[name]==nil then
        if self:getSocketCount()>0 then
            name=self:getFirstName();       
        end
    end   
    return name;
end

function NetClient:qryCmdID(cmd_str)
    return skdCmds()[cmd_str];
end

function NetClient:qryCmdStr(cmd_id)
    for key,value in pairs(skdCmds()) do
        if value==cmd_id then
            return key;
        end
    end
    return "";
end

function NetClient:detectCmdType(cmd)
    local cmd_str=self:qryCmdStr(cmd);
    if string.find(cmd_str,"_SVR_")~=nil then
        return 1;
    elseif string.find(cmd_str,"_USR_")~=nil then
        return 2;
    else
        return 0;
    end
end

function NetClient:getServerCmd(cmd)
    local ctype=self:detectCmdType(cmd);
    if ctype==2 then -- if client convert
        local cmdstr=self:qryCmdStr(cmd);
        cmdstr=string.gsub(cmdstr,"_USR_","_SVR_");
        return self:qryCmdID(cmdstr);
    else --other no change
        return cmd;
    end
end

function NetClient:getClientCmd(cmd)
    local ctype=self:detectCmdType(cmd);
    if ctype==1 then -- if client convert
        local cmdstr=self:qryCmdStr(cmd);
        cmdstr=string.gsub(cmdstr,"_SVR_","_USR_");
        return self:qryCmdID(cmdstr);
    else --other no change
        return cmd;
    end
end

function NetClient:registerCBHandler(cmd,cb_func,name)
   local name=self:getSocketName(name);
   local hname=name.."_"..tostring(self:getServerCmd(cmd));
   gcb_handlers[hname]=cb_func;
end

function NetClient:getCBHandler(cmd,name)
    local name=self:getSocketName(name);
    local scmdstr=tostring(self:getServerCmd(cmd));
    --skdLog():logStr("handler %s",hname);
    local names=self:getHandlerNames(scmdstr);
    for i,iname in ipairs(names) do
        if gcb_handlers[iname]~=nil then
            return gcb_handlers[iname];
        end
    end
    return nil;
end

function NetClient:getHandlerNames(cmdstr)
    local names={};
    for i,socket in pairs(sockets) do
        table.insert(names,socket.name.."_"..cmdstr);
    end
    return names;
end

function NetClient:sendCBData(tab,cb_func,name)
    local cmd=self:qryCmdID(tab.cmd);
    local body=ByteArray.new();
    body=skdProtocols():encode(tab,body);
    body:setPos(1);

    local sendData={};
    sendData.icmd=cmd;
    sendData.ibody=body;
    sendData.ifunc=cb_func;
    sendData.iname=name;
    table.insert(sendDataQueues,sendData);    
end

function NetClient:getHeader(body,cmd)    
    local header=ByteArray.new();
    --network head
    header:writeUInt(body:getLen()+12);             --length&flag
    --MessageHead
    header:writeUShort(body:getLen()+16);           --total length
    header:writeUShort(cmd);                        --command 0x2001
    local user,inner=skdClient():getUserId();
    header:writeUInt(packetIndex or 0);             --innerid
    header:writeUInt(userID or 0);                  --userid    
    skdLog():logStr("write userid %d",userID);
    header:setPos(1);
    packetIndex=packetIndex+1;
    return header;
end

function NetClient:sendCBBuffer(cmd,body,cb_func,name)
    self:registerCBHandler(cmd,cb_func,name);
    return self:sendBuffer(cmd,body,name);
end

--参数 命令字cmd，缓冲区ba，服务名name,为空则发送setDefaultServer的名字，如果没有设置则为Game
function NetClient:sendBuffer(cmd,body,name)
    name=self:getSocketName(name);
    local packet=ByteArray.new();
    local header=self:getHeader(body,cmd);    
    packet:writeBuf(header:readBuf(header:getLen()));
    packet:writeBuf(body:readBuf(body:getLen()));
    return sockets[name].socket:send(packet:getPack());    
end

--name要发送的网络名称，name为空的话用默认值Game
function NetClient:sendRawBuf(ba,name)
    name=self:getSocketName(name);
    sockets[name].socket:send(ba:getPack());
end

function NetClient:onStatus(event)
    bs.bsShine:socketStatus(event.target.name,event.name,handler(self,self.onCppCallSend));
    --print("Socket Status="..tostring(event.name));
    sockets[event.target.name].status=event.name;
    sockets[event.target.name].cb_func(event.target.name,event.name);
end

function NetClient:onCppCallSend(data,len,name)  
    local ba=ByteArray.new();
    ba:writeBytes(data,0,len);
    self:sendBuf(ba,name);
    skdLog():logTFStr("NetClient:onCppCallSend");
end

function  NetClient:compress(packet)
    local compress=zlib.deflate();    
    local deflate,eof,bytes_in,bytes_out=compress(packet:getBytes(),"finish");
    return bytes_out,deflate,eof,bytes_in;
end

function  NetClient:uncompress(packet)    
    local uncompress=zlib.inflate();
    local inflate,eof,bytes_in,bytes_out=uncompress(packet:getBytes(),"finish");
    return bytes_out,inflate,eof,bytes_in;
end

function NetClient:onRecvData(event)    
    --print("Socket Status:%s data:%s",event.name,ByteArray.toString(event.data, 16));
    --skdLog():logTFStr("Socket Recv Status:%s ",event.name);
    local packet=ByteArray.new();
    packet:writeBuf(event.data);
    packet:setPos(1);
    local flag=packet:readUInt();    
    --print("flen="..tostring(packet:readUInt()));     --length&flag
    local len=packet:readUShort();
    --nRealLen = (len+7)&0xfffffff8;--encrypt data , un use , is wired method       
    local real_len=bit.band(len,0x7fff);
    if bit.band(len,0x8000)>0 then --need unzip        
        --#define ZIP_COMPRESS_SIZE 1024
        --#define ZIP_COMPRESSFLAG ((DWORD)1 << 31)
        --uCompSize |= ZIP_COMPRESSFLAG;
        len=bit.band(len,0x7fff);
        local rawpacket=ByteArray.new();
        packet:readBytes(rawpacket,0,len);
        packet=self:uncompress(rawpacket);
    end        
    local plen=packet:getLen();
    local cmd=packet:readUShort();
    if (real_len+4)~=plen then
        --skdLog():logTFStr("error packet flag=%d rlen=%d plen=%d cmd=%d %s",flag,real_len,plen,cmd,skdProtocols():qryCmdStr(cmd));
    end    
    skdLog():logTFStr("onRecv cmd=%d %s rlen=%d",cmd,skdProtocols():qryCmdStr(cmd),real_len);
    self:parseData(packet,flag,len,cmd);
end

function NetClient:parseData(packet,flag,len,cmd)    
    --print("cmd="..tostring(cmd));           --command
    local inner = packet:readUInt();
    --print("innerid="..tostring(inner));     --innerid
    local user = packet:readUInt();    
    --print("userid="..tostring(user));       --userid
    if cmd==skdCmds().JSQY_LG_SVR_PT_AUTH then
        innerID=inner;
        userID=user;
        skdClient():setUserId(user,inner);
        skdLog():logTFStr("set userid="..tostring(user));     --userid
    end    
    if cmd==0 then --for debug 
        skdLog():logTFStr("Debug Packet cmd %d",cmd);
    end    
    local callback=self:getCBHandler(cmd);
    if callback~=nil then -- message handler
        --skdLog():logTFStr("id=%d;flag=%d;len=%d,packetlen %d",cmd,flag,len,packet:getLen());
        local bya=ByteArray.new();
        local len=packet:getLen()-packet:getPos()+1;
        bya:writeBuf(packet:readBuf(len));
        bya:setPos(1);
        local srv_data=skdProtocols():decode(cmd,bya,len,len);
        callback(cmd,srv_data);        
    else
        --un process
        skdLog():logTFStr("id=%d;flag=%d;len=%d;CBHandler nil",cmd,flag,len);
    end
end

return NetClient;