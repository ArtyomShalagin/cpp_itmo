                section         .text

                global          start
start:

                sub             rsp, 2 * 128 * 8
                mov		rdi, rsp
                mov             rcx, 128
                call            read_long
                lea		rdi, [rsp + 128 * 8]
                call            read_long
                mov             rsi, rdi
		mov		rdi, rsp
		sub		rsp, 2 * 128 * 8
		mov 		r15, rdi
		mov		rdi, rsp
		mov		rcx, 256
		call		set_zero
		mov		rdi, r15
		mov		r15, rsp
		mov		rcx, 128
                call            mult_long_long

                call            write_long

                mov             al, 0x0a
                call            write_char

                jmp             exit

; multiplies two long number
;    rdi -- address of factor #1 (long number)
;    rsi -- address of factor #2 (long number)
;    rcx -- length of long numbers in qwords
; result:
;    product is written to rdi
mult_long_long:
                push            rdi
                push            rsi
                push            rcx

		xor 		r8, r8
		mov 		r14, rcx 	; save len
		mov		r9, rdi 	; save pointer
.loop_mult:
		mov		r10, [rsi] 	; curr value of #2 num
		lea		rsi, [rsi + 8]
		mov		rbx, r14 	; restore len
		mov		rdi, r9 	; restore rdi
		mov 		r13, r8		; set offset at sum
		xor 		r11, r11	; carry
		clc
.loop_add:	
                mov             rax, [rdi]
		lea             rdi, [rdi + 8]
		mul             r10
		
		add		[r15 + r13], rax; add first part to ans
		adc		r11, 0		; new carry here
		add		r13, 8 		; local offset moved
		add		[r15 + r13], r11
		xor		r11, r11
		add		[r15 + r13], rdx; add second part
		adc		r11, 0		; new carry here
		
		add		r13, 8
		add		[r15 + r13], r11; add new carry to the next part 
		sub		r13, 8
		xor		r11, r11	; by the end of the loop all carries added
                dec             rbx
                jnz             .loop_add
		
		add		r8, 8		; move offset
		dec		rcx 		; move on 1 number
		jnz		.loop_mult		

                pop             rcx
                pop             rsi
                pop             rdi
		mov		rdi, r15 	; mov rdi to ans
		ret
; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:
                push            rdi
                push            rcx
                push            rdx

                xor             rdx,rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rdx
                pop             rcx
                pop             rdi
                ret

; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to rdi
mul_long_short:
                push            rax
                push            rdi
                push            rcx

                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [rdi], rax
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rdi
                pop             rax
                ret

; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:
                push            rdi
                push            rax
                push            rcx

                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx

.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             rdi
                ret

; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep stosq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep scasq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long:
                push            rcx
                push            rdi

                call            set_zero
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char

                sub             rax, '0'
                mov             rbx, 10
                call            mul_long_short
                call            add_long_short
                jmp             .loop

.done:
                pop             rdi
                pop             rcx
                ret

.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char

.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop

; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx

                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax

                mov             rsi, rbp

.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop

                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string

                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret

; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
		mov 		rax, 0x2000003
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 0x2000004	;change to 1 in Linux
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 0x2000001 	;change to 60 in Linux
                xor             rdi, rdi
                syscall

; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax

                mov             rax, 0x2000004	;change to 1 in Linux
                mov             rdi, 1
                syscall

                pop             rax
                ret


                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: equ             $ - invalid_char_msg