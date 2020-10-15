#include "interface.h"
#include "gamelib.h"

#define _1299EXIT_FAILURE 1
#define _1299EXIT_SUCCESS 0
#define CAMERA_MOVEMENT_SPEED 2.5
#define CAMERA_CATCH 20
#define SETTINGS_LOW
#define FULLSCREEN 0

#ifdef SETTINGS_LOW
#define QUALITY_SETTING 1
#endif

#ifdef SETTINGS_MEDIUM
#define QUALITY_SETTING 2
#endif

#ifdef SETTINGS_HIGH
#define QUALITY_SETTING 3
#endif

bool ssao, bloom, antialias, dof, fog, godrays;
int aFilter, tFilter;
TGame game;	// general info about the scene, like screen resolution
int mouseX, mouseY;
bool vSync;
bool firstHit = true;
bool selectionMode = false;
int selectionStartX;
int selectionStartY;
TTexture mouseTexture;
TTexture uiConsoleTexture;
TTexture uiLoadingScreen1;
TTexture fowTerrainTexture;

TTexture selectionBright;
TTexture selectionDark;

void SetEffects()
{
#ifdef SETTINGS_HIGH
	aFilter    = MAXINT;
	tFilter    = 1;
	ssao       = true;
	bloom      = true;
	antialias  = true;
	dof        = false;
	fog        = false;
	godrays    = true;
	vSync      = true;
#endif

#ifdef SETTINGS_LOW
	aFilter    = 0;
	tFilter    = 0;
	ssao       = false;
	bloom      = false;
	antialias  = false;
	dof        = false;
	fog        = false;
	godrays    = false;
	vSync      = false;
#endif

	game.scene.GetFramewerk().SetStats(true);
#ifdef SETTINGS_HIGH
	game.scene.GetFramewerk().GetRenderer().SetReflectionElements(ENTITY_ALL);
#endif

#ifdef SETTINGS_LOW
	game.scene.GetFramewerk().GetRenderer().SetReflectionElements(ENTITY_TERRAIN);
#endif

	game.scene.GetFramewerk().GetRenderer().SetAntialias(antialias);
	game.scene.GetFramewerk().GetRenderer().SetSSAO(ssao);
	game.scene.GetFramewerk().GetRenderer().SetGodRays(godrays);
	game.scene.GetFramewerk().GetRenderer().SetBloom(bloom);		

	if(aFilter == MAXINT)
		AFilter(MaxAFilter());
	else
		AFilter(aFilter);

	TFilter(tFilter);

	//game.scene.GetFramewerk().GetRenderer().SetDistanceFogColor(Vec4(0.9f,0.9f,1,0.96f));
	//game.scene.GetFramewerk().GetRenderer().SetDistanceFogAngle(Vec2(0,10));
	//game.scene.GetFramewerk().GetRenderer().SetNearDOFRange(Vec2(0,5));
	//game.scene.GetFramewerk().GetRenderer().SetFarDOFRange(Vec2(50,1000));
	//game.scene.GetFramewerk().GetRenderer().SetNearDOF(dof);
	//game.scene.GetFramewerk().GetRenderer().SetFarDOF(dof);
}

void SetCamera()
{
	game.scene.campos = Vec3(0, 70, 0);
	game.scene.camrot = Vec3(60,-45,0);
	game.scene.SetupCamera();
}

void InitializeGraphics()
{
	if(!Initialize()) exit(_1299EXIT_FAILURE);
	RegisterAbstractPath("C:/Software/multimedia/1299 New/Abstract");
	
	SetAppTitle("1299: Bir Efsanenin Doðuþu");
 
	game.Initialize(1024,768,FULLSCREEN);

	mouseTexture = LoadTexture("abstract::cursor.dds");
	uiConsoleTexture = LoadTexture("abstract::ui_new.dds");
	uiLoadingScreen1 = LoadTexture("abstract::ui_loading1.dds");

	selectionBright = LoadTexture("abstract::blank_green.dds");
	selectionDark = LoadTexture("abstract::blank_green_alpha.dds");

	fowTerrainTexture = LoadTexture("abstract::fowBase.dds");
}

void InitializeWorld()
{
	SetEffects();
	SetWorldSize(Vec3(50000));

	MoveMouse( game.scene.cx, game.scene.cy );
	HideMouse();

	mouseX = GraphicsWidth()/ 2;
	mouseY = GraphicsHeight()/ 2;
}

void LoadLevel()
{
	DrawImage(uiLoadingScreen1, 0, 0);
	Flip(vSync);
	
	game.scene.LoadMap("abstract::part1_level1.sbx", QUALITY_SETTING);

	SetTerrainTexture(game.scene.terrain, fowTerrainTexture, 0, 2);
	SetTerrainTextureScale(game.scene.terrain, game.scene.terrainresolution* 2, 2);

	for(int i = 0; i< 32; i++)
		for(int j = 0; j< 32; j++)
			BlendTerrainTexture(game.scene.terrain, i, j, 0.1f, 2);

	BlendTerrainTexture(game.scene.terrain, 48, 48, 0.1f, 2);

	for(int i = 92; i< 128; i++)
		for(int j = 92; j< 128; j++)
			BlendTerrainTexture(game.scene.terrain, i, j, 0.1f, 2);
}

int InitializeGame()
{
	InitializeGraphics();
	InitializeWorld();
	LoadLevel();
	SetCamera();

	return 1;
}

void DisplayGUI()
{
	SetBlend(BLEND_ALPHA);
	DrawImage(uiConsoleTexture, 0, 0);
	SetBlend(BLEND_NONE);
}

