" 插件管理器会从这里开始加载插件
call plug#begin('~/.vim/plugged')

" 推荐的新手插件套装
Plug 'preservim/nerdtree'
Plug 'neoclide/coc.nvim', {'branch': 'release'}
Plug 'vim-airline/vim-airline'

" 所有插件声明在这里结束
call plug#end()
