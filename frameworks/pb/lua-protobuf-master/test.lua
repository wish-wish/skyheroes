local lu = require "luaunit"

local pb     = require "pb"
local pbio   = require "pb.io"
local buffer = require "pb.buffer"
local slice  = require "pb.slice"
local conv   = require "pb.conv"
local protoc = require "protoc"

-- local assert_not = lu.assertEvalToFalse
local eq       = lu.assertEquals
local table_eq = lu.assertItemsEquals
local fail     = lu.assertErrorMsgContains

local types = 0
for _ in pb.types() do
   types = types + 1
end
pbio.write("pb predefined types: ", tostring(types), "\n")

local function check_load(chunk, name)
   local pbdata = protoc.new():compile(chunk, name)
   local ret, offset = pb.load(pbdata)
   if not ret then
      error("load error at "..offset..
            "\nproto: "..chunk..
            "\ndata: "..buffer(pbdata):tohex())
   end
end

local function check_msg(name, data, r)
   local chunk2, _ = assert(pb.encode(name, data))
   local data2 = assert(pb.decode(name, chunk2))
   --print("msg: ", name, "<"..chunk2:gsub(".", function(s)
      --return ("%02X "):format(s:byte())
   --end)..">")
   eq(data2, r or data)
end

_G.test_io = {} do

function _G.test_io.setup()
   pbio.dump("address.proto", [[
   message Phone {
      optional string name        = 1;
      optional int64  phonenumber = 2;
   }
   message Person {
      optional string name     = 1;
      optional int32  age      = 2;
      optional string address  = 3;
      repeated Phone  contacts = 4;
   } ]])
end

function _G.test_io.teardown()
   os.remove "address.proto"
   os.remove "address.pb"
end

function _G.test_io.test()
   local code = "assert(io.write(require 'pb.io'.read()))"
   assert(pbio.dump("t.lua", code))
   local fh = assert(io.popen("lua t.lua < t.lua", "r"))
   eq(fh:read "*a", code)
   fh:close()
   assert(os.remove "t.lua")
   fail("-not-exists-", function() assert(pbio.read "-not-exists-") end)

   local chunk = assert(protoc.new():compile(pbio.read "address.proto",
                                             "address.proto"))
   assert(pbio.dump("address.pb", chunk))
   assert(pb.loadfile "address.pb")
   assert(pb.type "Person")
   eq(pb.type "-not-exists-", nil)
   local ft = {}
   for name in pb.fields "Person" do
      ft[name] = true
   end
   table_eq(ft, { name=true, age=true,address=true,contacts=true })

   eq(pb.decode("Person", "\240\255\255\255\255\255\255\255\255\1\1"), {})
   eq(pb.decode("Person", "\240\255\255\255\255\255\255\255\255\255"), {})
   eq(pb.decode("Person", "\240\255\255\255\255"), {})
   eq(pb.decode("Person", "\242\255\255\255\255\1\255"), {})
   eq(pb.decode("Person", "\x71"), {})
   eq(pb.decode("Person", "\x33\1\2\3\4\x34"), {})
   eq(pb.decode("Person", "\x33\1\2\3\4\x44"), {})
   eq(pb.decode("Person", "\x33\1\2\3\4"), {})
   eq(pb.decode("Person", "\x75"), {})
   eq(pb.decode("Person", "\x75\1\1\1\1"), {})

   fail("type 'Foo' does not exists", function() assert(pb.encode("Foo", {})) end)
   fail("type 'Foo' does not exists", function() assert(pb.decode("Foo", "")) end)

   fail("string expected at field 'name', got boolean", function()
      assert(pb.encode("Person", { name = true }))
   end)

   fail("type mismatch at offset 2, bytes expected for type string, got varint", function()
      assert(pb.decode("Person", "\8\1"))
   end)

   fail("invalid varint value at offset 2", function()
      assert(pb.decode("Person", "\16\255"))
   end)

   fail("invalid bytes length: 0 (at offset 2)", function()
      assert(pb.decode("Person", "\10\255"))
   end)

   fail("un-finished bytes (len 10 at offset 3)", function()
      assert(pb.decode("Person", "\10\10"))
   end)

   local data = {
      name = "ilse",
      age  = 18,
      contacts = {
         { name = "alice", phonenumber = 12312341234 },
         { name = "bob",   phonenumber = 45645674567 }
      }
   }
   check_msg(".Person", data)

   pb.clear()
   protoc.reload()

   fail("-not-exists-", function() assert(pb.loadfile "-not-exists-") end)
