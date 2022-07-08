#pragma once
#include "framework.h"

namespace Net
{
	static char (*InitHost)(AOnlineBeaconHost*);
	static void (*SetWorld)(UNetDriver*, UWorld*);
	static void (*ServerReplicateActors)(UReplicationDriver*);
	static void (*MarkArrayDirty)(FFastArraySerializer*);
	
	static void Init()
	{
		auto BaseAddr = (uintptr_t)GetModuleHandle(0);
		InitHost = decltype(InitHost)(BaseAddr + 0x39DAB0);
		SetWorld = decltype(SetWorld)(BaseAddr + 0x2298C60);
		ServerReplicateActors = decltype(ServerReplicateActors)(BaseAddr + 0x700820);
		MarkArrayDirty = decltype(MarkArrayDirty)(BaseAddr + 0x340210);

		auto Beacon = GetWorld()->SpawnActor<AOnlineBeaconHost>({}, {});
		Beacon->ListenPort = 7777;

		if (!InitHost(Beacon))
		{
			UE_LOG(LogNet, "AOnlineBeaconHost::InitHost failed");
			return;
		}

		GetWorld()->NetDriver = Beacon->NetDriver;
		GetWorld()->NetDriver->NetDriverName = FName(282);
		SetWorld(GetWorld()->NetDriver, GetWorld());
		GetWorld()->LevelCollections[1].NetDriver = GetWorld()->NetDriver;

		auto a1 = (__int64*)GetWorld()->NetDriver;
		auto v16 = (__int64)NewObject<UFortReplicationGraph>(GetWorld()->NetDriver);

		a1[163] = v16;
		if (v16)
		{
			(*(void(__fastcall**)(__int64, __int64*))(*(__int64*)v16 + 568))(v16, a1);
			(*(void(__fastcall**)(__int64, __int64))(*(__int64*)a1[163] + 576))(a1[163], a1[23]);
		}

		UE_LOG(LogNet, "Listening on port {}", Beacon->ListenPort);
	}
}