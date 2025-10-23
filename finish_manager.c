#include "horror.h"

float timer = 0.0f;
bool soundPlayed = false;

void UpdateAndDrawFinish(int screenWidth, int screenHeight, Sound sound, GameState *state)
{
  timer += GetFrameTime();
  if (!soundPlayed && timer >= 5.0f)
  {
    PlaySound(sound);
    soundPlayed = true;
  }
  if (IsKeyPressed(KEY_ENTER)) *state = MENU;
	BeginDrawing();
	DrawText("Eny Tafavoaka ianao fa...", screenWidth * 0.2, screenHeight * 0.2, screenHeight * 0.05, RAYWHITE);
	if (timer >= 8.0f)
  {
    DrawTextInCenterX("Efitra mitahiry fanahy ratsy hafa indray no miandry anao...", screenHeight * 0.7, screenWidth, screenHeight * 0.05, RED, true);
	  DrawTextInCenterX("Tsindrio ny ENTER raha hiverina any amin'ny MENU", screenHeight - (screenHeight * 0.02), screenWidth, screenHeight * 0.02, WHITE, true);
  }
  EndDrawing();
}
