#include "raylib.h"
#include "rcamera.h"
#include "raymath.h"

#include <stdio.h>

// Declaration of all globals variables
#define CAMERA_MOUSE_MOVE_SENSITIVITY	0.003f

static float volume_of(Camera camera, float x, float z);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 450;

	SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)
	InitWindow(screenWidth, screenHeight, "Tranon-dolo 3D");
	InitAudioDevice();

	// Define the camera to look into our 3d world
	Camera camera = { 0 };
	camera.position = (Vector3){ -12.0f, 0.6f, -7.3f };    // Camera position
	camera.target = (Vector3){ -11.55f, 1.4f, 6.85f };    // Camera looking at point
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 30.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	Image imMap = LoadImage("resources/cubicmap.png");      // Load cubicmap image (RAM)
	Texture2D cubicmap = LoadTextureFromImage(imMap);       // Convert image to texture to display (VRAM)
	Mesh mesh = GenMeshCubicmap(imMap, (Vector3){ 1.0f, 1.0f, 1.0f });
	Model model = LoadModelFromMesh(mesh);

	// NOTE: By default each cube is mapped to one part of texture atlas
	Texture2D texture = LoadTexture("resources/cubicmap_atlas.png");    // Load map texture
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;    // Set map diffuse texture

	// Get map image data to be used for collision detection
	Color *mapPixels = LoadImageColors(imMap);
	UnloadImage(imMap);             // Unload image from RAM

	Vector3 mapPosition = { -16.0f, 0.0f, -8.0f };  // Set model position

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

	DisableCursor();                // Limit cursor to relative movement inside the window

	SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	int decal = 0;

	// Head Bobbing
	float bobbingTimer = 0.0f;
	float bobbingSpeed = 10.0f;
	float bobbingAmount = 0.05f;

	Vector3 baseCameraPos = camera.position;

	static float movingTime = 0.0f;
	static bool soupirePlayed = false;

	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		bool isMoving = IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D);

		float dt = GetFrameTime();
		float cameraMoveSpeed = 1.5f * dt;
		double elpst = GetTime();
		int timer = 301 - (int)elpst;
		int minute = timer / 60;
		int seconde = timer % 60;
		float vol1 = volume_of(camera, -2.7f, -4.6f);
		float vol2 = volume_of(camera, 13.80f, -1.60f);

		// Update
		//----------------------------------------------------------------------------------
		SetSoundVolume(lev2, vol1 + 0.5);
		SetSoundVolume(lev3, vol2);
		if ((int)elpst - decal == 10)
		{
			PlaySound(lev2);
			PlaySound(lev3);
			decal += 10;
		}
		if (IsKeyPressed(KEY_L)) {
			// printf("posx: %0.2f, posy: %0.2f, posz: %0.2f\n", camera.position.x, camera.position.y, camera.position.z);
			// printf("distance: %i\n", (int)Vector2Distance((Vector2) {camera.position.x, camera.position.z},(Vector2) {-2.7f, -4.6f}));
			PlaySound(click);
			if (light) light = false;
			else light = true;
		}
		if ((int)elpst == 3) PlaySound(lev1);
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

		UpdateMusicStream(bg_music);
		SetMusicVolume(bg_music, 0.2);
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
		float currentPhase = fmodf(bobbingTimer, 2 * 3.14159f);
		float prevPhase = 0;  // à stocker également dans une variable statique ou globale
		SetSoundVolume(pas, 0.5);
		if ((prevPhase < stepAngleThreshold) && (currentPhase >= stepAngleThreshold))
			PlaySound(pas);
		prevPhase = currentPhase;

		// Keyboard support
        if (IsKeyDown(KEY_W)) CameraMoveForward(&camera, cameraMoveSpeed, 1);
        if (IsKeyDown(KEY_A)) CameraMoveRight(&camera, -cameraMoveSpeed, 1);
        if (IsKeyDown(KEY_S)) CameraMoveForward(&camera, -cameraMoveSpeed, 1);
        if (IsKeyDown(KEY_D)) CameraMoveRight(&camera, cameraMoveSpeed, 1);
		// Mouse support
		Vector2 mousePositionDelta = GetMouseDelta();
		CameraYaw(&camera, -mousePositionDelta.x*CAMERA_MOUSE_MOVE_SENSITIVITY, 0);
		CameraPitch(&camera, -mousePositionDelta.y*CAMERA_MOUSE_MOVE_SENSITIVITY, 1, 0, 0);

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
			EndMode3D();

			if (light) {
				DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color) { 10, 10, 10, 200 });
				DrawCircle(GetScreenWidth() / 2, GetScreenHeight(), 300, (Color) { 255, 244, 0, 15 });
			}
			else 
				DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color) { 10, 10, 10, 240 });

			DrawTextureEx(cubicmap, (Vector2){ GetScreenWidth() - cubicmap.width*4.0f - 20, 20.0f }, 0.0f, 4.0f, RED);
			DrawRectangleLines(GetScreenWidth() - cubicmap.width*4 - 20, 20, cubicmap.width*4, cubicmap.height*4, DARKGREEN);

			// Draw player position radar
			DrawRectangle(GetScreenWidth() - cubicmap.width*4 - 20 + playerCellX*4, 20 + playerCellY*4, 4, 4, YELLOW);

			DrawText(TextFormat("%02i:%02i", minute, seconde), 2, GetScreenHeight() - 30, 30, timerColor);
			DrawFPS(10, 10);

		EndDrawing();
		//----------------------------------------------------------------------------------
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