
#include "CUESDK.h"
#include "CUELFX/CUELFX.h"
#include "Shared/LFX.h"

#include <iostream>
#include <thread>
#include <string>
#include <Windows.h>
#include <vector>

const CorsairLedId bottom[15] = { CLK_LeftShift,CLK_LeftShift,CLK_LeftShift,CLK_Z,CLK_X,CLK_C,CLK_V,CLK_B,CLK_N,CLK_M,CLK_CommaAndLessThan,CLK_PeriodAndBiggerThan,CLK_SlashAndQuestionMark,CLK_RightShift,CLK_RightShift};
const CorsairLedId second[15] = { CLK_CapsLock,CLK_CapsLock,CLK_A,CLK_S,CLK_D,CLK_F,CLK_G,CLK_H,CLK_J,CLK_K,CLK_L,CLK_SemicolonAndColon,CLK_ApostropheAndDoubleQuote,CLK_Enter,CLK_Enter };
const CorsairLedId third[15] = { CLK_Tab,CLK_Tab,CLK_Q,CLK_W,CLK_E,CLK_R,CLK_T,CLK_Y,CLK_U,CLK_I,CLK_O,CLK_P,CLK_BracketLeft,CLK_BracketRight,CLK_Backslash };
const CorsairLedId fourth[15] = { CLK_GraveAccentAndTilde,CLK_1,CLK_2,CLK_3,CLK_4,CLK_5,CLK_6,CLK_7,CLK_8,CLK_9,CLK_0,CLK_MinusAndUnderscore,CLK_EqualsAndPlus,CLK_Backspace,CLK_Backslash };
const CorsairLedId fifth[15] = { static_cast<CorsairLedId>(-1),static_cast<CorsairLedId>(-1),CLK_F1,CLK_F2,CLK_F3,CLK_F4,CLK_F5,CLK_F6,CLK_F7,CLK_F8,static_cast<CorsairLedId>(-1),CLK_F9,CLK_F10,CLK_F11,CLK_F12 };

const CorsairLedId *cols[5] = { bottom,second,third,fourth,fifth };

CorsairLedId keypad[10] = { CLK_Keypad0, CLK_Keypad1, CLK_Keypad2, CLK_Keypad3, CLK_Keypad4, CLK_Keypad5, CLK_Keypad6, CLK_Keypad7, CLK_Keypad8, CLK_Keypad9 };

const CorsairLedId three[13] = { CLK_9, CLK_I, CLK_J, CLK_N, CLK_B, CLK_B, CLK_G, CLK_T, CLK_V, CLK_C,CLK_D,CLK_E,CLK_3 };
const CorsairLedId two[13] = { CLK_9,CLK_8,CLK_I,CLK_J,CLK_H,CLK_G,CLK_R,CLK_4,CLK_E,CLK_D,CLK_X,CLK_F,CLK_3 };
const CorsairLedId one[13] = { CLK_9, CLK_O, CLK_L, CLK_K, CLK_J, CLK_H, CLK_G, CLK_F, CLK_D, CLK_E, CLK_X, CLK_3, CLK_LeftAlt};

const CorsairLedId all[74] = { CLK_LeftCtrl,CLK_LeftGui,CLK_LeftAlt,CLK_Space,CLK_RightAlt,CLK_RightGui,CLK_Application,CLK_RightCtrl,
CLK_LeftShift,CLK_Z,CLK_X,CLK_C,CLK_V,CLK_B,CLK_N,CLK_M,CLK_CommaAndLessThan,CLK_PeriodAndBiggerThan,CLK_SlashAndQuestionMark,CLK_RightShift,
CLK_CapsLock, CLK_A, CLK_S, CLK_D, CLK_F, CLK_G, CLK_H, CLK_J, CLK_K, CLK_L, CLK_SemicolonAndColon, CLK_ApostropheAndDoubleQuote, CLK_Enter, 
CLK_Tab,CLK_Q,CLK_W,CLK_E,CLK_R,CLK_T,CLK_Y,CLK_U,CLK_I,CLK_O,CLK_P,CLK_BracketLeft,CLK_BracketRight,CLK_Backslash, CLK_GraveAccentAndTilde,
CLK_1,CLK_2,CLK_3,CLK_4,CLK_5,CLK_6,CLK_7,CLK_8,CLK_9,CLK_0,CLK_MinusAndUnderscore,CLK_EqualsAndPlus,CLK_Backspace, 
CLK_F1,CLK_F2,CLK_F3,CLK_F4,CLK_F5,CLK_F6,CLK_F7,CLK_F8, CLK_F9,CLK_F10,CLK_F11,CLK_F12,CLK_Escape };

