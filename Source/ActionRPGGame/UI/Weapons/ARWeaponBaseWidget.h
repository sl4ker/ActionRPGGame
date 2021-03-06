// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ARUMGWidgetBase.h"
#include "ARWeaponBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponBaseWidget : public UARUMGWidgetBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon Manager")
		class UARWeaponManagerComponent* WeaponManager;

public:
	void NativePreConstruct();
	void NativeConstruct();
	
	
	
};
