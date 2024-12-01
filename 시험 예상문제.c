#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h> // Sleep 함수 사용
#include <conio.h>   // _kbhit()와 _getch() 사용

// 기본 화면 크기 정의
#define WIDTH 40   // 화면의 가로 크기 (한 줄의 문자 수)
#define HEIGHT 24  // 화면의 세로 크기 (줄 수)
#define FPS 60     // 초당 프레임 수
#define FRAME_DURATION (1000 / FPS) // 한 프레임의 지속 시간 (밀리초)

// 각도를 라디안 값으로 변환하는 매크로
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0) 

#ifndef M_PI
#define M_PI 3.14159265358979323846 // 원주율 정의 (C 표준에서 누락 시 대비)
#endif

// 화면 데이터를 저장할 버퍼 (HEIGHT x WIDTH 크기)
char screenBuffer[HEIGHT][WIDTH + 1]; 

// 초기화 함수: 콘솔 커서를 숨겨 화면 깜박임 방지
void Elf2DInitScreen() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo); // 커서 정보 가져오기
    cursorInfo.bVisible = FALSE; // 커서를 보이지 않도록 설정
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo); // 변경된 커서 정보 적용
}

// 스크린 버퍼 초기화 함수: 모든 좌표를 공백으로 설정
void initializeBuffer() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            screenBuffer[y][x] = ' '; // 공백 문자로 초기화
        }
        screenBuffer[y][WIDTH] = '\0'; // 각 줄 끝에 NULL 문자 추가
    }
}

// 스크린 좌표에 문자를 기록하는 함수
void setPixel(int x, int y, const char* pixel) {
    // 좌표가 화면 크기를 초과하지 않는지 확인
    if (x >= 0 && x < WIDTH / 2 && y >= 0 && y < HEIGHT) {
        screenBuffer[y][x * 2] = pixel[0];       // 첫 번째 문자 기록
        screenBuffer[y][x * 2 + 1] = pixel[1];  // 두 번째 문자 기록
    } else {
        // 좌표가 범위를 벗어난 경우 디버그 메시지 출력
        printf("setPixel out of bounds: x=%d, y=%d\n", x, y);
    }
}

// 브레젠햄 알고리즘을 사용하여 직선을 그리는 함수
void drawLine(int x0, int y0, int x1, int y1, const char* pixel) {
    int dx = abs(x1 - x0);  // 가로 방향 거리
    int dy = abs(y1 - y0);  // 세로 방향 거리
    int sx = (x0 < x1) ? 1 : -1; // x 좌표 이동 방향
    int sy = (y0 < y1) ? 1 : -1; // y 좌표 이동 방향
    int err = (dx > dy ? dx : -dy) / 2; // 초기 오차 계산
    int e2;

    // 시작점부터 끝점까지 픽셀 기록
    while (1) {
        setPixel(x0, y0, pixel); // 현재 좌표에 픽셀 기록
        if (x0 == x1 && y0 == y1) break; // 끝점 도달 시 루프 종료
        e2 = err;
        if (e2 > -dx) { // x 방향 이동
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) { // y 방향 이동
            err += dx;
            y0 += sy;
        }
    }
}

// 점을 특정 각도만큼 회전시키는 함수
void rotatePoint(int cx, int cy, int* x, int* y, double angle) {
    double s = sin(angle); // 회전 각도의 사인 값
    double c = cos(angle); // 회전 각도의 코사인 값

    // 회전 기준점에서의 상대 좌표 계산
    int px = *x - cx;
    int py = *y - cy;

    // 회전 변환 수행
    int rx = (int)(px * c - py * s + cx);
    int ry = (int)(px * s + py * c + cy);

    // 회전된 좌표를 원본 좌표로 반환
    *x = rx;
    *y = ry;
}

// 게임 상태를 업데이트하는 함수
void update() {
    // A: (0, 0) -> (3, 0) 고정 직선
    int ax0 = 0, ay0 = 0;
    int ax1 = 2, ay1 = 0;
    drawLine(ax0, ay0, ax1, ay1, "AA");

    // B: A의 끝점에서 시작해 반시계 방향으로 30도 회전
    int bx0 = ax1, by0 = ay1;
    int bx1 = 6, by1 = 0;
    rotatePoint(bx0, by0, &bx1, &by1, DEG_TO_RAD(30));
    drawLine(bx0, by0, bx1, by1, "BB");

    // C: B의 끝점에서 시작해 반시계 방향으로 30도 회전
    int cx0 = bx1, cy0 = by1;
    int cx1 = 12, cy1 = 0;
    rotatePoint(cx0, cy0, &cx1, &cy1, DEG_TO_RAD(30));
    drawLine(cx0, cy0, cx1, cy1, "CC");

    // C의 끝점에 픽셀 표시 ('FF')
    setPixel(cx1, cy1, "FF");
}

// 화면을 렌더링하는 함수
void render() {
    // 커서를 화면의 처음 위치로 이동해 깜박임 방지
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    SetConsoleCursorPosition(hConsole, coord);

    // 버퍼의 내용을 화면에 출력
    for (int y = 0; y < HEIGHT; y++) {
        printf("%s\n", screenBuffer[y]);
    }
}

// 메인 게임 루프
void gameLoop() {
    int running = 1; // 게임 실행 상태
    clock_t lastFrameTime = clock(); // 마지막 프레임의 시간 기록

    while (running) {
        clock_t currentFrameTime = clock(); // 현재 프레임의 시간 기록
        if ((currentFrameTime - lastFrameTime) * 1000 / CLOCKS_PER_SEC >= FRAME_DURATION) {
            lastFrameTime = currentFrameTime; // 프레임 시작 시간 갱신

            initializeBuffer(); // 화면 초기화
            update();           // 게임 상태 업데이트
            render();           // 화면 렌더링

            // 사용자 입력 처리: 'q' 입력 시 게임 종료
            if (_kbhit()) {
                char input = _getch();
                if (input == 'q') running = 0;
            }
        }

        // CPU 사용률 절약을 위해 잠시 대기
        Sleep(1);
    }
}

// 프로그램 시작점
int main() {
    Elf2DInitScreen(); // 콘솔 초기화 (커서 숨기기)
    initializeBuffer(); // 화면 버퍼 초기화
    gameLoop();         // 메인 게임 루프 실행
    return 0;
}
