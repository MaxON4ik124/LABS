    // FILE* file = fopen("line.txt", "r");
    // char file_name[128];
    // char* fn_copy = (char*)malloc(128 * sizeof(char));
    // fgets(file_name, 128, file);
    // strncpy(fn_copy, file_name, strlen(file_name));
    // file_name[strcspn(file_name, "\n")] = '\0';
    // int argc = 0;
    // char* argv[4];
    // char* token = strtok(file_name, " ");
    // int i = 0;
    // while(token != NULL)
    // {
    //     argv[argc++] = token;
    //     printf("argv[%d]: %s, addr: %p\n", i, argv[i], (void*)argv[i]);
    //     i++;
    //     token = strtok(NULL, " ");
    // }
    // argv[argc] = NULL;
    // char stack_buf[128];
    // char *sp = stack_buf + 128;
    // void** esp = malloc(64 * sizeof(void**));
    // int cnt = argc-1;
    // i = 0;
    // while(cnt >= 0)
    // {
    //     // *esp = argv[cnt];
    //     strncpy(*esp, argv[cnt], strlen(argv[cnt])+1);
    //     printf("| ESP: %s |\n", (char*)*esp);
    //     esp--;
    //     cnt--;
    // }
    // cnt = argc;
    // while(cnt >= 0)
    // {
    //     *esp = &argv[cnt];
    //     printf("| ESP: %s |\n", *((char**)*esp));
    //     esp--;
    //     cnt--;
    // }
    // esp--;
    // *esp = (void*)argc;
    // printf("| ESP: %d |\n", *esp);
    // esp--;
    // memset(esp, 0, 8);
    // printf("| ESP: %p |\n", *esp);