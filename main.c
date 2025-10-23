/*
 * Game name: Tranon-dolo project 2025
 * Autor: Luca RANDRIANIRINA
 * Made with Raylib 5
 */

#include "horror.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//--------------------------------------------------------------------------------------
	t_res res;
  t_hb hb;

  int screenWidth = 0;
	int screenHeight = 0;

	GameState currentState = MENU;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE); // Enable Multi Sampling Anti Aliasing 4x (if available)
	InitWindow(screenWidth, screenHeight, "Tranon-dolo 3D");
	screenWidth = GetScreenWidth();
	screenHeight = GetScreenHeight();
	SetExitKey(KEY_NULL);
	InitAudioDevice();

	LoadResources(&res);
  
  const  char *storyText =
   "Trano maizina tanteraka sy feno mistery no nifohazanao.\n\nTsy tsaroanao akory ny nahatongavanao tao.\n\n\n\n"
   "Tokana ny zavatra fantatrao:\n\n"
   "\tMisy fanalahidy miafina any ho any.\n\n\tNy fahitana izay fanalahidy izay no hany fanantenana hivoahana ao\n\nmialohan'ny hisehoan-javatra hafahafa na sanatria matotoa.\n\n\n\n"
   "TANDREMO! Tsy ianao irery no ao. Maro ny fanahy voafatotra ao izay tsy manaiky ny hivoahanao.\n\n"
   "Fohy ny fotoana ary ny segondra iray dia mampanantona anao amin'ny loza izay miha-mitombo.\n\n\n\n"
   "Tadiavo ilay fanalahidy. Mitandrema. Areheto sy vonoy arak'izay hilàna azy ny jironao.\n\n"
   "Henoy ny feo manodidina anao. Ary indrindra...AZA MITODIKA ANY AORINANAO.";

  int storyTextLength;
  int storyCurrentChar = 0;
  float storyTimeAccumulator = 0.0f;
  const float charDisplayTime = 0.03f; // 30 ms par caractère (ajustable)
  bool storyFullyDisplayed = false;

	// Define the camera to look into our 3d world
	Camera camera = {0};
	camera.position = (Vector3){-12.0f, 0.6f, -7.3f}; // Camera position
	camera.target = (Vector3){-11.55f, 1.4f, 6.85f};  // Camera looking at point
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};		  // Camera up vector (rotation towards target)
	camera.fovy = 30.0f;							  // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;			  // Camera projection type

  InitHeadBobbing(&hb, camera.position);
	
  Vector3 mapPosition = {-16.0f, 0.0f, -8.0f}; // Set model position

	// Initialisation du graine aléatoire.
	srand((unsigned int)time(NULL));

	// Ajout de clé à trouver aléatoirement et du porte de sortie dans le jeu
	Vector3 keyPosition = {0};
  keyPosition = RandomizeKeyPosition(&res, mapPosition);
 
  bool hasKey = false;
	float doorOpenAngle = 0.0f;
	bool doorOpening = false;
	const float doorOpenSpeed = 40.0f;
	Vector3 exitPosition = {11.0f, 0.5f, 6.4f};

	bool light = false;

	PlayMusicStream(res.bg_music);

	Color timerColor = WHITE;

	const int phraseCount = 4;
	const char *phrases[] = {
		"REHEFA HITA ILAY FANALAHIDY DIA TADIAVO NY FIVOAHANA.",
		"TSINDRIO NY <L> RAHA HAMONO NA HAMELONA JIRO.",
		"TANDREMO NY MITODITODIKA SAO HAHITA ANGANO.",
		"FOHY NY FOTOANA HANANANAO KA TADIAVO AINGANA NY FIVOAHANA."};

	int currentPhraseIndex = 0;
	float phraseTimer = 0.0f;

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	int decal = 0;
  int decal2 = 0;

  // Menu
	unsigned int on_what = 1;
	bool draw_info = false;

	double gameStartTime = 0;

	float ghostScale = 0.0f;
	const float ghostScaleSpeed = 1.0f; // Vitesse à laquelle il grandit (échelle par seconde)
	bool timerFinished = false;

	float torchBattery = 120.0f;

  double pauseStartTime = 0.0f;
  double totalPauseDuration = 0.0f;

  storyTextLength = (int)strlen(storyText);

	DisableCursor();
	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		UpdateMusicStream(res.bg_music);
		SetMusicVolume(res.bg_music, 0.2);
		float dt = GetFrameTime();

		switch (currentState)
		{
      case STORY:
      {
        UpdateAndDrawStory(storyText, storyTextLength, &storyCurrentChar, &storyTimeAccumulator, charDisplayTime, 
                           &storyFullyDisplayed, screenWidth, screenHeight, &currentState, &gameStartTime, &totalPauseDuration);
        break;
      }
			case GAME:
			{
				phraseTimer += dt;
				if (phraseTimer >= PHRASE_DURATION)
				{
					phraseTimer = 0.0f;
					currentPhraseIndex++;
					if (currentPhraseIndex >= phraseCount)
						currentPhraseIndex = 0;
				}
				double elpst = GetTime() - gameStartTime - totalPauseDuration;
				int timer = 300 - (int)elpst;
				int minute = timer / 60;
				int seconde = timer % 60;
				
        float vol1 = VolumeAtPosition(camera.position, -2.7f, -4.6f);
				float vol2 = VolumeAtPosition(camera.position, 13.80f, -1.60f);
        float keyVol = VolumeAtPosition(camera.position, keyPosition.x, keyPosition.z);
        float pleurVol = VolumeAtPosition(camera.position, -15.3f, 6.4f);
				SetSoundVolume(res.lev2, vol1 + 0.5);
				SetSoundVolume(res.lev3, vol2);
				SetSoundVolume(res.soupire, 1.5);
				SetSoundVolume(res.click, 0.2);
				SetSoundVolume(res.dead, 0.6);
				SetSoundVolume(res.close, 0.8);
        SetSoundVolume(res.keyPos, keyVol - 0.2);
        SetSoundVolume(res.pleur, (pleurVol - 0.5f < 0.f) ? 0.f : (pleurVol - 0.5f));

        if (minute == 0)
				{
					if (seconde == 1)
						PlaySound(res.dead);
					if (seconde == 0)
						timerFinished = true;
				}
				if (timerFinished)
				{
					ghostScale += ghostScaleSpeed * dt * (1.0f + ghostScale * 3.0f);
					if (ghostScale > GHOST_MAX_SCALE)
						ghostScale = GHOST_MAX_SCALE;
				}
				else
					ghostScale = 0.0f; // Si on veut, on peut reset la taille tant que le timer n'est pas fini

				// Calcul d'opacité pour rendre l'apparition plus fantomatique
				float ghostAlpha = ghostScale * 255.0f;
				if (ghostAlpha > 255)
					ghostAlpha = 255;

				// Oscillation légère de la position pour effet "tremblant"
				float oscillateOffset = 5.0f * sinf(GetTime() * 15.0f);
				if (ghostScale == GHOST_MAX_SCALE)
					currentState = OVER;

				// Centrer la texture
				Vector2 screenCenter = {screenWidth / 2.0f + oscillateOffset, screenHeight / 2.0f + oscillateOffset};
				Vector2 origin = {res.ghostText.width / 2.0f, res.ghostText.height / 2.0f};
				Rectangle sourceRec = {0, 0, (float)res.ghostText.width, (float)res.ghostText.height};
				Rectangle destRec = {screenCenter.x, screenCenter.y, res.ghostText.width * ghostScale, res.ghostText.height * ghostScale};
				Color ghostColor = (Color){255, 255, 255, (unsigned char)ghostAlpha};

				if ((int)elpst - decal == 10)
				{
					PlaySound(res.lev2);
					PlaySound(res.lev3);
          PlaySound(res.pleur);
					decal += 10;
				}
        if ((int)elpst - decal2 == 15 && !hasKey)
        {
          PlaySound(res.keyPos);
          decal2 += 15;
        }
				if ((int)elpst == 3)
					PlaySound(res.lev1);
				if ((int)elpst == 0)
					PlaySound(res.close);
				if (minute <= 0)
				{
					if (seconde < 10)
						timerColor = RED;
					if (seconde <= 0)
					{
						minute = 0;
						seconde = 0;
					}
				}
        UpdateTorchLight(&light, &torchBattery, dt, res.click);
        UpdateDoorOpening(&doorOpenAngle, &exitPosition, doorOpenSpeed, dt, doorOpening, &currentState);
        UpdatePlayerMovementAndCamera(&camera, &hb, dt, mapPosition, &res, &currentState, &pauseStartTime);

				//----------------------------------------------------------------------------------
				// Draw
				//----------------------------------------------------------------------------------
				BeginDrawing();

				ClearBackground(RAYWHITE);

				BeginMode3D(camera);
				DrawModel(res.model, mapPosition, 1.0f, WHITE); // Draw maze map
				DrawModelEx(res.doorModel, exitPosition, (Vector3){0.0f, 1.0f, 0.0f}, -doorOpenAngle, (Vector3){1.0f,1.0f,1.0f}, WHITE);
			  DrawCubeV((Vector3) { 11.0f, 0.5f, 6.5f }, (Vector3) { 1.0f, 1.0f, 0.1f }, BLACK);
        if (!hasKey)
					DrawSphere(keyPosition, 0.05f, (Color){255, 203, 0, 200});
				EndMode3D();
				
        DrawTorchLightEffect(light, torchBattery, screenWidth, screenHeight);

        float distToKey = Vector3Distance(camera.position, keyPosition);
				float distToExit = Vector3Distance(camera.position, exitPosition);
				if (!hasKey && distToKey < INTERACTION_DISTANCE)
				{
					DrawText("Tsindrio ny E raha handray fanalahidy", 10, screenHeight - 60, 20, YELLOW);
					if (IsKeyPressed(KEY_E))
					{
						PlaySound(res.click);
						hasKey = true;
					}
				}

				if (hasKey && distToExit < INTERACTION_DISTANCE)
				{
					DrawText("Tsindrio ny E raha hivoaka", 10, screenHeight - 60, 20, GREEN);
					if (IsKeyPressed(KEY_E) && !doorOpening)
					{
						PlaySound(res.click);
						doorOpening = true;
					}
				}
				else if (!hasKey && distToExit < INTERACTION_DISTANCE)
					DrawText("Mihidy ity varavarana ity, tadiavo ny fanalahidy", 10, screenHeight - 60, 20, RED);

				DrawText(phrases[currentPhraseIndex], 10, 10, 20, (Color){255, 255, 255, 120});

				// Assombrir l'écran de plus en plus selon la taille du ghost
				DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, (unsigned char)(ghostAlpha * 0.8f)});
				DrawTexturePro(res.ghostText, sourceRec, destRec, origin, 0.0f, ghostColor);

				DrawText(TextFormat("%02i:%02i", minute, seconde), 2, screenHeight - 30, 30, timerColor);

        DrawBatteryBar(torchBattery, screenWidth, screenHeight);

				EndDrawing();
				break;
			}
			case PAUSE:
			{
				if (IsKeyPressed(KEY_ENTER))
				{
					currentState = GAME;
					totalPauseDuration += GetTime() - pauseStartTime;
				}
				else if (IsKeyPressed(KEY_ESCAPE))
				{
					currentState = MENU;
					pauseStartTime = 0.0f;
					totalPauseDuration = 0.0f;
					StopMusicStream(res.bg_music);
					PlayMusicStream(res.bg_music);
				}

				BeginDrawing();
				const char *pauseText = "MATAHOTRA ANGAHA!!!";
        DrawTextInCenterX(pauseText, screenHeight / 3, screenWidth, 40, RED, true);
				const char *resumeText = "Tsindrio ny ENTER raha MAFY BE!";
				const char *quitText = "ESC raha matahotra!";
        DrawTextInCenterX(resumeText, screenHeight / 2, screenWidth, 20, LIGHTGRAY, true);
        DrawTextInCenterX(quitText, screenHeight / 2 + 40, screenWidth, 20, LIGHTGRAY, true);
				EndDrawing();
				break;
			}
			case OVER:
			{
				float oscillateOffset = 5.0f * sinf(GetTime() * 15.0f);
				Vector2 screenCenter = {screenWidth / 2.0f + oscillateOffset, screenHeight / 2.0f + oscillateOffset};
				Vector2 origin = {res.ghostText.width / 2.0f, res.ghostText.height / 2.0f};
				Rectangle sourceRec = {0, 0, (float)res.ghostText.width, (float)res.ghostText.height};
				Rectangle destRec = {screenCenter.x, screenCenter.y, res.ghostText.width * GHOST_MAX_SCALE, res.ghostText.height * GHOST_MAX_SCALE};
				Color ghostColor = (Color){255, 255, 255, (unsigned char)255};

				float bloodX = destRec.x + destRec.width / 2 - 300;
				float bloodY = destRec.y;
				Vector2 bloodOrigin = {res.bloodText.width / 2.0f, res.bloodText.height / 2.0f};
				Rectangle bloodSourceRec = {0, 0, (float)res.bloodText.width, (float)res.bloodText.height};
				Rectangle bloodDestRec = {bloodX, bloodY, (float)res.bloodText.width, (float)res.bloodText.height};
				Color bloodColor = WHITE;

        if (IsKeyPressed(KEY_ENTER)) currentState = MENU;

				BeginDrawing();
				ClearBackground(BLACK);
				DrawTexturePro(res.bloodText, bloodSourceRec, bloodDestRec, bloodOrigin, 0.0f, bloodColor);
				DrawTexturePro(res.ghostText, sourceRec, destRec, origin, 0.0f, ghostColor);
	      DrawTextInCenterX("Tsindrio ny ENTER raha hiverina any amin'ny MENU", screenHeight - (screenHeight * 0.02), screenWidth, screenHeight * 0.02, WHITE, true);
				EndDrawing();
				break;
			}
			case MENU:
			{
				// Define the camera to look into our 3d world
				camera.position = (Vector3){-12.0f, 0.6f, -7.3f}; // Camera position
				camera.target = (Vector3){-11.55f, 1.4f, 6.85f};  // Camera looking at point
				camera.up = (Vector3){0.0f, 1.0f, 0.0f};		  // Camera up vector (rotation towards target)
				camera.fovy = 30.0f;							  // Camera field-of-view Y
				camera.projection = CAMERA_PERSPECTIVE;			  // Camera projection type

				light = false;
				torchBattery = 120.0f;
        hasKey = false;
        doorOpening = false;
        doorOpenAngle = 0.0f;
        exitPosition = (Vector3){11.0f, 0.5f, 6.4f};

        keyPosition = RandomizeKeyPosition(&res, mapPosition);

        UpdateAndDrawMenu(&res, screenWidth, screenHeight, &on_what, &draw_info, &currentState, &storyCurrentChar, &storyFullyDisplayed, &gameStartTime);
				break;
			}
			case (FINISH):
			{
        UpdateAndDrawFinish(screenWidth, screenHeight, res.cry, &currentState);
				break;
			}
		}
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	UnloadResources(&res);
	CloseAudioDevice();
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
