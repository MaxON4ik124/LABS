import ida_segment

def find_TLS():
    print("TLS DETECT REPORT:")
    tls = ida_segment.get_segm_by_name(".tls")
    if tls:
        print(f"TLS at {hex(tls.start_ea)} - {hex(tls.end_ea)}")
