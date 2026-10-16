## Crackme #9

There are lots of functions with junk codes, after removing obfuscating, some of them have similar format:

```cpp 
int __thiscall Obf_funcXX(_DWORD *this)
{
  this_1 = this;
  if ( !this[XX] )
  {
    xx = Val;
    v4[1] = *this_1;
    v5 = Obf_funcXX(this_1);
    v2 = sub_401CBA(v4, v5, Val);
    v3 = sub_40118D(v2);
    this_1[12] = v3;
  }
  return this_1[XX];
}
```

The `sub_401CBA` is a function to search for DLL functions:

| Address of function    | Name sub_401CBA returns         |
| ---------------------- | ------------------------------- |
| sub_401397             | user32_CallWindowProcA          |
| sub_401010, sub_4013DA | advapi32_CryptAcquireContextA   |
| sub_401022, sub_401443 | advapi32_CryptCreateHash        |
| sub_401058, sub_40158D | advapi32_CryptDestroyHash       |
| sub_40107C, sub_40166A | advapi32_CryptGetHashParam      |
| sub_40108E, sub_4016DF | advapi32_CryptHashData          |
| sub_402C24, sub_40187A | ntdll_KiUserExceptionDispatcher |
| sub_4010E4, sub_40194C | ntdll_NtContinue                |
| sub_4010F6, sub_4019B5 | ntdll_NtQueryInformationProcess |
| sub_402BC7, sub_401B2E | ntdll_Wow64Transition           |

Notice that there is a function executed before main:

```cpp
void *__thiscall Hook_CallWindowProcA(void *this)
{
  sub_40113E();
  sub_40112C();
  CallWindowProcA = (LRESULT (__stdcall *)(WNDPROC, HWND, UINT, WPARAM, LPARAM))sub_402E5A;
  return this;
}
```

This function changed the address of `CallWindowProcA` to a custom function `sub_402E5A`. For this function:

```cpp
int __stdcall Hooked_CallWindowProcA(int a1, int a2, int a3, int a4, int a5)
{
  v5 = sub_402BF1();
  if ( sub_402B6D(v5) )
  {
    v6 = (_BYTE *)sub_402CAC();
    sub_402CE0(v6);
  }
  else
  {
    sub_402CAC();
    sub_402C24(p_sub_402E1C, &unk_40D24C);
  }
  v7 = sub_401367();
  v9 = (int (__cdecl *)(int, int, int, int, int))sub_401397(v7);
  return v9(a1, a2, a3, a4, a5);
}
```

The function `sub_402B6D` is an anti-debug function, will return 1 if debugger exists. `p_sub_402E1C` will install some handler for exception:

```cpp
void __cdecl __noreturn p_sub_402E1C(int a1)
{
  unk_40D254 = a1;
  dword_40D250 = retaddr;
  v1 = sub_40448C();
  sub_404562(v1, (_DWORD *)dword_40D250, unk_40D254);
  v2 = sub_40113E();
  sub_4010E4(v2, unk_40D254, 0);
  __debugbreak();
}

_DWORD *__thiscall sub_404562(int this, _DWORD *a2, int a3)
{
  if ( !*(_BYTE *)(this + 8) )
  {
    result = a2;
    switch ( *a2 )
    {
      case 0x80000003:
        return (_DWORD *)sub_4042EC((_DWORD *)this, a3);
      case 0x80000004:
        return (_DWORD *)sub_4043CA(this, a3);
      case 0x80000001:
        return (_DWORD *)sub_404350(this, (int)a2, a3);
    }
  }
  return result;
}
```

Then for the interaction logic of the program, check dialog table:

```assembly
.rdata:0040B3F0                 dd offset ??_R4MainDialog@@6B@ ; const MainDialog::`RTTI Complete Object Locator'
.rdata:0040B3F4 ; const MainDialog::`vftable'
.rdata:0040B3F4 ??_7MainDialog@@6B@ dd offset sub_405377
.rdata:0040B3F4                                         ; DATA XREF: sub_405196+15↑o
.rdata:0040B3F4                                         ; sub_4052B8+7↑o
.rdata:0040B3F8                 dd offset sub_4055D3
.rdata:0040B3FC                 dd offset sub_405536
.rdata:0040B400                 dd offset sub_404E71
.rdata:0040B404                 dd offset sub_405497
```

Interface selection function:

```cpp
int __userpurge sub_405536@<eax>(int a1@<ecx>, int a2@<ebx>, __int16 a3, int a4)
{
  switch ( a3 )
  {
    case 1001:
      sub_405399(a1, a2);
      break;
    case 1002:
      hInstance = *(HINSTANCE *)(a1 + 4);
      sub_4025EC(v7, &unk_40B3CD);
      sub_40574D(hInstance, 132, v7[0], v7[1], v7[2], v7[3], v7[4], v7[5]);
      sub_404E89((LPARAM)dwInitParam, *(HWND *)(a1 + 8));
      sub_4057DA(dwInitParam);
      break;
    case 1003:
      EndDialog(*(HWND *)(a1 + 8), 0);
      break;
    case 1007:
      sub_405702();
      break;
    case 1010:
      ShellExecuteA(0, "open", "https://crackmes.one", 0, 0, 3);
      break;
    default:
      return 0;
  }
  return 1;
}
```

For the check part:

```cpp
unsigned int __usercall sub_405399@<eax>(int a1@<ecx>, int a2@<ebx>)
{
  memset(input_, 0, sizeof(input_));
  GetDlgItemTextA(*(HWND *)(a1 + 8), 1006, input_, 255);
  if ( sub_4030A5(*(_DWORD **)(a1 + 20), a2, (int)input_) )
  {
    sub_4025EC(Src_1, input_);
    sub_40268B(v16, (int)Src_1);
    sub_405724((int)Src_1);
    sub_402876(Src, (int)v17);
    hInstance = *(HINSTANCE *)(a1 + 4);
    sub_40515A(&v6, Src);
    sub_40574D(dwInitParam, hInstance, 134, v6, v7, v8, v9, v10, v11);
    sub_404E89((LPARAM)dwInitParam, *(HWND *)(a1 + 8));
    sub_4057DA(dwInitParam);
    sub_405724((int)Src);
    return sub_402781(v16);
  }
  else
  {
    hInstance_1 = *(HINSTANCE *)(a1 + 4);
    sub_4025EC(&v6, &unk_40B3CE);
    sub_40574D(dwInitParam, hInstance_1, 136, v6, v7, v8, v9, v10, v11);
    sub_404E89((LPARAM)dwInitParam, *(HWND *)(a1 + 8));
    return sub_4057DA(dwInitParam);
  }
}
```

The `sub_4030A5` is the input processing function:

```cpp
char __userpurge sub_4030A5@<al>(int *a1@<ecx>, int a2@<ebx>, int input)
{
  if ( !input )
    return 0;
  v5 = sub_40448C();
  sub_4046C8((int)v5, *a1, a1[1], a1[2], a1[3]);
  v6 = sub_408AAA(0x14u);
  if ( v6 )
  {
    *v6 = 0;
    v6[1] = 0;
    v6[2] = 0;
    v6[3] = 0;
    v6[4] = 0;
    v7 = (void *)((int (*)(void))loc_40A000)();
  }
  else
  {
    v7 = 0;
  }
  n20 = 20;
  v8 = (*(int (__thiscall **)(void *, int, int))byte_40A025)(v7, input, a2);
  sub_408ADA(v7);
  v9 = sub_40448C();
  sub_404732((int)v9);
  return v8;
}
```

At sub_4046C8, once 0x80000003 and several times 0x80000004 were triggered. Next, the program runs to a custom section `.pc` at 0x40A000. This area is filled with illegal instructions and soon caused exception 0x80000001:

```cpp
int __thiscall Handler_0x80000001(int this, int a2, int a3)
{
  if ( **(_BYTE **)(this + 36) )
  {
    sub_4037C9();
    if ( *(_DWORD *)(a2 + 16) )
    {
      n8 = *(_DWORD *)(a2 + 20);
      if ( n8 < 2 )
      {
        sub_4037CD(**(_DWORD **)(this + 28), *(_DWORD *)(*(_DWORD *)(this + 28) + 8));
      }
      else if ( n8 == 8 )
      {
        v5 = a3;
        v6 = *(char **)(a3 + 184);
        if ( sub_4044FA((_DWORD *)this, (unsigned int)v6) )
        {
          *(_DWORD *)(this + 60) = v6;
          sub_40413B((void *)this, v5, v6);
        }
        else
        {
          sub_4037FA(&a3);
        }
      }
    }
  }
  return -1;
}
```

The a2 is the kind of exception, and `a3 + 184` is `CONTEXT.Eip`. In sub_4037FA, the a1 points to DR (Debug Registers), and this function set DR0, DR6 and DR7 to 0, which equivalents to clearing all hardware breakpoints:

```cpp
int __stdcall sub_4037FA(int *a1)
{
  *(_DWORD *)(*a1 + 24) = 0;
  *(_DWORD *)(*a1 + 20) = 0;
  *(_DWORD *)(*a1 + 4) = 0;
  result = *a1;
  *(_DWORD *)(*a1 + 192) |= 0x100u;
  return result;
}
```

Patch to:

```cpp
int __stdcall sub_4037FA(int *a1)
{
  result = *a1;
  *(_DWORD *)(*a1 + 192) |= 0x100u;
  return result;
}
```

At sub_40413B, there's an algorithm:

```cpp
int __thiscall sub_40413B(void *this, int a2, char *Src)
{
  v3 = sub_403FBA((int)this, Src);
  sub_404684((int)&a2, (int)v3);
  return sub_4037FA(&a2);
}

char *__thiscall sub_403FBA(int this, char *Src)
{
  sub_404286((int **)this);
  sub_40464A((_DWORD *)this);
  Src_1 = Src;
  v4 = sub_404059((size_t *)this, Src);
  if ( sub_40477E((char *)v4) )
  {
    Src_2 = (char *)sub_40479C((_DWORD *)this, *(_DWORD *)(this + 4), 12288, 64);
    *(_DWORD *)(this + 56) = Src_2;
    Src = Src_2;
    sub_403A69((int *)(this + 48), v7, &Src);
    memcpy(*(void **)(this + 56), v4, *(_DWORD *)this);
    Src_1 = *(char **)(this + 56);
  }
  else
  {
    memcpy(Src_1, v4, *(_DWORD *)this);
    Src = &Src_1[-**(_DWORD **)(this + 28)];
    sub_4039D4((int *)(this + 40), v7, &Src);
  }
  j_j_free_0(v4);
  return Src_1;
}

void *__thiscall sub_404059(size_t *this, char *Src)
{
  v3 = sub_408AE8(*this);
  memcpy(v3, Src, *this);
  v4 = &Src[-*(_DWORD *)this[7]];
  v10[0] = 0xD0C0B0A;
  v10[1] = 0x11100F0E;
  v5 = (char *)sub_403477();
  v6 = sub_40330A(v5);
  sub_401E39(src_, (int)v6, (int)v10, 0, 0);
  sub_4022B8((int)src_, (unsigned int)v4);
  sub_401F4D(src_, v7, (int)v3, 0x10u);
  return v3;
}

_DWORD *__thiscall sub_401E39(_DWORD *this, int a2, int a3, int a4, int a5)
{
  sub_401E67(this, a2, a3);
  this[12] = a4;
  this[13] = a5;
  this_1 = this;
  this[32] = 64;
  return this_1;
}

_DWORD *__thiscall sub_401E67(_DWORD *this, int a2, int a3)
{
  key[0] = 0xB979379E;
  key[1] = 0x157C4A7F;
  key[2] = 0x60C09CF3;
  key[3] = 0x34C8ED5C;
  *this = sub_402292(key);
  this[1] = sub_402292(&key[1]);
  this[2] = sub_402292(&key[2]);
  this[3] = sub_402292(&key[3]);
  this[4] = sub_402292(a2);
  this[5] = sub_402292(a2 + 4);
  this[6] = sub_402292(a2 + 8);
  this[7] = sub_402292(a2 + 12);
  this[8] = sub_402292(a2 + 16);
  this[9] = sub_402292(a2 + 20);
  this[10] = sub_402292(a2 + 24);
  this[11] = sub_402292(a2 + 28);
  this[12] = 0;
  this[13] = 0;
  this[14] = sub_402292(a3);
  this[15] = sub_402292(a3 + 4);
  return this;
}
```

The array a2 is 8 dwords at 0x40D268, generated from the program's .text section's sha256 hash. This means we can't set software breakpoints and patch the program before the checksum finished. The right checksum is `F6 30 AA 38 D5 72 97 37 5D 64 55 59 C3 34 FD 50 D5 5C A1 D1 77 D2 65 5A 04 23 51 CF 69 24 4B F2`.  After patching the checksum:

```cpp
_DWORD *__thiscall sub_401E67(_DWORD *this, int a2, int a3)
{
  v5[0] = 0xB979379E;
  v5[1] = 0x157C4A7F;
  v5[2] = 0x60C09CF3;
  v5[3] = 0x34C8ED5C;
  *this = sub_402292(v5);
  this[1] = sub_402292(&v5[1]);
  this[2] = sub_402292(&v5[2]);
  this[3] = sub_402292(&v5[3]);
  this[4] = 0x38AA30F6;
  this[5] = 0x379772D5;
  this[6] = 0x5955645D;
  this[7] = 0x50FD34C3;
  this[8] = 0xD1A15CD5;
  this[9] = 0x5A65D277;
  this[10] = 0xCF512304;
  this[11] = 0xF24B2469;
  this[12] = 0;
  this[13] = 0;
  this[14] = sub_402292(a3);
  this[15] = sub_402292(a3 + 4);
  return this;
}
```

the program successfully returns the "try again" window. The self-decrypt algorithm is a modified chacha20. Decrypt the .pc section:

```python
import struct
from pathlib import Path

def rotl32(x, n): return ((x << n) | (x >> (32 - n))) & 0xFFFFFFFF

def quarter_round(x, a, b, c, d):
    x[a] = (x[a] + x[b]) & 0xFFFFFFFF
    x[d] ^= x[a]; x[d] = rotl32(x[d], 16)
    x[c] = (x[c] + x[d]) & 0xFFFFFFFF
    x[b] ^= x[c]; x[b] = rotl32(x[b], 12)
    x[a] = (x[a] + x[b]) & 0xFFFFFFFF
    x[d] ^= x[a]; x[d] = rotl32(x[d], 8)
    x[c] = (x[c] + x[d]) & 0xFFFFFFFF
    x[b] ^= x[c]; x[b] = rotl32(x[b], 7)

def chacha20_block(state16_words):
    x = state16_words[:]
    for _ in range(10):
        quarter_round(x, 0, 4, 8, 12)
        quarter_round(x, 1, 5, 9, 13)
        quarter_round(x, 2, 6, 10, 14)
        quarter_round(x, 3, 7, 11, 15)
        quarter_round(x, 0, 5, 10, 15)
        quarter_round(x, 1, 6, 11, 12)
        quarter_round(x, 2, 7, 8, 13)
        quarter_round(x, 3, 4, 9, 14)
    out = [(x[i] + state16_words[i]) & 0xFFFFFFFF for i in range(16)]
    return b"".join(struct.pack("<I", w) for w in out)

def chacha20_xor(data_bytes, consts4, key8, nonce2, counter0=0, counter1=0):
    out = bytearray()
    c0, c1 = counter0 & 0xFFFFFFFF, counter1 & 0xFFFFFFFF
    for bi in range((len(data_bytes) + 63) // 64):
        state = consts4 + key8 + [c0, c1] + nonce2
        ks = chacha20_block(state)
        chunk = data_bytes[bi * 64:(bi + 1) * 64]
        out.extend(bytes(chunk[i] ^ ks[i] for i in range(len(chunk))))
        c0 = (c0 + 1) & 0xFFFFFFFF
        if c0 == 0: c1 = (c1 + 1) & 0xFFFFFFFF
    return bytes(out)

def extract_section(pe_path: Path, sec_name: str) -> bytes:
    pe = pe_path.read_bytes()
    if pe[:2] != b"MZ": raise ValueError("Not an MZ executable")
    e_lfanew = struct.unpack_from("<I", pe, 0x3C)[0]
    if pe[e_lfanew:e_lfanew+4] != b"PE\0\0": raise ValueError("Not a PE file")
    coff = e_lfanew + 4
    num_sections = struct.unpack_from("<H", pe, coff + 2)[0]
    opt_size = struct.unpack_from("<H", pe, coff + 16)[0]
    sec_off = coff + 20 + opt_size
    for i in range(num_sections):
        off = sec_off + i * 40
        name = pe[off:off+8].split(b"\0", 1)[0].decode("ascii", "ignore")
        vsize, vaddr, raw_size, raw_ptr = struct.unpack_from("<IIII", pe, off + 8)
        if name == sec_name: return pe[raw_ptr:raw_ptr + raw_size]
    raise KeyError(f"Section {sec_name!r} not found")

def main():
    consts = [0xB979379E, 0x157C4A7F, 0x60C09CF3, 0x34C8ED5C]
    key    = [0x38AA30F6, 0x379772D5, 0x5955645D, 0x50FD34C3, 0xD1A15CD5, 0x5A65D277, 0xCF512304, 0xF24B2469]
    counter = (0, 0)
    nonce  = [0x0D0C0B0A, 0x11100F0E]
    exe = Path("Crackme.exe")
    pc  = extract_section(exe, ".pc")
    dec = chacha20_xor(pc, consts, key, nonce, counter0=counter[0], counter1=counter[1])
    Path("pc_decrypted.bin").write_bytes(dec)
    print(f"[+] .pc size = {len(pc)} bytes")

if __name__ == "__main__":
    main()
```

However, the decrypted shellcode is full of junk codes and control flow flattening obfuscating, making it difficult to analyze.

To find the function after decrypt, after called `loc_40A000`, in `sub_40413B`:

```cpp
int __thiscall sub_40413B(void *this, int a2, char *Src)
{
  v3 = sub_403FBA((int)this, Src);
  sub_404684((int)&a2, (int)v3);
  return sub_4037FA(&a2);
}
```

The `v3` is the place of decrypted function. First function:

```c++
_DWORD *__fastcall sub_40A000(_DWORD *key)
{
  key[0] = 0x865DBB47;
  key[1] = 0x0A6EB190;
  key[2] = 0x20476C33;
  key[3] = 0x1C8A7693;
  key[4] = 0x59FEBDFB;
}
```

returns 5 dwords (key). Debugging found that the program decrypts 16 bytes of encrypted bytecode each time, and then only executes one line of assembly code. The next decryption starts from the end of the previous assembly code. When set breakpoint at 0x404010 (before memcpy of decrypted code),  the code will store at *EBX. Dump the assembly code:

```python
import os
import struct
import idc
import ida_dbg
import ida_kernwin
import idaapi

BREAK_EA        = 0x00404010
OUTPUT_PATH     = r"E:\CTF\temp\dump.bin"
READ_SIZE       = 0x10
PAD_SIZE        = 0x10
g_hooks = None
g_fp = None

def _execute_sync(fn):
    try: ida_kernwin.execute_sync(fn, ida_kernwin.MFF_FAST)
    except Exception:
        try: fn()
        except Exception: pass

def _read_mem(ea, size, tid=None):
    try:
        bs = idaapi.dbg_read_memory(ea, size)
        if bs: return bytes(bs)
    except Exception: pass
    try:
        bs = ida_dbg.read_dbg_memory(ea, size)
        if bs: return bytes(bs)
    except Exception: pass
    if tid is not None:
        try:
            bs = ida_dbg.read_dbg_memory(tid, ea, size)
            if bs: return bytes(bs)
        except Exception: pass
    try:
        buf = bytearray(size)
        ok = ida_dbg.read_dbg_memory(ea, buf, size)
        if ok: return bytes(buf)
    except Exception: pass
    print(f"[dump] failed to read dbg memory @ 0x{ea:08X}, size=0x{size:X}")
    return None

def _dump_once(tid=None):
    global g_fp
    try: p = ida_dbg.get_reg_val("EBX")
    except Exception as e:
        print(f"[dump] get_reg_val(EBX) failed: {e}")
        return
    if p is None or p < 0x10000:
        print(f"[dump] EBX=0x{0 if p is None else p:08X} looks invalid, skip")
        return
    data = _read_mem(p, READ_SIZE, tid=tid)
    if data is None:
        print(f"[dump] failed to read {READ_SIZE} bytes @ EBX=0x{p:08X}")
        return
    if len(data) < READ_SIZE: data += b"\x00" * (READ_SIZE - len(data))
    out = data + (b"\x00" * PAD_SIZE)
    try:
        g_fp.write(out)
        g_fp.flush()
        print(f"[dump] hit 0x{BREAK_EA:08X} | EBX=0x{p:08X} | wrote {len(out)} bytes")
    except Exception as e: print(f"[dump] file write error: {e}")

def _continue():
    try:
        ida_dbg.continue_process()
        return
    except Exception: pass
    try:
        ida_dbg.request_continue_process()
        return
    except Exception as e: print(f"[dump] continue failed: {e}")

class DumpHooks(ida_dbg.DBG_Hooks):
    def dbg_bpt(self, tid, ea):
        try:
            if ea != BREAK_EA: return 0
            _dump_once(tid=tid)
            def _cont(): _continue()
            _execute_sync(_cont)
            return 0
        except Exception as e:
            print(f"[dump] dbg_bpt exception: {e}")
            return 0

    def dbg_process_exit(self, pid, tid, ea, exit_code):
        print(f"[dump] process exit (code={exit_code}), stopping.")
        try: stop()
        except Exception: pass
        return 0

def start():
    global g_hooks, g_fp
    if g_hooks is not None:
        print("[dump] already started")
        return
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    g_fp = open(OUTPUT_PATH, "ab")
    try:
        idc.add_bpt(BREAK_EA)
        idc.enable_bpt(BREAK_EA, True)
    except Exception:
        try: ida_dbg.add_bpt(BREAK_EA)
        except Exception as e:
            print(f"[dump] add_bpt failed: {e}")
            raise
    g_hooks = DumpHooks()
    g_hooks.hook()
    print("[dump] started.")
    print(f"  BREAK_EA = 0x{BREAK_EA:08X}")
    print(f"  OUTPUT   = {OUTPUT_PATH}")

def stop():
    global g_hooks, g_fp
    if g_hooks is not None:
        try: g_hooks.unhook()
        except Exception: pass
        g_hooks = None
    try: idc.enable_bpt(BREAK_EA, False)
    except Exception: pass
    if g_fp is not None:
        try:
            g_fp.flush()
            g_fp.close()
        except Exception: pass
        g_fp = None
    print("[dump] stopped.")

if __name__ == "__main__":
    start()
```

Disasm the shellcode:

```python
# -*- coding: utf-8 -*-
import os
import idc
import idaapi
import ida_bytes
import ida_funcs
import ida_lines
import idautils

OUTPUT_PATH = r"E:\CTF\temp\trace.txt"
STEP        = 0x40
TARGET_SEG_NAME = None

def pick_target_seg():
    if TARGET_SEG_NAME:
        seg = idaapi.get_segm_by_name(TARGET_SEG_NAME)
        if seg: return seg
        print(f"[trace] segment '{TARGET_SEG_NAME}' not found, fallback to auto-pick")
    seg0 = idaapi.getseg(0)
    if seg0: return seg0
    best = None
    for s in idautils.Segments():
        seg = idaapi.getseg(s)
        if not seg: continue
        if best is None or seg.start_ea < best.start_ea: best = seg
    return best

def undefine_all(seg_start, seg_end):
    funcs = list(idautils.Functions(seg_start, seg_end))
    for f in funcs:
        try: ida_funcs.del_func(f)
        except Exception: pass
    size = seg_end - seg_start
    try: ida_bytes.del_items(seg_start, ida_bytes.DELIT_EXPAND | ida_bytes.DELIT_SIMPLE, size)
    except Exception: ida_bytes.del_items(seg_start, ida_bytes.DELIT_SIMPLE, size)

def make_code_and_get_line(ea):
    try: idc.create_insn(ea)
    except Exception: pass
    line = idc.generate_disasm_line(ea, 0)
    if not line:
        try: idaapi.auto_wait()
        except Exception: pass
        line = idc.generate_disasm_line(ea, 0)
    if not line: return None
    try: line = ida_lines.tag_remove(line)
    except Exception: pass
    return line.strip()

def main():
    seg = pick_target_seg()
    if not seg:
        print("[trace] no segment found")
        return
    seg_start = seg.start_ea
    seg_end   = seg.end_ea
    print("[trace] target segment:")
    print(f"  name  = {idaapi.get_segm_name(seg)}")
    print(f"  range = 0x{seg_start:08X} - 0x{seg_end:08X} (size=0x{seg_end-seg_start:X})")
    print(f"  step  = 0x{STEP:X}")
    print(f"  out   = {OUTPUT_PATH}")
    undefine_all(seg_start, seg_end)
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    addrs = list(range(seg_start, seg_end, STEP))
    print(f"[trace] creating code & dumping first lines: {len(addrs)} entries ...")
    with open(OUTPUT_PATH, "w", encoding="utf-8", newline="\n") as fp:
        for ea in addrs:
            line = make_code_and_get_line(ea)
            if line is None: fp.write(f"{ea:08X}: <disasm failed>\n")
            else: fp.write(f"{ea:08X}: {line}\n")
    print("[trace] done.")

if __name__ == "__main__":
    main()
```

From the trace, it was found that the shellcode has the behavior of accessing sha256 hash, as previous program hash. Set bp at `advapi32_CryptCreateHash`, `advapi32_CryptHashData` and `advapi32_CryptGetHashParam`, ant it was found that these functions were called at least dozens of times, and each time they were generating a hash of the .text section. Hook hash:

```python
import ida_dbg
import idaapi
import idc
import struct
import os

TEXT_BASE      = 0x401000
TEXT_LEN       = 0x8A00
PATCH_DST      = 0x40D268
CRYPTSP_GHP_RET_EA_ABS = 0x720D50D2
CRYPT_HASHDATA_NAME_CANDIDATES = ["cryptsp_CryptHashData", "cryptsp.dll_CryptHashData", "CryptHashData"]
PATCH_BYTES = bytes.fromhex("F6 30 AA 38 D5 72 97 37 5D 64 55 59 C3 34 FD 50 D5 5C A1 D1 77 D2 65 5A 04 23 51 CF 69 24 4B F2")
g_hooks = None
g_hash_seen_text = set()

def _u32(x): return x & 0xFFFFFFFF

def dbg_read_mem(ea, n):
    try:
        bs = idaapi.dbg_read_memory(ea, n)
        return bytes(bs) if bs else None
    except Exception: return None

def dbg_write_mem(ea, data: bytes):
    try:
        ok = idaapi.dbg_write_memory(ea, data)
        return bool(ok)
    except Exception: pass
    try:
        ok = ida_dbg.write_dbg_memory(ea, data)
        return bool(ok)
    except Exception: pass
    try:
        buf = bytearray(data)
        ok = ida_dbg.write_dbg_memory(ea, buf, len(buf))
        return bool(ok)
    except Exception: return False

def dbg_read_u32(ea):
    bs = dbg_read_mem(ea, 4)
    if not bs or len(bs) != 4: return None
    return struct.unpack("<I", bs)[0]

def dbg_write_u32(ea, v): return dbg_write_mem(ea, struct.pack("<I", _u32(v)))

def is_in_text(pb, cb):
    if pb is None or cb is None: return False
    start = pb
    end   = pb + cb
    return (start >= TEXT_BASE) and (end <= (TEXT_BASE + TEXT_LEN))

def find_module_base(substr: str):
    substr = substr.lower()
    try:
        m = ida_dbg.get_first_module()
        while m:
            name = (getattr(m, "name", "") or "").lower()
            if substr in name: return int(getattr(m, "base", 0))
            m = ida_dbg.get_next_module(m)
    except Exception: pass
    return None

def resolve_ea_by_names(names):
    for nm in names:
        ea = idc.get_name_ea_simple(nm)
        if ea != idc.BADADDR and ea != 0: return ea
    return None

def add_bpt(ea):
    try:
        idc.add_bpt(ea)
        idc.enable_bpt(ea, True)
        return True
    except Exception: pass
    try:
        ida_dbg.add_bpt(ea)
        return True
    except Exception: return False

class Hooks(ida_dbg.DBG_Hooks):
    def dbg_bpt(self, tid, ea):
        esp = ida_dbg.get_reg_val("ESP")
        if ea == self.crypt_hashdata_ea:
            hHash  = dbg_read_u32(esp + 4)
            pbData = dbg_read_u32(esp + 8)
            cbData = dbg_read_u32(esp + 0xC)
            if is_in_text(pbData, cbData):
                g_hash_seen_text.add(hHash)
                print(f"[hash] tid={tid} hHash=0x{hHash:08X} HASHED_TEXT pb=0x{pbData:08X} len=0x{cbData:X}")
            ida_dbg.continue_process()
            return 0
        if ea == self.crypt_gethashparam_ret_ea:
            hHash    = dbg_read_u32(esp + 4)
            dwParam  = dbg_read_u32(esp + 8)
            pbOut    = dbg_read_u32(esp + 0xC)
            pdwLen   = dbg_read_u32(esp + 0x10)
            do_patch = (dwParam == 2 and hHash in g_hash_seen_text and pbOut == PATCH_DST)
            if do_patch:
                ok1 = dbg_write_mem(PATCH_DST, PATCH_BYTES)
                ok2 = True
                if pdwLen: ok2 = dbg_write_u32(pdwLen, 0x20)
                print(f"[patch] tid={tid} hHash=0x{hHash:08X} patched @0x{PATCH_DST:08X} bytes_ok={ok1} len_ok={ok2} (dwParam=HP_HASHVAL)")
            ida_dbg.continue_process()
            return 0
        return 0

def start():
    global g_hooks
    if g_hooks is not None:
        print("[patchhash] already started")
        return
    cryptsp_base = find_module_base("cryptsp")
    if cryptsp_base: crypt_gethashparam_ret = cryptsp_base + 0x50D2
    else: crypt_gethashparam_ret = CRYPTSP_GHP_RET_EA_ABS
    crypt_hashdata_ea = resolve_ea_by_names(CRYPT_HASHDATA_NAME_CANDIDATES)
    if crypt_hashdata_ea is None:
        print("[patchhash] WARNING: cannot resolve cryptsp_CryptHashData by name.")
        print("[patchhash] Please rename it or set its EA manually in the script.")
        return
    if not add_bpt(crypt_hashdata_ea):
        print(f"[patchhash] failed to add bpt @ 0x{crypt_hashdata_ea:08X} (CryptHashData)")
        return
    if not add_bpt(crypt_gethashparam_ret):
        print(f"[patchhash] failed to add bpt @ 0x{crypt_gethashparam_ret:08X} (GetHashParam RET)")
        return
    g_hooks = Hooks()
    g_hooks.crypt_hashdata_ea = crypt_hashdata_ea
    g_hooks.crypt_gethashparam_ret_ea = crypt_gethashparam_ret
    g_hooks.hook()
    print("[patchhash] started.")
    print(f"  CryptHashData EA      = 0x{crypt_hashdata_ea:08X}")
    print(f"  GetHashParam RET EA   = 0x{crypt_gethashparam_ret:08X}")
    print(f"  TEXT region           = 0x{TEXT_BASE:08X} .. 0x{(TEXT_BASE+TEXT_LEN):08X}")
    print(f"  PATCH dst             = 0x{PATCH_DST:08X}")
    print(f"  PATCH bytes len       = {len(PATCH_BYTES)}")
def stop():
    global g_hooks
    if g_hooks:
        try: g_hooks.unhook()
        except Exception: pass
        g_hooks = None
    print("[patchhash] stopped.")

start()
```