end

end

_G.test_depend = {} do

function _G.test_depend.setup()
   pbio.dump("depend1.proto", [[
      syntax = "proto2";

      message Depend1Msg {
          optional int32  id   = 1;
          optional string name = 2;
      } ]])
end

function _G.test_depend.teardown()
   os.remove "depend1.proto"
end

function _G.test_depend.test()
   local function load_depend(p)
      p:load [[
         syntax = "proto2";

         import "depend1.proto";

         message Depend2Msg {
             optional Depend1Msg dep1  = 1;
             optional int32      other = 2;
         } ]]
   end

   load_depend(protoc.new())
   local t = { dep1 = { id = 1, name = "foo" }, other = 2 }
   check_msg("Depend2Msg", t, { other = 2 })

   eq(protoc.new():loadfile "depend1.proto", true)
   check_msg("Depend2Msg", t)

   pb.clear "Depend1Msg"
   check_msg("Depend2Msg", t, { other = 2 })

   eq(protoc.new():loadfile "depend1.proto", true)
   check_msg("Depend2Msg", t)

   pb.clear "Depend1Msg"
   pb.clear "Depend2Msg"

   local p = protoc.new()
   p.include_imports = true
   load_depend(p)
   check_msg("Depend2Msg", t)
end

end

function _G.test_extend()
   local P = protoc.new()

   P.unknown_import = true
   P.unknown_type = true
   assert(P:load([[
      syntax = "proto2";
      import "descriptor.proto";

      extend google.protobuf.EnumValueOptions {
         optional string name = 51000;
      }

      message Extendable {
         extend NestExtend {
            optional string ext_name = 100;
         }
         optional int32 id = 1;
         extensions 100 to max;
      } ]], "extend1.proto"))
   eq(pb.type "Extendable", ".Extendable")

   P.unknown_import = nil
   P.unknown_type = nil
   local chunk = assert(P:compile [[
      syntax = "proto2";
      import "extend1.proto"

      enum MyEnum {
         First  = 1 [(name) = "first"];
         Second = 2 [(name) = "second"];
         Third  = 3 [(name) = "third"];
      }

      message NestExtend {
         optional uint32 id = 1;
         extensions 100 to max;
      }

      extend Extendable {
         optional string ext_name = 100;
      } ]])
   assert(pb.load(chunk))
   assert(pb.type "MyEnum")

   local t = { ext_name = "foo", id = 10 }
   check_msg("Extendable", t)

   local t2 = { ext_name = "foo", id = 10 }
   check_msg("NestExtend", t2)

   local data = assert(pb.decode("google.protobuf.FileDescriptorSet", chunk))
   eq(data.file[1].enum_type[1].value[1].options.name, "first")
   eq(data.file[1].enum_type[1].value[2].options.name, "second")
   eq(data.file[1].enum_type[1].value[3].options.name, "third")
end

function _G.test_type()
   check_load [[
   message TestTypes {
      optional double   dv    = 1;
      optional float    fv    = 2;
      optional int64    i64v  = 3;
      optional uint64   u64v  = 4;
      optional int32    i32v  = 5;
      optional uint32   u32v  = 13;
      optional fixed64  f64v  = 6;
      optional fixed32  f32v  = 7;
      optional bool     bv    = 8;
      optional string   sv    = 9;
      optional bytes    btv   = 12;
      optional sfixed32 sf32v = 15;
      optional sfixed64 sf64v = 16;
      optional sint32   s32v  = 17;
      optional sint64   s64v  = 18;
   } ]]

   local data = {
      dv    = 0.125;
      fv    = 0.5;
      i64v  = -12345678901234567;
      u64v  = 12345678901234567;
      i32v  = -2101112222;
      u32v  = 2101112222;
      f64v  = 12345678901234567;
      f32v  = 1231231234;
      bv    = true;
      sv    = "foo";
      btv   = "bar";
      sf32v = -123;
      sf64v = -456;
      s32v  = -1234;
      s64v  = -4321;
   }

   check_msg(".TestTypes", data)
   pb.clear "TestTypes"

   check_load [[
      message test_type {
         optional uint32 r = 1;
      }
      message test2 {
        optional test_type test_type = 1;
      } ]]
   check_msg("test_type", {r = 1})
   pb.clear "test_type"
   pb.clear "test2"
