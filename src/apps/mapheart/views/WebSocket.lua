
local WebSockets=class("WebSockets")

local cmds=require("....bytepack.skydream_cmd")
local protos=require("....bytepack.skydream_protocol")
local envelope=require("....bytepack.ProtocolEnvelope")

---------------------------------------------------------------wsText---------------------------------------------------------------begin
WebSockets.wsText=cc.WebSocket:create("wss://127.0.0.1:7681");
WebSockets.TextStatus="";
WebSockets.TextRecvs={};

local function wsSendTextOpen(strData)
    WebSockets.TextStatus="Send Text WS was opened.";
end

local function wsSendTextMessage(strData)
    receiveTextTimes= receiveTextTimes + 1
    local strInfo= "response text msg: "..strData..", "..receiveTextTimes;
    WebSockets.TextStatus=strInfo;
    table.insert(WebSockets.TextRecvs,strData);
end

local function wsSendTextClose(strData)
    WebSockets.TextStatus="close:"..strData;
    WebSockets.wsText = nil
end

local function wsSendTextError(strData)
    WebSockets.TextStatus="sendText Error was fired"..strData;
end

WebSockets.wsText:registerScriptHandler(wsSendTextOpen,cc.WEBSOCKET_OPEN)
WebSockets.wsText:registerScriptHandler(wsSendTextMessage,cc.WEBSOCKET_MESSAGE)
WebSockets.wsText:registerScriptHandler(wsSendTextClose,cc.WEBSOCKET_CLOSE)
WebSockets.wsText:registerScriptHandler(wsSendTextError,cc.WEBSOCKET_ERROR)
WebSockets.wsText:sendString("Hello WebSocket中文, I'm a text message.")

---------------------------------------------------------------wsText---------------------------------------------------------------end

---------------------------------------------------------------wsBinary-------------------------------------------------------------begin

WebSockets.wsBinary=cc.WebSocket:create("ws://127.0.0.1:7681");
WebSockets.BinaryStatus=""
WebSockets.BinaryRecvs={};

local function wsSendBinaryOpen(strData)
    WebSockets.BinaryStatus="Send Binary WS was opened.";
end

local function wsSendBinaryMessage(paramTable)
    local length = table.getn(paramTable)
    local i = 1
    local strInfo = "response bin msg: "
    for i = 1,length do
        if 0 == paramTable[i] then
            strInfo = strInfo.."\'\\0\'"
        else
            strInfo = strInfo..string.char(paramTable[i])
        end
    end
    receiveBinaryTimes = receiveBinaryTimes + 1
    strInfo = strInfo..receiveBinaryTimes
    WebSockets.BinaryStatus=strInfo;
    table.insert(WebSockets.BinaryRecvs,paramTable);
end

local function wsSendBinaryClose(strData)
    WebSockets.BinaryStatus="_wsiSendBinary websocket instance closed.";
    WebSockets.wsBinary = nil
end

local function wsSendBinaryError(strData)
    WebSockets.BinaryStatus="sendBinary Error was fired";
end

WebSockets.wsBinary:registerScriptHandler(wsSendBinaryOpen,cc.WEBSOCKET_OPEN)
WebSockets.wsBinary:registerScriptHandler(wsSendBinaryMessage,cc.WEBSOCKET_MESSAGE)
WebSockets.wsBinary:registerScriptHandler(wsSendBinaryClose,cc.WEBSOCKET_CLOSE)
WebSockets.wsBinary:registerScriptHandler(wsSendBinaryError,cc.WEBSOCKET_ERROR)

WebSockets.wsBinary:sendString("Hello WebSocket中文--,\0 I'm\0 a\0 binary\0 message\0.")

---------------------------------------------------------------wsBinary-------------------------------------------------------------end

--wsSendText   = cc.WebSocket:create("wss://echo.websocket.org", {"myprotocol_1", "myprotocol_2"}, "cacert.pem")
--wsSendBinary = cc.WebSocket:create("ws://echo.websocket.org", {"hello"}, "cacert.pem")
--wsError      = cc.WebSocket:create("ws://invalid.url.com", {"invalid_protocol"})

WebSockets.datacbs={}
WebSockets.syscbs={}
WebSockets.sockets={}

function WebSockets:testProto()
    print(cmds.JSQY_GS,protos.JSQY_MsgHead.m_uTotalLength,envelope:qryCmdStr(cmds.GDE_GetSmallRoleInfo_TopList))
end

function WebSockets:newWs(name,addr,callback)
    self.syscbs[name]=callback;
    self.sockets[name]=cc.WebSocket:create(addr);
    local function wsOpen(strData)
        self.syscbs[name](self.sockets[name],"open",strData);
    end
    local function wsRecvMessage(dataTable)
        local packet=ByteArray.new();
        packet:writeBuf(dataTable);
        packet:setPos(1);
        local flag=packet:readUInt();    
        local len=packet:readUShort();
        local real_len=bit.band(len,0x7fff);
        if bit.band(len,0x8000)>0 then --need unzip        
            len=bit.band(len,0x7fff);
            local rawpacket=ByteArray.new();
            packet:readBytes(rawpacket,0,len);
            packet=self:uncompress(rawpacket);
        end        
        local datalen=packet:getLen();
        local cmd=packet:readUShort();
        if real_len+4~=datalen then
            print("data len not complete");
        end
        local key=self:getKey(name,cmd);
        if self.datacbs[key] then
            self.datacbs[key](self.sockets[name],key,cmd,packet);
        else
            self.syscbs[name](self.sockets[name],"recv",key,cmd,packet);
        end
    end
    local function wsClose(strData)
        self.syscbs[name](self.sockets[name],"close",strData);
    end
    local function wsError(strData)
        self.syscbs[name](self.sockets[name],"error",strData);
    end
    self.sockets[name]:registerScriptHandler(wsOpen,cc.WEBSOCKET_OPEN)
    self.sockets[name]:registerScriptHandler(wsRecvMessage,cc.WEBSOCKET_MESSAGE)
    self.sockets[name]:registerScriptHandler(wsClose,cc.WEBSOCKET_CLOSE)
    self.sockets[name]:registerScriptHandler(wsError,cc.WEBSOCKET_ERROR)
end

function WebSockets:encryptData(name,data)
    --TODO:
end

function WebSockets:decryptData(name,data)
    --TODO:
end

function WebSockets:sendData(name,data)
    self.sockets[name]:sendString(data:getPack())
end

function WebSockets:getKey(name,cmd)
    local key=string.format("%s_%d",name,cmd);
    return key;
end

function WebSockets:getKey3(name,cmd,cmd1,cmd2,cmd3)--cmd1,cmd2,cmd3 is optional
    local key=string.format("%s_%d",name,cmd);
    if cmd1 then
        key=string.format("%s_%d_%d",name,cmd,cmd1)
        if cmd2 then
            key=string.format("%s_%d_%d_%d",name,cmd,cmd1,cmd2)
            if cmd3 then
                key=string.format("%s_%d_%d_%d_%d",name,cmd,cmd1,cmd2,cmd3)
            end
        end
    end
    return key;
end

function WebSockets:setDataback(key,callback)
    self.datacbs[key]=callback;
end

return WebSockets