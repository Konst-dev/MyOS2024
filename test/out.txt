00000000  00800A004865      add [eax+0x6548000a],al
00000006  6C                insb
00000007  6C                insb
00000008  6F                outsd
00000009  20576F            and [edi+0x6f],dl
0000000C  726C              jc 0x7a
0000000E  642121            and [fs:ecx],esp
00000011  2100              and [eax],eax
00000013  0488              add al,0x88
00000015  0100              add [eax],eax
00000017  001C00            add [eax+eax],bl
0000001A  0000              add [eax],al
0000001C  1C00              sbb al,0x0
0000001E  0000              add [eax],al
00000020  0000              add [eax],al
00000022  0000              add [eax],al
00000024  1500000000        adc eax,0x0
00000029  41                inc ecx
0000002A  0E                push cs
0000002B  088502420D05      or [ebp+0x50d4202],al
00000031  51                push ecx
00000032  C50C04            lds ecx,[esp+eax]
00000035  0400              add al,0x0
00000037  001C00            add [eax+eax],bl
0000003A  0000              add [eax],al
0000003C  3C00              cmp al,0x0
0000003E  0000              add [eax],al
00000040  150000001B        adc eax,0x1b000000
00000045  0000              add [eax],al
00000047  0000              add [eax],al
00000049  41                inc ecx
0000004A  0E                push cs
0000004B  088502420D05      or [ebp+0x50d4202],al
00000051  57                push edi
00000052  C50C04            lds ecx,[esp+eax]
00000055  0400              add al,0x0
00000057  001C00            add [eax+eax],bl
0000005A  0000              add [eax],al
0000005C  5C                pop esp
0000005D  0000              add [eax],al
0000005F  0030              add [eax],dh
00000061  0000              add [eax],al
00000063  001E              add [esi],bl
00000065  0000              add [eax],al
00000067  0000              add [eax],al
00000069  41                inc ecx
0000006A  0E                push cs
0000006B  088502420D05      or [ebp+0x50d4202],al
00000071  5A                pop edx
00000072  C50C04            lds ecx,[esp+eax]
00000075  0400              add al,0x0
00000077  001C00            add [eax+eax],bl
0000007A  0000              add [eax],al
0000007C  7C00              jl 0x7e
0000007E  0000              add [eax],al
00000080  4E                dec esi
00000081  0000              add [eax],al
00000083  001500000000      add [dword 0x0],dl
00000089  41                inc ecx
0000008A  0E                push cs
0000008B  088502420D05      or [ebp+0x50d4202],al
00000091  51                push ecx
00000092  C50C04            lds ecx,[esp+eax]
00000095  0400              add al,0x0
00000097  0020              add [eax],ah
00000099  0000              add [eax],al
0000009B  009C0000006300    add [eax+eax+0x630000],bl
000000A2  0000              add [eax],al
000000A4  3D00000000        cmp eax,0x0
000000A9  41                inc ecx
000000AA  0E                push cs
000000AB  088502420D05      or [ebp+0x50d4202],al
000000B1  41                inc ecx
000000B2  830378            add dword [ebx],byte +0x78
000000B5  C5                db 0xc5
000000B6  C3                ret
000000B7  0C04              or al,0x4
000000B9  0400              add al,0x0
000000BB  0020              add [eax],ah
000000BD  0000              add [eax],al
000000BF  00C0              add al,al
000000C1  0000              add [eax],al
000000C3  00A00000006D      add [eax+0x6d000000],ah
000000C9  0000              add [eax],al
000000CB  0000              add [eax],al
000000CD  41                inc ecx
000000CE  0E                push cs
000000CF  088502420D05      or [ebp+0x50d4202],al
000000D5  44                inc esp
000000D6  830302            add dword [ebx],byte +0x2
000000D9  65                gs
000000DA  C5                db 0xc5
000000DB  C3                ret
000000DC  0C04              or al,0x4
000000DE  0400              add al,0x0
000000E0  1000              adc [eax],al
000000E2  0000              add [eax],al
000000E4  E400              in al,0x0
000000E6  0000              add [eax],al
000000E8  0000              add [eax],al
000000EA  0000              add [eax],al
000000EC  0400              add al,0x0
000000EE  0000              add [eax],al
000000F0  0000              add [eax],al
000000F2  0000              add [eax],al
000000F4  1000              adc [eax],al
000000F6  0000              add [eax],al
000000F8  F8                clc
000000F9  0000              add [eax],al
000000FB  0000              add [eax],al
000000FD  0000              add [eax],al
000000FF  000400            add [eax+eax],al
00000102  0000              add [eax],al
00000104  0000              add [eax],al
00000106  0000              add [eax],al
00000108  8B5DFC            mov ebx,[ebp-0x4]
0000010B  C9                leave
0000010C  C3                ret
0000010D  6690              xchg ax,ax
0000010F  90                nop
00000110  E8FCFFFFFF        call 0x111
00000115  EBFE              jmp short 0x115
