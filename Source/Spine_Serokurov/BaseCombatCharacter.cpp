// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCombatCharacter.h"
#include "AbilitySystemComponent.h"
#include "AS_CombatAttributes.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseCombatCharacter::ABaseCombatCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    CombatAttributes = CreateDefaultSubobject<UAS_CombatAttributes>(TEXT("CombatAttributes"));
}

UAbilitySystemComponent* ABaseCombatCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void ABaseCombatCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAS_CombatAttributes::GetHealthAttribute())
            .AddUObject(this, &ABaseCombatCharacter::HandleHealthChanged);
    }

    for (const FWeaponCollisionSlotConfig& SlotConfig : WeaponCollisionSlots)
    {
        if (SlotConfig.Component)
        {
            SlotConfig.Component->OnComponentBeginOverlap.AddDynamic(this, &ABaseCombatCharacter::OnWeaponSlotOverlap);
        }
    }
}

void ABaseCombatCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
    if (Data.OldValue > 0.0f && Data.NewValue <= 0.0f)
    {
        TriggerHitstop(GetHitstopRealDuration(), GetHitstopTimeDilation());
        if (USoundBase* Death = GetDeathSound())
        {
            UGameplayStatics::PlaySound2D(this, Death, 1.0f, GetDeathSoundPitch());
        }
    }
    else if (Data.NewValue < Data.OldValue)
    {
        if (USoundBase* HitReact = GetHitReactSound())
        {
            UGameplayStatics::PlaySound2D(this, HitReact, 1.0f, GetHitReactSoundPitch());
        }
    }

    OnHealthChanged(Data.OldValue, Data.NewValue);
}

bool ABaseCombatCharacter::HasAlreadyHitActorThisWindow(FGameplayTag Slot, AActor* OtherActor) const
{
    const TSet<TWeakObjectPtr<AActor>>* HitSet = HitActorsThisWindowBySlot.Find(Slot);
    return HitSet ? HitSet->Contains(OtherActor) : false;
}

void ABaseCombatCharacter::MarkActorHitThisWindow(FGameplayTag Slot, AActor* OtherActor)
{
    HitActorsThisWindowBySlot.FindOrAdd(Slot).Add(OtherActor);
}

void ABaseCombatCharacter::ClearHitActorsThisWindow(FGameplayTag Slot)
{
    HitActorsThisWindowBySlot.FindOrAdd(Slot).Empty();
}

// Called every frame
void ABaseCombatCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsRotationBlending)
    {
        RotationBlendElapsed += DeltaTime;
        const float Alpha = FMath::Clamp(RotationBlendElapsed / RotationBlendDuration, 0.0f, 1.0f);

        const FQuat NewQuat = FQuat::Slerp(RotationBlendStart.Quaternion(), RotationBlendTarget.Quaternion(), Alpha);
        SetActorRotation(NewQuat.Rotator());

        if (Alpha >= 1.0f)
        {
            bIsRotationBlending = false;
        }
    }

    if (bIsHitstopActive)
    {
        const float NowReal = GetWorld()->GetRealTimeSeconds();
        HitstopRealElapsed += (NowReal - HitstopLastRealTimeSeconds);
        HitstopLastRealTimeSeconds = NowReal;

        if (HitstopRealElapsed >= HitstopRealDuration)
        {
            bIsHitstopActive = false;
            UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
        }
    }
}

void ABaseCombatCharacter::TriggerHitstop(float RealDuration, float TimeDilationScale)
{
    UGameplayStatics::SetGlobalTimeDilation(this, TimeDilationScale);
    HitstopRealDuration = RealDuration;
    HitstopRealElapsed = 0.0f;
    HitstopLastRealTimeSeconds = GetWorld()->GetRealTimeSeconds();
    bIsHitstopActive = true;
}

void ABaseCombatCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bIsHitstopActive && GetWorld())
    {
        UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
    }
    Super::EndPlay(EndPlayReason);
}

