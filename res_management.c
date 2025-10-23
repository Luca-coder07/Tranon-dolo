#include "horror.h"

void InitHeadBobbing(t_hb *hb, Vector3 camPos)
{
	hb->bobbingTimer = 0.0f;
	hb->bobbingSpeed = 10.0f;
	hb->bobbingAmount = 0.05f;

	hb->baseCameraPos = camPos;

	hb->movingTime = 0.0f;
	hb->soupirePlayed = false;
}

void LoadResources(t_res *res)
{
    Image menuImg = LoadImage("resources/screen.jpg");
    res->menuText = LoadTextureFromImage(menuImg);
    UnloadImage(menuImg);

    Image ghostImg = LoadImage("resources/ghost.png");
    res->ghostText = LoadTextureFromImage(ghostImg);
    UnloadImage(ghostImg);

    Image bloodImg = LoadImage("resources/blood.jpg");
    res->bloodText = LoadTextureFromImage(bloodImg);
    UnloadImage(bloodImg);

    Image imMap = LoadImage("resources/cubicmap.png");
    res->cubicmap = LoadTextureFromImage(imMap);

    Mesh mesh = GenMeshCubicmap(imMap, (Vector3){1.0f, 1.0f, 1.0f});
    res->model = LoadModelFromMesh(mesh);

    res->texture = LoadTexture("resources/cubicmap_atlas.png");
    res->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = res->texture;

    res->doorTexture = LoadTexture("resources/door.jpg");
    Mesh doorMesh = GenMeshCube(1.0f, 1.0f, 0.1f);
    res->doorModel = LoadModelFromMesh(doorMesh);
    res->doorModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = res->doorTexture;

    res->mapPixels = LoadImageColors(imMap);
    UnloadImage(imMap);

    res->bg_music = LoadMusicStream("resources/bg_music.mp3");

    res->click = LoadSound("resources/click.ogg");
    res->lev1 = LoadSound("resources/ALALANA AVY AIZA.mp3");
    res->lev2 = LoadSound("resources/VONJEO.mp3");
    res->lev3 = LoadSound("resources/MITADY HITSOAKA.mp3");
    res->pas = LoadSound("resources/pas.mp3");
    res->soupire = LoadSound("resources/MISEFOSEFO.mp3");
    res->dead = LoadSound("resources/dead.mp3");
    res->close = LoadSound("resources/closedoorcreak.mp3");
    res->keyPos = LoadSound("resources/keypos.mp3");
    res->pleur = LoadSound("resources/pleur.mp3");
    res->cry = LoadSound("resources/cry.mp3");
}

void UnloadResources(t_res *res)
{
    UnloadImageColors(res->mapPixels);
    UnloadTexture(res->cubicmap);
    UnloadTexture(res->texture);
    UnloadModel(res->model);
    UnloadTexture(res->doorTexture);
    UnloadModel(res->doorModel);

    UnloadMusicStream(res->bg_music);

    UnloadSound(res->click);
    UnloadSound(res->lev1);
    UnloadSound(res->lev2);
    UnloadSound(res->lev3);
    UnloadSound(res->pas);
    UnloadSound(res->soupire);
    UnloadSound(res->dead);
    UnloadSound(res->close);
    UnloadSound(res->keyPos);
    UnloadSound(res->pleur);
    UnloadSound(res->cry);

    UnloadTexture(res->menuText);
    UnloadTexture(res->ghostText);
    UnloadTexture(res->bloodText);
}
