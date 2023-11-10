#!/usr/bin/env luajit
--
-- NeoVim Lua plugin for C.L.A.V.M.
--
-- Karl Yerkes
-- 2023-11-07
--


local ffi = require'ffi'
ffi.cdef [[
const char* submit(const char*);
]]
local lib = ffi.load('build/libsubmit.dylib')

local function submit(code)
  local reply = ffi.string(lib.submit(code))
  print(reply)
end

submit("main() {}")
