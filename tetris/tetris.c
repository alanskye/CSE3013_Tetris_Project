#include "treap.h"
#include "tetris.h"

#define Recommend 1

static struct sigaction act, oact;

Train_gene genomes[Train_POPULATION];
double Recommend_weights[Recommend_FEATURES] = { // diff = after - before
    100,        // line clear
    10,       // touch floor
    0.75,            // touch wall
    2, // touch block
    -10,     // max diff, -
    -30,   // hole diff, -
    0.0,    // stdev;
    0.0,    // minmaxdiff
    0.0,
    0.0
};

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}


bool invalidBlock(const int f[HEIGHT], int x, int y) {
    // 조각을 놓기 위해서는 아래의 두 조건을 만족해야 한다
    // 1. 필드 안에 포함
    // 2. 필드 내부의 다른 블록과 겹치지 않음
    return x < 0 or x >= HEIGHT or y < 0 or y >= WIDTH or (f[x] & (1 << y));
}

bool checkToMove(const int f[HEIGHT], int id, int x, int y, int r) {
    // 블록을 구성하고 있는 4개의 조각에 대해서 필드에 놓을 수 있는지 확인
    for (int b = 0; b < 4; b++)
        if (invalidBlock(f, block[id][r][b][0] + x, block[id][r][b][1] + y))
            return false;
    return true;
}

void drawField() {
    for (int i = 0; i < HEIGHT; i++) {
        move(i + 1, 1);
        for (int j = 1; j < BIT_MAX; j <<= 1) {
            if (field[i] & j) {
                attron(A_REVERSE);
                printw(" ");
                attroff(A_REVERSE);
            }
            else printw(".");
        }
    }
}

void eraseBlock(int id, int x, int y, int r, char tile) {
    for (int b = 0; b < 4; b++) {
        move(x + block[id][r][b][0] + 1, y + block[id][r][b][1] + 1);
        printw("%c", tile);
    }
    move(HEIGHT, WIDTH + 10);
}

void drawBlock(int id, int x, int y, int r, char tile) {
    for (int b = 0; b < 4; b++) {
        move(x + block[id][r][b][0] + 1, y + block[id][r][b][1] + 1);
        attron(A_REVERSE);
        printw("%c", tile);
        attroff(A_REVERSE);
    }
    move(HEIGHT, WIDTH + 10);
}

void drawShadow(int id, int x, int y, int r) {
    // 한 칸씩 아래로 블록을 이동하며 가능한 위치인지 확인
    while (checkToMove(field, id, x + 1, y, r)) x++;
    drawBlock(id, x, y, r, '/');
    shadowX = x;
    shadowY = y;
    shadowR = r;
}

void drawBlockWithFeatures() {
    if (!Recommend_giveup)
        drawBlock(blockID, recoX, recoY, recoR, 'R');
    drawShadow(blockID, blockX, blockY, blockR);
    drawBlock(blockID, blockX, blockY, blockR, ' ');
}

void drawBox(int x, int y, int h, int w) {
    move(x, y);
    addch(ACS_ULCORNER);
    for (int j = 0; j < w; j++)
        addch(ACS_HLINE);
    addch(ACS_URCORNER);
    for (int i = 0; i < h; i++) {
        move(x + i + 1, y);
        addch(ACS_VLINE);
        move(x + i + 1, y + w + 1);
        addch(ACS_VLINE);
    }
    move(x + h + 1, y);
    addch(ACS_LLCORNER);
    for (int j = 0; j < w; j++)
        addch(ACS_HLINE);
    addch(ACS_LRCORNER);
}

void drawOutline() {	
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	drawBox(0, 0, HEIGHT, WIDTH);
    move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	drawBox(3 ,WIDTH + 10, 4, 8);
    drawBox(9, WIDTH + 10, 4, 8);
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16 ,WIDTH + 10);
	printw("SCORE");
	drawBox(17, WIDTH + 10, 1, 8);
}

void drawNextBlock() {
    drawBlock(nextBlock[1], 2, WIDTH + 12, 0, ' ');
    drawBlock(nextBlock[2], 8, WIDTH + 12, 0, ' ');
    move(HEIGHT, WIDTH + 10);
}


void drawChange(int cmd, int oldX, int oldY, int oldR) {
    eraseBlock(blockID, oldX, oldY, oldR, '.');
    eraseBlock(blockID, shadowX, shadowY, shadowR, '.');
    // 3.
    drawBlockWithFeatures();
}

