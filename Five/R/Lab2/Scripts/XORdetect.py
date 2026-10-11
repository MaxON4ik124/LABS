import idautils
import idc
def find_XOR():
    print("XOR DETECTION REPORT: ")

    for segment in idautils.Segments():
        if idc.get_segm_name(segment) != ".text":
            continue

        address = idc.get_segm_start(segment)
        end = idc.get_segm_end(segment)

        while address < end:
            if idc.print_insn_mnem(address).lower() == "xor":
                op1 = idc.print_operand(address, 0)
                op2 = idc.print_operand(address, 1)
                if op1 != op2:
                    if "0x" in op2 or "[" in op2 or "[" in op1:
                        if "[" in op1 and "[" not in op2:
                            left, right = op2, op1
                        else:
                            right, left = op2, op1
                        print(f"XOR at {hex(address)}: {left} - {right};")
            address = idc.next_head(address)