Mix:

```python
import os
import struct
import idc
import ida_dbg
import ida_kernwin
import idaapi

BREAK_EA        = 0x00404010
OUTPUT_PATH     = r"E:\CTF\temp\dump.bin"
READ_SIZE       = 0x10
PAD_SIZE        = 0x10
APPEND_OUTPUT   = False
DEDUP_CONSECUTIVE = True
MAX_DUMPS       = 0
TEXT_BASE       = 0x00401000
TEXT_LEN        = 0x8A00
PATCH_DST       = 0x0040D268
CRYPTSP_GHP_RET_EA_ABS = 0x720D50D2
CRYPT_HASHDATA_NAME_CANDIDATES = ["cryptsp_CryptHashData", "cryptsp.dll_CryptHashData", "CryptHashData"]
HP_HASHVAL = 2
PATCH_BYTES = bytes.fromhex("F6 30 AA 38 D5 72 97 37 5D 64 55 59 C3 34 FD 50 D5 5C A1 D1 77 D2 65 5A 04 23 51 CF 69 24 4B F2")
g_hooks = None
g_fp = None
g_hash_seen_text = set()
g_last_dump16 = None
g_dump_count = 0

def _execute_sync(fn):
    try: ida_kernwin.execute_sync(fn, ida_kernwin.MFF_FAST)
    except Exception:
        try: fn()
        except Exception: pass

def _continue():
    try:
        ida_dbg.continue_process()
        return
    except Exception as e: print(f"[mix] continue failed: {e}")

def dbg_read_mem(ea, size):
    try:
        bs = idaapi.dbg_read_memory(ea, size)
        return bytes(bs) if bs else None
    except Exception: return None

def dbg_write_mem(ea, data: bytes):
    try:
        ok = idaapi.dbg_write_memory(ea, data)
        return bool(ok)
    except Exception: return False

def dbg_read_u32(ea):
    bs = dbg_read_mem(ea, 4)
    if not bs or len(bs) != 4: return None
    return struct.unpack("<I", bs)[0]

def dbg_write_u32(ea, v): return dbg_write_mem(ea, struct.pack("<I", v & 0xFFFFFFFF))

def is_in_text(pb, cb):
    if pb is None or cb is None: return False
    return (pb >= TEXT_BASE) and ((pb + cb) <= (TEXT_BASE + TEXT_LEN))

def find_module_base(substr: str):
    substr = substr.lower()
    try:
        m = ida_dbg.get_first_module()
        while m:
            name = (getattr(m, "name", "") or "").lower()
            if substr in name: return int(getattr(m, "base", 0))
            m = ida_dbg.get_next_module(m)
    except Exception: pass
    return None

def resolve_ea_by_names(names):
    for nm in names:
        ea = idc.get_name_ea_simple(nm)
        if ea != idc.BADADDR and ea != 0: return ea
    return None

def add_bpt(ea):
    try:
        idc.add_bpt(ea)
        idc.enable_bpt(ea, True)
        return True
    except Exception: pass
    try:
        ida_dbg.add_bpt(ea)
        return True
    except Exception: return False

def _dump_once(tid=None):
    global g_fp, g_last_dump16, g_dump_count
    if MAX_DUMPS and g_dump_count >= MAX_DUMPS: return
    try: p = ida_dbg.get_reg_val("EBX")
    except Exception as e:
        print(f"[dump] get_reg_val(EBX) failed: {e}")
        return
    if p is None or p < 0x10000: return
    data = dbg_read_mem(p, READ_SIZE)
    if data is None:
        print(f"[dump] failed to read {READ_SIZE} bytes @ EBX=0x{p:08X}")
        return
    if len(data) < READ_SIZE: data += b"\x00" * (READ_SIZE - len(data))
    if DEDUP_CONSECUTIVE and g_last_dump16 == data: return
    out = data + (b"\x00" * PAD_SIZE)
    try:
        g_fp.write(out)
        g_fp.flush()
        g_last_dump16 = data
        g_dump_count += 1
        print(f"[dump] #{g_dump_count} hit 0x{BREAK_EA:08X} | EBX=0x{p:08X} | wrote {len(out)} bytes")
    except Exception as e:
        print(f"[dump] file write error: {e}")

def _maybe_patch_hash():
    esp = ida_dbg.get_reg_val("ESP")
    hHash   = dbg_read_u32(esp + 4)
    dwParam = dbg_read_u32(esp + 8)
    pbOut   = dbg_read_u32(esp + 0xC)
    pdwLen  = dbg_read_u32(esp + 0x10)
    if dwParam != HP_HASHVAL: return
    if (hHash in g_hash_seen_text) and (pbOut == PATCH_DST):
        ok1 = dbg_write_mem(PATCH_DST, PATCH_BYTES)
        ok2 = True
        if pdwLen: ok2 = dbg_write_u32(pdwLen, 0x20)
        print(f"[patch] hHash=0x{hHash:08X} patched @0x{PATCH_DST:08X} bytes_ok={ok1} len_ok={ok2}")
    
class MixHooks(ida_dbg.DBG_Hooks):
    def __init__(self, crypt_hashdata_ea, crypt_gethashparam_ret_ea):
        super().__init__()
        self.crypt_hashdata_ea = crypt_hashdata_ea
        self.crypt_gethashparam_ret_ea = crypt_gethashparam_ret_ea

    def dbg_bpt(self, tid, ea):
        try:
            if ea == self.crypt_hashdata_ea:
                esp = ida_dbg.get_reg_val("ESP")
                hHash  = dbg_read_u32(esp + 4)
                pbData = dbg_read_u32(esp + 8)
                cbData = dbg_read_u32(esp + 0xC)

                if is_in_text(pbData, cbData):
                    g_hash_seen_text.add(hHash)
                    print(f"[hash] tid={tid} hHash=0x{hHash:08X} HASHED_TEXT pb=0x{pbData:08X} len=0x{cbData:X}")
                _execute_sync(_continue)
                return 0
            if ea == self.crypt_gethashparam_ret_ea:
                _maybe_patch_hash()
                _execute_sync(_continue)
                return 0
            if ea == BREAK_EA:
                _dump_once(tid=tid)
                if MAX_DUMPS and g_dump_count >= MAX_DUMPS:
                    print(f"[dump] reached MAX_DUMPS={MAX_DUMPS}, stopping hooks.")
                    stop()
                    return 0
                _execute_sync(_continue)
                return 0
            return 0
        except Exception as e:
            print(f"[mix] dbg_bpt exception: {e}")
            try: _execute_sync(_continue)
            except Exception: pass
            return 0

    def dbg_process_exit(self, pid, tid, ea, exit_code):
        print(f"[mix] process exit (code={exit_code}), stopping.")
        try: stop()
        except Exception: pass
        return 0

def start():
    global g_hooks, g_fp, g_hash_seen_text, g_last_dump16, g_dump_count
    if g_hooks is not None:
        print("[mix] already started")
        return
    g_hash_seen_text = set()
    g_last_dump16 = None
    g_dump_count = 0
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    g_fp = open(OUTPUT_PATH, "ab" if APPEND_OUTPUT else "wb")
    crypt_hashdata_ea = resolve_ea_by_names(CRYPT_HASHDATA_NAME_CANDIDATES)
    if crypt_hashdata_ea is None:
        print("[mix] ERROR: cannot resolve cryptsp_CryptHashData by name.")
        print("[mix] Please rename that function in IDA or hardcode its EA in the script.")
        return

    cryptsp_base = find_module_base("cryptsp")
    if cryptsp_base: crypt_gethashparam_ret_ea = cryptsp_base + 0x50D2
    else: crypt_gethashparam_ret_ea = CRYPTSP_GHP_RET_EA_ABS
    if not add_bpt(BREAK_EA):
        print(f"[mix] ERROR: add_bpt failed @ 0x{BREAK_EA:08X}")
        return
    if not add_bpt(crypt_hashdata_ea):
        print(f"[mix] ERROR: add_bpt failed @ 0x{crypt_hashdata_ea:08X} (CryptHashData)")
        return
    if not add_bpt(crypt_gethashparam_ret_ea):
        print(f"[mix] ERROR: add_bpt failed @ 0x{crypt_gethashparam_ret_ea:08X} (GetHashParam RET)")
        return
    g_hooks = MixHooks(crypt_hashdata_ea, crypt_gethashparam_ret_ea)
    g_hooks.hook()
    print("[mix] started.")
    print(f"  BREAK_EA                 = 0x{BREAK_EA:08X}")
    print(f"  CryptHashData EA         = 0x{crypt_hashdata_ea:08X}")
    print(f"  GetHashParam RET EA      = 0x{crypt_gethashparam_ret_ea:08X}")
    print(f"  TEXT                     = 0x{TEXT_BASE:08X}..0x{(TEXT_BASE+TEXT_LEN):08X}")
    print(f"  PATCH_DST                = 0x{PATCH_DST:08X}")
    print(f"  OUTPUT                   = {OUTPUT_PATH}")
    print(f"  DEDUP_CONSECUTIVE        = {DEDUP_CONSECUTIVE}")
    print(f"  MAX_DUMPS                = {MAX_DUMPS or 'unlimited'}")
    
def stop():
    global g_hooks, g_fp
    if g_hooks is not None:
        try: g_hooks.unhook()
        except Exception: pass
        g_hooks = None
    try: idc.enable_bpt(BREAK_EA, False)
    except Exception: pass
    if g_fp is not None:
        try:
            g_fp.flush()
            g_fp.close()
        except Exception: pass
        g_fp = None
    print("[mix] stopped.")

if __name__ == "__main__":
    start()
```

