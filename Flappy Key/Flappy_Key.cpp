
#include "CUESDK.h"

#include <iostream>
#include <thread>
#include <string>
#include <Windows.h>


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

int main()
{
	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << std::endl;
		getchar();
		return -1;
	}
	auto bird_loc = CLK_F6;//Start "bird" at F6 key
	CorsairRequestControl(CAM_ExclusiveLightingControl);
	while (bird_loc != CLK_Escape)
	{
		auto bird = CorsairLedColor{ bird_loc,255,0,0 };
		CorsairSetLedsColors(1, &bird);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		bird = CorsairLedColor{ bird_loc,0,0,0 };
		CorsairSetLedsColors(1, &bird);

		if (GetAsyncKeyState(VK_ESCAPE))//If user presses ESC bird moves up
		{
			if (bird_loc < CLK_F11)
			{
				bird_loc = static_cast<CorsairLedId>(bird_loc + 1);
			}
			else if (bird_loc == CLK_F11)
			{
				bird_loc = static_cast<CorsairLedId>(CLK_F12);
			}
		}
		else
		{
			if(bird_loc == CLK_F12)
			{
				bird_loc = static_cast<CorsairLedId>(CLK_F11);
			}
			else
			{
				bird_loc = static_cast<CorsairLedId>(bird_loc - 1);
			}
		}
	}

	std::cout << "bird died :/";
	getchar();

	return 0;
}