#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <iostream>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <fstream>
using namespace std;

int highScore = 0;
int spawnMoveable = 0;

const int key_up = 72;
const int key_down = 115;
const int key_right = 77;
const int key_left = 75;
const int key_super_down = 119;
const int spacebar = 32;
const int key_speed_down = 80;
const int hold = 9;

const int width = 12;
const int height = 24 + 4;
int displayBuffer[width * height] = { 0 };
int block_array[width * height] = { 0 };
int ColorBuffer[width * height] = { 0 };
int Color_array[width * height] = { 0 };
int DestroyLines[height - 2] = { 0 };
int contect_count = 0;
int lineBlocks[height - 2] = { 0 };

void draw(int w, int h, int* buffer, int* blocks);
void destroyLine();
void checkLine();

class Tetrominoes {
public:
	int x;
	int y;
	int size;
	int color;
	int* blocks;
	int length;

	Tetrominoes();
	Tetrominoes(int* blck, int leng, int siz, int xpos, int ypos, int clr);
	void clone(Tetrominoes& object);
	int* rotate_return(int direction);
	void rotate(int direction);
	int isCollsion(int vx, int vy, int vd);
	void build();

	~Tetrominoes() { delete[] blocks; delete& x; delete& y; delete& size; delete& length; delete& color; }
};

Tetrominoes::Tetrominoes() { }

Tetrominoes::Tetrominoes(int* blck, int leng, int siz, int xpos, int ypos, int clr) {
	blocks = blck; length = leng; size = siz; x = xpos; y = ypos; color = clr;
}

void Tetrominoes::clone(Tetrominoes& object) {
	x = object.x; y = object.y; size = object.size; length = object.length; color = object.color;
	delete[] blocks;
	blocks = new int[object.length];
	for (int i = 0; i < object.length; i++) {
		blocks[i] = object.blocks[i];
	}
}

int* Tetrominoes::rotate_return(int direction) { //later : d = 3 -> -1
	int r_direction = direction % 4;
	int d = 0;

	if (r_direction != 0)
		d = abs(r_direction) / r_direction;
	else
		return 0;

	int* rotation = new int[length]; // [0] 오버런 

	for (int j = 0; j < abs(r_direction); j++) {
		for (int i = 0; i < length; i++) {
			int index = blocks[i];
			int x = index % size;
			int y = index / size;
			int temp = x;
			x = (size - 1) * (d == 1) + -(d * y);
			y = (size - 1) * (d != 1) + (d * temp);
			rotation[i] = x + y * size; // [0]오버런 발생했음
		}
	}
	return rotation;
}

void Tetrominoes::rotate(int direction) { //later : d = 3 -> -1
	int r_direction = direction % 4;
	int d = 0;
	if (r_direction != 0)
		d = abs(r_direction) / r_direction;
	else
		return;


	for (int j = 0; j < abs(r_direction); j++) {
		for (int i = 0; i < 4; i++) {
			int index = blocks[i];
			int x = index % size;
			int y = index / size;
			int temp = x;
			x = (size - 1) * (d == 1) + -(d * y);
			y = (size - 1) * (d != 1) + (d * temp);
			blocks[i] = x + y * size;
		}
	}
}

int Tetrominoes::isCollsion(int vx, int vy, int vd) {
	int* future;

	if (vd != 0) {
		future = rotate_return(vd);
	}
	else {
		future = new int[length];
		for (int i = 0; i < length; i++) {
			future[i] = blocks[i];
		}
	}

	int collision = 0;
	for (int i = 0; i < length; i++) {
		int index = future[i];
		int i_x = index % size;
		int i_y = index / size;
		if (x + i_x + vx < 0 || x + i_x + vx > width || y + i_y + vy < 0 || y + i_y + vy > height) { continue; }
		if (block_array[x + i_x + vx + (y + i_y + vy) * width] != 0)
			collision = 1;
	}

	delete[] future;
	return collision;
}

void Tetrominoes::build() {
	for (int i = 0; i < length; i++) {
		int index = blocks[i];
		int rx = x + index % size;
		int ry = y + index / size;
		if (rx < 0 || rx > width || ry < 0 || ry > height) { continue; }
		block_array[rx + ry * width] = 1;
		Color_array[rx + ry * width] = color;
	}
}

//tetrominoes setting
int t_I[4] = { 1, 5, 9, 13 };
Tetrominoes tetro_I(t_I, 4, 4, 4, 5, 11);