void printScore(int score){
	move(18,WIDTH+11);
	printw("%8d",score);
}

int getCommand(){
    int command;
	command = wgetch(stdscr);
    switch(command){    
    case SPEED_UP:
        Recommend_interval = max(20, Recommend_interval - 20);
        Recommend_setTimer(Recommend_interval);
        command = NOTHING;
        break;
    case SPEED_DN:
        Recommend_interval = min(1000, Recommend_interval + 20);
        Recommend_setTimer(Recommend_interval);
        command = NOTHING;
        break;
    case KEY_UP:    break;
	case KEY_DOWN:  break;
	case KEY_LEFT:  break;
	case KEY_RIGHT: break;
	case ' ':       break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}

    if (Recommend_play)
        return command == QUIT ? QUIT : NOTHING;

	return command;
}

int processCommand(int command){
	int ret=1;
	int drawFlag=0;
    int oldX = blockX, oldY = blockY, oldR = blockR; 
    switch(command){
    case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if ((drawFlag = checkToMove(field, nextBlock[0], blockX, blockY, (blockR + 1) % 4)))
			blockR = (blockR + 1) % 4;
		break;
	case KEY_DOWN:
		if ((drawFlag = checkToMove(field, nextBlock[0], blockX + 1, blockY, blockR)))
			blockX++;
		break;
	case KEY_RIGHT:
		if ((drawFlag = checkToMove(field, nextBlock[0], blockX, blockY + 1, blockR)))
			blockY++;
		break;
	case KEY_LEFT:
		if ((drawFlag = checkToMove(field, nextBlock[0], blockX, blockY - 1, blockR)))
			blockY--;
        break;
    case ' ':
        while ((drawFlag = checkToMove(field, nextBlock[0], blockX + 1, blockY, blockR)))
            blockX++;
        drawFlag = true;
		break;
	default:
		break;
	}
	if (drawFlag) drawChange(command, oldX, oldY, oldR);
	if (command == ' ') blockDown(0);
    refresh();
    return ret;	
}

void blockDown(int sig){
	// 다음 blockDown을 위한 timer 초기화
    int id = nextBlock[0];
    // 더 이상 아래로 블록이 내려갈 수 없는 경우를 확인
    if (!checkToMove(field, id, blockX + 1, blockY, blockR)) {
        // 불가능하면서 블록이 맨 위에 있다면 게임 종료
        if (blockX == -1) {
            gameOver = true;
        }
        else {
            // recommend 삭제
            eraseBlock(nextBlock[0], recoX, recoY, recoR, '.');
            score += addBlockToField(id, blockX, blockY, blockR);
            score += deleteLine();
            
            // 다음 블록 생성
            printScore(score);
            
            // 다음 블록 그림 삭제
            eraseBlock(nextBlock[1], 2, WIDTH + 12, 0, ' ');
            eraseBlock(nextBlock[2], 8, WIDTH + 12, 0, ' ');
            // 새 블록 생성
            nextBlock[0] = nextBlock[1];
            nextBlock[1] = nextBlock[2];
            nextBlock[2] = rand() % 7;
            blockID = nextBlock[0];
            if (!train)
                drawNextBlock();
            // block 위치 초기화
            blockX = -1; blockY = WIDTH / 2 - 2; blockR = 0;
            shadowX = -1; shadowY = WIDTH / 2 - 2; shadowR = 0;

            // 새 블록의 recommend 구하기
            recoX = 0, recoY = WIDTH / 2 - 2, recoR = 0;
            if (Train_move >= Train_MOVELIMIT or !drawRecommend()) {
                Recommend_giveup = true;
                if (Recommend_play)
                    gameOver = true;
                move(HEIGHT + 3, 1);
                printw("Recommend System give up\n");
                refresh();
            }

            if (Recommend_play) {
                blockX = recoX;
                blockY = recoY;
                blockR = recoR;
                drawBlock(nextBlock[0], blockX, blockY, blockR, 'R');
            }
            else {
                drawBlockWithFeatures();
            }
        }
    }
    else {
        eraseBlock(id, blockX, blockY, blockR, '.');
        eraseBlock(id, shadowX, shadowY, shadowR, '.');
        blockX++;
        drawBlockWithFeatures();
    }
    refresh();
    timed_out = 0;
}

