--
-- NeoVim plugin for C.L.A.V.M.
--
-- Karl Yerkes
-- 2020-10-03
--


local socket_send = nil

--------------------------------------------------------------------------------
-- Listen for buffer changes; Spurt contents via UDP 127.0.0.1:11000 -----------
--------------------------------------------------------------------------------

-- local function text_change(_, bufnr, changedtick, firstline, lastline, new_lastline, old_byte_size, old_utf32_size, old_utf16_size)
local function text_change(_, bufnr)

  vim.schedule(function()
    vim.api.nvim_command(string.format('echo "text changed"'))
  end)

  -- get all the lines in the current buffer and make a single string
  --
  local lines = vim.api.nvim_buf_get_lines(bufnr, 0, -1, true)
  local content = table.concat(lines, "\n")

  -- TODO: maybe throttle here; return quick if the text did not actually change
  -- probably hashing is the only way to detect this.

  -- TODO: possibly prepend metadata here; maybe buffer number?

  -- prepend an OSC header like this:
  -- XX XX XX XX NN NN NN NN CC BB 00 00 ZZ ZZ ZZ ZZ [16 bytes]
  --  c  o  d  e \0 \0 \0 \0  ,  b \0 \0 (data size)
  -- data size: big-endian 32-bit int
  --
  local length = string.len(content)
  local bits = string.char(
    bit.band(bit.rshift(length, 24), 255),
    bit.band(bit.rshift(length, 16), 255),
    bit.band(bit.rshift(length, 8), 255),
    bit.band(length, 255))
  local data = "code\0\0\0\0,b\0\0" .. bits .. content

  -- check that the string is no more than 65507 bytes total
  if (string.len(data) > 65507) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: code too big"))
    return
  end

  if (socket_send == nil) then
    socket_send = vim.loop.new_udp()
  end

  -- send a datagram with the content to the server
  local bytes = vim.loop.udp_try_send(socket_send, data, "127.0.0.1", 11000)
  if (bytes == nil) then
    -- depends on OS settings; fails at 9216 bytes for me
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: udp send"))
  end
end

local function buffer_listen()
  -- TODO: check the current buffer number and fail if we already listen

  -- attach a listener to the current buffer (0 means current buffer)
  local result = vim.api.nvim_buf_attach(0, false, {on_lines = text_change})
  if (result == nil) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: could not attach"))
  end
  vim.schedule(function()
    vim.api.nvim_command(string.format('echo "listening to buffer"'))
  end)
end


local function nothing(_, bufnr)
end

local function buffer_ignore()
  local result = vim.api.nvim_buf_attach(0, false, {on_lines = nothing})
  if (result == nil) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: could not detach"))
  end
  vim.schedule(function()
    vim.api.nvim_command(string.format('echo "ignoring buffer"'))
  end)
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

  -- TODO: strip OSC metadata shaped like these 16 bytes:
  -- XX XX XX XX NN NN NN NN CC BB 00 00 ZZ ZZ ZZ ZZ
  --  r  p  l  y \0 \0 \0 \0  ,  b \0 \0 (data size)
  -- data size: big-endian 32-bit int

--  if (not string.find(data, "rply")) then
--    return
--  end

  -- actually, we just ignore the first 16 bytes; we know the size
  local content = string.sub(data, 15)

  -- TODO: join multiple lines?

  vim.schedule(function()
    vim.api.nvim_command(string.format('echo "%s"', content))
  end)
  
  -- TODO:
  -- * check for failure above
  -- * put data on statusline rather than echo line
  --   + vim.api.nvim_set_option('statusline', addr)
  -- * make statusline sub-variable to work within AirLine plugin
  -- * parse and validate data?
end

local socket_receive = nil

local function status_listen()
  if (socket_receive) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: already bound"))
    return
  end

  socket_receive = vim.loop.new_udp()
  local result = vim.loop.udp_bind(socket_receive, "127.0.0.0", 10001)
  if (result == nil) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: could not bind"))
  end
  vim.loop.udp_recv_start(socket_receive, status_update)
end

local function status_ignore()
  if (socket_receive) then
    vim.loop.close(socket_receive)
  end
end

local function test_status_listen()
  local content = "rply\0\0\0\0,b\0\0\0\0this is the truth\na second line here"
  -- local content = "this is the truth"
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
  buffer_ignore = buffer_ignore,
  test_status_listen = test_status_listen,
}
-- Make NeoVim commands to call these exported functions:
--
-- command! BufferListen lua require"clavm".buffer_listen()
-- command! StatusListen lua require"clavm".status_listen()
-- command! TestStatusListen lua require"clavm".test_status_listen()
--


-- " ----------------------
-- " C.L.A.V.M.
-- " ----------------------
-- " load and start CLAVM
-- "lua require'clavm'.status_listen()
-- command! BufferListen lua require"clavm".buffer_listen()
-- command! BufferIgnore lua require"clavm".buffer_ignore()
-- command! StatusListen lua require"clavm".status_listen()
-- command! StatusIgnore lua require"clavm".status_ignore()
-- "command! TestStatusListen lua require"clavm".test_status_listen()
-- then:
-- :BufferListen
-- :BufferIgnore


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
