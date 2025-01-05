
#include "GMessage/Action/K2Node_AsyncAction_ListenForGMessage.h"
// include Engine
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "EdGraph/EdGraphPin.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_TemporaryVariable.h"
// include Internal Plugin
#include "GMessage/Action/AsyncAction_ListenForGMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_AsyncAction_ListenForGMessage)

namespace K2Node_AsyncAction_ListenForGMessage
{
    static FName ActualChannelPinName = "ActualChannel";
    static FName PayloadPinName = "Payload";
    static FName PayloadTypePinName = "PayloadType";
    static FName DelegateProxyPinName = "ProxyObject";
};


void UK2Node_AsyncAction_ListenForGMessage::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();

    UEdGraphPin* DelegateProxyPin = FindPin(K2Node_AsyncAction_ListenForGMessage::DelegateProxyPinName);
    if (ensure(DelegateProxyPin))
    {
        DelegateProxyPin->bHidden = true;
    }

    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, K2Node_AsyncAction_ListenForGMessage::PayloadPinName);
}

void UK2Node_AsyncAction_ListenForGMessage::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    struct GetMenuActions_Utils
    {
        static void SetNodeFunc(UEdGraphNode* NewNode, bool /*bIsTemplateNode*/, TWeakObjectPtr<UFunction> FunctionPtr)
        {
            UK2Node_AsyncAction_ListenForGMessage* AsyncTaskNode = CastChecked<UK2Node_AsyncAction_ListenForGMessage>(NewNode);
            if (FunctionPtr.IsValid())
            {
                UFunction* Func = FunctionPtr.Get();
                FObjectProperty* ReturnProp = CastFieldChecked<FObjectProperty>(Func->GetReturnProperty());

                AsyncTaskNode->ProxyFactoryFunctionName = Func->GetFName();
                AsyncTaskNode->ProxyFactoryClass = Func->GetOuterUClass();
                AsyncTaskNode->ProxyClass = ReturnProp->PropertyClass;
            }
        }
    };

    UClass* NodeClass = GetClass();
    ActionRegistrar.RegisterClassFactoryActions<UAsyncAction_ListenForGMessage>(
        FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateLambda([NodeClass](const UFunction* FactoryFunc) -> UBlueprintNodeSpawner* {
            UBlueprintNodeSpawner* NodeSpawner = UBlueprintFunctionNodeSpawner::Create(FactoryFunc);
            check(NodeSpawner);
            NodeSpawner->NodeClass = NodeClass;

            TWeakObjectPtr<UFunction> FunctionPtr = MakeWeakObjectPtr(const_cast<UFunction*>(FactoryFunc));
            NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(GetMenuActions_Utils::SetNodeFunc, FunctionPtr);

            return NodeSpawner;
        }));
}

void UK2Node_AsyncAction_ListenForGMessage::PostReconstructNode()
{
    Super::PostReconstructNode();

    RefreshOutputPayloadType();
}

void UK2Node_AsyncAction_ListenForGMessage::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
    if (ChangedPin == GetPayloadTypePin())
    {
        if (ChangedPin->LinkedTo.Num() == 0)
        {
            RefreshOutputPayloadType();
        }
    }
}

void UK2Node_AsyncAction_ListenForGMessage::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
    Super::GetPinHoverText(Pin, HoverTextOut);

    if (Pin.PinName == K2Node_AsyncAction_ListenForGMessage::PayloadPinName)
    {
        HoverTextOut = HoverTextOut + FString(TEXT("\n\n수신한 메시지 구조"));
    }
}

bool UK2Node_AsyncAction_ListenForGMessage::HandleDelegates(const TArray<FBaseAsyncTaskHelper::FOutputPinAndLocalVariable>& VariableOutputs, UEdGraphPin* ProxyObjectPin, UEdGraphPin*& InOutLastThenPin, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext)
{
    bool bIsErrorFree = true;

    if (VariableOutputs.Num() != 3)
    {
        ensureMsgf(false, TEXT("UK2Node_AsyncAction_ListenForGMessage::HandleDelegates - 가변 출력 배열이 유효하지 않습니다. 출력 델리게이트에는 단일 프록시 객체 출력만 있어야 하며 페이로드에 대한 핀이 있어야 합니다."));
        return false;
    }

    for (TFieldIterator<FMulticastDelegateProperty> PropertyIt(ProxyClass); PropertyIt && bIsErrorFree; ++PropertyIt)
    {
        UEdGraphPin* LastActivatedThenPin = nullptr;
        bIsErrorFree &= FBaseAsyncTaskHelper::HandleDelegateImplementation(
            *PropertyIt,
            VariableOutputs,
            ProxyObjectPin,
            InOutLastThenPin,
            LastActivatedThenPin,
            this,
            SourceGraph,
            CompilerContext
        );

        bIsErrorFree &= HandlePayloadImplementation(
            *PropertyIt,
            VariableOutputs[0],
            VariableOutputs[2],
            VariableOutputs[1],
            LastActivatedThenPin,
            SourceGraph,
            CompilerContext
        );
    }

    return bIsErrorFree;
}

