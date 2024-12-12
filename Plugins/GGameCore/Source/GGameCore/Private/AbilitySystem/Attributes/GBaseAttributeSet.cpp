#include "AbilitySystem/Attributes/GBaseAttributeSet.h"

UGBaseAttributeSet::UGBaseAttributeSet()
	: Health(100.0f), AttackRate(1.0f), Damage(5.0f)
{
	bOutOfHealth = false;
}
