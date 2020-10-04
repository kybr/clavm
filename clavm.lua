-- Karl Yerkes
-- 2020-10-03
-- C.L.A.V.M.
--

-- TODO:
-- The overall architecture of this plugin needs more thought; Can we use one
-- UDP socket that we create upfront? does this script run per-buffer or does
-- it work for many buffers provided the user only changes one buffer at a time?
--
--

local socket = nil -- use the same socket for all

--------------------------------------------------------------------------------
-- Listen for buffer changes; Spurt contents via UDP 127.0.0.1:9999 ------------
--------------------------------------------------------------------------------

-- local function text_change(_, bufnr, changedtick, firstline, lastline, new_lastline, old_byte_size, old_utf32_size, old_utf16_size)
local function text_change(_, bufnr)

  -- TODO: throttle here; return quick if no actual change
  -- probably hashing is the only way.

  -- get all the lines in the current buffer and make a single string
  --
  local lines = vim.api.nvim_buf_get_lines(bufnr, 0, -1, true)
  local content = table.concat(lines, "\n")

  -- TODO: possibly prepend metadata here
  -- TODO: check that the string is no more than 65507 bytes

  -- send a datagram with the content
  --
  local bytes = vim.loop.udp_try_send(socket, content, "127.0.0.1", 9999)
  if (bytes == nil) then
    -- TODO: handle this; or not. why would it fail?
  end
end

local function buffer_listen()
  -- TODO: check the current buffer number and fail if we already listen
  local result = vim.api.nvim_buf_attach(0, false, {on_lines = text_change})
  if (result == nil) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: could not attach"))
  end
end

-- TODO
local function buffer_ignore()
end


--------------------------------------------------------------------------------
-- Listen for status information on UDP 10001; Show on status line -------------
--------------------------------------------------------------------------------

local function status_update(err, data, addr, flags)

  -- this callback is called twice for each packet (why?) and gets no arguments
  -- the second time it is called
  if (data == nil) then
    return
  end

  -- XXX: can this fail?
  vim.schedule(function()
    vim.api.nvim_command(string.format('echo "%s"', data))
  end)
  
  -- TODO:
  -- * make statusline sub-variable
  -- * parse and validate data
  -- * put data on statusline
  -- vim.api.nvim_set_option('statusline', addr)
end

--------------------------------------------------------------------------------
-- Set up CLAVM ----------------------------------------------------------------
--------------------------------------------------------------------------------

local function test()
  local content = "this is the truth"
  local socket = vim.loop.new_udp()
  local bytes = vim.loop.udp_try_send(socket, content, "127.0.0.1", 10001)
  if (bytes == nil) then
    vim.api.nvim_command(string.format('echo "FAIL"'))
  end
end

local function status_listen()
  socket = vim.loop.new_udp()
  local result = vim.loop.udp_bind(socket, "127.0.0.1", 10001)
  if (result == nil) then
    vim.api.nvim_command(string.format('echo "%s"', "FAIL: could not bind"))
  end
  vim.loop.udp_recv_start(socket, status_update)
end


-- export the API of this plugin
--
return {
  status_listen = status_listen,
  buffer_listen = buffer_listen,
}


-- Notes
--
-- not allowed to call nvim_command or the like in vim.loop context
--
-- lua print(vim.inspect(vim.loop))
--
-- https://github.com/luvit/luv/blob/master/docs.md
-- https://neovim.io/doc/user/lua.html
-- https://dev.to/jamestthompson3/using-libuv-in-neovim-900
-- https://dev.to/2nit/how-to-write-neovim-plugins-in-lua-5cca
