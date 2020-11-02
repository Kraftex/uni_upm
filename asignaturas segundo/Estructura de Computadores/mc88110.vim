" Vim syntax file
" Language: Assembly for MC88110 Motorola
" Maintainer: Julian C.
" Version: 2.1
" Lastest Revision: 02 November 2020

if exists("b:current_syntax")
    finish
endif

" Keywords, CCR?
syn keyword mc88110KeysCompFunc low high
syn keyword mc88110KeysComp data res org stop
syn keyword mc88110Keys eq ne gt lt ge le
syn keyword mc88110Keys eq0 ne0 gt0 lt0 ge0 le0

" Valid labels
syn case ignore
syn match mc88110Label "^[a-z_?.][a-z0-9_?.$]*$"
syn match mc88110Label "^[a-z_?.][a-z0-9_?.$]*\s"he=e-1
syn match mc88110Label "^\s*[a-z_?.][a-z0-9_?.$]*:"he=e-1
syn case match

" Formats
syn match decNumber "\<[0-9]\+\>"
syn match hexNumber "\<0x[0-9a-fA-F]\+\>"

" Macro definition
syn match mc88110MacroBegin "\<MACRO\>"
syn match mc88110Macro "^[a-zA-Z0-9_]\+:[ ]*MACRO" contains=mc88110MacroBegin
syn match mc88110MacroEnd "\<ENDMACRO\>"
syn match mc88110MacroPart "([a-zA-Z0-9_, ]*)" contains=decNumber,hexNumber
syn match mc88110MacroUse "[a-zA-Z0-9_]\+[ ]*([a-zA-Z0-9_, ]*)" contains=mc88110MacroPart

" Matches
syn match mc88110Opcode "\<add\(\.c[io]\|.cio\)\=\s"
syn match mc88110Opcode "\<addu\(\.c[io]\|.cio\)\=\s"
syn match mc88110Opcode "\<and\(\.[cu]\)\=\s"
syn match mc88110Opcode "\<bb[01]\(\.n\)\=\s"
syn match mc88110Opcode "\<bcnd\(\.n\)\=\s"
syn match mc88110Opcode "\<br\(\.n\)\=\s"
syn match mc88110Opcode "\<bsr\(\.n\)\=\s"
syn match mc88110Opcode "\<\(cmp\)\=\s"
syn match mc88110Opcode "\<\(divs\)\=\s"
syn match mc88110Opcode "\<divu\(\.d\)\=\s"
syn match mc88110Opcode "\<extu\=\s"
syn match mc88110Opcode "\<fadd\.\([sdx]\)\{3}\s"
syn match mc88110Opcode "\<fcmpu\=\.s\([sdx]\)\{2}\s"
syn match mc88110Opcode "\<fcvt\.\([sdx]\)\{2}\s"
syn match mc88110Opcode "\<fdiv\.\([sdx]\)\{3}\s"
syn match mc88110Opcode "\<ff[01]\s"
syn match mc88110Opcode "\<fldcr\s"
syn match mc88110Opcode "\<flt\.[sdx]s\s"
syn match mc88110Opcode "\<fmul\.\([sdx]\)\{3}\s"
syn match mc88110Opcode "\<fsqrt\.\([sdx]\)\{2}\s"
syn match mc88110Opcode "\<fstcr\s"
syn match mc88110Opcode "\<fsub\.\([sdx]\)\{3}\s"
syn match mc88110Opcode "\<fxcr\s"
syn match mc88110Opcode "\<int\.s[sdx]\s"
syn match mc88110Opcode "\<jmp\(\.n\)\=\s"
syn match mc88110Opcode "\<jsr\(\.n\)\=\s"
syn match mc88110Opcode "\<ld\(\.[bh]u\=\|\.[dx]\)\=\(\.usr\)\=\s"
syn match mc88110Opcode "\<lda\(\.[hdx]\)\=\s"
syn match mc88110Opcode "\<ldcr\s"
syn match mc88110Opcode "\<mak\s"
syn match mc88110Opcode "\<mask\(\.u\)\=\s"
syn match mc88110Opcode "\<mov\(\.[ds]\)\=\s"
syn match mc88110Opcode "\<mul\(s\|u\(\.d\)\=\)\s"
syn match mc88110Opcode "\<nint\.s[sdx]\s"
syn match mc88110Opcode "\<or\(\.[cu]\)\=\s"
syn match mc88110Opcode "\<padd\(\.[bh]\)\=\s"
syn match mc88110Opcode "\<padds\.\([us]\|us\)\(\.[bh]\)\=\s"
syn match mc88110Opcode "\<pcmp\s"
syn match mc88110Opcode "\<pmul\s"
syn match mc88110Opcode "\<ppack\.\(8\|16\(\.h\)\|32\(\.[bh]\)\)\s"
syn match mc88110Opcode "\<prot\s"
syn match mc88110Opcode "\<psub\(\.[bh]\)\=\s"
syn match mc88110Opcode "\<psubs\.\([us]\|us\)\(\.[bh]\)\=\s"
syn match mc88110Opcode "\<psub\.[bhn]\s"
syn match mc88110Opcode "\<rot\s"
syn match mc88110Opcode "\<ret\s"
syn match mc88110Opcode "\<set\s"
syn match mc88110Opcode "\<st\(\.[bh]\|\.[dx]\)\=\(\.usr\)\=\(\.wt\)\=\s"
syn match mc88110Opcode "\<stcr\s"
syn match mc88110Opcode "\<sub\(\.c[io]\|.cio\)\=\s"
syn match mc88110Opcode "\<subu\(\.c[io]\|.cio\)\=\s"
syn match mc88110Opcode "\<tb[01]\s"
syn match mc88110Opcode "\<tbnd\s"
syn match mc88110Opcode "\<tcnd\s"
syn match mc88110Opcode "\<trnc\.s[dsx]\s"
syn match mc88110Opcode "\<xcr\s"
syn match mc88110Opcode "\<xmem\(\.bu\)\=\(\.usr\)\=\s"
syn match mc88110Opcode "\<xor\(\.[cu]\)\=\s"

" Special items for comments
syn match mc88110Todo       "TODO"
syn region mc88110Todo      start="TODO:" end="$" 

" Comments
syn match mc88110Comment    ";.*" contains=mc88110Todo
syn match mc88110Comment    "\s!.*"ms=s+1 contains=mc88110Todo
syn match mc88110Comment    "^\s*[*!].*" contains=mc88110Todo

" Highlighting
hi def link mc88110Todo         Todo
hi def link mc88110Comment      Comment

hi def link hexNumber		    Number
hi def link decNumber		    Number

hi def link mc88110Keys         Identifier
hi def link mc88110KeysComp     Special
hi def link mc88110KeysCompFunc Function
hi def link mc88110Label        Type
hi def link mc88110Macro        Macro
hi def link mc88110MacroBegin   Special
hi def link mc88110MacroEnd     Macro
hi def link mc88110MacroUse     Macro
hi def link mc88110Opcode       Statement

let b:current_syntax = "mc88110"
