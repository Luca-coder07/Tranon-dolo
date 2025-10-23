#include "horror.h"

void UpdateAndDrawMenu(t_res *res, int screenWidth, int screenHeight, unsigned int *onWhat, bool *drawInfo, GameState *currentState, int *storyCurrentChar, bool *storyFullyDisplayed, double *gameStartTime)
{
    // Input handling
    if (IsKeyPressed(KEY_UP) && !(*drawInfo))
    {
        if (*onWhat > 1) *onWhat -= 1;
        else *onWhat = 3;
    }
    else if (IsKeyPressed(KEY_DOWN) && !(*drawInfo))
    {
        if (*onWhat < 3) *onWhat += 1;
        else *onWhat = 1;
    }

    // Update text colors based on selection
    Color textColor1 = RAYWHITE;
    Color textColor2 = RAYWHITE;
    Color textColor3 = RAYWHITE;

    switch (*onWhat)
    {
        case 1:
            textColor1 = GOLD;
            if (IsKeyPressed(KEY_ENTER))
            {
                *storyCurrentChar = 0;
                *storyFullyDisplayed = false;
                *currentState = STORY;
                *gameStartTime = GetTime();
            }
            break;
        case 2:
            textColor2 = RED;
            if (IsKeyPressed(KEY_ENTER))
            {
                UnloadResources(res);
                CloseAudioDevice();
                CloseWindow();
                exit(0);
            }
            break;
        case 3:
            textColor3 = GREEN;
            if (IsKeyPressed(KEY_ENTER))
            {
                *drawInfo = !(*drawInfo);
            }
            break;
    }

    // Draw menu
    BeginDrawing();
    DrawTexturePro(res->menuText, (Rectangle){0, 0, res->menuText.width, res->menuText.height}, 
                   (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2){0, 0}, 0.0f, WHITE);
    DrawText("Hilalao", screenWidth * 0.12, screenHeight * 0.45, screenWidth * 0.02, textColor1);
    DrawText("Hiala", screenWidth * 0.12, screenHeight * 0.5, screenWidth * 0.02, textColor2);
    DrawText("Momba ny mpamorona", screenWidth * 0.12, screenHeight * 0.8, screenWidth * 0.015, textColor3);
    DrawText("Casque/Ecouteur recommandé", screenWidth * 0.02, screenHeight * 0.95, screenWidth * 0.015, RAYWHITE);

    if (*drawInfo)
    {
        float rectWidth = screenWidth * 0.2f;
        float rectHeight = screenHeight * 0.2f;
        float rectX = (screenWidth * 0.51f) - (rectWidth / 2);
        float rectY = (screenHeight * 0.32f) - (rectHeight / 2);
        DrawRectangle(rectX, rectY, rectWidth, rectHeight, DARKGRAY);
        DrawRectangle(rectX + 5, rectY + 5, rectWidth - 10, rectHeight - 10, (Color){76, 63, 47, 200});
        int fontSize = screenWidth * 0.02f;
        int lineSpacing = fontSize + 10;
        DrawText("RANDRIA Luca", rectX + 10, rectY + 10, fontSize, RAYWHITE);
        DrawText("Tranon-dolo project 2025", rectX + 10, rectY + 10 + lineSpacing, fontSize * 0.7, RAYWHITE);
        DrawText("Press ENTER", rectX + 10, rectY + 10 + 3 * lineSpacing, fontSize * 0.6, RAYWHITE);
    }
    EndDrawing();
}
