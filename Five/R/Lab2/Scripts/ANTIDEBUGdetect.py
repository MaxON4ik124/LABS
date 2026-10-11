import ida_segment
import idc

def find_antiebug():
    DEBUG_SIGNATURES = [
        "IsDebuggerPresent",
        "CheckRemoteDebuggerPresent",
        "NtQueryInformationProcess"
    ]
    print("DEBUG SIGNATURE DETECT REPORT:")
    for api in DEBUG_SIGNATURES:
        address = idc.get_name_ea_simple(api)
        if address != idc.BADADDR:
            print(f"DEBUG SIGNATURE {api} at {hex(address)}")


