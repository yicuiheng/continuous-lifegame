#include <iostream>
#include <random>
#include <vector>
#include <GL/gl.h>
#include <GL/glut.h>

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;
constexpr int CHANNEL = 3;
std::vector<GLubyte> g_buffer(WIDTH *HEIGHT *CHANNEL);

std::random_device g_rand_seed_gen;
std::mt19937 rand_engine(g_rand_seed_gen());

void disp(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(-1, -1);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, g_buffer.data());
    glFlush();
}

GLubyte check_alive(int x, int y, std::vector<GLubyte> &buffer)
{
    int inner_radius = 2;
    int outer_radius = 4;

    int inner_count = 0;
    int inner_max = 0;
    int outer_count = 0;
    int outer_max = 0;
    for (int x_idx = x - outer_radius; x_idx <= x + outer_radius; x_idx++)
    {
        for (int y_idx = y - outer_radius; y_idx <= y + outer_radius; y_idx++)
        {
            if (x_idx < 0 || x_idx >= WIDTH)
                continue;
            if (y_idx < 0 || y_idx >= HEIGHT)
                continue;
            if (x_idx == x && y_idx == y)
                continue;

            int x_diff = x_idx - x;
            int y_diff = y_idx - y;

            if (x_diff * x_diff + y_diff * y_diff > outer_radius * outer_radius)
                continue;

            int idx = (y_idx * WIDTH + x_idx) * 3;
            if (x_diff * x_diff + y_diff * y_diff > inner_radius * inner_radius)
            {
                // neighbor
                outer_max += 256;
                outer_count += buffer[idx];
            }
            else
            {
                // self
                inner_max += 256;
                inner_count += buffer[idx];
            }
        }
    }
    if (inner_count < inner_max / 2 && 7 * outer_max / 16 < outer_count && outer_count < outer_max / 2)
        return (double)(inner_max - inner_count) / inner_max * 255;
    if (inner_count >= inner_max / 2 && 11 * outer_max / 24 < outer_count && outer_count < outer_max / 2)
        return (double)inner_count / inner_max * 255;

    return 0;
}

void fill(int x, int y, std::vector<GLubyte> &buffer)
{
    int radius = 4;
    for (int x_idx = x - radius; x_idx <= x + radius; x_idx++)
    {
        for (int y_idx = y - radius; y_idx <= y + radius; y_idx++)
        {
            if (x_idx < 0 || x_idx >= WIDTH)
                continue;
            if (y_idx < 0 || y_idx >= HEIGHT)
                continue;
            int x_diff = x - x_idx;
            int y_diff = y - y_idx;
            if (x_diff * x_diff + y_diff * y_diff > radius * radius)
                continue;

            int idx = (y_idx * WIDTH + x_idx) * 3;
            buffer[idx] += 4;
            buffer[idx + 1] += 4;
            buffer[idx + 2] += 4;
        }
    }
}

void timer(int value)
{
    if (value < 0)
    {

        std::vector<GLubyte> prev_buffer = g_buffer;
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                if (check_alive(x, y, prev_buffer))
                {
                    fill(x, y, g_buffer);
                }
            }
        }
    }
    glutTimerFunc(1000 / 60, timer, value - 1);
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);

    std::uniform_int_distribution<> dist(0, 1);

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            int value = dist(rand_engine) * 255;
            int index = (WIDTH * i + j) * 3;
            g_buffer[index] = value;
            g_buffer[index + 1] = value;
            g_buffer[index + 2] = value;
        }
    }

    glutCreateWindow("continuous lifegame");
    glutDisplayFunc(disp);
    glutTimerFunc(0, timer, 180);

    glutMainLoop();

    return 0;
}