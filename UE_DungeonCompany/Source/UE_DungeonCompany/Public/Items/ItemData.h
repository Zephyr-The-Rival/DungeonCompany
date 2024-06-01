// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

/**
 * 
 */

class AWorldItem;

UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UItemData : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AWorldItem> MyWorldItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UObject> AnimationBlueprintClass;



	UFUNCTION(BlueprintNativeEvent)
	FString SerializeMyData();
	FString SerializeMyData_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void DeserializeMyData(const FString& StringData);
	void DeserializeMyData_Implementation(const FString& StringData);

protected:


	UFUNCTION(BlueprintCallable)
	TArray<FString> SeperateStringData(const FString& StringData);


};