int addBlockToField(int id, int x, int y, int r) {
    int ret = 0;
    for (int b = 0; b < 4; b++) {
        int bx = x + block[id][r][b][0], by = y + block[id][r][b][1];
        // n번째 줄에 새로 놓인 블록의 위치 bit를 1로 토글
        field[bx] |= (1 << (by));
        // 추가하려는 블록이 필드의 바닥에 다았다면 점수 추가
        if (bx == HEIGHT - 1) ret++;
    }
    // 추가된 블록을 필드 상에 표시
    drawBlock(id, x, y, r, ' ');
    // 점수 반환
    return ret * 10;
}

int deleteLine() {
    int temp[HEIGHT] = {0}, d[HEIGHT] = {0};
    int sum = 0;
    for (int i = HEIGHT - 2; i >= 0; i--) {
        if (field[i + 1] == FULL) {
            field[i + 1] = 0;
            sum++;
        }
        d[i] = sum;
    }
    if (sum == 0) return 0;

    for (int i = HEIGHT - 1; i >= 0; i--)
        temp[i + d[i]] = field[i];
 
    // 3. memcpy
    memcpy(field, temp, sizeof(field));
    
    drawField();
    return (sum * sum) * 100;
}

char menu() {
    printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	printw("last Score = %d\n", score);
    return wgetch(stdscr);
}

void initTetris() {
    memset(field, 0, sizeof(field));
    timed_out = 0;
    if (train) {
        move(6, WIDTH + 21);
        printw("epoch = %d, g = %d\n", Train_epoch, Train_currentGene);
    }
            // 새 블록 생성
    nextBlock[0] = nextBlock[1];
    nextBlock[1] = nextBlock[2];
    nextBlock[2] = rand() % 7;

	blockID = nextBlock[0];
    blockX = shadowX = -1;
	blockY = shadowY = WIDTH / 2 - 2;
    blockR = shadowR = 0;
	score=0;	
	gameOver=0;
    Recommend_giveup = false;
	drawOutline();
	drawField();
#if Recommend
    drawRecommend();
#endif
    drawBlockWithFeatures();
	drawNextBlock();
    printScore(score);
    move(HEIGHT, WIDTH + 10);
    if (Recommend_play) {
        blockX = recoX;
        blockY = recoY;
        blockR = recoR;
        drawBlock(nextBlock[0], blockX, blockY, blockR, 'R');
    }
    refresh();
}

void play() {
	int command;
    clear();
    Recommend_setTimer(Recommend_interval);
    act.sa_handler = blockDown;
	sigaction(SIGALRM, &act, &oact);
    timed_out = 0;
    initTetris();
    if (train) {
        for (int g = 0; g < Recommend_FEATURES; g++) {
            move(HEIGHT + 11 + g, 0);
            printw("%d = %lf\n", g, Recommend_weights[g]);
        }
        refresh();
    }
    do {
        if (timed_out == 0) {
            if (Recommend_play) {
                setitimer(ITIMER_REAL, &Recommend_it, NULL);
            }
            else {
                alarm(1);
            }
            timed_out = 1;
        }
        if (!train) {
            int command = getCommand();
		    if(processCommand(command) == QUIT){
		    	alarm(0);
                timed_out = 0;
		    	drawBox(HEIGHT / 2 - 1,WIDTH / 2 - 5, 1, 10);
		    	move(HEIGHT / 2, WIDTH / 2 - 4);
		    	printw("Good-bye!!");
		    	refresh();
		    	getch();
		    	return;
		    }
        }
	} while(!gameOver);
	alarm(0);
    timed_out = 0;
	if (!train) getch();
	drawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");
	refresh(); 
    if (!train) {
	    getch();
	    newRank(score);
    }
    return;
}

// week 2

FILE *fp = NULL;
int recCount = 0;
bool flag1 = false;
Treap *root = NULL;


/* Treap Functions */
void Treap_record(const Treap *self) {
    if (self->left) Treap_record(self->left);
    fprintf(fp, "%s %d\n", self->str, self->key);
    if (self->right) Treap_record(self->right);
}

void Treap_searchName(const Treap *self, const char *str) {
    if (self->left) Treap_searchName(self->left, str);
    if (strncmp(str, self->str, NAMELEN) == 0) {
        printw("  %-*s | %9d\n", NAMELEN, self->str, self->key);
        flag1 = true;
    }
    if (self->right) Treap_searchName(self->right, str);
}

