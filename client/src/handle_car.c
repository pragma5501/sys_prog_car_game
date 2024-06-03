#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <signal.h>
#include "handle_car.h"

#define DEVICE_ADDR 0x16
#define I2C_BUS "/dev/i2c-1"

typedef uint8_t u8;

#define ON 1
#define OFF 0

int fd; 
const static u8 PIN_IRs[4] = {2, 3, 0, 7};
u8 IR_data = 0;
u8 IR_data_arr[4] = {0,};

u8 repeat_count = 0;
Direction current_direction = 0;

int current_position[2] = {0, 0};

static int** best_path;


void setup_IR()
{
    for (int i = 0; i < 4; i++) {
        pinMode(PIN_IRs[i], INPUT);
    }
}

void setup_I2C() {
    if ((fd = open(I2C_BUS, O_RDWR)) < 0) 
    {
        exit(1);
    }
    if (ioctl(fd, I2C_SLAVE, DEVICE_ADDR) < 0) {
        exit(1);
    }
}

void write_array(int reg, u8 *data, int length) 
{
        u8 *arr = (u8*)malloc(sizeof(u8) * length + 1);
        arr[0] = reg;

        for (int i = 0; i < length; i++) {
                arr[i + 1] = data[i];
        }

        if (write(fd, arr, length + 1) != length + 1) {
                printf("write_array I2C error\n");
        }
        free(arr);
}

void write_u8(int reg, u8 data) 
{
    u8 arr[2] = {reg, data};

    if (write(fd, arr, 2) != 2) {
        printf("write err\n");
    }
}

void request_car(u8 dir_l, u8 speed_l, u8 dir_r, u8 speed_r) 
{
        u8 data[4] = {dir_l, speed_l, dir_r, speed_r};
        write_array(0x01, data, 4);
}


void read_IR()
{
	IR_data = 0;
    for (int i = 0; i < 4; i++) {
        IR_data += digitalRead(PIN_IRs[i]) << (3-i);

        IR_data_arr[i] = digitalRead(PIN_IRs[i]);
        printf("Sensor Value %d : %d\n", i, IR_data_arr[i]);
	}
	printf("READ IR_data : 0x%x\n", IR_data);
}

void car_forward(u8 speed1, u8 speed2) 
{
    request_car(ON, speed1, ON, speed2);
}

void car_stop() 
{
        write_u8(0x02, 0x00);
}

void handler(int sig){
    car_stop();

    exit(0);
}

#define BLACK 0
#define WHITE 1

#define HYPER_SPEED	100
#define HIGH_SPEED	 50
#define LOW_SPEED	 30
#define ZERO_SPEED	 0

u8 right_speed = 0;
u8 left_speed = 0;
#define WEIGHT_MID  50
#define WEIGHT_SIDE 80

