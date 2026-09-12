#include <stdio.h>
#include <stdlib.h>
#include <string.h>
size_t maxlen = 1000;
FILE* source;
FILE* from;
FILE* to;
FILE* dest;
char btw;
int end = 0;
char signs[100] = "{}[]/~!,.?:@#$%&*"; 
int access;
char* signbuf;
char* get_word(FILE* file)
{
    int limit = maxlen;
    char* word = (char*)calloc(maxlen, sizeof(char));
    char c = fgetc(file);
    word[0] = c;
    int l = 1;
    while(c != ' ' && c != '\n' && c != '\t' && c != EOF)
    {
        if(l >= limit-2)
        {
            limit *= 2;
            word = (char*)realloc(word, limit*sizeof(char));
        }
        c = fgetc(file);
        word[l] = c;
        l++;
    }
    if(file == source) btw = word[l-1];
    word[l-1] = '\0';
    if(c == EOF) end = 1;
    return word;
}
int correct(char* word);
int replace(char* word);
int main()
{
    access = strlen(signs);
    char* path1 = (char*)malloc(maxlen * sizeof(char));
    fgets(path1, maxlen, stdin);
    path1[strlen(path1)-1] = '\0';
    char* path2 = (char*)malloc(maxlen * sizeof(char));
    fgets(path2, maxlen, stdin);
    path2[strlen(path2)-1] = '\0';
    char* path3 = (char*)malloc(maxlen * sizeof(char));
    fgets(path3, maxlen, stdin);
    path3[strlen(path3)-1] = '\0';
    char* path4 = (char*)malloc(maxlen * sizeof(char));
    fgets(path4, maxlen, stdin);
    path4[strlen(path4)-1] = '\0';
    source = fopen(path1, "r");
    from = fopen(path2, "r");
    to = fopen(path3, "r");
    dest = fopen(path4, "w");
    char* buffer = get_word(source);
    while(end != 1)
    {
        correct(buffer);
        replace(buffer);
        fprintf(dest, "%s", signbuf);
        if(btw != EOF) fputc(btw, dest);
        free(signbuf);
        free(buffer);
        buffer = get_word(source);
        fclose(from);
        fclose(to);
        from = fopen(path2, "r");
        to = fopen(path3, "r");
    }
    if(buffer[strlen(buffer)] == EOF) buffer[strlen(buffer)] = '\0';
    correct(buffer);
    replace(buffer);
    fprintf(dest, "%s", signbuf);
    free(signbuf);
    free(buffer);
    end = 0;
    fclose(source);
    fclose(from);
    fclose(to);
    fclose(dest);
    free(path1);
    free(path2);
    free(path3);
    free(path4);
    return 0;
}
int correct(char* word)
{
    signbuf = (char*)calloc(maxlen, sizeof(char));
    int i = strlen(word)-1;
    int g = 0;
    int s = 0;
    for(i;i > 0;i--)
    {
        for(g = 0;g < access;g++)
        {
            if(word[i] == signs[g])
            {
                signbuf[s] = signs[g];
                s++;
                break;
            }
        }
        if(g == access)
        {
            word[i+1] = '\0';
            signbuf[s] = '\0';
            return 0;
        }
    }
}
int replace(char* word)
{
    int found = 0;
    char* buf1;
    while(end != 1)
    {
        buf1 = get_word(from);
        // printf("%s %s\n", word, buf1);
        if(strcmp(word, buf1) == 0)
        {
            free(buf1);
            char* buf2 = get_word(to);
            for(int i = 0;i < found;i++)
            {
                free(buf2);
                buf2 = get_word(to);
            }
            fprintf(dest, "%s", buf2);
            free(buf2);
            end = 0;
            return 0;
        }
        found++;
    }
    end = 0;
    free(buf1);
    fprintf(dest, "%s", word);
}