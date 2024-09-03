	[BITS 16]
	[ORG 0x7c00]

		; check reading reading media with lba
		; read 2 first block (superblock & inode)
		; validate that it's a StupidFS by checking magic
		; check if inode 2 is present (loader reside in inode 2)
		; get loader size
		; iterate direct zone and load data to LOADER_BASE
		; iterate double indirect and load data (in order to keep things simple we won't support loader > 69632 bytes)

	LOADER_BASE  equ 0x1000
	DISK_BUFFER  equ 0x8000
	STPDFS_MAGIC equ 0x44505453
	BLOCK_SIZE   equ 512
	INODE_SIZE   equ 0x46
	INODE_ALLOCATED equ (1<<15)
	CR equ 0xD
	LF equ 0xA

	struc inode
		.mode:    resw 1
		.nlink:   resw 1
		.uid:     resw 1
		.gid:     resw 1
		.flags:   resw 1
		.size:    resd 1
		.zones:   resd 10
		.actime:  resq 1
		.modtime: resq 1
	endstruc

	jmp start
	nop

bpb:
	db 0, 0, 0
.oem_id:            db "STUPIDFS"
.sector_size:       dw 512
.sects_per_cluster: db 0
.reserved_sects:    dw 0
.fat_count:         db 0
.root_dir_entries:  dw 0
.sector_count:      dw 0
.media_type:        db 0
.sects_per_fat:     dw 0
.sects_per_track:   dw 18
.head_count:        dw 2
.hidden_sects:      dd 0
.sector_count_big:  dd 0
.drive_num:         db 0
.reserved:          db 0
.signature:         db 0
.volume_id:         dd 0
.volume_label:      db "STPD.MKFS  "
.filesystem_type:   times 8 db 0

start:
	cli
	cld
	jmp 0x0:.canonicalize_cs 

.canonicalize_cs:
	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov sp, 0x7c00

	mov [bpb.drive_num], dl

	; ensure lba mode is supported
	mov ah, 0x41
	mov bx, 0x55AA
	int 0x13
	jc .err_lba

	; reset disk
.reset_disk:
	mov dl, [bpb.drive_num]
	xor ah, ah
	int 0x13
	jc .reset_disk

	; read superblock
	mov ax, DISK_BUFFER/0x10
	mov es, ax

	; load two sector in memory 
	; superblock and first inodes
	mov eax, 1
	mov cx, 2
	xor bx, bx
	call disk_read_sectors
	
	mov dword eax, [DISK_BUFFER]
	cmp eax, STPDFS_MAGIC
	jne .err_magic

	; inode 0 bad inode
	; inode 1 root dir
	; inode 2 is loader, let's keep things easy
	mov ax, word [DISK_BUFFER + 512 + INODE_SIZE * 2 + inode.flags]
	and ax, INODE_ALLOCATED
	jz .err_no_loader

	mov dword eax, [DISK_BUFFER + 512 + INODE_SIZE * 2 + inode.size] ; size
	mov dword [uFsize], eax

	; copy data to LOADER_BASE
	mov ax, LOADER_BASE/0x10
	mov es, ax

	; direct read
	xor edx, edx
	xor ebx, ebx
.direct_loop:
	cmp ebx, [uFsize]
	jg .all_read

	mov eax, edx
	shl eax, 2
	add eax, DISK_BUFFER + 512 + INODE_SIZE * 2 + inode.zones

	push edx

	; copy block
	mov eax, [eax]
	mov cx, 1
	call disk_read_sectors

	pop edx

	add ebx, BLOCK_SIZE
	inc edx
	cmp edx, 7
	jb .direct_loop

.indirect_read:
	push ebx

	mov ax, DISK_BUFFER/0x10
	mov es, ax

	mov eax, [DISK_BUFFER + 512 + INODE_SIZE * 2 + inode.zones + 28] ;zones[7] == indirect
	mov cx, 1
	xor bx, bx
	call disk_read_sectors

	mov ax, LOADER_BASE/0x10
	mov es, ax
	pop ebx

	xor edx, edx
.indirect_loop:
	cmp ebx, [uFsize]
	jg .all_read
	mov eax, edx 
	shl eax, 2
	add eax, DISK_BUFFER

	mov eax, [eax]
	mov cx, 1
	push edx
	call disk_read_sectors
	pop edx

	add ebx, BLOCK_SIZE
	inc edx
	cmp edx, 128
	jbe .indirect_loop

.all_read:

	; jump to loader
	mov dl, [bpb.drive_num]
	jmp 0x0:LOADER_BASE


.err_no_loader:
	mov si, szErrorNoLoader
	call bios_print
	jmp .end
.err_magic:
	mov si, szErrorNoLoader
	call bios_print
	jmp .end
.err_read:
	mov si, szErrorRead
	call bios_print
	jmp .end
.err_lba:
	mov si, szErrorLBA
	call bios_print
.end:
	hlt
	jmp $

bios_print:
	lodsb
	or al, al
	jz .end
	mov ah, 0x0E
	int 0x10
	jmp bios_print
.end:
	ret

szErrorNoLoader: db "Loader not found", CR, LF, 0
szErrorBadMagic: db "Err: magic", CR, LF, 0
szErrorLBA: db "Err: LBA", CR, LF, 0
szErrorRead: db "Err: read", CR, LF, 0
uFsize: dd 0

disk_read_sectors:
	mov word [disk_packet.sectors], cx
	mov word [disk_packet.segment], es
	mov word [disk_packet.offset], bx
	mov dword [disk_packet.lba], eax 
	mov si, disk_packet
	mov dl, [bpb.drive_num]	
	mov ah, 0x42
	int 0x13
	jc start.err_read
	ret

disk_packet:
	db 0x10
	db 0
.sectors:
	dw 0
.offset:
	dw 0
.segment:
	dw 0
.lba:
	dd 0
	dd 0

	times 510-($-$$) db 0
	dw 0xAA55
