#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE* file;
int id = 1;
char placee[] = "None";
int EventCnt = 0;
int end = 0;
int cnt = 0;
typedef struct Event
{
    int id;
    char* name;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    char* weekday;
    char* description;
    char* place;
    int importance;
} Event;
typedef struct TreeNode
{
    Event event;
    struct TreeNode* left;
    struct TreeNode* right;
    int height;
} TreeNode;
int sortByID(Event event1, Event event2) 
{ 
    if(event1.id == event2.id) return -1;
    else if(event1.id > event2.id) return 1;
    else if(event1.id < event2.id) return 0;
}
int sortByImportance(Event event1, Event event2) 
{
    if(event1.importance == event2.importance) return -1;
    else if(event1.importance > event2.importance) return 0;
    else if(event1.importance < event2.importance) return 1;
}
int sortByDate(Event event1, Event event2)
{
    if(event1.year < event2.year) return 0;
    else if(event1.year > event2.year) return 1;
    else
    {
        if(event1.month < event2.month) return 0;
        else if(event1.month > event2.month) return 1;
        else
        {
            if(event1.day < event2.day) return 0;
            else if(event1.day > event2.day) return 1;
            else
            {
                if(event1.hour < event2.hour) return 0;
                else if(event1.hour > event2.hour) return 1;
                else
                {
                    if(event1.minute < event2.minute) return 0;
                    else if(event1.minute == event2.minute) return -1;
                    else return 1;
                }
            }
        }
    }
}
void swap(TreeNode* a, TreeNode* b)
{
    Event data = a->event;
    a->event = b->event;
    b->event = data;
}
Event SearchNode(int id, TreeNode* root)
{
    if(root->event.id < id && root->right != NULL) return SearchNode(id, root->right);
    else if(root->event.id > id && root->left != NULL) return SearchNode(id, root->left);
    else if(root->event.id == id) return root->event;
}
void UpdateHeight(TreeNode* root)
{
    if(root->left != NULL && root->right != NULL)
    {
        if(root->left->height > root->right->height) root->height = root->left->height + 1;
        else root->height = root->right->height + 1;
    }
    if(root->left == NULL && root->right == NULL) root->height = 0;
    if(root->left == NULL && root->right != NULL) root->height = root->right->height + 1;
    if(root->left != NULL && root->right == NULL) root->height = root->left->height + 1;
}
int CheckOverload(TreeNode* root)
{
    int overload = 0;
    if(root->left == NULL && root->right != NULL) overload = root->right->height + 1;
    else if(root->right == NULL && root->left != NULL) overload = -(root->left->height) - 1;
    else if(root->right != NULL && root->left != NULL) overload = root->right->height - root->left->height;
    else if(root->right == NULL && root->left == NULL) overload = 0;
    return overload;
}
TreeNode* RightRotate(TreeNode* root)
{
    swap(root, root->left);
    TreeNode* temp = root->right;
    root->right = root->left;
    root->left = root->right->left;
    root->right->left = root->right->right;
    root->right->left = temp;
    UpdateHeight(root->right);
    UpdateHeight(root);
    return root;
}
TreeNode* LeftRotate(TreeNode* root)
{
    swap(root, root->right);
    TreeNode* temp = root->left;
    root->left = root->right;
    root->right = root->right->right;
    root->left->right = root->left->left;
    root->left->left = temp;
    UpdateHeight(root->left);
    UpdateHeight(root);
    return root;
}
TreeNode* BalanceNode(TreeNode* root)
{
    
    int overload = CheckOverload(root);
    if(overload == -2)
    {
        if(CheckOverload(root->left) == 1) root->left = LeftRotate(root->left);
        root = RightRotate(root);
    }
    else if(overload == 2)
    {
        if(CheckOverload(root->right) == -1) root->right = RightRotate(root->right);
        root = LeftRotate(root);
    }
    return root;
}
int FilterNone(Event event, char* place) {return 1;}
int FilterPlace(Event event, char* placee) 
{
    int res = (strcmp(event.place, placee) == 0 || (strstr(event.place, placee) != NULL));
    return res;

}
char* get_text(FILE* file)
{
    int buffer = 10;
    char* text = (char*)calloc(buffer, sizeof(char));
    char c;
    int i = 0;
    while(1)
    {
        c = fgetc(file);
        if(c == '\n') break;
        if(c == EOF)
        {
            end = 1;
            break;
        }
        if(i >= buffer-1)
        {
            buffer *= 2;
            text = (char*)realloc(text, buffer);
        }
        text[i] = c;
        i++;
    }
    text[i] = '\0';
    if(i == 0) text[0] = '\n';
    // if(file != stdin) printf("!%s", text);
    return text;
}
void extract_data(int *year,int *month,int *day,int *hour,int *minute, FILE* file)
{
    char* text = get_text(file);
    char* token = strtok(text, "._:");
    *day = atoi(token);
    token = strtok(NULL, "._:");
    *month = atoi(token);
    token = strtok(NULL, "._:");
    *year = atoi(token);
    token = strtok(NULL, "._:");
    *hour = atoi(token);
    token = strtok(NULL, "._:");
    *minute = atoi(token);
}
char Weekdays[9][100] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
int filter(int beg, int end, int num)
{
    if(num < beg || num > end) return 0;
    return 1;
}
int detect_leap(Event event)
{
    if(event.year % 400 == 0) return 1;
    if(event.year % 100 == 0) return 0;
    if(event.year % 4 == 0) return 1;
    return 0;
}
int predictWeekday(Event event)
{
    int monthCodesNL[13] = {1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};
    int monthCodesL[13] = {0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};
    int yearCode = (6 + event.year % 100 + ((event.year % 100) / 4)) % 7;
    int monthCode = monthCodesNL[event.month-1];
    int weekday = (event.day + monthCode + yearCode) % 7;
    return weekday;
}
Event createEvent(int id)
{
    Event newEvent;
    newEvent.id = id;
    printf("Enter the name of your new event: ");
    newEvent.name = get_text(stdin);
    int correct = 0;
    while(correct != 1)
    {
        printf("Enter date of your event in format DD.MM.YYYY_HH:mm (Y - year; M - month; D - day; H - hour; m - minute)\n");
        extract_data(&newEvent.year, &newEvent.month, &newEvent.day, &newEvent.hour, &newEvent.minute, stdin);
        if(filter(2025, 1000000, newEvent.year) && filter(1, 12, newEvent.month) && filter(0, 23, newEvent.hour) && filter(0, 59, newEvent.minute))
        {
            if((newEvent.month == 2) && (filter(1, 28, newEvent.day)) && (detect_leap(newEvent) == 0)) correct = 1;
            else if((newEvent.month == 2) && (filter(1, 29, newEvent.day)) && (detect_leap(newEvent) == 1)) correct = 1;
            else if((newEvent.month == 4 || newEvent.month == 6 || newEvent.month == 9 || newEvent.month == 11) && filter(1, 30, newEvent.day)) correct = 1;
            else if(filter(1, 31, newEvent.day) && (newEvent.month == 1 || newEvent.month == 3 || newEvent.month == 5 || newEvent.month == 7 || newEvent.month == 8 || newEvent.month == 10 || newEvent.month == 12)) correct = 1;
        }
        if(correct == 0) printf("Entered incorrect data!\n");
    }
    newEvent.weekday = Weekdays[predictWeekday(newEvent)];
    printf("Enter any description of the event(end entering description with ENTER): ");
    newEvent.description = get_text(stdin);
    printf("Enter place of your event(end entering with ENTER): ");
    newEvent.place = get_text(stdin);
    printf("Enter an importance of your event (0 - not important at all;10 - very very important): ");
    newEvent.importance = atoi(get_text(stdin));
    while(filter(1, 10, newEvent.importance) == 0)
    {
        printf("Enter PLEASE an importance of your event (0 - not important at all;10 - very very important): ");
        newEvent.importance = atoi(get_text(stdin));
    }
    printf("Event successfully recorded!\n");
    return newEvent;
}
void PrintEvent(Event event)
{
    printf("ID: %d\n", event.id);
    printf("Name: %s\n", event.name);
    printf("Date: ");
    if(event.month < 10) printf("0%d.", event.month);
    else printf("%d.", event.month);
    if(event.day < 10) printf("0%d_", event.day);
    else printf("%d_", event.day);
    printf("%d ", event.year);
    if(event.hour < 10) printf("0%d:", event.hour);
    else printf("%d:", event.hour);
    if(event.minute < 10) printf("0%d\n", event.minute);
    else printf("%d\n", event.minute);
    printf("Weekday: %s\n", event.weekday);
    printf("Description: %s\n", event.description);
    printf("Place: %s\n", event.place);
    printf("Importance: %d\n", event.importance);
}
void PrintTitleOfEvent(Event event)
{
    printf("%d\t", event.id);
    if(event.day < 10) printf("0%d_", event.day);
    else printf("%d_", event.day);
    if(event.month < 10) printf("0%d.", event.month);
    else printf("%d.", event.month);
    printf("%d ", event.year);
    if(event.hour < 10) printf("0%d:", event.hour);
    else printf("%d:", event.hour);
    if(event.minute < 10) printf("0%d\t", event.minute);
    else printf("%d\t", event.minute);
    printf("Name: %s\tPlace: %s\t", event.name, event.place);
    printf("Impotance: %d\n", event.importance);
}
TreeNode* CreateNode(Event _event)
{
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if(newNode == NULL)
    {
        printf("Memory ERROR!!!!404\n");
        exit(1);
    }
    newNode->event.id = _event.id;
    newNode->event.year = _event.year;
    newNode->event.month = _event.month;
    newNode->event.day = _event.day;
    newNode->event.hour = _event.hour;
    newNode->event.minute = _event.minute;
    newNode->event.importance = _event.importance;
    newNode->event.name = NULL;
    newNode->event.weekday = NULL;
    newNode->event.description = NULL;
    newNode->event.place = NULL;
    newNode->event.name = (char*)malloc((size_t)(strlen(_event.name) + 1));
    strcpy(newNode->event.name, _event.name);
    newNode->event.weekday = (char*)malloc((size_t)(strlen(_event.weekday) + 1));
    strcpy(newNode->event.weekday, _event.weekday);
    newNode->event.description = (char*)malloc((size_t)(strlen(_event.description) + 1));
    strcpy(newNode->event.description, _event.description);
    newNode->event.place = (char*)malloc((size_t)(strlen(_event.place) + 1));
    strcpy(newNode->event.place, _event.place);
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 0;
    return newNode;
}
TreeNode* InsertNode(TreeNode* root, Event event, int (*condition)(Event, Event))
{
    if(root == NULL)
    {
        root = CreateNode(event);
    }
    else if(condition(root->event, event) != 0)
    {
        root->left = InsertNode(root->left, event, condition);
    }
    else if(condition(root->event, event) == 0)
    {
        root->right = InsertNode(root->right, event, condition);
    }
    root = BalanceNode(root);
    UpdateHeight(root);
    return root;
}
void PrintTree(TreeNode* root, int (*_filter)(Event, char*), char* correctPlace)
{
    if(root != NULL)
    {
        PrintTree(root->left, _filter, correctPlace);
        if(_filter(root->event, correctPlace) == 1)
        { 
            // printf("(%d)\t", root->height);
            PrintTitleOfEvent(root->event);
        }
        PrintTree(root->right, _filter, correctPlace);
    }
    else if(EventCnt == 0) printf(" . . : There is nothing here    *chirping of crickets* . .  . : : ;\n");
}
TreeNode* FindMinNode(TreeNode* root)
{
    while(root->left != NULL) root = root->left;
    return root;
}
TreeNode* FindMaxNode(TreeNode* root)
{
    while(root->right != NULL) root = root->right;
    return root;
}
TreeNode* RemoveNode(TreeNode*root, Event event, int(*condition)(Event, Event))
{
    if(root == NULL)
    {
        printf("There are no events here!");
        return root;
    }
    else if(condition(root->event, event) == 1) root->left = RemoveNode(root->left, event, condition);
    else if(condition(root->event, event) == 0) root->right = RemoveNode(root->right, event, condition);
    else if(condition(root->event, event) == -1)
    {
        if(root->left == NULL && root->right == NULL)
        {
            root = NULL;
            return root;
        }
        else if(root->left == NULL && root->right != NULL)
        {
            root = root->right;
            UpdateHeight(root);
            return root;
        }
        else if(root->left != NULL && root->right == NULL)
        {
            root = root->left;
            UpdateHeight(root);
            return root;
        }
        else if(root->left != NULL && root->right != NULL)
        {
            TreeNode* temp1 = FindMaxNode(root->left);
            TreeNode* temp2 = FindMinNode(root->right);
            if(condition(temp1->event, temp2->event) == 1)
            {
                root = RemoveNode(root, temp1->event, condition);
                root->event = temp1->event;
                root = BalanceNode(root);
                UpdateHeight(root);
                return root;
            }
            if(condition(temp1->event, temp2->event) == 0)
            {
                root = RemoveNode(root, temp2->event, condition);
                root->event = temp2->event;
                root = BalanceNode(root);
                UpdateHeight(root);
                return root;
            }
        }
    }
}
TreeNode* InputFromFile(char* filepath, TreeNode* root, int (*cond)(Event, Event))
{
    file = fopen(filepath, "r");
    cnt = 0;
    while(end == 0)
    {
        Event curEvent;
        curEvent.id = id;
        id++;
        curEvent.name = get_text(file);
        extract_data(&curEvent.year, &curEvent.month, &curEvent.day, &curEvent.hour, &curEvent.minute, file);
        int correct = 0;
        if(filter(2025, 1000000, curEvent.year) && filter(1, 12, curEvent.month) && filter(0, 23, curEvent.hour) && filter(0, 59, curEvent.minute))
        {
            if((curEvent.month == 2) && (filter(1, 28, curEvent.day)) && (detect_leap(curEvent) == 0)) correct = 1;
            else if((curEvent.month == 2) && (filter(1, 29, curEvent.day)) && (detect_leap(curEvent) == 1)) correct = 1;
            else if((curEvent.month == 4 || curEvent.month == 6 || curEvent.month == 9 || curEvent.month == 11) && filter(1, 30, curEvent.day)) correct = 1;
            else if(filter(1, 31, curEvent.day) && (curEvent.month == 1 || curEvent.month == 3 || curEvent.month == 5 || curEvent.month == 7 || curEvent.month == 8 || curEvent.month == 10 || curEvent.month == 12)) correct = 1;
        }
        if(correct == 0)
        {
            printf("Invalid data in file!\n");
            break;
        }
        curEvent.description = get_text(file);
        curEvent.place = get_text(file);
        curEvent.importance = atoi(get_text(file));
        if(filter(1, 10, curEvent.importance) == 0)
        {
            printf("Invalid data in file!\n");
            break;
        }
        char* separator = get_text(file);
        free(separator);
        curEvent.weekday = Weekdays[predictWeekday(curEvent)];
        root = InsertNode(root, curEvent, cond);
        cnt++;
    }
    id -= cnt;
    end = 0;
    fclose(file);
    return root;
}
void ExportEvent(FILE* file, TreeNode* root)
{
    if(root != NULL)
    {
        ExportEvent(file, root->left);
        fprintf(file, "%s\n", root->event.name);
        fprintf(file, "%d.%d.%d_%d:%d\n", root->event.day, root->event.month, root->event.year, root->event.hour, root->event.minute);
        fprintf(file, "%s\n", root->event.weekday);
        fprintf(file, "%s\n", root->event.description);
        fprintf(file, "%s\n", root->event.place);
        fprintf(file, "%d\n\n", root->event.importance);
        ExportEvent(file, root->right);
    }
}
void ExportEvents(char* filepath, TreeNode* root)
{
    file = fopen(filepath, "w");
    ExportEvent(file, root);
    fclose(file);
}
void main()
{
    TreeNode* root = NULL;
    TreeNode* rootD = NULL;
    TreeNode* rootI = NULL;
    int choice = 0;
    int mode = 0;
    while(1)
    {
        if(mode == 0)
        {
            printf("Current action?\n");
            printf("1. Add event\n");
            printf("2. Delete event\n");
            printf("3. Show list of events\n");
            printf("4. Import events from file\n");
            printf("5. Export events to file\n");
            printf("6. Exit\n");
            choice = atoi(get_text(stdin));
            switch(choice)
            {
                case 1:
                {
                    Event Nevent = createEvent(id);
                    root = InsertNode(root, Nevent, sortByID);
                    rootD = InsertNode(rootD, Nevent,  sortByDate);
                    rootI = InsertNode(rootI, Nevent, sortByImportance);
                    id++;
                    EventCnt++;
                    break;
                }
                case 2:
                {
                    printf("=======================================THERE ARE %d EVENTS=======================================\n", EventCnt);
                    PrintTree(root, FilterNone, placee);
                    mode = 2;
                    break;
                }
                case 3:
                {
                    printf("=======================================THERE ARE %d EVENTS=======================================\n", EventCnt);
                    PrintTree(root, FilterNone, placee);
                    if(EventCnt != 0) mode = 1;
                    break;
                }
                case 4:
                {
                    printf("Enter import file path please\n ");
                    printf("!!!ATENTION!!!\nEvent in file must be in this format:\n{Name}\n{DD.MM.YYYY_HH:mm} D - day;M - month;Y - year;H - hour;m - minute\n{Description}\n{Place}\n{Importance}\n*separator between events - empty string*\n");
                    char* inputPath = get_text(stdin);
                    root = InputFromFile(inputPath, root, sortByID);
                    rootD = InputFromFile(inputPath, rootD, sortByDate);
                    rootI = InputFromFile(inputPath, rootI, sortByImportance);
                    id += cnt;
                    EventCnt += cnt;
                    printf("Events successfully imported\n");
                    cnt = 0;
                    break;
                }
                case 5:
                {
                    printf("Enter export file path please\n");
                    char* ExportPath = get_text(stdin);
                    ExportEvents(ExportPath, root);
                    printf("Events successfully exported\n");
                    break;
                }
                case 6:{
                    exit(0);}
                default: {printf("You entered a bullshit\n");}
            }
        }
        else if(mode == 1)
        {
            printf("Next action?\n");
            printf("1. Show full info about event\n");
            printf("2. Sort events by date\n");
            printf("3. Sort events by importance\n");
            printf("4. Filter events by place\n");
            printf("5. Return back\n");
            choice = atoi(get_text(stdin));
            switch (choice)
            {
            case 1:
            {
                int eventid = 0;
                printf("Enter ID of event, which you want to view: ");
                eventid = atoi(get_text(stdin));
                while(eventid > id && eventid < 1)
                {
                    printf(". . . : : : . .There is nothing here  *chirping of crickets*. . . . : : . . \n");
                    printf("Enter ID of event, which you want to view: ");
                    eventid = atoi(get_text(stdin));
                }
                Event viewEvent = SearchNode(eventid, root);
                PrintEvent(viewEvent);
                mode = 0;
                break;
            }
            case 2:
            {
                printf("Events successfully sorted by date\n");
                printf("=======================================THERE ARE %d EVENTS=======================================\n", EventCnt);
                PrintTree(rootD, FilterNone, placee);
                break;
            }
            case 3:
            {
                printf("Events successfully sorted by importance\n");
                printf("=======================================THERE ARE %d EVENTS=======================================\n", EventCnt);
                PrintTree(rootI, FilterNone, placee);
                mode = 0;
                break;
            }
            case 4:
            {
                printf("Enter the place by which filtering will occur:\n");
                char* place = get_text(stdin);
                printf("=======================================THERE ARE %d EVENTS=======================================\n", EventCnt);
                PrintTree(root, FilterPlace, place);
                mode = 0;
                break;
            }
            case 5:
            {
                mode = 0;
                break;
            }
            default:
            {
                printf("You entered a bullshit!\n");
                break;
            }
            }
        }
        else if(mode == 2)
        {
            printf("Which event you want to delete? (Enter ID of this event)\n");
            choice = atoi(get_text(stdin));
            while(choice < 1 && choice > id)
            {
                printf("This event doesn't exist!\n");
                printf("Which event you want to delete? (Enter ID of this event)\n");
                choice = atoi(get_text(stdin));
            }
            Event eventdel = SearchNode(choice, root);
            root = RemoveNode(root, eventdel, sortByID);
            rootD = RemoveNode(rootD, eventdel, sortByDate);
            rootI = RemoveNode(rootI, eventdel, sortByImportance);
            printf("Event successfully deleted!\n");
            EventCnt--;
            mode = 0;
        }
    }
}