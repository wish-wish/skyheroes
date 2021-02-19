
local ProtocolEnvelope = class("ProtocolEnvelope");

local Bit=import(".BitOpr");
local ByteArray=import(".ByteArray");
local ByteArrayVariant=import(".ByteArrayVarint");
local Protocols=import(".skydream_protocol");
local Cmds=import(".skydream_cmd");
local Log=import("..apps.LogCenter");

function ProtocolEnvelope:cloneTab(atab)
    if atab==nil then
        return nil;
    end 
    local tab={};
    for k,v in pairs(atab) do
        if type(v)~="table" then
            tab[k]=v;
        else
            tab[k]=self:cloneTab(v);
        end
    end
    return tab;
end

function cloneTab(atab)
    return ProtocolEnvelope:cloneTab(atab);
end

function ProtocolEnvelope:encode(atab,bytearr)
    
    if atab.data==nil then
        return nil;
    end    
    for k,field_desc in ipairs(atab.data) do
        local basetype=field_desc[2];
        local blocktype=field_desc[3];
            
        local function BaseTypeWrite()
            if basetype=="bit" then
                --TODO:writeBit
            elseif basetype=="INT8" then
                bytearr:writeByte(atab[field_desc[1]]);
            elseif basetype=="UINT8" then
                bytearr:writeUByte(atab[field_desc[1]]);
            elseif basetype=="INT16" then
                bytearr:writeShort(atab[field_desc[1]]);
            elseif basetype=="UINT16" then
                bytearr:writeUShort(atab[field_desc[1]]);
            elseif basetype=="INT32" then
                bytearr:writeInt(atab[field_desc[1]]);
            elseif basetype=="UINT32" then
                bytearr:writeUInt(atab[field_desc[1]]);
            elseif basetype=="INT64" then
                --TODO:int64 存储
                bytearr:writeInt(atab[field_desc[1]]);
                bytearr:writeInt(atab[field_desc[1]]);
            elseif basetype=="UINT64" then
                --TODO:Uint64 存储
                bytearr:writeUInt(atab[field_desc[1]]);
                bytearr:writeUInt(atab[field_desc[1]]);
            elseif basetype=="INT128" then
                --TODO:int128 存储
                bytearr:writeInt(atab[field_desc[1]]);
                bytearr:writeInt(atab[field_desc[1]]);
                bytearr:writeInt(atab[field_desc[1]]);
                bytearr:writeInt(atab[field_desc[1]]);
            elseif basetype=="UINT128" then
                --TODO:Uint128 存储
                bytearr:writeUInt(atab[field_desc[1]]);
                bytearr:writeUInt(atab[field_desc[1]]);
                bytearr:writeUInt(atab[field_desc[1]]);
                bytearr:writeUInt(atab[field_desc[1]]);
            elseif basetype=="FLOAT" then
                bytearr:writeFloat(atab[field_desc[1]]);
            elseif basetype=="DOUBLE" then
                bytearr:writeDouble(atab[field_desc[1]]);
            elseif basetype=="U_VipItem" then
                ret=bytearr:writeUShort(atab[field_desc[1]]);
            elseif basetype=="U_PlayerState" then
                ret=bytearr:writeUByte(atab[field_desc[1]]);
            else

            end
        end
        if blocktype>19 then --array
            if blocktype==20 then --base array
                if basetype=="INT8" or basetype=="UINT8" then
                    local len=string.len(atab[field_desc[1]]);
                    bytearr:writeString(atab[field_desc[1]]);
                    for i=1,field_desc[5]-len,1 do
                        --local char=string.char(0);
                        bytearr:writeByte(0);
                    end
                else
                    for i=1,field_desc[5],1 do --write array
                        BaseTypeWrite();
                    end
                end
            elseif blocktype==21 then --enum array
                for i=1,field_desc[5],1 do
                    bytearr:writeInt(atab[field_desc[1]]);
                end
            elseif blocktype==22 then --struct array
                for i=1,field_desc[5],1 do
                    self:encode(atab[field_desc[1]], bytearr);
                end
            end
        else
            if blocktype==0 then --base
                BaseTypeWrite();
            elseif blocktype==1 then --enum
                bytearr:writeInt(atab[field_desc[1]]);
            elseif blocktype==2 then --struct
                if self:isSpecialType(atab[field_desc[2]]) then
                    BaseTypeWrite();    
                else
                    self:encode(atab[field_desc[1]],bytearr);
                end
            end
        end
    end
    return bytearr;
