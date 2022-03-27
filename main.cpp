#include "lib.cpp"
#include "config.cpp"


int ScreenBG[COL][ROW]; 				// ScreenBG 		0
int ScreenLayout[COL][ROW];			// ScreenLayout 	1


int BlockMatrix[4][4];

//Shapes
int ShapeI[4][4] = {
	0,1,0,0,
	0,1,0,0,
	0,1,0,0,
	0,1,0,0
};
int ShapeT[4][4] = {
	0,1,0,0,
	0,1,1,0,
	0,1,0,0,
	0,0,0,0
};
int ShapeL[4][4] = {
	0,0,1,0,
	1,1,1,0,
	0,0,0,0,
	0,0,0,0
};
int ShapeL2[4][4] = {
	1,0,0,0,
	1,1,1,0,
	0,0,0,0,
	0,0,0,0
};
int ShapeS[4][4] = {
	0,1,1,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0
};
int ShapeS2[4][4] = {
	1,1,0,0,
	0,1,1,0,
	0,0,0,0,
	0,0,0,0
};
int ShapeO[4][4] = {
	1,1,0,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0
};

// Bitmaps
void  *bmBlueBrick;
void  *bmRedBrick;
void  *bmGreenBrick;
void  *bmYellowBrick;
void  *bmPurpleBrick;
void  *bmBlankBrick;

void  *bmScore;
void  *bmSpeed;
void  *bmLevel;
void  *bmLinesCleared;
void  *bmScreen;

int CurrentShape;
int NextShape;
int NextColor;
int LinesCleared = 0;
int Level = 1;
int LineScore = 100;
int Speed = 95;
int BlockX = 7;
int BlockY = 0;
bool GameOver = false;
char lineScore[40];
char lineSpeed[40];
char lineLevel[40];
char lineLinesCleared[40];
unsigned long Score = 0;

void Brick(int x, int y, int Color);
void GetImages();
void InitScreenLayout();
void SplashScreen();
void SetCurrentShape(int Shape, int Color);
int  GetRandomColor();
int  GetRandomShape();
void DrawBox(int x1, int y1, int x2, int y2, int Color1, int Color2, int Color3);
void DisplayUI();
void PrintInfo();
void PrintLinesCleared();
void DisplayNextShape();

int MoveBlock(int Direction);
int CheckMove(int Direction);
int MoveBlock(int Direction);
void UpdateScreen();
void DisplayBlock(int Bx, int By);
void GetNextBlock();
void CheckLine();
void RemoveLine(int LineNumber);
void Rotate();

int main() {
	char key;
	int scanCode;

	int count = 0;

	initwindow(750, 600, "Tetris LK");
	cleardevice();

	// init Layout
	InitScreenLayout();

	// create img box
	GetImages();

	cleardevice();
	// Splash Screen Game  
	SplashScreen();

	cleardevice();
	//
	SetCurrentShape(GetRandomShape(), GetRandomColor());

	NextShape = GetRandomShape();
	NextColor = GetRandomColor();

	// User Interface
	DisplayUI();
	DisplayNextShape();

	MoveBlock(REFRESH);

	while(kbhit()) getch(); 

	// game run
	while(!GameOver) {
		count ++;
		if(count >= Speed ) {
			count = 0;
			MoveBlock(DOWN);
		}
		if(kbhit()) {
			key = getch();
			if(key == 0) {
				scanCode = getch();
				cout<<"\t scancode :"<<scanCode<<endl;
				switch(scanCode) {
					case KEY_UP:
						Rotate();
						MoveBlock(DOWN);
						break;
					case KEY_LEFT:
						MoveBlock(LEFT);
						break;
					case KEY_RIGHT:
						MoveBlock(RIGHT);
						break;
					case KEY_DOWN:
						MoveBlock(DOWN);
						break;
					default:
						break;
				}
			}
			cout<<"key :" <<key <<endl;
		}
		delay(2);

	}
	if(GameOver) {
		setcolor(GREENBR);
		settextstyle(10,0,5);
		int Color = 0;
		while(!kbhit()) {
			switch(Color++) {
				case 0:
					setcolor(GREENBR);
					break;
				case 1:
					setcolor(GREEN);
					break;
				case 2:
					setcolor(GREENDR);
					break;
				case 3:
					setcolor(GREEN);
					break;
			}
			outtextxy(getmaxx()/2-100,getmaxy()/2,"Game Over");
			if(Color > 3) {
				Color = 0;
			}
			delay(80);
		}
	}


	closegraph();
	return 1;
}

