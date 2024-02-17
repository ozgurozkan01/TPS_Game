#pragma once
#include "Engine/DataTable.h"
#include "WeaponRarityTable.generated.h"

USTRUCT(BlueprintType)
struct FWeaponRarityTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table Property")
	FLinearColor GlowColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table Property")
	FLinearColor BrightColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table Property")
	FLinearColor DarkColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table Property")
	int32 ActiveStar;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table Property")
	int32 CustomDepthStencil;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Table Property")
	UTexture2D* SlotBackgroundImage;
};