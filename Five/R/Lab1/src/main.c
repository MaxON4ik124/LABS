#include "main.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    if (width > 0 && height > 0) {
        glViewport(0, 0, width, height);
    }
}

int main(void) {

    FILE* Password = fopen("password.txt", "w");
    char* attempt;
    fscanf(Password, "%s", attempt);
    if(strcmp(attempt, "gk3fld03g;d") != 0)
    {
        fprintf(stdout, "Wrong Password");
        return -1;
    }
    else
        fprintf(stdout, "Correct Password");

    



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

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double frame_dt = now - last_time;
        last_time = now;


        if (frame_dt > 0.25) frame_dt = 0.25;

        accumulator += frame_dt;
        animation_time += frame_dt;

        glfwPollEvents();

        while (accumulator >= FIXED_DT) {
            delta_time = FIXED_DT;
            process_input(window);

            if (game_state == GAME_PLAYING) {
                update_game((float)FIXED_DT);
            }
            else if (game_state == GAME_LEVEL_TRANSITION) {
                update_level_transition((float)FIXED_DT);
            }

            accumulator -= FIXED_DT;
        }

        render();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
