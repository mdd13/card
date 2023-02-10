#pragma once

#include "common.cpp"
#include "game_mem.cpp"
#include "game_strings.cpp"

#define clrf "\r\n"
#define clrf_len 2
#define space_len 1

char *BuildHttpRequestMessage(const char *method,
							  const char *path,
							  const char *body) {
	usize content_len = StringLen(body);
	char *content_len_str = StringFromI32(content_len);

	usize hdr_len = 0;
	hdr_len += StringLen(method) + space_len +
		StringLen(path) + space_len +
		StringLen(HTTP_PROTOCOL) + clrf_len;
	hdr_len += StringLen("User-Agent: ") + StringLen(HTTP_USER_AGENT) + clrf_len;
	hdr_len += StringLen("Host: ") + StringLen(HTTP_CARD_SERVER_HOST) + clrf_len;
	hdr_len += StringLen("Connection: close") + clrf_len;
	hdr_len += StringLen("Content-Length: ") + StringLen(content_len_str) + clrf_len + clrf_len;

	char *result = (char *)GameMemAlloc(hdr_len + content_len + 1);
	StringConcat(result, method);
	StringConcat(result, " ");

	StringConcat(result, path);
	StringConcat(result, " ");

	StringConcat(result, HTTP_PROTOCOL);
	StringConcat(result, clrf);

	StringConcat(result, "User-Agent: ");
	StringConcat(result, HTTP_USER_AGENT);
	StringConcat(result, clrf);

	StringConcat(result, "Host: ");
	StringConcat(result, HTTP_CARD_SERVER_HOST);
	StringConcat(result, clrf);

	StringConcat(result, "Connection: close");
	StringConcat(result, clrf);

	StringConcat(result, "Content-Length: ");
	StringConcat(result, content_len_str);
	StringConcat(result, clrf);
	StringConcat(result, clrf);

	StringConcat(result, body);
	GameMemFree(content_len_str);

	return result;
}


struct HttpResponse {
	char *status;
	char *body;
};

void HttpResponseFree(HttpResponse *response) {
	GameMemFree(response->status);
	GameMemFree(response->body);
	GameMemFree(response);
}

HttpResponse *ParseHttpResponse(char *msg) {
	i32 msg_len = StringLen(msg);

	i32 status_begin_idx = StringIndex(msg, " ") + 1;
	Assert(status_begin_idx != 0);

	i32 header_end_idx = StringIndex(msg, clrf clrf);
	Assert(header_end_idx != -1);
	i32 body_begin_idx = header_end_idx + clrf_len + clrf_len;

	/// TODO: Valid the header

	/// Get the  string
	char *status = StringCopyPart(msg, status_begin_idx, status_begin_idx + 2);
	char *body = StringCopyPart(msg, body_begin_idx, msg_len - 1);

	HttpResponse *result = (HttpResponse *)GameMemAlloc(sizeof(HttpResponse));
	result->status = status;
	result->body = body;

	return result;
}

char *HttpSendMessage(const char *host, u16 port, const char *msg) {
	IPaddress address;
	i32 err = SDLNet_ResolveHost(&address, host, port);
	Assert(!err);

	TCPsocket sock = SDLNet_TCP_Open(&address);
	Assert(sock);

	// NOTE: Send message
	{
		i32 len = SDLNet_TCP_Send(sock, msg, StringLen(msg));
		Assert(len >= 0);
	}

	// NOTE: Receive message
#define RECV_MAX_LEN 1024
#define RECV_MAX_BUF 1024 + 1
	usize len = RECV_MAX_LEN;
	char *result = (char *)GameMemAlloc(RECV_MAX_BUF);

	i32 read_idx = 0;

	for(;;) {
		i32 n = SDLNet_TCP_Recv(sock, result + read_idx, RECV_MAX_LEN);
		if (n == 0 || n == -1) {
			break;
		}
		len += n;
		read_idx = len;
		if (n == RECV_MAX_LEN) {
			GameMemRealloc(result, len, len + RECV_MAX_BUF);
		}
	}

	SDLNet_TCP_Close(sock);
	result[len] = '\0';

	return result;
}

HttpResponse *HttpRequestCardServer(const char *method,
									const char *path,
									const char *body) {
	char *msg = BuildHttpRequestMessage(method, path, body);
	char *res_msg = HttpSendMessage(HTTP_CARD_SERVER_HOST, HTTP_CARD_SERVER_PORT, msg);
	HttpResponse *response = ParseHttpResponse(res_msg);
	GameMemFree(msg);
	GameMemFree(res_msg);

	return response;
}
