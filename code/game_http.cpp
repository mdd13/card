#pragma once

#include "common.cpp"
#include "game_mem.cpp"
#include "game_strings.cpp"

struct HttpOptions {
	char *method;
	char *path;
	char *protocol;
	char *user_agent;
	char *host;
};

#define SPACE_LEN 1
#define CLRF_LEN 2
#define CLRF "\r\n"

char *BuildHttpMessage(HttpOptions *options, const char *body) {
	usize method_len = StringLen(options->method);
	usize path_len = StringLen(options->path);
	usize protocol_len = StringLen(options->path);
	usize user_agent_len = StringLen(options->user_agent);
	usize host_len = StringLen(options->host);
	usize content_len = StringLen(body);

	usize body_len = StringLen(body);
	char body_len_str[10];
	itoa(body_len, body_len_str, 10);

	usize header_len = 0;
	header_len += method_len + SPACE_LEN +
		path_len + SPACE_LEN +
		protocol_len + SPACE_LEN + CLRF_LEN +
		StringLen("User-Agent: ") + user_agent_len + CLRF_LEN +
		StringLen("Host: ") + host_len + CLRF_LEN +
		StringLen("Connection: close") + CLRF_LEN +
		StringLen("Content-Length: ") + content_len + CLRF_LEN + CLRF_LEN;

	char *message = (char *)GameMemAlloc(header_len + body_len + 1);
	strcat(message, options->method); strcat(message, " ");
	strcat(message, options->path); strcat(message, " ");
	strcat(message, options->protocol); strcat(message, CLRF);

	strcat(message, "User-Agent: "); strcat(message, options->user_agent); strcat(message, CLRF);
	strcat(message, "Host: "); strcat(message, options->host); strcat(message, CLRF);
	strcat(message, "Connection: close"); strcat(message, CLRF);
	strcat(message, "Content-Length: "); strcat(message, body_len_str); strcat(message, CLRF);
	strcat(message, CLRF);

	strcat(message, body);

	SDL_Log("message:\n%s\n", message);

	return message;
}

struct HttpResponse {
	char *status;
	char *body;
};

HttpResponse *ParseHttpResponse(const char *msg) {
	i32 msg_len = StringLen(msg);

	i32 status_begin_idx = StringIndex(msg, " ") + 1;
	Assert(status_begin_idx != 0);

	i32 header_end_idx = StringIndex(msg, "\r\n\r\n");
	Assert(header_end_idx != -1);
	i32 body_begin_idx = header_end_idx + CLRF_LEN * 2;

	/// TODO: Valid the header

	/// Get the  string
	char *status = StringCopyPart(msg, status_begin_idx, status_begin_idx + 2);
	char *body = StringCopyPart(msg, body_begin_idx, msg_len - 1);

	HttpResponse *result = (HttpResponse *)GameMemAlloc(sizeof(HttpResponse));
	result->status = status;
	result->body = body;

	return result;
}

void HttpResponseFree(HttpResponse *response) {
	GameMemFree(response->status);
	GameMemFree(response->body);
	GameMemFree(response);
}


char *HttpRequest(const char *host, u16 port,
				  HttpOptions *options,
				  const char *body) {
	IPaddress address;
	i32 err = SDLNet_ResolveHost(&address, host, port);
	Assert(!err);

	TCPsocket sock = SDLNet_TCP_Open(&address);
	Assert(sock);

	char *message = BuildHttpMessage(options, body);
	i32 sent_len = SDLNet_TCP_Send(sock, message, StringLen(message));
	Assert(sent_len >= 0);

#define MAX_READ_LEN 1024
	usize result_len = 0;
	char *result = (char *)GameMemAlloc(result_len);
	i32 read_idx = 0;
	i32 n = -1;
	i32 timeout_ms = 0;
	while(timeout_ms < 500) {
		n = SDLNet_TCP_Recv(sock, result + read_idx, MAX_READ_LEN);
		Assert(n != -1);
		if (n == 0) {
			SDL_Delay(100);
			timeout_ms += 100;
		}
		
		result_len += n;		
		read_idx = result_len;
		if (n == MAX_READ_LEN) {
			GameMemRealloc(result, result_len, result_len + MAX_READ_LEN);
		}
	}
	// Free message after sent
	GameMemFree(message);

	SDLNet_TCP_Close(sock);

	result[result_len] = '\0';

	return result;
}


HttpResponse *HttpCardServerRequest(const char *body) {
	HttpOptions	options = {};
	options.method = (char *)"POST";
	options.path = (char *)"/";
	options.protocol = (char *)"HTTP/1.1";
	options.user_agent = (char *)"cardclient";
	options.host = (char *)"cardserver";

	char *res_msg = HttpRequest(HTTP_CARD_SERVER_HOST,
								HTTP_CARD_SERVER_PORT,
								&options,
								body);
	HttpResponse *result = ParseHttpResponse(res_msg);
	GameMemFree(res_msg);

	return result;
}
