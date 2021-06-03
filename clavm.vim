set runtimepath+=./
autocmd TextChanged,TextChangedI *.c lua require"clavm".send_code()

" try:
"
" nvim -u clavm.vim play/sine.c
"
