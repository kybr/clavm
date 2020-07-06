"set noshowmode

function! DoThings()
  "let l:message = split("\n", system('./a.out', getline(0, line('$'))))
  let l:message = join(systemlist('./a.out', getline(0, line('$'))), '|')

  "echo 'len ' . len(l:message)
  "echo 'type ' . type(l:message)
  echo 'got ' . l:message

" redraw!
endfunction

