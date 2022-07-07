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

		if (FuncName.contains("ServerExecuteInventoryItem"))
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
					}
				}
			}
		}

		if (FuncName.contains("ServerTryActivateAbility"))
		{
			auto AbilitySystemComponent = (UAbilitySystemComponent*)Object;
			auto CurrentParams = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Params;

			auto EventData = FuncName.contains("WithEventData") ? &(((UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params*)Params))->TriggerEventData : nullptr;

			UGameplayAbility* InstancedAbility;
			Abilities::InternalTryActivateAbility(AbilitySystemComponent, CurrentParams->AbilityToActivate, CurrentParams->PredictionKey, &InstancedAbility, nullptr, EventData);
		}

		if (FuncName.contains("ServerAbilityRPCBatch"))
		{
			auto AbilitySystemComponent = (UAbilitySystemComponent*)Object;
			auto CurrentParams = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Params;

			UGameplayAbility* InstancedAbility;
			Abilities::InternalTryActivateAbility(AbilitySystemComponent, CurrentParams->BatchInfo.AbilitySpecHandle, CurrentParams->BatchInfo.PredictionKey, &InstancedAbility, nullptr, nullptr);
		}

		if (FuncName.contains("ReadyToStartMatch"))
		{
			static bool bInit = false;

			if (!bInit)
			{
				bInit = true;

				Net::Init();

				auto GameState = (AAthena_GameState_C*)GetWorld()->GameState;
				
				GameState->GamePhase = EAthenaGamePhase::Warmup;
				GameState->OnRep_GamePhase(EAthenaGamePhase::Setup);

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

				Inventory::InitForPlayer(PlayerController);

				auto AbilitySet = Util::FindObjectFast<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");

				for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
					Util::GrantAbility(Pawn, AbilitySet->GameplayAbilities[i]);

				Util::GrantAbility(Pawn, Util::FindObjectFast<UClass>("/Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C"));
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

		return TickFlush(NetDriver);
	}

	static void Init()
	{
		auto BaseAddr = (uintptr_t)GetModuleHandle(0);
		auto SetClientLoginStateAddr = BaseAddr + 0x2297FA0;
		auto KickPlayerAddr = BaseAddr + 0x216DEA0;
		auto ProcessEventAddr = BaseAddr + 0x158FB20;
		auto TickFlushAddr = BaseAddr + 0x2299FF0;

		MH_CreateHook((void*)ProcessEventAddr, ProcessEventHook, (void**)&ProcessEvent);
		MH_EnableHook((void*)ProcessEventAddr);
		MH_CreateHook((void*)SetClientLoginStateAddr, SetClientLoginStateHook, (void**)&SetClientLoginState);
		MH_EnableHook((void*)SetClientLoginStateAddr);
		MH_CreateHook((void*)KickPlayerAddr, KickPlayerHook, nullptr);
		MH_EnableHook((void*)KickPlayerAddr);
		MH_CreateHook((void*)TickFlushAddr, TickFlushHook, (void**)&TickFlush);
		MH_EnableHook((void*)TickFlushAddr);
	}
}