void Rotate() {
	int tmpBlockMatrix[4][4];
	
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			tmpBlockMatrix[i][j] = BlockMatrix[i][j];
		}
	}

	switch(CurrentShape) {
		case SHAPE_O:
			return;
		case SHAPE_L:
		case SHAPE_L2:
		case SHAPE_S:
		case SHAPE_S2:
		case SHAPE_T:
			BlockMatrix[0][0] = tmpBlockMatrix[0][2];
			BlockMatrix[0][1] = tmpBlockMatrix[1][2];
			BlockMatrix[0][2] = tmpBlockMatrix[2][2];
			BlockMatrix[1][2] = tmpBlockMatrix[2][1];
			BlockMatrix[2][2] = tmpBlockMatrix[2][0];
			BlockMatrix[2][1] = tmpBlockMatrix[1][0];
			BlockMatrix[2][0] = tmpBlockMatrix[0][0];
			BlockMatrix[1][0] = tmpBlockMatrix[0][1];
			break;
		case SHAPE_I:
			BlockMatrix[0][1] = tmpBlockMatrix[1][0];
			BlockMatrix[1][0] = tmpBlockMatrix[0][1];
			BlockMatrix[2][1] = tmpBlockMatrix[1][2];
			BlockMatrix[1][2] = tmpBlockMatrix[2][1];
			BlockMatrix[3][1] = tmpBlockMatrix[1][3];
			BlockMatrix[1][3] = tmpBlockMatrix[3][1];
			break;
	}
	// copy
	if(!CheckMove(0)) {

		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				BlockMatrix[i][j] = tmpBlockMatrix[i][j];
			}
		}
	}
}

int MoveBlock(int Direction) {
	switch(Direction) {
		case LEFT:
			if(!CheckMove(LEFT))
				return 0;
			DisplayBlock(--BlockX, BlockY);
			break;
		case RIGHT:
			if(!CheckMove(RIGHT))
				return 0;
			DisplayBlock(++BlockX, BlockY);
			break;
		case DOWN:
			if(!CheckMove(DOWN)) {
				GetNextBlock();
				return 0;
			}
			DisplayBlock(BlockX, ++BlockY);
			break;
		case REFRESH:
			DisplayBlock(BlockX, BlockY);
			break;
	}
	return 1;
}


int CheckMove(int Direction) {
	int Bx = BlockX;
	int By = BlockY;
	switch(Direction) {
		case LEFT:
			Bx--;
			break;
		case RIGHT:
			Bx++;
			break;
		case DOWN:
			By++;
			break;
		default:
			break;
	}

	// Left check
	if(Bx < 0) {
		for(int x = 0; (x + Bx)<0; x++){
			for(int y = 0; y < 4; y++){
				if(BlockMatrix[x][y]!=0){
					return 0;
				}
			}
		}
	}
	// Right check
	if(Bx > COL-4) {
		for(int x=Bx+3; x>=COL; x--){
			for(int y=0; y<4; y++){
				if(BlockMatrix[x-Bx][y]!=0){
					return 0;
				}
			}
		}
	}
	// Bottom check
	for(int x=0; x<4; x++){
		for(int y=0; y<4; y++) {
			if(ScreenBG[Bx+x][By+y]!=BLANK && BlockMatrix[x][y]!=BLANK){
				return 0;
			}
			if(By+y >=ROW && BlockMatrix[x][y]!=BLANK){
				return 0;
			}
		}
	}

	return 1;
}

