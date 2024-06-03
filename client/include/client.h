#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "type.h"

const int MAX_SCORE = 4; // Item max score
const int SETTING_PERIOD = 20; //Boradcast & Item generation period
const int INITIAL_ITEM = 10; //Initial number of item
const int INITIAL_BOMB = 4; //The number of bomb for each user
const int SCORE_DEDUCTION = 2; //The amount of score deduction due to bomb

//섹션1 서버가 여러분에게 주는 구조체에요.

//여기서 row, col을 통해 상대방의 위치 정보를 알 수 있어요.
//만약 전략을 설정하는데 상대방의 점수와 trap 개수가 필요하다면 score, bomb을 통해 알 수 있어요.

void printMap(void* arg); 