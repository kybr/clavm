--
-- NeoVim plugin for C.L.A.V.M.
--
-- Karl Yerkes
-- 2020-10-03
--


local socket = nil -- use the same socket for all

--------------------------------------------------------------------------------
-- Listen for buffer changes; Spurt contents via UDP 127.0.0.1:9999 ------------
--------------------------------------------------------------------------------

-- local function text_change(_, bufnr, changedtick, firstline, lastline, new_lastline, old_byte_size, old_utf32_size, old_utf16_size)
local function text_change(_, bufnr)

  -- TODO: maybe throttle here; return quick if the text did not actually change
  -- Probably hashing is the only way to detect this.

  -- get all the lines in the current buffer and make a single string
  --
  local lines = vim.api.nvim_buf_get_lines(bufnr, 0, -1, true)
  local content = table.concat(lines, "\n")

  -- TODO: possibly prepend metadata here; maybe buffer number?
  -- TODO: check that the string is no more than 65507 bytes total

  -- send a datagram with the content to the server
  --
  local bytes = vim.loop.udp_try_send(socket, content, "127.0.0.1", 9999)
  if (bytes == nil) then
    -- TODO: handle this; or not. why would it fail?
  end
end

local function buffer_listen()
  -- TODO: check the current buffer number and fail if we already listen

  -- attach a listener to the current buffer (0 means current buffer)
  local result = vim.api.nvim_buf_attach(0, false, {on_lines = text_change})
  if (result == nil) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: could not attach"))
  end
end

-- TODO: un-listen to buffers
local function buffer_ignore()
end


--------------------------------------------------------------------------------
-- Listen for status information on UDP 10001; Show on the status line ---------
--------------------------------------------------------------------------------

local function status_update(err, data, addr, flags)

  -- XXX: this callback is called twice for each packet (why?)
  -- and gets no arguments the second time it is called; return
  if (data == nil) then
    return
  end

  vim.schedule(function()
    vim.api.nvim_command(string.format('echo "%s"', data))
  end)
  
  -- TODO:
  -- * check for failure above
  -- * put data on statusline rather than echo line
  --   + vim.api.nvim_set_option('statusline', addr)
  -- * make statusline sub-variable to work within AirLine plugin
  -- * parse and validate data?
end


local function status_listen()
  socket = vim.loop.new_udp()
  local result = vim.loop.udp_bind(socket, "127.0.0.1", 10001)
  if (result == nil) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: could not bind"))
  end
  vim.loop.udp_recv_start(socket, status_update)
end

local function test_status_listen()
  local content = "this is the truth"
  local socket = vim.loop.new_udp()
  local bytes = vim.loop.udp_try_send(socket, content, "127.0.0.1", 10001)
  if (bytes == nil) then
    vim.api.nvim_command(string.format('echo "FAIL"'))
  end
end


--------------------------------------------------------------------------------
-- Export API and other notes --------------------------------------------------
--------------------------------------------------------------------------------


-- export the API of this plugin
-- 
return {
  status_listen = status_listen,
  buffer_listen = buffer_listen,
  test_status_listen = test_status_listen,
}
-- Make NeoVim commands to call these exported functions:
--
-- command! BufferListen lua require"clavm".buffer_listen()
-- command! StatusListen lua require"clavm".status_listen()
-- command! TestStatusListen lua require"clavm".test_status_listen()
--


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
