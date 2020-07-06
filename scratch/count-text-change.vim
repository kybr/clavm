let g:count = 0

function! g:ChangedText()
  let g:count += 1
  echo g:count
endfunction

autocmd TextChanged,TextChangedI * :call ChangedText()
