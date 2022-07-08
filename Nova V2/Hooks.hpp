#pragma once
#include "framework.h"

namespace Hooks
{
	static void* (*ProcessEvent)(void*, void*, void*);
	static void* (*SetClientLoginState)(UNetConnection*, int);
	static void* (*TickFlush)(UNetDriver* NetDriver);

	static void* ProcessEventHook(UObject* Object, UFunction* Function, void* Params)
	{
		if (!Object || !Function)
			return ProcessEvent(Object, Function, Params);
		
		auto FuncName = Function->GetName();

		if (GetKeyState(VK_F7) & 1)
		{
			bool bStarted = false;

			if (!bStarted)
			{
				bStarted = true;

				auto EventPlayer = UObject::FindObject<ULevelSequencePlayer>("LevelSequencePlayer Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.LevelSequence_LaunchRocket.AnimationPlayer");
				EventPlayer->Play();
			}
		}

		if (FuncName == "ServerExecuteInventoryItem")
		{
			auto ItemGuid = ((AFortPlayerController_ServerExecuteInventoryItem_Params*)Params)->ItemGuid;
			auto PlayerController = (AFortPlayerController*)Object;
			
			for (int i = 0; i < PlayerController->WorldInventory->Inventory.ItemInstances.Num(); i++)
			{
				auto ItemInstance = PlayerController->WorldInventory->Inventory.ItemInstances[i];

				if (Util::CompareGuid(ItemGuid, ItemInstance->GetItemGuid()))
				{
					if (auto Pawn = (AFortPlayerPawn*)PlayerController->Pawn)
					{
						auto Weap = Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)ItemInstance->GetItemDefinitionBP(), ItemGuid);
						Weap->AmmoCount = 999;
						Weap->ClientGivenTo(Pawn);
						Pawn->ClientInternalEquipWeapon(Weap);
					}
				}
			}
		}

		if (FuncName == "ServerTryActivateAbility")
		{
			auto AbilitySystemComponent = (UAbilitySystemComponent*)Object;
			auto CurrentParams = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Params;

			UGameplayAbility* InstancedAbility;
			Abilities::InternalTryActivateAbility(AbilitySystemComponent, CurrentParams->AbilityToActivate, CurrentParams->PredictionKey, &InstancedAbility, nullptr, nullptr);
		}

		if (FuncName == "ServerCreateBuildingActor")
		{
			auto CurrentParams = (AFortPlayerController_ServerCreateBuildingActor_Params*)Params;
			auto PlayerController = (AFortPlayerController*)Object;

			if (CurrentParams->BuildingClassData.BuildingClass)
			{
				auto BuildingActor = GetWorld()->SpawnActor<ABuildingActor>(CurrentParams->BuildLoc, CurrentParams->BuildRot, CurrentParams->BuildingClassData.BuildingClass);
				BuildingActor->InitializeKismetSpawnedBuildingActor(BuildingActor, PlayerController);
				BuildingActor->Team = ((AFortPlayerStateAthena*)PlayerController->PlayerState)->TeamIndex;
				BuildingActor->OnRep_Team();
			}
		}

		if (FuncName == "ServerTryActivateAbilityWithEventData")
		{
			auto AbilitySystemComponent = (UAbilitySystemComponent*)Object;
			auto CurrentParams = (UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params*)Params;

			UGameplayAbility* InstancedAbility;
			Abilities::InternalTryActivateAbility(AbilitySystemComponent, CurrentParams->AbilityToActivate, CurrentParams->PredictionKey, &InstancedAbility, nullptr, &CurrentParams->TriggerEventData);
		}
		
		if (FuncName == "ServerAttemptAircraftJump")
		{
			auto CurrentParams = (AFortPlayerControllerAthena_ServerAttemptAircraftJump_Params*)Params;
			auto PlayerController = (AFortPlayerControllerAthena*)Object;
			
			if (PlayerController->Pawn)
				return ProcessEvent(Object, Function, Params);

			auto GameState = (AAthena_GameState_C*)GetWorld()->GameState;
			auto Aircraft = GameState->GetAircraft(0);

			if (!Aircraft)
				return ProcessEvent(Object, Function, Params);

			auto Pawn = GetWorld()->SpawnActor<APlayerPawn_Athena_C>(Aircraft->K2_GetActorLocation(), {});
			PlayerController->Possess(Pawn);

			PlayerController->SetControlRotation(CurrentParams->ClientRotation);
		}

		if (FuncName == "ServerAbilityRPCBatch")
		{
			auto AbilitySystemComponent = (UAbilitySystemComponent*)Object;
			auto CurrentParams = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Params;

			UGameplayAbility* InstancedAbility;
			Abilities::InternalTryActivateAbility(AbilitySystemComponent, CurrentParams->BatchInfo.AbilitySpecHandle, CurrentParams->BatchInfo.PredictionKey, &InstancedAbility, nullptr, nullptr);
		}

		if (FuncName == "ReadyToStartMatch")
		{
			static bool bInit = false;

			if (!bInit)
			{
				bInit = true;

				Net::Init();

				NewObject<UCheatManager>(GEngine->GameInstance->LocalPlayers[0]->PlayerController)->DestroyAll(AAthena_PlayerController_C::StaticClass());

				auto GameState = (AAthena_GameState_C*)GetWorld()->GameState;
				
				GameState->CurrentPlaylistData = Util::FindObjectFast<UFortPlaylistAthena>("/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground");
				GameState->OnRep_CurrentPlaylistData();

				GameState->GamePhase = EAthenaGamePhase::None;
				GameState->OnRep_GamePhase(EAthenaGamePhase::None);

				auto GameMode = (AAthena_GameMode_C*)GetWorld()->AuthorityGameMode;

				GameMode->StartMatch();
				GameMode->StartPlay();
			}
		}

	    return ProcessEvent(Object, Function, Params);
	}

	static void* SetClientLoginStateHook(UNetConnection* Connection, int State) // gets called in NMT_Join
	{
		if (State == 3)
		{
			if (auto PlayerController = (AFortPlayerControllerAthena*)Connection->PlayerController)
			{
				auto PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;
				auto Pawn = (APlayerPawn_Athena_C*)GetWorld()->SpawnActor<APlayerPawn_Athena_C>({0, 0, 5000}, {});

				Pawn->bAlwaysRelevant = true;

				PlayerController->Possess(Pawn);

				PlayerController->bHasClientFinishedLoading = true;
				PlayerController->bHasServerFinishedLoading = true;
				PlayerController->OnRep_bHasServerFinishedLoading();

				PlayerState->bHasFinishedLoading = true;
				PlayerState->bHasStartedPlaying = true;
				PlayerState->OnRep_bHasStartedPlaying();

				PlayerController->QuickBars = GetWorld()->SpawnActor<AFortQuickBars>({}, {});
				PlayerController->QuickBars->SetOwner(PlayerController);
				PlayerController->QuickBars->OnRep_Owner();

				PlayerController->OverriddenBackpackSize = 5;

				PlayerState->CharacterParts[0] = Util::FindObjectFast<UCustomCharacterPart>("/Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1");
				PlayerState->CharacterParts[1] = Util::FindObjectFast<UCustomCharacterPart>("/Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01");
				PlayerState->OnRep_CharacterParts();

				Inventory::InitForPlayer(PlayerController);

				auto AbilitySet = Util::FindObjectFast<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");

				for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
					Util::GrantAbility(Pawn, AbilitySet->GameplayAbilities[i]);

				Util::GrantAbility(Pawn, Util::FindObjectFast<UClass>("/Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C"));

				Pawn->ForceNetUpdate();
				PlayerController->ForceNetUpdate();
			}
		}

		return SetClientLoginState(Connection, State);
	}

	static char KickPlayerHook(__int64, __int64)
	{
		return 0;
	}

	static void* TickFlushHook(UNetDriver* NetDriver)
	{
		if (NetDriver->ReplicationDriver)
			Net::ServerReplicateActors(NetDriver->ReplicationDriver);
		else
			UE_LOG(LogNet, "ReplicationDriver is null!");

		return TickFlush(NetDriver);
	}

	static void Init()
	{
		auto BaseAddr = (uintptr_t)GetModuleHandle(0);
		auto SetClientLoginStateAddr = BaseAddr + 0x2297FA0;
		auto KickPlayerAddr = BaseAddr + 0x216DEA0;
		auto ProcessEventAddr = BaseAddr + 0x158FB20;
		auto TickFlushAddr = BaseAddr + 0x2299FF0;
		auto CreateReplicationDriverAddr = BaseAddr + 0x2284200;

		CREATE_HOOK(ProcessEventAddr, ProcessEventHook, &ProcessEvent);
		CREATE_HOOK(SetClientLoginStateAddr, SetClientLoginStateHook, &SetClientLoginState);
		CREATE_HOOK(KickPlayerAddr, KickPlayerHook, nullptr);
		CREATE_HOOK(TickFlushAddr, TickFlushHook, &TickFlush);
	}
}