bool UK2Node_AsyncAction_ListenForGMessage::HandlePayloadImplementation(FMulticastDelegateProperty* CurrentProperty, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& ProxyObjectVar, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& PayloadVar, const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& ActualChannelVar, UEdGraphPin*& InOutLastActivatedThenPin, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext)
{
    bool bIsErrorFree = true;
    const UEdGraphPin* PayloadPin = GetPayloadPin();
    const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

    check(CurrentProperty && SourceGraph && Schema);

    const FEdGraphPinType& PinType = PayloadPin->PinType;

    if (PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
    {
        if (PayloadPin->LinkedTo.Num() == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    UK2Node_TemporaryVariable* TempVarOutput = CompilerContext.SpawnInternalVariable(
        this, PinType.PinCategory, PinType.PinSubCategory, PinType.PinSubCategoryObject.Get(), PinType.ContainerType, PinType.PinValueType);

    UK2Node_CallFunction* const CallGetPayloadNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    CallGetPayloadNode->FunctionReference.SetExternalMember(TEXT("GetPayload"), CurrentProperty->GetOwnerClass());
    CallGetPayloadNode->AllocateDefaultPins();

    UEdGraphPin* GetPayloadCallSelfPin = Schema->FindSelfPin(*CallGetPayloadNode, EGPD_Input);
    if (GetPayloadCallSelfPin)
    {
        bIsErrorFree &= Schema->TryCreateConnection(GetPayloadCallSelfPin, ProxyObjectVar.TempVar->GetVariablePin());

        UEdGraphPin* GetPayloadExecPin = CallGetPayloadNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute);
        UEdGraphPin* GetPayloadThenPin = CallGetPayloadNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);

        UEdGraphPin* LastThenPin = nullptr;
        UEdGraphPin* GetPayloadPin = CallGetPayloadNode->FindPinChecked(TEXT("OutPayload"));
        bIsErrorFree &= Schema->TryCreateConnection(TempVarOutput->GetVariablePin(), GetPayloadPin);


        UK2Node_AssignmentStatement* AssignNode = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph);
        AssignNode->AllocateDefaultPins();
        bIsErrorFree &= Schema->TryCreateConnection(GetPayloadThenPin, AssignNode->GetExecPin());
        bIsErrorFree &= Schema->TryCreateConnection(PayloadVar.TempVar->GetVariablePin(), AssignNode->GetVariablePin());
        AssignNode->NotifyPinConnectionListChanged(AssignNode->GetVariablePin());
        bIsErrorFree &= Schema->TryCreateConnection(AssignNode->GetValuePin(), TempVarOutput->GetVariablePin());
        AssignNode->NotifyPinConnectionListChanged(AssignNode->GetValuePin());


        bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*InOutLastActivatedThenPin, *AssignNode->GetThenPin()).CanSafeConnect();
        bIsErrorFree &= Schema->TryCreateConnection(InOutLastActivatedThenPin, GetPayloadExecPin);

        UEdGraphPin* OutActualChannelPin = GetOutputChannelPin();
        bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*OutActualChannelPin, *ActualChannelVar.TempVar->GetVariablePin()).CanSafeConnect();
    }

    return bIsErrorFree;
}

void UK2Node_AsyncAction_ListenForGMessage::RefreshOutputPayloadType()
{
    UEdGraphPin* PayloadPin = GetPayloadPin();
    UEdGraphPin* PayloadTypePin = GetPayloadTypePin();

    if (PayloadTypePin->DefaultObject != PayloadPin->PinType.PinSubCategoryObject)
    {
        if (PayloadPin->SubPins.Num() > 0)
        {
            GetSchema()->RecombinePin(PayloadPin);
        }

        PayloadPin->PinType.PinSubCategoryObject = PayloadTypePin->DefaultObject;
        PayloadPin->PinType.PinCategory = (PayloadTypePin->DefaultObject == nullptr) ? UEdGraphSchema_K2::PC_Wildcard : UEdGraphSchema_K2::PC_Struct;
    }
}

UEdGraphPin* UK2Node_AsyncAction_ListenForGMessage::GetPayloadPin() const
{
    UEdGraphPin* Pin = FindPinChecked(K2Node_AsyncAction_ListenForGMessage::PayloadPinName);
    check(Pin->Direction == EGPD_Output);
    return Pin;
}

UEdGraphPin* UK2Node_AsyncAction_ListenForGMessage::GetPayloadTypePin() const
{
    UEdGraphPin* Pin = FindPinChecked(K2Node_AsyncAction_ListenForGMessage::PayloadTypePinName);
    check(Pin->Direction == EGPD_Input);
    return Pin;
}

UEdGraphPin* UK2Node_AsyncAction_ListenForGMessage::GetOutputChannelPin() const
{
    UEdGraphPin* Pin = FindPinChecked(K2Node_AsyncAction_ListenForGMessage::ActualChannelPinName);
    check(Pin->Direction == EGPD_Output);
    return Pin;
}
