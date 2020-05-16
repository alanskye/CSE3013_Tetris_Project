#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#ifndef _TETRIS_H_
#define _TETRIS_H_

#define and &&
#define or ||

#define WIDTH	        10
#define HEIGHT	        22
#define NOTHING	        0
#define QUIT	        'q'
#define SPEED_UP        'k'
#define SPEED_DN        'j'
#define NUM_OF_SHAPE	7
#define NUM_OF_ROTATE	4
#define BLOCK_HEIGHT	4
#define BLOCK_WIDTH	    4
#define BLOCK_NUM	    4

// menu number
#define MENU_PLAY       '1'
#define MENU_RANK       '2'
#define MENU_RECO       '3'
#define MENU_EXIT       '4'

// 사용자 이름의 길이
#define NAMELEN         16


const int FULL = (1 << WIDTH) - 1;
const int BIT_MAX = 1 << WIDTH;

const int block[NUM_OF_SHAPE][NUM_OF_ROTATE][4][2] = {
    { /* shape 0 */
        { /* rotation 0 */ {1, 0}, {1, 1}, {1, 2}, {1, 3} },
        { /* rotation 1 */ {0, 1}, {1, 1}, {2, 1}, {3, 1} },
        { /* rotation 2 */ {1, 0}, {1, 1}, {1, 2}, {1, 3} },
        { /* rotation 3 */ {0, 1}, {1, 1}, {2, 1}, {3, 1} },
    },
    { /* shape 1 */
        { /* rotation 0 */ {2, 1}, {2, 2}, {2, 3}, {3, 3} },
        { /* rotation 1 */ {1, 2}, {1, 3}, {2, 2}, {3, 2} },
        { /* rotation 2 */ {1, 1}, {2, 1}, {2, 2}, {2, 3} },
        { /* rotation 3 */ {1, 2}, {2, 2}, {3, 1}, {3, 2} },
    },
    { /* shape 2 */
        { /* rotation 0 */ {2, 1}, {2, 2}, {2, 3}, {3, 1} },
        { /* rotation 1 */ {1, 2}, {2, 2}, {3, 2}, {3, 3} },
        { /* rotation 2 */ {1, 3}, {2, 1}, {2, 2}, {2, 3} },
        { /* rotation 3 */ {1, 1}, {1, 2}, {2, 2}, {3, 2} },
    },
    { /* shape 3 */
        { /* rotation 0 */ {1, 1}, {2, 0}, {2, 1}, {2, 2} },
        { /* rotation 1 */ {1, 1}, {2, 0}, {2, 1}, {3, 1} },
        { /* rotation 2 */ {2, 0}, {2, 1}, {2, 2}, {3, 1} },
        { /* rotation 3 */ {1, 1}, {2, 1}, {2, 2}, {3, 1} },
    },
    { /* shape 4 */
        { /* rotation 0 */ {2, 1}, {2, 2}, {3, 1}, {3, 2} },
        { /* rotation 1 */ {2, 1}, {2, 2}, {3, 1}, {3, 2} },
        { /* rotation 2 */ {2, 1}, {2, 2}, {3, 1}, {3, 2} },
        { /* rotation 3 */ {2, 1}, {2, 2}, {3, 1}, {3, 2} },
    },
    { /* shape 5 */
        { /* rotation 0 */ {1, 2}, {1, 3}, {2, 1}, {2, 2} },
        { /* rotation 1 */ {1, 1}, {2, 1}, {2, 2}, {3, 2} },
        { /* rotation 2 */ {1, 2}, {1, 3}, {2, 1}, {2, 2} },
        { /* rotation 3 */ {1, 1}, {2, 1}, {2, 2}, {3, 2} },
    },
    { /* shape 6 */
        { /* rotation 0 */ {2, 1}, {2, 2}, {3, 2}, {3, 3} },
        { /* rotation 1 */ {1, 2}, {2, 1}, {2, 2}, {3, 1} },
        { /* rotation 2 */ {2, 1}, {2, 2}, {3, 2}, {3, 3} },
        { /* rotation 3 */ {1, 2}, {2, 1}, {2, 2}, {3, 1} },
    }
};

const int ROT[] = {1, 3, 3, 3, 0, 1, 1};

int timed_out;
int field[HEIGHT];
int nextBlock[BLOCK_NUM];
int blockID, blockX, blockY, blockR;
int shadowX, shadowY, shadowR;
int recoX, recoY, recoR;
int score;
bool gameOver = false;

// week 1
bool invalidBlock(const int f[HEIGHT], int x, int y);
bool checkToMove(const int f[HEIGHT], int id, int x, int y, int r);
void drawField();
void drawBlock(int id, int x, int y, int r, char tile);
void eraseBlock(int id, int x, int y, int r, char tile);
void drawShadow(int blockID, int blockX, int blockY, int blockRotate);
void drawBlockWithFeatures();
void drawBox(int x, int y, int h, int w);
void drawOutline();
void drawNextBlock();
void drawChange(int cmd, int oldX, int oldY, int oldR);
void printScore(int score);
int getCommand();
int processCommand(int cmd);
void blockDown(int sig);
int addBlockToField(int id, int x, int y, int r);
int deleteLine();
char menu();
void initTetris();
void play();

// week 2

void createRankList();
void rank();
void newRank(int score);
void writeRankFile();


// week 3
#define VISIBLE_BLOCK           4
#define Recommend_CANDMAX       120
#define Recommend_PRUNING       8
#define Recommend_FEATURES      11
#define Recommend_MIN           -987654321.0
typedef struct Recommend_node {
    double score;
    int x, y, r;
} Recommend_node;

enum Recommend_WEIGHTNAME {
    Recommend_LINECLEAR = 0,
    Recommend_TOUCHFLOOR,
    Recommend_TOUCHWALL,
    Recommend_TOUCHBLOCK,
    Recommend_MAXDIFF,
    Recommend_ROUGHNESS,
    Recommend_STDEV,
    Recommend_ABSMINMAX,
    Recommend_COLMAX,
    Recommend_COLSUM,
    Recommend_HOLEAFT,
};


const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, -1, 0, 1};

bool Recommend_giveup = false;

Recommend_node Recommend_NULL = {Recommend_MIN, 0, 0, 0};

void Recommend_addBlock(int f[HEIGHT], int id, int x, int y, int r);
void Recommend_eraseBlock(int f[HEIGHT], int id, int x, int y, int r);
int Recommend_compare(const void *a, const void *b);
bool drawRecommend();

void Recommend_dfs(Recommend_node *ret, const int f[HEIGHT], bool vis[HEIGHT + 1][WIDTH + 1], int *cands, int lv, int x, int y, int r);
Recommend_node* Recommend_test(const int f[HEIGHT], int lv, int *sz);
Recommend_node Recommend_search(const int f[HEIGHT], int lv);
double Recommend_evaluate(const int f[HEIGHT], int lv, int x, int y, int r);

// week 3 Recommend Play

bool Recommend_play = false;
int Recommend_interval = 150;
struct itimerval Recommend_it;

void Recommend_initTimer();
void Recommend_setTimer(int newInterval);

// training
typedef struct Train_gene {
    double weights[Recommend_FEATURES];
    int score;
} Train_gene;

#define Train_POPULATION 128
#define Train_MOVELIMIT 8192
#define Train_SUPCNT 32
#define Train_DROP 32

const int Train_MUTRATIO = 5;
const double Train_MUTAMT = 0.05;

int Train_move = 0;
int Train_epoch = 0;
int Train_currentGene;

bool train = false;
void Train_init();
void Train_manager();
void Train_play(int p);
void Train_crossOver();
void Train_mutation();

#endif
