#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
// #include <GLFW/glfw3.h>

// Константы
#define WIDTH 800
#define HEIGHT 600
#define TANK_SIZE 30
#define BULLET_SIZE 5
#define TANK_SPEED 100.0f
#define BOT_SPEED 100.0f
#define ROTATE_SPEED 0.001f
#define BULLET_SPEED 1000.0f
#define MAX_BULLETS 5
#define MAP_WIDTH 20
#define MAP_HEIGHT 15
#define M_PI 3.14f

// Структура объекта танка
typedef struct {
    float x, y;
    float angle;
    int health;
    bool shooting;
    int cooldown;
} Tank;

// Структура объекта снаряда
typedef struct {
    float x, y;
    float dx, dy;
    bool active;
} Bullet;

// Карта с препятствиями (0 - пустое пространство, 1 - стена)
int map[MAP_HEIGHT][MAP_WIDTH] = { 0 };

// Глобальные переменные
Tank player;
Tank bot;
Bullet player_bullets[MAX_BULLETS];
Bullet bot_bullets[MAX_BULLETS];
bool keys[1024] = { 0 };
double last_time = 0;
double delta_time = 0;

// Инициализация игры
void init_game() {
    // Инициализация игрока
    player.x = 100;
    player.y = HEIGHT / 2;
    player.angle = 0;
    player.health = 3;
    player.shooting = false;
    player.cooldown = 0;

    // Инициализация бота
    bot.x = WIDTH - 100;
    bot.y = HEIGHT / 2;
    bot.angle = M_PI;
    bot.health = 3;
    bot.shooting = false;
    bot.cooldown = 0;

    // Инициализация снарядов
    for (int i = 0; i < MAX_BULLETS; i++) {
        player_bullets[i].active = false;
        bot_bullets[i].active = false;
    }

    // Создание стен на карте
    srand(time(NULL));
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) {
                map[y][x] = 1;  // Стены по периметру
            }
            else if (rand() % 10 == 0) {
                map[y][x] = 1;  // Случайные стены
            }
        }
    }

    // Убедимся, что начальные позиции танков свободны
    int px = player.x / (WIDTH / MAP_WIDTH);
    int py = player.y / (HEIGHT / MAP_HEIGHT);
    int bx = bot.x / (WIDTH / MAP_WIDTH);
    int by = bot.y / (HEIGHT / MAP_HEIGHT);

    if (px >= 0 && px < MAP_WIDTH && py >= 0 && py < MAP_HEIGHT)
        map[py][px] = 0;
    if (bx >= 0 && bx < MAP_WIDTH && by >= 0 && by < MAP_HEIGHT)
        map[by][bx] = 0;
}

// Обработка ввода
void process_input() {
    float dx = 0, dy = 0;

    if (keys[GLFW_KEY_W]) {
        dx += cos(player.angle) * TANK_SPEED;
        dy += sin(player.angle) * TANK_SPEED;
    }
    if (keys[GLFW_KEY_S]) {
        dx -= cos(player.angle) * TANK_SPEED;
        dy -= sin(player.angle) * TANK_SPEED;
    }
    if (keys[GLFW_KEY_A]) {
        player.angle -= ROTATE_SPEED;
    }
    if (keys[GLFW_KEY_D]) {
        player.angle += ROTATE_SPEED;
    }

    // Проверка столкновений перед движением
    float new_x = player.x + dx * delta_time;
    float new_y = player.y + dy * delta_time;

    int cell_x = new_x / (WIDTH / MAP_WIDTH);
    int cell_y = new_y / (HEIGHT / MAP_HEIGHT);

    if (cell_x >= 0 && cell_x < MAP_WIDTH && cell_y >= 0 && cell_y < MAP_HEIGHT) {
        if (map[cell_y][cell_x] == 0) {
            player.x = new_x;
            player.y = new_y;
        }
    }

    // Ограничение движения за пределы экрана
    if (player.x < TANK_SIZE / 2) player.x = TANK_SIZE / 2;
    if (player.x > WIDTH - TANK_SIZE / 2) player.x = WIDTH - TANK_SIZE / 2;
    if (player.y < TANK_SIZE / 2) player.y = TANK_SIZE / 2;
    if (player.y > HEIGHT - TANK_SIZE / 2) player.y = HEIGHT - TANK_SIZE / 2;

    // Стрельба
    if (keys[GLFW_KEY_SPACE] && player.cooldown <= 0) {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!player_bullets[i].active) {
                player_bullets[i].x = player.x + cos(player.angle) * TANK_SIZE;
                player_bullets[i].y = player.y + sin(player.angle) * TANK_SIZE;
                player_bullets[i].dx = cos(player.angle) * BULLET_SPEED;
                player_bullets[i].dy = sin(player.angle) * BULLET_SPEED;
                player_bullets[i].active = true;
                player.cooldown = 30;  // Задержка между выстрелами
                break;
            }
        }
    }

    // Уменьшение задержки между выстрелами
    if (player.cooldown > 0) {
        player.cooldown--;
    }
}

