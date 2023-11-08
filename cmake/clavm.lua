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

local function submit()
  local lines = vim.api.nvim_buf_get_lines(0, 0, -1, true)
  --vim.api.nvim_command(string.format('echo "%s"', vim.inspect(lines)))

  local reply = ffi.string(lib.submit(lines[0]))
  vim.print(vim.inspect(reply))
--  if (result == nil) then
--    vim.api.nvim_command(string.format('echo "%s"', "FAIL: timeout reached"))
--  else
--    vim.api.nvim_command(string.format('echo "%s"', reply))
--  end
end

return {
  submit = submit
}

-- autocmd TextChanged,TextChangedI *.c lua require"clavm".send_code()


-- Notes
--
-- You're not allowed to call nvim_command or the like in vim.loop context
--
-- List all the vim.loop offerings with lua print(vim.inspect(vim.loop))
--
-- Useful posts and reference docs:
--   https://github.com/luvit/luv/blob/master/docs.md
--   https://neovim.io/doc/user/lua.html
--   https://dev.to/jamestthompson3/using-libuv-in-neovim-900
--   https://dev.to/2nit/how-to-write-neovim-plugins-in-lua-5cca
--
-- XXX don't overwrite 'vim' symbol!
