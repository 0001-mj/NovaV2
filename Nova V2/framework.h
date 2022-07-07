#pragma once
#include <Windows.h>
#include <format>
#include <iostream>
#include "Memory.hpp"

#include "minhook/MinHook.h"
#pragma comment(lib, "minhook/minhook.lib")

#include "SDK.hpp"

using namespace SDK;

UGameEngine* GEngine;
UWorld** GWorld;

static UWorld* GetWorld()
{
	return *GWorld;
}

template <typename T>
static T* NewObject(UObject* Outer = nullptr)
{
	return (T*)((UGameplayStatics*)UGameplayStatics::StaticClass())->STATIC_SpawnObject(T::StaticClass(), Outer);
}

#define UE_LOG(log, msg, ...) std::cout << #log << ": " << std::format(msg,  ##__VA_ARGS__) << std::endl
#define UE_CLOG(check, fatal, log, msg, ...) if (check) { UE_LOG(log, msg, __VA_ARGS__); if (fatal) { exit(0); } }
//#define NOLOGGING

// Our codes

#include "Util.hpp"
#include "Net.hpp"
#include "Inventory.hpp"
#include "Abilities.hpp"
#include "Hooks.hpp"