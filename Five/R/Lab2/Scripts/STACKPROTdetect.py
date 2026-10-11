import idc

def find_STCKSEC():
    print("STACKSEC DETECT REPORT:")
    STACK_SEC = ["__security_cookie", "_security_cookie",]

    for symbol in STACK_SEC:
        address = idc.get_name_ea_simple(symbol)
        if address != idc.BADADDR:
            print(f"STACKSEC {symbol} at {hex(address)}")