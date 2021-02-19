
--日志中心

local LogCenter = class("LogCenter")

local logfile="log"..os.date("%Y_%m_%d_%H_%M_%S")..".txt";

local logLevel={
"Message",
"Warning",
"Error",
"Fatal",
"System"
}

function LogCenter:ctor()
    
end

function LogCenter:dispose()
    
end

function LogCenter:logTFStr(fmt,...)    
    local logstr=string.format("t(%d.%.4f)"..fmt,os.time(),os.clock(),...);
    local outfile=cc.FileUtils:getInstance():getWritablePath()..logfile;
    local logfile = io.open(outfile,"a");
    logfile:write(logstr.."\n");
    print(logstr);
    logfile:close();
end

function LogCenter:logStr(fmt,...)
    local logstr=string.format(fmt,...);
    print(logstr);
end

function LogCenter:logTStr(fmt,...)
    local logstr=string.format("t(%d)"..fmt,os.time(),...);    
    self:logStr("t(%d)"..fmt,os.time(),...);
end

function LogCenter:logBattle(fmt,...)
    self:logTFStr(fmt,...);
end

function LogCenter:logMessage(fmt,...)
   self:logStr(logLevel[1]..fmt,...);
end

function LogCenter:logWarning(fmt,...)
   self:logStr(logLevel[2]..fmt,...); 
end

function LogCenter:logError(fmt,...)
    self:logTStr(logLevel[3]..fmt,...); 
end

function LogCenter:logFatal(fmt,...)
    self:logTStr(logLevel[4]..fmt,...); 
end

function LogCenter:logSystem(fmt,...)
    self:logTStr(logLevel[5]..fmt,...); 
end

return LogCenter

