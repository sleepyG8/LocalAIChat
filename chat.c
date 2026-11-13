#include <Windows.h>
#include <stdio.h>

// Sleepy 2026 

// You can use this code in your projects but give credit 
// wher eit is do

// wrote this up in about an hour to communicate with 
// a local model over localhost

// I learned that foundry host a http server for you 
// and had to implement this.

// The main foundry api docs do no justice in desrcibing 
// the proper JSON format but this is correct. No APIKEY
// or TOKEN is needed unlike the docs suggest.

// I tested this with qwen and this was also my first 
// time ever messing with a local model. This will be added
// to Glyph as a AI assistant DLL to describe memory dumps

// no ai was used to write this or any other of my projects

#define model "qwen2.5-coder-1.5b-instruct-generic-gpu:4"               // will need to put your model name
#define portNum 59325                                                   // port number cmd: (foundry server status)
#define portchar "59325"

int main() {

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN dat;
    dat.sin_family = AF_INET;
    dat.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    dat.sin_port = htons(portNum);

    if (connect(sock, &dat, sizeof(SOCKADDR_IN)) == -1) {
        printf("Error connecting\n");
        return 1;
    }

    while(1) {

    unsigned char cmd[1024] = {0};
    printf("ai: ");
    fgets(&cmd, sizeof(cmd), stdin);

    if (strncmp(cmd, "/clear", 6) == 0) {
        printf("\033[2J");
        continue;
    }

    if (strncmp(cmd, "/exit", 5) == 0) {
        return 0;
    }

    printf("\n");
    cmd[strlen(cmd) - 1] = 00;

    unsigned char sendData[1028];
    snprintf(sendData, sizeof(sendData), "{\"model\":\"%s\",\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}]}", model, &cmd);

    const char string[4096];
    snprintf(string, sizeof(string),
    "POST /v1/chat/completions HTTP/1.1\r\n"
    "Host: 127.0.0.1:%lu\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: %lu\r\n"
    "\r\n"
    "%s", 
    portchar, strlen(sendData), sendData);

    if (send(sock, string, strlen(string), 0) == -1) {
        printf("Error sending\n");
        return 1;
    }

    unsigned char inBuff[0x1000] = {0};
    int size = recv(sock, &inBuff, sizeof(inBuff), 0);

    for (int i=0; i < size; i++) {
        if (strncmp(inBuff + i, "content\":\"", 10) == 0) {
        for (int j=0; ; j++) {
            if (inBuff[i+10+j] == '\\' && inBuff[i+10+j+1] == 'n') {
                printf("\n");
                i++;
                continue;
            }

            if (strncmp(inBuff+i+10+j, "\",\"", 3) == 0) break;
            printf("%c", inBuff[i+10+j]);
        }
        }
    }

    printf("\n");

    }

    return 0;
}