end

function _G.test_default()
   check_load [[
      enum TestDefaultColor {
         RED = 0;
         GREEN = 1;
         BLUE = 2;
      }
      message TestDefault {
         // some fields here
         optional int32 foo = 1;

         optional int32 defaulted_int = 10 [ default = 777 ];
         optional bool defaulted_bool = 11 [ default = true ];
         optional string defaulted_str = 12 [ default = "foo" ];
         optional float defaulted_num = 13 [ default = 0.125 ];

         optional TestDefaultColor color = 14 [default = RED];
         optional bool bool1 = 15 [default=false];
         optional bool bool2 = 16 [default=foo];
      } ]]
   check_msg("TestDefault", { foo = 1 })
   table_eq(pb.defaults "TestDefault", {
            defaulted_int = 777,
            defaulted_bool = true,
            defaulted_str = "foo",
            defaulted_num = 0.125,
            color = "RED",
            bool1 = false,
            bool2 = nil
         })
   pb.clear "TestDefault"
   pb.clear "TestDefaultColor"
end

function _G.test_enum()
   check_load [[
      enum Color {
         Red = 0;
         Green = 1;
         Blue = 2;
      }
      message TestEnum {
         optional Color color  = 1;
      } ]]
   eq(pb.enum("Color", 0), "Red")
   eq(pb.enum("Color", "Red"), 0)
   eq(pb.enum("Color", 1), "Green")
   eq(pb.enum("Color", "Green"), 1)
   local t = {}
   for name, number in pb.fields "Color" do
      t[name] = number
   end
   table_eq(t, {Red=0, Green=1, Blue=2})
   eq({pb.field("TestEnum", "color")}, {"color", 1, ".Color", nil, "optional"})

   local data = { color = "Red" }
   check_msg("TestEnum", data)

   local data2 = { color = 123 }
   check_msg("TestEnum", data2)

   pb.option "enum_as_value"
   check_msg("TestEnum", data, { color = 0 })

   pb.option "enum_as_name"
   check_msg("TestEnum", data, { color = "Red" })

   fail("invalid varint value at offset 2", function()
      assert(pb.decode("TestEnum", "\8\255"))
   end)
   fail("number/string expected at field 'color', got boolean", function()
      assert(pb.encode("TestEnum", { color = true }))
   end)
   fail("can not encode unknown enum 'foo' at field 'color'", function()
      assert(pb.encode("TestEnum", { color = "foo" }))
   end)

   check_load [[
      message TestAlias {
        enum AliasedEnum {
          option allow_alias = true;
          ZERO = 0;
          ONE = 1;
          TWO = 2;
          FIRST = 1;
        }
        repeated AliasedEnum aliased_enumf = 2;
      } ]]
   check_msg("TestAlias",
             { aliased_enumf = { "ZERO", "FIRST", "TWO", 23, "ONE" } },
             { aliased_enumf = { "ZERO", "FIRST", "TWO", 23, "FIRST" } })
end

