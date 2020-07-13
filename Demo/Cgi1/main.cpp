#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>

#define TEST_FUNC  1

void main(int argc, char **argv)
{
#if (TEST_FUNC == 1)

    printf("Content-Type: text/html\n\n");
    printf("<html><head><title>CGI Demo</title></head>\n");
    printf("<body bgcolor=\"#000000\" text=\"#FFFFFF\">\n");
    printf("<br><center><h1>Hello World, CGI!</h1></center>\n");
    printf("</body></html>\n");

#elif (TEST_FUNC == 2)

    printf("Status: 300 test\nContent-Type: text/html\nContent-Length: 1\nContent:\n\n");
    printf("<html><head><title>CGI Demo</title></head>\n");
    printf("<body bgcolor=\"#000000\" text=\"#FFFFFF\">\n");
    printf("<br><center><h1>Hello World CGI!</h1></center>\n");
    printf("</body></html>\n");

#elif (TEST_FUNC == 3)

    //printf("Location: /index.htm\n\n");
    //printf("Content-Type: text/plain\nStatus: 300 test\n\n");
    printf("<html><head><title>CGI Demo</title></head>\n");
    printf("<body bgcolor=\"#000000\" text=\"#FFFFFF\">\n");
    for(int i = 0; i < 10; i++)
        printf("<br><center><h1>Hello World CGI!</h1></center>\n");
    printf("</body></html>\n");

#elif (TEST_FUNC == 4)

    printf("Content-Type: text/html\n\n");
    for(TCHAR **pVariable = _tenviron; *pVariable; pVariable++ )
        printf("%s\n", *pVariable);

    printf("argc=%d\n", argc);
    for(int i = 0; i < argc; i++)
        printf("argv[%d]=%s\n", i, argv[i]);
    char CurDir[300];
    ::GetCurrentDirectory(300, CurDir);
    printf("CurDir=%s\n", CurDir);

#else
    #error not code.
#endif;
}
/*
IIS 5.1对CGI程序的输出的处理规则：
1. 查找\r\n\r\n，如果没有找到，则输出502错误；如果找到，则对这个头作如下后续分析：
2. 如果发现Status行，则提取Status的内容，作为返回的HTTP/1.1行的内容；如果没有找到Status行，则置为200 OK
3. 如果发现头中有Location行，则忽略其它所有的（包括Status及后续内容）。完全用自己合成的内容"302 Object Moved"
4. 如果发现Conteng-Length行，取其值，若为0或取值失败，则取消后续内容的输出，只输出头
5. 对于其它所有不认识的行（包括Content-Type、Connection、Server、HTTP/1.1等），提取出来，全部放到头的最后
6. 忽略CGI中输出的Connection字段，全部都会输出Connection: close
7. CGI中的Status及Location等都不必写在第一行
8. 如果找到多个Status行（或Location行），则以最后一个为准
//Status在前Location在后，以Location为准；Location在前Status在后，以Status为准，会把Location当成一个普通行
*/