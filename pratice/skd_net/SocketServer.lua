
--lua socket ²âÊÔ·şÎñ¶Ë
--[[
--iocp:https://blog.csdn.net/zhanghefu/article/details/4420832
--https://blog.csdn.net/heyan1853/article/details/6457362
--epoll(Linux):https://blog.csdn.net/ljx0305/article/details/4065058
--select/poll/epoll:http://www.cnblogs.com/Anker/p/3265058.html
--kqueue(Kqueue):https://blog.csdn.net/zdy0_2004/article/details/51805744
--epoll/kqueue:http://www.cnblogs.com/moonz-wu/p/4740908.html
--epoll,kqueue,rtsig,eventport,poll,select
--]]

local SocketServer = {}

function SocketServer:ctor()
    
end

function SocketServer:dispose()
    
end

function SocketServer:startServer()
    local socket=require("socket")
    local host="127.0.0.1"
    local port=12345;
    local server=socket.bind(host,port,1024);
    server:settimeout(0);
    local client_tab={};
    local conn_count = 0;
    print("Server Start %s:%d",host,port);
    while true do
        local conn=server:accept();
        if conn then
            conn_count=conn_count+1;
            client_tab[conn_count]=conn;
            print("A client successfully connect!"..conn_count);
            conn:send("connect ok\n");
        end
        for cindex,client in pairs(client_tab) do
            local recvt,sendt,status = socket.select({client},nil,1);            
            if #recvt>0 then
                local receiver,receive_status=client:receive();                
                if receive_status~="closed" then
                    if receiver then
                        print("Receive Client "..cindex..":"..receiver);
                        client:send("client "..cindex.." Send");
                        client:send(receiver.." Send");                        
                    end
                else
                    table.remove(client_tab,cindex);
                    client:close();
                    print("client "..cindex.. "disconnect");
                end
            end
        end
    end
end

SocketServer:startServer();

return SocketServer
