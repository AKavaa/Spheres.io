// Assignment_1.cpp: A program using the TL-Engine

// Aleksander Kavaleuskiy
#include <TL-Engine.h>
#include <sstream>
#include <time.h>

using namespace tle;
using namespace std;

/// <summary>
///  Here are the declared variables and booleans which are used in the mainloop
/// </summary>

const int numCubes = 13;
const float KSphereSpeed = 1.8f;
const float KRotateSpeed = 1.8f;
const float waterLimit = 100.0f;
const float sphereRadius = 12.5f;

bool isometric = false;
bool gameWon = false;
bool gameLost = false;

const float sphereHalfWidth = 6.0f;
const float sphereHalfHeight = 6.0f;
const float sphereHalfDepth = 6.0f;

const float miniCubeHalfWidth = 2.5f;
const float miniCubeHalfHeight = 2.5f;
const float miniCubeHalfDepth = 2.5f;

const float enemysphereHalfWidth = 6.0f;
const float enemysphereHalfHeight = 6.0f;
const float enemysphereHalfDepth = 6.0f;

bool isHyperMode = false;
float hyperModeTime = 0.0f;
IModel *hyperCube = nullptr;
const float hyperModeDuration = 3.5f;
const float attractingCubes = 50.0f;
const float attractingCubesSpeed = 2.5f;