function _G.test_packed()
   check_load [[
   message TestPacked {
      repeated int64 packs = 1 [packed=true];
   } ]]

   local data = {
      packs = { 1,2,3,4,-1,-2,3 }
   }
   check_msg(".TestPacked", data)
   fail("table expected at field 'packs', got boolean", function()
      assert(pb.encode("TestPacked", { packs = true }))
   end)

   pb.clear "TestPacked"
   eq(pb.type("TestPacked"), nil)
   for _, name in pb.types() do
      assert(name ~= "TestPacked", name)
   end
   eq(pb.types()(nil, "not-exists"), nil)

   check_load [[
      syntax="proto3";

      message MyMessage
      {
          repeated int32 intList = 1;
      } ]]
   local b = assert(pb.encode("MyMessage", {
                              intList = { 1,2,3 }
                           }))
   eq(pb.tohex(b), "0A 03 01 02 03")

   check_load [[
      syntax="proto3";

      message MessageA
      {
          int32 intValue = 1;
      }
      message MessageB
      {
          repeated MessageA messageValue = 1;
      } ]]
   check_msg("MessageB", { messageValue = { {} } })
   check_msg("MessageB", { messageValue = { { intValue = 1 } } })
   eq(pb.tohex(pb.encode(
      "MessageB", { messageValue = { {} } })), "0A 00")
   eq(pb.tohex(pb.encode(
      "MessageB", { messageValue = { { intValue = 1 } } })), "0A 02 08 01")
   pb.clear "MessageA"
   pb.clear "MessageB"
end

function _G.test_map()
   check_load [[
   syntax = "proto3";
   message TestMap {
       map<string, int32> map = 1;
       map<string, int32> packed_map = 2 [packed=true];
   } ]]

   local data = {
      map = { one = 1, two = 2, three = 3 };
      packed_map = { one = 1, two = 2, three = 3 }
   }
   check_msg(".TestMap", data)

   local data2 = { map = { one = 1, [1]=1 } }
   fail("string expected at field 'key', got number", function()
      local chunk = assert(pb.encode("TestMap", data2))
      table_eq(pb.decode("TestMap", chunk), { map = {one = 1} })
   end)
   eq(pb.decode("TestMap", "\10\4\3\10\1\1"), { map = {} })

   check_load [[
   syntax = "proto2";
   message TestMap2 {
       map<string, int32> map = 1;
   } ]]
   check_msg("TestMap2", { map = { one = 1, two = 2, three = 3 } })
end

function _G.test_oneof()
   check_load [[
   syntax = "proto3";
   message TestOneof {
       oneof test_oneof {
         string name = 4;
         int32  value = 5;
       }
   } ]]

   local data = { name = "foo" }
   check_msg("TestOneof", data)

   data = { name = "foo", value = 5 }
   check_msg("TestOneof", data)
   eq(pb.field("TestOneof", "name"), "name")
   pb.clear("TestOneof", "name")
   eq(pb.field("TestOneof", "name"), nil)
   eq(pb.type "TestOneof", ".TestOneof")
   pb.clear "TestOneof"
   eq(pb.type "TestOneof", nil)
end

function _G.test_conv()
   eq(conv.encode_uint32(-1), 0xFFFFFFFF)
   eq(conv.decode_uint32(0xFFFFFFFF), 0xFFFFFFFF)
   eq(conv.decode_uint32(conv.encode_uint32(-1)), 0xFFFFFFFF)

   eq(conv.encode_int32(0x12300000123), 0x123)
   eq(conv.encode_int32(0xFFFFFFFF), -1)
   eq(conv.encode_int32(0x123FFFFFFFF), -1)
   eq(conv.encode_int32(0x123FFFFFFFE), -2)
   eq(conv.decode_int32(0x12300000123), 0x123)
   eq(conv.decode_int32(0xFFFFFFFF), -1)
   eq(conv.decode_int32(0x123FFFFFFFF), -1)
   eq(conv.decode_int32(0x123FFFFFFFE), -2)
   eq(conv.decode_int32(conv.encode_int32(-1)), -1)

   eq(conv.encode_sint32(0), 0)
   eq(conv.encode_sint32(-1), 1)
   eq(conv.encode_sint32(1), 2)
   eq(conv.encode_sint32(-2), 3)
   eq(conv.encode_sint32(2), 4)
   eq(conv.encode_sint32(-3), 5)
   eq(conv.encode_sint32(-123), 245)
   eq(conv.encode_sint32(123), 246)
   eq(conv.encode_sint64(-123), 245)
   eq(conv.encode_sint64(123), 246)

   eq(conv.decode_sint32(0), 0)
   eq(conv.decode_sint32(1), -1)
   eq(conv.decode_sint32(2), 1)
   eq(conv.decode_sint32(3), -2)
   eq(conv.decode_sint32(4), 2)
   eq(conv.decode_sint32(5), -3)
   eq(conv.decode_sint32(245), -123)
   eq(conv.decode_sint32(246), 123)
   eq(conv.decode_sint32(0xFFFFFFFF), -0x80000000)
   eq(conv.decode_sint32(0xFFFFFFFE), 0x7FFFFFFF)
   eq(conv.decode_sint64(0xFFFFFFFF), -0x80000000)
   eq(conv.decode_sint64(0xFFFFFFFE), 0x7FFFFFFF)

   eq(conv.decode_float(conv.encode_float(123.125)), 123.125)
   eq(conv.decode_double(conv.encode_double(123.125)), 123.125)
