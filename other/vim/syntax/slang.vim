" Vim syntax file
" Language: slang
" Generated from BNF notation

if exists("b:current_syntax")
  finish
endif

" Ключевые слова
syn keyword slangKeyword import class interface fn return for while if elif else var
syn keyword slangBoolean true false
syn keyword slangOperator and or

" Типы
syn keyword slangType int float bool string ustring
syn match slangCustomType /\<[A-Za-z_][A-Za-z0-9_]*\>/ contained
syn region slangTypeDecl start=/:/ end=/\s/ contains=slangType,slangCustomType

" Строки
syn region slangString start=/\[\[/ end=/\]\]/
syn region slangString start=/"/ end=/"/ skip=/\\"/

" Числа
syn match slangNumber /\<[+-]\?\d\+\>/
syn match slangFloat /\<[+-]\?\d\+\.\d\+\>/
syn match slangFloat /\<[+-]\?\.\d\+\>/

" Операторы
syn match slangOperator /[+\-*\/%]/
syn match slangOperator /[=!><]=*/
syn match slangOperator /[?.:;,(){}]/
syn match slangCompoundOperator /[+\-*\/%]=/
syn match slangIncrementOperator /[+\-][+\-]/

" Комментарии (предполагаем C-стиль)
syn region slangComment start="\/\/" end="$"
syn region slangComment start="\/\*" end="\\*\/"

" Функции
syn match slangFunction /\<[a-zA-Z_][a-zA-Z0-9_]*\s*(/me=e-1
syn match slangMethodCall /\<[a-zA-Z_][a-zA-Z0-9_]*\.\s*[a-zA-Z_][a-zA-Z0-9_]*\s*(/me=e-1

" Классы и интерфейсы
syn match slangClassDecl /\<class\s\+[a-zA-Z_][a-zA-Z0-9_]*/ contains=slangKeyword
syn match slangInterfaceDecl /\<interface\s\+[a-zA-Z_][a-zA-Z0-9_]*/ contains=slangKeyword

" Подсветка
hi def link slangKeyword Keyword
hi def link slangType Type
hi def link slangCustomType Type
hi def link slangBoolean Boolean
hi def link slangString String
hi def link slangNumber Number
hi def link slangFloat Float
hi def link slangOperator Operator
hi def link slangCompoundOperator Operator
hi def link slangIncrementOperator Operator
hi def link slangComment Comment
hi def link slangFunction Function
hi def link slangMethodCall Function
hi def link slangClassDecl Structure
hi def link slangInterfaceDecl Structure
hi def link slangTypeDecl Typedef

let b:current_syntax = "slang"
