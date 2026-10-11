import idautils
import idc

def find_PEB():
    print("PEB DETECTION REPORT:")

    for segment in idautils.Segments():
        if idc.get_segm_name(segment) != ".text": continue

        address = idc.get_segm_start(segment)
        end = idc.get_segm_end(segment)

        while address < end:
            disasmbled = idc.GetDisasm(address)

            if "fs:30h" in disasmbled or "gs:60h" in disasmbled:
                print(f"PEB at {hex(address)} -> {disasmbled}")

            address = idc.next_head(address)