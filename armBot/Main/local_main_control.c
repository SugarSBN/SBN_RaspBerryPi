
/* server.c */

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <stdio.h>
#include "wiringPi.h"
#include <wiringPiI2C.h> 
#include <unistd.h>
#include <termio.h>
#include <string.h>
#pragma pack(1)

typedef struct payload_t {
    uint32_t id;
    uint32_t counter;
    float temp;
} payload;

#pragma pack()

int createSocket(int port)
{
    int sock, err;
    struct sockaddr_in server;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("ERROR: Socket creation failed\n");
        exit(1);
    }
    printf("Socket created\n");

    bzero((char *) &server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if (bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("ERROR: Bind failed\n");
        exit(1);
    }
    printf("Bind done\n");

    listen(sock , 3);

    return sock;
}

void closeSocket(int sock)
{
    close(sock);
    return;
}

bool PCA9685Init();
void ResetPca9685();
void PCA9685SetPwmFreq(unsigned short freq);
void PCA9685SetPwm(unsigned char channel, unsigned short on, unsigned short value);
void SetServoPulse(unsigned char channel, unsigned short pulse);

#define PCA9685_ADDRESS			0x40			//pca9685地址
#define PCA9685_CLOCK_FREQ		25000000		//PWM频率25MHz
#define PCA9685_MODE1			0x00
#define PCA9685_MODE2			0x01
#define PCA9685_PRE_SCALE		0xFE
#define PCA9685_LED0_ON_L		0x06
#define PCA9685_LED0_ON_H		0x07
#define PCA9685_LED0_OFF_L		0x08
#define PCA9685_LED0_OFF_H		0x09
#define PCA9685_LED_SHIFT		4


bool WriteByte(int fd, unsigned char regAddr, unsigned char data);
unsigned char ReadByte(int fd, unsigned char regAddr);


bool PCA9685_initSuccess = false;
int PCA9685_fd = 0;

bool check(char *a, char *b){
	int len = strlen(b);
	for (int i = 0;i < len;i++)
		if (*(a + i) != * (b + i))	return 1;
	return 0;
}

