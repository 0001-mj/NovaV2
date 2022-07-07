#pragma once
#include "framework.h"

namespace Net
{
	static char (*InitHost)(AOnlineBeaconHost*);
	static void (*SetWorld)(UNetDriver*, UWorld*);
	static void (*ReplicationDriver_SetWorld)(UReplicationDriver*, UWorld*);
	static void (*ReplicationDriver_SetNetDriver)(UReplicationDriver*, UNetDriver*);
	static void (*ServerReplicateActors)(UReplicationDriver*);
	static void (*MarkArrayDirty)(FFastArraySerializer*);
	
	static void SetReplicationDriver(UNetDriver* NetDriver, UReplicationDriver* ReplicationDriver)
	{
		//pseudocode my love, pasted pseudocode because SetReplicationDriver is inline
		auto v16 = (__int64)ReplicationDriver;
		auto a1 = (__int64*)NetDriver;
		a1[163] = v16;
		if (v16)
		{
			(*(void(__fastcall**)(__int64, __int64*))(*(__int64*)v16 + 568))(v16, a1); //UReplicationDriver::SetNetDriver
			(*(void(__fastcall**)(__int64, __int64))(*(__int64*)v16 + 576))(v16, a1[23]); //UReplicationDriver::SetWorld
		}
	}

	static void Init()
	{
		auto BaseAddr = (uintptr_t)GetModuleHandle(0);
		InitHost = decltype(InitHost)(BaseAddr + 0x39DAB0);
		SetWorld = decltype(SetWorld)(BaseAddr + 0x2298C60);
		ServerReplicateActors = decltype(ServerReplicateActors)(BaseAddr + 0x700820);
		MarkArrayDirty = decltype(MarkArrayDirty)(BaseAddr + 0x340210);

		auto Beacon = GetWorld()->SpawnActor<AOnlineBeaconHost>({}, {});
		Beacon->ListenPort = 7777;
		Beacon->NetDriverName.ComparisonIndex = 282;

		if (!InitHost(Beacon))
		{
			UE_LOG(LogNet, "AOnlineBeaconHost::InitHost failed");
			return;
		}

		GetWorld()->NetDriver = Beacon->NetDriver;
		SetWorld(GetWorld()->NetDriver, GetWorld());
		GetWorld()->LevelCollections[1].NetDriver = GetWorld()->NetDriver;

		SetReplicationDriver(GetWorld()->NetDriver, NewObject<UFortReplicationGraph>());

		UE_LOG(LogNet, "Listening on port {}", Beacon->ListenPort);
	}
}