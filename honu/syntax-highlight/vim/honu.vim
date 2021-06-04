syn keyword ControlFlow Debut Fin Si Sinon Alors Tant que Faire while do if then else true false vrai faux
syn keyword ConstantLang Avance Tourne HautPinceau BasPinceau Affiche ChangeEpaisseur ChangeCouleur forward turn up down print size color write push pop
syn keyword StorageType Proc proc Var var
syn keyword Not not Non
syn match Operators "\(+\|*\|-\|/\|=\|&\||\|>\|<\|!\)"
syn match Delimiters "\((\|)\|{\|}\|,\)"
syn match Numbers "\d\d*"
syn match Comments "\(\#\|\~|\).*$"
syn match Strings "\".*\""
syn match Functions "[a-zA-Z][a-zA-Z0-9]*\((\)\@="
syn match Variables "[a-zA-Z][a-zA-Z0-9]*\([a-zA-Z0-9]*(\)\@!"

hi def link ControlFlow Statement
hi def link ConstantLang PreProc
hi def link StorageType Structure
hi def link Operators Operator
hi def link Not Operator
hi def link Numbers Number
hi def link Variables Identifier
hi def link Functions Function
hi def link Comments Comment
hi def link SpeComments Comment
hi def link Strings String
