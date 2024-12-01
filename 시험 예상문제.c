#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h> // Sleep 함수 사용 (윈도우에서 CPU 절약을 위해 사용)

// 기본 화면 크기 정의
#define WIDTH 40   // 화면의 가로 크기 (픽셀 단위)
#define HEIGHT 24  // 화면의 세로 크기 (픽셀 단위)
#define FPS 60     // 초당 프레임 수 (FPS)
#define FRAME_DURATION (1000 / FPS) // 한 프레임의 지속 시간 (밀리초 단위)

// 각도를 라디안으로 변환하는 매크로 정의
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0) // 각도를 도에서 라디안으로 변환

#ifndef M_PI
#define M_PI 3.14159265358979323846 // 원주율 값을 직접 정의 (컴파일러가 정의하지 않았다면 사용)
#endif

// 스크린 버퍼 선언: 화면의 픽셀을 저장할 2D 배열
char screenBuffer[HEIGHT][WIDTH + 1];

// 화면 초기화 함수
void initializeBuffer() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            screenBuffer[y][x] = ' '; // 화면을 공백으로 초기화
        }
        screenBuffer[y][WIDTH] = '\0'; // 각 행의 끝에 NULL 문자 추가 (문자열 끝 표시)
    }
}

// 특정 좌표에 픽셀을 설정하는 함수
void setPixel(int x, int y, const char* pixel) {
    // x, y가 화면의 유효 범위 내에 있는지 확인 (픽셀 좌표가 범위를 벗어나지 않도록)
    if (x >= 0 && x < WIDTH / 2 && y >= 0 && y < HEIGHT) {
        // 화면의 가로 픽셀은 2칸씩 차지하므로, x*2로 계산하여 적절한 위치에 픽셀 설정
        screenBuffer[y][x * 2] = pixel[0];  // 첫 번째 문자 저장
        screenBuffer[y][x * 2 + 1] = pixel[1];  // 두 번째 문자 저장
    }
}

// 두 점을 연결하는 직선을 그리는 함수 (Bresenham 알고리즘 사용)
void drawLine(int x0, int y0, int x1, int y1, const char* pixel) {
    int dx = abs(x1 - x0); // 가로 방향 거리
    int dy = abs(y1 - y0); // 세로 방향 거리
    int sx = (x0 < x1) ? 1 : -1; // x 방향 진행 방향
    int sy = (y0 < y1) ? 1 : -1; // y 방향 진행 방향
    int err = (dx > dy ? dx : -dy) / 2; // 오류 값 초기화
    int e2;

    // Bresenham 알고리즘을 사용하여 직선을 그리기
    while (1) {
        setPixel(x0, y0, pixel); // 현재 위치에 픽셀 설정
        if (x0 == x1 && y0 == y1) break; // 목표 지점에 도달하면 종료
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx; // x 방향으로 이동
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy; // y 방향으로 이동
        }
    }
}

// 주어진 점을 중심으로 회전하는 함수 (회전 행렬을 사용)
void rotatePoint(int cx, int cy, int* x, int* y, double angle) {
    double s = sin(angle); // sin(angle)
    double c = cos(angle); // cos(angle)

    int px = *x - cx; // 중심에서 x의 거리
    int py = *y - cy; // 중심에서 y의 거리

    // 회전된 좌표 계산
    int rx = (int)(px * c - py * s + cx); // 회전된 x좌표
    int ry = (int)(px * s + py * c + cy); // 회전된 y좌표

    *x = rx; // 새로운 x값
    *y = ry; // 새로운 y값
}

// 입력을 처리하는 함수 (키보드 입력 감지)
int Input() {
    if (_kbhit()) { // 키보드 입력이 있으면
        char input = _getch(); // 입력된 문자 가져오기
        if (input == 'q') return 99; // 'q' 키를 누르면 게임 종료
    }
    return 0; // 그 외의 입력은 0 반환
}

// 게임 상태를 업데이트하는 함수 (현재는 빈 함수로, 상태 변경을 여기에 구현)
void Update(int* LineObj, int numLines, int event, double deltaTime) {
    // 여기에 게임 상태를 업데이트하는 코드 작성
    // 예: 객체의 위치 업데이트, 회전 등
}

// 화면을 렌더링하는 함수 (현재는 빈 함수로, 게임 화면을 그리는 코드 작성 예정)
void Render(int* LineObj, int numLines, char screenBuffer[HEIGHT][WIDTH + 1], int screenWidth, int screenHeight) {
    // 여기에 게임 화면을 렌더링하는 코드 작성
    // 예: LineObj의 상태를 바탕으로 화면에 그리기
}

// 게임 루프 함수
void gameLoop() {
    int running = 1;  // 게임이 진행 중인지를 나타내는 변수
    clock_t lastFrameTime = clock(); // 마지막 프레임 시간 기록
    double deltaTime = 0.0; // Delta Time 초기화 (두 프레임 사이의 시간 차이)

    // 게임 루프
    while (running) {
        clock_t currentFrameTime = clock(); // 현재 시간 기록

        // Delta Time 계산: 현재 시간과 마지막 프레임의 시간을 비교하여 시간 차이를 계산
        deltaTime = (double)(currentFrameTime - lastFrameTime) / CLOCKS_PER_SEC * 1000.0; // 밀리초 단위로 변환
        lastFrameTime = currentFrameTime; // 마지막 프레임 시간 업데이트

        // 1. 입력 처리
        int event = Input(); // 키보드 입력을 처리
        if (event == 99) { // 'q' 입력 시 게임 종료
            running = 0;
        }

        // 2. 게임 상태 업데이트
        Update(NULL, 0, event, deltaTime); // 게임 객체 상태 업데이트

        // 3. 화면 렌더링
        Render(NULL, 0, screenBuffer, WIDTH, HEIGHT); // 화면에 객체 상태 렌더링

        // 4. 스크린 버퍼 출력: 화면을 갱신하여 출력
        system("cls"); // 콘솔 화면을 지우고, 새로운 화면을 그리기
        for (int y = 0; y < HEIGHT; y++) {
            printf("%s\n", screenBuffer[y]); // 각 줄을 출력
        }

        // CPU 사용률 절약: Delta Time에 맞춰 Sleep 시간을 계산하여 CPU 버닝 방지
        double frameTime = FRAME_DURATION - deltaTime; // 한 프레임에 필요한 시간과 실제 걸린 시간 차이 계산
        if (frameTime > 0) {
            Sleep((DWORD)frameTime); // 밀리초 단위로 Sleep 호출하여 대기
        }
    }
}

// 프로그램의 시작점 (main 함수)
int main() {
    initializeBuffer(); // 화면 버퍼 초기화
    gameLoop(); // 게임 루프 실행
    return 0; // 프로그램 종료
}
