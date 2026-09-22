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
}

void ABaseCombatCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
    if (Data.OldValue > 0.0f && Data.NewValue <= 0.0f)
    {
        TriggerHitstop(HitstopDefaultRealDuration, HitstopDefaultTimeDilation);
        if (DeathSound)
        {
            UGameplayStatics::PlaySound2D(this, DeathSound, 1.0f, DeathSoundPitch);
        }
    }
    else if (Data.NewValue < Data.OldValue)
    {
        if (HitReactSound)
        {
            UGameplayStatics::PlaySound2D(this, HitReactSound, 1.0f, HitReactSoundPitch);
        }
    }

    OnHealthChanged(Data.OldValue, Data.NewValue);
}

bool ABaseCombatCharacter::HasAlreadyHitActorThisWindow(EWeaponCollisionSlot Slot, AActor* OtherActor) const
{
    const TSet<TWeakObjectPtr<AActor>>* HitSet = HitActorsThisWindowBySlot.Find(Slot);
    return HitSet ? HitSet->Contains(OtherActor) : false;
}

void ABaseCombatCharacter::MarkActorHitThisWindow(EWeaponCollisionSlot Slot, AActor* OtherActor)
{
    HitActorsThisWindowBySlot.FindOrAdd(Slot).Add(OtherActor);
}

void ABaseCombatCharacter::ClearHitActorsThisWindow(EWeaponCollisionSlot Slot)
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