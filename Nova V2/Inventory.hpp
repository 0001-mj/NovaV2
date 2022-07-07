#pragma once
#include "framework.h"

namespace Inventory
{
	static void AddItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* ItemDef, EFortQuickBars QuickBar, int Slot, int Count, bool bAddToQuickbar = true)
	{
		auto ItemInstance = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(1, 0);
		ItemInstance->ItemEntry.Count = Count;
		ItemInstance->SetOwningControllerForTemporaryItem(PlayerController);

		PlayerController->WorldInventory->Inventory.ItemInstances.Add(ItemInstance);
		PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(ItemInstance->ItemEntry);

		if (bAddToQuickbar)
			PlayerController->QuickBars->ServerAddItemInternal(ItemInstance->GetItemGuid(), QuickBar, Slot);
	}

	static void UpdateInventory(AFortPlayerControllerAthena* PlayerController)
	{
		PlayerController->HandleWorldInventoryLocalUpdate();

		PlayerController->WorldInventory->HandleInventoryLocalUpdate();
		PlayerController->WorldInventory->bRequiresLocalUpdate = true;
		PlayerController->QuickBars->OnRep_PrimaryQuickBar();
		PlayerController->QuickBars->OnRep_SecondaryQuickBar();
		PlayerController->QuickBars->ForceNetUpdate();
		PlayerController->WorldInventory->ForceNetUpdate();

		Net::MarkArrayDirty(&PlayerController->WorldInventory->Inventory);
	}

	static void InitForPlayer(AFortPlayerControllerAthena* PlayerController)
	{
		AddItem(PlayerController, Util::FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"), EFortQuickBars::Primary, 0, 1);
		AddItem(PlayerController, Util::FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool"), EFortQuickBars::Primary, 0, 1, false);
		AddItem(PlayerController, Util::FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"), EFortQuickBars::Secondary, 0, 1);
		AddItem(PlayerController, Util::FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"), EFortQuickBars::Secondary, 1, 1);
		AddItem(PlayerController, Util::FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"), EFortQuickBars::Secondary, 2, 1);
		AddItem(PlayerController, Util::FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"), EFortQuickBars::Secondary, 3, 1);

		UpdateInventory(PlayerController);
	}
}