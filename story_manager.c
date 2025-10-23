#include "horror.h"

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
    double *totalPauseDuration)
{
    float dt = GetFrameTime();

    if (!(*storyFullyDisplayed))
    {
        *storyTimeAccumulator += dt;
        while (*storyTimeAccumulator >= charDisplayTime && *storyCurrentChar < storyTextLength)
        {
            (*storyCurrentChar)++;
            *storyTimeAccumulator -= charDisplayTime;
        }
        if (*storyCurrentChar >= storyTextLength)
        {
            *storyCurrentChar = storyTextLength;
            *storyFullyDisplayed = true;
        }
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (!(*storyFullyDisplayed))
        {
            *storyCurrentChar = storyTextLength;
            *storyFullyDisplayed = true;
        }
        else
        {
            *currentState = GAME;
            *gameStartTime = GetTime();
            *totalPauseDuration = 0.0;
        }
    }

    BeginDrawing();
    ClearBackground(BLACK);
    char displayedText[STORY_TEXT_MAX_LENGTH] = {0};
    strncpy(displayedText, storyText, *storyCurrentChar);
    displayedText[*storyCurrentChar] = '\0';
    DrawText(displayedText, (int)(screenWidth * 0.02f), (int)(screenHeight * 0.07f), (int)(screenWidth * 0.015f), (Color){180, 180, 180, 255});
    if (*storyFullyDisplayed)
        DrawText("Tsindrio ny ENTER raha hanomboka.", (int)(screenWidth * 0.01f), screenHeight - (int)(screenHeight * 0.03f), (int)(screenWidth * 0.012), RED);
    else
        DrawText("Tsindrio ny ENTER raha hampiseho hatramin'ny farany.", (int)(screenWidth * 0.01f), screenHeight - (int)(screenHeight * 0.03f), (int)(screenWidth * 0.012f), RED);
    EndDrawing();
}