int analy(char *a, int idx){
	int res = 0;
	if (idx == 0){
		for (int i = 0;i < strlen(a);i++){
			if (a[i] == '.')	return res;
			res = res * 10 + a[i] - '0';
		}
	}
	if (idx == 1){
		int st = 0;
		for (int i = 0;i < strlen(a);i++){
			if (a[i] == ','){
				st =  i + 1;
				break;
			}
		}
		for (int i = st;i < strlen(a);i++){
			if (a[i] == '.')	return res;
			res = res * 10 + a[i] - '0';
		}
	}
	if (idx == 2){
		int st = 0;
		for (int i = 0;i < strlen(a);i++){
			if (a[i] == ','){
				st =  i + 1;
				break;
			}
		}
		int st1 = 0;
		for (int i = st;i < strlen(a);i++){
			if (a[i] == ','){
				st1 =  i + 1;
				break;
			}
		}
		for (int i = st1;i < strlen(a);i++){
			if (a[i] == '.')	return res;
			res = res * 10 + a[i] - '0';
		}
		
	}
	return res;
}
const int R_MAX = 2200, R_MIN = 1300, R1_MAX = 2450, R1_MIN = 1500, R_R1_MIN = 2350;
int r = 1300;
int r1 = 2250;
int r2 = 1000;
void mainArmForward(){
		if (R_MIN <= r + 50 && r + 50 <= R_MAX && R_R1_MIN <= r + 50 + r1){
			r += 50;
			SetServoPulse(15, r);
		}	
}
void mainArmBackward(){
		if (R_MIN <= r - 50 && r - 50 <= R_MAX && R_R1_MIN <= r - 50 + r1){
			r -= 50;
			SetServoPulse(15, r);
		}		    
}
void subArmForward(int x){
		if (R1_MIN <= r1 - x && r1 - x <= R1_MAX && R_R1_MIN <= r + r1 - x){
			r1 -= x;
			SetServoPulse(7, r1);
		}	  
}
void subArmBackward(int x){
		if (R1_MIN <= r1 + x && r1 + x <= R1_MAX && R_R1_MIN <= r + r1 + x){
			r1 += x;
			SetServoPulse(7, r1);
		}   
}
void openClaw(){
		if (r2 <= 1950)	r2 += 50;
		SetServoPulse(0, r2);	
}
void closeClaw(){
		if (r2 >= 550)	r2 -= 50;
		SetServoPulse(0, r2);
}
void leftRotate(int x){
		SetServoPulse(8, 500);
		usleep(x * 1000);
		SetServoPulse(8, 1500);
}
void rightRotate(int x){
		SetServoPulse(8, 2500);
		usleep(x * 1000);
		SetServoPulse(8, 1500);
}
int main()
{
    int PORT = 2300;
    int BUFFSIZE = 512;
    char buff[BUFFSIZE];
    int ssock, csock;
    int nread;
    struct sockaddr_in client;
    int clilen = sizeof(client);

    ssock = createSocket(PORT);
    printf("Server listening on port %d\n", PORT);
    
    int PCA9865_channel = 15;
	PCA9685Init();
	PCA9685SetPwmFreq(50);
	SetServoPulse(8, 1500);
	SetServoPulse(15, r);
	SetServoPulse(7, r1);
	SetServoPulse(0, r2);
    goto label2;
label1:
    while (1)
    {
        
        csock = accept(ssock, (struct sockaddr *)&client, &clilen);
        if (csock < 0)
        {
            printf("Error: accept() failed\n");
            continue;
        }
	
        printf("Accepted connection from %s\n", inet_ntoa(client.sin_addr));
        bzero(buff, BUFFSIZE);
	buff[0] ='\0';
        while ((nread=read(csock, buff, BUFFSIZE)) > 0)
        {
            //printf("\nReceived %d bytes\n", nread);
	    printf("%s\n", buff);
	    if (!check(buff, "MainArmForward"))	mainArmForward();
	    if (!check(buff, "MainArmBackward")) mainArmBackward();
	    if (!check(buff, "SubArmForward")) subArmForward(50);
	    if (!check(buff, "SubArmBackward"))	subArmBackward(50);
	    if (!check(buff, "OpenClaw"))  openClaw();
	    if (!check(buff, "CloseClaw"))	closeClaw();
	    if (!check(buff, "LeftRotate"))	leftRotate(50);
	    if (!check(buff, "RightRotate"))	rightRotate(50);
		printf("%d %d\n", r, r1);
	}
        printf("Closing connection to client\n");
        printf("-------	---------------------\n");
        closeSocket(csock);
        
    }
label2:
    while (1)
    {
        
        csock = accept(ssock, (struct sockaddr *)&client, &clilen);
        if (csock < 0)
        {
            printf("Error: accept() failed\n");
            continue;
        }
	
        printf("Accepted connection from %s\n", inet_ntoa(client.sin_addr));
        bzero(buff, BUFFSIZE);
	buff[0] ='\0';
        while ((nread=read(csock, buff, BUFFSIZE)) > 0)
        {
            //printf("\nReceived %d bytes\n", nread);
	    printf("%s\n", buff);
	    int x = analy(buff, 0), y = analy(buff, 1), z = analy(buff, 2);
	    printf("%d %d %d\n", x, y, z);
	    if (y > 270 + 20){
		//for (int i = 1;270 + 50 * i <= y;i++)	rightRotate();
		rightRotate((y - 270) / 8);
	    }
	    if (y < 270 - 20){
		//for (int i = 1;y + 50 * i <= 270;i++)	leftRotate();
		leftRotate((270 - y) / 8);
	    }
	    if (x > 240 + 20){
		//for (int i = 1;240 + 70 * i <= x;i++)	subArmForward();
		subArmForward((x - 240) / 2);
	    }
	    if (x < 240 - 20){
		//for (int i = 1;x + 60 * i <= 240;i++)	subArmBackward();
		subArmBackward((240 - x) / 2);
	    }
	    
	}
        printf("Closing connection to client\n");
        printf("-------	---------------------\n");
        closeSocket(csock);
        
    }
    printf("bye");
    return 0;
}

bool PCA9685Init() 
{
	//初始化
	PCA9685_fd = wiringPiI2CSetup(PCA9685_ADDRESS);
	if (PCA9685_fd <= 0) 
		return false;
	PCA9685_initSuccess = true;
	ResetPca9685();
	return true;
}

