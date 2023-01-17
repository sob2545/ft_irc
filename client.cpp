#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>


using namespace std;

void usage(char *argv){
    cout << "Usage : " << argv << " [ip] [port]" << endl;
    cout << "Example) " << argv << " 192.168.0.12 1234" << endl;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        cout << "Argument sucks" << endl;
        exit(1);
    }

    struct sockaddr_in addr_server = {};
    /*
struct sockaddr_in
{
	short    sin_family;          // 주소 체계: AF_INET(필수)
	u_short  sin_port;            // 16 비트 포트번호, network byte order
	struct   in_addr  sin_addr;   // 32 비트 IP 주소
	char     sin_zero[8];         // 전체 크기를 16 비트로 맞추기 위한 dummy
};*/

    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_family = AF_INET; // IPv4 통신을 하겠다
    addr_server.sin_addr.s_addr = inet_addr(argv[1]); // 첫번째 인자 IP 대입
    /*
     * inet_addr() -> ip주소를 string으로 바꿔주는 친구
     */
    addr_server.sin_port = htons(atoi(argv[2])); // 두번째 인자 PORT 대입
    /*
    host to network short -> 16비트 int를 network byte order로 바꿔준다. 네트워크로 데이터를 보낼 때 사용한다.
    왜 이 과정을 거치냐면, 컴퓨터마다 int를 메모리로 표현하는 방법이 다르기 때문이다. 네트워크 바이트로 보냄으로써 알아서 번역하게 해준다
    hton은 16비트 int(short)를 바꾸기 위한 함수다. long같은건 다른거 찾아라.

    sin_port는 소켓의 포트번호를 넣기 위한 공간이다. 포트번호는 16비트 unsigned int다. 0~ 65535
    근데 int의 바이트는 시스템마다 다 다르다. 그런데 전송되는 데이터는 항상 같아야 한다. 그래서 여기 담겨있는 포트 번호는 네트워크 바이트 오더로
    변환해야 한다.
    */

    char w_buff[256]; // 쓰기용 버퍼 선언
    char r_buff[256]; // 읽기용 버퍼 선언

    int sock_client = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성(IPv4, TCP, 기본프로토콜)
    if(sock_client == -1){
        cout << "socket error" << endl;
        close(sock_client);
        exit(1);
    }

    if(connect(sock_client, (struct sockaddr*) &addr_server, sizeof(addr_server)) == -1){ // 연결 요청
        cout << "connect error" << endl;
        close(sock_client);
        exit(1);
    }

    //================== listen =======================//

    while(1){
        memset(r_buff, 0, 256); // Clear read buffer
        cin.getline(w_buff, sizeof(w_buff)); // Get message to send from user
        if(strlen(w_buff)>255) break; // Avoid buffer overflow

        // Send message using send()
        int write_chk = send(sock_client, w_buff, strlen(w_buff), 0);
        if(write_chk == -1){
            cout << "send error" << endl;
            break;
        }

        // Receive message using recv()
        int read_chk = recv(sock_client, r_buff, sizeof(r_buff)-1, 0);
        if(read_chk == -1){
            cout << "recv error" << endl;
            break;
        }else{
            r_buff[read_chk] = '\0';
            cout << "Received message: " << r_buff << endl;
        }
    }
    close(sock_client); // 연결 종료
    return 0;
}