int t_J[4] = { 1, 4, 6, 7 };
Tetrominoes tetro_J(t_J, 4, 3, 4, 5, 9);

int t_L[4] = { 1, 4, 7, 8 };
Tetrominoes tetro_L(t_L, 4, 3, 4, 5, 6);

int t_O[4] = { 0, 1, 2, 3 };
Tetrominoes tetro_O(t_O, 4, 2, 4, 5, 14);

int t_S[4] = { 4, 5, 6, 7 };
Tetrominoes tetro_S(t_S, 4, 3, 4, 4, 10);

int t_T[4] = { 1, 3, 4, 5 };
Tetrominoes tetro_T(t_T, 4, 3, 4, 5, 13);

int t_Z[4] = { 3, 4, 7, 8 };
Tetrominoes tetro_Z(t_Z, 4, 3, 4, 4, 12);

Tetrominoes tetrominoes_array[7] = { tetro_O, tetro_S, tetro_I, tetro_J, tetro_L, tetro_T, tetro_Z };


void gotoxy(int x, int y) {
	COORD pos = { (short)x, (short)y }; //x, y 좌표 설정
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); //커서 설정
}
void cursorView() {
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.bVisible = FALSE;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void draw_letter(char* character, int color, int x, int y, int* letter, int length, int w) {
	setColor(color);
	for (int i = 0; i < length; i++) {
		int index = letter[i];
		gotoxy(x + index % w, y + index / w);
		printf("%s", character);
	}
}

int toint(char* number) {
	int result = 0;
	for (;*number; number++)
	{
		result = result * 10 + *number - 48;
	}
	return result;
}


void spawn(Tetrominoes* falling_tetrominoes, int* game);
void getGhost(Tetrominoes* falling_tetrominoes);

int timer = 0;
int timer2 = 0;
int timer3 = 0;
int score = 0;
int level = 1;
int delay = 64;
int level_next = 0;
int isBreaking = 0;
int checkSinkY = 0;
int nextTetro = 0;
int wait = 0;
int holdTetro = -1;
int nowTetro = 0;
int lines = 0;

int blink = 0;


void initialize() {
	timer = 0;
	timer2 = 0;
	timer3 = 0;
	score = 0;
	level = 1;
	delay = 64;
	level_next = 0;
	isBreaking = 0;
	checkSinkY = 0;
	nextTetro = 0;
	wait = 0;
	holdTetro = -1;
	nowTetro = 0;
	lines = 0;

	blink = 0;
	memset(displayBuffer, 0, sizeof(int) * width * height);
	memset(block_array, 0, sizeof(int) * width * height);
	memset(ColorBuffer, 0, sizeof(int) * width * height);
	memset(Color_array, 0, sizeof(int) * width * height);
	memset(DestroyLines, 0, sizeof(int) * (static_cast<unsigned long long>(height) - 2));
	memset(lineBlocks, 0, sizeof(int) * (static_cast<unsigned long long>(height) - 2));
	contect_count = 0;

	srand(time(NULL));
}

int game() {

	for (int i = 0; i < width * height; i++) {
		if (i % width == 0 || i % width == width - 1 || i / width == height - 1) {
			if (!((i % width == 0 || i % width == width - 1) && i / width < 5)) {
				block_array[i] = 2;
				Color_array[i] = 8;
			}
		}
	}

	int key = 0;
	nowTetro = rand() % 7;
	Tetrominoes* falling_tetrominoes;
	falling_tetrominoes = new Tetrominoes();
	falling_tetrominoes->clone(tetrominoes_array[nowTetro]);

	getGhost(falling_tetrominoes);


	//printf("_________   ________   _________    _______      ___    ________\n\n");
	//printf("|__   __|   |  ____|   |__   __|    |   _  \\     | |    |  ____|\n\n");
	//printf("   | |      | |____       | |       |  |_| |     | |    | |____ \n\n");
	//printf("   | |      |  ____|      | |       |  __  /     | |    |____  |\n\n");
	//printf("   | |      | |____       | |       | |  \\ \\     | |    _____| | \n\n");
	//printf("   |_|      |______|      |_|       |_|   \\_\\    |_|    |______|\n\n");


	int title_x = 8;
	int title_y = 5;

	char character[] = "▣";
	int* letter = new int[22] { 0, 1, 2, 3, 4, 5, 8, 9, 14, 15, 20, 21, 26, 27, 32, 33, 38, 39, 44, 45, 50, 51 };
	draw_letter(character, 4, title_x, title_y, letter, 22, 6);
	delete[] letter;

	letter = new int[30] { 0, 1, 2, 3, 4, 5, 6, 7, 12, 13, 18, 19, 24, 25, 26, 27, 28, 29, 30, 31, 36, 37, 42, 43, 48, 49, 50, 51, 52, 53};
	draw_letter(character, 6, 7 + title_x, title_y, letter, 30, 6);
	delete[] letter;

	letter = new int[22] { 0, 1, 2, 3, 4, 5, 8, 9, 14, 15, 20, 21, 26, 27, 32, 33, 38, 39, 44, 45, 50, 51 };
	draw_letter(character, 14, 14 + title_x, title_y, letter, 22, 6);
	delete[] letter;

	letter = new int[39] { 0, 1, 2, 3, 4, 6, 7, 10, 11, 12, 13, 17, 18, 19, 21, 22, 23, 24, 25, 26, 27, 30, 31, 33, 34, 36, 37, 39, 40, 42, 43, 46, 47, 48, 49, 52, 53};
	draw_letter(character, 2, 21 + title_x, title_y, letter, 39, 6);
	delete[] letter;

	letter = new int[26] { 0, 1, 2, 3, 4, 5, 8, 9, 14, 15, 20, 21, 26, 27, 32, 33, 38, 39, 44, 45, 48, 49, 50, 51, 52, 53 };
	draw_letter(character, 1, 28 + title_x, title_y, letter, 26, 6);
	delete[] letter;

	letter = new int[32] { 2, 3, 4, 5, 7, 8, 9, 10, 12, 13, 14, 18, 19, 20, 25, 26, 27, 28, 33, 34, 35, 39, 40, 41, 43, 44, 45, 46, 48, 49, 50, 51 };
	draw_letter(character, 5, 35 + title_x, title_y, letter, 32, 6);
	delete[] letter;

	setColor(8);
	gotoxy(22, 22);
	printf("SPIN : ");
	setColor(3);
	printf("\"↑\", \"S\" ");
	gotoxy(22, 24);
	setColor(8);
	printf("MOVE : ");
	setColor(3);
	printf("\"←\", \"→\"");
	gotoxy(22, 26);
	setColor(8);
	printf("HOLD : ");
	setColor(3);
	printf("\"TAP\"");
	gotoxy(22, 28);
	setColor(8);
	printf("SOFT DROP : ");
	setColor(3);
	printf("\"↓\" ");
	gotoxy(22, 30);
	setColor(8);
	printf("HARD DROP : ");
	setColor(3);
	printf("\"W\", \"SPACE\" ");

	gotoxy(22,2);
	setColor(3);
	printf("HIGH SCORE : %d", highScore);


	int start_wait = 0;
	int sw = 15;
	int colors[7] = { 4, 12, 6, 2, 3, 2, 5 };
	while (1) { //gmae start input
		if (start_wait < 350 * sw) {
			setColor(0);
			gotoxy(20, 17);
			printf("Enter KEY to start");
		}
		else
		{
			if (start_wait < 400 * sw) {
				setColor(colors[0]);
				gotoxy(20, 17);
				printf("Enter ");
				setColor(15);
				printf("KEY to start");
			}
			else if (start_wait < 450 * sw) {
				setColor(colors[1]);
				gotoxy(20, 17);
				printf("Enter ");
				setColor(15);
				printf("KEY to start");
			}
			else if (start_wait < 500 * sw) {
				setColor(colors[2]);
				gotoxy(20, 17);
				printf("Enter ");
				setColor(15);
				printf("KEY to start");
			}
			else if (start_wait < 550 * sw) {
				setColor(colors[3]);
				gotoxy(20, 17);
				printf("Enter ");
				setColor(15);
				printf("KEY to start");
			}
			else if (start_wait < 600 * sw) {
				setColor(colors[4]);
				gotoxy(20, 17);
				printf("Enter ");
				setColor(15);
				printf("KEY to start");
			}
			else if (start_wait < 650 * sw) {
				setColor(colors[5]);
				gotoxy(20, 17);
				printf("Enter ");
				setColor(15);
				printf("KEY to start");
			}
			else if (start_wait < 700 * sw) {
				setColor(colors[6]);
				gotoxy(20, 17);
				printf("Enter ");
				setColor(15);
				printf("KEY to start");
			}
			if (start_wait == 700 * sw) {
				start_wait = 0;
			}
		}

		start_wait++;


		if (_kbhit()) {
			if (_getch() == '\r') {
				break;
			}
		}
	}
	PlaySound(TEXT("break.wav"), 0, SND_ASYNC);

	system("cls");

	///*PlaySound(TEXT("BackgroundMusic.wav"), 0, SND_ASYNC | SND_LOOP);*/

	int game = 1;
	while (game) {
		memset(displayBuffer, 0, sizeof(int) * width * height);
		memset(ColorBuffer, 0, sizeof(int) * width * height);


		if (_kbhit()) {
			key = _getch();
			if (key == 224) {
				key = _getch();
			}
		}
		else {
			key = 0;
		}



		if (key == key_right) {
			if (falling_tetrominoes->isCollsion(1, 0, 0) == 0) {
				falling_tetrominoes->x++;
				getGhost(falling_tetrominoes);
			}
			PlaySound(TEXT("change.wav"), 0, SND_ASYNC);
		}
		else if (key == key_left) {
			if (falling_tetrominoes->isCollsion(-1, 0, 0) == 0) {
				falling_tetrominoes->x--;
				getGhost(falling_tetrominoes);
			}
			PlaySound(TEXT("change.wav"), 0, SND_ASYNC);
		}
		else if (key == key_up) { // right rotate
			if (falling_tetrominoes->isCollsion(0, 0, 1) == 0) {
				falling_tetrominoes->rotate(1);
				PlaySound(TEXT("change.wav"), 0, SND_ASYNC);
			}
			else
			{
				falling_tetrominoes->rotate(1);

				int notCollision = 0;
				for (int i = 1; i <= falling_tetrominoes->size / 2; i++) {
					if (falling_tetrominoes->isCollsion(i, 0, 0) == 0) {
						notCollision = 1;
						falling_tetrominoes->x += i;
						break;
					}
				}
				if (notCollision == 0) {
					for (int i = 1; i <= falling_tetrominoes->size / 2; i++) {
						if (falling_tetrominoes->isCollsion(-i, 0, 0) == 0) {
							notCollision = 1;
							falling_tetrominoes->x -= i;
							break;
						}
					}
				}

				if (notCollision == 0) {
					falling_tetrominoes->rotate(-1);
				}
				else {
					PlaySound(TEXT("change.wav"), 0, SND_ASYNC);
				}
			}

			getGhost(falling_tetrominoes);
		}
		else if (key == key_down) { // left rotate
			if (falling_tetrominoes->isCollsion(0, 0, -1) == 0) {
				falling_tetrominoes->rotate(-1);
				PlaySound(TEXT("change.wav"), 0, SND_ASYNC);
			}
			else
			{
				falling_tetrominoes->rotate(-1);

				int notCollision = 0;
				for (int i = 1; i <= falling_tetrominoes->size / 2; i++) {
					if (falling_tetrominoes->isCollsion(i, 0, 0) == 0) {
						notCollision = 1;
						falling_tetrominoes->x += i;
						break;
					}
				}
				if (notCollision == 0) {
					for (int i = 1; i <= falling_tetrominoes->size / 2; i++) {
						if (falling_tetrominoes->isCollsion(-i, 0, 0) == 0) {
							notCollision = 1;
							falling_tetrominoes->x -= i;
							break;
						}
					}
				}

				if (notCollision == 0) {
					falling_tetrominoes->rotate(1);
				}
				else {
					PlaySound(TEXT("change.wav"), 0, SND_ASYNC);
				}
			}

			getGhost(falling_tetrominoes);
		}
		else if (key == key_speed_down) { // move down
			if (falling_tetrominoes->isCollsion(0, 1, 0) == 0) {
				falling_tetrominoes->y++;
				getGhost(falling_tetrominoes);
			}
			else {
				spawn(falling_tetrominoes, &game);

				if (game) {
					getGhost(falling_tetrominoes);
				}
				else {
					draw(width, height, displayBuffer, block_array);
					break;
				}

				score += 10;

				checkLine();
			}
		}
		else if (key == key_super_down || key == spacebar) { // drop to the bottom
			getGhost(falling_tetrominoes);

			falling_tetrominoes->y += checkSinkY;

			spawn(falling_tetrominoes, &game);

			if (game) {
				getGhost(falling_tetrominoes);
			}
			else {
				draw(width, height, displayBuffer, block_array);
				break;
			}

			score += 20;

			checkLine();

			PlaySound(TEXT("drop.wav"), 0, SND_ASYNC);
		}
		else if (key == hold) {
			if (nowTetro != holdTetro) {
				PlaySound(TEXT("swipe.wav"), 0, SND_ASYNC);

				if (holdTetro >= 0) {
					falling_tetrominoes->clone(tetrominoes_array[holdTetro]);
					getGhost(falling_tetrominoes);
					holdTetro = nowTetro;
				}
				else {
					holdTetro = nowTetro;

					int index = rand() % 7;
					int lastIndex = index;
					if (nextTetro == index && rand() % 10 != 0) {
						index = rand() % 6;
						if (index >= lastIndex)
							index++;
					}
					nextTetro = nextTetro % 7;
					nowTetro = nextTetro;
					falling_tetrominoes->clone(tetrominoes_array[nextTetro]);

					int up = 0;
					for (; up < height; up++) {
						if (falling_tetrominoes->isCollsion(0, -up, 0) == 0) {
							break;
						}
					}

					falling_tetrominoes->y -= up;
					if (up > 0) {
						falling_tetrominoes->build();
						draw(width, height, displayBuffer, block_array);
						break;
					}

					getGhost(falling_tetrominoes);

					nextTetro = index;
				}

			}
		}


		level_next = (level) * (level) * 250;
		if (score >= level_next) {
			level++;
			if (level < 8) {
				delay = (int)(delay / 1.5);
			}
		}

		if (timer >= delay) {
			if (falling_tetrominoes->isCollsion(0, 1, 0) == 0) {
				falling_tetrominoes->y++;
				getGhost(falling_tetrominoes);
				timer3 = 0;
			}
			else {
				wait = 1;
			}
			timer = 0;
		}
		if (!isBreaking) {
			timer++;
		}

		if (wait) {
			timer3++;
			if (timer3 > 20 && falling_tetrominoes->isCollsion(0, 1, 0)) {

				spawn(falling_tetrominoes, &game);

				if (game) {
					getGhost(falling_tetrominoes);

				}
				else {
					draw(width, height, displayBuffer, block_array);
					break;
				}

				score += 5;

				checkLine();


				timer3 = 0;
				wait = 0;
			}
		}


		if (isBreaking) {
			if (blink == 12) {
				int count = 0;
				for (int i = 0; i < height - 2; i++) {
					if (DestroyLines[i] == 1) {
						count++;
					}
				}
				lines += count;
				if (count < 4) {
					score += count * 100;
				}
				else
				{
					score += count * 200;
				}

				blink = 0;
				isBreaking = 0;

				destroyLine();

				getGhost(falling_tetrominoes);

				PlaySound(TEXT("break.wav"), 0, SND_ASYNC);
			}
			else {
				for (int i = 0; i < contect_count; i++) {
					for (int y = lineBlocks[i * 2]; y <= lineBlocks[i * 2 + 1]; y++) {
						for (int x = 1; x < width - 1; x++) {
							if (blink % 3 == 0) {
								if (Color_array[x + y * width] == 0) {
									Color_array[x + y * width] = 15;
								}
								else {
									Color_array[x + y * width] = 0;
								}
							}
						}
					}
				}
			}blink++;
		}

		if (timer2 > 8000 / level && isBreaking == 0) {
			if (level >= 8) {
				int Hole = rand() % (width - 2) + 1;
				for (int y = 1; y < height; y++) {
					for (int x = 1; x < width - 1; x++) {
						int b = block_array[x + y * width];
						int c = Color_array[x + y * width];
						if (y == height - 1) {
							if (x == Hole) {
								b = 0;
								c = 0;
							}
							else
							{
								b = 1;
								c = 8;
							}
						}
						block_array[x + (y - 1) * width] = b;
						Color_array[x + (y - 1) * width] = c;
					}
				}
				if (falling_tetrominoes->isCollsion(0, 0, 0)) {
					falling_tetrominoes->y--;
					checkSinkY = 0;
				}
				else
				{
					getGhost(falling_tetrominoes);
				}
			}
			timer2 = 0;
		}timer2++;


		for (int i = 0; i < falling_tetrominoes->length && !isBreaking && game; i++) {
			int index = falling_tetrominoes->blocks[i];
			int x = falling_tetrominoes->x + index % falling_tetrominoes->size;
			int y = falling_tetrominoes->y + index / falling_tetrominoes->size + checkSinkY;
			if (!(x < 0 || x > width || y < 0 || y > height)) {
				if (displayBuffer[x + y * width] == 0) {
					displayBuffer[x + y * width] = 3;
					ColorBuffer[x + y * width] = falling_tetrominoes->color;
				}
			}

			y -= checkSinkY;
			if (!(x < 0 || x > width || y < 0 || y > height)) {
				displayBuffer[x + y * width] = 1;
				ColorBuffer[x + y * width] = falling_tetrominoes->color;
			}
		}


		gotoxy(width + 23, 5);
		printf("NEXT");
		gotoxy(13, 5);
		printf("HOLD");

		setColor(8);
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				if ((y == 0 || y == 5) || (x == 0 || x == 5)) {
					gotoxy(width + 23 + x, 7 + y);
					printf("▣");
					gotoxy(13 + x, 7 + y);
					printf("▣");
				}
				else {
					gotoxy(width + 23 + x, 7 + y);
					printf(" ");
					gotoxy(13 + x, 7 + y);
					printf(" ");
				}
			}
		}

		setColor(tetrominoes_array[nextTetro].color);
		for (int i = 0; i < tetrominoes_array[nextTetro].length; i++) {
			int index = tetrominoes_array[nextTetro].blocks[i];
			int x = index % tetrominoes_array[nextTetro].size;
			int y = index / tetrominoes_array[nextTetro].size;
			gotoxy(x + 24 + width + 2 - tetrominoes_array[nextTetro].size / 2, y + 8 + 2 - tetrominoes_array[nextTetro].size / 2);
			printf("▣");
		}

		if (holdTetro >= 0) {
			setColor(tetrominoes_array[holdTetro].color);
			for (int i = 0; i < tetrominoes_array[holdTetro].length; i++) {
				int index = tetrominoes_array[holdTetro].blocks[i];
				int x = index % tetrominoes_array[holdTetro].size;
				int y = index / tetrominoes_array[holdTetro].size;
				gotoxy(x + 14 + 2 - tetrominoes_array[holdTetro].size / 2, y + 8 + 2 - tetrominoes_array[holdTetro].size / 2);
				printf("▣");
			}
		}

		setColor(8);
		gotoxy(23 + width, 16);
		printf("LEVEL : %d", level);
		gotoxy(23 + width, 18);
		printf("SCORE : %d         ", score);
		gotoxy(23 + width, 20);
		printf("NEXT : %d", level_next);
		gotoxy(23 + width, 22);
		printf("LINES : %d", lines);

		draw(width, height, displayBuffer, block_array);

		Sleep(1);
	}

	PlaySound(TEXT("gameover.wav"), 0, SND_FILENAME | SNAPSHOT_POLICY_ALWAYS);

	return key;
}

