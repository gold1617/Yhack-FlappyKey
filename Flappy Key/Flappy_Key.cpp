
#include "CUESDK.h"

#include <iostream>
#include <thread>
#include <string>
#include <Windows.h>
#include <vector>

const CorsairLedId bottom[15] = { CLK_LeftShift,CLK_LeftShift,CLK_LeftShift,CLK_Z,CLK_X,CLK_C,CLK_V,CLK_B,CLK_N,CLK_M,CLK_CommaAndLessThan,CLK_PeriodAndBiggerThan,CLK_SlashAndQuestionMark,CLK_RightShift};

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

CorsairLedId moveBird(CorsairLedId loc)
{
	if (GetAsyncKeyState(VK_LCONTROL))//If user presses ESC bird moves up
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

int main()
{
	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << std::endl;
		getchar();
		return -1;
	}

	auto bird_loc = CLK_F6;//Start "bird" at F6 key
	auto bird = CorsairLedColor{ bird_loc,255,0,0 };//make a red bird
	
	CorsairLedColor block;

	CorsairRequestControl(CAM_ExclusiveLightingControl);//request  that this application has exclusive control of LEDs

	while (bird_loc != CLK_Escape)
	{
		drawGrass();
		CorsairSetLedsColors(1, &bird);//draw bird
		generateLine();

		for (int i = 0; i < 12; i++)//Draw barrier
		{
			block = CorsairLedColor{ bottom[line.lit[i]], 0, 0, 255 };
			CorsairSetLedsColors(1, &block);
		}


		std::this_thread::sleep_for(std::chrono::seconds(1));

		bird = CorsairLedColor{ bird_loc,0,0,0 };//Turn off old bird location before moving
		CorsairSetLedsColors(1, &bird);

		for (int i = 0; i < 12; i++)//Turn off barrier
		{
			block = CorsairLedColor{ bottom[line.lit[i]], 0, 0, 0 };
			CorsairSetLedsColors(1, &block);
		}
		
		bird_loc = moveBird(bird_loc);//move bird
		bird = CorsairLedColor{ bird_loc,255,0,0 };
	}

	std::cout << "bird died :/";
	getchar();

	return 0;
}