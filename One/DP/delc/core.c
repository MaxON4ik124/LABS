#include <stdio.h>
#include <string.h>
int n;
char inp_files[100][1000];
char out_files[100][1000];
void create_outputs(int n)
{
    for(int i = 0;i < n;i++)
        strcpy(out_files[i], inp_files[i]);
    for(int i = 0;i < n;i++)
    {
        int ln = strlen(out_files[i]);
        if(i == n - 1)
            ln++;
        else
            inp_files[i][ln-1] = '\0';
        out_files[i][ln-2] = 'w';
        out_files[i][ln-1] = 'c';
        out_files[i][ln] = '\0';
    }
}
int delete_comments(char input[], char output[])
{
    FILE *inp = fopen(input, "r");
    FILE *out = fopen(output, "w");
    int del_alarm = 0;
    int save_alarm1 = 0;
    int save_alarm2 = 0;
    int current;
    int past;
    while(1)
    {
        current = fgetc(inp);
        if(current == '"' && del_alarm == 0 && save_alarm2 == 0)
        {
            if(save_alarm1 == 0)
                save_alarm1 = 1;
            else if(save_alarm1 == 1 && past != '\\')
                save_alarm1 = 0;
            fputc(current, out);
            past = current;
            continue;
        }
        if(current == '\'' && del_alarm == 0 && save_alarm1 == 0)
        {
            if(save_alarm2 == 0)
                save_alarm2 = 1;
            else if(save_alarm2 == 1 && past != '\\')
                save_alarm2 = 0;
            fputc(current, out);
            past = current;
            continue;
        }
        if(save_alarm1 == 0 && save_alarm2 == 0)
        {
            if(past == '/' && current == '/')
            {
                del_alarm = 1;
                while(del_alarm)
                {
                    past = current;
                    while((current = fgetc(inp)) != '\\' && current != '\n')
                    {
                        if(current == EOF)
                            break;
                    }
                    if(current == '\n')
                        del_alarm = 0;
                    if(current == '\\')
                    {
                        if((current = fgetc(inp)) == '\n')
                            continue;
                    }
                    if(current == EOF)
                        break;
                    continue;
                }
            }
            else if(current == '*' && past == '/')
            {
                del_alarm = 1;
                while(del_alarm)
                {
                    current = fgetc(inp);
                    if(current == EOF)
                        break;
                    if(current == '/' && past == '*')
                        del_alarm = 0;
                    past = current;
                }
            }
        }
        if(current == '/' && save_alarm1 == 0 && save_alarm2 == 0)
        {
            past = current;
            continue;
        }
        if(del_alarm == 0 && current != EOF)
            fputc(current, out);
        past = current;
        if(current == EOF)
            break;
    }
    fclose(inp);
    fclose(out);
    return 0;
}
int main()
{
    FILE *main = fopen("project.txt", "r");
    fscanf(main, "%d\n", &n);
    for(int i = 0;i < n;i++)
        fgets(inp_files[i], 1000, main);
    create_outputs(n);
    for(int i = 0;i < n;i++)
        delete_comments(inp_files[i], out_files[i]);
}