.text:00971150                 push    ebp
.text:00971151                 mov     ebp, esp
.text:00971153                 sub     esp, 0Ch
.text:00971156                 push    ebx
.text:00971157                 push    esi
.text:00971158                 push    edi
.text:00971159                 mov     [ebp-8], ecx
.text:0097115C                 jo      short loc_971161
.text:0097115E                 jno     short loc_971161
.text:0097115E ; ---------------------------------------------------------------------------
.text:00971160                 db 0E9h
.text:00971161 ; ---------------------------------------------------------------------------
.text:00971161
.text:00971161 loc_971161:                             ; CODE XREF: .text:0097115C↑j
.text:00971161                                         ; .text:0097115E↑j
.text:00971161                 mov     dword ptr [ebp-0Ch], 0
.text:00971168                 jnb     short loc_97116D
.text:0097116A                 jb      short loc_97116D
.text:0097116A ; ---------------------------------------------------------------------------
.text:0097116C                 db 0E8h
.text:0097116D ; ---------------------------------------------------------------------------
.text:0097116D
.text:0097116D loc_97116D:                             ; CODE XREF: .text:00971168↑j
.text:0097116D                                         ; .text:0097116A↑j
.text:0097116D                 mov     eax, large fs:30h
.text:00971173                 xor     ebx, ebx
.text:00971175                 mov     bl, [eax+2]
.text:00971178                 xor     dword_976018, ebx
.text:0097117E                 mov     ecx, [ebp-8]
.text:00971181                 mov     esi, 13h
.text:00971186                 movsx   eax, byte ptr [ecx]
.text:00971189                 cdq
.text:0097118A                 idiv    esi
.text:0097118C                 movsx   eax, byte ptr [ecx+1]
.text:00971190                 mov     ecx, 1
.text:00971195                 add     edx, eax
.text:00971197                 mov     eax, 0
.text:0097119C                 cmp     edx, dword_976018
.text:009711A2                 cmovz   eax, ecx
.text:009711A5                 mov     [ebp-4], eax
.text:009711A8                 jz      short loc_9711AD
.text:009711AA                 jnz     short loc_9711AD
.text:009711AA ; ---------------------------------------------------------------------------
.text:009711AC                 db 0E9h
.text:009711AD ; ---------------------------------------------------------------------------
.text:009711AD
.text:009711AD loc_9711AD:                             ; CODE XREF: .text:009711A8↑j
.text:009711AD                                         ; .text:009711AA↑j
.text:009711AD                 mov     eax, large fs:30h
.text:009711B3                 xor     ebx, ebx
.text:009711B5                 mov     bl, [eax+2]
.text:009711B8                 xor     dword_976018+1, ebx
.text:009711BE                 mov     eax, [ebp-8]
.text:009711C1                 movsx   ecx, byte ptr [eax+2]
.text:009711C5                 movsx   eax, byte ptr [eax]
.text:009711C8                 add     eax, 13h
.text:009711CB                 add     eax, ecx
.text:009711CD                 cmp     eax, dword_97601C
.text:009711D3                 jnz     short loc_9711D8
.text:009711D5                 inc     dword ptr [ebp-4]
.text:009711D8
.text:009711D8 loc_9711D8:                             ; CODE XREF: .text:009711D3↑j
.text:009711D8                 ja      short loc_9711DD
.text:009711DA                 jbe     short loc_9711DD
.text:009711DA ; ---------------------------------------------------------------------------
.text:009711DC                 db 0E8h
.text:009711DD ; ---------------------------------------------------------------------------
.text:009711DD
.text:009711DD loc_9711DD:                             ; CODE XREF: .text:loc_9711D8↑j
.text:009711DD                                         ; .text:009711DA↑j
.text:009711DD                 mov     eax, large fs:30h
.text:009711E3                 xor     ebx, ebx
.text:009711E5                 mov     bl, [eax+2]
.text:009711E8                 xor     dword_976018+2, ebx
.text:009711EE                 mov     eax, [ebp-8]
.text:009711F1                 movsx   ecx, byte ptr [eax]
.text:009711F4                 movsx   eax, byte ptr [eax+3]
.text:009711F8                 imul    ecx, 13h
.text:009711FB                 add     ecx, eax
.text:009711FD                 cmp     ecx, dword_976020
.text:00971203                 jnz     short loc_971208
.text:00971205                 inc     dword ptr [ebp-4]
.text:00971208
.text:00971208 loc_971208:                             ; CODE XREF: .text:00971203↑j
.text:00971208                 js      short loc_97120D
.text:0097120A                 jns     short loc_97120D
.text:0097120A ; ---------------------------------------------------------------------------
.text:0097120C                 db 0E9h
.text:0097120D ; ---------------------------------------------------------------------------
.text:0097120D
.text:0097120D loc_97120D:                             ; CODE XREF: .text:loc_971208↑j
.text:0097120D                                         ; .text:0097120A↑j
.text:0097120D                 mov     eax, large fs:30h
.text:00971213                 xor     ebx, ebx
.text:00971215                 mov     bl, [eax+2]
.text:00971218                 xor     dword_976018+3, ebx
.text:0097121E                 mov     ecx, [ebp-8]
.text:00971221                 mov     esi, 14h
.text:00971226                 movsx   eax, byte ptr [ecx+4]
.text:0097122A                 cdq
.text:0097122B                 idiv    esi
.text:0097122D                 movsx   eax, byte ptr [ecx+5]
.text:00971231                 add     edx, eax
.text:00971233                 cmp     edx, dword_976024
.text:00971239                 jnz     short loc_97123E
.text:0097123B                 inc     dword ptr [ebp-4]
.text:0097123E
.text:0097123E loc_97123E:                             ; CODE XREF: .text:00971239↑j
.text:0097123E                 jnp     short loc_971243
.text:00971240                 jp      short loc_971243
.text:00971240 ; ---------------------------------------------------------------------------
.text:00971242                 db 0E8h
.text:00971243 ; ---------------------------------------------------------------------------
.text:00971243
.text:00971243 loc_971243:                             ; CODE XREF: .text:loc_97123E↑j
.text:00971243                                         ; .text:00971240↑j
.text:00971243                 mov     eax, large fs:30h
.text:00971249                 xor     ebx, ebx
.text:0097124B                 mov     bl, [eax+2]
.text:0097124E                 xor     dword_97601C, ebx
.text:00971254                 mov     eax, [ebp-8]
.text:00971257                 movsx   ecx, byte ptr [eax+4]
.text:0097125B                 movsx   eax, byte ptr [eax+6]
.text:0097125F                 add     eax, 14h
.text:00971262                 add     eax, ecx
.text:00971264                 cmp     eax, dword_976028
.text:0097126A                 jnz     short loc_97126F
.text:0097126C                 inc     dword ptr [ebp-4]
.text:0097126F
.text:0097126F loc_97126F:                             ; CODE XREF: .text:0097126A↑j
.text:0097126F                 jl      short loc_971274
.text:00971271                 jge     short loc_971274
.text:00971271 ; ---------------------------------------------------------------------------
.text:00971273                 db 0E9h
.text:00971274 ; ---------------------------------------------------------------------------
.text:00971274
.text:00971274 loc_971274:                             ; CODE XREF: .text:loc_97126F↑j
.text:00971274                                         ; .text:00971271↑j
.text:00971274                 mov     eax, large fs:30h
.text:0097127A                 xor     ebx, ebx
.text:0097127C                 mov     bl, [eax+2]
.text:0097127F                 xor     dword_97601C+1, ebx
.text:00971285                 mov     eax, [ebp-8]
.text:00971288                 movsx   ecx, byte ptr [eax+4]
.text:0097128C                 movsx   eax, byte ptr [eax+7]
.text:00971290                 lea     ecx, [ecx+ecx*4]
.text:00971293                 lea     eax, [eax+ecx*4]
.text:00971296                 cmp     eax, dword_97602C
.text:0097129C                 jnz     short loc_9712A1
.text:0097129E                 inc     dword ptr [ebp-4]
.text:009712A1
.text:009712A1 loc_9712A1:                             ; CODE XREF: .text:0097129C↑j
.text:009712A1                 jg      short loc_9712A6
.text:009712A3                 jle     short loc_9712A6
.text:009712A3 ; ---------------------------------------------------------------------------
.text:009712A5                 db 0E8h
.text:009712A6 ; ---------------------------------------------------------------------------
.text:009712A6
.text:009712A6 loc_9712A6:                             ; CODE XREF: .text:loc_9712A1↑j
.text:009712A6                                         ; .text:009712A3↑j
.text:009712A6                 mov     eax, large fs:30h
.text:009712AC                 xor     ebx, ebx
.text:009712AE                 mov     bl, [eax+2]
.text:009712B1                 xor     dword_97601C+2, ebx
.text:009712B7                 mov     ecx, [ebp-8]
.text:009712BA                 mov     esi, 15h
.text:009712BF                 movsx   eax, byte ptr [ecx+8]
.text:009712C3                 cdq
.text:009712C4                 idiv    esi
.text:009712C6                 movsx   eax, byte ptr [ecx+9]
.text:009712CA                 add     edx, eax
.text:009712CC                 cmp     edx, dword_976030
.text:009712D2                 jnz     short loc_9712D7
.text:009712D4                 inc     dword ptr [ebp-4]
.text:009712D7
.text:009712D7 loc_9712D7:                             ; CODE XREF: .text:009712D2↑j
.text:009712D7                 jo      short loc_9712DC
.text:009712D9                 jno     short loc_9712DC
.text:009712D9 ; ---------------------------------------------------------------------------
.text:009712DB                 db 0E9h
.text:009712DC ; ---------------------------------------------------------------------------
.text:009712DC
.text:009712DC loc_9712DC:                             ; CODE XREF: .text:loc_9712D7↑j
.text:009712DC                                         ; .text:009712D9↑j
.text:009712DC                 mov     eax, large fs:30h
.text:009712E2                 xor     ebx, ebx
.text:009712E4                 mov     bl, [eax+2]
.text:009712E7                 xor     dword_97601C+3, ebx
.text:009712ED                 mov     eax, [ebp-8]
.text:009712F0                 movsx   ecx, byte ptr [eax+8]
.text:009712F4                 movsx   eax, byte ptr [eax+0Ah]
.text:009712F8                 add     eax, 15h
.text:009712FB                 add     eax, ecx
.text:009712FD                 cmp     eax, dword_976034
.text:00971303                 jnz     short loc_971308
.text:00971305                 inc     dword ptr [ebp-4]
.text:00971308
.text:00971308 loc_971308:                             ; CODE XREF: .text:00971303↑j
.text:00971308                 jnb     short loc_97130D
.text:0097130A                 jb      short loc_97130D
.text:0097130A ; ---------------------------------------------------------------------------
.text:0097130C                 db 0E8h
.text:0097130D ; ---------------------------------------------------------------------------
.text:0097130D
.text:0097130D loc_97130D:                             ; CODE XREF: .text:loc_971308↑j
.text:0097130D                                         ; .text:0097130A↑j
.text:0097130D                 mov     eax, large fs:30h
.text:00971313                 xor     ebx, ebx
.text:00971315                 mov     bl, [eax+2]
.text:00971318                 xor     dword_976020, ebx
.text:0097131E                 mov     eax, [ebp-8]
.text:00971321                 movsx   ecx, byte ptr [eax+8]
.text:00971325                 movsx   eax, byte ptr [eax+0Bh]
.text:00971329                 imul    ecx, 15h
.text:0097132C                 add     ecx, eax
.text:0097132E                 cmp     ecx, dword_976038
.text:00971334                 jnz     short loc_971339
.text:00971336                 inc     dword ptr [ebp-4]
.text:00971339
.text:00971339 loc_971339:                             ; CODE XREF: .text:00971334↑j
.text:00971339                 jz      short loc_97133E
.text:0097133B                 jnz     short loc_97133E
.text:0097133B ; ---------------------------------------------------------------------------
.text:0097133D                 db 0E9h
.text:0097133E ; ---------------------------------------------------------------------------
.text:0097133E
.text:0097133E loc_97133E:                             ; CODE XREF: .text:loc_971339↑j
.text:0097133E                                         ; .text:0097133B↑j
.text:0097133E                 mov     eax, large fs:30h
.text:00971344                 xor     ebx, ebx
.text:00971346                 mov     bl, [eax+2]
.text:00971349                 xor     dword_976020+1, ebx
.text:0097134F                 mov     ecx, [ebp-8]
.text:00971352                 mov     esi, 16h
.text:00971357                 movsx   eax, byte ptr [ecx+0Ch]
.text:0097135B                 cdq
.text:0097135C                 idiv    esi
.text:0097135E                 movsx   eax, byte ptr [ecx+0Dh]
.text:00971362                 add     edx, eax
.text:00971364                 cmp     edx, dword_97603C
.text:0097136A                 jnz     short loc_97136F
.text:0097136C                 inc     dword ptr [ebp-4]
.text:0097136F
.text:0097136F loc_97136F:                             ; CODE XREF: .text:0097136A↑j
.text:0097136F                 ja      short loc_971374
.text:00971371                 jbe     short loc_971374
.text:00971371 ; ---------------------------------------------------------------------------
.text:00971373                 db 0E8h
.text:00971374 ; ---------------------------------------------------------------------------
.text:00971374
.text:00971374 loc_971374:                             ; CODE XREF: .text:loc_97136F↑j
.text:00971374                                         ; .text:00971371↑j
.text:00971374                 mov     eax, large fs:30h
.text:0097137A                 xor     ebx, ebx
.text:0097137C                 mov     bl, [eax+2]
.text:0097137F                 xor     dword_976020+2, ebx
.text:00971385                 mov     eax, [ebp-8]
.text:00971388                 movsx   ecx, byte ptr [eax+0Ch]
.text:0097138C                 movsx   eax, byte ptr [eax+0Eh]
.text:00971390                 add     eax, 16h
.text:00971393                 add     eax, ecx
.text:00971395                 cmp     eax, dword_976040
.text:0097139B                 jnz     short loc_9713A0
.text:0097139D                 inc     dword ptr [ebp-4]
.text:009713A0
.text:009713A0 loc_9713A0:                             ; CODE XREF: .text:0097139B↑j
.text:009713A0                 js      short loc_9713A5
.text:009713A2                 jns     short loc_9713A5
.text:009713A2 ; ---------------------------------------------------------------------------
.text:009713A4                 db 0E9h
.text:009713A5 ; ---------------------------------------------------------------------------
.text:009713A5
.text:009713A5 loc_9713A5:                             ; CODE XREF: .text:loc_9713A0↑j
.text:009713A5                                         ; .text:009713A2↑j
.text:009713A5                 mov     eax, large fs:30h
.text:009713AB                 xor     ebx, ebx
.text:009713AD                 mov     bl, [eax+2]
.text:009713B0                 xor     dword_976020+3, ebx
.text:009713B6                 mov     eax, [ebp-8]
.text:009713B9                 movsx   ecx, byte ptr [eax+0Ch]
.text:009713BD                 movsx   eax, byte ptr [eax+0Fh]
.text:009713C1                 imul    ecx, 16h
.text:009713C4                 add     ecx, eax
.text:009713C6                 cmp     ecx, dword_976044
.text:009713CC                 jnz     short loc_9713D1
.text:009713CE                 inc     dword ptr [ebp-4]
.text:009713D1
.text:009713D1 loc_9713D1:                             ; CODE XREF: .text:009713CC↑j
.text:009713D1                 jnp     short loc_9713D6
.text:009713D3                 jp      short loc_9713D6
.text:009713D3 ; ---------------------------------------------------------------------------
.text:009713D5                 db 0E8h
.text:009713D6 ; ---------------------------------------------------------------------------
.text:009713D6
.text:009713D6 loc_9713D6:                             ; CODE XREF: .text:loc_9713D1↑j
.text:009713D6                                         ; .text:009713D3↑j
.text:009713D6                 mov     eax, large fs:30h
.text:009713DC                 xor     ebx, ebx
.text:009713DE                 mov     bl, [eax+2]
.text:009713E1                 xor     dword_976024, ebx
.text:009713E7                 mov     ecx, [ebp-8]
.text:009713EA                 mov     esi, 17h
.text:009713EF                 movsx   eax, byte ptr [ecx+10h]
.text:009713F3                 cdq
.text:009713F4                 idiv    esi
.text:009713F6                 movsx   eax, byte ptr [ecx+11h]
.text:009713FA                 add     edx, eax
.text:009713FC                 cmp     edx, dword_976048
.text:00971402                 jnz     short loc_971407
.text:00971404                 inc     dword ptr [ebp-4]
.text:00971407
.text:00971407 loc_971407:                             ; CODE XREF: .text:00971402↑j
.text:00971407                 jl      short loc_97140C
.text:00971409                 jge     short loc_97140C
.text:00971409 ; ---------------------------------------------------------------------------
.text:0097140B                 db 0E9h
.text:0097140C ; ---------------------------------------------------------------------------
.text:0097140C
.text:0097140C loc_97140C:                             ; CODE XREF: .text:loc_971407↑j
.text:0097140C                                         ; .text:00971409↑j
.text:0097140C                 mov     eax, large fs:30h
.text:00971412                 xor     ebx, ebx
.text:00971414                 mov     bl, [eax+2]
.text:00971417                 xor     dword_976024+1, ebx
.text:0097141D                 mov     eax, [ebp-8]
.text:00971420                 movsx   ecx, byte ptr [eax+12h]
.text:00971424                 movsx   eax, byte ptr [eax+10h]
.text:00971428                 add     eax, 17h
.text:0097142B                 add     eax, ecx
.text:0097142D                 cmp     eax, dword_97604C
.text:00971433                 jnz     short loc_971438
.text:00971435                 inc     dword ptr [ebp-4]
.text:00971438
.text:00971438 loc_971438:                             ; CODE XREF: .text:00971433↑j
.text:00971438                 jg      short loc_97143D
.text:0097143A                 jle     short loc_97143D
.text:0097143A ; ---------------------------------------------------------------------------
.text:0097143C                 db 0E8h
.text:0097143D ; ---------------------------------------------------------------------------
.text:0097143D
.text:0097143D loc_97143D:                             ; CODE XREF: .text:loc_971438↑j
.text:0097143D                                         ; .text:0097143A↑j
.text:0097143D                 mov     eax, large fs:30h
.text:00971443                 xor     ebx, ebx
.text:00971445                 mov     bl, [eax+2]
.text:00971448                 xor     dword_976024+2, ebx
.text:0097144E                 mov     eax, [ebp-8]
.text:00971451                 movsx   ecx, byte ptr [eax+10h]
.text:00971455                 movsx   eax, byte ptr [eax+13h]
.text:00971459                 imul    ecx, 17h
.text:0097145C                 add     ecx, eax
.text:0097145E                 cmp     ecx, dword_976050
.text:00971464                 jnz     short loc_971469
.text:00971466                 inc     dword ptr [ebp-4]
.text:00971469
.text:00971469 loc_971469:                             ; CODE XREF: .text:00971464↑j
.text:00971469                 jo      short loc_97146E
.text:0097146B                 jno     short loc_97146E
.text:0097146B ; ---------------------------------------------------------------------------
.text:0097146D                 db 0E9h
.text:0097146E ; ---------------------------------------------------------------------------
.text:0097146E
.text:0097146E loc_97146E:                             ; CODE XREF: .text:loc_971469↑j
.text:0097146E                                         ; .text:0097146B↑j
.text:0097146E                 mov     eax, large fs:30h
.text:00971474                 xor     ebx, ebx
.text:00971476                 mov     bl, [eax+2]
.text:00971479                 xor     dword_976024+3, ebx
.text:0097147F                 mov     ecx, [ebp-8]
.text:00971482                 mov     esi, 18h
.text:00971487                 movsx   eax, byte ptr [ecx+14h]
.text:0097148B                 cdq
.text:0097148C                 idiv    esi
.text:0097148E                 movsx   eax, byte ptr [ecx+15h]
.text:00971492                 add     edx, eax
.text:00971494                 cmp     edx, dword_976054
.text:0097149A                 jnz     short loc_97149F
.text:0097149C                 inc     dword ptr [ebp-4]
.text:0097149F
.text:0097149F loc_97149F:                             ; CODE XREF: .text:0097149A↑j
.text:0097149F                 jnb     short loc_9714A4
.text:009714A1                 jb      short loc_9714A4
.text:009714A1 ; ---------------------------------------------------------------------------
.text:009714A3                 db 0E8h
.text:009714A4 ; ---------------------------------------------------------------------------
.text:009714A4
.text:009714A4 loc_9714A4:                             ; CODE XREF: .text:loc_97149F↑j
.text:009714A4                                         ; .text:009714A1↑j
.text:009714A4                 mov     eax, large fs:30h
.text:009714AA                 xor     ebx, ebx
.text:009714AC                 mov     bl, [eax+2]
.text:009714AF                 xor     dword_976028, ebx
.text:009714B5                 mov     eax, [ebp-8]
.text:009714B8                 movsx   ecx, byte ptr [eax+14h]
.text:009714BC                 movsx   eax, byte ptr [eax+16h]
.text:009714C0                 add     eax, 18h
.text:009714C3                 add     eax, ecx
.text:009714C5                 cmp     eax, dword_976058
.text:009714CB                 jnz     short loc_9714D0
.text:009714CD                 inc     dword ptr [ebp-4]
.text:009714D0
.text:009714D0 loc_9714D0:                             ; CODE XREF: .text:009714CB↑j
.text:009714D0                 jz      short loc_9714D5
.text:009714D2                 jnz     short loc_9714D5
.text:009714D2 ; ---------------------------------------------------------------------------
.text:009714D4                 db 0E9h
.text:009714D5 ; ---------------------------------------------------------------------------
.text:009714D5
.text:009714D5 loc_9714D5:                             ; CODE XREF: .text:loc_9714D0↑j
.text:009714D5                                         ; .text:009714D2↑j
.text:009714D5                 mov     eax, large fs:30h
.text:009714DB                 xor     ebx, ebx
.text:009714DD                 mov     bl, [eax+2]
.text:009714E0                 xor     dword_976028+1, ebx
.text:009714E6                 mov     eax, [ebp-8]
.text:009714E9                 movsx   ecx, byte ptr [eax+14h]
.text:009714ED                 movsx   eax, byte ptr [eax+17h]
.text:009714F1                 lea     ecx, [ecx+ecx*2]
.text:009714F4                 lea     eax, [eax+ecx*8]
.text:009714F7                 cmp     eax, dword_97605C
.text:009714FD                 jnz     short loc_971502
.text:009714FF                 inc     dword ptr [ebp-4]
.text:00971502
.text:00971502 loc_971502:                             ; CODE XREF: .text:009714FD↑j
.text:00971502                 ja      short loc_971507
.text:00971504                 jbe     short loc_971507
.text:00971504 ; ---------------------------------------------------------------------------
.text:00971506                 db 0E8h
.text:00971507 ; ---------------------------------------------------------------------------
.text:00971507
.text:00971507 loc_971507:                             ; CODE XREF: .text:loc_971502↑j
.text:00971507                                         ; .text:00971504↑j
.text:00971507                 mov     eax, large fs:30h
.text:0097150D                 xor     ebx, ebx
.text:0097150F                 mov     bl, [eax+2]
.text:00971512                 xor     dword_976028+2, ebx
.text:00971518                 mov     ecx, [ebp-8]
.text:0097151B                 mov     esi, 19h
.text:00971520                 movsx   eax, byte ptr [ecx+18h]
.text:00971524                 cdq
.text:00971525                 idiv    esi
.text:00971527                 movsx   eax, byte ptr [ecx+19h]
.text:0097152B                 add     edx, eax
.text:0097152D                 cmp     edx, dword_976060
.text:00971533                 jnz     short loc_971538
.text:00971535                 inc     dword ptr [ebp-4]
.text:00971538
.text:00971538 loc_971538:                             ; CODE XREF: .text:00971533↑j
.text:00971538                 js      short loc_97153D
.text:0097153A                 jns     short loc_97153D
.text:0097153A ; ---------------------------------------------------------------------------
.text:0097153C                 db 0E9h
.text:0097153D ; ---------------------------------------------------------------------------
.text:0097153D
.text:0097153D loc_97153D:                             ; CODE XREF: .text:loc_971538↑j
.text:0097153D                                         ; .text:0097153A↑j
.text:0097153D                 mov     eax, large fs:30h
.text:00971543                 xor     ebx, ebx
.text:00971545                 mov     bl, [eax+2]
.text:00971548                 xor     dword_976028+3, ebx
.text:0097154E                 mov     eax, [ebp-8]
.text:00971551                 movsx   ecx, byte ptr [eax+18h]
.text:00971555                 movsx   eax, byte ptr [eax+1Ah]
.text:00971559                 add     eax, 19h
.text:0097155C                 add     eax, ecx
.text:0097155E                 cmp     eax, dword_976064
.text:00971564                 jnz     short loc_971569
.text:00971566                 inc     dword ptr [ebp-4]
.text:00971569
.text:00971569 loc_971569:                             ; CODE XREF: .text:00971564↑j
.text:00971569                 jnp     short loc_97156E
.text:0097156B                 jp      short loc_97156E
.text:0097156B ; ---------------------------------------------------------------------------
.text:0097156D                 db 0E8h
.text:0097156E ; ---------------------------------------------------------------------------
.text:0097156E
.text:0097156E loc_97156E:                             ; CODE XREF: .text:loc_971569↑j
.text:0097156E                                         ; .text:0097156B↑j
.text:0097156E                 mov     eax, large fs:30h
.text:00971574                 xor     ebx, ebx
.text:00971576                 mov     bl, [eax+2]
.text:00971579                 xor     dword_97602C, ebx
.text:0097157F                 mov     eax, [ebp-8]
.text:00971582                 movsx   ecx, byte ptr [eax+18h]
.text:00971586                 movsx   eax, byte ptr [eax+1Bh]
.text:0097158A                 imul    ecx, 19h
.text:0097158D                 add     ecx, eax
.text:0097158F                 cmp     ecx, dword_976068
.text:00971595                 jnz     short loc_97159A
.text:00971597                 inc     dword ptr [ebp-4]
.text:0097159A
.text:0097159A loc_97159A:                             ; CODE XREF: .text:00971595↑j
.text:0097159A                 jl      short loc_97159F
.text:0097159C                 jge     short loc_97159F
.text:0097159C ; ---------------------------------------------------------------------------
.text:0097159E                 db 0E9h
.text:0097159F ; ---------------------------------------------------------------------------
.text:0097159F
.text:0097159F loc_97159F:                             ; CODE XREF: .text:loc_97159A↑j
.text:0097159F                                         ; .text:0097159C↑j
.text:0097159F                 mov     eax, large fs:30h
.text:009715A5                 xor     ebx, ebx
.text:009715A7                 mov     bl, [eax+2]
.text:009715AA                 xor     dword_97602C+1, ebx
.text:009715B0                 mov     ecx, [ebp-8]
.text:009715B3                 mov     esi, 1Ah
.text:009715B8                 movsx   eax, byte ptr [ecx+1Ch]
.text:009715BC                 cdq
.text:009715BD                 idiv    esi
.text:009715BF                 movsx   eax, byte ptr [ecx+1Dh]
.text:009715C3                 add     edx, eax
.text:009715C5                 cmp     edx, dword_97606C
.text:009715CB                 jnz     short loc_9715D0
.text:009715CD                 inc     dword ptr [ebp-4]
.text:009715D0
.text:009715D0 loc_9715D0:                             ; CODE XREF: .text:009715CB↑j
.text:009715D0                 jg      short loc_9715D5
.text:009715D2                 jle     short loc_9715D5
.text:009715D2 ; ---------------------------------------------------------------------------
.text:009715D4                 db 0E8h
.text:009715D5 ; ---------------------------------------------------------------------------
.text:009715D5
.text:009715D5 loc_9715D5:                             ; CODE XREF: .text:loc_9715D0↑j
.text:009715D5                                         ; .text:009715D2↑j
.text:009715D5                 mov     eax, large fs:30h
.text:009715DB                 xor     ebx, ebx
.text:009715DD                 mov     bl, [eax+2]
.text:009715E0                 xor     dword_97602C+2, ebx
.text:009715E6                 mov     eax, [ebp-8]
.text:009715E9                 movsx   ecx, byte ptr [eax+1Ch]
.text:009715ED                 movsx   eax, byte ptr [eax+1Eh]
.text:009715F1                 add     eax, 1Ah
.text:009715F4                 add     eax, ecx
.text:009715F6                 cmp     eax, dword_976070
.text:009715FC                 jnz     short loc_971601
.text:009715FE                 inc     dword ptr [ebp-4]
.text:00971601
.text:00971601 loc_971601:                             ; CODE XREF: .text:009715FC↑j
.text:00971601                 jo      short loc_971606
.text:00971603                 jno     short loc_971606
.text:00971603 ; ---------------------------------------------------------------------------
.text:00971605                 db 0E9h
.text:00971606 ; ---------------------------------------------------------------------------
.text:00971606
.text:00971606 loc_971606:                             ; CODE XREF: .text:loc_971601↑j
.text:00971606                                         ; .text:00971603↑j
.text:00971606                 mov     eax, large fs:30h
.text:0097160C                 xor     ebx, ebx
.text:0097160E                 mov     bl, [eax+2]
.text:00971611                 xor     dword_97602C+3, ebx
.text:00971617                 mov     eax, [ebp-8]
.text:0097161A                 movsx   ecx, byte ptr [eax+1Ch]
.text:0097161E                 movsx   eax, byte ptr [eax+1Fh]
.text:00971622                 imul    ecx, 1Ah
.text:00971625                 add     ecx, eax
.text:00971627                 cmp     ecx, dword_976074
.text:0097162D                 jnz     short loc_971632
.text:0097162F                 inc     dword ptr [ebp-4]
.text:00971632
.text:00971632 loc_971632:                             ; CODE XREF: .text:0097162D↑j
.text:00971632                 jnb     short loc_971637
.text:00971634                 jb      short loc_971637
.text:00971634 ; ---------------------------------------------------------------------------
.text:00971636                 db 0E8h
.text:00971637 ; ---------------------------------------------------------------------------
.text:00971637
.text:00971637 loc_971637:                             ; CODE XREF: .text:loc_971632↑j
.text:00971637                                         ; .text:00971634↑j
.text:00971637                 mov     eax, large fs:30h
.text:0097163D                 xor     ebx, ebx
.text:0097163F                 mov     bl, [eax+2]
.text:00971642                 xor     dword_976030, ebx
.text:00971648                 mov     ecx, [ebp-8]
.text:0097164B                 mov     esi, 1Bh
.text:00971650                 movsx   eax, byte ptr [ecx+20h]
.text:00971654                 cdq
.text:00971655                 idiv    esi
.text:00971657                 movsx   eax, byte ptr [ecx+21h]
.text:0097165B                 add     edx, eax
.text:0097165D                 cmp     edx, dword_976078
.text:00971663                 jnz     short loc_971668
.text:00971665                 inc     dword ptr [ebp-4]
.text:00971668
.text:00971668 loc_971668:                             ; CODE XREF: .text:00971663↑j
.text:00971668                 jz      short loc_97166D
.text:0097166A                 jnz     short loc_97166D
.text:0097166A ; ---------------------------------------------------------------------------
.text:0097166C                 db 0E9h
.text:0097166D ; ---------------------------------------------------------------------------
.text:0097166D
.text:0097166D loc_97166D:                             ; CODE XREF: .text:loc_971668↑j
.text:0097166D                                         ; .text:0097166A↑j
.text:0097166D                 mov     eax, large fs:30h
.text:00971673                 xor     ebx, ebx
.text:00971675                 mov     bl, [eax+2]
.text:00971678                 xor     dword_976030+1, ebx
.text:0097167E                 mov     eax, [ebp-8]
.text:00971681                 movsx   ecx, byte ptr [eax+22h]
.text:00971685                 movsx   eax, byte ptr [eax+20h]
.text:00971689                 add     eax, 1Bh
.text:0097168C                 add     eax, ecx
.text:0097168E                 cmp     eax, dword_97607C
.text:00971694                 jnz     short loc_971699
.text:00971696                 inc     dword ptr [ebp-4]
.text:00971699
.text:00971699 loc_971699:                             ; CODE XREF: .text:00971694↑j
.text:00971699                 ja      short loc_97169E
.text:0097169B                 jbe     short loc_97169E
.text:0097169B ; ---------------------------------------------------------------------------
.text:0097169D                 db 0E8h
.text:0097169E ; ---------------------------------------------------------------------------
.text:0097169E
.text:0097169E loc_97169E:                             ; CODE XREF: .text:loc_971699↑j
.text:0097169E                                         ; .text:0097169B↑j
.text:0097169E                 mov     eax, large fs:30h
.text:009716A4                 xor     ebx, ebx
.text:009716A6                 mov     bl, [eax+2]
.text:009716A9                 xor     dword_976030+2, ebx
.text:009716AF                 mov     eax, [ebp-8]
.text:009716B2                 movsx   ecx, byte ptr [eax+20h]
.text:009716B6                 movsx   eax, byte ptr [eax+23h]
.text:009716BA                 imul    ecx, 1Bh
.text:009716BD                 add     ecx, eax
.text:009716BF                 cmp     ecx, dword_976080
.text:009716C5                 jnz     short loc_9716CA
.text:009716C7                 inc     dword ptr [ebp-4]
.text:009716CA
.text:009716CA loc_9716CA:                             ; CODE XREF: .text:009716C5↑j
.text:009716CA                 js      short loc_9716CF
.text:009716CC                 jns     short loc_9716CF
.text:009716CC ; ---------------------------------------------------------------------------
.text:009716CE                 db 0E9h
.text:009716CF ; ---------------------------------------------------------------------------
.text:009716CF
.text:009716CF loc_9716CF:                             ; CODE XREF: .text:loc_9716CA↑j
.text:009716CF                                         ; .text:009716CC↑j
.text:009716CF                 mov     eax, large fs:30h
.text:009716D5                 xor     ebx, ebx
.text:009716D7                 mov     bl, [eax+2]
.text:009716DA                 xor     dword_976030+3, ebx
.text:009716E0                 mov     ecx, [ebp-8]
.text:009716E3                 mov     esi, 1Ch
.text:009716E8                 movsx   eax, byte ptr [ecx+24h]
.text:009716EC                 cdq
.text:009716ED                 idiv    esi
.text:009716EF                 movsx   eax, byte ptr [ecx+25h]
.text:009716F3                 add     edx, eax
.text:009716F5                 cmp     edx, dword_976084
.text:009716FB                 jnz     short loc_971700
.text:009716FD                 inc     dword ptr [ebp-4]
.text:00971700
.text:00971700 loc_971700:                             ; CODE XREF: .text:009716FB↑j
.text:00971700                 jnp     short loc_971705
.text:00971702                 jp      short loc_971705
.text:00971702 ; ---------------------------------------------------------------------------
.text:00971704                 db 0E8h
.text:00971705 ; ---------------------------------------------------------------------------
.text:00971705
.text:00971705 loc_971705:                             ; CODE XREF: .text:loc_971700↑j
.text:00971705                                         ; .text:00971702↑j
.text:00971705                 mov     eax, large fs:30h
.text:0097170B                 xor     ebx, ebx
.text:0097170D                 mov     bl, [eax+2]
.text:00971710                 xor     dword_976034, ebx
.text:00971716                 mov     eax, [ebp-8]
.text:00971719                 movsx   ecx, byte ptr [eax+24h]
.text:0097171D                 movsx   eax, byte ptr [eax+26h]
.text:00971721                 add     eax, 1Ch
.text:00971724                 add     eax, ecx
.text:00971726                 cmp     eax, dword_976088
.text:0097172C                 jnz     short loc_971731
.text:0097172E                 inc     dword ptr [ebp-4]
.text:00971731
.text:00971731 loc_971731:                             ; CODE XREF: .text:0097172C↑j
.text:00971731                 jl      short loc_971736
.text:00971733                 jge     short loc_971736
.text:00971733 ; ---------------------------------------------------------------------------
.text:00971735                 db 0E9h
.text:00971736 ; ---------------------------------------------------------------------------
.text:00971736
.text:00971736 loc_971736:                             ; CODE XREF: .text:loc_971731↑j
.text:00971736                                         ; .text:00971733↑j
.text:00971736                 mov     eax, large fs:30h
.text:0097173C                 xor     ebx, ebx
.text:0097173E                 mov     bl, [eax+2]
.text:00971741                 xor     dword_976034+1, ebx
.text:00971747                 mov     eax, [ebp-8]
.text:0097174A                 movsx   ecx, byte ptr [eax+24h]
.text:0097174E                 movsx   eax, byte ptr [eax+27h]
.text:00971752                 lea     edx, ds:0[ecx*8]
.text:00971759                 sub     edx, ecx
.text:0097175B                 lea     eax, [eax+edx*4]
.text:0097175E                 cmp     eax, dword_97608C
.text:00971764                 jnz     short loc_971769
.text:00971766                 inc     dword ptr [ebp-4]
.text:00971769
.text:00971769 loc_971769:                             ; CODE XREF: .text:00971764↑j
.text:00971769                 jg      short loc_97176E
.text:0097176B                 jle     short loc_97176E
.text:0097176B ; ---------------------------------------------------------------------------
.text:0097176D                 db 0E8h
.text:0097176E ; ---------------------------------------------------------------------------
.text:0097176E
.text:0097176E loc_97176E:                             ; CODE XREF: .text:loc_971769↑j
.text:0097176E                                         ; .text:0097176B↑j
.text:0097176E                 mov     eax, large fs:30h
.text:00971774                 xor     ebx, ebx
.text:00971776                 mov     bl, [eax+2]
.text:00971779                 xor     dword_976034+2, ebx
.text:0097177F                 mov     eax, [ebp-8]
.text:00971782                 movsx   ecx, byte ptr [eax+24h]
.text:00971786                 movsx   eax, byte ptr [eax+28h]
.text:0097178A                 lea     edx, ds:0[ecx*8]
.text:00971791                 sub     edx, ecx
.text:00971793                 lea     eax, [eax+edx*4]
.text:00971796                 cmp     eax, dword_976090
.text:0097179C                 jnz     short loc_9717A1
.text:0097179E                 inc     dword ptr [ebp-4]
.text:009717A1
.text:009717A1 loc_9717A1:                             ; CODE XREF: .text:0097179C↑j
.text:009717A1                 jo      short loc_9717A6
.text:009717A3                 jno     short loc_9717A6
.text:009717A3 ; ---------------------------------------------------------------------------
.text:009717A5                 db 0E9h
.text:009717A6 ; ---------------------------------------------------------------------------
.text:009717A6
.text:009717A6 loc_9717A6:                             ; CODE XREF: .text:loc_9717A1↑j
.text:009717A6                                         ; .text:009717A3↑j
.text:009717A6                 mov     eax, large fs:30h
.text:009717AC                 xor     ebx, ebx
.text:009717AE                 mov     bl, [eax+2]
.text:009717B1                 xor     dword_976034+3, ebx
.text:009717B7                 mov     eax, [ebp-8]
.text:009717BA                 movsx   ecx, byte ptr [eax+28h]
.text:009717BE                 movsx   esi, byte ptr [eax+23h]
.text:009717C2                 add     esi, ecx
.text:009717C4                 movsx   edx, byte ptr [eax+22h]
.text:009717C8                 movsx   ecx, byte ptr [eax+25h]
.text:009717CC                 add     esi, ecx
.text:009717CE                 movsx   ecx, byte ptr [eax+24h]
.text:009717D2                 add     esi, ecx
.text:009717D4                 movsx   ecx, byte ptr [eax+27h]
.text:009717D8                 add     esi, ecx
.text:009717DA                 movsx   ecx, byte ptr [eax+26h]
.text:009717DE                 add     esi, ecx
.text:009717E0                 movsx   ecx, byte ptr [eax+21h]
.text:009717E4                 xor     edx, ecx
.text:009717E6                 movsx   ecx, byte ptr [eax+30h]
.text:009717EA                 imul    esi, edx
.text:009717ED                 add     esi, ecx
.text:009717EF                 movsx   ecx, byte ptr [eax+2Ch]
.text:009717F3                 add     esi, ecx
.text:009717F5                 movsx   ecx, byte ptr [eax+2Dh]
.text:009717F9                 add     esi, ecx
.text:009717FB                 movsx   ecx, byte ptr [eax+29h]
.text:009717FF                 add     esi, ecx
.text:00971801                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971805                 add     esi, ecx
.text:00971807                 movsx   ecx, byte ptr [eax+2Ah]
.text:0097180B                 add     esi, ecx
.text:0097180D                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971811                 movsx   eax, byte ptr [eax+2Bh]
.text:00971815                 add     esi, ecx
.text:00971817                 add     esi, eax
.text:00971819                 cmp     esi, dword_976094
.text:0097181F                 jnz     short loc_971824
.text:00971821                 inc     dword ptr [ebp-4]
.text:00971824
.text:00971824 loc_971824:                             ; CODE XREF: .text:0097181F↑j
.text:00971824                 jnb     short loc_971829
.text:00971826                 jb      short loc_971829
.text:00971826 ; ---------------------------------------------------------------------------
.text:00971828                 db 0E8h
.text:00971829 ; ---------------------------------------------------------------------------
.text:00971829
.text:00971829 loc_971829:                             ; CODE XREF: .text:loc_971824↑j
.text:00971829                                         ; .text:00971826↑j
.text:00971829                 mov     eax, large fs:30h
.text:0097182F                 xor     ebx, ebx
.text:00971831                 mov     bl, [eax+2]
.text:00971834                 xor     dword_976038, ebx
.text:0097183A                 mov     eax, [ebp-8]
.text:0097183D                 movsx   ecx, byte ptr [eax+25h]
.text:00971841                 movsx   esi, byte ptr [eax+28h]
.text:00971845                 add     esi, ecx
.text:00971847                 movsx   edx, byte ptr [eax+22h]
.text:0097184B                 movsx   ecx, byte ptr [eax+24h]
.text:0097184F                 add     esi, ecx
.text:00971851                 movsx   ecx, byte ptr [eax+27h]
.text:00971855                 add     esi, ecx
.text:00971857                 movsx   ecx, byte ptr [eax+26h]
.text:0097185B                 add     esi, ecx
.text:0097185D                 movsx   ecx, byte ptr [eax+23h]
.text:00971861                 xor     edx, ecx
.text:00971863                 movsx   ecx, byte ptr [eax+21h]
.text:00971867                 xor     edx, ecx
.text:00971869                 movsx   ecx, byte ptr [eax+30h]
.text:0097186D                 imul    esi, edx
.text:00971870                 add     esi, ecx
.text:00971872                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971876                 add     esi, ecx
.text:00971878                 movsx   ecx, byte ptr [eax+2Dh]
.text:0097187C                 add     esi, ecx
.text:0097187E                 movsx   ecx, byte ptr [eax+29h]
.text:00971882                 add     esi, ecx
.text:00971884                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971888                 add     esi, ecx
.text:0097188A                 movsx   ecx, byte ptr [eax+2Ah]
.text:0097188E                 add     esi, ecx
.text:00971890                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971894                 movsx   eax, byte ptr [eax+2Bh]
.text:00971898                 add     esi, ecx
.text:0097189A                 add     esi, eax
.text:0097189C                 cmp     esi, dword_976098
.text:009718A2                 jnz     short loc_9718A7
.text:009718A4                 inc     dword ptr [ebp-4]
.text:009718A7
.text:009718A7 loc_9718A7:                             ; CODE XREF: .text:009718A2↑j
.text:009718A7                 jz      short loc_9718AC
.text:009718A9                 jnz     short loc_9718AC
.text:009718A9 ; ---------------------------------------------------------------------------
.text:009718AB                 db 0E9h
.text:009718AC ; ---------------------------------------------------------------------------
.text:009718AC
.text:009718AC loc_9718AC:                             ; CODE XREF: .text:loc_9718A7↑j
.text:009718AC                                         ; .text:009718A9↑j
.text:009718AC                 mov     eax, large fs:30h
.text:009718B2                 xor     ebx, ebx
.text:009718B4                 mov     bl, [eax+2]
.text:009718B7                 xor     dword_976038+1, ebx
.text:009718BD                 mov     eax, [ebp-8]
.text:009718C0                 movsx   ecx, byte ptr [eax+28h]
.text:009718C4                 movsx   esi, byte ptr [eax+25h]
.text:009718C8                 xor     esi, ecx
.text:009718CA                 movsx   edx, byte ptr [eax+23h]
.text:009718CE                 movsx   ecx, byte ptr [eax+24h]
.text:009718D2                 xor     esi, ecx
.text:009718D4                 movsx   ecx, byte ptr [eax+26h]
.text:009718D8                 xor     esi, ecx
.text:009718DA                 movsx   ecx, byte ptr [eax+27h]
.text:009718DE                 xor     esi, ecx
.text:009718E0                 movsx   ecx, byte ptr [eax+22h]
.text:009718E4                 add     edx, ecx
.text:009718E6                 movsx   ecx, byte ptr [eax+21h]
.text:009718EA                 add     edx, ecx
.text:009718EC                 movsx   ecx, byte ptr [eax+30h]
.text:009718F0                 imul    esi, edx
.text:009718F3                 sub     esi, ecx
.text:009718F5                 movsx   ecx, byte ptr [eax+2Ch]
.text:009718F9                 sub     esi, ecx
.text:009718FB                 movsx   ecx, byte ptr [eax+2Dh]
.text:009718FF                 sub     esi, ecx
.text:00971901                 movsx   ecx, byte ptr [eax+29h]
.text:00971905                 sub     esi, ecx
.text:00971907                 movsx   ecx, byte ptr [eax+2Eh]
.text:0097190B                 sub     esi, ecx
.text:0097190D                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971911                 sub     esi, ecx
.text:00971913                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971917                 movsx   eax, byte ptr [eax+2Bh]
.text:0097191B                 sub     esi, ecx
.text:0097191D                 sub     esi, eax
.text:0097191F                 cmp     esi, dword_97609C
.text:00971925                 jnz     short loc_97192A
.text:00971927                 inc     dword ptr [ebp-4]
.text:0097192A
.text:0097192A loc_97192A:                             ; CODE XREF: .text:00971925↑j
.text:0097192A                 ja      short loc_97192F
.text:0097192C                 jbe     short loc_97192F
.text:0097192C ; ---------------------------------------------------------------------------
.text:0097192E                 db 0E8h
.text:0097192F ; ---------------------------------------------------------------------------
.text:0097192F
.text:0097192F loc_97192F:                             ; CODE XREF: .text:loc_97192A↑j
.text:0097192F                                         ; .text:0097192C↑j
.text:0097192F                 mov     eax, large fs:30h
.text:00971935                 xor     ebx, ebx
.text:00971937                 mov     bl, [eax+2]
.text:0097193A                 xor     dword_976038+2, ebx
.text:00971940                 mov     eax, [ebp-8]
.text:00971943                 movsx   ecx, byte ptr [eax+1Fh]
.text:00971947                 movsx   esi, byte ptr [eax+1Ch]
.text:0097194B                 add     esi, ecx
.text:0097194D                 movsx   edx, byte ptr [eax+19h]
.text:00971951                 movsx   ecx, byte ptr [eax+1Eh]
.text:00971955                 add     esi, ecx
.text:00971957                 movsx   ecx, byte ptr [eax+1Bh]
.text:0097195B                 add     esi, ecx
.text:0097195D                 movsx   ecx, byte ptr [eax+20h]
.text:00971961                 add     esi, ecx
.text:00971963                 movsx   ecx, byte ptr [eax+1Dh]
.text:00971967                 add     esi, ecx
.text:00971969                 movsx   ecx, byte ptr [eax+1Ah]
.text:0097196D                 xor     edx, ecx
.text:0097196F                 movsx   ecx, byte ptr [eax+30h]
.text:00971973                 imul    esi, edx
.text:00971976                 add     esi, ecx
.text:00971978                 movsx   ecx, byte ptr [eax+2Ch]
.text:0097197C                 add     esi, ecx
.text:0097197E                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971982                 add     esi, ecx
.text:00971984                 movsx   ecx, byte ptr [eax+29h]
.text:00971988                 add     esi, ecx
.text:0097198A                 movsx   ecx, byte ptr [eax+2Eh]
.text:0097198E                 add     esi, ecx
.text:00971990                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971994                 add     esi, ecx
.text:00971996                 movsx   ecx, byte ptr [eax+2Fh]
.text:0097199A                 movsx   eax, byte ptr [eax+2Bh]
.text:0097199E                 add     esi, ecx
.text:009719A0                 add     esi, eax
.text:009719A2                 cmp     esi, dword_9760A0
.text:009719A8                 jnz     short loc_9719AD
.text:009719AA                 inc     dword ptr [ebp-4]
.text:009719AD
.text:009719AD loc_9719AD:                             ; CODE XREF: .text:009719A8↑j
.text:009719AD                 js      short loc_9719B2
.text:009719AF                 jns     short loc_9719B2
.text:009719AF ; ---------------------------------------------------------------------------
.text:009719B1                 db 0E9h
.text:009719B2 ; ---------------------------------------------------------------------------
.text:009719B2
.text:009719B2 loc_9719B2:                             ; CODE XREF: .text:loc_9719AD↑j
.text:009719B2                                         ; .text:009719AF↑j
.text:009719B2                 mov     eax, large fs:30h
.text:009719B8                 xor     ebx, ebx
.text:009719BA                 mov     bl, [eax+2]
.text:009719BD                 xor     dword_976038+3, ebx
.text:009719C3                 mov     eax, [ebp-8]
.text:009719C6                 movsx   ecx, byte ptr [eax+1Eh]
.text:009719CA                 movsx   esi, byte ptr [eax+1Fh]
.text:009719CE                 movsx   edx, byte ptr [eax+19h]
.text:009719D2                 imul    esi, ecx
.text:009719D5                 movsx   ecx, byte ptr [eax+20h]
.text:009719D9                 imul    esi, ecx
.text:009719DC                 movsx   ecx, byte ptr [eax+1Ah]
.text:009719E0                 xor     edx, ecx
.text:009719E2                 movsx   ecx, byte ptr [eax+1Bh]
.text:009719E6                 xor     edx, ecx
.text:009719E8                 movsx   ecx, byte ptr [eax+30h]
.text:009719EC                 add     esi, edx
.text:009719EE                 sub     esi, ecx
.text:009719F0                 movsx   ecx, byte ptr [eax+2Ch]
.text:009719F4                 sub     esi, ecx
.text:009719F6                 movsx   ecx, byte ptr [eax+2Dh]
.text:009719FA                 sub     esi, ecx
.text:009719FC                 movsx   ecx, byte ptr [eax+29h]
.text:00971A00                 sub     esi, ecx
.text:00971A02                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971A06                 sub     esi, ecx
.text:00971A08                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971A0C                 sub     esi, ecx
.text:00971A0E                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971A12                 sub     esi, ecx
.text:00971A14                 movsx   ecx, byte ptr [eax+2Bh]
.text:00971A18                 sub     esi, ecx
.text:00971A1A                 movsx   ecx, byte ptr [eax+1Ch]
.text:00971A1E                 movsx   eax, byte ptr [eax+1Dh]
.text:00971A22                 add     esi, ecx
.text:00971A24                 add     esi, eax
.text:00971A26                 cmp     esi, dword_9760A4
.text:00971A2C                 jnz     short loc_971A31
.text:00971A2E                 inc     dword ptr [ebp-4]
.text:00971A31
.text:00971A31 loc_971A31:                             ; CODE XREF: .text:00971A2C↑j
.text:00971A31                 jnp     short loc_971A36
.text:00971A33                 jp      short loc_971A36
.text:00971A33 ; ---------------------------------------------------------------------------
.text:00971A35                 db 0E8h
.text:00971A36 ; ---------------------------------------------------------------------------
.text:00971A36
.text:00971A36 loc_971A36:                             ; CODE XREF: .text:loc_971A31↑j
.text:00971A36                                         ; .text:00971A33↑j
.text:00971A36                 mov     eax, large fs:30h
.text:00971A3C                 xor     ebx, ebx
.text:00971A3E                 mov     bl, [eax+2]
.text:00971A41                 xor     dword_97603C, ebx
.text:00971A47                 mov     eax, [ebp-8]
.text:00971A4A                 movsx   ecx, byte ptr [eax+1Eh]
.text:00971A4E                 movsx   edx, byte ptr [eax+1Ch]
.text:00971A52                 imul    edx, ecx
.text:00971A55                 movsx   ecx, byte ptr [eax+1Dh]
.text:00971A59                 imul    edx, ecx
.text:00971A5C                 movsx   ecx, byte ptr [eax+1Fh]
.text:00971A60                 xor     edx, ecx
.text:00971A62                 movsx   ecx, byte ptr [eax+20h]
.text:00971A66                 xor     edx, ecx
.text:00971A68                 movsx   ecx, byte ptr [eax+30h]
.text:00971A6C                 sub     edx, ecx
.text:00971A6E                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971A72                 sub     edx, ecx
.text:00971A74                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971A78                 sub     edx, ecx
.text:00971A7A                 movsx   ecx, byte ptr [eax+29h]
.text:00971A7E                 sub     edx, ecx
.text:00971A80                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971A84                 sub     edx, ecx
.text:00971A86                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971A8A                 sub     edx, ecx
.text:00971A8C                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971A90                 sub     edx, ecx
.text:00971A92                 movsx   ecx, byte ptr [eax+2Bh]
.text:00971A96                 sub     edx, ecx
.text:00971A98                 movsx   ecx, byte ptr [eax+19h]
.text:00971A9C                 add     edx, ecx
.text:00971A9E                 movsx   ecx, byte ptr [eax+1Bh]
.text:00971AA2                 movsx   eax, byte ptr [eax+1Ah]
.text:00971AA6                 add     edx, ecx
.text:00971AA8                 add     edx, eax
.text:00971AAA                 cmp     edx, dword_9760A8
.text:00971AB0                 jnz     short loc_971AB5
.text:00971AB2                 inc     dword ptr [ebp-4]
.text:00971AB5
.text:00971AB5 loc_971AB5:                             ; CODE XREF: .text:00971AB0↑j
.text:00971AB5                 jl      short loc_971ABA
.text:00971AB7                 jge     short loc_971ABA
.text:00971AB7 ; ---------------------------------------------------------------------------
.text:00971AB9                 db 0E9h
.text:00971ABA ; ---------------------------------------------------------------------------
.text:00971ABA
.text:00971ABA loc_971ABA:                             ; CODE XREF: .text:loc_971AB5↑j
.text:00971ABA                                         ; .text:00971AB7↑j
.text:00971ABA                 mov     eax, large fs:30h
.text:00971AC0                 xor     ebx, ebx
.text:00971AC2                 mov     bl, [eax+2]
.text:00971AC5                 xor     dword_97603C+1, ebx
.text:00971ACB                 mov     eax, [ebp-8]
.text:00971ACE                 movsx   ecx, byte ptr [eax+18h]
.text:00971AD2                 movsx   esi, byte ptr [eax+15h]
.text:00971AD6                 xor     esi, ecx
.text:00971AD8                 movsx   edx, byte ptr [eax+13h]
.text:00971ADC                 movsx   ecx, byte ptr [eax+14h]
.text:00971AE0                 xor     esi, ecx
.text:00971AE2                 movsx   ecx, byte ptr [eax+16h]
.text:00971AE6                 xor     esi, ecx
.text:00971AE8                 movsx   ecx, byte ptr [eax+17h]
.text:00971AEC                 xor     esi, ecx
.text:00971AEE                 movsx   ecx, byte ptr [eax+12h]
.text:00971AF2                 imul    edx, ecx
.text:00971AF5                 movsx   ecx, byte ptr [eax+11h]
.text:00971AF9                 imul    edx, ecx
.text:00971AFC                 movsx   ecx, byte ptr [eax+30h]
.text:00971B00                 add     esi, edx
.text:00971B02                 sub     esi, ecx
.text:00971B04                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971B08                 sub     esi, ecx
.text:00971B0A                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971B0E                 sub     esi, ecx
.text:00971B10                 movsx   ecx, byte ptr [eax+29h]
.text:00971B14                 sub     esi, ecx
.text:00971B16                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971B1A                 sub     esi, ecx
.text:00971B1C                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971B20                 sub     esi, ecx
.text:00971B22                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971B26                 movsx   eax, byte ptr [eax+2Bh]
.text:00971B2A                 sub     esi, ecx
.text:00971B2C                 sub     esi, eax
.text:00971B2E                 cmp     esi, dword_9760AC
.text:00971B34                 jnz     short loc_971B39
.text:00971B36                 inc     dword ptr [ebp-4]
.text:00971B39
.text:00971B39 loc_971B39:                             ; CODE XREF: .text:00971B34↑j
.text:00971B39                 jg      short loc_971B3E
.text:00971B3B                 jle     short loc_971B3E
.text:00971B3B ; ---------------------------------------------------------------------------
.text:00971B3D                 db 0E8h
.text:00971B3E ; ---------------------------------------------------------------------------
.text:00971B3E
.text:00971B3E loc_971B3E:                             ; CODE XREF: .text:loc_971B39↑j
.text:00971B3E                                         ; .text:00971B3B↑j
.text:00971B3E                 mov     eax, large fs:30h
.text:00971B44                 xor     ebx, ebx
.text:00971B46                 mov     bl, [eax+2]
.text:00971B49                 xor     dword_97603C+2, ebx
.text:00971B4F                 mov     eax, [ebp-8]
.text:00971B52                 movsx   ecx, byte ptr [eax+12h]
.text:00971B56                 movsx   edx, byte ptr [eax+13h]
.text:00971B5A                 imul    edx, ecx
.text:00971B5D                 movsx   ecx, byte ptr [eax+11h]
.text:00971B61                 imul    edx, ecx
.text:00971B64                 movsx   ecx, byte ptr [eax+18h]
.text:00971B68                 sub     edx, ecx
.text:00971B6A                 movsx   ecx, byte ptr [eax+15h]
.text:00971B6E                 sub     edx, ecx
.text:00971B70                 movsx   ecx, byte ptr [eax+17h]
.text:00971B74                 sub     edx, ecx
.text:00971B76                 movsx   ecx, byte ptr [eax+16h]
.text:00971B7A                 sub     edx, ecx
.text:00971B7C                 movsx   ecx, byte ptr [eax+30h]
.text:00971B80                 add     edx, ecx
.text:00971B82                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971B86                 add     edx, ecx
.text:00971B88                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971B8C                 add     edx, ecx
.text:00971B8E                 movsx   ecx, byte ptr [eax+29h]
.text:00971B92                 add     edx, ecx
.text:00971B94                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971B98                 add     edx, ecx
.text:00971B9A                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971B9E                 add     edx, ecx
.text:00971BA0                 movsx   ecx, byte ptr [eax+14h]
.text:00971BA4                 add     edx, ecx
.text:00971BA6                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971BAA                 movsx   eax, byte ptr [eax+2Bh]
.text:00971BAE                 add     edx, ecx
.text:00971BB0                 add     edx, eax
.text:00971BB2                 cmp     edx, dword_9760B0
.text:00971BB8                 jnz     short loc_971BBD
.text:00971BBA                 inc     dword ptr [ebp-4]
.text:00971BBD
.text:00971BBD loc_971BBD:                             ; CODE XREF: .text:00971BB8↑j
.text:00971BBD                 jo      short loc_971BC2
.text:00971BBF                 jno     short loc_971BC2
.text:00971BBF ; ---------------------------------------------------------------------------
.text:00971BC1                 db 0E9h
.text:00971BC2 ; ---------------------------------------------------------------------------
.text:00971BC2
.text:00971BC2 loc_971BC2:                             ; CODE XREF: .text:loc_971BBD↑j
.text:00971BC2                                         ; .text:00971BBF↑j
.text:00971BC2                 mov     eax, large fs:30h
.text:00971BC8                 xor     ebx, ebx
.text:00971BCA                 mov     bl, [eax+2]
.text:00971BCD                 xor     dword_97603C+3, ebx
.text:00971BD3                 mov     eax, [ebp-8]
.text:00971BD6                 movsx   ecx, byte ptr [eax+13h]
.text:00971BDA                 movsx   edx, byte ptr [eax+12h]
.text:00971BDE                 xor     edx, ecx
.text:00971BE0                 movsx   ecx, byte ptr [eax+11h]
.text:00971BE4                 xor     edx, ecx
.text:00971BE6                 movsx   ecx, byte ptr [eax+30h]
.text:00971BEA                 add     edx, ecx
.text:00971BEC                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971BF0                 add     edx, ecx
.text:00971BF2                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971BF6                 add     edx, ecx
.text:00971BF8                 movsx   ecx, byte ptr [eax+29h]
.text:00971BFC                 add     edx, ecx
.text:00971BFE                 movsx   ecx, byte ptr [eax+18h]
.text:00971C02                 add     edx, ecx
.text:00971C04                 movsx   ecx, byte ptr [eax+15h]
.text:00971C08                 add     edx, ecx
.text:00971C0A                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971C0E                 add     edx, ecx
.text:00971C10                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971C14                 add     edx, ecx
.text:00971C16                 movsx   ecx, byte ptr [eax+14h]
.text:00971C1A                 add     edx, ecx
.text:00971C1C                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971C20                 add     edx, ecx
.text:00971C22                 movsx   ecx, byte ptr [eax+2Bh]
.text:00971C26                 add     edx, ecx
.text:00971C28                 movsx   ecx, byte ptr [eax+17h]
.text:00971C2C                 movsx   eax, byte ptr [eax+16h]
.text:00971C30                 add     edx, ecx
.text:00971C32                 add     edx, eax
.text:00971C34                 cmp     edx, dword_9760B4
.text:00971C3A                 jnz     short loc_971C3F
.text:00971C3C                 inc     dword ptr [ebp-4]
.text:00971C3F
.text:00971C3F loc_971C3F:                             ; CODE XREF: .text:00971C3A↑j
.text:00971C3F                 jnb     short loc_971C44
.text:00971C41                 jb      short loc_971C44
.text:00971C41 ; ---------------------------------------------------------------------------
.text:00971C43                 db 0E8h
.text:00971C44 ; ---------------------------------------------------------------------------
.text:00971C44
.text:00971C44 loc_971C44:                             ; CODE XREF: .text:loc_971C3F↑j
.text:00971C44                                         ; .text:00971C41↑j
.text:00971C44                 mov     eax, large fs:30h
.text:00971C4A                 xor     ebx, ebx
.text:00971C4C                 mov     bl, [eax+2]
.text:00971C4F                 xor     dword_976040, ebx
.text:00971C55                 mov     eax, [ebp-8]
.text:00971C58                 movsx   ecx, byte ptr [eax+0Eh]
.text:00971C5C                 movsx   esi, byte ptr [eax+0Ch]
.text:00971C60                 xor     esi, ecx
.text:00971C62                 movsx   edx, byte ptr [eax+9]
.text:00971C66                 movsx   ecx, byte ptr [eax+0Fh]
.text:00971C6A                 xor     esi, ecx
.text:00971C6C                 movsx   ecx, byte ptr [eax+0Dh]
.text:00971C70                 xor     esi, ecx
.text:00971C72                 movsx   ecx, byte ptr [eax+10h]
.text:00971C76                 xor     esi, ecx
.text:00971C78                 movsx   ecx, byte ptr [eax+0Bh]
.text:00971C7C                 imul    edx, ecx
.text:00971C7F                 movsx   ecx, byte ptr [eax+0Ah]
.text:00971C83                 imul    edx, ecx
.text:00971C86                 movsx   ecx, byte ptr [eax+30h]
.text:00971C8A                 add     esi, edx
.text:00971C8C                 sub     esi, ecx
.text:00971C8E                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971C92                 sub     esi, ecx
.text:00971C94                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971C98                 sub     esi, ecx
.text:00971C9A                 movsx   ecx, byte ptr [eax+29h]
.text:00971C9E                 sub     esi, ecx
.text:00971CA0                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971CA4                 sub     esi, ecx
.text:00971CA6                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971CAA                 sub     esi, ecx
.text:00971CAC                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971CB0                 movsx   eax, byte ptr [eax+2Bh]
.text:00971CB4                 sub     esi, ecx
.text:00971CB6                 sub     esi, eax
.text:00971CB8                 cmp     esi, dword_9760B8
.text:00971CBE                 jnz     short loc_971CC3
.text:00971CC0                 inc     dword ptr [ebp-4]
.text:00971CC3
.text:00971CC3 loc_971CC3:                             ; CODE XREF: .text:00971CBE↑j
.text:00971CC3                 jz      short loc_971CC8
.text:00971CC5                 jnz     short loc_971CC8
.text:00971CC5 ; ---------------------------------------------------------------------------
.text:00971CC7                 db 0E9h
.text:00971CC8 ; ---------------------------------------------------------------------------
.text:00971CC8
.text:00971CC8 loc_971CC8:                             ; CODE XREF: .text:loc_971CC3↑j
.text:00971CC8                                         ; .text:00971CC5↑j
.text:00971CC8                 mov     eax, large fs:30h
.text:00971CCE                 xor     ebx, ebx
.text:00971CD0                 mov     bl, [eax+2]
.text:00971CD3                 xor     dword_976040+1, ebx
.text:00971CD9                 mov     eax, [ebp-8]
.text:00971CDC                 movsx   ecx, byte ptr [eax+0Bh]
.text:00971CE0                 movsx   esi, byte ptr [eax+0Ch]
.text:00971CE4                 add     esi, ecx
.text:00971CE6                 movsx   edx, byte ptr [eax+9]
.text:00971CEA                 movsx   ecx, byte ptr [eax+0Eh]
.text:00971CEE                 xor     esi, ecx
.text:00971CF0                 movsx   ecx, byte ptr [eax+0Fh]
.text:00971CF4                 xor     esi, ecx
.text:00971CF6                 movsx   ecx, byte ptr [eax+0Dh]
.text:00971CFA                 xor     esi, ecx
.text:00971CFC                 movsx   ecx, byte ptr [eax+10h]
.text:00971D00                 xor     esi, ecx
.text:00971D02                 movsx   ecx, byte ptr [eax+0Ah]
.text:00971D06                 add     edx, ecx
.text:00971D08                 movsx   ecx, byte ptr [eax+30h]
.text:00971D0C                 imul    esi, edx
.text:00971D0F                 sub     esi, ecx
.text:00971D11                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971D15                 sub     esi, ecx
.text:00971D17                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971D1B                 sub     esi, ecx
.text:00971D1D                 movsx   ecx, byte ptr [eax+29h]
.text:00971D21                 sub     esi, ecx
.text:00971D23                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971D27                 sub     esi, ecx
.text:00971D29                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971D2D                 sub     esi, ecx
.text:00971D2F                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971D33                 movsx   eax, byte ptr [eax+2Bh]
.text:00971D37                 sub     esi, ecx
.text:00971D39                 sub     esi, eax
.text:00971D3B                 cmp     esi, dword_9760BC
.text:00971D41                 jnz     short loc_971D46
.text:00971D43                 inc     dword ptr [ebp-4]
.text:00971D46
.text:00971D46 loc_971D46:                             ; CODE XREF: .text:00971D41↑j
.text:00971D46                 ja      short loc_971D4B
.text:00971D48                 jbe     short loc_971D4B
.text:00971D48 ; ---------------------------------------------------------------------------
.text:00971D4A                 db 0E8h
.text:00971D4B ; ---------------------------------------------------------------------------
.text:00971D4B
.text:00971D4B loc_971D4B:                             ; CODE XREF: .text:loc_971D46↑j
.text:00971D4B                                         ; .text:00971D48↑j
.text:00971D4B                 mov     eax, large fs:30h
.text:00971D51                 xor     ebx, ebx
.text:00971D53                 mov     bl, [eax+2]
.text:00971D56                 xor     dword_976040+2, ebx
.text:00971D5C                 mov     eax, [ebp-8]
.text:00971D5F                 movsx   ecx, byte ptr [eax+0Eh]
.text:00971D63                 movsx   edx, byte ptr [eax+30h]
.text:00971D67                 sub     edx, ecx
.text:00971D69                 movsx   ecx, byte ptr [eax+0Bh]
.text:00971D6D                 sub     edx, ecx
.text:00971D6F                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971D73                 add     edx, ecx
.text:00971D75                 movsx   ecx, byte ptr [eax+0Ch]
.text:00971D79                 add     edx, ecx
.text:00971D7B                 movsx   ecx, byte ptr [eax+9]
.text:00971D7F                 add     edx, ecx
.text:00971D81                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971D85                 add     edx, ecx
.text:00971D87                 movsx   ecx, byte ptr [eax+29h]
.text:00971D8B                 add     edx, ecx
.text:00971D8D                 movsx   ecx, byte ptr [eax+0Fh]
.text:00971D91                 add     edx, ecx
.text:00971D93                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971D97                 add     edx, ecx
.text:00971D99                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971D9D                 add     edx, ecx
.text:00971D9F                 movsx   ecx, byte ptr [eax+0Ah]
.text:00971DA3                 add     edx, ecx
.text:00971DA5                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971DA9                 add     edx, ecx
.text:00971DAB                 movsx   ecx, byte ptr [eax+2Bh]
.text:00971DAF                 add     edx, ecx
.text:00971DB1                 movsx   ecx, byte ptr [eax+10h]
.text:00971DB5                 movsx   eax, byte ptr [eax+0Dh]
.text:00971DB9                 add     edx, ecx
.text:00971DBB                 add     edx, eax
.text:00971DBD                 cmp     edx, dword_9760C0
.text:00971DC3                 jnz     short loc_971DC8
.text:00971DC5                 inc     dword ptr [ebp-4]
.text:00971DC8
.text:00971DC8 loc_971DC8:                             ; CODE XREF: .text:00971DC3↑j
.text:00971DC8                 js      short loc_971DCD
.text:00971DCA                 jns     short loc_971DCD
.text:00971DCA ; ---------------------------------------------------------------------------
.text:00971DCC                 db 0E9h
.text:00971DCD ; ---------------------------------------------------------------------------
.text:00971DCD
.text:00971DCD loc_971DCD:                             ; CODE XREF: .text:loc_971DC8↑j
.text:00971DCD                                         ; .text:00971DCA↑j
.text:00971DCD                 mov     eax, large fs:30h
.text:00971DD3                 xor     ebx, ebx
.text:00971DD5                 mov     bl, [eax+2]
.text:00971DD8                 xor     dword_976040+3, ebx
.text:00971DDE                 mov     eax, [ebp-8]
.text:00971DE1                 movsx   ecx, byte ptr [eax+5]
.text:00971DE5                 movsx   edx, byte ptr [eax+3]
.text:00971DE9                 movsx   esi, byte ptr [eax+30h]
.text:00971DED                 xor     edx, ecx
.text:00971DEF                 movsx   ecx, byte ptr [eax+8]
.text:00971DF3                 xor     edx, ecx
.text:00971DF5                 movsx   ecx, byte ptr [eax+4]
.text:00971DF9                 xor     edx, ecx
.text:00971DFB                 movsx   ecx, byte ptr [eax+6]
.text:00971DFF                 xor     edx, ecx
.text:00971E01                 movsx   ecx, byte ptr [eax+7]
.text:00971E05                 xor     edx, ecx
.text:00971E07                 movsx   ecx, byte ptr [eax+1]
.text:00971E0B                 sub     esi, edx
.text:00971E0D                 sub     esi, ecx
.text:00971E0F                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971E13                 add     esi, ecx
.text:00971E15                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971E19                 add     esi, ecx
.text:00971E1B                 movsx   ecx, byte ptr [eax+29h]
.text:00971E1F                 add     esi, ecx
.text:00971E21                 movsx   ecx, byte ptr [eax+2]
.text:00971E25                 add     esi, ecx
.text:00971E27                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971E2B                 add     esi, ecx
.text:00971E2D                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971E31                 add     esi, ecx
.text:00971E33                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971E37                 movsx   eax, byte ptr [eax+2Bh]
.text:00971E3B                 add     esi, ecx
.text:00971E3D                 add     esi, eax
.text:00971E3F                 cmp     esi, dword_9760C4
.text:00971E45                 jnz     short loc_971E4A
.text:00971E47                 inc     dword ptr [ebp-4]
.text:00971E4A
.text:00971E4A loc_971E4A:                             ; CODE XREF: .text:00971E45↑j
.text:00971E4A                 jnp     short loc_971E4F
.text:00971E4C                 jp      short loc_971E4F
.text:00971E4C ; ---------------------------------------------------------------------------
.text:00971E4E                 db 0E8h
.text:00971E4F ; ---------------------------------------------------------------------------
.text:00971E4F
.text:00971E4F loc_971E4F:                             ; CODE XREF: .text:loc_971E4A↑j
.text:00971E4F                                         ; .text:00971E4C↑j
.text:00971E4F                 mov     eax, large fs:30h
.text:00971E55                 xor     ebx, ebx
.text:00971E57                 mov     bl, [eax+2]
.text:00971E5A                 xor     dword_976044, ebx
.text:00971E60                 mov     eax, [ebp-8]
.text:00971E63                 movsx   ecx, byte ptr [eax+30h]
.text:00971E67                 movsx   edx, byte ptr [eax+2Ch]
.text:00971E6B                 xor     edx, ecx
.text:00971E6D                 movsx   ecx, byte ptr [eax+29h]
.text:00971E71                 xor     edx, ecx
.text:00971E73                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971E77                 xor     edx, ecx
.text:00971E79                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971E7D                 xor     edx, ecx
.text:00971E7F                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971E83                 xor     edx, ecx
.text:00971E85                 movsx   ecx, byte ptr [eax+2Bh]
.text:00971E89                 xor     edx, ecx
.text:00971E8B                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971E8F                 xor     edx, ecx
.text:00971E91                 movsx   ecx, byte ptr [eax+2]
.text:00971E95                 sub     edx, ecx
.text:00971E97                 movsx   ecx, byte ptr [eax+3]
.text:00971E9B                 add     edx, ecx
.text:00971E9D                 movsx   ecx, byte ptr [eax+8]
.text:00971EA1                 add     edx, ecx
.text:00971EA3                 movsx   ecx, byte ptr [eax+5]
.text:00971EA7                 add     edx, ecx
.text:00971EA9                 movsx   ecx, byte ptr [eax+4]
.text:00971EAD                 add     edx, ecx
.text:00971EAF                 movsx   ecx, byte ptr [eax+7]
.text:00971EB3                 add     edx, ecx
.text:00971EB5                 movsx   ecx, byte ptr [eax+6]
.text:00971EB9                 movsx   eax, byte ptr [eax+1]
.text:00971EBD                 add     edx, ecx
.text:00971EBF                 add     edx, eax
.text:00971EC1                 cmp     edx, dword_9760C8
.text:00971EC7                 jnz     short loc_971ECC
.text:00971EC9                 inc     dword ptr [ebp-4]
.text:00971ECC
.text:00971ECC loc_971ECC:                             ; CODE XREF: .text:00971EC7↑j
.text:00971ECC                 jl      short loc_971ED1
.text:00971ECE                 jge     short loc_971ED1
.text:00971ECE ; ---------------------------------------------------------------------------
.text:00971ED0                 db 0E9h
.text:00971ED1 ; ---------------------------------------------------------------------------
.text:00971ED1
.text:00971ED1 loc_971ED1:                             ; CODE XREF: .text:loc_971ECC↑j
.text:00971ED1                                         ; .text:00971ECE↑j
.text:00971ED1                 mov     eax, large fs:30h
.text:00971ED7                 xor     ebx, ebx
.text:00971ED9                 mov     bl, [eax+2]
.text:00971EDC                 xor     dword_976044+1, ebx
.text:00971EE2                 mov     eax, [ebp-8]
.text:00971EE5                 movsx   ecx, byte ptr [eax+5]
.text:00971EE9                 movsx   edx, byte ptr [eax+3]
.text:00971EED                 movsx   esi, byte ptr [eax+30h]
.text:00971EF1                 xor     edx, ecx
.text:00971EF3                 movsx   ecx, byte ptr [eax+8]
.text:00971EF7                 xor     edx, ecx
.text:00971EF9                 movsx   ecx, byte ptr [eax+4]
.text:00971EFD                 xor     edx, ecx
.text:00971EFF                 movsx   ecx, byte ptr [eax+6]
.text:00971F03                 xor     edx, ecx
.text:00971F05                 movsx   ecx, byte ptr [eax+7]
.text:00971F09                 xor     edx, ecx
.text:00971F0B                 movsx   ecx, byte ptr [eax+2]
.text:00971F0F                 sub     esi, edx
.text:00971F11                 sub     esi, ecx
.text:00971F13                 movsx   ecx, byte ptr [eax+2Ch]
.text:00971F17                 add     esi, ecx
.text:00971F19                 movsx   ecx, byte ptr [eax+2Dh]
.text:00971F1D                 add     esi, ecx
.text:00971F1F                 movsx   ecx, byte ptr [eax+29h]
.text:00971F23                 add     esi, ecx
.text:00971F25                 movsx   ecx, byte ptr [eax+2Eh]
.text:00971F29                 add     esi, ecx
.text:00971F2B                 movsx   ecx, byte ptr [eax+2Ah]
.text:00971F2F                 add     esi, ecx
.text:00971F31                 movsx   ecx, byte ptr [eax+2Fh]
.text:00971F35                 add     esi, ecx
.text:00971F37                 movsx   ecx, byte ptr [eax+2Bh]
.text:00971F3B                 movsx   eax, byte ptr [eax+1]
.text:00971F3F                 add     esi, ecx
.text:00971F41                 add     esi, eax
.text:00971F43                 cmp     esi, dword_9760CC
.text:00971F49                 jnz     short loc_971F4E
.text:00971F4B                 inc     dword ptr [ebp-4]
.text:00971F4E
.text:00971F4E loc_971F4E:                             ; CODE XREF: .text:00971F49↑j
.text:00971F4E                 jg      short loc_971F53
.text:00971F50                 jle     short loc_971F53
.text:00971F50 ; ---------------------------------------------------------------------------
.text:00971F52                 db 0E8h
.text:00971F53 ; ---------------------------------------------------------------------------
.text:00971F53
.text:00971F53 loc_971F53:                             ; CODE XREF: .text:loc_971F4E↑j
.text:00971F53                                         ; .text:00971F50↑j
.text:00971F53                 mov     eax, large fs:30h
.text:00971F59                 xor     ebx, ebx
.text:00971F5B                 mov     bl, [eax+2]
.text:00971F5E                 xor     dword_976044+2, ebx
.text:00971F64                 mov     eax, [ebp-8]
.text:00971F67                 movsx   eax, byte ptr [eax+29h]
.text:00971F6B                 cmp     eax, dword_9760D0
.text:00971F71                 jnz     short loc_971F76
.text:00971F73                 inc     dword ptr [ebp-4]
.text:00971F76
.text:00971F76 loc_971F76:                             ; CODE XREF: .text:00971F71↑j
.text:00971F76                 jo      short loc_971F7B
.text:00971F78                 jno     short loc_971F7B
.text:00971F78 ; ---------------------------------------------------------------------------
.text:00971F7A                 db 0E9h
.text:00971F7B ; ---------------------------------------------------------------------------
.text:00971F7B
.text:00971F7B loc_971F7B:                             ; CODE XREF: .text:loc_971F76↑j
.text:00971F7B                                         ; .text:00971F78↑j
.text:00971F7B                 mov     eax, large fs:30h
.text:00971F81                 xor     ebx, ebx
.text:00971F83                 mov     bl, [eax+2]
.text:00971F86                 xor     dword_976044+3, ebx
.text:00971F8C                 mov     eax, [ebp-8]
.text:00971F8F                 movsx   eax, byte ptr [eax+2Ah]
.text:00971F93                 cmp     eax, dword_9760D4
.text:00971F99                 jnz     short loc_971F9E
.text:00971F9B                 inc     dword ptr [ebp-4]
.text:00971F9E
.text:00971F9E loc_971F9E:                             ; CODE XREF: .text:00971F99↑j
.text:00971F9E                 jnb     short loc_971FA3
.text:00971FA0                 jb      short loc_971FA3
.text:00971FA0 ; ---------------------------------------------------------------------------
.text:00971FA2                 db 0E8h
.text:00971FA3 ; ---------------------------------------------------------------------------
.text:00971FA3
.text:00971FA3 loc_971FA3:                             ; CODE XREF: .text:loc_971F9E↑j
.text:00971FA3                                         ; .text:00971FA0↑j
.text:00971FA3                 mov     eax, large fs:30h
.text:00971FA9                 xor     ebx, ebx
.text:00971FAB                 mov     bl, [eax+2]
.text:00971FAE                 xor     dword_976048, ebx
.text:00971FB4                 mov     eax, [ebp-8]
.text:00971FB7                 movsx   eax, byte ptr [eax+2Bh]
.text:00971FBB                 cmp     eax, dword_9760D8
.text:00971FC1                 jnz     short loc_971FC6
.text:00971FC3                 inc     dword ptr [ebp-4]
.text:00971FC6
.text:00971FC6 loc_971FC6:                             ; CODE XREF: .text:00971FC1↑j
.text:00971FC6                 jz      short loc_971FCB
.text:00971FC8                 jnz     short loc_971FCB
.text:00971FC8 ; ---------------------------------------------------------------------------
.text:00971FCA                 db 0E9h
.text:00971FCB ; ---------------------------------------------------------------------------
.text:00971FCB
.text:00971FCB loc_971FCB:                             ; CODE XREF: .text:loc_971FC6↑j
.text:00971FCB                                         ; .text:00971FC8↑j
.text:00971FCB                 mov     eax, large fs:30h
.text:00971FD1                 xor     ebx, ebx
.text:00971FD3                 mov     bl, [eax+2]
.text:00971FD6                 xor     dword_976048+1, ebx
.text:00971FDC                 mov     eax, [ebp-8]
.text:00971FDF                 movsx   eax, byte ptr [eax+2Ch]
.text:00971FE3                 cmp     eax, dword_9760DC
.text:00971FE9                 jnz     short loc_971FEE
.text:00971FEB                 inc     dword ptr [ebp-4]
.text:00971FEE
.text:00971FEE loc_971FEE:                             ; CODE XREF: .text:00971FE9↑j
.text:00971FEE                 ja      short loc_971FF3
.text:00971FF0                 jbe     short loc_971FF3
.text:00971FF0 ; ---------------------------------------------------------------------------
.text:00971FF2                 db 0E8h
.text:00971FF3 ; ---------------------------------------------------------------------------
.text:00971FF3
.text:00971FF3 loc_971FF3:                             ; CODE XREF: .text:loc_971FEE↑j
.text:00971FF3                                         ; .text:00971FF0↑j
.text:00971FF3                 mov     eax, large fs:30h
.text:00971FF9                 xor     ebx, ebx
.text:00971FFB                 mov     bl, [eax+2]
.text:00971FFE                 xor     dword_976048+2, ebx
.text:00972004                 mov     eax, [ebp-8]
.text:00972007                 movsx   eax, byte ptr [eax+2Dh]
.text:0097200B                 cmp     eax, dword_9760E0
.text:00972011                 jnz     short loc_972016
.text:00972013                 inc     dword ptr [ebp-4]
.text:00972016
.text:00972016 loc_972016:                             ; CODE XREF: .text:00972011↑j
.text:00972016                 js      short loc_97201B
.text:00972018                 jns     short loc_97201B
.text:00972018 ; ---------------------------------------------------------------------------
.text:0097201A                 db 0E9h
.text:0097201B ; ---------------------------------------------------------------------------
.text:0097201B
.text:0097201B loc_97201B:                             ; CODE XREF: .text:loc_972016↑j
.text:0097201B                                         ; .text:00972018↑j
.text:0097201B                 mov     eax, large fs:30h
.text:00972021                 xor     ebx, ebx
.text:00972023                 mov     bl, [eax+2]
.text:00972026                 xor     dword_976048+3, ebx
.text:0097202C                 mov     eax, [ebp-8]
.text:0097202F                 movsx   eax, byte ptr [eax+2Eh]
.text:00972033                 cmp     eax, dword_9760E4
.text:00972039                 jnz     short loc_97203E
.text:0097203B                 inc     dword ptr [ebp-4]
.text:0097203E
.text:0097203E loc_97203E:                             ; CODE XREF: .text:00972039↑j
.text:0097203E                 jnp     short loc_972043
.text:00972040                 jp      short loc_972043
.text:00972040 ; ---------------------------------------------------------------------------
.text:00972042                 db 0E8h
.text:00972043 ; ---------------------------------------------------------------------------
.text:00972043
.text:00972043 loc_972043:                             ; CODE XREF: .text:loc_97203E↑j
.text:00972043                                         ; .text:00972040↑j
.text:00972043                 mov     eax, large fs:30h
.text:00972049                 xor     ebx, ebx
.text:0097204B                 mov     bl, [eax+2]
.text:0097204E                 xor     dword_97604C, ebx
.text:00972054                 mov     eax, [ebp-8]
.text:00972057                 movsx   eax, byte ptr [eax+2Fh]
.text:0097205B                 cmp     eax, dword_9760E8
.text:00972061                 jnz     short loc_972066
.text:00972063                 inc     dword ptr [ebp-4]
.text:00972066
.text:00972066 loc_972066:                             ; CODE XREF: .text:00972061↑j
.text:00972066                 jl      short loc_97206B
.text:00972068                 jge     short loc_97206B
.text:00972068 ; ---------------------------------------------------------------------------
.text:0097206A                 db 0E9h
.text:0097206B ; ---------------------------------------------------------------------------
.text:0097206B
.text:0097206B loc_97206B:                             ; CODE XREF: .text:loc_972066↑j
.text:0097206B                                         ; .text:00972068↑j
.text:0097206B                 mov     eax, large fs:30h
.text:00972071                 xor     ebx, ebx
.text:00972073                 mov     bl, [eax+2]
.text:00972076                 xor     dword_97604C+1, ebx
.text:0097207C                 mov     eax, [ebp-8]
.text:0097207F                 movsx   eax, byte ptr [eax+30h]
.text:00972083                 cmp     eax, dword_9760EC
.text:00972089                 jnz     short loc_97208E
.text:0097208B                 inc     dword ptr [ebp-4]
.text:0097208E
.text:0097208E loc_97208E:                             ; CODE XREF: .text:00972089↑j
.text:0097208E                 jg      short loc_972093
.text:00972090                 jle     short loc_972093
.text:00972090 ; ---------------------------------------------------------------------------
.text:00972092                 db 0E8h
.text:00972093 ; ---------------------------------------------------------------------------
.text:00972093
.text:00972093 loc_972093:                             ; CODE XREF: .text:loc_97208E↑j
.text:00972093                                         ; .text:00972090↑j
.text:00972093                 mov     eax, [ebp-8]
.text:00972096                 lea     edx, [eax+1]
.text:00972099                 nop     dword ptr [eax+00000000h]
.text:009720A0
.text:009720A0 loc_9720A0:                             ; CODE XREF: .text:009720A5↓j
.text:009720A0                 mov     cl, [eax]
.text:009720A2                 inc     eax
.text:009720A3                 test    cl, cl
.text:009720A5                 jnz     short loc_9720A0
.text:009720A7                 sub     eax, edx
.text:009720A9                 cmp     eax, 31h ; '1'
.text:009720AC                 jnz     short loc_9720BF
.text:009720AE                 xor     ecx, ecx
.text:009720B0                 mov     eax, 1
.text:009720B5                 cmp     dword ptr [ebp-4], 36h ; '6'
.text:009720B9                 cmovz   ecx, eax
.text:009720BC                 mov     [ebp-0Ch], ecx
.text:009720BF
.text:009720BF loc_9720BF:                             ; CODE XREF: .text:009720AC↑j
.text:009720BF                 jo      short loc_9720C4
.text:009720C1                 jno     short loc_9720C4
.text:009720C1 ; ---------------------------------------------------------------------------
.text:009720C3                 db 0E9h
.text:009720C4 ; ---------------------------------------------------------------------------
.text:009720C4
.text:009720C4 loc_9720C4:                             ; CODE XREF: .text:loc_9720BF↑j
.text:009720C4                                         ; .text:009720C1↑j
.text:009720C4                 mov     eax, [ebp-0Ch]
.text:009720C7                 pop     edi
.text:009720C8                 pop     esi
.text:009720C9                 pop     ebx
.text:009720CA                 mov     esp, ebp
.text:009720CC                 pop     ebp
.text:009720CD                 retn