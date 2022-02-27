#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>

#define TRUE 1
#define FALE 0

typedef int Bool;

typedef struct Bullet {
	int x;		//미사일 x좌표
	int y;		//미사일 y좌표
	Bool fire;	//미사일 발사 상태
} Bullet;

//배열로 배경을 좌표로.
char bg[25][80];

//플레이어 좌표
int player_x;
int player_y;

//적 좌표
int enemy_x;
int enemy_y;

//미사일 갯수
Bullet player_bullets[20];

//함수 설계
void init_game();				//게임 초기화(위치,맵 초기화)
void clear_screen();			//화면을 지운다.
void game_main();				//게임 전체를 관리하는 함수
void print_screen();			//화면을 그려주는 함수
void key_control();				//키관련 함수
void bullet_draw();				//미사일을 그려주는 함수
void player_draw();				//플레이어 그려주는 함수
void enemy_draw();				//적 그려주는 함수
void enemy_move();				//적 움직여주는 함수
void clash_enemy_and_bullet();	//충돌처리 함수(미사일,적)
void clash_enemy_and_player();	//충돌처리 함수(적,를레이어)
void _game_over();				//게임오버처리 함수
void _arrow_key();				//방향키 입력 처리
void _space_key();				//스페이스바 입력 처리

inline
int get_tick_count() {
	return GetTickCount64();
}

inline
void set_window() {
	system("mode con cols=80 lines=25");
}

int main(void) {
	//랜덤함수 준비
	srand((unsigned)time(NULL));
	//게임 초기화
	init_game();
	//화면 크기 조정
	set_window();

	//현재 초단위 시간을 받아온다.
	int dw_time = get_tick_count();

	//게임실행 무한루프
	while (TRUE) {
		//0.05초 지연
		if (dw_time + 50 < get_tick_count()) {
			dw_time = get_tick_count();
			//화면 지우기
			clear_screen();
			//플레이어나 적이 움직임.
			game_main();
			//그려주기
			print_screen();
		}
	}

	return 0;
}

void init_game() {
	//플레이어 좌표위치
	player_x = 0;
	player_y = 12;
	//적 좌표 위치
	enemy_x = 77;
	enemy_y = 12;

	//플레이어 미사일 초기화 (미사일 발사전 준비상태)
	for (int i = 0; i < 20; i++) {
		player_bullets[i].x = 0;
		player_bullets[i].y = 0;
		player_bullets[i].fire = FALSE;
	}
}

void clear_screen() {
	//화면 지우기.
	system("cls");

	//배열 공백으로 초기화
	for (int y = 0; y < 25; y++) {
		for (int x = 0; x < 80; x++) {
			bg[y][x] = ' ';
		}
	}
}//화면을 지운다.

void game_main() {
	//키 입력
	key_control();
	//미사일을 그려준다.
	bullet_draw();
	//플레이어 그려준다.
	player_draw();
	//적의 움직임
	enemy_move();
	//적을 그려주는 부분
	enemy_draw();
	//미사일과 적 충돌
	clash_enemy_and_bullet();
	//플레이어와 적 충돌
	clash_enemy_and_player();
}//게임 전체를 관리하는 함수

void print_screen() {
	bg[24][79] = 0;
	printf("%s", (char*)bg);
}//화면을 그려주는 함수

void key_control() {
	char press_key;

	//키가 입력되면 잡아준다.
	if (_kbhit()) {
		press_key = _getch();

		switch (press_key) {
		case -32:
			// 화살표 키
			_arrow_key();
			break;
		case ' ':
			// 스페이스바 키
			_space_key();
			break;
		}
	}
}//키관련 함수

void bullet_draw() {
	Bullet* ptmpbullet;
	for (int i = 0; i < 20; i++) {
		//플레이어 미사일 날아가고 잇는 상태 true
		if (player_bullets[i].fire == TRUE) {
			// 미사일 그리기
			ptmpbullet = player_bullets + i;
			bg[ptmpbullet->y][ptmpbullet->x - 1] = '-';
			bg[ptmpbullet->y][ptmpbullet->x - 0] = '>';
			//미사일앞으로 + 1
			ptmpbullet->x++;
			//미사일이 80 넘어가면...
			if (ptmpbullet->x > 79) {
				// 준비상태로 전환
				ptmpbullet->fire = FALSE;
			}
		}
	}
}//미사일을 그려주는 함수

