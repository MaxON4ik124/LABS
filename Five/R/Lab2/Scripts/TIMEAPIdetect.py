import idautils
import idc

def find_timeAPI():
    print("TIME API REPORT: ")

    TIME_APIS = [
        "GetTickCount",
        "GetTickCount64",
        "QueryPerformanceCounter",
        "timeGetTime",
    ]

    for segment in idautils.Segments():

        if idc.get_segm_name(segment) != ".text":
            continue

        address = idc.get_segm_start(segment)
        end = idc.get_segm_end(segment)

        while address < end:
            if idc.print_insn_mnem(address).lower() == "rdtsc":
                print(f"RDTSC at {hex(address)}")

            address = idc.next_head(address)

    for api in TIME_APIS:
        address = idc.get_name_ea_simple(api)
        if address != idc.BADADDR:
            print(f"TIME API {api} at {hex(address)}")

        