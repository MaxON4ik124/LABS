#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void GoToLobby()
{
    while(1)
    {
        printf("Next action?\n1. add new ip\n2. Remove ip\n3. Show ip list\n4. Exit\n");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            printf("Something went wrong...\n");
            break;
        case 2:
            printf("Something went wrong...\n");
            break;
        case 3:
            printf("List of header's ip:\n1: 105.34.0.102\n2: 54.254.255.100\n3: 50.20.69.10\n");
            break;
        case 4:
            return;
            break;
        default:
            printf("Entered invalid choice!\n");
            break;
        }
    }
}
int main()
{
    int a = 5;
    char text[250];
    printf("Enter the password: ");
    scanf("%s", text);
    if(strcmp(text, "efuhefg[hufgeuh]") != 0) printf("Wrong password!");
    else GoToLobby();
}