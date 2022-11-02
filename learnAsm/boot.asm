ORG 0x7c00
BITS 16

start:
	mov si, message   ; copy content of message to si
	call print        ; call print label
	
	jmp $  ; jump to times below label 


print:
	mov bx, 0  ; set bx register to 0
.loop:
	lodsb      ; load each char from message to al
	cmp al, 0  ; compare content of al with 0
	je .done   ; je == jump equal, if compare is true --> jump to .done label
	call print_char  ; call print_char label
	jmp .loop  ; jump to .loop label

.done:
	ret        ; == return command in c 


print_char:
	mov ah, 0eh ; combine with int 0x10 to set video mode (video -- teletype output)
	int 0x10   ; call interrupt number 10
	ret

;0 mean end of string 

message: db 'Hello World khi that chuyen gi dang xay ra o day vay',0

times 510-($ -$$) db 0 ; not known
dw 0xAA55			   ; not known
