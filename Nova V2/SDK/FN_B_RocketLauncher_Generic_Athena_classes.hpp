#pragma once

// Fortnite (4.5-CL-4159770) SDK

#ifdef _MSC_VER
	#pragma pack(push, 0x8)
#endif

namespace SDK
{
//---------------------------------------------------------------------------
//Classes
//---------------------------------------------------------------------------

// BlueprintGeneratedClass B_RocketLauncher_Generic_Athena.B_RocketLauncher_Generic_Athena_C
// 0x0014 (0x0F20 - 0x0F0C)
class AB_RocketLauncher_Generic_Athena_C : public AB_Ranged_Generic_C
{
public:
	unsigned char                                      UnknownData00[0x4];                                       // 0x0F0C(0x0004) MISSED OFFSET
	struct FPointerToUberGraphFrame                    UberGraphFrame;                                           // 0x0F10(0x0008) (Transient, DuplicateTransient)
	class UParticleSystemComponent*                    RearMuzzle;                                               // 0x0F18(0x0008) (BlueprintVisible, ZeroConstructor, InstancedReference, IsPlainOldData)

	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindClass("BlueprintGeneratedClass B_RocketLauncher_Generic_Athena.B_RocketLauncher_Generic_Athena_C");
		return ptr;
	}


	void UserConstructionScript();
	void Muzzle_Flash_FX(bool* Persistent_Fire);
	void ExecuteUbergraph_B_RocketLauncher_Generic_Athena(int EntryPoint);
};


}

#ifdef _MSC_VER
	#pragma pack(pop)
#endif
