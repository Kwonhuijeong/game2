#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 24
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void clearScreen() {
    system("cls");
}

void printCentered(int y, const char* text) {
    int textLength = strlen(text);
    int centerX = (WIDTH - textLength) / 2;

    // 이동 후 텍스트 출력
    printf("\033[%d;%dH%s", y + 1, centerX + 1, text);
    fflush(stdout);
}

typedef struct {
    float x, y, w;
} vec3;

typedef struct {
    float x, y;
} vec2;

typedef struct {
    float m[3][3];
} Matrix3x3;

void initIdentityMatrix(Matrix3x3* mat) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat->m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

Matrix3x3 createRotationMatrix(float angle) {
    Matrix3x3 mat;
    initIdentityMatrix(&mat);
    float radian = angle * M_PI / 180.0f;
    mat.m[0][0] = cos(radian);
    mat.m[0][1] = -sin(radian);
    mat.m[1][0] = sin(radian);
    mat.m[1][1] = cos(radian);
    return mat;
}

void drawPixel(int x, int y, const char* pixel) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        printf("\033[%d;%dH%s", y + 1, x + 1, pixel);
    }
}

void drawFilledRectangle(vec2 center, int size, float angle, const char* pixel) {
    for (float y = -size; y <= size; y += 0.5f) {
        for (float x = -size; x <= size; x += 0.5f) {
            int rotatedX = (int)(x * cos(angle * M_PI / 180) - y * sin(angle * M_PI / 180) + center.x);
            int rotatedY = (int)(x * sin(angle * M_PI / 180) + y * cos(angle * M_PI / 180) + center.y);
            drawPixel(rotatedX, rotatedY, pixel);
        }
    }
}

float sign(vec2 p1, vec2 p2, vec2 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

int pointInTriangle(vec2 pt, vec2 v1, vec2 v2, vec2 v3) {
    float d1, d2, d3;
    int has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

void drawSun(vec2 sunCenter, float sunSelfAngle) {
    float size = 5.0f;
    float angleRad = sunSelfAngle * M_PI / 180.0f;

    vec2 p1 = { sunCenter.x + size * cos(angleRad), sunCenter.y + size * sin(angleRad) };
    vec2 p2 = { sunCenter.x + size * cos(angleRad + 2 * M_PI / 3), sunCenter.y + size * sin(angleRad + 2 * M_PI / 3) };
    vec2 p3 = { sunCenter.x + size * cos(angleRad + 4 * M_PI / 3), sunCenter.y + size * sin(angleRad + 4 * M_PI / 3) };

    float minX = fminf(fminf(p1.x, p2.x), p3.x);
    float maxX = fmaxf(fmaxf(p1.x, p2.x), p3.x);
    float minY = fminf(fminf(p1.y, p2.y), p3.y);
    float maxY = fmaxf(fmaxf(p1.y, p2.y), p3.y);

    for (float y = minY; y <= maxY; y++) {
        for (float x = minX; x <= maxX; x++) {
            if (pointInTriangle((vec2) { x, y }, p1, p2, p3)) {
                drawPixel((int)x, (int)y, "SS");
            }
        }
    }
}

void drawEarth(vec2 earthCenter, float earthSelfAngle) {
    drawFilledRectangle(earthCenter, 2, earthSelfAngle, "E");
}

void drawMoon(vec2 moonCenter, float moonSelfAngle) {
    drawFilledRectangle(moonCenter, 1, moonSelfAngle, "M");
}

void drawPlanets(int earthAngle, int moonAngle, float earthSelfAngle, float moonSelfAngle, float sunSelfAngle) {
    clearScreen();
    int centerX = WIDTH / 2;
    int centerY = HEIGHT / 2;

    vec2 sunPos = { centerX, centerY };
    vec3 earthPos = { 12.0f, 0.0f, 1.0f };
    earthPos.x = sunPos.x + 24 * cos(earthAngle * M_PI / 180);
    earthPos.y = sunPos.y + 12 * sin(earthAngle * M_PI / 180);

    vec3 moonPos = { 4.0f, 0.0f, 1.0f };
    moonPos.x = earthPos.x + 8 * cos(moonAngle * M_PI / 180);
    moonPos.y = earthPos.y + 4 * sin(moonAngle * M_PI / 180);

    drawSun(sunPos, sunSelfAngle);
    drawEarth((vec2) { earthPos.x, earthPos.y }, earthSelfAngle);
    drawMoon((vec2) { moonPos.x, moonPos.y }, moonSelfAngle);
}

int main() {
    int running = 1;
    int earthAngle = 0;
    int moonAngle = 0;
    float earthSelfAngle = 0;
    float moonSelfAngle = 0;
    float sunSelfAngle = 0;
    int state = 0; // 0: 시작, 1: 행성 배치, 2: 코드 실행
    int startTime = time(NULL);

    while (running) {
        if (state == 0) {
            clearScreen();
            printCentered(HEIGHT / 2 - 1, "학번: 20200660");
            printCentered(HEIGHT / 2, "이름: 권희정");
            printCentered(HEIGHT / 2 + 1, "스페이스바를 눌러주세요.");

            if (_kbhit()) {
                char ch = _getch();
                if (ch == 27) { // ESC 키로 종료
                    running = 0;
                }
                else if (ch == 32) { // 스페이스바로 행성 배치
                    state = 1;
                }
            }

        }
        else if (state == 1) {
            drawPlanets(earthAngle, moonAngle, earthSelfAngle, moonSelfAngle, sunSelfAngle);

            if (_kbhit()) {
                char ch = _getch();
                if (ch == 32) { // 스페이스바로 코드 실행
                    state = 2;
                }
                else if (ch == 27) { // ESC 키로 종료
                    running = 0;
                }
            }
        }
        else if (state == 2) {
            drawPlanets(earthAngle, moonAngle, earthSelfAngle, moonSelfAngle, sunSelfAngle);
            earthAngle = (earthAngle + 6) % 360;
            moonAngle = (moonAngle + 30) % 360;
            earthSelfAngle -= 360.0f / (2.0f * 10.0f);
            moonSelfAngle -= 360.0f / (1.0f * 10.0f);
            sunSelfAngle -= 360.0f / (5.0f * 10.0f);

            Sleep(100);

            if (_kbhit()) {
                char ch = _getch();
                if (ch == 27) { // ESC 키로 종료
                    running = 0;
                }
            }

            // 5분 이상 실행되도록 보장
            if (time(NULL) - startTime > 300) {
                startTime = time(NULL); // 5분마다 시간 리셋
            }
        }
    }

    return 0;
}