void Treap_serachRange(const Treap *self, const int x, const int y) {
    int leftSize = 0;
    if (self->left != NULL) leftSize = self->left->size;
    if (y <= leftSize) Treap_serachRange(self->left, x, y);
    else if (x > leftSize + 1) Treap_serachRange(self->right, x - (leftSize + 1), y - (leftSize + 1));
    else {
        if (x <= leftSize) Treap_serachRange(self->left, x, leftSize);
        printw("  %-*s | %9d\n", NAMELEN, self->str, self->key);
        if (y > leftSize + 1) Treap_serachRange(self->right, 1, y - (leftSize + 1));
    }
}

/* Rank Functions */
void createRankList() {
    fp = fopen("rank.txt", "r");
    if (fp == NULL) return;
    fscanf(fp, "%d", &recCount);
    char recName[NAMELEN];
    int recScore;
    for (int i = 0; i < recCount; i++) {
        fscanf(fp, "%s %d\n", recName, &recScore);
        root = Treap_insert(root, Treap_new(recScore, recName));
    }
}

char rankMenu() {
    clear();
    printw("1: list rank from x to y\n");
    printw("2: list rank by a specific name\n");
    printw("3: delete a specific rank\n");
    printw("4: return to main menu\n");
    refresh();
    return wgetch(stdscr);
}

void rank1() {
    const int INF = 987654321;
    int x = INF, y = INF;
    printw("X: "); scanw("%d", &x);
    printw("Y: "); scanw("%d", &y);
    if (x == INF) x = 1;
    if (y == INF) y = recCount;
    printw("       name        |    score     \n");
    printw("----------------------------------\n\n");
    if (x > recCount or x < 1 or x > y or root == NULL)
        printw("search failure: no rank in the list\n");
    else {
        y = min(recCount, y);
        Treap_serachRange(root, x, y); 
        refresh();
    }
    printw("Press any key to continue ... \n");
    wgetch(stdscr);
}

void rank2() {
    char recName[NAMELEN];
    printw("input the name: "); getnstr(recName, NAMELEN);
    printw("        name        |    score     \n");
    printw("-----------------------------------\n\n");
    flag1 = false;
    if (root != NULL)
        Treap_searchName(root, recName);
    if (!flag1)
        printw("search failure: no name in the list\n"); 
    printw("Press any key to continue ... \n");
    wgetch(stdscr);
}

void rank3() {
    int k = 0;
    printw("Input the rank: "); scanw("%d", &k);
    if (root != NULL and 1 <= k and k <= recCount) {
        root = Treap_eraseKth(root, k);
        printw("result: the rank is deleted\n");
        recCount--;
    }
    else 
        printw("search failure: the rank %d not in the list\n", k);
    printw("Press any key to continue ... \n");
    wgetch(stdscr);
}

void rank() {
    bool exit = false;
    while (!exit) {
        noecho();
        char cmd = rankMenu();
        echo();
        switch (cmd) {
            case '1': rank1();              break;
            case '2': rank2();              break;
            case '3': rank3();              break;
            case MENU_EXIT: exit = true;    break;
            default:                        break;
        }
    }
}

void newRank(int score) {
    clear();
    printw("Score = %d\n", score);
    printw("your name: ");
    echo();
    char recName[NAMELEN];
    getnstr(recName, NAMELEN);
    recCount++;
    root = Treap_insert(root, Treap_new(score, recName));
    noecho();
}

void writeRankFile() {
    if (fp != NULL)  fclose(fp); // rank.txt is closed //
    fp = fopen("rank.txt", "w");
    fprintf(fp, "%d\n", recCount); 
    if (root != NULL)
        Treap_record(root);
    fclose(fp);
}

// week 3

int Recommend_compare(const void *lhs, const void *rhs) {
    double a = *(const double*)lhs;
    double b = *(const double*)rhs;
    if (a < b) return 1;
    if (a > b) return -1;
    return 0;
}

