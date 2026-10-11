import idautils
import idc


EXOTIC_SIGNATURES = [
    "int",
    "icebp",
    "cpuid",
    "in",
    "out"
]
def find_exotic():
    print("EXOTIC INSTRUCTIONS REPORT: ")
    for segment in idautils.Segments():
        if idc.get_segm_name(segment) != ".text": continue

        address = idc.get_segm_start(segment)
        end = idc.get_segm_end(segment)

        while address < end:
            cur_mnemo = idc.print_insn_mnem(address).lower()
            disasm = idc.GetDisasm(address)

            if cur_mnemo in EXOTIC_SIGNATURES:
                if cur_mnemo == "int" and "2d" not in disasm:
                    address = idc.next_head(address)
                    continue
                
                print(f"{cur_mnemo.upper()} at {hex(address)}")
            address = idc.next_head(address)