set runtimepath+=./
autocmd TextChanged,TextChangedI *.c lua require"clavm".send_code()


command! StatusListen lua require"clavm".status_listen()
command! StatusIgnore lua require"clavm".status_ignore()
command! TestStatusListen lua require"clavm".test_status_listen()

" try:
"
" nvim -u clavm.vim play/sine.c
"
