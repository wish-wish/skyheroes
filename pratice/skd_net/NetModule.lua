--module(...,package.seeall);

local gNetClient=nil;
function bsNetClient()
    if gNetClient == nil then
        gNetClient=import("NetClient");
    end
    return gNetClient;
end

local gCommands=nil;
function skdCmds()
    if gCommands == nil then
        gCommands=import("skydream_cmd");
    end
    return gCommands;
end

local gProtocols=nil;
function skdProtocols()
    if gProtocols == nil then
        gProtocols=import("ProtocolLoader");
    end
    return gProtocols;
end

function skdReloadLua(filepath)
    package.loaded[filepath]=nil;
    import(filepath);
end

function skdReloadData()
    skdReloadLua(".skydream_cmd");
end