void proc_pattern_new()
{
	right_speed = 0;
	left_speed = 0;
	if (IR_data_arr[0] == BLACK) right_speed += WEIGHT_SIDE;
	if (IR_data_arr[1] == BLACK) right_speed += WEIGHT_MID;
	if (IR_data_arr[2] == BLACK) left_speed  += WEIGHT_MID;
	if (IR_data_arr[3] == BLACK) left_speed  += WEIGHT_SIDE;

        printf("left speed : %d\n", left_speed);
        printf("right speed : %d\n", right_speed);

        if (right_speed + left_speed == 0) {
                printf("car back\n");
                car_forward(HIGH_SPEED, HIGH_SPEED);
                //usleep(1000 * 200);
                return ;
        }

        // if (right_speed + left_speed == (WEIGHT_MID + WEIGHT_SIDE) * 2) car_forward(HIGH_SPEED, HIGH_SPEED);

        right_speed = (right_speed < left_speed) ? 0 : right_speed;
        left_speed  = (left_speed  < right_speed) ? 0 : left_speed;
        
        car_forward(left_speed, right_speed);

        if (right_speed == (WEIGHT_MID + WEIGHT_SIDE) || left_speed == (WEIGHT_MID + WEIGHT_SIDE)){
            if (current_direction == UP){
                if (best_path[repeat_count + 1][0] - best_path[repeat_count][0] == 1){
                    current_direction = RIGHT;
                    car_forward(0, WEIGHT_MID + WEIGHT_SIDE);
                    usleep(1000 * 1100);

                }
                if (best_path[repeat_count + 1][0] - best_path[repeat_count][0] == -1){
                    current_direction = LEFT;
                    car_forward(WEIGHT_MID + WEIGHT_SIDE, 0);
                    usleep(1000 * 1100);
                }
                if (best_path[repeat_count + 1][1] - best_path[repeat_count][1] == 1){
                    current_direction = UP;
                    car_forward(50, 50);
                    usleep(1000 * 400);

                }
            }
            if (current_direction == DOWN){
                if (best_path[repeat_count + 1][0] - best_path[repeat_count][0] == 1){
                    current_direction = RIGHT;
                    car_forward(WEIGHT_MID + WEIGHT_SIDE, 0);
                    usleep(1000 * 1100);

                }
                if (best_path[repeat_count + 1][0] - best_path[repeat_count][0] == -1){
                    current_direction = LEFT;
                    car_forward(0, WEIGHT_MID + WEIGHT_SIDE);
                    usleep(1000 * 1100);
  
                }
                if (best_path[repeat_count + 1][1] - best_path[repeat_count][1] == -1){
                    current_direction = DOWN;
                    car_forward(50, 50);
                    usleep(1000 * 400);

                }

            }
            if (current_direction == RIGHT){
                if (best_path[repeat_count + 1][0] - best_path[repeat_count][0] == 1){
                    current_direction = RIGHT;
                    car_forward(50, 50);
                    usleep(1000 * 400);

                }
                if (best_path[repeat_count + 1][1] - best_path[repeat_count][1] == 1){
                    current_direction = UP;
                    car_forward(WEIGHT_MID + WEIGHT_SIDE, 0);
                    usleep(1000 * 1100);

                }
                if (best_path[repeat_count + 1][1] - best_path[repeat_count][1] == -1){
                    current_direction = DOWN;
                    car_forward(0, WEIGHT_MID + WEIGHT_SIDE);
                    usleep(1000 * 1100);

                }
            }
            if (current_direction == LEFT){
                if (best_path[repeat_count + 1][0] - best_path[repeat_count][0] == -1){
                    current_direction = LEFT;
                    car_forward(50, 50);
                    usleep(1000 * 400);

                }
                if (best_path[repeat_count + 1][1] - best_path[repeat_count][1] == 1){
                    current_direction = UP;
                    car_forward(0, WEIGHT_MID + WEIGHT_SIDE);
                    usleep(1000 * 1100);

                }
                if (best_path[repeat_count + 1][1] - best_path[repeat_count][1] == -1){
                    current_direction = DOWN;
                    car_forward(WEIGHT_MID + WEIGHT_SIDE, 0);
                    usleep(1000 * 1100);

                }
            }
            repeat_count++;
            return;
        }

        if (right_speed == (WEIGHT_MID + WEIGHT_SIDE)) {
                usleep(1000 * 1100);
        }
        if (left_speed == (WEIGHT_MID + WEIGHT_SIDE)) {
                usleep(1000 * 1100);
        }
	if (right_speed == (WEIGHT_SIDE) || left_speed == (WEIGHT_SIDE)){
                usleep(1000 * 200);
        }

}

/*
void* car_run(void* arg)
{
    if (repeat_count == 4){
        // 여기서 정지
        repeat_count = 0;
        dgist = NULL; // dgist 서버에서 받아오기
        int start_row = best_path[4][0], start_col = best_path[4][1];
        int max_score_obtained = getMaxScore(dgist, start_row, start_col);
    }
    usleep(1000 * 10);
    read_IR();
    proc_pattern_new();
}*/

void* handle_car (void* arg)
{
    signal(SIGINT, handler);

    if (wiringPiSetup() == -1) {
        printf("Unable to initialize WiringPi library\n");
        return (void*)1;
    }

    setup_IR();
    setup_I2C(); 
    car_stop();

    current_direction = UP;

    best_path = (int**)malloc(sizeof(int*) * 5);
    for (int i = 0; i < 2; i++) {
        best_path[i] = (int*)malloc(sizeof(int) * 2);
    }
    
    MAP_t* dgist = (MAP_t*)arg; // dgist 서버에서 받아오기
    int start_row = 0, start_col = 0; // 시작 지점 설정
    int max_score_obtained = getMaxScore(dgist, start_row, start_col, best_path);
    
    while(1) {
        if (repeat_count == 4){
            // 여기서 정지
            repeat_count = 0;
            dgist = NULL;
            int start_row = best_path[4][0], start_col = best_path[4][1];
            max_score_obtained = getMaxScore(dgist, start_row, start_col, best_path);
            
        }
        usleep(1000 * 10);
        read_IR();
        proc_pattern_new();
    }

    close(fd);
    return (void*)0;
}