Set hardware breakpoint at 0x19F38C, and the shellcode firstly calls strlen and cmp the length with 19. However, setting hardware breakpoints can also cause subsequent instructions to malfunction after triggering the breakpoint. Parse dump.bin (not that if the operator is call, it will not appears at the first line of a block):

```python
# -*- coding: utf-8 -*-
import os
import ida_segment
import ida_bytes
import ida_ua
import idc
import idaapi

RECORD_SIZE   = 0x20
VALID_PREFIX  = 0x10
OUT_ASMCODE   = r"E:\CTF\temp\asmcode.txt"

def to_pat_hex(bs: bytes) -> str: return bs.hex()

def main():
    seg = ida_segment.getseg(0)
    start, end = seg.start_ea, seg.end_ea
    nrec = (end - start) // RECORD_SIZE
    os.makedirs(os.path.dirname(OUT_ASMCODE), exist_ok=True)
    out_lines = []
    for i in range(nrec):
        ea = start + i * RECORD_SIZE
        ida_bytes.del_items(ea, ida_bytes.DELIT_SIMPLE, VALID_PREFIX)
        insn1 = ida_ua.insn_t()
        ilen1 = ida_ua.decode_insn(insn1, ea)
        if ilen1 <= 0 or ilen1 > VALID_PREFIX: continue
        bytes1 = ida_bytes.get_bytes(ea, ilen1)
        if not bytes1: continue
        out_lines.append(to_pat_hex(bytes1))
        ea2 = ea + ilen1
        if ea2 < ea + VALID_PREFIX:
            insn2 = ida_ua.insn_t()
            ilen2 = ida_ua.decode_insn(insn2, ea2)
            if ilen2 > 0 and insn2.itype == idaapi.NN_call:
                bytes2 = ida_bytes.get_bytes(ea2, ilen2)
                if bytes2: out_lines.append(to_pat_hex(bytes2))  
    with open(OUT_ASMCODE, "w", encoding="utf-8") as f: 
        for s in out_lines: f.write(s + "\n")
    print(f"[ok] wrote {len(out_lines)} lines -> {OUT_ASMCODE}")

if __name__ == "__main__":
    main()
```

And then patch decrypted .pc to another Crackme.exe, get the disasm:

```python
import os
import idc
import ida_bytes
import ida_segment
import ida_ua

ASMCODE_PATH = r"E:\CTF\temp\asmcode.txt"
OUT_TRACEASM = r"E:\CTF\temp\traceasm.txt"
PC_START_EA  = 0x0040A000
WINDOW       = 0x20

def hex_to_bytes(hx: str) -> bytes:
    hx = hx.strip().lower()
    if len(hx) % 2 != 0: raise ValueError(f"bad hex len: {hx}")
    return bytes.fromhex(hx)

def bytes_to_patstr(bs: bytes) -> str: return " ".join(f"{b:02x}" for b in bs)

def get_pc_seg():
    for name in (".pc", "pc"):
        seg = ida_segment.get_segm_by_name(name)
        if seg: return seg
    return ida_segment.getseg(PC_START_EA)

def read_bytes(ea: int, n: int) -> bytes | None:
    bs = ida_bytes.get_bytes(ea, n)
    return bs if bs else None

pc_blob = 0
seg_start = 0
seg_end = 0

def find_in_range(start_ea: int, end_ea: int, needle: bytes) -> int:
    global pc_blob, seg_start, seg_end
    if not pc_blob:
        return idc.BADADDR
    s = max(start_ea, seg_start) - seg_start
    e = min(end_ea,   seg_end)   - seg_start
    if s < 0: s = 0
    if e < s: return idc.BADADDR
    idx = pc_blob.find(needle, s, e)
    return (seg_start + idx) if idx != -1 else idc.BADADDR


def decode_len_and_mnem(ea: int, fallback_len: int):
    idc.create_insn(ea)
    insn = ida_ua.insn_t()
    ilen = ida_ua.decode_insn(insn, ea)
    if ilen <= 0: ilen = fallback_len
    mnem = (idc.print_insn_mnem(ea) or "").lower()
    return ilen, mnem

def op_target(ea: int) -> int:
    try:
        v = idc.get_operand_value(ea, 0)
        return v if isinstance(v, int) else 0
    except Exception:
        return 0

def is_ret(mnem: str) -> bool: return mnem.startswith("ret")
def is_call(mnem: str) -> bool: return mnem == "call" or mnem == "callf"
def is_jmp(mnem: str) -> bool: return mnem == "jmp" or mnem == "jmpf"

def is_cond_jump_or_loop(mnem: str) -> bool:
    if mnem in ("jcxz", "jecxz", "loop", "loope", "loopz", "loopne", "loopnz"): return True
    return mnem.startswith("j") and not is_jmp(mnem)

def main():
    global pc_blob, seg_start, seg_end
    seg = get_pc_seg()
    seg_start, seg_end = seg.start_ea, seg.end_ea
    if not (seg_start <= PC_START_EA < seg_end): raise RuntimeError(f"PC_START_EA=0x{PC_START_EA:08X} not in 0x{seg_start:08X}-0x{seg_end:08X}")
    pc_blob = ida_bytes.get_bytes(seg_start, seg_end - seg_start) or b""
    codes_hex = []
    with open(ASMCODE_PATH, "r", encoding="utf-8") as f:
        for line in f:
            s = line.strip()
            if s: codes_hex.append(s)
    os.makedirs(os.path.dirname(OUT_TRACEASM), exist_ok=True)
    cursor = PC_START_EA
    alt_cursor = None
    call_stack = []
    last_occ = {}
    restart_guard = 0
    with open(OUT_TRACEASM, "w", encoding="utf-8") as out:
        i = 0
        while i < len(codes_hex):
            # print(i)
            if restart_guard > 20000:
                out.write("[fatal] too many restarts, stop.\n")
                break
            hx = codes_hex[i]
            bs = hex_to_bytes(hx)
            pat = bytes_to_patstr(bs)
            blen = len(bs)
            def try_match_from(pos: int) -> int:
                here = read_bytes(pos, blen)
                if here == bs: return pos
                win_end = min(pos + WINDOW, seg_end)
                return find_in_range(pos, win_end, bs)
            ea = try_match_from(cursor)
            if ea == idc.BADADDR and alt_cursor is not None:
                ea2 = try_match_from(alt_cursor)
                if ea2 != idc.BADADDR:
                    cursor = alt_cursor
                    alt_cursor = None
                    ea = ea2
            if ea == idc.BADADDR:
                if hx in last_occ: ea = last_occ[hx]
                else:
                    ea3 = find_in_range(PC_START_EA, seg_end, bs)
                    if ea3 == idc.BADADDR:
                        cursor = PC_START_EA
                        alt_cursor = None
                        call_stack.clear()
                        last_occ.clear()
                        i = 0
                        restart_guard += 1
                        continue
                    ea = ea3
            last_occ[hx] = ea
            ida_bytes.del_items(ea, ida_bytes.DELIT_SIMPLE, 16)
            idc.create_insn(ea)
            dis = idc.generate_disasm_line(ea, 0) or ""
            ilen, mnem = decode_len_and_mnem(ea, blen)
            out.write(f"{i:06d}\t0x{ea:08X}\t{hx}\t{dis}\n")
            next_fall = ea + ilen
            alt_cursor = None
            if is_call(mnem):
                ret_addr = ea + ilen
                tgt = op_target(ea)
                call_stack.append(ret_addr)
                if seg_start <= tgt < seg_end: cursor = tgt
                else: cursor = ret_addr
            elif is_ret(mnem):
                if call_stack: cursor = call_stack.pop()
                else: cursor = next_fall
            elif is_jmp(mnem):
                tgt = op_target(ea)
                if seg_start <= tgt < seg_end: cursor = tgt
                else: cursor = next_fall
            elif is_cond_jump_or_loop(mnem):
                tgt = op_target(ea)
                cursor = next_fall
                if seg_start <= tgt < seg_end: alt_cursor = tgt
            else: cursor = next_fall
            i += 1
    print(f"[ok] wrote -> {OUT_TRACEASM}")
    print(f"[info] pc_seg=0x{seg_start:08X}-0x{seg_end:08X}, start=0x{PC_START_EA:08X}")

if __name__ == "__main__":
    main()
```

Through the assembly code, it was found that shellcode uses [ebp-0x10] to [ebp-0x4] as temporary registers, add to dump:

