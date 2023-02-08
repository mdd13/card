// #pragma once

#include "common.cpp"

#include "SDL2/SDL_net.h"


void Send(char *host, u16 port, char *body) {
	IPaddress address;
	i32 err = SDLNet_ResolveHost(&address, host, port);
	Assert(!err);

	TCPsocket sock = SDLNet_TCP_Open(&address);
	SDL_Log("%s\n", SDLNet_GetError());
	Assert(sock);

	i32 sent_len = -1;
	do {
		sent_len = SDLNet_TCP_Send(sock, body, strlen(body));
		SDL_Log("%d\n", sent_len);
		Assert(sent_len >= 0);
	} while (sent_len > 0);

	SDLNet_TCP_Close(sock);
}

int WinMain() {
	SDLNet_Init();
	Send((char *)"192.168.1.2", 8888, (char *)"Hello, world");

	return 0;
}
