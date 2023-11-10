set noshowmode

function! TryCode()
  redraw!

  " trim takes the \n off the end of the message which makes the echo 
  " work without waiting for a user response
  let l:message = trim(system('./per-sample-c-client', getline(0, line('$'))))

  if l:message != ''
    echohl WarningMsg | echo 'C ' . l:message | echohl None
    "echohl WarningMsg | echo 'Per-Sample C: ' . l:message | echohl None
  elseif v:shell_error != 0
    " TODO get the shell text from the crash
    echohl Error | echo 'C ' . v:shell_error | echohl None
    "echohl Error | echo 'Per-Sample C: CRASH! ' . v:shell_error | echohl None
  endif
endfunction

" Notes:
" Vim’s event model by Damian Conway (2010)
"   https://developer.ibm.com/tutorials/l-vim-script-5/
"
" autocmd  EventName  filename_pattern   :command
"


autocmd TextChanged,TextChangedI *.c :call TryCode()
"autocmd CursorHold,CursorHoldI *.c :call TryCode()
"autocmd BufWrite *.c :call TryCode()





