section .data
_MSG_OVERFLOW db 'Overflow detected. Aborting process', 0Dh, 0Ah
_MSG_OVERFLOW_SIZE EQU $-_MSG_OVERFLOW
_DES dd 10
_DISPLAY_INT db 0, 0, 0, 0, 0Dh, 0Ah
_CHARACTERS_READ db 'Caracteres lidos:  '
_CHARACTERS_READ_SIZE EQU $-_CHARACTERS_READ

; caracteres ascii
hifen EQU 2Dh

section .bss
_INT_CACHE resd 1
_CONVERTED_INT resb 4

section .text

%define buffer DWORD [EBP+8]
%define signal BYTE [EBP-1]
_LERINTEIRO:
    enter 1, 0

    ; lê os 4 caracteres: 1 para sinal e 3 para números
    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, 4
    int 80h

    ; declara a quantidade de caracteres lidos
    push eax    ; guarda para recuperar ao final da função
    mov DWORD [_INT_CACHE], eax
    push _CHARACTERS_READ_SIZE
    push _CHARACTERS_READ
    call _ESCREVERSTRING
    push _INT_CACHE
    call _ESCREVERINTEIRO

    ; itera entre cada um dos caracteres lidos para converter para um número
    mov ecx, eax
    ; guarda o endereço do buffer
    mov ebx, buffer
    mov eax, 0  ; vai armazenar o montante final

    ; verifica pelo sinal de negativo
    cmp BYTE [ebx], hifen  ; compara com o hífen
    je _INPUT_NEGATIVE
    mov signal, 1
    jmp _INPUT_LOOP
_INPUT_NEGATIVE:
    mov signal, -1
    inc ebx ; avança o ponteiro
    dec ecx ; desconta um ciclo de iteração
    
_INPUT_LOOP:
    ; guarda o ecx para poder usa-lo
    push ecx
    mov ecx, 0  ; zera o leitor de digito
    mov cl, [ebx] ; lê o char da posição indicada

    ; descarta o enter
    cmp cl, 0Ah
    je _INPUT_LOOP_END
    cmp cl, 0Dh
    je _INPUT_LOOP_END

    imul DWORD [_DES] ; abre espaço para o próximo dígito

    sub cl, 30h  ; tira o ascii de '0' para converter de char para int
    add al, cl   ; soma esse digito no montante    
    inc ebx  ; desloca o ponteiro para o próximo dígito

_INPUT_LOOP_END:
    pop ecx
    loop _INPUT_LOOP
    mov ebx, buffer

    ; aplica a negativa se for necessário
    cmp BYTE signal, -1
    jne _INPUT_COMMIT
    mov ecx, 0
    sub ecx, eax
    mov eax, ecx

_INPUT_COMMIT:    
    mov [ebx], eax ; armazena o que foi lido
    
    pop eax
    leave
    ret 4
    
%define buffer DWORD [EBP+8]
%define display_begin BYTE [EBP-1]
_ESCREVERINTEIRO:
    enter 1, 0

    ; se não for negativo, o número começa na primeira posição do display
    mov display_begin, 0

    %define remainder dl
    %define iteration ecx
    mov iteration, 0
    mov eax, buffer ; pega o endereço do número
    mov eax, [eax]  ; lê o número para eax

    ; verifica se é negativo
    cmp eax, 0
    jge _OUTPUT_LOOP    ; se não for menor que zero, segue normal
    mov BYTE [_DISPLAY_INT], hifen
    mov display_begin, 1    ; indica que o display agora deve começar após o primeiro caracter
    ; torna o número positivo para impressão em tela
    mov ebx, 0
    sub ebx, eax
    mov eax, ebx
    
_OUTPUT_LOOP:
    cdq ; estende o sinal antes da divisão
    idiv DWORD [_DES] ; pega o último dígito
    add remainder, 30h  ; converte para um char
    mov [_CONVERTED_INT + iteration], remainder
    inc iteration
    cmp eax, 0
    jne _OUTPUT_LOOP

    ; em ecx ficou o número total de iterações do loop anterior
    ; como é little endian, precisa inverter a order lida
    mov eax, 0
    add al, display_begin
_OUTPUT_LOOP2:
    mov ebx, ecx
    dec ebx ; necessário para acessar em ecx-1
    mov dl, [_CONVERTED_INT + ebx]
    mov BYTE [_DISPLAY_INT + eax], dl ; para cada dígito lido, coloca ele na ordem reversa no campo _DISPLAY_INT
    inc eax
    loop _OUTPUT_LOOP2

    ; coloca espaços no que sobrou, para não ter problemas em futuras impressões
_OUTPUT_SPACES:
    cmp eax, 4
    je _OUTPUT_ENTER    ; pula fora se já tiver preenchido os 4 espaços
    mov BYTE [_DISPLAY_INT + eax], 20h
    inc eax
    jmp _OUTPUT_SPACES

_OUTPUT_ENTER:

    mov eax, 4
    mov ebx, 1
    mov ecx, _DISPLAY_INT
    mov edx, 6
    int 80h
    
    leave
    ret 4

%define buffer DWORD [EBP+8]
_LERCHAR:
    enter 0, 0

    ; lê o caracter
    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, 1
    int 80h

    ; declara a quantidade de caracteres lidos
    push eax    ; guarda para recuperar ao final da função
    mov DWORD [_INT_CACHE], eax
    push _CHARACTERS_READ_SIZE
    push _CHARACTERS_READ
    call _ESCREVERSTRING
    push _INT_CACHE
    call _ESCREVERINTEIRO
    
    pop eax
    leave
    ret 4

%define buffer DWORD [EBP+8]
_ESCREVERCHAR:
    enter 0, 0

    ; mostra os caracteres
    mov eax, 4
    mov ebx, 1
    mov ecx, buffer
    mov edx, 1
    int 80h
    
    leave
    ret 4

%define size DWORD [EBP+12]
%define buffer DWORD [EBP+8]
_LERSTRING:
    enter 0, 0

    ; lê os caracteres
    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, size
    int 80h

    ; declara a quantidade de caracteres lidos
    push eax    ; guarda para recuperar ao final da função
    mov DWORD [_INT_CACHE], eax
    push _CHARACTERS_READ_SIZE
    push _CHARACTERS_READ
    call _ESCREVERSTRING
    push _INT_CACHE
    call _ESCREVERINTEIRO
    
    pop eax
    leave
    ret 8
    
%define size DWORD [EBP+12]
%define buffer DWORD [EBP+8]
_ESCREVERSTRING:
    enter 0, 0

    ; mostra os caracteres
    mov eax, 4
    mov ebx, 1
    mov ecx, buffer
    mov edx, size
    int 80h
    
    leave
    ret 8

_OVERFLOW:
    mov eax, 4
    mov ebx, 1
    mov ecx, _MSG_OVERFLOW
    mov edx, _MSG_OVERFLOW_SIZE
    int 80h

    mov eax, 1
    mov ebx, 1
    int 80h