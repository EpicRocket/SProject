

#include "StageMonsterUnit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageMonsterUnit)

/*
 * 공격
 * 죽는거
 * 
 * Character {
 * void Attack(Target[]); <<- 이건 뭔가 좀 느낌이 이상하다? <- 이건 그게 아니니까
 * 
 * void AddOrderToTarget(Order, Target) // 명령
 * void AddOrderToSelf(Order)
 * 
 * void Death(); (얘는 가능할 거 같은데) (체력이 0이 되면 삭제 된다.) Character라면 모두 가지고 있는 행위 * }
 * }
 * 
 * 
 * [ 플레이어 -> 캐릭터에게 명령을 내리면 -> 캐릭터가 명령을 수신받고 행동하는 ] 가능하다면...
 * 
 * 명령 -> GameplayTag로 Broadcast해서 (대체)
 * 플레이어가 -> 선택된 캐릭터에게 특정 GameplayEffect(Ability여도 되고)를 부여하면 GameplayEffect에서 캐릭터에게 자신이 가지고 있는 태그(명령)을 부여하고
 * Effect에서 캐릭터가 Ability(공격, 이동 등등)을 가지고 있으면 특정 GameplayTag 캐릭터에게 부여되면 Ability에서 수신받아서 이벤트 발생 가능!
 * 
 * 공격, 죽는거랑 무슨 상관이냐... -> 명령을 받을 수 있으니까 독립적으로 구현이 가능하다.
 * 
 * 플레이어가 -> A 캐릭터에게 공격명령을 내리면 -> A 캐릭터가 공격 능력(GameplayAbility)가 없으면 이벤트 수신이 안되니까 발생 하지 않음.
 * 플레이어가 -> B 캐릭터에게 공격명령을 내리면 -> B 캐릭터가 공격 능력(GameplayAbility)가 있음으로 이벤트 수신되어 공격 이벤트를 발생.
 * 
 * 
 * 
 */