bool clr = false;
int ymin, ymax;
bool drawRecommend() {
    // recommned block의 삭제는 점수에 추가한 직후에 한다
    if (field[2]) return false;
    clr = false;
    Recommend_node ideal = Recommend_search(field, 0);
    if (train) {
        Train_move++;
        move(HEIGHT + 10, 1);
        printw("move10 = %d\n", Train_move / 10);
    }
    if (ideal.score == Recommend_MIN) return false;
    
    recoX = ideal.x, recoY = ideal.y, recoR = ideal.r;
    // move(3, WIDTH + 20);
    // printw("predict = %lf\n", ideal.score);
    //move(4, WIDTH + 20);
    //if (clr) {
    //    printw("clr OK\n");
    //}
    //else {
    //    printw("X\n");
    //}
    return true;
    // drawBlock(nextBlock[0], recoX, recoY, recoR, 'R');
}


void Recommend_addBlock(int f[HEIGHT], int lv, int x, int y, int r) {
    int id = nextBlock[lv];
    for (int b = 0; b < 4; b++) {
        f[x + block[id][r][b][0]] |= (1 << (y + block[id][r][b][1]));
    }
}

void Recommend_eraseBlock(int f[HEIGHT], int lv, int x, int y, int r) {
    int id = nextBlock[lv];
    for (int b = 0; b < 4; b++)
        f[x + block[id][r][b][0]] &= ((1 << (y + block[id][r][b][1])) ^ -1);
}

int Recommend_holeCnt(const int f[HEIGHT]) {
    int h = 0;
    for (int j = 0; j < WIDTH; j++) {
        for (int i = HEIGHT - 1, cnt = 0; i >= 0; i--) {
            if (f[i] & (1 << j)) { // block
                h += cnt;
                cnt = 0;
            }
            else
                cnt++;
        }
    }
    return h;
}

void Recommend_stdev_minmax_cols(const int f[HEIGHT], double *stdev, int *absminmax, double *colmax, double *colsum) {
    int top[WIDTH] = {0};
    for (int j = 0; j < WIDTH; j++) {
        for (int i = 0; i < HEIGHT; i++) {
            if (f[i] & (1 << j)) {
                top[j] = HEIGHT - i;
                break;
            }
        }
    }
    double s = 0.0, mean = 0.0;
   int a = 100, b = -1;

    for (int j = 0; j < WIDTH; j++) {
        mean += top[j];
        a = min(a, top[j]);
        b = max(b, top[j]);
    }

    *colmax = b;
    *colsum = mean;

    mean /= WIDTH;
    for (int j = 0; j < WIDTH; j++)
        s += (top[j] - mean) * (top[j] - mean);
    *stdev = sqrt(s);
    *absminmax = abs(b - a);
}

double Recommend_evaluate(const int f[HEIGHT], int lv, int x, int y, int r) {
    int id = nextBlock[lv];
    double ret = 0.0;
    double vector[Recommend_FEATURES] = {0};
    int temp[HEIGHT] = {0}, d[HEIGHT] = {0}, after[HEIGHT] = {0};
    int befMax = 0, aftMax = 0, maxDiff;
    int befHole, aftHole, holeDiff;
    double stdev, colmax, colsum;
    int absminmax;

    befHole = Recommend_holeCnt(f);
    
    for (int b = 0; b < 4; b++) {
        for (int d = 0; d < 4; d++) {
            int i = x + block[id][r][b][0] + dx[d];
            int j = y + block[id][r][b][1] + dy[d];
            if (j < 0 or j >= WIDTH)
                vector[Recommend_TOUCHWALL] += 1.0;
            if (i >= HEIGHT) {
                vector[Recommend_TOUCHFLOOR] += 1.0;
                continue;
            }
            if (i >= 0 and f[i] & (1 << j))
                vector[Recommend_TOUCHBLOCK] += 1.0;
            
        }
    } 

    memcpy(temp, f, sizeof(temp));    
    Recommend_addBlock(temp, lv, x, y, r);
    int sum = 0;
    for (int i = HEIGHT - 2; i >= 0; i--) {
        if (temp[i + 1]) befMax = i;
        if (temp[i + 1] == FULL) {
            temp[i + 1] = 0;
            clr = true;
            sum++;
        }
        d[i] = sum;
    }
    
    for (int i = HEIGHT - 1; i >= 0; i--) {
        after[i + d[i]] = temp[i];
        if (temp[i]) aftMax = i + d[i];
    }
    Recommend_stdev_minmax_cols(f, &stdev, &absminmax, &colmax, &colsum);
    // diff = after - before
    aftHole = Recommend_holeCnt(after);
    holeDiff = aftHole - befHole;

    befMax = HEIGHT - befMax;
    aftMax = HEIGHT - aftMax;
    maxDiff = aftMax - befMax;

    vector[Recommend_LINECLEAR] += sum;
    vector[Recommend_MAXDIFF] += maxDiff;
    vector[Recommend_HOLEDIFF] += holeDiff;
    vector[Recommend_STDEV] += stdev;
    vector[Recommend_ABSMINMAX] += absminmax;
    vector[Recommend_COLMAX] += colmax;
    vector[Recommend_COLSUM] += colsum;
    for (int i = 0; i < Recommend_FEATURES; i++)
        ret += Recommend_weights[i] * vector[i]; 
    return ret;
}

