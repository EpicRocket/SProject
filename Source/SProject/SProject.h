// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Project, Log, All);

#define LOG(Format, ...) UE_LOG(Project, Log, TEXT(Format), ##__VA_ARGS__);
#define LOG_WARNING(Format, ...) UE_LOG(Project, Warning, TEXT(Format), ##__VA_ARGS__);
#define LOG_ERROR(Format, ...) UE_LOG(Project, Error, TEXT(Format), ##__VA_ARGS__);

#define CALLINFO_S (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define CALLINFO UE_LOG(Project, Log, TEXT("%s"), *CALLINFO_S)
#define CALLINFO_WARNING UE_LOG(Project, Warning, TEXT("%s"), *CALLINFO_S)
#define CALLINFO_ERROR UE_LOG(Project, Error, TEXT("%s"), *CALLINFO_S)

#define LOG_WITH_CALLINFO(Format, ...) UE_LOG(Project, Log, TEXT("%s %s"), *CALLINFO_S, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LOG_WARNING_WITH_CALLINFO(Format, ...) UE_LOG(Project, Warning, TEXT("%s %s"), *CALLINFO_S, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LOG_ERROR_WITH_CALLINFO(Format, ...) UE_LOG(Project, Error, TEXT("%s %s"), *CALLINFO_S, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

#define	LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__));

#define LOG_CHECK(Expr, ...) { if(!(Expr)) { LOG_ERROR(TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }

#define VERIFY(Expr) verifyf(Expr != nullptr, TEXT("%s ASSERTION : %s"), *CALLINFO_S, TEXT("'"#Expr"'"));
#define VERIFYF(Expr, Format, ...) verifyf(Expr != nullptr, TEXT("%s ASSERTION : %s, %s"), *CALLINFO_S, TEXT("'"#Expr"'"), *FString::Printf(TEXT(Format), ##__VA_ARGS__));
#define VERIFY_EQUAL(Val1, Val2) verifyf(Val1 == Val2, TEXT("%s ASSERTION : %s != %s"), *CALLINFO_S, TEXT("'"#Val1"'"), TEXT("'"#Val2"'"));
#define VERIFYF_EQUAL(Val1, Val2, Format, ...) verifyf(Val1 == Val2, TEXT("%s ASSERTION : %s != %s, %s"), *CALLINFO_S, TEXT("'"#Val1"'"), TEXT("'"#Val2"'"), *FString::Printf(TEXT(Format), ##__VA_ARGS__));