end

function _G.test_buffer()
   eq(buffer.pack("vvv", 1,2,3), "\1\2\3")
   eq(buffer.tohex(pb.pack("d", 4294967295)), "FF FF FF FF")
   if _VERSION == "Lua 5.3" then
      eq(buffer.tohex(pb.pack("q", 9223372036854775807)), "FF FF FF FF FF FF FF 7F")
   end
   eq(pb.pack("s", "foo"), "\3foo")
   eq(pb.pack("cc", "foo", "bar"), "foobar")
   eq(buffer():pack("vvv", 1,2,3):result(), "\1\2\3")

   eq(buffer("foo", "bar"):result(), "foobar")
   eq(buffer.new("foo", "bar"):result(), "foobar")

   local b = buffer.new()
   b:pack("b", true);       eq(b:tohex(-1), "01")
   b:pack("f", 0.125);      eq(b:tohex(-4), "00 00 00 3E")
   b:pack("F", 0.125);      eq(b:tohex(-8), "00 00 00 00 00 00 C0 3F")
   b:pack("i", 4294967295); eq(b:tohex(-5), "FF FF FF FF 0F")
   b:pack("j", 4294967295); eq(b:tohex(-1), "01")
   b:pack("u", 4294967295); eq(b:tohex(-5), "FF FF FF FF 0F")
   b:pack("x", 4294967295); eq(b:tohex(-4), "FF FF FF FF")
   b:pack("y", 4294967295); eq(b:tohex(-4), "FF FF FF FF")
   if _VERSION == "Lua 5.3" then
      b:pack("I", 9223372036854775807); eq(b:tohex(-9), "FF FF FF FF FF FF FF FF 7F")
      b:pack("J", 9223372036854775807); eq(b:tohex(-10), "FE FF FF FF FF FF FF FF FF 01")
      b:pack("U", 9223372036854775807); eq(b:tohex(-9), "FF FF FF FF FF FF FF FF 7F")
      b:pack("X", 9223372036854775807); eq(b:tohex(-8), "FF FF FF FF FF FF FF 7F")
      b:pack("Y", 9223372036854775807); eq(b:tohex(-8), "FF FF FF FF FF FF FF 7F")
   end
   assert(#b ~= 0)
   assert(#b:reset() == 0)
   assert(tostring(b):match 'pb.Buffer')

   b = buffer.new()
   eq(b:pack("(vvv)", 1,2,3):tohex(-4), "03 01 02 03")
   eq(b:pack("((vvv))", 1,2,3):tohex(-5), "04 03 01 02 03")
   fail("unmatch '(' in format", function() pb.pack "(" end)
   fail("unexpected ')' in format", function() pb.pack ")" end)
   fail("number expected for type 'int32', got string", function() pb.pack("i", "foo") end)
   fail("invalid formater: '!'", function() pb.pack '!' end)

   b = buffer.new()
   eq(b:pack("c", ("a"):rep(1025)):result(), ("a"):rep(1025))
   eq(b:pack("c", ("b"):rep(1025)):result(), ("a"):rep(1025)..("b"):rep(1025))
   eq(#b, 2050)
   b:reset("foo", "bar")
   eq(#b, 6)

   fail("number expected, got string", function() pb.pack("v", "foo") end)

   b = buffer.new()
   fail("encode bytes fail", function() b:pack("#", 10) end)
   check_load [[
   message Test { optional int32 value = 1 }
   ]]
   local len = #b
   eq(#b, 0)
   eq(pb.encode("Test", { value = 1 }, b), b)
   eq(#b, 2)
   b:pack("#", len)
   eq(b:tohex(), "02 08 01")
end

function _G.test_slice()
   local s = slice.new "\3\1\2\3"
   eq(#s, 4)
   eq(s:level(), 1)
   eq(s:level(1), 1)
   eq(s:level(2), nil)
   eq({s:level(-1)}, {1,1,4})
   eq(s:enter(), s)
   eq(s:level(), 2)
   eq({s:level(-1)}, {2,2,4})
   eq({s:level(1)}, {5,1,4})
   eq({s:unpack "vvv"}, {1,2,3})
   eq(s:unpack "v", nil)
   eq(s:leave(), s)
   eq(s:unpack("+s", -4), "\1\2\3")

   s = slice "\3\1\2\3"
   for i = 2, 20 do
      s:enter(1, 4); eq(s:level(), i)
   end
   for i = 19, 1 do
      s:leave(); eq(s:level(), i)
   end
   eq(s:tohex(), "03 01 02 03")
   eq(s:result(-3), "\1\2\3")
   eq(#s, 4)
   eq(#s:reset(), 0)
   eq(#s:reset"foo", 3)

   eq({pb.unpack("\255\1", "v@")}, { 255, 3 })
   eq({pb.unpack("\1", "v*v", 1)}, { 1, 1 })
   fail("invalid formater: '!'", function() pb.unpack("\1", '!') end)

   table_eq({slice.unpack("\1\2\3", "vvv")}, {1,2,3})
   eq(pb.unpack("\255\255\255\255", "d"), 4294967295)
   if _VERSION == "Lua 5.3" then
      eq(pb.unpack("\255\255\255\255\255\255\255\127", "q"), 9223372036854775807)
   end
   eq(pb.unpack("\3foo", "s"), "foo")
   eq({pb.unpack("foobar", "cc", 3, 3)}, {"foo", "bar"})

   eq(pb.unpack("\255\255\255\127\255", "v"), 0xFFFFFFF)
   fail("invalid varint value at offset 1", function()
      pb.unpack(("\255"):rep(10), "v") end)
   fail("invalid varint value at offset 1", function() pb.unpack("\255\255\255", "v") end)
   fail("invalid varint value at offset 1", function() pb.unpack("\255\255\255", "v") end)
   fail("invalid bytes value at offset 1", function() pb.unpack("\3\1\2", "s") end)
   fail("invalid fixed32 value at offset 1", function() pb.unpack("\1\2\3", "d") end)
   fail("invalid fixed64 value at offset 1", function() pb.unpack("\1\2\3", "q") end)
   fail("invalid sub string at offset 1", function() pb.unpack("\3\1\2", "c", 5) end)
   fail("invalid varint value at offset 1", function() pb.unpack("\255\255\255", "i") end)
   fail("invalid fixed32 value at offset 1", function() pb.unpack("\255\255\255", "x") end)
   fail("invalid fixed64 value at offset 1", function() pb.unpack("\255\255\255", "X") end)
   fail("string/buffer/slice expected, got boolean", function() pb.unpack(true, "v") end)
   fail("bytes wireformat expected at offset 1", function() slice"\1":enter() end)

   fail("level (3) exceed max level 2", function()
      local s1 = slice.new "\1\2\3"
      s1:enter()
      s1:leave(3)
   end)

   s:reset "\1\2\3"
   eq({s:leave()}, {s, 1})
   eq(s:level(), 1)
   eq({s:level(-1)}, {1,1,3})


   assert(tostring(s):match 'pb.Slice')
end

function _G.test_load()
   eq({pb.load "\10\2\18\3"}, {false, 4})
end

os.exit(lu.LuaUnit.run(), true)
