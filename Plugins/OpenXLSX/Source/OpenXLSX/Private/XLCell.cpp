
#include "XLCell.h"

#include "OpenXLSXLogging.h"
#include "OpenXLSXUtil.h"
#include "OpenXLSXTypes.h"

#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#endif

EOpenXLSXType XLCell::Type() const
{
#if PLATFORM_WINDOWS
	if (Cell.IsValid())
	{
		try
		{
			const auto& V = Cell->value();
			auto T = V.type();
			switch (T)
			{
			case OpenXLSX::XLValueType::String:
				return EOpenXLSXType::String;

			case OpenXLSX::XLValueType::Integer:
				return EOpenXLSXType::Integer;

			case OpenXLSX::XLValueType::Float:
				return EOpenXLSXType::Float;

			case OpenXLSX::XLValueType::Boolean:
				return EOpenXLSXType::Boolean;
			}
		}
		catch (std::exception& Exception)
		{
			UE_LOG(LogOpenXLSX, Error, TEXT("{What}"), *OpenXLSX::UTF8ToTCHARString(Exception.what()));
		}
	}
	
#endif
	return EOpenXLSXType::Null;
}

FString XLCell::StringValue() const
{
	FString Result;
#if PLATFORM_WINDOWS
	if (Cell.IsValid())
	{
		try
		{
			const auto& V = Cell->value();
			auto T = V.type();
			if (T == OpenXLSX::XLValueType::String)
			{
				Result = OpenXLSX::UTF8ToTCHARString(V.get<std::string>());
			}
		}
		catch (std::exception& Exception)
		{
			UE_LOG(LogOpenXLSX, Error, TEXT("{What}"), *OpenXLSX::UTF8ToTCHARString(Exception.what()));
		}
	}
#endif
	return Result;
}

int64 XLCell::IntValue() const
{
	int64 Result = 0;
#if PLATFORM_WINDOWS
	if (Cell.IsValid())
	{
		try
		{
			const auto& V = Cell->value();
			auto T = V.type();
			if (T == OpenXLSX::XLValueType::Integer)
			{
				Result = V.get<std::int64_t>();
			}
		}
		catch (std::exception& Exception)
		{
			UE_LOG(LogOpenXLSX, Error, TEXT("{What}"), *OpenXLSX::UTF8ToTCHARString(Exception.what()));
		}
	}
#endif
	return Result;
}

double XLCell::RealValue() const
{
	double Result = 0.0;
#if PLATFORM_WINDOWS
	if (Cell.IsValid())
	{
		try
		{
			const auto& V = Cell->value();
			auto T = V.type();
			if (T == OpenXLSX::XLValueType::Float)
			{
				Result = V.get<double>();
			}
		}
		catch (std::exception& Exception)
		{
			UE_LOG(LogOpenXLSX, Error, TEXT("{What}"), *OpenXLSX::UTF8ToTCHARString(Exception.what()));
		}
	}
#endif
	return Result;
}

bool XLCell::BoolValue() const
{
	bool Result = false;
#if PLATFORM_WINDOWS
	if (Cell.IsValid())
	{
		try
		{
			const auto& V = Cell->value();
			auto T = V.type();
			if (T == OpenXLSX::XLValueType::Boolean)
			{
				Result = V.get<bool>();
			}
		}
		catch (std::exception& Exception)
		{
			UE_LOG(LogOpenXLSX, Error, TEXT("{What}"), *OpenXLSX::UTF8ToTCHARString(Exception.what()));
		}
	}
#endif
	return Result;
}

#if PLATFORM_WINDOWS
void XLCell::SetCell(OpenXLSX::XLCell InCell)
{
	Cell = MakeShared<OpenXLSX::XLCell>(InCell);
}
#endif