void ABaseCombatCharacter::StartRotationBlend(FRotator TargetRotation, float Duration)
{
    RotationBlendStart = GetActorRotation();
    RotationBlendTarget = TargetRotation;
    RotationBlendDuration = FMath::Max(Duration, KINDA_SMALL_NUMBER);
    RotationBlendElapsed = 0.0f;
    bIsRotationBlending = true;
}

// Called to bind functionality to input
void ABaseCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseCombatCharacter::GetMaxHealth() const
{
    if (CombatFeelOverride.bOverrideMaxHealth) return CombatFeelOverride.MaxHealth;
    return CombatFeelConfig ? CombatFeelConfig->MaxHealth : 100.0f;
}

float ABaseCombatCharacter::GetHitstopRealDuration() const
{
    if (CombatFeelOverride.bOverrideHitstopRealDuration) return CombatFeelOverride.HitstopRealDuration;
    return CombatFeelConfig ? CombatFeelConfig->HitstopRealDuration : 0.1f;
}

float ABaseCombatCharacter::GetHitstopTimeDilation() const
{
    if (CombatFeelOverride.bOverrideHitstopTimeDilation) return CombatFeelOverride.HitstopTimeDilation;
    return CombatFeelConfig ? CombatFeelConfig->HitstopTimeDilation : 0.05f;
}

USoundBase* ABaseCombatCharacter::GetHitReactSound() const
{
    if (CombatFeelOverride.bOverrideHitReactSound) return CombatFeelOverride.HitReactSound;
    return CombatFeelConfig ? CombatFeelConfig->HitReactSound : nullptr;
}

USoundBase* ABaseCombatCharacter::GetDeathSound() const
{
    if (CombatFeelOverride.bOverrideDeathSound) return CombatFeelOverride.DeathSound;
    return CombatFeelConfig ? CombatFeelConfig->DeathSound : nullptr;
}

float ABaseCombatCharacter::GetHitReactSoundPitch() const
{
    if (CombatFeelOverride.bOverrideHitReactSoundPitch) return CombatFeelOverride.HitReactSoundPitch;
    return CombatFeelConfig ? CombatFeelConfig->HitReactSoundPitch : 1.0f;
}

float ABaseCombatCharacter::GetDeathSoundPitch() const
{
    if (CombatFeelOverride.bOverrideDeathSoundPitch) return CombatFeelOverride.DeathSoundPitch;
    return CombatFeelConfig ? CombatFeelConfig->DeathSoundPitch : 1.0f;
}

float ABaseCombatCharacter::GetReactionPlayRate() const
{
    if (CombatFeelOverride.bOverrideReactionPlayRate) return CombatFeelOverride.ReactionPlayRate;
    return CombatFeelConfig ? CombatFeelConfig->ReactionPlayRate : 1.5f;
}

float ABaseCombatCharacter::GetEnemyRotationBlendDuration() const
{
    if (CombatFeelOverride.bOverrideEnemyRotationBlendDuration) return CombatFeelOverride.EnemyRotationBlendDuration;
    return CombatFeelConfig ? CombatFeelConfig->EnemyRotationBlendDuration : 0.3f;
}

void ABaseCombatCharacter::DisableAllWeaponCollisionSlots()
{
    for (const FWeaponCollisionSlotConfig& SlotConfig : WeaponCollisionSlots)
    {
        if (SlotConfig.Component)
        {
            if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(SlotConfig.Component))
            {
                Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
        }
    }
}

FGameplayTag ABaseCombatCharacter::GetSlotTagForComponent(UPrimitiveComponent* Component) const
{
    for (const FWeaponCollisionSlotConfig& SlotConfig : WeaponCollisionSlots)
    {
        if (SlotConfig.Component == Component)
        {
            return SlotConfig.SlotTag;
        }
    }
    return FGameplayTag();
}

void ABaseCombatCharacter::ClearHitActorsThisWindowForActiveSlot()
{
    ClearHitActorsThisWindow(CurrentActiveCollisionSlot);
}

void ABaseCombatCharacter::OnWeaponSlotOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    const FGameplayTag Slot = GetSlotTagForComponent(OverlappedComponent);
    HandleWeaponOverlap(OtherActor, Slot);
}