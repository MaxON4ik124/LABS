#include <stdio.h>



int check_password(const char* password) {
    const char* correct_password = "JKnusnj2ui23dn";
    while (*password && *correct_password) {
        if (*password != *correct_password) {
            return 0; // Password does not match
        }
        password++;
        correct_password++;
    }
    return *password == *correct_password; // Check if both strings have reached the end
}

int main() {
    char input[50];
    printf("Enter password: ");
    scanf("%49s", input);

    if (check_password(input)) {
        printf("Code:{k3jgh3uigsjigvheiu}\n");
    } else {
        printf("Access denied!\n");
    }

    return 0;
}