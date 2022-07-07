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

// BlueprintGeneratedClass GA_ContainmentUnit_Trigger.GA_ContainmentUnit_Trigger_C
// 0x0020 (0x0930 - 0x0910)
class UGA_ContainmentUnit_Trigger_C : public UFortGameplayAbility
{
public:
	struct FPointerToUberGraphFrame                    UberGraphFrame;                                           // 0x0910(0x0008) (Transient, DuplicateTransient)
	class UAbilitySystemComponent*                     OriginalDamageInstigator;                                 // 0x0918(0x0008) (Edit, BlueprintVisible, ZeroConstructor, DisableEditOnInstance, InstancedReference, IsPlainOldData)
	class UAbilitySystemComponent*                     OriginalDamageTarget;                                     // 0x0920(0x0008) (Edit, BlueprintVisible, ZeroConstructor, DisableEditOnInstance, InstancedReference, IsPlainOldData)
	struct FGameplayTag                                RequestReflectionTag;                                     // 0x0928(0x0008) (Edit, BlueprintVisible, DisableEditOnInstance)

	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindClass("BlueprintGeneratedClass GA_ContainmentUnit_Trigger.GA_ContainmentUnit_Trigger_C");
		return ptr;
	}


	void K2_ActivateAbilityFromEvent(struct FGameplayEventData* EventData);
	void ExecuteUbergraph_GA_ContainmentUnit_Trigger(int EntryPoint);
};


}

#ifdef _MSC_VER
	#pragma pack(pop)
#endif
