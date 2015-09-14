;*********************************************
;	BootLoader.asm
;	512바이트 부트섹터 코드
;
;*********************************************
bits	16					; 최초 x86 머신은 16비트 명령어만 읽을 수 있으므로
							; 명확하게 이후 명령어들이 16비트 임을 선언한다. 
 
org		0				; 바이오스는 512바이트 부트섹터를 0x7C00 위치에 로드
							; 따라서 어셈블러에게 현재 물리주소가 0x7c00라고 알려주도록 한다.							
 
jmp Start
nop
bpbOEM			db "My OS   "
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	DB 2
bpbRootEntries: 	DW 224
bpbTotalSectors: 	DW 2880
bpbMedia: 		DB 0xf0  ;; 0xF1
bpbSectorsPerFAT: 	DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 	DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 		DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MOS FLOPPY "
bsFileSystem: 	        DB "FAT12   "
msgBootSectorLoaded  db "Boot Sector Loaded ", 0
;************************************************;
;	Prints a string
;	DS=>SI: 0 terminated string
;************************************************;
Print:
			lodsb				; load next byte from string from SI to AL
			or	al, al			; Does AL=0?
			jz	PrintDone		; Yep, null terminator found-bail out
			mov	ah, 0eh			; Nope-Print the character
			int	10h
			jmp	Print			; Repeat until null terminator found
	PrintDone:
			ret				; we are done, so return	
 
Start: 
	
   cli						; disable interrupts
          mov     ax, 0x07C0				; setup registers to point to our segment
          mov     ds, ax
          mov     es, ax
          mov     fs, ax
          mov     gs, ax

     ;----------------------------------------------------
     ; create stack
     ;----------------------------------------------------
     
          mov     ax, 0x0000				; set the stack
          mov     ss, ax
          mov     sp, 0xFFFF
          sti		

	mov	si, msgBootSectorLoaded						; our message to print
	call	Print						; call our print function

	xor	ax, ax						; clear ax
	int	0x12						; get the amount of KB from the BIOS

	cli							; Clear all Interrupts
	hlt							; halt the system
	

	
times 510 - ($-$$) db 0				; We have to be 512 bytes. Clear the rest of the bytes with 0
 
dw 0xAA55					; 부트섹터에 기록된 내용이 OS 부팅을 위한 섹터임을 확인하게 해주는 시그너처