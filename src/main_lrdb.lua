--package.path = package.path .. ";.;src/?.luac;src/?/init.luac;src/;src/?/init.lua;"
--package.cpath = package.cpath ..";.;simulator/debugger/?.dll;"

local lrdb = require("lrdb_server")
lrdb.activate(21110) --21110 is using port number. waiting for connection by debug client.

--debuggee lua code
dofile("luascript.lua");

lrdb.deactivate() --deactivate debug server if you want.