void Recommend_dfs(Recommend_node *ret, const int f[HEIGHT], bool vis[HEIGHT + 1][WIDTH + 1], int *cands, int lv, int x, int y, int r) {
    int id = nextBlock[lv];
    if (vis[x + 1][y + 1]) return; // y can be -1
    vis[x + 1][y + 1] = true;
    if (!checkToMove(f, id, x + 1, y, r)) {
        ret[*cands].x = x; ret[*cands].y = y; ret[*cands].r = r;
        ret[*cands].score = Recommend_evaluate(f, lv, x, y, r);
        (*cands)++;
    }
    for (int k = 1; k < 4; k++) {
        int nx = x + dx[k], ny = y + dy[k];
        if (ny > -2 and checkToMove(f, id, nx, ny, r))
            Recommend_dfs(ret, f, vis, cands, lv, nx, ny, r);
    }
}

Recommend_node* Recommend_test(const int f[HEIGHT], int lv, int *sz) {
    int id = nextBlock[lv], cands = 0;
    if (f[2] > 0) return NULL; 
    Recommend_node *ret = (Recommend_node*)calloc(Recommend_CANDMAX, sizeof(Recommend_node));
    for (int r = 0; r <= ROT[id]; r++) {
        bool vis[HEIGHT + 1][WIDTH + 1] = {0};
        Recommend_dfs(ret, f, vis, &cands, lv, 0, WIDTH / 2 - 2, r);
    } 
    if (cands == 0) { 
        free(ret);
        return NULL;
    }
    qsort(ret, cands, sizeof(Recommend_node), Recommend_compare);
    *sz = cands = min(Recommend_PRUNING, cands);
    ret = (Recommend_node*)realloc(ret, cands * sizeof(Recommend_node));
    return ret;
}


Recommend_node Recommend_search(const int f[HEIGHT], int lv) {
    int id = nextBlock[lv];
    int temp[HEIGHT] = {0}, d[HEIGHT] = {0}, newF[HEIGHT] = {0};
    int candSize = 0, retIdx = 0;
    double nextScore = Recommend_MIN;
    Recommend_node ret = Recommend_NULL;
    Recommend_node *cand = Recommend_test(f, lv, &candSize);
     
    if (cand == NULL) return ret;

    if (lv + 1 == VISIBLE_BLOCK) {
        ret = cand[0];
        free(cand);
        return ret;
    }

    for (int i = 0; i < candSize; i++) {

        // init
        memcpy(temp, f, sizeof(temp));

        // add block to field
        Recommend_addBlock(temp, lv, cand[i].x, cand[i].y, cand[i].r);

        // delete line
        int sum = 0;
        for (int i = HEIGHT - 2; i >= 0; i--) {
            if (temp[i + 1] == FULL) {
                temp[i + 1] = 0;
                sum++;
            }
            d[i] = sum;
        }

        if (sum != 0) {
            for (int i = HEIGHT - 1; i >= 0; i--)
                newF[i + d[i]] = temp[i];
        }
        
        Recommend_node result = Recommend_search(newF, lv + 1);
        
        if (nextScore < result.score) {
            nextScore = result.score;
            retIdx = i;
        }
    }
    ret = cand[retIdx];
    if (nextScore != Recommend_MIN)
        ret.score += nextScore;
    free(cand);
    return ret;
}

void Recommend_initTimer() {
    
}

void Recommend_setTimer(int newInterval) {
    move(HEIGHT + 4, 1);
    printw("timer ok %d\n", newInterval);
    Recommend_interval = newInterval;
    Recommend_it.it_value.tv_sec = Recommend_interval / 1000;
    Recommend_it.it_value.tv_usec = (Recommend_interval * 1000) % 1000000;
    Recommend_it.it_interval = Recommend_it.it_value;
}


// train

