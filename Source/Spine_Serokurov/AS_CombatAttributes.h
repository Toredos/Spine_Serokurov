#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AS_CombatAttributes.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
       GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
       GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
       GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
       GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class SPINE_SEROKUROV_API UAS_CombatAttributes : public UAttributeSet
{
    GENERATED_BODY()

public:
    UAS_CombatAttributes();

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UAS_CombatAttributes, Health)

        virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};