/// <summary>
/// here i have the spawning of the cubes and the hyperCube, i randomize the x and z coordinates
/// so the cubes appear at random position
/// </summary>
/// <param name="minicube"></param>
/// <param name="cubes"></param>
/// <param name="numCubes"></param>
/// <param name="sphere"></param>
/// <param name="hyperCube"></param>
void cubeSpawn(IMesh *minicube, IModel *cubes[], int numCubes, IModel *sphere, IModel *&hyperCube)
{
    for (int i = 0; i < numCubes; i++)
    {

        bool validPosition;
        float x, y = 2.0, z;

        do
        {
            x = rand() % 200 - 100; // adding random positions to cubes
            z = rand() % 200 - 100; // adding random positions to cubes

            validPosition = true;

            // calculating the distance from the sphere
            float dxSphere = x - sphere->GetX();
            float dzSphere = z - sphere->GetZ();

            float distanceToSphere = sqrt(dxSphere * dxSphere + dzSphere * dzSphere);

            // if its too close to the sphere, the positions is invalid
            if (distanceToSphere < 10.0f + sphereRadius)
            {
                validPosition = false;
                continue;
            }

            for (int j = 0; j < numCubes; j++)
            {
                // check if the cube is not null
                if (cubes[j] != nullptr && cubes[j] != cubes[i])
                {
                    float dxCube = x - cubes[j]->GetX();
                    float dzCube = z - cubes[j]->GetZ();

                    float distanceToCube = sqrt(dxCube * dxCube + dzCube * dzCube);

                    if (distanceToCube < 10.0f)
                    {
                        validPosition = false;
                        break;
                    }
                }
            }

        } while (!validPosition); // repeating the proccess until a valid position for the cubes
        // to spawn is found
        cubes[i] = minicube->CreateModel(x, y, z); // creates the minicube at the x, y, z position

        if (hyperCube == nullptr)
        {
            float x = rand() % 200 - 100;
            float z = rand() % 200 - 100;
            hyperCube = minicube->CreateModel(x, y, z);
            hyperCube->SetSkin("hypercube.jpg");
        }
    }
}
/// <summary>
/// Implementation of the Pause button when Key P is pressed
/// </summary>
/// <param name="myEngine"></param>
/// <param name="gamePaused"></param>
/// <param name="myFont"></param>
void Paused(I3DEngine *myEngine, bool &gamePaused, IFont *myFont)
{
    if (myEngine->KeyHit(Key_P))
    {
        gamePaused = !gamePaused;
    }
}
/// <summary>
/// A box to box collision between the sphere and the minicubes, this code was done in the week 8 labs
/// </summary>
/// <param name="sphere"></param>
/// <param name="minicube"></param>
/// <returns></returns>
bool sphere_to_minicubes_boxCollision(IModel *sphere, IModel *minicube)
{
    float sphereMinX = sphere->GetX() - sphereHalfWidth;
    float sphereMaxX = sphere->GetX() + sphereHalfWidth;

    float sphereMinY = sphere->GetY() - sphereHalfHeight;
    float sphereMaxY = sphere->GetY() + sphereHalfHeight;

    float sphereMinZ = sphere->GetZ() - sphereHalfDepth;
    float sphereMaxZ = sphere->GetZ() + sphereHalfDepth;

    float minicubeMinX = minicube->GetX() - miniCubeHalfWidth;
    float minicubeMaxX = minicube->GetX() + miniCubeHalfWidth;

    float minicubeMinY = minicube->GetY() - miniCubeHalfHeight;
    float minicubeMaxY = minicube->GetY() + miniCubeHalfHeight;

    float minicubeMinZ = minicube->GetZ() - miniCubeHalfDepth;
    float minicubeMaxZ = minicube->GetZ() + miniCubeHalfDepth;

    bool collisionX = (minicubeMaxX >= sphereMinX) && (minicubeMinX <= sphereMaxX);
    bool collisionY = (minicubeMaxY >= sphereMinY) && (minicubeMinY <= sphereMaxY);
    bool collisionZ = (minicubeMaxZ >= sphereMinZ) && (minicubeMinZ <= sphereMaxZ);

    if (collisionX && collisionY && collisionZ)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/// <summary>
/// here we check if the sphere leaves the island and reaches or touches the water, the game freezes and a gamelost message appears
/// Basically this code checks the boundaries or the limits of the island ground so when the sphere contacts with the limit of the boundary
/// the game freezes
/// /// </summary>
/// <param name="sphere"></param>
/// <param name="waterLimit"></param>
/// <param name="myEngine"></param>
/// <param name="gameLost"></param>
void sphere_to_water_collision(IModel *sphere, const float waterLimit, I3DEngine *myEngine, bool &gameLost)
{
    float SphereX = sphere->GetX();
    float SphereZ = sphere->GetZ();

    if (SphereX < -waterLimit || SphereX > waterLimit || SphereZ < -waterLimit || SphereZ > waterLimit)
    {
        gameLost = true; // checks whether the x and z coordinates are outside the boundaries
    }
}

/// <summary>
/// Controls of the camera, by pressing the key 2, it changes the angle of the camera and when the key 1 is pressed the camera is set back to its
/// initial position
/// keys up,down,left,right are responsible for extra camera movement
/// </summary>
/// <param name="myCamera"></param>
/// <param name="myEngine"></param>
void cameraControls(ICamera *myCamera, I3DEngine *myEngine)
{
    if (!gameWon)
    {
        if (!isometric)
        {

            if (myEngine->KeyHeld(Key_Up))
            {
                myCamera->MoveZ(1.0f); // moves up
            }

            if (myEngine->KeyHeld(Key_Down))
            {
                myCamera->MoveZ(-1.0f); // movs down
            }

            if (myEngine->KeyHeld(Key_Right))
            {
                myCamera->MoveX(1.0f); // moves to the right
            }

            if (myEngine->KeyHeld(Key_Left))
            {
                myCamera->MoveX(-1.0f); // moves to the left
            }
        }
        if (myEngine->KeyHit(Key_2))
        {
            myCamera->ResetOrientation();          // resets the camera
            myCamera->SetPosition(150, 150, -150); // puts it in the given position
            myCamera->RotateLocalY(-45);
            myCamera->RotateLocalX(45);
            !isometric;
        }
        if (myEngine->KeyHit(Key_1))
        {
            myCamera->ResetOrientation();
            myCamera->SetPosition(0, 200, 0);
            myCamera->RotateX(90);
            isometric;
        }
    }
}

/// <summary>
/// SPhere controls with the W,A,S,D keys
/// </summary>
/// <param name="sphere"></param>
/// <param name="myEngine"></param>
void sphereControls(IModel *sphere, I3DEngine *myEngine, float &frameTime)
{
    if (!gameWon)
    {

        if (myEngine->KeyHeld(Key_W))
        {
            sphere->MoveLocalZ((KSphereSpeed)*frameTime);
        }
        if (myEngine->KeyHeld(Key_S))
        {
            sphere->MoveLocalZ((-KSphereSpeed) * frameTime);
        }

        if (myEngine->KeyHeld(Key_A))
        {
            sphere->RotateLocalY((-KRotateSpeed) * frameTime);
        }

        if (myEngine->KeyHeld(Key_D))
        {
            sphere->RotateLocalY((KRotateSpeed)*frameTime);
        }
    }
}
/// <summary>
/// when the collision between the sphere and the minicubes is happening the player is awarded 10 points for each eaten minicube
/// </summary>
/// <param name="sphere"></param>
/// <param name="minicubes"></param>
/// <param name="playerPoints"></param>
/// <param name="myEngine"></param>
/// <param name="myFont"></param>
void player_points(IModel *sphere, IModel *minicubes, int &playerPoints, I3DEngine *myEngine, IFont *myFont)
{
    if (!sphere_to_minicubes_boxCollision(sphere, minicubes))
    {

        // The code for displaying the score was found in the week labs 3-4
        stringstream playerScore;
        playerScore << "User's Point : 0" << playerPoints << endl;
        playerPoints += 10;
        myFont->Draw(playerScore.str(), 1050, 20, kDarkGrey);
        playerScore.str("");
    }
}
/// <summary>
/// When the game reaches 120 points the game is won
/// </summary>
/// <param name="playerPoints"></param>
/// <param name="myFont"></param>
/// <param name="gameWon"></param>
void GameWon(int playerPoints, IFont *myFont, bool &gameWon)
{
    if (playerPoints >= 120 && !gameWon)
    {
        gameWon = true;
    }
}
/// <summary>
/// if game is won, a game won message is appeared
/// </summary>
/// <param name="myFont"></param>
void GameWonMessage(IFont *myFont)
{
    myFont->Draw("Congrats,You Won!", 640, 320, kBlack, kCentre, kVCentre);
}
/// <summary>
/// if game is lost, a game lost message is appeared
/// </summary>
/// <param name="myFont"></param>
void GameLostMessage(IFont *myFont)
{
    myFont->Draw("You Lost...", 640, 320, kRed, kCentre, kVCentre);
}

void GamePausedMessage(IFont *myFont)
{
    myFont->Draw("Game Paused", 640, 320, kRed, kCentre, kVCentre);
}

// <summary>
/// sphere to hypercube collision is pretty much the same as the box to box collision which was seen before
/// </summary>
/// <param name="sphere"></param>
/// <param name="hypercube"></param>
/// <returns></returns>
bool sphere_to_hypercube_collision(IModel *sphere, IModel *hypercube)
{

    float sphereMinX = sphere->GetX() - sphereHalfWidth;
    float sphereMaxX = sphere->GetX() + sphereHalfWidth;

    float sphereMinY = sphere->GetY() - sphereHalfHeight;
    float sphereMaxY = sphere->GetY() + sphereHalfHeight;

    float sphereMinZ = sphere->GetZ() - sphereHalfDepth;
    float sphereMaxZ = sphere->GetZ() + sphereHalfDepth;

    float hyperCubeMinX = hyperCube->GetX() - sphereHalfWidth;
    float hyperCubeMaxX = hyperCube->GetX() + sphereHalfWidth;

    float hyperCubeMinY = hyperCube->GetY() - sphereHalfHeight;
    float hyperCubeMaxY = hyperCube->GetY() + sphereHalfHeight;

    float hyperCubeMinZ = hyperCube->GetZ() - sphereHalfDepth;
    float hyperCubeMaxZ = hyperCube->GetZ() + sphereHalfDepth;

    return (hyperCubeMaxX >= sphereMinX && hyperCubeMinX <= sphereMaxX) && (hyperCubeMaxY >= sphereMinY && hyperCubeMinY <= sphereMaxY) && (hyperCubeMaxZ >= sphereMinZ && hyperCubeMinZ <= sphereMaxZ);
}
/// <summary>
/// for each cube in the array, the code checks if the cube exists, basically its not null
/// after calculating how far the cubes are from the sphere by measuring the distance of x and z coordinates
/// if the cubes are in a certain distance, the attractingCubes variable attarcts the cubes closer to the sphere
/// </summary>
/// <param name="sphere"></param>
/// <param name="cubes"></param>
/// <param name="numCubes"></param>
/// <param name="speed"></param>
void miniCubesAttract(IModel *sphere, IModel *cubes[], int numCubes, float speed = 2.0f)
{
    for (int i = 0; i < numCubes; i++)
    {
        if (cubes[i] != nullptr)
        {
            float dx = sphere->GetX() - cubes[i]->GetX();
            float dz = sphere->GetZ() - cubes[i]->GetZ();
            float distance = sqrt(dx * dx + dz * dz);

            if (distance < attractingCubes)
            {
                cubes[i]->MoveX(dx / distance * speed);
                cubes[i]->MoveZ(dz / distance * speed);
            }
        }
    }
}

/// <summary>
/// adding the enemysphere into the game
/// </summary>
/// <param name="enemySphere"></param>
/// <param name="enemySphereMesh"></param>
/// <param name="myEngine"></param>
void enemy_Sphere(IModel *&enemySphere, IMesh *&enemySphereMesh, I3DEngine *myEngine)
{

    if (enemySphereMesh == nullptr)
    {

        enemySphereMesh = myEngine->LoadMesh("Sphere.x");

        if (enemySphere == nullptr)
        {
            enemySphere = enemySphereMesh->CreateModel(rand() % 200 - 100, 5, rand() % 200 - 100);
            enemySphere->SetSkin("enemysphere.jpg");
        }
    }
}

const float boundaryX = 95.0f;
const float boundaryZ = 95.0f;

/// <summary>
/// Here is the movement of the enemy sphere
/// boundaries are set so the sphere doesnt leave the island
/// the velocity variables are for maintaining a smoother movement to the sphere
///  the moveX and moveZ determine how much the sphere will move on the Y and Z axis, there are random numbers between 2 and -2 and multiplied by the speed
/// </summary>
/// <param name="enemySphere"></param>
/// <param name="cubes"></param>
/// <param name="numCubes"></param>
/// <param name="speed"></param>
void enemySphereMove(IModel *enemySphere, IModel *cubes[], int numCubes, float &frameTime, float speed = 1.8f)
{

    float currentX = enemySphere->GetX(); // current X position of the sphere
    float currentZ = enemySphere->GetZ(); // current Z position of the sphere

    float moveX = (rand() % 5 - 2) * speed * frameTime;
    float moveZ = (rand() % 5 - 2) * speed * frameTime;

    static float velocityX = 0.0f;
    static float velocityZ = 0.0f;

    float accelerate = 0.3f;
    float MaxSpeed = 2.7f;

    velocityX += moveX * accelerate * frameTime; // updating the velocity and adding a movement with the accelaration
    velocityZ += moveZ * accelerate * frameTime; // updating the velocity and adding a movement with the accelaration

    velocityX *= 0.97;
    velocityZ *= 0.97;

    // making the velocity to not exceed the maxSpeed
    if (velocityX > MaxSpeed)
        velocityX = MaxSpeed;
    if (velocityX < -MaxSpeed)
        velocityX = -MaxSpeed;

    if (velocityZ > MaxSpeed)
        velocityZ = MaxSpeed;
    if (velocityZ < -MaxSpeed)
        velocityZ = -MaxSpeed;

    // moving the enemy sphere along the Z and X axes
    enemySphere->MoveX(velocityX);
    enemySphere->MoveZ(velocityZ);

    // checking the boundaries so the enemySphere doesnt go beyong them
    if (currentX + velocityX > boundaryX || currentX + velocityX < -boundaryX)
    {
        velocityX = -velocityX;
    }
    if (currentZ + velocityZ > boundaryZ || currentZ + velocityZ < -boundaryZ)
    {
        velocityZ = -velocityZ;
    }
}

/// <summary>
/// collision of the enemy sphere with th minicubes, the same procedure was used also with the other collisions
/// </summary>
/// <param name="enemySphere"></param>
/// <param name="minicube"></param>
/// <returns></returns>
bool enemySphere_collision_minicubes(IModel *enemySphere, IModel *minicube)
{

    float enemysphereMinX = enemySphere->GetX() - enemysphereHalfWidth;
    float enemysphereMaxX = enemySphere->GetX() + enemysphereHalfWidth;

    float enemysphereMinY = enemySphere->GetY() - enemysphereHalfHeight;
    float enemysphereMaxY = enemySphere->GetY() + enemysphereHalfHeight;

    float enemysphereMinZ = enemySphere->GetZ() - enemysphereHalfDepth;
    float enemysphereMaxZ = enemySphere->GetZ() + enemysphereHalfDepth;

    float minicubeMinX = minicube->GetX() - miniCubeHalfWidth;
    float minicubeMaxX = minicube->GetX() + miniCubeHalfWidth;

    float minicubeMinY = minicube->GetY() - miniCubeHalfHeight;
    float minicubeMaxY = minicube->GetY() + miniCubeHalfHeight;

    float minicubeMinZ = minicube->GetZ() - miniCubeHalfDepth;
    float minicubeMaxZ = minicube->GetZ() + miniCubeHalfDepth;

    bool collisionX = (minicubeMaxX >= enemysphereMinX) && (minicubeMinX <= enemysphereMaxX);
    bool collisionY = (minicubeMaxY >= enemysphereMinY) && (minicubeMinY <= enemysphereMaxY);
    bool collisionZ = (minicubeMaxZ >= enemysphereMinZ) && (minicubeMinZ <= enemysphereMaxZ);

    if (collisionX && collisionY && collisionZ)
    {
        return false;
    }
    else
    {
        return true;
    }
}
/// <summary>
/// more accurate and continuous collision of the enemy sphere, when the enemysphere collides withe the cubes, the position of the cube changes
/// and it is appeared elsewhere in the map
/// </summary>
/// <param name="enemySphere"></param>
/// <param name="cubes"></param>
/// <param name="numCubes"></param>
/// <param name="myEngine"></param>
/// <returns></returns>
bool Handle_enemySphere_Collision(IModel *enemySphere, IModel *cubes[], int numCubes, I3DEngine *myEngine)
{

    bool collisionHappened = false;
    for (int i = 0; i < numCubes; i++)
    {
        if (cubes[i] != nullptr && !enemySphere_collision_minicubes(enemySphere, cubes[i]))
        {

            float newPosX = rand() % 200 - 100;
            float newPosZ = rand() % 200 - 100;

            cubes[i]->SetPosition(-900, -900, -900);

            myEngine->Timer();
            cubes[i]->SetPosition(newPosX, 5, newPosZ);

            return true;
        }
    }
    return false;
}

/// <summary>
/// same collision was seen before, enemysphere collision with the hyperCube
/// </summary>
/// <param name="enemySphere"></param>
/// <param name="hypeCube"></param>
/// <returns></returns>
bool enemy_sphere_to_hyperCube_Collision(IModel *enemySphere, IModel *hypeCube)
{
    float enemysphereMinX = enemySphere->GetX() - enemysphereHalfWidth;
    float enemysphereMaxX = enemySphere->GetX() + enemysphereHalfWidth;

    float enemysphereMinY = enemySphere->GetY() - enemysphereHalfHeight;
    float enemysphereMaxY = enemySphere->GetY() + enemysphereHalfHeight;

    float enemysphereMinZ = enemySphere->GetZ() - enemysphereHalfDepth;
    float enemysphereMaxZ = enemySphere->GetZ() + enemysphereHalfDepth;

    float hyperCubeMinX = hyperCube->GetX() - enemysphereHalfWidth;
    float hyperCubeMaxX = hyperCube->GetX() + enemysphereHalfWidth;

    float hyperCubeMinY = hyperCube->GetY() - enemysphereHalfHeight;
    float hyperCubeMaxY = hyperCube->GetY() + enemysphereHalfHeight;

    float hyperCubeMinZ = hyperCube->GetZ() - enemysphereHalfDepth;
    float hyperCubeMaxZ = hyperCube->GetZ() + enemysphereHalfDepth;

    return (hyperCubeMaxX >= enemysphereMinX && hyperCubeMinX <= enemysphereMaxX) && (hyperCubeMaxY >= enemysphereMinY && hyperCubeMinY <= enemysphereMaxY) && (hyperCubeMaxZ >= enemysphereMinZ && hyperCubeMinZ <= enemysphereMaxZ);
}

/// <summary>
/// The same logic was used for attracting the minicubes
/// </summary>
/// <param name="enemySphere"></param>
/// <param name="cubes"></param>
/// <param name="numCubes"></param>
/// <param name="speed"></param>
void enemyminiCubesAttract(IModel *enemySphere, IModel *cubes[], int numCubes, float &frameTime, float speed = 2.0f)
{
    for (int i = 0; i < numCubes; i++)
    { // looping through the minicubes
        if (cubes[i] != nullptr)
        { // ensuring the cube exist

            // distance vector between, between the enemy sphere and the cube
            float dx = enemySphere->GetX() - cubes[i]->GetX();
            float dz = enemySphere->GetZ() - cubes[i]->GetZ();
            float distance = sqrt(dx * dx + dz * dz);

            // check the cube if its in the attraacting range
            if (distance < attractingCubes)
            {
                cubes[i]->MoveX((dx / distance * speed) * frameTime); // moving the cube in the X direction
                cubes[i]->MoveZ((dz / distance * speed) * frameTime); // moving the cube in the Z direction
            }
        }
    }
}

int enemyplayerPoints = 0;
/// <summary>
/// When the enemysphere its a minicube 10 points are awarded
/// </summary>
/// <param name="enemySphere"></param>
/// <param name="minicubes"></param>
/// <param name="enemyplayerPoints"></param>
/// <param name="myEngine"></param>
/// <param name="myFont"></param>
void enemyplayer_points(IModel *enemySphere, IModel *minicubes, int &enemyplayerPoints, I3DEngine *myEngine, IFont *myFont)
{
    if (!enemySphere_collision_minicubes(enemySphere, minicubes))
    {

        stringstream enemyplayerScore;
        enemyplayerScore << "Enemy Point : 0" << enemyplayerPoints << endl;
        enemyplayerPoints += 10;
        myFont->Draw(enemyplayerScore.str(), 50, 20, kDarkGrey);
        enemyplayerScore.str("");
    }
}

/// <summary>
/// The Gamestates of the game
/// </summary>
enum GameStates
{
    Playing,
    GamePaused,
    Game_Won,
    GameLost
};

void main()
{
    // Create a 3D engine (using TLX engine here) and open a window for it
    I3DEngine *myEngine = New3DEngine(kTLX);
    myEngine->StartWindowed();

    srand(static_cast<unsigned int>(time(0))); // code for randomising the cubes in this

    float frameTime = myEngine->Timer(); // code for Frametime to make the game run more smooth on all the engines

    // Add default folder for meshes and other media
    myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");

    /// meshes and models which were used
    IMesh *waterMesh = myEngine->LoadMesh("Water.x"); // Water Mesh
    IModel *water = waterMesh->CreateModel(0, -5, 0);

    IMesh *islandMesh = myEngine->LoadMesh("Island.x"); // Island Mesh
    IModel *island = islandMesh->CreateModel(0, -5, 0);

    IMesh *sphereMesh = myEngine->LoadMesh("Sphere.x"); // Sphere Mesh
    IModel *sphere = sphereMesh->CreateModel(0, 6, 0);

    IMesh *skyMesh = myEngine->LoadMesh("Sky.x"); // Sky Mesh
    IModel *sky = skyMesh->CreateModel(0, -960, 0);

    IMesh *minicube = myEngine->LoadMesh("MiniCube.x"); // Minicube Mesh

    IFont *myFont = myEngine->LoadFont("Arial", 28); // Font used for the game points

    IModel *cubes[numCubes] = {nullptr};
    cubeSpawn(minicube, cubes, numCubes, sphere, hyperCube);

    IMesh *enemySphereMesh = nullptr;
    IModel *enemySphere = nullptr;

    ICamera *myCamera;
    myCamera = myEngine->CreateCamera(kManual);

    myCamera->SetPosition(0, 200, 0); // initial camera position
    myCamera->RotateX(90);

    bool gamePaused = false;
    int playerPoints = 0;
    int collectedCubes = 0;

    GameStates gameState = Playing;

    // The main game loop, repeat until engine is stopped

    // implementing a switch statement, so it easy to track all the gamestates
    while (myEngine->IsRunning())
    {

        switch (gameState)
        {
        case Playing:

            if (!isHyperMode)
            { // if the sphere is not in Hypermode, the skin is set to a regular sphere
                sphere->SetSkin("regularsphere.jpg");
            }

            Paused(myEngine, gamePaused, myFont);
            if (!gamePaused)
            {

                // Draw the scene
                myEngine->DrawScene();

                sphereControls(sphere, myEngine, frameTime);

                // setting the camera controls to default at first
                cameraControls(myCamera, myEngine);

                // enemysphere spawn
                enemy_Sphere(enemySphere, enemySphereMesh, myEngine);

                // applying enemysphere controls
                enemySphereMove(enemySphere, cubes, numCubes, frameTime, 1.8f);

                // here im checking the collision between the sphere and the minicubes
                // if collision happens the player is awarded with 10 points and the eaten cube is spawned randomly
                for (int i = 0; i < numCubes; i++)
                { // looping through the cubes
                    if (!sphere_to_minicubes_boxCollision(sphere, cubes[i]))
                    {

                        cubes[i]->SetPosition(rand() % 200 - 100, 2.0f, rand() % 200 - 100); // spawning the cube in
                        // random position

                        player_points(sphere, cubes[i], playerPoints, myEngine, myFont);

                        playerPoints += 10;
                        collectedCubes++;

                        if (collectedCubes == 4)
                        {
                            sphere->Scale(1.2f);
                            collectedCubes = 0;
                        }
                    }
                }

                // here im checking the collision between the enemysphere and the minicubes
                // if collision happens the player is awarded with 10 points and the eaten cube is spawned randomly

                for (int i = 0; i < numCubes; i++)
                {
                    if (!enemySphere_collision_minicubes(enemySphere, cubes[i]))
                    {

                        cubes[i]->SetPosition(rand() % 200 - 100, 2.0f, rand() % 200 - 100);

                        enemyplayer_points(enemySphere, cubes[i], playerPoints, myEngine, myFont);

                        enemyplayerPoints += 10;
                        collectedCubes++;

                        if (collectedCubes == 4)
                        {
                            enemySphere->Scale(1.2f);
                            collectedCubes = 0;
                        }
                    }
                }

                // collsiion between the sphere and the hypercube
                if (sphere_to_hypercube_collision(sphere, hyperCube))
                {

                    // if it collides with the hypercube the sphere gets bigger and the
                    if (!isHyperMode)
                    {

                        isHyperMode = true;
                        hyperModeTime = 0.0f;
                        sphere->SetSkin("hypersphere.jpg");
                        sphere->Scale(1.7);
                        hyperCube->SetPosition(-900, -900, -900);
                    }
                }

                if (isHyperMode)
                {
                    // implementing the attraction of the cubes when the sphere is collided with the hypercube

                    hyperModeTime += myEngine->Timer();
                    miniCubesAttract(sphere, cubes, numCubes, attractingCubesSpeed);
                    if (hyperModeDuration >= hyperModeTime)
                    {
                        isHyperMode = false;
                        sphere->SetSkin("regularsphere.jpg");
                    }
                }

                // enemysphere collision witb the minicubes
                if (Handle_enemySphere_Collision(enemySphere, cubes, numCubes, myEngine))
                {
                    for (int i = 0; i < numCubes; i++)
                    {
                        if (!enemySphere_collision_minicubes(enemySphere, cubes[i]))
                        {
                            cubes[i]->SetPosition(rand() % 200 - 100, 5, rand() % 200 - 100);
                        }
                    }
                }

                // collsiion between the sphere and the hypercube
                // if it collides with the hypercube the sphere gets bigger and the skin changes to a hypersphere
                if (enemy_sphere_to_hyperCube_Collision(enemySphere, hyperCube))
                {
                    if (!isHyperMode)
                    {

                        isHyperMode = true;
                        hyperModeTime = 0.0f;
                        enemySphere->SetSkin("hypersphere.jpg");
                        enemySphere->Scale(1.7);
                        hyperCube->SetPosition(-900, -900, -900);
                    }
                }

                if (isHyperMode)
                {
                    // implementing the attraction of the cubes when the sphere is collided with the hypercube

                    hyperModeTime += myEngine->Timer();
                    enemyminiCubesAttract(enemySphere, cubes, numCubes, frameTime, attractingCubesSpeed);
                    if (hyperModeDuration >= hyperModeTime)
                    {
                        isHyperMode = false;
                        enemySphere->SetSkin("enemysphere.jpg");
                    }
                }

                // drawing the points on the screen for the User's sphere and the enemysphere
                // code was found in the lab 3-4

                stringstream playerScore;
                playerScore << "User's Points: " << playerPoints << endl;
                myFont->Draw(playerScore.str(), 1050, 20, kDarkGrey);
                playerScore.str("");

                stringstream enemyplayerScore;
                enemyplayerScore << "Enemy Points: " << enemyplayerPoints << endl;
                myFont->Draw(enemyplayerScore.str(), 40, 20, kDarkGrey);
                playerScore.str("");

                GameWon(playerPoints, myFont, gameWon);

                // if the sphere reaches the limits of the water a gameLost message will appear
                sphere_to_water_collision(sphere, waterLimit, myEngine, gameLost);

                // if 120 points are reached the game enters the gameWon state
                if (playerPoints == 120)
                {
                    gameState = Game_Won;
                }

                // if the enemysphere reaches 120 points the game enters the GameLost state
                if (enemyplayerPoints == 120)
                {
                    gameState = GameLost;
                }
            }

            if (gameWon)
            {
                gameState = Game_Won;
            }
            else if (gameLost)
            {
                gameState = GameLost;
            }

            // if Key Escape is hit the game/program will automatically close
            if (myEngine->KeyHit(Key_Escape))
            {
                myEngine->Stop();
            }

            break;

        case GamePaused:
            // if the key P is pressed the game freezes and it is paused
            myEngine->DrawScene();
            if (myEngine->KeyHit(Key_P))
            {
                gameState = Playing;
            }
            break;

        case Game_Won:
            myEngine->DrawScene();
            GameWonMessage(myFont);

            break;

        case GameLost:
            myEngine->DrawScene();
            GameLostMessage(myFont);

            break;
        }
    }

    // Delete the 3D engine now we are finished with it
    myEngine->Delete();
}