void DisplayBlock(int Bx, int By) {
	cout<<"\n  ScreenBG   \n";
	for(int i=0; i<COL ; i++){
		for(int j=0; j<ROW; j++){
			cout<<ScreenBG[i][j]<<"  ";
		}
		cout<<"\n";
	}
	cout<<"\n\n===========================\n\n";
	cout<<"\n  ScreenLayout   \n";
	for(int i=0; i<COL ; i++){
		for(int j=0; j<ROW; j++){
			cout<<ScreenLayout[i][j]<<"  ";
		}
		cout<<"\n";
	}
	
	for(int i=0; i<COL ; i++){
		for(int j=0; j<ROW; j++){
			ScreenLayout[i][j] = ScreenBG[i][j];
		}
	}

	for(int i=0; i<4; i++) {
		if((Bx+i)<0 || (Bx+i) > COL){
			continue;
		}
		for(int j=0; j<4; j++) {
			if((By+j)>ROW){
				continue;
			}
			if(BlockMatrix[i][j] == 0){
				continue;
			}
			ScreenLayout[Bx+i][By+j] = BlockMatrix[i][j];
		}
	}
	UpdateScreen();
}

void UpdateScreen() {
	for(int x=0; x<COL; x++){
		for(int y=0; y<ROW; y++) {

			switch(ScreenLayout[x][y]) {
				case BLUE:
					putimage(LEFT+x*21, TOP+y*21, bmBlueBrick, 0);
					break;
				case RED:
					putimage(LEFT+x*21, TOP+y*21, bmRedBrick, 0);
					break;
				case GREEN:
					putimage(LEFT+x*21, TOP+y*21, bmGreenBrick, 0);
					break;
				case YELLOW:
					putimage(LEFT+x*21, TOP+y*21, bmYellowBrick, 0);
					break;
				case PURPLE:
					putimage(LEFT+x*21, TOP+y*21, bmPurpleBrick, 0);
					break;
				default:
					putimage(LEFT+x*21, TOP+y*21, bmBlankBrick, 0);
			}
		}
	}
}

void GetNextBlock() {
	for(int x=0; x<4; x++) {
		for(int y=0; y<4; y++) {
			if(BlockMatrix[x][y] != BLANK) {
				ScreenBG[BlockX+x][BlockY+y] = BlockMatrix[x][y];
			}
		}
	}
	CheckLine();
	SetCurrentShape(NextShape, NextColor);
	NextShape = GetRandomShape();
	NextColor = GetRandomColor();
	DisplayNextShape();
	BlockX = 7;
	BlockY = 0;
	if(!MoveBlock(LEFT)) {
		GameOver = true;
	}
}

void CheckLine() {
	int remove;
	int count = 1;
	for(int x = 0; x < ROW; x++) {
		remove = 1;
		for(int y = 0; y < COL; y++) {
			if(ScreenLayout[y][x] == BLANK){
				remove = 0;
				break;
			}
		}
		if(remove) {
			RemoveLine(x);
			Score += LineScore * count;
			count++;
			PrintInfo();
		}
	}
	while (kbhit()) getch();
}

void RemoveLine(int line) {
	int count = 0;
	int Temp[COL];
	for(int i = 0; i < COL; i++) {
		Temp[i] = ScreenLayout[i][line];
	}
	for(int j = 0; j < 6; j++) {
		for(int i = 0; i < COL; i++) {
			ScreenLayout[i][line]=(count ? Temp[i] : BLANK);
		}
		count = !count;
		UpdateScreen();
		delay(50);
	}
	
	for(int i = line; i>0; i--){
		for(int j=0; j<COL; j++) {
			ScreenBG[j][i] = ScreenBG[j][i-1];
		}
	}
	for(int j=0; j<COL; j++){
		ScreenBG[j][0] = BLANK;
	}
	for(int i=0; i<COL; i++){
		for(int j=0; j<ROW; j++){
			ScreenLayout[i][j] = ScreenBG[i][j];
		}
	}
	LinesCleared++;
	PrintInfo();
	if(LinesCleared % 5 == 0){
		if(Speed-5 >= 0){
			Speed -= 5;
			Level++;
			PrintInfo();
		}
	}
}