int Train_compare(const void *a, const void *b) {
    const Train_gene *ta = (const Train_gene*)a;
    const Train_gene *tb = (const Train_gene*)b;
    if (ta->score < tb->score)
        return 1;
    if (ta->score > tb->score)
        return -1;
    return 0;
}

void Train_crossOver() {
    qsort(genomes, Train_POPULATION, sizeof(Train_gene), Train_compare);
    Train_gene newGenomes[Train_POPULATION];
    int id = 0;
    for (int p = 0; p < Train_POPULATION; p++) {
        Train_gene child;
        child.score = genomes[p].score;
        for (int g = 0; g < Recommend_FEATURES; g++) {
            if (rand() & 1) {
                int idx = rand() % Train_SUPCNT;
                child.weights[g] = genomes[idx].weights[g];
            }
            else
                child.weights[g] = 
                    genomes[rand() % (Train_POPULATION - Train_DROP)].weights[g];
        }
        newGenomes[p] = child;
    }
    // copy
    for (int p = 0; p < Train_POPULATION; p++)
        genomes[p] = newGenomes[p];
}

void Train_mutation() {
    for (int p = 0; p < Train_POPULATION; p++) {
        for (int g = 0; g < Recommend_FEATURES; g++) {
            if (rand() % 100 < Train_MUTRATIO) {
                double change = 
                    genomes[p].weights[g] * Train_MUTAMT * (rand() & 1) ? 1.0 : -1.0;
                genomes[p].weights[g] += change;
            }
        }
    }
}

void Train_init() {
    FILE *f = fopen("genesinit.txt", "r");
    for (int p = 0; p < Train_POPULATION; p++) {
        for (int g = 0; g < Recommend_FEATURES; g++) {
            fscanf(f, "%lf", &(genomes[p].weights[g]));
        }
    }
    fclose(f);
}

void Train_startFrom(int s) {
    char geneFile[16];
    sprintf(geneFile, "ep_%03d.txt", s);
    FILE *f = fopen(geneFile, "r");
    for (int p = 0; p < Train_POPULATION; p++) {
        for (int g = 0; g < Recommend_FEATURES; g++) {
            fscanf(f, "%lf", &(genomes[p].weights[g]));
        }
    }
    fclose(f);
}


void Train_write(int ep) {
    char geneFile[16];
    int maxScore = 0, minScore = 987654321;
    long long int avg = 0;
    sprintf(geneFile, "ep_%03d.txt", Train_epoch);
    FILE *f = fopen(geneFile, "w");
    for (int p = 0; p < Train_POPULATION; p++) {
        for (int g = 0; g < Recommend_FEATURES; g++)
            fprintf(f, "%lf ", genomes[p].weights[g]);
        fprintf(f, "\n");
        maxScore = max(genomes[p].score, maxScore);
        minScore = min(genomes[p].score, minScore);
        avg += genomes[p].score;
    }
    avg /= Train_POPULATION;
    fprintf(f, "%d %d %lld\n", maxScore, minScore, avg);
    fclose(f);
}

void Train_manager() {
    Recommend_interval = 2;
    //Train_init();
    Train_startFrom(60);
    for (Train_epoch = 61; Train_epoch <= 70; Train_epoch++) {
        Train_move = 0;
        for (int p = 0; p < Train_POPULATION; p++) {
            clear(); refresh();
            Train_currentGene = p;
            // give w
            for (int g = 0; g < Recommend_FEATURES; g++) {
                // Recommend_weights[g]
                Recommend_weights[g] = genomes[p].weights[g];
            }
            Train_move = 0;
            play();
            genomes[p].score = score;
        }
        Train_crossOver();
        Train_mutation();
        Train_write(Train_epoch);
    }
}

int main() {
    int exit = 0;
    createRankList();
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    srand((unsigned int)time(NULL));
    while (!exit) {
        clear(); refresh();
        gameOver = false;
        Recommend_play = false;
        train = false;
        Train_move = 0;
        switch (menu()) {
            case '5': 
                train = true;
                Recommend_play = true;
                Train_manager();
            break;
            case MENU_RECO: Recommend_play = true;
            case MENU_PLAY: play();     break;
            case MENU_RANK: rank();     break;    
            case MENU_EXIT: exit = 1;   break;
            default:                    break;
        }
    }
    endwin();
    system("clear");
    writeRankFile();
    if (root != NULL)
        Treap_freeAll(root);
    return 0;
}