// ИИ для бота
void update_bot() {
    // Простой ИИ - бот преследует игрока
    float angle_to_player = atan2(player.y - bot.y, player.x - bot.x);
    bot.angle = angle_to_player;

    // Двигаемся к игроку, только если он на определенном расстоянии
    float distance = sqrt(pow(player.x - bot.x, 2) + pow(player.y - bot.y, 2));

    if (distance > TANK_SIZE * 5) {
        float dx = cos(bot.angle) * BOT_SPEED * delta_time;
        float dy = sin(bot.angle) * BOT_SPEED * delta_time;

        // Проверка столкновений
        int cell_x = (bot.x + dx) / (WIDTH / MAP_WIDTH);
        int cell_y = (bot.y + dy) / (HEIGHT / MAP_HEIGHT);

        if (cell_x >= 0 && cell_x < MAP_WIDTH && cell_y >= 0 && cell_y < MAP_HEIGHT) {
            if (map[cell_y][cell_x] == 0) {
                bot.x += dx;
                bot.y += dy;
            }
        }
    }

    // Бот стреляет, если видит игрока
    if (bot.cooldown <= 0) {
        // Проверяем линию видимости
        bool clear_line = true;
        float dx = (player.x - bot.x) / 20.0f;
        float dy = (player.y - bot.y) / 20.0f;
        float check_x = bot.x;
        float check_y = bot.y;

        for (int i = 0; i < 20; i++) {
            check_x += dx;
            check_y += dy;
            int cell_x = check_x / (WIDTH / MAP_WIDTH);
            int cell_y = check_y / (HEIGHT / MAP_HEIGHT);

            if (cell_x >= 0 && cell_x < MAP_WIDTH && cell_y >= 0 && cell_y < MAP_HEIGHT) {
                if (map[cell_y][cell_x] == 1) {
                    clear_line = false;
                    break;
                }
            }
        }

        if (clear_line && distance < WIDTH / 2) {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bot_bullets[i].active) {
                    bot_bullets[i].x = bot.x + cos(bot.angle) * TANK_SIZE;
                    bot_bullets[i].y = bot.y + sin(bot.angle) * TANK_SIZE;
                    bot_bullets[i].dx = cos(bot.angle) * BULLET_SPEED;
                    bot_bullets[i].dy = sin(bot.angle) * BULLET_SPEED;
                    bot_bullets[i].active = true;
                    bot.cooldown = 45;  // Немного больше задержка для бота
                    break;
                }
            }
        }
    }

    // Уменьшение задержки между выстрелами
    if (bot.cooldown > 0) {
        bot.cooldown--;
    }
}

// Обновление снарядов
void update_bullets() {
    // Обновление снарядов игрока
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (player_bullets[i].active) {
            player_bullets[i].x += player_bullets[i].dx * delta_time;
            player_bullets[i].y += player_bullets[i].dy * delta_time;

            // Столкновение со стенами
            int cell_x = player_bullets[i].x / (WIDTH / MAP_WIDTH);
            int cell_y = player_bullets[i].y / (HEIGHT / MAP_HEIGHT);

            if (cell_x < 0 || cell_x >= MAP_WIDTH || cell_y < 0 || cell_y >= MAP_HEIGHT ||
                map[cell_y][cell_x] == 1) {
                player_bullets[i].active = false;
                continue;
            }

            // Столкновение с ботом
            float dx = player_bullets[i].x - bot.x;
            float dy = player_bullets[i].y - bot.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < TANK_SIZE / 2) {
                player_bullets[i].active = false;
                bot.health--;
                if (bot.health <= 0) {
                    printf("Игрок победил!\n");
                    // Здесь можно добавить перезапуск игры
                    init_game();
                }
            }

            // Удаление снарядов, которые вышли за пределы экрана
            if (player_bullets[i].x < 0 || player_bullets[i].x > WIDTH ||
                player_bullets[i].y < 0 || player_bullets[i].y > HEIGHT) {
                player_bullets[i].active = false;
            }
        }
    }

    // Обновление снарядов бота
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bot_bullets[i].active) {
            bot_bullets[i].x += bot_bullets[i].dx * delta_time;
            bot_bullets[i].y += bot_bullets[i].dy * delta_time;

            // Столкновение со стенами
            int cell_x = bot_bullets[i].x / (WIDTH / MAP_WIDTH);
            int cell_y = bot_bullets[i].y / (HEIGHT / MAP_HEIGHT);

            if (cell_x < 0 || cell_x >= MAP_WIDTH || cell_y < 0 || cell_y >= MAP_HEIGHT ||
                map[cell_y][cell_x] == 1) {
                bot_bullets[i].active = false;
                continue;
            }

            // Столкновение с игроком
            float dx = bot_bullets[i].x - player.x;
            float dy = bot_bullets[i].y - player.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < TANK_SIZE / 2) {
                bot_bullets[i].active = false;
                player.health--;
                if (player.health <= 0) {
                    printf("Бот победил!\n");
                    // Здесь можно добавить перезапуск игры
                    init_game();
                }
            }

            // Удаление снарядов, которые вышли за пределы экрана
            if (bot_bullets[i].x < 0 || bot_bullets[i].x > WIDTH ||
                bot_bullets[i].y < 0 || bot_bullets[i].y > HEIGHT) {
                bot_bullets[i].active = false;
            }
        }
    }
}

