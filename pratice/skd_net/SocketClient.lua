
--lua socket ²âÊÔ¿Í»§¶Ë

local SocketClient = {}

function SocketClient:ctor()
    
end

function SocketClient:dispose()
    
end

function SocketClient:isIPV6(addr)
    local addrifo = socket.dns.getaddrinfo(addr);
    if addrifo ~= nil then
        for k,v in pairs(addrifo) do
            if v.family == "inet6" then
                isipv6_only = true
                break
            end
        end
    end
    if isipv6_only then
        self.tcp = socket.tcp6()
    else
        self.tcp = socket.tcp()
    end
    return self.tcp;
end

function SocketClient:startClient()
    local socket = require("socket");
    local host="127.0.0.1";
    local port=12345;
    local sock=socket.connect(host,port);
    sock:settimeout(0);
    local input,recvt,sendt,status;
    while true do        
        input = io.read();
        if input~=nil and #input>0 then
            sock:send(input.."\n");
            print("send:"..input);            
        end      
        self:receiveServer(sock);          
    end            
end

function SocketClient:receiveServer(sock)
    local recvt,sendt,status = socket.select({sock},nil,1);
    while #recvt >0 do        
        local response,receive_status = sock:receive();        
        if receive_status~="closed" then
            if response then                
                print("recv fromSrever :"..response);
                recvt,sendt,status = socket.select({sock},nil,1);                
            end
        else
            break;
        end
    end
end

SocketClient:startClient();

return SocketClient
