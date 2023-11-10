#!/usr/bin/env luajit
--
-- NeoVim Lua plugin for C.L.A.V.M.
--
-- Karl Yerkes
-- 2023-11-07
--


local ffi = require'ffi'
ffi.cdef [[
void submit(char*, const char*);
]]
local lib = ffi.load('build/libsubmit.dylib')

local function submit(source)
  print("source:", source)
  local reply = ffi.new("char[?]", 100)
  lib.submit(reply, source)
  print("reply:", ffi.string(reply))
end

print(submit("main() {}"))
print(submit("foo}"))
