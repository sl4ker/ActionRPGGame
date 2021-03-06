// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPlayerController.h"
#include "ARUIComponent.h"
#include "AFAbilityComponent.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "ARAbilityBase.h"

#include "Weapons/ARWeaponManagerComponent.h"
#include "Abilities/ARAbilityManagerComponent.h"



AARPlayerController::AARPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UIComponent = ObjectInitializer.CreateDefaultSubobject<UARUIComponent>(this, "UIComponent");
	WeaponManager = ObjectInitializer.CreateDefaultSubobject<UARWeaponManagerComponent>(this, "WeaponManager");
	AbilityManager = ObjectInitializer.CreateDefaultSubobject<UARAbilityManagerComponent>(this, "AbilityManager");

	AbilityManager->ComponentTags.Add(TEXT("AbilityManager"));
	bInputBount = false;
}

void AARPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	ENetMode NetMode = GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetPawn());
		if (!ABInt)
			return;

		UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
		if (!AbilityComp)
			return;

		if (!bInputBount)
		{
			AbilityComp->BindAbilityToAction(InputComponent, InputNextWeapon);
			AbilityComp->BindAbilityToAction(InputComponent, InputPreviousWeapon);
			AbilityComp->BindAbilityToAction(InputComponent, InputHolsterWeapon);
			InputComponent->BindAction("InputAbilityManager", IE_Pressed, this, &AARPlayerController::InputShowHideAbilityManager);
			InputComponent->BindAction("InputInventory", IE_Pressed, this, &AARPlayerController::InputShowHideInventory);

			AbilityManager->BindInputs(InputComponent, AbilityComp);
			WeaponManager->BindInputs(InputComponent, AbilityComp);
			bInputBount = true;
		}
		//doesn't matter. Internally ability component make sure abilities are instanced on server and replicated back.
		FAFOnAbilityReady del1 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytNextWeapon, InputNextWeapon);
		AbilityComp->AddOnAbilityReadyDelegate(AbilitytNextWeapon, del1);
		TArray<FGameplayTag> NextWeap;
		NextWeap.Add(InputNextWeapon);
		AbilityComp->NativeAddAbilityFromTag(AbilitytNextWeapon, nullptr, NextWeap);

		FAFOnAbilityReady del2 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytPreviousWeapon, InputPreviousWeapon);
		AbilityComp->AddOnAbilityReadyDelegate(AbilitytPreviousWeapon, del2);
		TArray<FGameplayTag> PrevWeap;
		PrevWeap.Add(InputPreviousWeapon);
		AbilityComp->NativeAddAbilityFromTag(AbilitytPreviousWeapon, nullptr, PrevWeap);

		FAFOnAbilityReady del3 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytHolstersWeapon, InputHolsterWeapon);
		AbilityComp->AddOnAbilityReadyDelegate(AbilitytHolstersWeapon, del3);
		TArray<FGameplayTag> HolsterInput;
		HolsterInput.Add(InputHolsterWeapon);
		AbilityComp->NativeAddAbilityFromTag(AbilitytHolstersWeapon, nullptr, HolsterInput);

		
	}
	WeaponManager->POwner = InPawn;
	//UIAbilityManagerComponent->BindInputs();
}

void AARPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("SwitchAbilitySet", IE_Pressed, this, &AARPlayerController::InputSwitchAbilitySet);
	
}

void AARPlayerController::InputSwitchAbilitySet()
{
}
void AARPlayerController::InputShowHideAbilityManager()
{
	AbilityManager->ShowHideAbilityManager();
}
void AARPlayerController::InputShowHideInventory()
{
	UIComponent->ShowHideInventory();
}
void AARPlayerController::OnInputAbilityReady(FGameplayTag InAbilityTag, FGameplayTag InInputTag)
{
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	UARAbilityBase* Ability = Cast<UARAbilityBase>(AbilityComp->BP_GetAbilityByTag(InAbilityTag));
	TArray<FGameplayTag> Inputs;
	Inputs.Add(InInputTag);
	AbilityComp->SetAbilityToAction(InAbilityTag, Inputs, FAFOnAbilityReady());
}
