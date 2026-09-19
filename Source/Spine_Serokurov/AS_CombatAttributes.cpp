#include "AS_CombatAttributes.h"

UAS_CombatAttributes::UAS_CombatAttributes()
{
    InitHealth(100.0f);
}

void UAS_CombatAttributes::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
    }
}