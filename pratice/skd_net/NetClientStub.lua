--Lua 网络

local NetClientStub = class("NetClientStub");


function NetClientStub:ctor()
    
end

function NetClientStub:dispose()
    
end

function NetClientStub:connect(name)
 
end

function NetClientStub:qryCmdID(cmd_str)
    return skdCmds()[cmd_str];
end

function NetClientStub:qryCmdStr(cmd_id)
    for key,value in pairs(skdCmds()) do
        if value==cmd_id then
            return key;
        end
    end
    return "";
end

function NetClientStub:detectCmdType(cmd)
    local cmd_str=self:qryCmdStr(cmd);
    if string.find(cmd_str,"_SVR_")~=nil then
        return 1;
    elseif string.find(cmd_str,"_USR_")~=nil then
        return 2;
    else
        return 0;
    end
end

function NetClientStub:sendCBData(tab,cb_func,name)
    
end

function NetClientStub:onRecvData(event)    
    
end

return NetClientStub;