```python
def _dump_once(tid=None):
    global g_fp, g_last_dump16, g_dump_count
    if MAX_DUMPS and g_dump_count >= MAX_DUMPS: return
    try: p = ida_dbg.get_reg_val("EBX")
    except Exception as e:
        print(f"[dump] get_reg_val(EBX) failed: {e}")
        return
    if p is None or p < 0x10000: return
    data = dbg_read_mem(p, READ_SIZE)
    if data is None:
        print(f"[dump] failed to read {READ_SIZE} bytes @ EBX=0x{p:08X}")
        return
    if len(data) < READ_SIZE: data += b"\x00" * (READ_SIZE - len(data))
    if DEDUP_CONSECUTIVE and g_last_dump16 == data: return
    out = data + (b"\x00" * PAD_SIZE)
    try:
        g_fp.write(out)
        g_fp.flush()
        g_last_dump16 = data
        g_dump_count += 1
        ebp_arr = [0]*4
        for i in range(4): ebp_arr[i] = read_dword_from_target(0x19F354+4*i)
        print(f"[dump] #{g_dump_count} hit 0x{BREAK_EA:08X} | EBX=0x{p:08X} | [EBP-0x10~EBP-0x4] = "+" ".join(f"0x{v:08X}" for v in ebp_arr))
    except Exception as e:
        print(f"[dump] file write error: {e}")
```

From the trace output, the 4 dwords are: `sum_diff`, `block_state`, `byte_processed` and `state_tag`. The `block_res` is initialized with 0xCAFEBABE, and updated every byte. When the number of processed byte reaches 4 (19 for the last one), the `block_state` will xor with the index of 5 dwords which load by `40A000`, and the result will or to `sum_diff`. Hardware breakpoint shows that the input will be load at `movzx   eax, byte ptr [ecx+eax]`, and the run amount of this instruction is just 19. And in the loop, [ebp+8] is the `block_state`, [ebp-4] is the input byte. Analyze the first part:

```assembly
0x0040A0AA      movzx eax, byte ptr [ecx+eax] ; inp[0]
0x0040A0AE      push eax
0x0040A0AF      push dword ptr [ebp-0Ch]
0x0040A0B2      mov ecx, [ebp-14h]
0x0040A0B5      call 0x40A1CE
0x0040A1CE      push ebp
0x0040A1CF      mov ebp, esp
0x0040A1D1      sub esp, 14h
0x0040A1D4      push ebx
0x0040A1D5      push esi
0x0040A1D6      push edi
0x0040A1D7      mov [ebp-14h], ecx
0x0040A1DA      movzx eax, byte ptr [ebp+0Ch]
0x0040A1DE      mov [ebp-4], eax
; jmp 0x0040A1E9
0x0040A1E9      mov eax, [ebp+8]
0x0040A1EC      and eax, 1 ; and block_state, 1
; if block_state & 1 == 0 {
    0x0040A1F5      cmp dword ptr [ebp-4], 40h ; num|let
	; if inp < 0x40 (num) {{
        0x0040A1FB      xor eax, eax
        0x0040A1FD      cmp eax, 6Ch ; 'l'
        0x0040A203      mov eax, [ebp-4]
        0x0040A206      and eax, 2 ; and inp, 2
		; if inp & 2 == 0 {{{
            0x0040A231      mov eax, [ebp-4]
            0x0040A234      shl eax, 4
            0x0040A237      mov ecx, [ebp+8]
            0x0040A23A      sub ecx, eax
            0x0040A23C      mov [ebp+8], ecx
            0x0040A23F      cmp dword ptr [ebp-4], 30h ; '0'
			; if inp == 0x30 {{{{
                0x0040A245      xor eax, eax
                0x0040A247      cmp eax, 79h ; 'y'
                0x0040A24D      mov eax, [ebp+8]
                0x0040A250      or eax, 0F0F0F0F0h
                0x0040A255      mov [ebp+8], eax
				; }}}} else (inp != 0x30) {{{{ (do nothing) }}}}.
			; }}} else (inp & 2 == 2) {{{
                0x0040A20B      xor eax, eax
                0x0040A20D      cmp eax, 0
                0x0040A213      mov eax, [ebp+8]
                0x0040A216      xor eax, 55AA55AAh
                0x0040A21B      mov [ebp+8], eax
                0x0040A21E      mov eax, [ebp+8]
                0x0040A221      add eax, [ebp-4]
                0x0040A224      mov [ebp+8], eax
            ; }}}.
    ; }} else (inp >= 0x40) (not num) {{
        0x0040A203      mov eax, [ebp-4]
        0x0040A25D      xor edx, edx
        0x0040A25F      push 2
        0x0040A261      pop ecx
        0x0040A262      div ecx
        0x0040A264      test edx, edx
        ; if inp % 2 == 0 {{{
            0x0040A26F      mov eax, [ebp+8]
            0x0040A272      shl eax, 3
            0x0040A275      mov ecx, [ebp+8]
            0x0040A278      shr ecx, 1Dh
            0x0040A27B      or eax, ecx
            0x0040A27D      mov [ebp+8], eax
            0x0040A280      mov eax, [ebp+8]
            0x0040A283      add eax, 12345678h
        ; }}} else (inp % 2 == 1) {{{
            0x0040A295      mov eax, [ebp+8]
            0x0040A298      shr eax, 5
            0x0040A29B      mov ecx, [ebp+8]
            0x0040A29E      shl ecx, 1Bh
            0x0040A2A1      or eax, ecx
            0x0040A2A3      mov [ebp+8], eax
            0x0040A2A6      mov eax, [ebp+8]
            0x0040A2A9      xor eax, 87654321h
        ; }}}.
    	0x0040A288      mov [ebp+8], eax
    ; }}.
; } else (state & 1 == 1) {
    0x0040A2BE      mov eax, [ebp+8]
    0x0040A2C1      xor eax, [ebp-4]
    0x0040A2C4      cmp eax, 80000000h
    ; if state & 0x80000000 == 0x80000000 {{
        0x0040A2D3      mov eax, [ebp+8]
        0x0040A2D6      sub eax, 21524111h
        0x0040A2DB      mov [ebp+8], eax
        0x0040A2DE      cmp dword ptr [ebp-4], 60h ; '`'
        ; if inp < 0x60 {{{ do nothing }}} else (inp >= 0x60) {{{
            0x0040A2EB      imul eax, [ebp-4], 21h ; '!'
            0x0040A2EF      xor eax, [ebp+8]
            0x0040A2F2      mov [ebp+8], eax
        ; }}}.
    ; }} else (state ^ inp & 0x80000000 == 0) {{
        0x0040A107      xor eax, eax
        0x0040A2F9      cmp eax, 9Dh
        0x0040A301      cmp dword ptr [ebp-4], 61h ; 'a'
		; if inp < 'a' {{{
			0x0040A318      cmp dword ptr [ebp-4], 41h ; 'A'
			; if inp >= 'A' {{{{
				0x0040A31E      cmp dword ptr [ebp-4], 5Ah ; 'Z'
				; if inp <= 'Z' {{{{{
                    0x0040A324      mov eax, 1
                    0x0040A329      inc eax
                    0x0040A32D      mov eax, [ebp+8]
                    0x0040A330      add eax, [ebp-4]
                    0x0040A333      mov [ebp+8], eax
                    0x0040A336      mov eax, [ebp+8]
                    0x0040A339      and eax, 100h
					; if state & 0x100 == 0 {{{{{{ (do nothing) }}}}}} else (state & 0x100 == 0x100) {{{{{{
						0x0040A340      xor eax, eax
                        0x0040A342      cmp eax, 0
                        0x0040A348      mov eax, [ebp+8]
                        0x0040A34B      xor eax, 13371337h
                        0x0040A350      mov [ebp+8], eax
                    ; }}}}}}.
				; }}}}} else (inp > 'Z') {{{{{
                    0x0040A107      xor eax, eax
                    0x0040A357      cmp eax, 0AEh
                    0x0040A35F      imul eax, [ebp+8], 9
                    0x0040A363      mov [ebp+8], eax
                ; }}}}}.
			; }}}} else (inp < 'A') {{{{
                ; the same as above inp > 'Z'.
            ; }}}}.
        ; }}} else (inp >= 'a') {{{
            0x0040A307      cmp dword ptr [ebp-4], 7Ah ; 'z'
            ; if inp <= 'z' {{{{
                0x0040A30D      mov eax, [ebp+8]
                0x0040A310      sub eax, [ebp-4]
                0x0040A313      mov [ebp+8], eax
            ; }}}} else (inp > 'z') {{{{
				; the same as above inp > 'Z'.
            ; }}}}.
        ; }}}.
    ; }}.
; }.
```

This part is a extremely complex function, as:

```python
def rol32(num, shift): return ((num << shift) | (num >> (0x20-shift))) & 0xFFFFFFFF
def ror32(num, shift): return ((num >> shift) | (num << (0x20-shift))) & 0xFFFFFFFF

def switch_case(inp, state):
    if state & 1 == 0:
        if inp < 0x40:
            if inp & 2 == 0:
                state = (state - (inp << 4)) & 0xFFFFFFFF
                if inp == 0x30: state |= 0xF0F0F0F0
            else: state = ((state ^ 0x55AA55AA) + inp) & 0xFFFFFFFF
        else:
            if inp % 2 == 0: state = (rol32(state, 3) + 0x12345678) & 0xFFFFFFFF
            else: state = (ror32(state, 5) ^ 0x87654321)
    else:
        if (state ^ inp) & 0x80000000 == 0:
            if ord("a") <= inp <= ord("z"): state = (state - inp) & 0xFFFFFFFF
            elif ord("A") <= inp <= ord("Z"):
                state = (state + inp) & 0xFFFFFFFF
                if state & 0x100 == 0x100: state ^= 0x13371337
            else: state = (state * 9) & 0xFFFFFFFF
        else:
            state = (state - 0x21524111) & 0xFFFFFFFF
            if 0x60 <= inp: state = state ^ ((inp * 0x21) & 0xFFFFFFFF)
    return state