int main()
{
	/* { move: <-, ->, rotate : up_arrow, s, drop : w, move down : down_arrow, hold : tap } */
	

	ifstream fin("highScore.txt");
	if (!fin) {
		printf("파일 열기를 실패...");
		return 0;
	}

	fin >> highScore;

	fin.close();

	system("tiTle TETRIS");

	cursorView();

	while (1) {
		initialize();
		
		int input;
		input = game();

		
		if (score > highScore) {
			ofstream fout("highScore.txt");
			if (!fout) {
				printf("파일 열기를 실패...");
				return 0;
			}

			fout << score;

			fout.close();
		}

		int i = 0;
		while (1) {	
			if (i % 10000 < 5000) {
				gotoxy(23 + width, 26);
				setColor(12);
				printf("  GAME OVER    \n\n\n\n\n");

				if (score > highScore) {
					gotoxy(23 + width, 24);
					setColor(3);
					printf("  NEW SCORE    \n\n\n\n\n");
				}
			}
			else
			{
				gotoxy(23 + width, 26);
				printf("               \n\n\n\n\n");
				gotoxy(23 + width, 24);
				printf("               \n\n\n\n\n");
			}
			if (i == 10000) {
				i = 0;
			}

			if (_kbhit()) {
				input = _getch();
			}
			else {
				input = 0;
			}

			if (input == '\r') {
				PlaySound(TEXT("drop.wav"), 0, SND_ASYNC);
				break;
			}

			i++;
		}

		if (score > highScore) {
			highScore = score;
		}

		setColor(8);
		system("cls");
	}

	return 0;
}

