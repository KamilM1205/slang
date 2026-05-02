" Vim indent file for slang
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=GetSlangIndent()
setlocal indentkeys=0{,0},0),0],!^F,o,O

function! GetSlangIndent()
  let prevlnum = prevnonblank(v:lnum-1)
  let prevline = getline(prevlnum)
  let curline = getline(v:lnum)

  " Начальный отступ предыдущей строки
  let ind = indent(prevlnum)

  " Увеличиваем отступ, если предыдущая строка заканчивается на '{' или '(', или если это начало блока (if, for, fn и т.п.)
  if prevline =~ '[{(]\s*$' || prevline =~ '\<\(if\|for\|fn\|while\)\>'
    let ind += shiftwidth()
  endif

  " Уменьшаем отступ, если текущая строка начинается с '}' или ')'
  if curline =~ '^\s*[})]'
    let ind -= shiftwidth()
  endif

  return ind
endfunction
