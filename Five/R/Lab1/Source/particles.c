#include "main.h"


Particle particles[MAX_PARTICLES];
int next_particle = 0;


void update_particles(float dt) {
    serial();
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            
            particles[i].x += particles[i].dx * dt;
            particles[i].y += particles[i].dy * dt;

            
            particles[i].life -= dt;

            if (particles[i].life <= 0) {
                particles[i].active = false;
            }
        }
    }
}


void add_particles(float x, float y, float r, float g, float b, int count, float speed, float size, float lifetime) {
    for (int i = 0; i < count; i++) {
        int index = next_particle;
        next_particle = (next_particle + 1) % MAX_PARTICLES;

        float angle = (float)(rand() % 360) * M_PI / 180.0f;
        float velocity = speed * (0.5f + (float)rand() / RAND_MAX);

        particles[index].x = x;
        particles[index].y = y;
        particles[index].dx = cosf(angle) * velocity;
        particles[index].dy = sinf(angle) * velocity;
        particles[index].life = lifetime * (0.7f + 0.6f * (float)rand() / RAND_MAX);
        particles[index].max_life = lifetime;
        particles[index].size = size * (0.5f + (float)rand() / RAND_MAX);
        particles[index].r = r;
        particles[index].g = g;
        particles[index].b = b;
        particles[index].active = true;
    }
}


void create_explosion(float x, float y, float r, float g, float b, int count, float size) {
    add_particles(x, y, r, g, b, count, 150.0f, size, 1.0f);

    
    add_particles(x, y, 0.3f, 0.3f, 0.3f, count / 2, 80.0f, size, 1.5f);
}


void draw_particles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            float alpha = particles[i].life / particles[i].max_life;

            glPushMatrix();
            glTranslatef(particles[i].x, particles[i].y, 0);

            glColor4f(particles[i].r, particles[i].g, particles[i].b, alpha);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 0);
            for (int j = 0; j <= 360; j += 45) {
                float radian = j * M_PI / 180.0f;
                glVertex2f(cos(radian) * particles[i].size, sin(radian) * particles[i].size);
            }
            glEnd();

            glPopMatrix();
        }
    }

    glDisable(GL_BLEND);
}