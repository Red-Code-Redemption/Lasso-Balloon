#include <pch.h>

// Log File
std::ofstream logfile("debug.log");

// Global Variable
bool IsModActive = false;

Actor PlayerActor = 0;
Vector3 PlayerCoords = {-1, -1, -1};
Actor TargetActor = 0;
Vector3 TargetActorCoords = { -1, -1, -1 };
Actor PrevTargetedActor = 0;
Vector3 PrevTargetedActorLastCoords = { -1, -1, -1 };

bool InitialAscendReched = false;
float MaxHeight = 50;
bool MaxHeightReached = false;
float CurrentHeight = 0;

// Initial Ascend
void InitialAscend()
{
    bool OnPosition = true;
    if (UNSORTED::IS_ACTOR_ON_GROUND(PlayerActor))
    {
        OnPosition = false;
        TargetActorCoords.y += 0.05f;
    }
    if (static_cast<int>(std::abs(TargetActorCoords.z - PlayerCoords.z)) > 0)
    {
        OnPosition = false;
        if (TargetActorCoords.z < PlayerCoords.z)
        {
            TargetActorCoords.z += 0.01f;
        }
        if (TargetActorCoords.z > PlayerCoords.z)
        {
            TargetActorCoords.z -= 0.01f;
        }
    }
    if (static_cast<int>(std::abs(TargetActorCoords.x - PlayerCoords.x)) > 0)
    {
        OnPosition = false;
        if (TargetActorCoords.x < PlayerCoords.x)
        {
            TargetActorCoords.x += 0.01f;
        }
        if (TargetActorCoords.x > PlayerCoords.x)
        {
            TargetActorCoords.x -= 0.01f;
        }
    }

    if (OnPosition)
    {
        PlayerCoords = UNSORTED::GET_POSITION(PlayerActor);
        UNSORTED::TELEPORT_ACTOR(PlayerActor, &PlayerCoords, false, false, false);
        logfile << "OK" << "\n";
        InitialAscendReched = true;
        return;
    }

    UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
    PrevTargetedActorLastCoords = TargetActorCoords;
    CurrentHeight = TargetActorCoords.y;
}

// Ascend Targeted NPC
void Ascend()
{
    if (TargetActorCoords.y >= MaxHeight)
    {
        MaxHeightReached = true;
        return;
    }
    TargetActorCoords.y += 0.05f;
    UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
    PrevTargetedActorLastCoords = TargetActorCoords;
    CurrentHeight = TargetActorCoords.y;
}

// Descend Targeted NPC
void Descend()
{
    MaxHeightReached = false;
    if (UNSORTED::IS_ACTOR_ON_GROUND(TargetActor))
    {
        InitialAscendReched = false;
        UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
        PrevTargetedActorLastCoords = TargetActorCoords;
        CurrentHeight = TargetActorCoords.y;
        return;
    }
    // PlayerCoords.y -= 0.05f;
    // UNSORTED::SET_OBJECT_POSITION(PlayerActor, Vector2{ PlayerCoords.x, PlayerCoords.y }, PlayerCoords.z);
    // UNSORTED::TELEPORT_ACTOR(PlayerActor, &PlayerCoords, false, false, false);
    // TargetActorCoords = UNSORTED::GET_POSITION(TargetActor);
    TargetActorCoords.y -= 0.05f;
    UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
    PrevTargetedActorLastCoords = TargetActorCoords;
    CurrentHeight = TargetActorCoords.y;
}

// Ideal Float
void Ideal()
{
    TargetActorCoords.y = CurrentHeight;
    UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
}

void Application::Initialize(HMODULE _Module)
{

    InputsManager::Register(); 

    ScriptRegister(_Module, []
        {
            while (true)
            {
                // Activate/Deactivate Lasso Balloon Mod
                if (Input::IsKeyJustPressed(KEY_NUMPAD_1))
                {
                    IsModActive = !IsModActive;
                    UNSORTED::HUD_CLEAR_OBJECTIVE_QUEUE();

                    if(IsModActive)
                    {
                        PlayerActor = UNSORTED::GET_PLAYER_ACTOR(0);
                        UNSORTED::PRINT_OBJECTIVE_B("<green>Lasso Baloon Active\n", 1.0f, true, 1, 0, 0, 0, 0);
                    }
                    else
                    {
                        PlayerActor = -1;
                        UNSORTED::PRINT_OBJECTIVE_B("<red>Lasso Baloon Deactive\n", 1.0f, true, 1, 0, 0, 0, 0);
                    }
                }
                if (IsModActive)
                {
                    // Get Data
                    PlayerCoords = UNSORTED::GET_POSITION(PlayerActor);
                    TargetActor = UNSORTED::GET_LASSO_TARGET(PlayerActor);
                    TargetActorCoords = UNSORTED::GET_POSITION(TargetActor);
                    CurrentHeight = TargetActorCoords.y;
                    PrevTargetedActor = TargetActor;

                    // Set Invulnerability for npc (or else the npc dies mid air)
                    UNSORTED::SET_ACTOR_INVULNERABILITY(PlayerActor, true);
                    UNSORTED::SET_ACTOR_INVULNERABILITY(TargetActor, true);

                    // When there is a active target
                    while (UNSORTED::GET_LASSO_TARGET(PlayerActor))
                    {
                        // Initial Ascend
                        if (InitialAscendReched == false)
                        {
                            InitialAscend();
                        }
                        // Ascend Control
                        if (Input::IsKeyPressed(KEY_SPACE) && MaxHeightReached == false)
                        {
                            Ascend();
                        }
                        // Dscend Control
                        else if (Input::IsKeyPressed(KEY_LEFT_ALT))
                        {
                            Descend();
                        }
                        // When Ideal (no Control)
                        else
                        {
                            Ideal();
                        }

                        UNSORTED::WAIT(0);
                    }
                    // When No Target (return to starting state)
                    UNSORTED::SET_ACTOR_INVULNERABILITY(PrevTargetedActor, false);
                    float distance = Vector3::Distance(PlayerCoords, PrevTargetedActorLastCoords);
                    while (!UNSORTED::IS_ACTOR_ON_GROUND(PlayerActor))
                    {
                        PrevTargetedActorLastCoords.y = UNSORTED::GET_POSITION(PlayerActor).y + distance;
                        UNSORTED::SET_OBJECT_POSITION(PrevTargetedActor, Vector2{ PrevTargetedActorLastCoords.x, PrevTargetedActorLastCoords.y }, PrevTargetedActorLastCoords.z);
                        PrevTargetedActorLastCoords = UNSORTED::GET_POSITION(PrevTargetedActor);
                        UNSORTED::WAIT(0);
                    }
                    // UNSORTED::SET_ACTOR_INVULNERABILITY(PlayerActor, false);
                    CurrentHeight = 0;
                    PrevTargetedActor = 0;
                    PrevTargetedActorLastCoords = { -1, -1, -1 };
                    InitialAscendReched = false;
                }

                logfile.flush();
                UNSORTED::WAIT(0);
            }
        });
}


void Application::Shutdown(HMODULE _Module)
{
    ScriptUnregister(_Module);
		
    InputsManager::Unregister();
}
