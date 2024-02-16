#pragma once
#include "Engine/DataTable.h"

USTRUCT(BlueprintType)
struct FWeaponRarityTable : FTableRowBase
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
	TObjectPtr<UTexture2D> BackgroundImage;
};