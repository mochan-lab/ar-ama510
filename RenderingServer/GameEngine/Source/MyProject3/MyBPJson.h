// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreFwd.h"
#include "Math/Rotator.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/CommandLine.h"

#include "MyBPJson.generated.h"


/**
 * 
 */
UCLASS()
class MYPROJECT3_API UMyBPJson : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "MyBPLibrary")
	static FString MyBytesToString(const TArray<uint8> Bytes);

	UFUNCTION(BlueprintCallable, Category = "MyBPLibrary")
		static TArray<uint8> MyStringToBytes(const FString MyString);

	UFUNCTION(BlueprintCallable, Category = "MyBPLibrary")
		static FVector MyArrayToVector(const TArray<double> MyArray);

	UFUNCTION(BlueprintCallable, Category = "MyBPLibrary")
		static FRotator MyArrayToRotator(const TArray<double> MyArray);

	UFUNCTION(BlueprintCallable, Category = "MyBPLibrary")
		static FString MyGetCommandLine(const FString& Key);

};


//UCLASS()
//class MYPROJECT3_API UMyBPJson : public UBlueprintFunctionLibrary
//{
//	GENERATED_BODY()
//
//		UFUNCTION(BlueprintCallable, Category = "MyBPLibrary")
//		static FVector MyArrayToVector(const TArray<double> MyArray);
//
//};