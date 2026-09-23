// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h" // for FOnAttributeChangeData
#include "GameplayTagContainer.h" // for FGameplayTag
#include "Config_Enemy.h"
#include "BaseCombatCharacter.generated.h"

/** Binds a weapon collision slot (tag) to a physical component of the character. */
USTRUCT(BlueprintType)
struct FWeaponCollisionSlotConfig
{
	GENERATED_BODY()

	/** Slot tag (Slot.*). Attack data uses it to select which limb deals damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SlotTag;

	/** Collision component that represents this slot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UPrimitiveComponent* Component = nullptr;
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

	/** Combat parameters config: max health, hitstop, hit reaction sounds and play rate, rotation towards the attacker.
	 *  If not assigned, C++ default values are used.
	 *  For per-instance tweaks on the level, use CombatFeelOverride. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UConfig_Enemy* CombatFeelConfig;

	/** Combo attack set (expects a ComboAttackDataAsset).
	 *  Number of hits = length of the ComboLinks array. Swapping the asset changes the combo without editing GA_ComboAttack. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UPrimaryDataAsset* ComboDataAsset;

	/** Per-instance overrides of the config.
	 *  A field value is applied only when its matching bOverride* checkbox is enabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FConfigEnemyOverride CombatFeelOverride;

	/** Runtime: damage of the current hit. Set by GA_ComboAttack from combo data; do not edit manually. */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Current Attack")
	float CurrentAttackDamage = 0.0f;

	void HandleHealthChanged(const FOnAttributeChangeData& Data);

	bool bIsHitstopActive = false;
	float HitstopRealElapsed = 0.0f;
	float HitstopRealDuration = 0.1f;
	float HitstopLastRealTimeSeconds = 0.0f;

	// Double-hit protection: set of targets already hit by each collision slot during the current hit window.
	// Not a UPROPERTY() because UHT does not support TSet as a TMap value type;
	// TWeakObjectPtr is safe without reflection, GC tracking is not needed here.
	TMap<FGameplayTag, TSet<TWeakObjectPtr<AActor>>> HitActorsThisWindowBySlot;

	bool bIsRotationBlending = false;
	FRotator RotationBlendStart;
	FRotator RotationBlendTarget;
	float RotationBlendElapsed = 0.0f;
	float RotationBlendDuration = 0.15f;

	FVector CachedMovementInputDirection = FVector::ZeroVector;

	UFUNCTION()
	void OnWeaponSlotOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Weapon collision slots: slot tag -> physical component.
	 *  Filled in the Construction Script, not via the Details panel: the panel cannot display a component reference
	 *  inside an array element struct. As a result, the array always looks empty in Class Defaults - this is expected.
	 *  Overlap events of all slots are bound automatically in BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FWeaponCollisionSlotConfig> WeaponCollisionSlots;

	/** Runtime: collision slot active for the current hit. Set by GA_ComboAttack from the ActiveSlot field of combo data. */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Current Attack")
	FGameplayTag CurrentActiveCollisionSlot;

	/** Opens/closes the collision window of the active slot. Implemented in the Blueprint subclass. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Combat")
	void SetWeaponCollisionEnabled(bool bEnabled);

	/** Called on every Health change (after the built-in C++ reaction: hitstop, sound). Implemented in the Blueprint subclass. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void OnHealthChanged(float OldValue, float NewValue);

	/** True if OtherActor has already been hit by this slot during the current hit window. Prevents repeated damage from a single swing. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool HasAlreadyHitActorThisWindow(FGameplayTag Slot, AActor* OtherActor) const;

	/** Marks OtherActor as already hit by this slot during the current hit window. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void MarkActorHitThisWindow(FGameplayTag Slot, AActor* OtherActor);

	/** Clears the list of hit targets for the given slot. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ClearHitActorsThisWindow(FGameplayTag Slot);

	/** Smoothly rotates the character to TargetRotation over Duration seconds (spherical interpolation, shortest path). */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartRotationBlend(FRotator TargetRotation, float Duration);

	/** Slows down global time for RealDuration real-time seconds. Adds a sense of impact to a hit. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TriggerHitstop(float RealDuration = 0.1f, float TimeDilationScale = 0.05f);

	/** Current movement input direction. Updated unconditionally, even while movement itself is blocked by an attack. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FVector GetCurrentMovementInputDirection() const { return CachedMovementInputDirection; }

	/** Updates the cached movement input direction (see GetCurrentMovementInputDirection). */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCurrentMovementInputDirection(FVector Direction) { CachedMovementInputDirection = Direction; }

	/** Unconditionally disables collision on all slots. Safeguard against a "stuck" window between hits. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DisableAllWeaponCollisionSlots();

	/** Runtime: hit sound of the current attack. Set by GA_ComboAttack, played on a confirmed hit. */
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Current Attack")
	class USoundBase* CurrentAttackHitSound;

	// Effective combat parameter values, resolved as: instance override -> config -> C++ default.
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetHitstopRealDuration() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetHitstopTimeDilation() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetReactionPlayRate() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetEnemyRotationBlendDuration() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") class USoundBase* GetHitReactSound() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") class USoundBase* GetDeathSound() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetHitReactSoundPitch() const;
	UFUNCTION(BlueprintCallable, Category = "Combat") float GetDeathSoundPitch() const;

	/** Returns the slot tag bound to the given component, or an empty tag if the component is not a weapon slot. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FGameplayTag GetSlotTagForComponent(UPrimitiveComponent* Component) const;

	/** Clears the list of hit targets for the active slot. Called from ANS_WeaponCollision when the hit window opens and closes. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ClearHitActorsThisWindowForActiveSlot();

	/** A weapon slot overlapped an actor. Called automatically from C++ (bound in BeginPlay);
	 *  the hit reaction (damage, sound, camera) is implemented in the Blueprint subclass. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void HandleWeaponOverlap(AActor* OtherActor, FGameplayTag Slot);
};