void DisplaySelection()
{
	SetBlend(BLEND_ALPHA);

	// display selection
	if(MouseDown(1))
	{
		selectionMode = true;
		if(firstHit)
		{
			firstHit = false;
			selectionStartX = mouseX;
			selectionStartY = mouseY;
		}

		int x1 = selectionStartX;
		int x2 = mouseX;
		int y1 = selectionStartY;
		int y2 = mouseY;

		if(x2 == x1+ 1)
			x2++;

		if(x1 == x2+ 1)
			x1++;

		if(y2 == y1+ 1)
			y2++;

		if(y1 == y2+ 1)
			y1++;

		DrawImage(selectionBright, x1, y1, 2, y2- y1);
		DrawImage(selectionBright, x1, y1, x2- x1, 2);
		DrawImage(selectionBright, x2, y1, 2, y2- y1);
		DrawImage(selectionBright, x1, y2, x2- x1, 2);

		DrawImage(selectionDark, x1+ 1, y1+ 1, (x2- x1)- 2, (y2- y1)- 2);

#ifdef _SELECTION_DEBUG
		DrawText(10, 30, "x1: %ld", x1);
		DrawText(10, 50, "y1: %ld", y1);
		DrawText(10, 70, "x2: %ld", x2);
		DrawText(10, 90, "y2: %ld", y2);
#endif
	}
	else
	{
		selectionMode = false;
		firstHit = true;
	}

	SetBlend(BLEND_NONE);
}

void DisplayMouse()
{
	SetBlend(BLEND_ALPHA);

	// display mouse cursor
	DrawImage(mouseTexture, mouseX, mouseY);

	SetBlend(BLEND_NONE);

	mouseX += MouseX()- game.scene.cx;
	mouseY += MouseY()- game.scene.cy;
	static int lastMouseZ = 0;
	static float mouseZ = 0;
	
	if(lastMouseZ != MouseZ())
	{
		mouseZ += (lastMouseZ- MouseZ())/ 10.0f;

		if(mouseZ <= -1.0f) mouseZ = -1.0f;
		if(mouseZ >= 0.0f) mouseZ = 0.0f;
		game.camzoom = 1.0- mouseZ;

		game.scene.GetFramewerk().SetZoom(game.camzoom);
		game.scene.GetFramewerk().GetMain().GetCamera().SetZoom(game.camzoom);

		if(game.scene.skycam) game.scene.GetFramewerk().GetBackground().GetCamera().SetZoom(game.camzoom);
		if(game.scene.foregroundcam) game.scene.GetFramewerk().GetTransparency().GetCamera().SetZoom(game.camzoom);

		lastMouseZ = MouseZ();
	}

#ifdef _DISPLAY_MOUSE_DEBUG
	DrawText(10, 50, "Mouse (X/Y): %lf, %lf", game.scene.campos.X, game.scene.campos.Z);	

	DrawText(10, 10, "Game Cam Zoom: %lf", game.camzoom);
	DrawText(10, 30, "mouseZ (var):  %lf", mouseZ);
	DrawText(10, 50, "MouseZ (func): %ld", MouseZ());
	DrawText(10, 70, "lastMouseZ:    %ld", lastMouseZ);
#endif

	if(mouseX< 0) mouseX = 0;
	if(mouseY< 0) mouseY = 0;
	if(mouseX> GraphicsWidth()- 1)  mouseX = GraphicsWidth()- 1;
	if(mouseY> GraphicsHeight()- 1) mouseY = GraphicsHeight()- 1;

	if(!selectionMode && mouseX <= CAMERA_CATCH)
	{
		game.scene.campos.X -= CAMERA_MOVEMENT_SPEED;
		game.scene.campos.Z += CAMERA_MOVEMENT_SPEED;
	}

	if(!selectionMode && mouseX >= GraphicsWidth()- CAMERA_CATCH)
	{
		game.scene.campos.X += CAMERA_MOVEMENT_SPEED;
		game.scene.campos.Z -= CAMERA_MOVEMENT_SPEED;
	}

	if(!selectionMode && mouseY <= CAMERA_CATCH)
	{
		game.scene.campos.X += CAMERA_MOVEMENT_SPEED;
		game.scene.campos.Z += CAMERA_MOVEMENT_SPEED;
	}

	if(!selectionMode && mouseY >= GraphicsHeight()- CAMERA_CATCH)
	{
		game.scene.campos.X -= CAMERA_MOVEMENT_SPEED;
		game.scene.campos.Z -= CAMERA_MOVEMENT_SPEED;
	}

	if(game.scene.campos.X >= 50.0f)
	{
		game.scene.campos.X = 50.0f;
	}
	
	if(game.scene.campos.X <= -115.0f)
	{
		game.scene.campos.X = -115.0f;
	}

	if(game.scene.campos.Z >= 50.0f)
	{
		game.scene.campos.Z = 50.0f;
	}
	
	if(game.scene.campos.Z <= -115.0f)
	{
		game.scene.campos.Z = -115.0f;
	}

	game.scene.SetupCamera();
	MoveMouse( game.scene.cx, game.scene.cy );
}

void MainLoop()
{
	game.scene.GetFramewerk().SetZoom(1);
	game.camzoom = 1;
	while(!KeyHit())
		{
			game.scene.Update();
	 
			if(!game.IsSuspended())
			{
				// Proceed to next frame
				game.scene.Render();

				DisplaySelection();
				DisplayGUI();
				DisplayMouse();
				Flip(vSync);
			}
		}
}