end

function ProtocolEnvelope:isSpecialType(cmd_id)
    if cmd_id=="U_VipItem" or cmd_id=="U_PlayerState" then
        return true;
    end
    return false;
end

function ProtocolEnvelope:decode(cmd_id,bytearr,alen,tlen,typeflag)
    local struct=nil;
    if typeflag==nil then
        struct=self:qryIdProtocol(cmd_id);
    elseif typeflag==1 then        
        struct=self:qryTypeProtocol(cmd_id);
    end
    if struct==nil then
        Log:logTFStr(cmd_id.." is nil");
        return nil;
    end
    local alen=alen or tlen;
    for k,field_desc in ipairs(struct.data) do        
        local basetype=field_desc[2];
        local blocktype=field_desc[3];
        if blocktype<20 then 
            if alen<field_desc[5]-1 then  --TODO:特殊处理，服务要求
                Log:logTFStr("%s for keys cmdid %d all len %d",field_desc[1],cmd_id,alen or -1);
                return struct;
            end
        end
        local ttlen=tlen;
        if bytearr:getPos()>ttlen then --TODO:特殊处理，服务要求
            Log:logTFStr("%s for keys cmdid %d pos %d blen %d",field_desc[1],cmd_id,bytearr:getPos(),ttlen or -1);
            return struct;
        end
        local function BaseTypeLen()            
            local ret=0
            if basetype=="bit" then
                --TODO:parseBit                
            elseif basetype=="INT8" then
                ret=1;                
            elseif basetype=="UINT8" then
                ret=1;
            elseif basetype=="INT16" then
                ret=2;
            elseif basetype=="UINT16" then
                ret=2;            
            elseif basetype=="INT32" then
                ret=4;
            elseif basetype=="UINT32" then
                ret=4;            
            elseif basetype=="INT64" then
                --TODO:int64 读取
                ret=8;                
            elseif basetype=="UINT64" then
                --TODO:Uint64 读取
                ret=8
            elseif basetype=="INT128" then
                --TODO:int128 读取
                ret=16;                
            elseif basetype=="UINT128" then
                --TODO:Uint128 读取
                ret=16
            elseif basetype=="FLOAT" then
                ret=8
            elseif basetype=="DOUBLE" then
                ret=8
            else
                print(string.format("decode::BaseTypeLen unknown type %s", basetype));
            end
            return ret;
        end
        local function BaseTypeRead()                        
            if alen<field_desc[5] then --TODO:特殊处理，服务要求
                Log:logTFStr("%s BaseTypeRead cmdid %d all len %d pos %d blen %d",field_desc[1],cmd_id,alen,bytearr:getPos(),ttlen);
                return struct;
            end            
            local ret=nil;
            if basetype=="bit" then
                --TODO:parseBit
                local num=bytearr:parseBit(ret,3,4);
                print(tostring(num));
            elseif basetype=="INT8" then
                ret=bytearr:readByte();                
            elseif basetype=="UINT8" then
                ret=bytearr:readUByte();
            elseif basetype=="INT16" then
                ret=bytearr:readShort();
            elseif basetype=="UINT16" then
                ret=bytearr:readUShort();            
            elseif basetype=="INT32" then
                ret=bytearr:readInt();
            elseif basetype=="UINT32" then
                ret=bytearr:readUInt();            
            elseif basetype=="INT64" then
                --TODO:int64 读取
                ret=bytearr:readInt();
                ret=bytearr:readInt();
            elseif basetype=="UINT64" then
                --TODO:Uint64 读取
                ret=bytearr:readUInt();
                ret=bytearr:readUInt();
            elseif basetype=="UINT64" then
                --TODO:Uint64 读取
                ret=bytearr:readUInt();
                ret=bytearr:readUInt();
                ret=bytearr:readUInt();
                ret=bytearr:readUInt();
            elseif basetype=="INT128" then
                --TODO:int128 读取
                ret=bytearr:readInt();
                ret=bytearr:readInt();
                ret=bytearr:readInt();
                ret=bytearr:readInt();
            elseif basetype=="FLOAT" then
                ret=bytearr:readFloat();
            elseif basetype=="DOUBLE" then
                ret=bytearr:readDouble();
            elseif basetype=="U_VipItem" then
                ret=bytearr:readUShort();
            elseif basetype=="U_PlayerState" then
                ret=bytearr:readUByte();      
            else
                Log:logTFStr(string.format("decode::BaseTypeRead unknown type %s", basetype));          
            end
            return ret;
        end
        --[[lua regex
        .：任意字符
        %s：空白符；%p：标点符号；%c：控制字符；%d：数字；%x：16禁止数字；%z：代表0的字符；%a：字母；%l：小写字母；%u：大写字母；%w：字母和数字；
        %：转义
        []：匹配；():捕获
        ^：补，取反
        +:一次或多次;*:0次或多次，最长匹配;-:0次或多次，最短匹配;?:0次或一次;^：匹配字符串开头；$:匹配字符串结尾
        ]]
        if blocktype>19 then --array
            if blocktype==20 then --base array
                if basetype=="INT8" or basetype=="UINT8" then               
                    local flen=field_desc[5];
                    if flen>alen then
                        Log:logTFStr("%s read StringLen %d alen %d",field_desc[1],flen,alen);
                        flen=alen;
                    end
                    --local flen=field_desc[5];
                    --local alen=bytearr:getLen();
                    local fstr=bytearr:readString(flen);
                    local s=string.gsub(fstr, "^%s*(.-)%s*$", "%1");
                    local ends="%z";
                    local fieldstr=string.gsub(s,ends,"");
                    --TODO:string reader
                    struct[field_desc[1]]=fieldstr;
                else
                    local iflen=field_desc[5];
                    if iflen==0 then
                        iflen=alen;
                    end
                    for i=1,iflen/field_desc[4],1 do --read array
                        if struct[field_desc[1]]==nil then --table init
                            struct[field_desc[1]]={};
                        end
                        table.insert(struct[field_desc[1]],BaseTypeRead());
                    end
                end
            elseif blocktype==21 then --enum int array
                local iflen=field_desc[5];
                if iflen==0 then
                    iflen=alen;
                end
                enumlen=iflen/4;
                for i=1,enumlen,1 do
                    if struct[field_desc[1]]==nil then --table init
                        struct[field_desc[1]]={};
                    end
                    table.insert(struct[field_desc[1]],bytearr:readInt());
                end
            elseif blocktype==22 then --struct array                                
                local iflen=field_desc[5];
                if iflen==0 then
                    struct_count=struct[struct.data[k-1][1]];
                else
                    struct_count=math.round(iflen/field_desc[4]);    
                end
                local ref_count_field=field_desc[7];
                if ref_count_field~=nil then
                    --struct_count=struct[ref_count_field];
                end
                if struct_count==nil then
                    struct_count=0;
                end
                --struct_count = 6;
                for i=1,struct_count,1 do                    
                    if struct[field_desc[1]]==nil then --table init
                        struct[field_desc[1]]={};
                    end
                    local specialValue=0;
                    if self:isSpecialType(field_desc[2]) then
                        specialValue=BaseTypeRead();
                    else                        
                        specialValue=self:decode(field_desc[2],bytearr,field_desc[4],ttlen,1);
                    end
                    table.insert(struct[field_desc[1]],specialValue);
                end
            end
        else
            if blocktype==0 then --base
                --print(field_desc[1]);  
                struct[field_desc[1]]=BaseTypeRead();
            elseif blocktype==1 then --enum
                struct[field_desc[1]]=bytearr:readInt();
            elseif blocktype==2 then --struct
                if self:isSpecialType(field_desc[2]) then
                    struct[field_desc[1]]=BaseTypeRead();
                else
                    struct[field_desc[1]]=self:decode(field_desc[2],bytearr,field_desc[5],ttlen,1);
                end
            end
        end
        alen=alen-field_desc[5];
    end
    return struct;
end

function ProtocolEnvelope:qryTypeProtocol(type_str)
    return self:cloneTab(Protocols[type_str]);
end

function ProtocolEnvelope:qryStrProtocol(cmd_str)
    for k,v in pairs(Protocols) do
        if v.cmd==cmd_str then
            return self:cloneTab(v);
        end
    end
    return nil;
end

function ProtocolEnvelope:qryIdProtocol(cmd_id)
    for k,v in pairs(Protocols) do
        local str=self:qryCmdStr(cmd_id);
        if v.cmd==str then
            return self:cloneTab(v);
        end
    end
    return nil;
end

function ProtocolEnvelope:qryCmdID(cmd_str)
    return Cmds[cmd_str];
end

function ProtocolEnvelope:qryCmdStr(cmd_id)
    for key,value in pairs(Cmds) do
        if value==cmd_id then
            return key;
        end
    end
end

return ProtocolEnvelope;
