#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h> // Sleep 함수 사용
#include <conio.h>   // _kbhit()와 _getch() 사용

// 기본 화면 크기 정의
#define WIDTH 40   // 실제 화면의 가로 크기 (픽셀 단위)
#define HEIGHT 24  // 실제 화면의 세로 크기
#define FPS 60
#define FRAME_DURATION (1000 / FPS) // 한 프레임의 지속 시간 (밀리초)

#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0) // 각도를 라디안으로 변환

#ifndef M_PI
#define M_PI 3.14159265358979323846 // 원주율 값을 직접 정의
#endif

// 스크린 버퍼 선언
char screenBuffer[HEIGHT][WIDTH + 1];

// 초기화 함수: 커서 숨기기
void Elf2DInitScreen() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE; // 커서를 숨김
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// 스크린 버퍼 초기화 함수
void initializeBuffer() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            screenBuffer[y][x] = ' '; // 공백으로 초기화
        }
        screenBuffer[y][WIDTH] = '\0'; // 줄 끝에 NULL 추가
    }
}

// 스크린 좌표를 버퍼에 기록
void setPixel(int x, int y, const char* pixel) {
    if (x >= 0 && x < WIDTH / 2 && y >= 0 && y < HEIGHT) {
        screenBuffer[y][x * 2] = pixel[0];
        screenBuffer[y][x * 2 + 1] = pixel[1];
    }
    else {
        printf("setPixel out of bounds: x=%d, y=%d\n", x, y);
    }
}

// 브레젠햄 알고리즘으로 직선 그리기
void drawLine(int x0, int y0, int x1, int y1, const char* pixel) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1) {
        setPixel(x0, y0, pixel);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

// 점 회전 함수
void rotatePoint(int cx, int cy, int* x, int* y, double angle) {
    double s = sin(angle);
    double c = cos(angle);

    int px = *x - cx;
    int py = *y - cy;

    int rx = (int)(px * c - py * s + cx);
    int ry = (int)(px * s + py * c + cy);

    *x = rx;
    *y = ry;
}

// 업데이트 함수
void update() {
    // 고정된 A: (0, 0) -> (3, 0)
    int ax0 = 0, ay0 = 0;
    int ax1 = 2, ay1 = 0;
    drawLine(ax0, ay0, ax1, ay1, "AA");

    // B: A의 끝점 (3, 0) -> (3, 3), 반시계 방향 30도 회전
    int bx0 = ax1, by0 = ay1;
    int bx1 = 6, by1 = 0;
    rotatePoint(bx0, by0, &bx1, &by1, DEG_TO_RAD(30));
    drawLine(bx0, by0, bx1, by1, "BB");

    // C: B의 끝점에서 (3, 3) -> (6, 3), 반시계 방향 30도 회전
    int cx0 = bx1, cy0 = by1;
    int cx1 = 12, cy1 = 0;
    rotatePoint(cx0, cy0, &cx1, &cy1, DEG_TO_RAD(30));
    drawLine(cx0, cy0, cx1, cy1, "CC");

    // C의 끝점 표시 ('FF')
    setPixel(cx1, cy1, "FF");
}

// 렌더링 함수
void render() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(hConsole, coord);

    for (int y = 0; y < HEIGHT; y++) {
        printf("%s\n", screenBuffer[y]);
    }
}

// 게임 루프
void gameLoop() {
    int running = 1;
    clock_t lastFrameTime = clock(); // 이전 프레임 시간

    while (running) {
        clock_t currentFrameTime = clock();
        if ((currentFrameTime - lastFrameTime) * 1000 / CLOCKS_PER_SEC >= FRAME_DURATION) {
            lastFrameTime = currentFrameTime;

            // 디버그 메시지
            // printf("Frame update: %ld\n", currentFrameTime);

            // 화면 초기화
            initializeBuffer();

            // 업데이트
            update();

            // 렌더링
            render();

            // 사용자 입력 처리
            if (_kbhit()) {
                char input = _getch();
                if (input == 'q') running = 0; // 'q'를 눌러 종료
            }
        }

        // Sleep 호출로 CPU 사용률 절약
        Sleep(1);
    }
}

// 메인 함수
int main() {
    Elf2DInitScreen(); // 초기화 함수 호출
    initializeBuffer();
    gameLoop(); // 게임 루프 실행
    return 0;
}
