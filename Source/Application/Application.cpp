#include <pch.h>
#include <windows.h>
#include <iostream>
#include <vector>

void LassoBaloon()
{

}

void Application::Initialize(HMODULE _Module)
{

    InputsManager::Register(); 

    ScriptRegister(_Module, []
        {
            bool IsModActive = false;
            Actor PlayerActor = 0;

            float MaxHeight = 50;
            float CurrentHeight = 0;
            int UpDownMotion = 1;

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
                if (IsModActive)
                {
                    Vector3 PlayerCoords = UNSORTED::GET_POSITION(PlayerActor);
                    Actor TargetActor = UNSORTED::GET_LASSO_TARGET(PlayerActor);
                    Vector3 TargetActorCoords = UNSORTED::GET_POSITION(TargetActor);
                    CurrentHeight = TargetActorCoords.y;

                    PrevTargetedActor = TargetActor;

                    UNSORTED::SET_ACTOR_INVULNERABILITY(PlayerActor, true);
                    UNSORTED::SET_ACTOR_INVULNERABILITY(TargetActor, true);

                    // Initial Ascend - TODO
                    while (UNSORTED::IS_ACTOR_ON_GROUND(PlayerActor) && UNSORTED::GET_LASSO_TARGET(PlayerActor))
                    {
                        if (!UNSORTED::IS_ACTOR_ON_GROUND(PlayerActor))
                        {
                            UNSORTED::TELEPORT_ACTOR(PlayerActor, &PlayerCoords, false, false, false);
                            break;
                        }
                        TargetActorCoords.y += 0.05;
                        PlayerCoords = UNSORTED::GET_POSITION(PlayerActor);
                        UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
                        PrevTargetedActorLastCoords = TargetActorCoords;
                        CurrentHeight = TargetActorCoords.y;
                        UNSORTED::WAIT(0);
                    }

                    // Ascend Targeted NPC
                    while(Input::IsKeyPressed(KEY_SPACE) && UNSORTED::GET_LASSO_TARGET(PlayerActor))
                    {
                        if (TargetActorCoords.y >= MaxHeight)
                        {
                            break;
                        }
                        TargetActorCoords.y += 0.05;
                        PlayerCoords = UNSORTED::GET_POSITION(PlayerActor);
                        UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
                        PrevTargetedActorLastCoords = TargetActorCoords;
                        CurrentHeight = TargetActorCoords.y;
                        UNSORTED::WAIT(0);
                    }

                    // Descend Targeted NPC
                    while(Input::IsKeyPressed(KEY_LEFT_ALT) && UNSORTED::GET_LASSO_TARGET(PlayerActor))
                    {
                        if (UNSORTED::IS_ACTOR_ON_GROUND(PlayerActor))
                        {
                            UNSORTED::TELEPORT_ACTOR(PlayerActor, &PlayerCoords, false, false, false);
                            break;
                        }

                        TargetActorCoords.y -= 0.05;
                        UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);

                        PrevTargetedActorLastCoords = TargetActorCoords;
                        CurrentHeight = TargetActorCoords.y;
                        UNSORTED::WAIT(0);
                    }

                    // Ideal Float
                    while (UNSORTED::GET_LASSO_TARGET(PlayerActor))
                    {
                        if ((Input::IsKeyPressed(KEY_LEFT_ALT) && !UNSORTED::IS_ACTOR_ON_GROUND(PlayerActor)) || Input::IsKeyPressed(KEY_SPACE))
                        {
                            break;
                        }
                        TargetActorCoords.y = CurrentHeight;
                        UNSORTED::TELEPORT_ACTOR(TargetActor, &TargetActorCoords, false, false, false);
                        UNSORTED::WAIT(0);
                    }

                    if (!UNSORTED::GET_LASSO_TARGET(PlayerActor))
                    {
                        while (!UNSORTED::IS_ACTOR_ON_GROUND(PlayerActor))
                        {
                            UNSORTED::SET_OBJECT_POSITION(PrevTargetedActor, Vector2{ PrevTargetedActorLastCoords.x, PrevTargetedActorLastCoords.y }, PrevTargetedActorLastCoords.z);
                            PrevTargetedActorLastCoords = UNSORTED::GET_POSITION(PrevTargetedActor);
                            UNSORTED::WAIT(0);
                        }
                        UNSORTED::SET_ACTOR_INVULNERABILITY(PlayerActor, false);
                        UNSORTED::SET_ACTOR_INVULNERABILITY(PrevTargetedActor, true);

                        CurrentHeight = 0;
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
