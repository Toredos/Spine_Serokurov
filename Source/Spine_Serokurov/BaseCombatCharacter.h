// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "BaseCombatCharacter.generated.h"

UENUM(BlueprintType)
enum class EWeaponCollisionSlot : uint8
{
	RightHand,
	LeftHand,
	RightFoot
};

UCLASS()
class SPINE_SEROKUROV_API ABaseCombatCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCombatCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	class UAS_CombatAttributes* CombatAttributes;

	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	float CurrentAttackDamage = 0.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void SetWeaponCollisionEnabled(EWeaponCollisionSlot Slot, bool bEnabled);

};