void DisplayNextShape() {
	int MarginTop = 400;
	int MarginLeft = 600;
	int TempMatrix[4][4];

	switch(NextShape) {
		case SHAPE_I:
			for(int i=0; i<4; i++){
				for(int j=0; j<4; j++){
					TempMatrix[i][j] = ShapeI[i][j]*NextColor;
				}
			}
			break;
		case SHAPE_T:
			for(int i=0; i<4; i++){
				for(int j=0; j<4; j++){
					TempMatrix[i][j] = ShapeT[i][j]*NextColor;
				}
			}
			break;
		case SHAPE_L:
			for(int i=0; i<4; i++){
				for(int j=0; j<4; j++){
					TempMatrix[i][j] = ShapeL[i][j]*NextColor;
				}
			}
			break;
		case SHAPE_L2:
			for(int i=0; i<4; i++){
				for(int j=0; j<4; j++){
					TempMatrix[i][j] = ShapeL2[i][j]*NextColor;
				}
			}
			break;
		case SHAPE_O:
			for(int i=0; i<4; i++){
				for(int j=0; j<4; j++){
					TempMatrix[i][j] = ShapeO[i][j]*NextColor;
				}
			}
			break;
		case SHAPE_S:
			for(int i=0; i<4; i++){
				for(int j=0; j<4; j++){
					TempMatrix[i][j] = ShapeS[i][j]*NextColor;
				}
			}
			break;
		case SHAPE_S2:
			for(int i=0; i<4; i++){
				for(int j=0; j<4; j++){
					TempMatrix[i][j] = ShapeS2[i][j]*NextColor;
				}
			}
			break;
		default:
			break;
	}

	for(int x=0; x<4; x++) {
		for(int y=0; y<4; y++) {
			switch(TempMatrix[x][y]) {
				case BLUE:
					putimage(MarginLeft+x*21, MarginTop+y*21, bmBlueBrick, 0);
					break;
				case RED:
					putimage(MarginLeft+x*21, MarginTop+y*21, bmRedBrick, 0);
					break;
				case GREEN:
					putimage(MarginLeft+x*21, MarginTop+y*21, bmGreenBrick, 0);
					break;
				case YELLOW:
					putimage(MarginLeft+x*21, MarginTop+y*21, bmYellowBrick, 0);
					break;
				case PURPLE:
					putimage(MarginLeft+x*21, MarginTop+y*21, bmPurpleBrick, 0);
					break;
				default:
					putimage(MarginLeft+x*21, MarginTop+y*21, bmBlankBrick, 0);
			}
		}
	}
}


void DisplayUI() {
	setcolor(15);
	// Game
	rectangle(199,30,518,560);
	// Score
	DrawBox(530,30,730,290, YELLOWBR, YELLOW, YELLOWDR);
	// Next Brick
	DrawBox(530,330,730,560, YELLOWBR, YELLOW, YELLOWDR);
	// Menu
	DrawBox(10,330,180,560, YELLOWBR, YELLOW, YELLOWDR);
	setcolor(GREEN);
	settextstyle(0,0,1);

	outtextxy(550,40, "Score:");
	rectangle(550,49,700,71);
	outtextxy(550,100, "Level:");
	rectangle(550,109,700,131);
	outtextxy(550,160, "Speed:");
	rectangle(550,169,700,191);

	outtextxy(550,220, "Lines Cleared:");
	rectangle(550,229,700,251);

	outtextxy(550,350, "Next Shape:");


	PrintInfo();

	setcolor(GREEN);
	settextstyle(8,0,5);
	outtextxy(30,340,"KEYS:");
	settextstyle(0,0,1);
	setcolor(PURPLE);
	outtextxy(30,380,"Left   :  < ");
	outtextxy(30,420,"Right  :  >");
	outtextxy(30,460,"Rotate :  ^");
	outtextxy(30,500,"Drop   :  v");
	setcolor(GREEN);

	getch();
}

