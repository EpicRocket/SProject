

#pragma once

#include "CoreMinimal.h"

#define MY_API SPROJECT_API

DECLARE_LOG_CATEGORY_EXTERN(LogProject, Log, All);

#define LOG(Format, ...) UE_LOG(LogProject, Log, TEXT(Format), ##__VA_ARGS__);
#define LOG_WARNING(Format, ...) UE_LOG(LogProject, Warning, TEXT(Format), ##__VA_ARGS__);
#define LOG_ERROR(Format, ...) UE_LOG(LogProject, Error, TEXT(Format), ##__VA_ARGS__);

#define CALLINFO_S (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define CALLINFO UE_LOG(LogProject, Log, TEXT("%s"), *CALLINFO_S)
#define CALLINFO_WARNING UE_LOG(LogProject, Warning, TEXT("%s"), *CALLINFO_S)
#define CALLINFO_ERROR UE_LOG(LogProject, Error, TEXT("%s"), *CALLINFO_S)

#define LOG_WITH_CALLINFO(Format, ...) UE_LOG(LogProject, Log, TEXT("%s %s"), *CALLINFO_S, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LOG_WARNING_WITH_CALLINFO(Format, ...) UE_LOG(LogProject, Warning, TEXT("%s %s"), *CALLINFO_S, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LOG_ERROR_WITH_CALLINFO(Format, ...) UE_LOG(LogProject, Error, TEXT("%s %s"), *CALLINFO_S, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

#define	LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__));

#define LOG_CHECK(Expr) { if(!(Expr)) { LOG_ERROR("ASSERTION : %s", TEXT("'"#Expr"'")) } }

#define VERIFY(Expr) verifyf(Expr != nullptr, TEXT("%s ASSERTION : %s"), *CALLINFO_S, TEXT("'"#Expr"'"));
#define VERIFYF(Expr, Format, ...) verifyf(Expr != nullptr, TEXT("%s ASSERTION : %s, %s"), *CALLINFO_S, TEXT("'"#Expr"'"), *FString::Printf(TEXT(Format), ##__VA_ARGS__));
#define VERIFY_EQUAL(Val1, Val2) verifyf(Val1 == Val2, TEXT("%s ASSERTION : %s != %s"), *CALLINFO_S, TEXT("'"#Val1"'"), TEXT("'"#Val2"'"));
#define VERIFYF_EQUAL(Val1, Val2, Format, ...) verifyf(Val1 == Val2, TEXT("%s ASSERTION : %s != %s, %s"), *CALLINFO_S, TEXT("'"#Val1"'"), TEXT("'"#Val2"'"), *FString::Printf(TEXT(Format), ##__VA_ARGS__));


namespace Global
{
template<typename T>
const T& GetEmpty()
{
	static T Empty;
	return Empty;
}
}
