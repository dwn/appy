////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Novel variant of nim.
////////////////////////////////////////////////////////////////////////////////
#pragma once

void nimInit();
void nimClose();
void nimReset();
void nimLoopInit(bool updateNoise=true);
void nimLoopUpdate();
void nimPretitleLoop();
void nimSegue(void (*callback)(void),int shaderIndexSet=0);
void nimCurtainLoop();
void nimTitleLoop();
void nimMainLoop();
void nimEndSequenceLoop();
char nimGameState(unsigned char v[]);
bool nimGetInput(unsigned char v[],const char* playerName[],int player);
unsigned char nimNextPlayer(unsigned char ind,unsigned char i=1) { return ((ind+i-1)&1)+1; }
int nimRateConfiguration(unsigned char v[],unsigned char index);
void nimCheckCatGame();
void nimDrawBoard();
void nimTakeFromRing(char ringIndex);
void nimBlackMarker(int xPos,int yPos);
void nimWhiteMarker(int xPos,int yPos);