void PrintInfo() {
	settextstyle(8,0,1);

	sprintf(lineScore, "%11ld", Score);
	putimage(560,50, bmScore,0);
	outtextxy(560,50,lineScore);

	sprintf(lineLevel, "%11d", Level);
	putimage(560,110, bmLevel,0);
	outtextxy(560,110,lineLevel);

	sprintf(lineSpeed, "%11d", 100-Speed);
	putimage(560,170, bmSpeed,0);
	outtextxy(560,170,lineSpeed);

	sprintf(lineLinesCleared, "%11d", LinesCleared);
	putimage(560,230, bmLinesCleared,0);
	outtextxy(560,230,lineLinesCleared);
}



void DrawBox(int x1, int y1, int x2, int y2, int Color1, int Color2, int Color3) {
	setlinestyle(0,0,1);
	// rectangle 1
	setcolor(Color1);
	rectangle(x1,y1,x2,y2);
	// rectangle 2
	setcolor(Color2);
	rectangle(x1+2,y1+2,x2-2,y2-2);
	// rectangle 3
	setcolor(Color3);
	rectangle(x1+4,y1+4,x2-4,y2-4);
}

int GetRandomColor() {
	int Color = rand() % 5;
	switch(Color) {
		case 0 :
			return BLUE;
		case 1:
			return RED;
		case 2:
			return GREEN;
		case 3:
			return YELLOW;
		case 4:
			return PURPLE;
	}
	return 0;
}

int GetRandomShape() {
	int Shape = rand() % 7;
	switch(Shape) {
		case 0 :
			return SHAPE_I;
		case 1:
			return SHAPE_L;
		case 2:
			return SHAPE_T;
		case 3:
			return SHAPE_O;
		case 4:
			return SHAPE_S;
		case 5:
			return SHAPE_S2;
		case 6:
			return SHAPE_L2;
	}
	return 0;
}

void SetCurrentShape(int Shape, int Color) {

	CurrentShape = Shape;

	switch(Shape) {
		case SHAPE_I:
			for(int i=0; i<4; i++) {
				for(int j=0; j<4; j++) {
					BlockMatrix[i][j] = ShapeI[i][j]*Color;
				}
			}
			break;
		case SHAPE_T:
			for(int i=0; i<4; i++) {
				for(int j=0; j<4; j++) {
					BlockMatrix[i][j] = ShapeT[i][j]*Color;
				}
			}
			break;
		case SHAPE_L:
			for(int i=0; i<4; i++) {
				for(int j=0; j<4; j++) {
					BlockMatrix[i][j] = ShapeL[i][j]*Color;
				}
			}
			break;
		case SHAPE_L2:
			for(int i=0; i<4; i++) {
				for(int j=0; j<4; j++) {
					BlockMatrix[i][j] = ShapeL2[i][j]*Color;
				}
			}
			break;
		case SHAPE_O:
			for(int i=0; i<4; i++) {
				for(int j=0; j<4; j++) {
					BlockMatrix[i][j] = ShapeO[i][j]*Color;
				}
			}
			break;
		case SHAPE_S:
			for(int i=0; i<4; i++) {
				for(int j=0; j<4; j++) {
					BlockMatrix[i][j] = ShapeS[i][j]*Color;
				}
			}
			break;
		case SHAPE_S2:
			for(int i=0; i<4; i++) {
				for(int j=0; j<4; j++) {
					BlockMatrix[i][j] = ShapeS2[i][j]*Color;
				}
			}
			break;
		default:
			break;
	}
}

