// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h" // для FOnAttributeChangeData
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

	void HandleHealthChanged(const FOnAttributeChangeData& Data);

	// Защита от дабл-хита: набор целей, уже поражённых каждым слотом коллизии в текущем окне удара
	// Не UPROPERTY() — UHT не поддерживает TSet как тип значения внутри TMap;
	// TWeakObjectPtr безопасен без рефлексии, GC не нужен для защиты
	TMap<EWeaponCollisionSlot, TSet<TWeakObjectPtr<AActor>>> HitActorsThisWindowBySlot;

	bool bIsRotationBlending = false;
	FRotator RotationBlendStart;
	FRotator RotationBlendTarget;
	float RotationBlendElapsed = 0.0f;
	float RotationBlendDuration = 0.15f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Combat")
	void SetWeaponCollisionEnabled(EWeaponCollisionSlot Slot, bool bEnabled);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnHealthChanged(float OldValue, float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool HasAlreadyHitActorThisWindow(EWeaponCollisionSlot Slot, AActor* OtherActor) const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void MarkActorHitThisWindow(EWeaponCollisionSlot Slot, AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ClearHitActorsThisWindow(EWeaponCollisionSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartRotationBlend(FRotator TargetRotation, float Duration);

};