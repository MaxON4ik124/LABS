import idautils
import idc
import ida_bytes




def decryptXOR():
    MIN_LEN = 4
    MAX_LEN = 64
    MAX_KEY = 0xFF
    is_ascii = lambda byte : 32 <= byte <= 126
    print("XOR DECRYPTION REPORT: ")
    for segment in idautils.Segments():
        segment_name = idc.get_segm_name(segment)

        if segment_name not in [".data", ".rdata", ".idata"]: continue

        start, end = idc.get_segm_start(segment), idc.get_segm_end(segment)

        addr = start

        while addr < end - MIN_LEN:
            detected = False
            for key in range(1, MAX_KEY+1):

                decoded = bytearray()

                for i in range(MAX_LEN):
                    b = ida_bytes.get_byte(addr + i)

                    if b is None: break

                    decoded_byte = b ^ key
                    if not is_ascii(decoded_byte):
                        break

                    decoded.append(decoded_byte)

                if len(decoded) >= MIN_LEN:
                    try:
                        string = decoded.decode("ascii")
                    except:
                        continue
                    print(f"XOR str {hex(addr)}: key={hex(key)} --> {string}")
                    addr += len(decoded)
                    detected = True
                    break

                if not detected:
                    addr += 1
            