void SplashScreen() {
	cleardevice();

	int splash[20][30] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,0,1,0,0,1,1,1,0,
		0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,1,0,0,0,0,
		0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,
		0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,1,1,0,0,
		0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,
		0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,
		0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,1,1,1,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	setfillstyle(10, BLUEDR);
	bar(0,0,getmaxx(),getmaxy());

	// T
	for(int i=0; i<15; i++) {
		for(int j=0; j<6; j++) {
			if(splash[i][j]) {
				putimage(j*25, i*25, bmBlueBrick,0);
			}
		}
	}
	// E
	for(int i=0; i<15; i++) {
		for(int j=6; j<12; j++) {
			if(splash[i][j]) {
				putimage(j*25, i*25, bmGreenBrick,0);
			}
		}
	}
	// T
	for(int i=0; i<15; i++) {
		for(int j=12; j<17; j++) {
			if(splash[i][j]) {
				putimage(j*25, i*25, bmRedBrick,0);
			}
		}
	}
	// R
	for(int i=0; i<15; i++) {
		for(int j=17; j<22; j++) {
			if(splash[i][j]) {
				putimage(j*25, i*25, bmYellowBrick,0);
			}
		}
	}

	// I
	for(int i=0; i<15; i++) {
		for(int j=23; j<25; j++) {
			if(splash[i][j]) {
				putimage(j*25, i*25, bmPurpleBrick,0);
			}
		}
	}

	// S
	for(int i=0; i<15; i++) {
		for(int j=25; j<30; j++) {
			if(splash[i][j]) {
				putimage(j*25, i*25, bmRedBrick,0);
			}
		}
	}
	// LK
	for(int i=12; i<20; i++) {
		for(int j=9; j<19; j++) {
			if(splash[i][j]) {
				putimage(j*25, i*25, bmPurpleBrick,0);
			}
		}
	}

	setcolor(GREENBR);
	settextstyle(10,0,3);
	int Color = 0;
	while(!kbhit()) {
		switch(Color++) {
			case 0:
				setcolor(GREENBR);
				break;
			case 1:
				setcolor(GREEN);
				break;
			case 2:
				setcolor(GREENDR);
				break;
			case 3:
				setcolor(GREEN);
				break;
		}
		outtextxy(500,500,"On Press Key");
		if(Color > 3) {
			Color = 0;
		}
		delay(80);
	}


}
// init Layout
void InitScreenLayout() {
	for (int x = 0; x < COL; x++) {
		for (int y = 0; y < ROW; y++) {
			ScreenLayout[x][y] = BLANK;
			ScreenBG[x][y] = BLANK;
		}
	}

}
// new img brick
void GetImages() {

	Brick(100,100,BLUE);
	Brick(130,100,RED);
	Brick(160,100,GREEN);
	Brick(190,100,YELLOW);
	Brick(220,100,PURPLE);

	bmBlueBrick   = malloc(imagesize(0,0,20,20));
	bmRedBrick    = malloc(imagesize(0,0,20,20));
	bmGreenBrick  = malloc(imagesize(0,0,20,20));
	bmYellowBrick = malloc(imagesize(0,0,20,20));
	bmPurpleBrick = malloc(imagesize(0,0,20,20));
	bmBlankBrick  = malloc(imagesize(0,0,20,20));
	bmScore       = malloc(imagesize(501,51,607,69));
	bmSpeed       = malloc(imagesize(501,51,607,69));
	bmLevel       = malloc(imagesize(501,51,607,69));
	bmLinesCleared= malloc(imagesize(501,51,607,69));
	bmScreen      = malloc(imagesize(0,0,640,480));

	getimage(100,100,120,120,bmBlueBrick);
	getimage(130,100,150,120,bmRedBrick);
	getimage(160,100,180,120,bmGreenBrick);
	getimage(190,100,210,120,bmYellowBrick);
	getimage(220,100,240,120,bmPurpleBrick);
	getimage(0,0,20,20,bmBlankBrick);
	getimage(501,51,607,69,bmScore);
	getimage(501,51,607,69,bmSpeed);
	getimage(501,51,607,69,bmLevel);
	getimage(501,51,607,69,bmLinesCleared);
}

// create Block
void Brick(int x, int y, int Color) {
	int top[] = {x, y, x + 20, y, x, y + 20};
	int bottom[] = {x + 20, y + 20, x + 20, y, x, y + 20};

	// tam giac tren

	setcolor(Color + 1);
	setfillstyle(1, Color + 1);
	fillpoly(3, top);
	// tam giac duoi
	setcolor(Color + 2);
	setfillstyle(1, Color + 2);
	fillpoly(3, bottom);
	// trong
	setcolor(Color);
	setfillstyle(1, Color);
	bar(x + 5, y + 5, x + 15, y + 15);

}
