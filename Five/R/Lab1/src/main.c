#include "main.h"

char attempt[128];
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    if (width > 0 && height > 0) {
        glViewport(0, 0, width, height);
    }
}

int main(void) {
    if(checkPass(attempt))
        return -1;
    serial();
    static const unsigned char passfile[] = "\x40\x2a\x43\x14\x47\x25\x8\x35\x40\x0\x3b\x2c\x3a";
    encrypt(passfile, sizeof(passfile) - 1);
    FILE* password_file = fopen(buf, "r");
    memset(buf, 0, BUFSIZE);
    fscanf(password_file, "%127s", attempt);
    if (!CheckPass(attempt)) {
        return -1;
    }
    if(check_pass(attempt))
        return -1;
    static const unsigned char serfile[] = "\x43\x2e\x42\xe\x51\x26\x54\x25\x16\x0\x49";
    encrypt(serfile, sizeof(serfile) - 1);
    FILE* serial = fopen(buf, "w");
    memset(buf, 0, BUFSIZE);
    encrypt(SERIALKEY, sizeof(SERIALKEY) - 1);
    fputs(buf, serial);
    memset(buf, 0, BUFSIZE);
    char* crmes = "\x73\x24\x42\x15\x55\x29\xe\x71\x3e\x15\x30\x2b\x47\x25\x22\x35\x31\x2d\x4e";
    fprintf(stdout, "%s\n", buf);
    memset(buf, 0, BUFSIZE);

    



    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 0);

    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    glfwWindowHint(GLFW_RED_BITS, 5);
    glfwWindowHint(GLFW_GREEN_BITS, 6);
    glfwWindowHint(GLFW_BLUE_BITS, 5);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();


    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "2D TANKS", NULL, NULL);

    if (!window) {

        window = glfwCreateWindow(WIDTH, HEIGHT, "2D TANKS", monitor, NULL);
    }

    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    int fb_w = 0, fb_h = 0;
    glfwGetFramebufferSize(window, &fb_w, &fb_h);
    framebuffer_size_callback(window, fb_w, fb_h);

    glfwSwapInterval(1);

    srand((unsigned int)time(NULL));
    init_game();

    last_time = glfwGetTime();

    const double FIXED_DT = 1.0 / FPS;
    double accumulator = 0.0;
    
    FILE* textures = fopen("Assets\\patrol_town.txt", "r");
    FILE* models = fopen("Models\\tank.json", "r");

    if(textures != NULL)
    {
        if(models != NULL)
        {
            blank2();
        }
        else if(models == NULL)
        {
            while (!glfwWindowShouldClose(window)) 
            {
                double now = glfwGetTime();
                double frame_dt = now - last_time;
                last_time = now;
    
    
                if (frame_dt > 0.25) frame_dt = 0.25;
    
                accumulator += frame_dt;
                animation_time += frame_dt;
    
                glfwPollEvents();
    
                while (accumulator >= FIXED_DT) 
                {
                    delta_time = FIXED_DT;
                    process_input(window);
    
                    if (game_state == GAME_PLAYING)
                    {
                        update_game((float)FIXED_DT);
                    }
                    else if (game_state == GAME_LEVEL_TRANSITION) 
                    {
                        update_level_transition((float)FIXED_DT);
                    }
                    else if(game_state == GAME_RENDER)
                    {
                        blank1();
                        update_render((float)FIXED_DT);
                    }
    
                    accumulator -= FIXED_DT;
                }
                render();
                glfwSwapBuffers(window);
            }
            glfwTerminate();
            return 0;
        }

    }



   
}
