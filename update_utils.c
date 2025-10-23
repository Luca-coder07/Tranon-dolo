#include "horror.h"

Vector3 RandomizeKeyPosition(t_res* res, Vector3 mapPosition) {
    typedef struct { int x, y; } Cell;
    Cell* freeCells = NULL;
    int freeCellCount = 0;

    for (int y = 0; y < res->cubicmap.height; y++) {
        for (int x = 0; x < res->cubicmap.width; x++) {
            Color pixel = res->mapPixels[y * res->cubicmap.width + x];
            if (pixel.r == 0) { // cell libre
                freeCellCount++;
                freeCells = realloc(freeCells, freeCellCount * sizeof(Cell));
                freeCells[freeCellCount - 1] = (Cell){x, y};
            }
        }
    }
    Vector3 keyPos = {0};
    if (freeCellCount > 0) {
        int idx = rand() % freeCellCount;
        keyPos.x = mapPosition.x + freeCells[idx].x;
        keyPos.y = 0.0f;
        keyPos.z = mapPosition.z + freeCells[idx].y;
    }
    free(freeCells);
    return keyPos;
}

void UpdatePlayerMovementAndCamera(Camera *camera, t_hb *hb, float dt, Vector3 mapPosition, t_res *res, GameState *currentState, double *pauseStartTime)
{
    float cameraMoveSpeed = 1.5f * dt;
    Vector3 oldCamPos = camera->position; // Store old camera position
    bool isMoving = (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D));

    HeadBobbingUpdate(hb, camera, dt, isMoving, res);

    // Keyboard support
    if (IsKeyDown(KEY_W))
        CameraMoveForward(camera, cameraMoveSpeed, 1);
    if (IsKeyDown(KEY_A))
        CameraMoveRight(camera, -cameraMoveSpeed, 1);
    if (IsKeyDown(KEY_S))
        CameraMoveForward(camera, -cameraMoveSpeed, 1);
    if (IsKeyDown(KEY_D))
        CameraMoveRight(camera, cameraMoveSpeed, 1);

    if (IsKeyPressed(KEY_ESCAPE))
    {
        *currentState = PAUSE;
        *pauseStartTime = GetTime();
    }

    // Mouse support
    Vector2 mousePositionDelta = GetMouseDelta();
    CameraYaw(camera, -mousePositionDelta.x * CAMERA_MOUSE_SENSITIVITY, 0);
    CameraPitch(camera, -mousePositionDelta.y * CAMERA_MOUSE_SENSITIVITY, 1, 0, 0);

    HandlePlayerCollision(camera, oldCamPos, mapPosition, res);
}

void UpdateDoorOpening(float *doorOpenAngle, Vector3 *pos, const float doorOpenSpeed, float dt, bool doorOpening, GameState *currentState)
{
    if (doorOpening && *doorOpenAngle < 80.0f)
    {
        *doorOpenAngle += doorOpenSpeed * dt;
        pos->x += 0.2f * dt;
        pos->z -= 0.2f * dt;
        if (*doorOpenAngle >= 80.0f)
        {
            *doorOpenAngle = 80.0f;
            *currentState = FINISH;
        }
    }
}

void UpdateTorchLight(bool *light, float *torchBattery, float dt, Sound clickSound)
{
    if (IsKeyPressed(KEY_L))
    {
        PlaySound(clickSound);
        if (!(*light) && *torchBattery > 0)
            *light = true;
        else
            *light = false;
    }
    if (*light && *torchBattery > 0.0f)
        *torchBattery -= dt;
    if (*torchBattery <= 0.0f)
        *light = false;
}

void HeadBobbingUpdate(t_hb *hb, Camera *camera, float dt, bool isMoving, t_res *res)
{
  if (isMoving)
  {
    hb->movingTime += dt;
    if (hb->movingTime >= 5.0f && !hb->soupirePlayed)
    {
      PlaySound(res->soupire);
      hb->soupirePlayed = true;
    }
    hb->bobbingTimer += dt * hb->bobbingSpeed;
    camera->position.y = hb->baseCameraPos.y + sinf(hb->bobbingTimer) * hb->bobbingAmount;
  }
  else
  {
    hb->movingTime = 0.0f;
    hb->soupirePlayed = false;
    hb->bobbingTimer = 0;
    camera->position.y = hb->baseCameraPos.y;
  }
  float stepAngleThreshold = PI / 2.0f;
  float currentPhase = fmodf(hb->bobbingTimer, 2 * PI);
  SetSoundVolume(res->pas, 0.6);
  if ((hb->prevPhase < stepAngleThreshold) && (currentPhase >= stepAngleThreshold))
    PlaySound(res->pas);
  hb->prevPhase = currentPhase;
}

// Returns true if collision occurred and position was reset, else false
bool HandlePlayerCollision(Camera *camera, Vector3 oldCamPos, Vector3 mapPosition, t_res *res)
{
    Vector2 playerPos = {camera->position.x, camera->position.z};
    float playerRadius = 0.1f;

    int playerCellX = (int)(playerPos.x - mapPosition.x + 0.5f);
    int playerCellY = (int)(playerPos.y - mapPosition.z + 0.5f);

    // Clamp to map bounds
    if (playerCellX < 0) playerCellX = 0;
    else if (playerCellX >= res->cubicmap.width) playerCellX = res->cubicmap.width - 1;

    if (playerCellY < 0) playerCellY = 0;
    else if (playerCellY >= res->cubicmap.height) playerCellY = res->cubicmap.height - 1;

    for (int y = 0; y < res->cubicmap.height; y++)
    {
        for (int x = 0; x < res->cubicmap.width; x++)
        {
            if ((res->mapPixels[y * res->cubicmap.width + x].r == 255) &&
                CheckCollisionCircleRec(playerPos, playerRadius,
                    (Rectangle){mapPosition.x - 0.5f + x, mapPosition.z - 0.5f + y, 1.0f, 1.0f}))
            {
                // Collision: reset position
                camera->position = oldCamPos;
                return true;
            }
        }
    }
    return false;
}

float VolumeAtPosition(Vector3 camPos, float x, float z)
{
	float dist = Vector2Distance((Vector2){camPos.x, camPos.z}, (Vector2){x, z});
	float volume = 1.f - dist * 0.1;
	return (volume < 0.f ? 0.f : volume);
};