void player_draw() {
	bg[player_y - 1][player_x + 1] = '>';
	bg[player_y - 1][player_x + 0] = '-';
	bg[player_y + 0][player_x + 1] = '>';
	bg[player_y + 0][player_x + 2] = '>';
	bg[player_y + 0][player_x + 3] = '>';
	bg[player_y + 1][player_x + 0] = '-';
	bg[player_y + 1][player_x + 1] = '>';
}//플레이어 그려주는 함수

void enemy_draw() {
	bg[enemy_y][enemy_x - 2] = '<';
	bg[enemy_y][enemy_x - 1] = '-';
	bg[enemy_y][enemy_x - 0] = 'O';
	bg[enemy_y][enemy_x + 1] = '-';
	bg[enemy_y][enemy_x + 2] = '>';
}//적 그려주는 함수

void enemy_move() {
	enemy_x--;
	if (enemy_x < 2) {
		enemy_x = 77;
		enemy_y = (rand() % 20) + 2;
	}
}//적 움직여주는 함수

void clash_enemy_and_bullet() {
	Bullet* ptmpbullet;
	// 미사일 20개 전부 판별
	for (int i = 0; i < 20; i++) {
		ptmpbullet = player_bullets + i;
		if (ptmpbullet->fire == TRUE) {
			//적과 미사일의 y값 비교
			if (ptmpbullet->y == enemy_y) {
				//적과 미사일의 x값 비교
				if (ptmpbullet->x >= (enemy_x - 1)
					&& ptmpbullet->x <= (enemy_x + 1)) {
					enemy_x = 77;
					enemy_y = (rand() % 20) + 2;
					ptmpbullet->fire = FALSE;
					break;
				}
			}
		}
	}
}//충돌처리

void clash_enemy_and_player() {
	//플레이어 몸통, 적 y값 비교
	if (enemy_y == player_y) {
		//플레이어 몸통, 적 x값 비교
		if (player_x + 2 >= (enemy_x - 1)
			&& player_x + 2 <= (enemy_x + 1)) {
			_game_over();
			return;
		}
	}

	//플레이어 날개, 적 y값 비교
	if (enemy_y >= (player_y - 2)
		&& enemy_y <= (player_y + 2)) {
		//플레이어 날개, 적 x값 비교
		if (player_x >= (enemy_x - 1)
			&& player_x <= (enemy_x + 1)) {
			_game_over();
			return;
		}
	}
}

void _game_over() {
	char gover[] = "Game Over!";
	int len = 10;

	clear_screen();

	for (int i = 0; i < len; i++)
		bg[10][20 + i] = gover[i];

	print_screen();

	init_game();

	system("pause");
	return;
}

void _arrow_key() {
	char press_key = _getch();
	switch (press_key) {
	case 72:
		//위쪽 방향
		player_y--;
		if (player_y < 1)
			player_y = 1;
		break;
	case 75:
		//왼쪽 방향
		player_x--;
		if (player_x < 0)
			player_x = 0;
		break;
	case 77:
		//오른쪽 방향
		player_x++;
		if (player_x > 75)
			player_x = 75;
		break;
	case 80:
		//아래쪽 방향
		player_y++;
		if (player_y > 23)
			player_y = 23;
	}
}//화살표 입력 처리

void _space_key() {
	//스페이스바키가 눌리면 미사일 발사
	for (int i = 0; i < 20; i++) {
		if (player_bullets[i].fire == FALSE) {
			player_bullets[i].fire = TRUE;
			//플레이어 앞에서 미사일 쏘기
			player_bullets[i].x = player_x + 5;
			player_bullets[i].y = player_y;
			break; // 미사일 한 개만..
		}
	}
}
