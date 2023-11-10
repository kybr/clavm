# Per-Sample C

**Install**:
- vim (v8.1+) or neovim (v0.4+)
- 


- `process` callback is called once per sample at ~44100 Hertz
- `o[0]` and `o[1]` are left and right outputs, respectively
  + generally, stay in (-1, 1)
- `double t` is time is seconds since the server started
