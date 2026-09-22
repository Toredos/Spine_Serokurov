// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h" // для FOnAttributeChangeData
#include "Config_Enemy.h"
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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	class UAS_CombatAttributes* CombatAttributes;

	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	float CurrentAttackDamage = 0.0f;

	void HandleHealthChanged(const FOnAttributeChangeData& Data);

	bool bIsHitstopActive = false;
	float HitstopRealElapsed = 0.0f;
	float HitstopRealDuration = 0.1f;
	float HitstopLastRealTimeSeconds = 0.0f;

	// Защита от дабл-хита: набор целей, уже поражённых каждым слотом коллизии в текущем окне удара
	// Не UPROPERTY() — UHT не поддерживает TSet как тип значения внутри TMap;
	// TWeakObjectPtr безопасен без рефлексии, GC не нужен для защиты
	TMap<EWeaponCollisionSlot, TSet<TWeakObjectPtr<AActor>>> HitActorsThisWindowBySlot;

	bool bIsRotationBlending = false;
	FRotator RotationBlendStart;
	FRotator RotationBlendTarget;
	float RotationBlendElapsed = 0.0f;
	float RotationBlendDuration = 0.15f;

	FVector CachedMovementInputDirection = FVector::ZeroVector;

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

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TriggerHitstop(float RealDuration = 0.1f, float TimeDilationScale = 0.05f);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FVector GetCurrentMovementInputDirection() const { return CachedMovementInputDirection; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCurrentMovementInputDirection(FVector Direction) { CachedMovementInputDirection = Direction; }

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	class USoundBase* CurrentAttackHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "! Combat Feel")
	UConfig_Enemy* CombatFeelConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "! Combat Feel")
	FConfigEnemyOverride CombatFeelOverride;

	UFUNCTION(BlueprintCallable, Category = "Combat") float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetHitstopRealDuration() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetHitstopTimeDilation() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetReactionPlayRate() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetEnemyRotationBlendDuration() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") class USoundBase* GetHitReactSound() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") class USoundBase* GetDeathSound() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetHitReactSoundPitch() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetDeathSoundPitch() const;

};