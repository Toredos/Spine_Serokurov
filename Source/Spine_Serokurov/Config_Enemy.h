#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Config_Enemy.generated.h"

USTRUCT(BlueprintType)
struct FConfigEnemyOverride
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Health")
	bool bOverrideMaxHealth = false;
	UPROPERTY(EditAnywhere, Category = "Health", meta = (EditCondition = "bOverrideMaxHealth"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Hitstop")
	bool bOverrideHitstopRealDuration = false;
	UPROPERTY(EditAnywhere, Category = "Hitstop", meta = (EditCondition = "bOverrideHitstopRealDuration"))
	float HitstopRealDuration = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Hitstop")
	bool bOverrideHitstopTimeDilation = false;
	UPROPERTY(EditAnywhere, Category = "Hitstop", meta = (EditCondition = "bOverrideHitstopTimeDilation"))
	float HitstopTimeDilation = 0.05f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	bool bOverrideHitReactSound = false;
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (EditCondition = "bOverrideHitReactSound"))
	class USoundBase* HitReactSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	bool bOverrideDeathSound = false;
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (EditCondition = "bOverrideDeathSound"))
	class USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	bool bOverrideHitReactSoundPitch = false;
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (EditCondition = "bOverrideHitReactSoundPitch"))
	float HitReactSoundPitch = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	bool bOverrideDeathSoundPitch = false;
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (EditCondition = "bOverrideDeathSoundPitch"))
	float DeathSoundPitch = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Reaction")
	bool bOverrideReactionPlayRate = false;
	UPROPERTY(EditAnywhere, Category = "Reaction", meta = (EditCondition = "bOverrideReactionPlayRate"))
	float ReactionPlayRate = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Reaction")
	bool bOverrideEnemyRotationBlendDuration = false;
	UPROPERTY(EditAnywhere, Category = "Reaction", meta = (EditCondition = "bOverrideEnemyRotationBlendDuration"))
	float EnemyRotationBlendDuration = 0.3f;
};

UCLASS(BlueprintType)
class SPINE_SEROKUROV_API UConfig_Enemy : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitstop")
	float HitstopRealDuration = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitstop")
	float HitstopTimeDilation = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundBase* HitReactSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	float HitReactSoundPitch = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	float DeathSoundPitch = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reaction")
	float ReactionPlayRate = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reaction")
	float EnemyRotationBlendDuration = 0.3f;
};