int score;
std::chrono::steady_clock::time_point startPoint;

struct Line
{
	int col;
	int lit[12];
} line;

const char* toString(CorsairError error)
{
	switch (error) {
	case CE_Success:
		return "CE_Success";
	case CE_ServerNotFound:
		return "CE_ServerNotFound";
	case CE_NoControl:
		return "CE_NoControl";
	case CE_ProtocolHandshakeMissing:
		return "CE_ProtocolHandshakeMissing";
	case CE_IncompatibleProtocol:
		return "CE_IncompatibleProtocol";
	case CE_InvalidArguments:
		return "CE_InvalidArguments";
	default:
		return "unknown error";
	}
}

bool isAnyKeyDown() {
	bool res = false;
	for (auto i = 1; i < 255; i++) {
		res = res || GetAsyncKeyState(i);
	}
	return res;
}

CorsairLedId moveBird(CorsairLedId loc)
{
	if (isAnyKeyDown())//If user presses ESC bird moves up
	{
		if (loc < CLK_F11)
		{
			loc = static_cast<CorsairLedId>(loc + 1);
		}
		else if (loc == CLK_F11)
		{
			loc = static_cast<CorsairLedId>(CLK_F12);
		}
	}
	else
	{
		if (loc == CLK_F12)
		{
			loc = static_cast<CorsairLedId>(CLK_F11);
		}
		else
		{
			loc = static_cast<CorsairLedId>(loc - 1);
		}
	}
	return loc;
}

void drawGrass()
{
	auto curkey = CorsairLedColor{ CLK_Escape, 0,255,0};
	CorsairSetLedsColors(1,&curkey);
	curkey = CorsairLedColor{ CLK_GraveAccentAndTilde, 0 , 255, 0 };
	CorsairSetLedsColors(1,&curkey);
	curkey = CorsairLedColor{ CLK_CapsLock, 0 , 255, 0 };
	CorsairSetLedsColors(1, &curkey);
	curkey = CorsairLedColor{ CLK_LeftShift, 0 , 255, 0 };
	CorsairSetLedsColors(1, &curkey);
	curkey = CorsairLedColor{ CLK_LeftCtrl, 0 , 255, 0 };
	CorsairSetLedsColors(1, &curkey);
	curkey = CorsairLedColor{ CLK_Tab, 0 , 255, 0 };
	CorsairSetLedsColors(1, &curkey);
}

void generateLine()
{
	line.col = 0;
	srand(time(NULL));
	int lower = rand() % 6 + 3;
	int i;
	for (i = 0; i < lower; i++)
	{
		line.lit[i] = i;
	}
	for (int j = 14; i < 12; i++, j--)
	{
		line.lit[i] = j;
	}
}

void gameOver()
{
	CorsairLedColor cur;
	for (int i = 0; i < 74; i++)
	{
		cur = CorsairLedColor{ all[i],255,0,0 };
		CorsairSetLedsColors(1, &cur);
	}
	std::cout << "Game Over\nScore: " << score << "\n";
	getchar();
	exit(0);
}


void checkCollision(CorsairLedId bird_loc)
{
	if (bird_loc == CLK_Escape)
	{
		gameOver();
	}
	if (line.col == 4)
	{
		for (int i = 0; i < 12; i++)
		{
			if (fifth[line.lit[i]] == bird_loc)
			{
				gameOver();
			}
		}
	}
 }

