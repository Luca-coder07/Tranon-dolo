/*
 * Game name: Tranon-dolo project 2025
 * Autor: Luca RANDRIANIRINA
 * Made with Raylib 5
*/
#include "raylib.h"
#include "rcamera.h"
#include "raymath.h"
#include <stdlib.h>
#include <time.h>

// Declaration of all globals variables
#define CAMERA_MOUSE_SENSITIVITY 0.002f
#define INTERACTION_DISTANCE 1.0f
#define TORCH_MAX_BATTERY 120.0f
#define GHOST_MAX_SCALE 2.0f
#define PHRASE_DURATION 10.0f

typedef enum { MENU, GAME, OVER, FINISH } GameState;

static float volume_of(Camera camera, float x, float z);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 0;
	int screenHeight = 0;

	GameState currentState = MENU;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE);  // Enable Multi Sampling Anti Aliasing 4x (if available)
	InitWindow(screenWidth, screenHeight, "Tranon-dolo 3D");
	screenWidth = GetScreenWidth();
	screenHeight = GetScreenHeight();
	InitAudioDevice();

	// Define the camera to look into our 3d world
	Camera camera = { 0 };
	camera.position = (Vector3){ -12.0f, 0.6f, -7.3f };    // Camera position
	camera.target = (Vector3){ -11.55f, 1.4f, 6.85f };    // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 30.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	Image menuImg = LoadImage("resources/screen.jpg");
	Texture2D menuText = LoadTextureFromImage(menuImg);
	UnloadImage(menuImg);

  Image ghostImg = LoadImage("resources/ghost.png");
  Texture2D ghostText = LoadTextureFromImage(ghostImg);
  UnloadImage(ghostImg);

  Image bloodImg = LoadImage("resources/blood.jpg");
  Texture2D bloodText = LoadTextureFromImage(bloodImg);
  UnloadImage(bloodImg);

	Image imMap = LoadImage("resources/cubicmap.png");      // Load cubicmap image (RAM)
	Texture2D cubicmap = LoadTextureFromImage(imMap);       // Convert image to texture to display (VRAM)
	Mesh mesh = GenMeshCubicmap(imMap, (Vector3){ 1.0f, 1.0f, 1.0f });
	Model model = LoadModelFromMesh(mesh);

	// NOTE: By default each cube is mapped to one part of texture atlas
	Texture2D texture = LoadTexture("resources/cubicmap_atlas.png");    // Load map texture
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;    // Set map diffuse texture
  
  // Déclaration en haut (avec les autres textures)
  Texture2D doorTexture;
  Model doorModel;

  // Après chargement du modèle principal
  doorTexture = LoadTexture("resources/door.jpg");
  Mesh doorMesh = GenMeshCube(1.0f, 1.0f, 0.1f);
  doorModel = LoadModelFromMesh(doorMesh);
  doorModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = doorTexture;

	// Get map image data to be used for collision detection
	Color *mapPixels = LoadImageColors(imMap);
	UnloadImage(imMap);             // Unload image from RAM

	Vector3 mapPosition = { -16.0f, 0.0f, -8.0f };  // Set model position

	// Initialisation du graine aléatoire.
	srand((unsigned int)time(NULL));

	// Ajout de clé à trouver aléatoirement et du porte de sortie dans le jeu
	Vector3 keyPosition = { 0 };
	bool foundValidPos = false;

	while (!foundValidPos) {
		// Genérer des coordonnées aléatoires dans les limites de la map
		int x = rand() % cubicmap.width;
		int y = rand() % cubicmap.height;

		Color pixel = mapPixels[y * cubicmap.width + x];
		if (pixel.r == 0) // Case libre (pas un mur)
		{
			// Position dans le monde : mapPosition.x + x, mapPosition.z + y
			keyPosition.x = mapPosition.x + (float)x;
			keyPosition.y = 0.0f;
			keyPosition.z = mapPosition.z + (float)y;
			foundValidPos = true;
		}
		// Sinon, réessayer une autre position
	}

  	bool hasKey = false;
  	Vector3 exitPosition = { 11.0f, 0.5f, 6.5f };

	bool light = false;

	Music bg_music = LoadMusicStream("resources/bg_music.mp3");
	PlayMusicStream(bg_music);

	Color  timerColor = WHITE;

	Sound click = LoadSound("resources/click.ogg");
	Sound lev1 = LoadSound("resources/ALALANA AVY AIZA.mp3");
	Sound lev2 = LoadSound("resources/VONJEO.mp3");
	Sound lev3 = LoadSound("resources/MITADY HITSOAKA.mp3");
	Sound pas = LoadSound("resources/pas.mp3");
	Sound soupire = LoadSound("resources/MISEFOSEFO.mp3");
  	Sound dead = LoadSound("resources/dead.mp3");
  	Sound close = LoadSound("resources/closedoorcreak.mp3");

  	const int phraseCount = 4;
  	const char *phrases[] = {
    "REHEFA HITA ILAY FANALAHIDY DIA TADIAVO NY FIVOAHANA.",
    "TSINDRIO NY <L> RAHA HAMONO NA HAMELONA JIRO.",
    "TANDREMO NY MITODITODIKA SAO HAHITA ANGANO.",
    "FOHY NY FOTOANA HANANANAO KA TADIAVO AINGANA NY FIVOAHANA."
  };

  int currentPhraseIndex = 0;
  float phraseTimer = 0.0f;

	SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	int decal = 0;

	static float prevPhase = 0;
	
  // Head Bobbing
	float bobbingTimer = 0.0f;
	float bobbingSpeed = 10.0f;
	float bobbingAmount = 0.05f;

	Vector3 baseCameraPos = camera.position;

	static float movingTime = 0.0f;
	static bool soupirePlayed = false;

	unsigned int on_what = 1;
	bool draw_info = false;

  double gameStartTime = 0;

  float ghostScale = 0.0f;
  const float ghostScaleSpeed = 1.0f; // Vitesse à laquelle il grandit (échelle par seconde)
  bool timerFinished = false;
	
  float torchBattery = 120.0f;

  DisableCursor();
	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
    //if (IsKeyPressed(KEY_X)) printf("%0.2f\n", torchBattery);
		UpdateMusicStream(bg_music);
		SetMusicVolume(bg_music, 0.2);
		float dt = GetFrameTime();

		switch (currentState)
		{
			case GAME:
			{
        phraseTimer += dt;
        if (phraseTimer >= PHRASE_DURATION)
        {
          phraseTimer = 0.0f;
          currentPhraseIndex++;
          if (currentPhraseIndex >= phraseCount) currentPhraseIndex = 0;
        }
		bool isMoving = (IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D));
		float cameraMoveSpeed = 1.5f * dt;

        double elpst = GetTime() - gameStartTime;
		int timer = 300 - (int)elpst;
		int minute = timer / 60;
		int seconde = timer % 60;
		float vol1 = volume_of(camera, -2.7f, -4.6f);
		float vol2 = volume_of(camera, 13.80f, -1.60f);

		SetSoundVolume(lev2, vol1 + 0.5);
		SetSoundVolume(lev3, vol2);
		SetSoundVolume(soupire, 1.5);
        SetSoundVolume(click, 0.2);
        SetSoundVolume(dead, 0.6);
        SetSoundVolume(close, 0.8);

        if (minute == 0)
        {
          if (seconde == 1) PlaySound(dead);
          if (seconde == 0) timerFinished = true;
        }
        if (timerFinished)
        {
          ghostScale += ghostScaleSpeed * dt * (1.0f + ghostScale * 3.0f);
          if (ghostScale > GHOST_MAX_SCALE) ghostScale = GHOST_MAX_SCALE;
        }
        else  ghostScale = 0.0f;   // Si on veut, on peut reset la taille tant que le timer n'est pas fini

        // Calcul d'opacité pour rendre l'apparition plus fantomatique
        float ghostAlpha = ghostScale * 255.0f;
        if (ghostAlpha > 255) ghostAlpha = 255;

        // Oscillation légère de la position pour effet "tremblant"
        float oscillateOffset = 5.0f * sinf(GetTime() * 15.0f);
        if (ghostScale == GHOST_MAX_SCALE) currentState = OVER;

        // Centrer la texture
        Vector2 screenCenter = { screenWidth / 2.0f + oscillateOffset, screenHeight / 2.0f + oscillateOffset };
        Vector2 origin = { ghostText.width / 2.0f, ghostText.height / 2.0f };
        Rectangle sourceRec = { 0, 0, (float)ghostText.width, (float)ghostText.height };
        Rectangle destRec = { screenCenter.x, screenCenter.y, ghostText.width * ghostScale, ghostText.height * ghostScale };
        Color ghostColor = (Color){ 255, 255, 255, (unsigned char)ghostAlpha };

        if ((int)elpst - decal == 10)
				{
					PlaySound(lev2);
					PlaySound(lev3);
					decal += 10;
				}
				if (IsKeyPressed(KEY_L)) {
					PlaySound(click);
					if (light) light = false;
					else
          {
            if (torchBattery > 0.0f) light = true;
            else light = false;
				  }
        }
        if (light && torchBattery > 0.0f) torchBattery -= dt;
        if (torchBattery <= 0.0f) light = false;
				if ((int)elpst == 3) PlaySound(lev1);
				if ((int)elpst == 0) PlaySound(close);
				if (minute <= 0)
				{
					if(seconde < 10)
						timerColor = RED;
					if (seconde <= 0)
					{
						minute = 0;
						seconde = 0;
					}
				}

				Vector3 oldCamPos = camera.position;    // Store old camera position

				if (isMoving)
				{
					movingTime += dt;
					if (movingTime >= 5.0f && !soupirePlayed)
					{
						PlaySound(soupire);
						soupirePlayed = true;
					}
					bobbingTimer += dt * bobbingSpeed;
					camera.position.y = baseCameraPos.y + sinf(bobbingTimer) * bobbingAmount;
				} else {
					// Reset when stopped
					movingTime = 0.0f;
					soupirePlayed = false;
					bobbingTimer = 0;
					camera.position.y = baseCameraPos.y;
				}

				float stepAngleThreshold = 3.14f / 2;   // approx π/2
				float currentPhase = fmodf(bobbingTimer, 2 * PI);
				SetSoundVolume(pas, 0.6);
				if ((prevPhase < stepAngleThreshold) && (currentPhase >= stepAngleThreshold)) PlaySound(pas);
				prevPhase = currentPhase;

				// Keyboard support
				if (IsKeyDown(KEY_W)) CameraMoveForward(&camera, cameraMoveSpeed, 1);
				if (IsKeyDown(KEY_A)) CameraMoveRight(&camera, -cameraMoveSpeed, 1);
				if (IsKeyDown(KEY_S)) CameraMoveForward(&camera, -cameraMoveSpeed, 1);
				if (IsKeyDown(KEY_D)) CameraMoveRight(&camera, cameraMoveSpeed, 1);
				// Mouse support
				Vector2 mousePositionDelta = GetMouseDelta();
				CameraYaw(&camera, -mousePositionDelta.x*CAMERA_MOUSE_SENSITIVITY, 0);
				CameraPitch(&camera, -mousePositionDelta.y*CAMERA_MOUSE_SENSITIVITY, 1, 0, 0);

				// Check player collision (we simplify to 2D collision detection)
				Vector2 playerPos = { camera.position.x, camera.position.z };
				float playerRadius = 0.1f;  // Collision radius (player is modelled as a cilinder for collision)

				int playerCellX = (int)(playerPos.x - mapPosition.x + 0.5f);
				int playerCellY = (int)(playerPos.y - mapPosition.z + 0.5f);

				// Out-of-limits security check
				if (playerCellX < 0) playerCellX = 0;
				else if (playerCellX >= cubicmap.width) playerCellX = cubicmap.width - 1;

				if (playerCellY < 0) playerCellY = 0;
				else if (playerCellY >= cubicmap.height) playerCellY = cubicmap.height - 1;

				// Check map collisions using image data and player position
				for (int y = 0; y < cubicmap.height; y++)
				{
					for (int x = 0; x < cubicmap.width; x++)
					{
						if ((mapPixels[y*cubicmap.width + x].r == 255) &&       // Collision: white pixel, only check R channel
							(CheckCollisionCircleRec(playerPos, playerRadius,
							(Rectangle){ mapPosition.x - 0.5f + x*1.0f, mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
						{
							// Collision detected, reset camera position
							camera.position = oldCamPos;
						}
					}
				}
				//----------------------------------------------------------------------------------
				// Draw
				//----------------------------------------------------------------------------------
				BeginDrawing();

					ClearBackground(RAYWHITE);

					BeginMode3D(camera);
						DrawModel(model, mapPosition, 1.0f, WHITE);// Draw maze map
            DrawModel(doorModel, exitPosition, 1.0f, WHITE);
            if (!hasKey) DrawSphere(keyPosition, 0.05f, (Color) { 255, 203, 0, 200 });
					EndMode3D();
          if (light && torchBattery > 0) {
            Vector2 lightPos = { screenWidth/2, screenHeight * 0.9f };
            float baseRadius = screenHeight * 0.9f * (torchBattery / TORCH_MAX_BATTERY);
            float flicker = 1.0f + 0.05f * sinf(GetTime() * 20.0f);

					  DrawRectangle(0, 0, screenWidth, screenHeight, (Color) { 0, 0, 0, 245 });
						DrawCircleGradient(lightPos.x, lightPos.y, (baseRadius + (screenHeight * 0.25f)) * flicker, (Color) { 255, 200, 150, 40 }, (Color) { 255, 200, 150, 0 });
            // Vignettage léger autour du centre de l'écran (ajuster si besoin)
            float vignetteRadius = screenHeight * 1.5f;
            Color vignetteColor = (Color){0, 0, 0, 100};
            DrawCircleGradient(lightPos.x, lightPos.y, vignetteRadius + (screenHeight * 0.25f), (Color){0,0,0, 0}, vignetteColor);
          }
          else DrawRectangle(0, 0, screenWidth, screenHeight, (Color) { 0, 0, 0, 245 });
          float distToKey = Vector3Distance(camera.position, keyPosition);
          float distToExit = Vector3Distance(camera.position, exitPosition);
          if (!hasKey && distToKey < INTERACTION_DISTANCE)
          {
            DrawText("Tsindrio ny E raha handray fanalahidy", 10, screenHeight - 60, 20, YELLOW);
            if (IsKeyPressed(KEY_E))
            {
              PlaySound(click);
              hasKey = true;
            }
          }

          if (hasKey && distToExit < INTERACTION_DISTANCE)
          {
            DrawText("Tsindrio ny E raha hivoaka", 10, screenHeight - 60, 20, GREEN);
            if (IsKeyPressed(KEY_E))
            {
              PlaySound(click);
              currentState = FINISH;
            }
          }
          else if (!hasKey && distToExit < INTERACTION_DISTANCE) DrawText("Mihidy ity varavarana ity, tadiavo ny fanalahidy", 10, screenHeight - 60, 20, RED);

					//DrawTextureEx(cubicmap, (Vector2){ screenWidth - cubicmap.width*4.0f - 20, 20.0f }, 0.0f, 4.0f, RED);
					//DrawRectangleLines(screenWidth - cubicmap.width*4 - 20, 20, cubicmap.width*4, cubicmap.height*4, DARKGREEN);
					// Draw player position radar
					//DrawRectangle(screenWidth - cubicmap.width*4 - 20 + playerCellX*4, 20 + playerCellY*4, 4, 4, YELLOW);

          DrawText(phrases[currentPhraseIndex], 10, 10, 20, (Color) { 255, 255, 255, 120 });

          // Assombrir l'écran de plus en plus selon la taille du ghost
          DrawRectangle(0, 0, screenWidth, screenHeight, (Color){ 0, 0, 0, (unsigned char)(ghostAlpha * 0.8f) });
          DrawTexturePro(ghostText, sourceRec, destRec, origin, 0.0f, ghostColor);

					DrawText(TextFormat("%02i:%02i", minute, seconde), 2, GetScreenHeight() - 30, 30, timerColor);

          // Dessiner la jauge de batterie
          int barWidth = 200;
          int barHeight = 20;
          int barX = screenWidth - barWidth - 10;
          int barY = screenHeight - barHeight - 10;

          DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
          float batteryRatio = torchBattery / TORCH_MAX_BATTERY;
          bool lowBattery = batteryRatio < 0.3f;
          Color barColor = lowBattery ? RED : GREEN;
          if (lowBattery) {
            float pulse = (sinf(GetTime() * 10.0f) + 1) / 2;
            barColor.a = (unsigned char)(255 * pulse);
          }
          DrawRectangle(barX, barY, (int)(barWidth * batteryRatio), barHeight, barColor);
          DrawRectangleLines(barX, barY, barWidth, barHeight, BLACK);

				EndDrawing();
				break;
			}
      case OVER:
      {
        float oscillateOffset = 5.0f * sinf(GetTime() * 15.0f);
        Vector2 screenCenter = { screenWidth / 2.0f + oscillateOffset, screenHeight / 2.0f + oscillateOffset };
        Vector2 origin = { ghostText.width / 2.0f, ghostText.height / 2.0f };
        Rectangle sourceRec = { 0, 0, (float)ghostText.width, (float)ghostText.height };
        Rectangle destRec = { screenCenter.x, screenCenter.y, ghostText.width * GHOST_MAX_SCALE, ghostText.height * GHOST_MAX_SCALE };
        Color ghostColor = (Color){ 255, 255, 255, (unsigned char)255 };

        float bloodX = destRec.x + destRec.width / 2 - 300;
        float bloodY = destRec.y;
        Vector2 bloodOrigin = { bloodText.width / 2.0f, bloodText.height / 2.0f };
        Rectangle bloodSourceRec = { 0, 0, (float)bloodText.width, (float)bloodText.height };
        Rectangle bloodDestRec = { bloodX, bloodY, (float)bloodText.width, (float)bloodText.height };
        Color bloodColor = WHITE; 

        BeginDrawing();
          ClearBackground(BLACK);
          DrawTexturePro(bloodText, bloodSourceRec, bloodDestRec, bloodOrigin, 0.0f, bloodColor);
          DrawTexturePro(ghostText, sourceRec, destRec, origin, 0.0f, ghostColor);
        EndDrawing();
        break;
      }
			case MENU:
			{
				Color textColor1 = RAYWHITE;
				Color textColor2 = RAYWHITE;
				Color textColor3 = RAYWHITE;

				if (IsKeyPressed(KEY_UP) && !draw_info)
				{
					if (on_what > 1) on_what--;
					else on_what = 3;
				}
				else if (IsKeyPressed(KEY_DOWN) && !draw_info)
				{
					if (on_what < 3) on_what++;
					else on_what = 1;
				}

				if (on_what == 1)
				{
					textColor1 = GOLD;
					textColor2 = RAYWHITE;
					textColor3 = RAYWHITE;
					if (IsKeyPressed(KEY_ENTER))
          {
              currentState = GAME;
              gameStartTime = GetTime();
          }
				}
				else if (on_what == 2)
				{
					textColor1 = RAYWHITE;
					textColor2 = RED;
					textColor3 = RAYWHITE;
					if (IsKeyPressed((KEY_ENTER)))
					{
						UnloadImageColors(mapPixels);   // Unload color array
						UnloadTexture(cubicmap);        // Unload cubicmap texture
						UnloadTexture(texture);         // Unload map texture
						UnloadModel(model);             // Unload map model
						UnloadMusicStream(bg_music);
						UnloadSound(click);
						UnloadSound(lev1);
						UnloadSound(lev2);
						UnloadSound(lev3);
						UnloadSound(pas);
						UnloadSound(soupire);
            UnloadSound(dead);
            UnloadSound(close);
            UnloadTexture(doorTexture);
            UnloadModel(doorModel);

						CloseAudioDevice();
						CloseWindow();
						return 0;
					}
				}
				else
				{
					textColor1 = RAYWHITE;
					textColor2 = RAYWHITE;
					textColor3 = GREEN;
					if (IsKeyPressed((KEY_ENTER)))
					{
						if (draw_info == false) draw_info = true;
						else draw_info = false;
					}
				}
				
				BeginDrawing();
					DrawTexturePro(menuText, (Rectangle) { 0, 0, menuText.width, menuText.height},
						(Rectangle) { 0, 0, screenWidth, screenHeight }, (Vector2) {0, 0}, 0.0f, WHITE);
					DrawText("Hilalao", screenWidth * 0.12, screenHeight * 0.45, screenWidth * 0.02, textColor1);
					DrawText("Hiala", screenWidth * 0.12, screenHeight * 0.5, screenWidth * 0.02, textColor2);
					DrawText("Momba ny mpamorona", screenWidth * 0.12, screenHeight * 0.8, screenWidth * 0.015, textColor3);
					DrawText("Casque/Ecouteur recommandé", screenWidth * 0.02, screenHeight * 0.95, screenWidth * 0.015, RAYWHITE);
					if (draw_info)
					{
            float rectWidth = screenWidth * 0.2f;
            float rectHeight = screenHeight * 0.2f;
            float rectX = (screenWidth * 0.51f) - (rectWidth / 2);
            float rectY = (screenHeight * 0.32f) - (rectHeight / 2);

						DrawRectangle(rectX, rectY, rectWidth, rectHeight, DARKGRAY);
						DrawRectangle(rectX + 5, rectY + 5, rectWidth - 10, rectHeight - 10, (Color) { 76, 63, 47, 200 });
					
            int fontSize = screenWidth * 0.02f;
            int lineSpacing = fontSize + 10;

            DrawText("RANDRIA Luca", rectX + 10, rectY + 10, fontSize, RAYWHITE);
            DrawText("Tranon-dolo project 2025", rectX + 10, rectY + 10 + lineSpacing, fontSize * 0.7, RAYWHITE);
            DrawText("Press ENTER to leave this info", rectX + 10, rectY + 10 + 3 * lineSpacing, fontSize * 0.6, RAYWHITE);
					}
				EndDrawing();
				break;
			}
      case (FINISH):
      {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Eny Tafavoaka ianao", screenWidth * 0.2, screenHeight * 0.2, 30, RAYWHITE);
        EndDrawing();
        break;
      }
		}
	}

		
	// De-Initialization
	//--------------------------------------------------------------------------------------
	UnloadImageColors(mapPixels);   // Unload color array
	UnloadTexture(cubicmap);        // Unload cubicmap texture
	UnloadTexture(texture);         // Unload map texture
	UnloadModel(model);             // Unload map model
	UnloadMusicStream(bg_music);
	UnloadSound(click);
	UnloadSound(lev1);
	UnloadSound(lev2);
	UnloadSound(lev3);
	UnloadSound(pas);
	UnloadSound(soupire);
  UnloadSound(dead);
  UnloadSound(close);
  UnloadTexture(doorTexture);
  UnloadModel(doorModel);

	CloseAudioDevice();
	CloseWindow();                  // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}

static float volume_of(Camera camera, float x, float z)
{
	float volume;
	float dist = Vector2Distance((Vector2) {camera.position.x, camera.position.z},(Vector2) {x, z});
	volume = 1 - (dist * 0.1);
	if (volume < 0) volume = 0;
	return (volume);
}