```

And in the second part, the [ebp-0Ch] is the round index, and the [ebp-10h] is the round amount:

```assembly
0x0040A0F5      mov eax, 1
0x0040A0FA      inc eax
0x0040A231      mov eax, [ebp-4]
0x0040A25D      xor edx, edx
0x0040A374      push 5
0x0040A376      pop ecx
0x0040A377      div ecx ; inp % 5
0x0040A379      inc edx
0x0040A37A      inc edx ; inp % 5 + 2, this is the amount of round
0x0040A37B      mov [ebp-10h], edx
0x0040A37E      mov dword ptr [ebp-0Ch], 0
0x0040A387      mov eax, [ebp-0Ch]
0x0040A391      cmp eax, [ebp-10h]
; begin (inp % 5) + 2 rounds:
0x0040A39E      mov eax, [ebp+8]
0x0040A3A1      and eax, 80000000h
; if state & 0x80000000 == 0x80000000 {
0x0040A3AF      mov eax, [ebp+8]
0x0040A3B2      shl eax, 1
0x0040A3B4      xor eax, 4C11DB7h
; } else (state & 0x80000000 == 0) {
0x0040A3C6      mov eax, [ebp+8]
0x0040A3C9      shl eax, 1
;}. 
0x0040A3B9      mov [ebp+8], eax
0x0040A3CE      xor eax, eax
0x0040A3D0      cmp eax, 0C4h
0x0040A3D8      mov eax, [ebp-0Ch]
0x0040A3DB      and eax, 80000001h
0x0040A3E7      test eax, eax
; if eax == 0 {
    0x0040A3EB      xor eax, eax
    0x0040A3ED      cmp eax, 0
    0x0040A3F3      mov eax, [ebp+8]
    0x0040A3F6      xor eax, [ebp-4]
; } else (eax == 1) {
    0x0040A405      imul eax, [ebp-0Ch], 0Ah
    0x0040A409      add eax, [ebp+8]
; }.
0x0040A40C      mov [ebp+8], eax
0x0040A3D8      mov eax, [ebp-0Ch]
0x0040A3E6      inc eax ; round tag ++
0x0040A5AA      mov [ebp-0Ch], eax
0x0040A5AD      mov eax, [ebp-0Ch]
0x0040A391      cmp eax, [ebp-10h] ; cmp with round amount
; loop end
```

This is:

```python
def round_func(inp, state):
    round_num = inp % 5 + 2
    for i in range(round_num):
        tmp = state & 0x80000000
        state = (state << 1) & 0xFFFFFFFF
        if not tmp == 0: state ^= 0x4C11DB7
        if i & 1 == 0: state ^= inp
        else:  state = (state + (i * 10)) & 0xFFFFFFFF
    return state
```

Part 3:

```assembly
0x0040A39E      mov eax, [ebp+8]
0x0040A5A4      mov [ebp-8], eax
0x0040A5A7      mov eax, [ebp-8]
0x0040A41D      shr eax, 10h
0x0040A420      xor eax, [ebp-8]
0x0040A423      mov [ebp-8], eax
0x0040A426      mov eax, [ebp-8]
0x0040A429      shr eax, 8
0x0040A42C      xor eax, [ebp-8]
0x0040A42F      mov [ebp-8], eax
0x0040A43A      mov eax, [ebp-8]
0x0040A43D      and eax, 0Fh
0x0040A440      cmp eax, 7
; if eax >= 7 {
    0x0040A445      mov eax, 1
    0x0040A44A      inc eax
    0x0040A44E      mov eax, [ebp+8]
    0x0040A451      not eax
    0x0040A453      mov [ebp+8], eax
; } else (eax < 7) {
	0x0040A458      cmp dword ptr [ebp+8], 0
; }.
; dword ptr [ebp+8] != 0:
0x0040A46F      mov eax, [ebp+8]
0x0040A472      pop edi
0x0040A473      pop esi
0x0040A474      pop ebx
0x0040A475      leave
0x0040A476      retn 8
0x0040A5AA      mov [ebp-0Ch], eax; write to [ebp-0Ch]
```

In the end of this part, the value of [ebp-0Ch] is updated. This part is as:

```python
def tail_func(inp, state):
    tmp = (state >> 0x10) ^ state
    tmp1 = (tmp >> 8) ^ tmp
    if tmp1 & 0xF > 7: state = (~state) & 0xFFFFFFFF
    return state
```

Therefore, we can search for the middle vars during the function, and the result is: the addr of ebp during the function is at `0x19F328`. [ebp-8] is`0x19F320`, as the tail_func tmp1; [ebp+8] is`0x19F330`, as the state in func. Add them to the mix hook:

```python
def _dump_once(tid=None):
    global g_fp, g_last_dump16, g_dump_count
    if MAX_DUMPS and g_dump_count >= MAX_DUMPS: return
    try: p = ida_dbg.get_reg_val("EBX")
    except Exception as e:
        print(f"[dump] get_reg_val(EBX) failed: {e}")
        return
    if p is None or p < 0x10000: return
    data = dbg_read_mem(p, READ_SIZE)
    if data is None:
        print(f"[dump] failed to read {READ_SIZE} bytes @ EBX=0x{p:08X}")
        return
    if len(data) < READ_SIZE: data += b"\x00" * (READ_SIZE - len(data))
    if DEDUP_CONSECUTIVE and g_last_dump16 == data: return
    out = data + (b"\x00" * PAD_SIZE)
    try:
        g_fp.write(out)
        g_fp.flush()
        g_last_dump16 = data
        g_dump_count += 1
        ebp_arr = [0]*4
        for i in range(4): ebp_arr[i] = read_dword_from_target(0x19F354+4*i)
        state = read_dword_from_target(0x19F330)
        tail_num = read_dword_from_target(0x19F320) & 0xF
        print(f"[dump] #{g_dump_count} hit 0x{BREAK_EA:08X} | EBX=0x{p:08X} | [EBP-0x10~EBP-0x4] = "+" ".join(f"0x{v:08X}" for v in ebp_arr)+f" | state = 0x{state:08X} | tail_num = {tail_num}")
    except Exception as e:
        print(f"[dump] file write error: {e}")
```

The total function of these 3 part is:

```python
def byte_func(inp, state):
    state = switch_case(inp, state)
    state = round_func(inp, state)
    state = tail_func(inp, state)
    return state
