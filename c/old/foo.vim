function! TryCode()
  let l:message = system('./client', getline(0, line('$')))

  " if there's a message, post it
  if l:message != ''
    echohl WarningMsg | echomsg 'Client: ' . l:message | echohl None
  else
    " if there's no message, but there's an error...
    if v:shell_error != 0
      echohl Error | echomsg 'Client: CRASH!' | echohl None
    endif
  endif
endfunction

" Notes:
" Vim’s event model by Damian Conway (2010)
"   https://developer.ibm.com/tutorials/l-vim-script-5/
"
" autocmd  EventName  filename_pattern   :command
"
autocmd TextChanged,TextChangedI *.c :call TryCode()
