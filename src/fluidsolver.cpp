#include "fluidsolver.h"

unsigned int getIndex(unsigned int i, unsigned int j)
{
    return i + (N + 2) * j;
}

float linInterp(float a, float b, float t)
{
    // linear interpolation between numbers a and b assuming t between 0 and 1
    return a + (b - a) * t;
}

float bilinInterp(float b_left, float t_left, float b_right, float t_right, float t_x, float t_y)
{
    float a = linInterp(b_left, t_left, t_y);
    float b = linInterp(b_right, t_right, t_y);
    return linInterp(a, b, t_x);
}

void addSource(unsigned int x, unsigned int y, std::vector<float>& v, float amount)
{
    if (x < 1 || x > N || y < 1 || y > N)
        return;
    v[getIndex(x, y)] += amount;
}

void diffuse(unsigned int N, int b, std::vector<float>& x, std::vector<float>& x0, float diff, float dt)
{
    float a = dt * diff * N * N;
    for (unsigned int k = 0; k < 20; k++) 
    {
        for (unsigned int i = 1; i <= N; i++) 
        {
            for (unsigned int j = 1; j <= N; j++) 
            {
                x[getIndex(i, j)] = (x0[getIndex(i, j)] + a * (x[getIndex(i + 1, j)] + x[getIndex(i - 1, j)] +
                    x[getIndex(i, j - 1)] + x[getIndex(i, j + 1)])) / (1.0f + 4.0f * a);
            }
        }
        setBoundary(N, b, x);
    }
}

void advect(unsigned int N, int b, std::vector<float>& a, std::vector<float>& a0, std::vector<float>& u, std::vector<float>& v, float dt)
{
    for (unsigned int i = 1; i <= N; i++)
    {
        for (unsigned int j = 1; j <= N; j++)
        {
            float x = i - dt * N * u[getIndex(i, j)];
            float y = j - dt * N * v[getIndex(i, j)];
            if (x < 0.5f) x = 0.5f;
            if (x > N + 0.5f) x = N + 0.5f;
            if (y < 0.5f) y = 0.5f;
            if (y > N + 0.5f) y = N + 0.5f;
            int x_int = (int)x;         // integer part of x
            int y_int = (int)y;         // integer part of y
            float x_frac = x - (int)x;  // fractional part of x
            float y_frac = y - (int)y;  // fractional part of y
            a[getIndex(i, j)] = bilinInterp(a0[getIndex(x_int, y_int)], a0[getIndex(x_int, y_int + 1)],
                a0[getIndex(x_int + 1, y_int)], a0[getIndex(x_int + 1, y_int + 1)], x_frac, y_frac);
        }
    }
    setBoundary(N, b, a);
}

void setBoundary(unsigned int N, int b, std::vector<float>& x)
{
    // b = 0 (scalar field), b = 1 (u velocity component), b = 2 (v velocity component)
    for (unsigned int i = 1; i <= N; i++)
    {
        x[getIndex(0, i)] = b == 1 ? -x[getIndex(1, i)] : x[getIndex(1, i)];
        x[getIndex(N + 1, i)] = b == 1 ? -x[getIndex(N, i)] : x[getIndex(N, i)];
        x[getIndex(i, 0)] = b == 2 ? -x[getIndex(i, 1)] : x[getIndex(i, 1)];
        x[getIndex(i, N + 1)] = b == 2 ? -x[getIndex(i, N)] : x[getIndex(i, N)];
    }
    x[getIndex(0, 0)] = 0.5f * (x[getIndex(1, 0)] + x[getIndex(0, 1)]);
    x[getIndex(0, N + 1)] = 0.5f * (x[getIndex(1, N + 1)] + x[getIndex(0, N)]);
    x[getIndex(N + 1, 0)] = 0.5f * (x[getIndex(N, 0)] + x[getIndex(N + 1, 1)]);
    x[getIndex(N + 1, N + 1)] = 0.5f * (x[getIndex(N, N + 1)] + x[getIndex(N + 1, N)]);
}

void project(unsigned int N, std::vector<float>& u, std::vector<float>& v, std::vector<float>& p, std::vector<float>& div)
{
    float h = 1.0f / N; // grid spacing

    for (unsigned int i = 1; i <= N; i++) 
    {
        for (unsigned int j = 1; j <= N; j++) 
        {
            div[getIndex(i, j)] = -0.5f * h * (u[getIndex(i + 1, j)] - u[getIndex(i - 1, j)] +
                v[getIndex(i, j + 1)] - v[getIndex(i, j - 1)]);
            p[getIndex(i, j)] = 0;
        }
    }

    setBoundary(N, 0, div);
    setBoundary(N, 0, p);

    for (unsigned int k = 0; k < 20; k++) 
    {
        for (unsigned int i = 1; i <= N; i++) 
        {
            for (unsigned int j = 1; j <= N; j++) 
            {
                p[getIndex(i, j)] = 0.25f * (div[getIndex(i, j)] + p[getIndex(i - 1, j)] + p[getIndex(i + 1, j)] +
                    p[getIndex(i, j - 1)] + p[getIndex(i, j + 1)]);
            }
        }
        setBoundary(N, 0, p);
    }

    for (unsigned int i = 1; i <= N; i++) 
    {
        for (unsigned int j = 1; j <= N; j++) 
        {
            u[getIndex(i, j)] -= 0.5f * (p[getIndex(i + 1, j)] - p[getIndex(i - 1, j)]) / h;
            v[getIndex(i, j)] -= 0.5f * (p[getIndex(i, j + 1)] - p[getIndex(i, j - 1)]) / h;
        }
    }

    setBoundary(N, 1, u);
    setBoundary(N, 2, v);
}