void ResetPca9685() 
{
	if (true == PCA9685_initSuccess) 
	{
		//sleep mode, Low power mode. Oscillator off
		WriteByte(PCA9685_fd, PCA9685_MODE1, 0x00);
		WriteByte(PCA9685_fd, PCA9685_MODE2, 0x04);
		usleep(1000);
		//Delay Time is 0, means it always turn into high at the begin
		WriteByte(PCA9685_fd, PCA9685_LED0_ON_L + PCA9685_LED_SHIFT * 0, 0);
		WriteByte(PCA9685_fd, PCA9685_LED0_ON_H + PCA9685_LED_SHIFT * 0, 0);
		WriteByte(PCA9685_fd, PCA9685_LED0_ON_L + PCA9685_LED_SHIFT * 7, 0);
		WriteByte(PCA9685_fd, PCA9685_LED0_ON_H + PCA9685_LED_SHIFT * 7, 0);
		WriteByte(PCA9685_fd, PCA9685_LED0_ON_L + PCA9685_LED_SHIFT * 8, 0);
		WriteByte(PCA9685_fd, PCA9685_LED0_ON_H + PCA9685_LED_SHIFT * 8, 0);
		WriteByte(PCA9685_fd, PCA9685_LED0_ON_L + PCA9685_LED_SHIFT * 15, 0);
		WriteByte(PCA9685_fd, PCA9685_LED0_ON_H + PCA9685_LED_SHIFT * 15, 0);
		usleep(1000);
	}
	else 
	{
		printf("pca9685 doesn't init\n");
	}
}

void PCA9685SetPwmFreq(unsigned short freq) 
{ //设置频率
	unsigned char preScale = (PCA9685_CLOCK_FREQ / 4096 / freq) - 1;
	unsigned char oldMode = 0;
	printf("set PWM frequency to %d HZ\n",freq);
	//read old mode
	oldMode = ReadByte(PCA9685_fd, PCA9685_MODE1);
	//setup sleep mode, Low power mode. Oscillator off (bit4: 1-sleep, 0-normal)
	WriteByte(PCA9685_fd, PCA9685_MODE1, (oldMode & 0x7F) | 0x10);
	//set freq
	WriteByte(PCA9685_fd, PCA9685_PRE_SCALE, preScale);
	//setup normal mode (bit4: 1-sleep, 0-normal)
	WriteByte(PCA9685_fd, PCA9685_MODE1, oldMode);
	usleep(1000); // >500us
	//setup restart (bit7: 1- enable, 0-disable)
	WriteByte(PCA9685_fd, PCA9685_MODE1, oldMode | 0x80);
	usleep(1000); // >500us
}

void PCA9685SetPwm(unsigned char channel, unsigned short on, unsigned short value)
{
	//设置各个通道的PWM
	if (!PCA9685_initSuccess) 
	{
		printf("Set Pwm failure!\n");
		return;
	}

	WriteByte(PCA9685_fd, PCA9685_LED0_ON_L + PCA9685_LED_SHIFT * channel, on & 0xFF);
	WriteByte(PCA9685_fd, PCA9685_LED0_ON_H + PCA9685_LED_SHIFT * channel, on >> 8);
	WriteByte(PCA9685_fd, PCA9685_LED0_OFF_L + PCA9685_LED_SHIFT * channel, value & 0xFF);
	WriteByte(PCA9685_fd, PCA9685_LED0_OFF_H + PCA9685_LED_SHIFT * channel, value >> 8);
}


void SetServoPulse(unsigned char channel, unsigned short pulse)
{
	pulse = pulse * 4096 / 20000;
	PCA9685SetPwm(channel, 0, pulse);
}







bool WriteByte(int fd, unsigned char regAddr, unsigned char data) 
{
	if (wiringPiI2CWriteReg8(fd, regAddr, data) < 0)
		return -1;
	return 0;
}



unsigned char ReadByte(int fd, unsigned char regAddr) 
{
	unsigned char data; // `data` will store the register data
	data = wiringPiI2CReadReg8(fd, regAddr);
	if (data < 0)
		return -1;
	return data;
}



