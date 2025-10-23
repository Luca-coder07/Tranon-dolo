#include "horror.h"

static float SimpleNoise(float t) {
  return (0.5f * (sinf(t * 1.7f + PI / 4)));
}

void DrawTorchLightEffect(bool light, float torchBattery, int screenWidth, int screenHeight)
{
    if (light && torchBattery > 0)
    {
        Vector2 lightPos = {screenWidth / 2, screenHeight * 0.9f};
        float baseRadius = screenHeight * 0.5f;

        float time = GetTime();
        float flicker = 1.0f + 0.05f * SimpleNoise(time * 20.0f);

        float intensity = 0.5 + 0.5f * (torchBattery / TORCH_MAX_BATTERY);

        float vignetteRadius = screenHeight * 1.4f;
        Color vignetteColor = (Color){0, 0, 0, 80};
        DrawCircleGradient(lightPos.x, lightPos.y, vignetteRadius, (Color){0, 0, 0, 0}, vignetteColor);
 
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 230});

        float lightRadius = (baseRadius + (screenHeight * 0.25f)) * flicker;
        Color coreColor = (Color) {255, 200, 150, (unsigned char)(40 * intensity)};
        Color edgeColor = (Color) {255, 200, 150, 0};

        DrawCircleGradient(lightPos.x, lightPos.y, lightRadius, coreColor, edgeColor);
    }
    else
    {
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 240});
    }
}

void DrawBatteryBar(float torchBattery, int screenWidth, int screenHeight)
{
  int barWidth = (int)(screenWidth * 0.15f);
  int barHeight = (int)(screenHeight * 0.02f);
  int barX = screenWidth - barWidth - 10;
  int barY = screenHeight - barHeight - 10;

  DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);

  float batteryRatio = torchBattery / TORCH_MAX_BATTERY;
  batteryRatio = fmaxf(0.0f, fminf(batteryRatio, 1.0f));
  Color fullColor = GREEN;
  Color lowColor = RED;
  Color barColor = {
    (unsigned char)((1 - batteryRatio) * lowColor.r + batteryRatio * fullColor.r),
    (unsigned char)((1 - batteryRatio) * lowColor.g + batteryRatio * fullColor.g),
    (unsigned char)((1 - batteryRatio) * lowColor.b + batteryRatio * fullColor.b),
    255
  };

  if (batteryRatio < 0.3f)
  {
    float pulse = 0.5f + 0.5f * sinf(GetTime() * 6.0f);
    barColor.a = (unsigned char)(255 * pulse * batteryRatio / 0.3f);
  }

  DrawRectangle(barX, barY, (int)(barWidth * batteryRatio), barHeight, barColor);
  DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);
  
  char batteryText[8];
  snprintf(batteryText, sizeof batteryText, "%d%%", (int)(batteryRatio * 100));
  int fontSize = barHeight + 4;
  int textWidth = MeasureText(batteryText, fontSize);
  int textPosX = barX + (barWidth / 2) - (textWidth / 2);
  int textPosY = barY - fontSize - 2;

  DrawText(batteryText, textPosX, textPosY, fontSize, WHITE);
}

void DrawTextInCenterX(const char *text, int posY, int screenWidth, int fontSize, Color color, bool withShadow)
{
  int textWidth = MeasureText(text, fontSize);
  int posX = screenWidth / 2 - textWidth / 2;

  if (withShadow) {
    DrawText(text, posX + 2, posY + 2, fontSize, (Color){ 20, 20, 20, color.a });
  }
  DrawText(TextFormat(text), posX, posY, fontSize, color);
}