void updateScore()
{
	CorsairLedColor k;
	for (int i = 0; i < 10; i++)
	{
		k = CorsairLedColor{keypad[i], 0, 0, 0};
		CorsairSetLedsColors(1,&k);
	}
	int tens, ones;
	tens = score / 10;
	ones = score % 10;

	if (ones == tens && tens > 0)
	{
		auto alternate = CUELFXCreateAlternatingRapidBlinkEffect ( 1, &(keypad[ones]), CorsairColor { 193,30,188 }, CorsairColor{ 218,145,19 } );
		auto offset = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startPoint).count();
		auto frame = CUELFXGetFrame(alternate->effectId, static_cast<int>(offset));

		if (frame && frame->ledsColors) 
		{
			auto res = CorsairSetLedsColors(frame->size, frame->ledsColors);
			CUELFXFreeFrame(frame);

			if (!res) 
			{
				std::cerr << "Failed to set led colors: " << toString(CorsairGetLastError()) << std::endl;
				return;
			}
		}
	}
	else
	{
		if (tens > 0)
		{
			k = CorsairLedColor{ keypad[tens],193,30,188 };
			CorsairSetLedsColors(1, &k);
			 
		}
		k = CorsairLedColor{ keypad[ones],218,145,19 };
		CorsairSetLedsColors(1, &k);
	}
}

void countdown()
{
	srand(time(NULL));
	int r = rand() % 255;
	srand(time(NULL));
	int g = rand() % 255;
	srand(time(NULL));
	int b = rand() % 255;
	CorsairLedColor k;
	for (int i = 0; i < 13.; i++)
	{
		k = CorsairLedColor{ three[i],r,g,b };
		CorsairSetLedsColors(1, &k);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	for (int i = 0; i < 13; i++)
	{
		k = CorsairLedColor{ three[i],0,0,0 };
		CorsairSetLedsColors(1, &k);
	}
	srand(time(NULL));
	r = rand() % 255;
	srand(time(NULL));
	g = rand() % 255;
	srand(time(NULL));
	b = rand() % 255;
	for (int i = 0; i < 13; i++)
	{
		k = CorsairLedColor{ two[i],r,g,b };
		CorsairSetLedsColors(1, &k);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	for (int i = 0; i < 13; i++)
	{
		k = CorsairLedColor{ two[i],0,0,0 };
		CorsairSetLedsColors(1, &k);
	}
	srand(time(NULL));
	r = rand() % 255;
	srand(time(NULL));
	g = rand() % 255;
	srand(time(NULL));
	b = rand() % 255;
	for (int i = 0; i < 12; i++)
	{
		k = CorsairLedColor{ one[i],r,g,b };
		CorsairSetLedsColors(1, &k);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(750)); 
	for (int i = 0; i < 13; i++)
	{
		k = CorsairLedColor{ one[i],0,0,0 };
		CorsairSetLedsColors(1, &k);
	}
}

int main()
{
	int frames = 1;
	startPoint = std::chrono::steady_clock::now();

	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << std::endl;
		getchar();
		return -1;
	}

	auto bird_loc = CLK_F6;//Start "bird" at F6 key
	auto bird = CorsairLedColor{ bird_loc,255,255,0 };//make a red bird
	
	CorsairLedColor block;
	generateLine();

	score = 0;

	CorsairRequestControl(CAM_ExclusiveLightingControl);//request  that this application has exclusive control of LEDs

	countdown();

	updateScore();
	while (true)
	{
		drawGrass();
		CorsairSetLedsColors(1, &bird);//draw bird
		
		if (line.col > 4)
		{
			generateLine();
			score++;
		}
		for (int i = 0; i < 12; i++)//Draw barrier
		{
			if (line.col < 5 && cols[line.col][line.lit[i]] != static_cast<CorsairLedId>(-1))
			{
				block = CorsairLedColor{ cols[line.col][line.lit[i]], 209, 19, 19 };
				CorsairSetLedsColors(1, &block);
			}
		}


		std::this_thread::sleep_for(std::chrono::milliseconds(100));//sleep to seperate frames

		bird = CorsairLedColor{ bird_loc,0,0,0 };//Turn off old bird location before moving
		CorsairSetLedsColors(1, &bird);

		bird_loc = moveBird(bird_loc);//move bird
		bird = CorsairLedColor{ bird_loc,255,255,0 };

		if (frames == 3)
		{
			for (int i = 0; i < 12; i++)//Turn off barrier
			{
				if (line.col < 5 && cols[line.col][i] != static_cast<CorsairLedId>(-1))
				{
					block = CorsairLedColor{ cols[line.col][line.lit[i]], 0, 0, 0 };
					CorsairSetLedsColors(1, &block);
				}
			}
			line.col++;
			frames = 1;
			checkCollision(bird_loc);
		}
		else
		{
			checkCollision(bird_loc);
			frames++;
		}
		updateScore();		
	}

	return 0;
}