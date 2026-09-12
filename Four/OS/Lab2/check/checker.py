# check = open("msg.txt", "r")
# etalon = open("expected_msg.txt", "r")
# check_data = check.readlines()
# etalon_data = etalon.readlines()
# for g in range(len(check_data)):
    # check_dataCut = check_data[g][check_data[g].index(" "):]
    # etalon_dataCut = etalon_data[g][etalon_data[g].index(" "):]
#     print("Test 1: " + check_dataCut == etalon_dataCut)
for i in range(1, 16):
    check = open(f"msg({i}).txt", "r")
    etalon = open(f"expected_msg({i}).txt", "r")
    check_data = check.readlines()
    etalon_data = etalon.readlines()
    print(f"Test {i}: {all(check_data[g][check_data[g].index(" "):] == etalon_data[g][etalon_data[g].index(" "):] for g in range(len(check_data)))}")
    for g in range(len(check_data)):
        check_dataCut = check_data[g][check_data[g].index(" "):]
        etalon_dataCut = etalon_data[g][etalon_data[g].index(" "):]
        if(check_dataCut != etalon_dataCut):
            print(f"{g+1}: Msg: {check_dataCut}")
            print(f"{g+1}: Etalon: {etalon_dataCut}")
            break