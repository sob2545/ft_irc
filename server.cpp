#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>
using namespace std;

void usage(char *argv){
    cout << "Usage : " << argv << " [port]" << endl;
    cout << "Example) " << argv << " 1234" << endl;
}

    /*
    How server work?

    1. socket()
    서버에서 제공하는 (TCP / UDP) 대화 프로토콜의 소켓 타입을 사용하는 함수
    
    2. bind()
    함수를 통해서 호스트 머신의 IP 주소와 포트를 지정하여 클라이언트가 접속할 수 있도록 함

    3. listen()
    listen()함수를 통해서 들어오는 접속을 읽음. 소켓에게 들어오는 접속의 최대 대기열의 수를 보고 기다릴 수 있게 함

    4. accept()
    accept() 함수를 통해서 들어온 접속을 허용함. 서버와 클라이언트가 서로 접속될 때까지 블락처리를 하고, 접속이 되면
    새로운 소켓 디스크립터를 통해 클라이언트와 소통을 할 수 있도록 만듬

    5. send()
    send() 함수와 recv() 함수를 통해 클라이언트가 새롭게 반환한 소켓 디스크립터로 대화를 하게됨
    이 함수들을 통해서 클라이언트와 서버가 서로 메세지를 보낼 수 있다
    
    6. close()
    이 함수를 통해서 클라이언트와의 통신을 종료함
    
    4-6을 반복하여 새로운 접속을 하고 서버는 accept()함수를 지속적으로 불러서 새로운 접속에 대한 핸들링을 처리함

    */

int main(int argc, char *argv[]){
    if(argc != 2){ // 인자가 2개가 아니면 usage 출력
        usage(argv[0]);
        return -1;
    }

    char buff[256]; // 읽기, 쓰기용 버퍼 선언

    struct sockaddr_in addr_server = {}; // 주소체계 구조체 선언
    struct sockaddr_in addr_client = {};
    socklen_t addr_client_len = sizeof(addr_client_len);
    /*
    socklen_t
    sockaddr 또는 sockaddr_in 구조체의 길이를 나타냄
    */

    memset(&addr_server, 0, sizeof(addr_server)); // 초기화
    addr_server.sin_family = AF_INET; // IPv4 인터넷 프로토콜
    addr_server.sin_port = htons(atoi(argv[1])); // 첫번째 인자 PORT 지정
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY); // Anybody
 

    int sock_server = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
    if(sock_server == -1){
        cout << "socket error" << endl;
        close(sock_server);
        exit(1);
    }


    if(bind(sock_server, (sockaddr*) &addr_server, sizeof(addr_server)) == -1){ // 주소 지정
        cout << "bind error" << endl;
        close(sock_server);
        exit(1);
    }

    if(listen(sock_server, 3) == -1){ // 연결 대기
        cout << "listen error" << endl;
        close(sock_server);
        exit(1);
    }

    int sock_client = accept(sock_server, (sockaddr*) &addr_client, &addr_client_len); // 연결 수락
    if(sock_client == -1){
        cout << "accept error" << endl;
        close(sock_server);
        exit(1);
    }

    while(1){
            memset(buff, 0, 256); // Clear buffer
        int read_chk = read(sock_client, buff, sizeof(buff)-1); // Read message from client
        if(read_chk == -1){
            cout << "read error" << endl;
            break;
        }
        buff[read_chk] = '\0';
        cout << "Received message: " << buff << endl;
        struct iovec iov[1];
        iov[0].iov_base = buff;
        iov[0].iov_len = strlen(buff);
        int write_chk = writev(sock_client, iov, 1); // Use writev() to send message back to client
        if(write_chk == -1){
            cout << "writev error" << endl;
            break;
        }
    } 
}