```

After this part and before the next char is loaded:

```assembly
0x0040A426      mov eax, [ebp-8]
0x0040A435      inc eax ; add the amount of char processed
0x0040A42F      mov [ebp-8], eax
0x0040A0CB      mov eax, [ebp-8]
0x0040A0CE      and eax, 80000003h
0x0040A0DA      test eax, eax ; and with 3, the case of per 4 char an outer round
0x0040A0EF      cmp dword ptr [ebp-8], 13h ; cmp with end encryption
0x0040A107      xor eax, eax
0x0040A109      cmp eax, 0
0x0040A10F      mov dword ptr [ebp-4], 1
0x0040A06D      mov ecx, eax
0x0040A06F	    call 0x40A4B5
0x0040A4B5      push ebp
0x0040A4B6      mov ebp, esp
0x0040A4B8      sub esp, 38h
0x0040A4BB      mov [ebp-4], ecx
0x0040A4BE      mov eax, [ebp-4]
0x0040A4C1      mov eax, [eax+24h]
0x0040A4C4      mov [ebp-18h], eax
0x0040A4C7      mov eax, [ebp-4]
0x0040A4CA      mov eax, [eax+28h]
0x0040A4CD      mov [ebp-14h], eax
0x0040A4D0      mov dword ptr [ebp-8], 0
0x0040A4D7      mov ecx, [ebp-4]
0x0040A4DA	    call 0x40A479
0x0040A479      push ebp
0x0040A47A      mov ebp, esp
0x0040A47C      sub esp, 10h
0x0040A47F      mov [ebp-8], ecx
0x0040A482      mov byte ptr [ebp-1], 90h
0x0040A486      mov eax, [ebp-8]
0x0040A489      mov eax, [eax+24h]
0x0040A48C      mov ecx, [ebp-8]
0x0040A48F      mov ecx, [ecx+28h]
0x0040A492      lea eax, [eax+ecx-1]
0x0040A496      mov [ebp-10h], eax
0x0040A499      mov eax, [ebp-10h]
0x0040A49C      mov [ebp-0Ch], eax
0x0040A49F      mov eax, [ebp-0Ch]
0x0040A4A2      mov al, [eax]
0x0040A4A4      mov [ebp-2], al
0x0040A4A7      mov eax, [ebp-0Ch]
0x0040A4AA      mov cl, [ebp-1]
0x0040A4AD      mov [eax], cl
0x0040A4AF      movzx eax, byte ptr [ebp-2]
0x0040A4B3      leave
0x0040A4B4      retn
0x0040A4DF      mov [ebp-38h], al
0x0040A4E2      call sub_40113E ; hash funcs
0x0040A4E7      mov [ebp-10h], eax
0x0040A4EA      mov eax, [ebp-4]
0x0040A4ED      mov eax, [eax]
0x0040A4EF      mov [ebp-0Ch], eax
0x0040A4F2      lea eax, [ebp-8]
0x0040A4F5      push eax
0x0040A4F6      push 0
0x0040A4F8      push 0
0x0040A4FA      push 800Ch
0x0040A4FF      push dword ptr [ebp-0Ch]
0x0040A502      mov ecx, [ebp-10h]
0x0040A505      call sub_401022
0x0040A50A      test eax, eax
0x0040A513      mov [ebp-20h], eax
0x0040A516      mov eax, [ebp-8]
0x0040A519      mov [ebp-1Ch], eax
0x0040A51C      push 0
0x0040A51E      push dword ptr [ebp-14h]
0x0040A521      push dword ptr [ebp-18h]
0x0040A524      push dword ptr [ebp-1Ch]
0x0040A527      mov ecx, [ebp-20h]
0x0040A52A      call sub_40108E
0x0040A52F      test eax, eax
0x0040A533      mov dword ptr [ebp-24h], 20h ; ' '
0x0040A53A      call sub_40113E
0x0040A53F      mov [ebp-2Ch], eax
0x0040A542      mov eax, [ebp-8]
0x0040A545      mov [ebp-28h], eax
0x0040A548      push 0
0x0040A54A      lea eax, [ebp-24h]
0x0040A54D      push eax
0x0040A54E      mov eax, [ebp-4]
0x0040A551      add eax, 4
0x0040A554      push eax
0x0040A555      push 2
0x0040A557      push dword ptr [ebp-28h]
0x0040A55A      mov ecx, [ebp-2Ch]
0x0040A55D      call sub_40107C
0x0040A562      nop
0x0040A563      call sub_40113E
0x0040A568      mov [ebp-34h], eax
0x0040A56B      mov eax, [ebp-8]
0x0040A56E      mov [ebp-30h], eax
0x0040A571      push dword ptr [ebp-30h]
0x0040A574      mov ecx, [ebp-34h]
0x0040A577      call sub_401058
0x0040A57C      nop
0x0040A57D      push dword ptr [ebp-38h]
0x0040A580      mov ecx, [ebp-4]
0x0040A583      call 0x40A58B
0x0040A58B      push ebp
0x0040A58C      mov ebp, esp
0x0040A58E      sub esp, 0Ch
0x0040A591      mov [ebp-4], ecx
0x0040A594      mov eax, [ebp-4]
0x0040A597      mov eax, [eax+24h]
0x0040A59A      mov ecx, [ebp-4]
0x0040A59D      mov ecx, [ecx+28h]
0x0040A5A0      lea eax, [eax+ecx-1]
0x0040A5A4      mov [ebp-8], eax
0x0040A5A7      mov eax, [ebp-8]
0x0040A5AA      mov [ebp-0Ch], eax
0x0040A5AD      mov eax, [ebp-0Ch]
0x0040A5B0      mov cl, [ebp+8]
0x0040A5B3      mov [eax], cl
0x0040A5B5      leave
0x0040A5B6      retn 4
0x0040A588      nop
0x0040A589      leave
0x0040A58A      retn
0x0040A107      xor eax, eax
0x0040A109      cmp eax, 0
0x0040A07C      cmp dword ptr [ebp-4], 0
0x0040A096      cmp dword ptr [ebp-4], 1
0x0040A0A4      mov eax, [ebp-8]
0x0040A0A7      mov ecx, [ebp+8]
```

These codes are mainly used for hash verification and their existence can be ignored. And for every 4 byte after calculating:

```assembly
0x0040A124      cdq
0x0040A125      and edx, 3 ; index & 3
0x0040A128      add eax, edx
0x0040A12A      sar eax, 2
0x0040A12D      dec eax ; index//4 - 1
0x0040A12E      mov [ebp-18h], eax
0x0040A131      mov eax, [ebp-18h]
0x0040A134      mov ecx, [ebp-14h]
0x0040A137      mov eax, [ecx+eax*4] ; take the 5 dwords 0x40A000 loaded
0x0040A13A      mov [ebp-1Ch], eax
0x0040A13D      mov eax, 1
0x0040A142      inc eax
0x0040A146      mov eax, [ebp-0Ch]
0x0040A149      xor eax, [ebp-1Ch] ; xor with cipher[index]
0x0040A14C      mov ecx, [ebp-10h]
0x0040A14F      or ecx, eax
0x0040A151      mov [ebp-10h], ecx ; accumulate bit difference
0x0040A154      xor eax, eax
0x0040A156      cmp eax, 48h ; 'H'
0x0040A15C      imul eax, [ebp-8], 112233h
0x0040A163      sub eax, 35014542h ; change state to i*0x112233-0x35014542, infact 0xCAFEBABE is -0x35014542
0x0040A168      mov [ebp-0Ch], eax
0x0040A16B      mov dword ptr [ebp-4], 1
0x0040A06D      mov ecx, eax
...
```

Therefore the encrypt process is:

```python
sn = bytearray(b"1234567890123456789")
cip = []
for l in range(0, len(sn), 4):
    inp = sn[l:l+4]
    state = (l * 0x112233 - 0x35014542) & 0xFFFFFFFF
    for i in range(len(inp)): state = byte_func(inp[i], state)
    cip.append(state)

enc = [0x865DBB47, 0x0A6EB190, 0x20476C33, 0x1C8A7693, 0x59FEBDFB]
assert cip == enc
```

For each dword:

```python
# -*- coding: utf-8 -*-
from z3 import *

BV32 = lambda x: BitVecVal(x & 0xFFFFFFFF, 32)
BV8  = lambda x: BitVecVal(x & 0xFF, 8)
def zext8_to_32(b8): return ZeroExt(24, b8)
def rol32(x, sh): return RotateLeft(x, sh)
def ror32(x, sh): return RotateRight(x, sh)
def ULT32(a, b): return ULT(a, b)
def ULE32(a, b): return ULE(a, b)
def UGT32(a, b): return UGT(a, b)
def UGE32(a, b): return UGE(a, b)
def ULT8(a, b): return ULT(a, b)
def ULE8(a, b): return ULE(a, b)
def UGT8(a, b): return UGT(a, b)
def UGE8(a, b): return UGE(a, b)

def switch_case(inp8, state32):
    inp32 = zext8_to_32(inp8)
    c_state_lsb0 = (state32 & BV32(1)) == BV32(0)
    c_inp_lt_40 = ULT8(inp8, BV8(0x40))
    c_inp_bit1_0 = ((inp8 & BV8(2)) == BV8(0))
    s1 = (state32 - (inp32 << 4)) & BV32(0xFFFFFFFF)
    s1 = If(inp8 == BV8(0x30), s1 | BV32(0xF0F0F0F0), s1)
    s2 = ((state32 ^ BV32(0x55AA55AA)) + inp32) & BV32(0xFFFFFFFF)
    c_inp_even = ((inp8 & BV8(1)) == BV8(0))
    s3 = (rol32(state32, 3) + BV32(0x12345678)) & BV32(0xFFFFFFFF)
    s4 = (ror32(state32, 5) ^ BV32(0x87654321)) & BV32(0xFFFFFFFF)
    branch_state0 = If(c_inp_lt_40, If(c_inp_bit1_0, s1, s2), If(c_inp_even, s3, s4))
    c_topbit_same = (((state32 ^ inp32) & BV32(0x80000000)) == BV32(0))
    s5 = (state32 - inp32) & BV32(0xFFFFFFFF)
    s6 = (state32 + inp32) & BV32(0xFFFFFFFF)
    s6 = If((s6 & BV32(0x100)) == BV32(0x100), s6 ^ BV32(0x13371337), s6)
    s7 = (state32 * BV32(9)) & BV32(0xFFFFFFFF)
    branch_topbit0 = If(And(UGE8(inp8, BV8(ord("a"))), ULE8(inp8, BV8(ord("z")))), s5, If(And(UGE8(inp8, BV8(ord("A"))), ULE8(inp8, BV8(ord("Z")))), s6, s7))
    s8 = (state32 - BV32(0x21524111)) & BV32(0xFFFFFFFF)
    s8 = If(UGE8(inp8, BV8(0x60)), s8 ^ ((inp32 * BV32(0x21)) & BV32(0xFFFFFFFF)), s8)
    branch_state1 = If(c_topbit_same, branch_topbit0, s8)
    return If(c_state_lsb0, branch_state0, branch_state1)

def round_func(inp8, state32):
    inp32 = zext8_to_32(inp8)
    round_num = URem(inp32, BV32(5)) + BV32(2)
    def one_iter(st, i):
        tmp = st & BV32(0x80000000)
        st2 = (st << 1) & BV32(0xFFFFFFFF)
        st2 = If(tmp != BV32(0), st2 ^ BV32(0x04C11DB7), st2)
        if (i & 1) == 0: st2 = st2 ^ inp32
        else: st2 = (st2 + BV32(i * 10)) & BV32(0xFFFFFFFF)
        return st2
    st = state32
    for i in range(6):
        st_i = one_iter(st, i)
        st = If(ULT32(BV32(i), round_num), st_i, st)
    return st

def tail_func(inp8, state32):
    tmp  = LShR(state32, 16) ^ state32
    tmp1 = LShR(tmp, 8) ^ tmp
    cond = UGT32(tmp1 & BV32(0xF), BV32(7))
    return If(cond, (~state32) & BV32(0xFFFFFFFF), state32)

def byte_func(inp8, state32):
    st = switch_case(inp8, state32)
    st = round_func(inp8, st)
    st = tail_func(inp8, st)
    return st

def main():
    enc = [0x865DBB47, 0x0A6EB190, 0x20476C33, 0x1C8A7693, 0x59FEBDFB]
    sn = bytearray()
    for i in range(4):
        b0, b1, b2, b3 = BitVecs("b0 b1 b2 b3", 8)
        bs = [b0, b1, b2, b3]
        s = Solver()
        for b in bs: s.add(And(b >= 0x20, b <= 0x7e))
        state = BV32(0xCAFEBABE+i*4*0x112233)
        for b in bs: state = byte_func(b, state)
        target = BV32(enc[i])
        s.add(state == target)
        if s.check() == sat:
            m = s.model()
            sol = [m.eval(b).as_long() for b in bs]
            print(f"[{i}] bytes = {sol}  hex = {bytes(sol).hex()}  ascii = {bytes(sol)}")
            sn += bytearray(sol)
    b0, b1, b2 = BitVecs("b0 b1 b2", 8)
    bs = [b0, b1, b2]
    s = Solver()
    for b in bs: s.add(And(b >= 0x20, b <= 0x7e))
    state = BV32(0xCAFEBABE+16*0x112233)
    for b in bs: state = byte_func(b, state)
    target = BV32(enc[4])
    s.add(state == target)
    if s.check() == sat:
        m = s.model()
        sol = [m.eval(b).as_long() for b in bs]
        print(f"[4] bytes = {sol}  hex = {bytes(sol).hex()}  ascii = {bytes(sol)}")
        sn += bytearray(sol)
    print(sn.decode)

if __name__ == "__main__":
    main()
```

The input is `D3FE-A7ED-BAAD-C0D3`.

<font color=deepskyblue>**CMO{byp4ss3d_f4tm1k3_2o26}**</font>

