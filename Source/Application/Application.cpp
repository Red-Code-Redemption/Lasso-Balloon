#include <pch.h>
#include <windows.h>
#include <iostream>
#include <vector>

void Application::Initialize(HMODULE _Module)
{

    InputsManager::Register(); 

    ScriptRegister(_Module, []
        {
            bool IsModActive = false;
            Actor PlayerActor = 0;
            float Targetheight = 50;
            bool HeightReached = false;
            Actor PrevTargetedActor = 0;
            Vector3 PrevTargetedActorLastCoords = { -1, -1, -1 };

            while (true)
            {
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
                UNSORTED::HUD_CLEAR_OBJECTIVE_QUEUE();
                UNSORTED::PRINT_OBJECTIVE_B("WORKING", 1.0f, true, 1, 0, 0, 0, 0);
                if (IsModActive)
                {
                    Actor TargetActor = UNSORTED::GET_LASSO_TARGET(PlayerActor);
                    Vector3 PlayerCoords = UNSORTED::GET_POSITION(PlayerActor);
                    Vector3 TargetActorCoords = UNSORTED::GET_POSITION(TargetActor);


                    if (TargetActor)
                    {
                        PrevTargetedActor = TargetActor;

                        while (!HeightReached && UNSORTED::GET_LASSO_TARGET(PlayerActor))
                        {
                            // Come to Actor Z
                            if (TargetActorCoords.z < PlayerCoords.z)
                            {
                                TargetActorCoords.z += 0.03;
                            }
                            if (TargetActorCoords.z < PlayerCoords.z)
                            {
                                TargetActorCoords.z -= 0.03;
                            }

                            // Come to Actor X
                            if (TargetActorCoords.x > PlayerCoords.x)
                            {
                                TargetActorCoords.x -= 0.03;
                            }
                            if (TargetActorCoords.x < PlayerCoords.x)
                            {
                                TargetActorCoords.x += 0.03;
                            }

                            if (TargetActorCoords.y >= Targetheight)
                            {
                                HeightReached = true;
                                break;
                            }

                            // Ascend Targeted NPC
                            TargetActorCoords.y += 0.05;
                            UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
                            UNSORTED::WAIT(0);
                        }
                        if(HeightReached)
                        {
                            // Maintain the Height
                            TargetActorCoords.y = Targetheight;
                            UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
                        }
                        PrevTargetedActorLastCoords = TargetActorCoords;
                    }
                    else
                    {
                        while (!UNSORTED::IS_ACTOR_ON_GROUND(PlayerActor))
                        {
                            HEALTH::SET_ACTOR_HEALTH(PrevTargetedActor, 100);
                            UNSORTED::SET_OBJECT_POSITION(PrevTargetedActor, Vector2{ PrevTargetedActorLastCoords.x, PrevTargetedActorLastCoords.y }, PrevTargetedActorLastCoords.z);
                            PrevTargetedActorLastCoords = UNSORTED::GET_POSITION(PrevTargetedActor);
                            UNSORTED::WAIT(0);
                        }
                        HeightReached = false;
                        PrevTargetedActor = 0;
                        PrevTargetedActorLastCoords = { -1, -1, -1 };
                    }
                }
                UNSORTED::WAIT(0);
            }
        });
}


void Application::Shutdown(HMODULE _Module)
{
    ScriptUnregister(_Module);
		
    InputsManager::Unregister();
}
