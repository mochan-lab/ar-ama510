// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBPJson.h"

FString UMyBPJson::MyBytesToString(const TArray<uint8> Bytes)
{
	FString retStr;
	FFileHelper::BufferToString(retStr, Bytes.GetData(), Bytes.Num());

	return	FString(retStr);
 
}

TArray<uint8> UMyBPJson::MyStringToBytes(const FString MyString)
{
	TArray<uint8> ResultBytes;
	ResultBytes.Append((uint8*)TCHAR_TO_UTF8(*MyString), MyString.Len());
	return ResultBytes;
}

FVector UMyBPJson::MyArrayToVector(const TArray<double> MyArray)
{
	FVector ff = FVector(MyArray[0], MyArray[1],MyArray[2]);
	return	ff;

}

FRotator UMyBPJson::MyArrayToRotator(const TArray<double> MyArray)
{
	FRotator ff = FRotator(MyArray[0], MyArray[1], MyArray[2]);
	return	ff;

}

FString UMyBPJson::MyGetCommandLine(const FString& Key)
{
	FString retVal;
	bool IsSuccess = FParse::Value(FCommandLine::Get(), *Key, retVal);
	if (!IsSuccess)
	{
		retVal = "";
	}
	return retVal;
}