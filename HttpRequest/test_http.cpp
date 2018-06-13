#include HttpReq.h
#include <string.h>
int main(void)
{
    HttpRequest* Http;
    char http_return[4096] = {0};
    char http_msg[4096] = {0};
    strcpy(http_msg, "http://host address & message to sent");
/*
    if(Http->HttpGet("http://www.baidu.com", str)) {
        printf("%s\n", str);
    } else {
        printf("http://www.baidu.com HttpGet Fail!\n");
    }

    if(Http->HttpGet("127.0.0.1", str)) {
        printf("%s\n", str);
    } else {
        printf("127.0.0.1 HttpGet Fail!\n");
    } */

    if(Http->HttpGet(http_msg, http_return)){
        std::cout << http_return << std::endl;
    }

    free(http_return);
    free(http_msg);
    return 0;
}