// Отрисовка танка
void draw_tank(Tank tank, int is_player) {
    glPushMatrix();
    glTranslatef(tank.x, tank.y, 0);
    glRotatef(tank.angle * 180 / M_PI, 0, 0, 1);

    // Основа танка
    if (is_player) {
        glColor3f(0.2f, 0.6f, 0.2f);  // Зеленый для игрока
    }
    else {
        glColor3f(0.8f, 0.2f, 0.2f);  // Красный для бота
    }

    glBegin(GL_QUADS);
    glVertex2f(-TANK_SIZE / 2, -TANK_SIZE / 2);
    glVertex2f(TANK_SIZE / 2, -TANK_SIZE / 2);
    glVertex2f(TANK_SIZE / 2, TANK_SIZE / 2);
    glVertex2f(-TANK_SIZE / 2, TANK_SIZE / 2);
    glEnd();

    // Дуло танка
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(0, -TANK_SIZE / 8);
    glVertex2f(TANK_SIZE, -TANK_SIZE / 8);
    glVertex2f(TANK_SIZE, TANK_SIZE / 8);
    glVertex2f(0, TANK_SIZE / 8);
    glEnd();

    glPopMatrix();

    // Индикатор здоровья
    glColor3f(1.0f, 0.0f, 0.0f);
    float health_width = TANK_SIZE * (tank.health / 3.0f);

    glBegin(GL_QUADS);
    glVertex2f(tank.x - TANK_SIZE / 2, tank.y - TANK_SIZE / 2 - 10);
    glVertex2f(tank.x - TANK_SIZE / 2 + health_width, tank.y - TANK_SIZE / 2 - 10);
    glVertex2f(tank.x - TANK_SIZE / 2 + health_width, tank.y - TANK_SIZE / 2 - 5);
    glVertex2f(tank.x - TANK_SIZE / 2, tank.y - TANK_SIZE / 2 - 5);
    glEnd();
}

// Отрисовка снаряда
void draw_bullet(Bullet bullet) {
    glPushMatrix();
    glTranslatef(bullet.x, bullet.y, 0);

    glColor3f(1.0f, 1.0f, 0.0f);  // Желтый снаряд
    glBegin(GL_QUADS);
    glVertex2f(-BULLET_SIZE / 2, -BULLET_SIZE / 2);
    glVertex2f(BULLET_SIZE / 2, -BULLET_SIZE / 2);
    glVertex2f(BULLET_SIZE / 2, BULLET_SIZE / 2);
    glVertex2f(-BULLET_SIZE / 2, BULLET_SIZE / 2);
    glEnd();

    glPopMatrix();
}

// Отрисовка карты
void draw_map() {
    float cell_width = WIDTH / MAP_WIDTH;
    float cell_height = HEIGHT / MAP_HEIGHT;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == 1) {
                glColor3f(0.5f, 0.5f, 0.5f);  // Серый для стен
                glBegin(GL_QUADS);
                glVertex2f(x * cell_width, y * cell_height);
                glVertex2f((x + 1) * cell_width, y * cell_height);
                glVertex2f((x + 1) * cell_width, (y + 1) * cell_height);
                glVertex2f(x * cell_width, (y + 1) * cell_height);
                glEnd();
            }
        }
    }
}

// Отрисовка
void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Отрисовка карты
    draw_map();

    // Отрисовка снарядов
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (player_bullets[i].active) {
            draw_bullet(player_bullets[i]);
        }
        if (bot_bullets[i].active) {
            draw_bullet(bot_bullets[i]);
        }
    }

    // Отрисовка танков
    draw_tank(player, 1);
    draw_tank(bot, 0);
}

// Колбэк-функция для ввода клавиш
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Ошибка инициализации GLFW\n");
        return -1;
    }

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Танки 2D", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Ошибка создания окна\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // Настройка проекции OpenGL
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Инициализация игры
    init_game();

    // Основной игровой цикл
    while (!glfwWindowShouldClose(window)) {
        // Расчет delta time
        double current_time = glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        process_input();
        update_bot();
        update_bullets();
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}