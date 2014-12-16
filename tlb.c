/*
    movzx eax,ch     ; extract bits 15..8 to get table index
    mov edx,ecx
    add edx,2          ; apply fudge factor for dword spanning check
    xor edx,pagetbl+eax*8
    test edx,0FFFF0101h ; check for address mismatch, misalignment, and/or spanning access
    mov edx,pagetbl+eax*8+4    ; map and adjust for endianness
    jne ReadHWLong    ; branch to slow path if any of those mismatches detected
    xor edx,ecx        ; map the guest address to host address
    movzx eax,dword ptr [edx-3]
*/

struct frame {
    unsigned long addr_page;
    unsigned long xor_value; 
};

struct frame pagetbl[256];
unsigned short * pb[256];

unsigned short * ReadGuestWordHandler(unsigned long);

unsigned short * ReadGuestLong(unsigned long addr) {
    unsigned long table_index = (addr >> 8) & 0x0FF;

    if (((pagetbl[table_index].addr_page ^ (addr + /* fudge */ 2)) 
        & 0xFF000101) == 0) {
            return (unsigned short *)(pb[(addr 
                ^ pagetbl[table_index].xor_value) - 1]);
    }

    return ReadGuestWordHandler(addr);
}
