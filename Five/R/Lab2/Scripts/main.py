from ANTIDEBUGdetect import find_antiebug
from EXOTICdetect import find_exotic
from PEBdetect import find_PEB
from TIMEAPIdetect import find_timeAPI
from XORdecrypt import decryptXOR
from XORdetect import find_XOR
from TLSdetect import find_TLS
from STACKPROTdetect import find_STCKSEC

def main():
    decryptXOR()
    find_timeAPI()
    find_exotic()
    find_PEB()
    find_XOR()
    find_TLS()
    find_antiebug()
    find_STCKSEC()

main()