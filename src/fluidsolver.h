#pragma once
#include <vector>

extern const unsigned int N;
extern const unsigned int SIZE;

unsigned int getIndex(unsigned int i, unsigned int j);
float linInterp(float a, float b, float t);
float bilinInterp(float b_left, float t_left, float b_right, float t_right, float t_x, float t_y);
void addSource(unsigned int x, unsigned int y, std::vector<float>& v, float amount);
void diffuse(unsigned int N, int b, std::vector<float>& x, std::vector<float>& x0, float diff, float dt);
void advect(unsigned int N, int b, std::vector<float>& a, std::vector<float>& a0, std::vector<float>& u, std::vector<float>& v, float dt);
void setBoundary(unsigned int N, int b, std::vector<float>& x);
void project(unsigned int N, std::vector<float>& u, std::vector<float>& v, std::vector<float>& p, std::vector<float>& div);
