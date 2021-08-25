#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "GameStructs.generated.h"

UENUM(BlueprintType)
enum class ECannonType : uint8 {
    FireProjectile = 0 UMETA(DisplayName = "Use projectile"),
    FireSpecialProjectile = 1 UMETA(DisplayName = "Use specialProjectile"),
    FireTrace = 2 UMETA(DisplayName = "Use trace"),
};