void draw(int w, int h, int* buffer, int* blocks) {
	for (int i = 0; i < w * h; i++) {
		if (i % w == 0)
			gotoxy(20, 1 + i / w); // real pos y -2
		if (Color_array[i] == 0) {
			setColor(ColorBuffer[i]);
		}
		else {
			setColor(Color_array[i]);
		}

		if (buffer[i] == 1)
			printf("▣");
		else if (blocks[i] == 1)
			printf("▣");
		else if (buffer[i] == 2)
			printf("▩");
		else if (buffer[i] == 3)
			printf("□");
		else if (blocks[i] == 2)
			printf("▦");
		else
			printf(" ");
	}
} //■

void checkLine()
{
	for (int y = 1; y < height - 1; y++) {
		int isLine = 1;
		for (int x = 1; x < width - 1; x++) {
			if (block_array[x + y * width] != 1) {
				isLine = 0;
				break;
			}
		}
		if (isLine) {
			DestroyLines[y - 1] = 1;
			isBreaking = 1;
		}
		else
		{
			DestroyLines[y - 1] = 0;
		}
	}

	contect_count = 0;
	int a = 0;
	int index = 0;
	for (int i = 0; i < height - 2; i++) {
		if (DestroyLines[i] != a) {
			a = DestroyLines[i];

			if (a == 0) {
				lineBlocks[index] = i;
			}
			else {
				lineBlocks[index] = i + 1;
				contect_count++;
			}

			index++;
		}
	}
	if (DestroyLines[height - 3] == 1) {
		lineBlocks[index] = height - 2;
	}

}

