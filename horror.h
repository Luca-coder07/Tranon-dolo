#ifndef HORROR_H
#define HORROR_H

#include "raylib.h"
#include "rcamera.h"
#include "raymath.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Declaration of all globals variables
#define CAMERA_MOUSE_SENSITIVITY 0.002f
#define INTERACTION_DISTANCE 1.0f
#define TORCH_MAX_BATTERY 120.0f
#define GHOST_MAX_SCALE 2.0f
#define PHRASE_DURATION 10.0f
#define STORY_TEXT_MAX_LENGTH 2048
#define MAX_WORDS 512

typedef enum
{
	MENU,
  STORY,
	GAME,
  PAUSE,
	OVER,
	FINISH
} GameState;

typedef struct s_res {
  Texture2D menuText;
  Texture2D ghostText;
  Texture2D bloodText;
  Texture2D cubicmap;
  Texture2D texture;
  Texture2D doorTexture;
  Model model;
  Model doorModel;
  Sound click, lev1, lev2, lev3, pas, soupire, dead, close, keyPos, pleur, cry;
  Music bg_music;
  Color *mapPixels;
} t_res;

typedef struct s_hb {
  float bobbingTimer;
  float bobbingSpeed;
  float bobbingAmount;
  Vector3 baseCameraPos;
  float movingTime;
  bool soupirePlayed;
  float prevPhase;
} t_hb;

void LoadResources(t_res *res);
void UnloadResources(t_res *res);
void InitHeadBobbing(t_hb *hb, Vector3 camPos);
void DrawTextInCenterX(const char *text, int posY, int screenWidth, int fontSize, Color color, bool withShadow);
void DrawTorchLightEffect(bool light, float torchBattery, int screenWidth, int screenHeight);
float VolumeAtPosition(Vector3 camPos, float x, float z);
bool HandlePlayerCollision(Camera *camera, Vector3 oldCamPos, Vector3 mapPosition, t_res *res);
void HeadBobbingUpdate(t_hb *hb, Camera *camera, float dt, bool isMoving, t_res *res);
void UpdateAndDrawStory(
    const char *storyText,
    int storyTextLength,
    int *storyCurrentChar,
    float *storyTimeAccumulator,
    const float charDisplayTime,
    bool *storyFullyDisplayed,
    int screenWidth,
    int screenHeight,
    GameState *currentState,
    double *gameStartTime,
    double *totalPauseDuration);
void UpdateAndDrawMenu(t_res *res, int screenWidth, int screenHeight, unsigned int *onWhat,
    bool *drawInfo, GameState *currentState, int *storyCurrentChar, bool *storyFullyDisplayed, double *gameStartTime);
void UpdateAndDrawFinish(int screenWidth, int screenHeight, Sound sound, GameState *state);
void DrawBatteryBar(float torchBattery, int screenWidth, int screenHeight);
void UpdateTorchLight(bool *light, float *torchBattery, float dt, Sound clickSound);
void UpdateDoorOpening(float *doorOpenAngle, Vector3 *pos, const float doorOpenSpeed, float dt, bool doorOpening, GameState *currentState);
void UpdatePlayerMovementAndCamera(Camera *camera, t_hb *hb, float dt, Vector3 mapPosition, t_res *res, GameState *currentState, double *pauseStartTime);
Vector3 RandomizeKeyPosition(t_res* res, Vector3 mapPosition);

#endif // ! HORROR_H