void destroyLine() {
	for (int i = 0; i < contect_count; i++) {
		for (int y = lineBlocks[i * 2]; y <= lineBlocks[i * 2 + 1]; y++) {
			for (int x = 1; x < width - 1; x++) {
				block_array[x + y * width] = 0;
				Color_array[x + y * width] = 0;
			}
		}
	}


	for (int i = 0; i < contect_count; i++) {
		int startY = lineBlocks[i * 2];
		int endY = lineBlocks[i * 2 + 1];
		for (int y = startY - 1; y > 0; y--) {
			for (int x = 1; x < width - 1; x++) {
				block_array[x + (endY + (y - startY + 1)) * width] = block_array[x + y * width];
				Color_array[x + (endY + (y - startY + 1)) * width] = Color_array[x + y * width];
			}
		}
	}

	for (int i = 0; i < height - 2; i++) {
		lineBlocks[i] = 0;
		DestroyLines[i] = 0;
	}
}

void getGhost(Tetrominoes* falling_tetrominoes) {
	checkSinkY = 0;
	for (int i = 0; i < height; i++) {
		if (falling_tetrominoes->isCollsion(0, i, 0)) {
			checkSinkY = i - 1;
			break;
		}
	}
}

void spawn(Tetrominoes* falling_tetrominoes, int* game) {
	int index = rand() % 7;
	int lastIndex = index;
	if (nextTetro == index && rand() % 10 != 0) {
		index = rand() % 6;
		if (index >= lastIndex)
			index++;
	}
	nextTetro = nextTetro % 7;
	nowTetro = nextTetro;
	falling_tetrominoes->build();
	falling_tetrominoes->clone(tetrominoes_array[nextTetro]);

	int up = 0;
	for (; up < height; up++) {
		if (falling_tetrominoes->isCollsion(0, -up, 0) == 0) {
			break;
		}
	}

	
	nextTetro = index;

	int notCollision = 0;
	if (up > 0 && spawnMoveable) {
		for (int i = width-1; i > 0 - 1; i--) {
			if (falling_tetrominoes->isCollsion(i - falling_tetrominoes->x, 0, 0) == 0) {
				notCollision = 1;
				falling_tetrominoes->x = i;
				break;
			}
		}
	}

	if (notCollision == 0 && up > 0) {
		falling_tetrominoes->y -= up;

		falling_tetrominoes->build();
		draw(width, height, displayBuffer, block_array);
		*game = 0;
		return;
	}
}