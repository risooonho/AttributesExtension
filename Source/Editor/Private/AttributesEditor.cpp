// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "AttributesEditor.h"
#include <AssetToolsModule.h>

#include "Customizations/FloatAttrCustomization.h"
#include "Customizations/Int32AttrCustomization.h"
#include "Customizations/AttrCategoryCustomization.h"
#include "Customizations/AttrCategoryPinFactory.h"


#define LOCTEXT_NAMESPACE "AttributesEditor"

DEFINE_LOG_CATEGORY(LogAttributesEd)

void FAttributesEditorModule::StartupModule()
{
	UE_LOG(LogAttributesEd, Log, TEXT("AttributesEditor: Log Started"));

	RegisterPropertyTypeCustomizations();

	// Register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
}

void FAttributesEditorModule::ShutdownModule()
{
	UE_LOG(LogAttributesEd, Log, TEXT("AttributesEditor: Log Ended"));

	// Unregister all the asset types
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();

	// Unregister all pin customizations
	for (auto& FactoryPtr : CreatedPinFactories)
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(FactoryPtr);
	}
	CreatedPinFactories.Empty();
}


void FAttributesEditorModule::RegisterPropertyTypeCustomizations()
{
	RegisterCustomPropertyTypeLayout("FloatAttr", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFloatAttrCustomization::MakeInstance));
	RegisterCustomPropertyTypeLayout("Int32Attr", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FInt32AttrCustomization::MakeInstance));
	RegisterCustomPropertyTypeLayout("AttrCategory", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FAttrCategoryCustomization::MakeInstance));

	RegisterCustomPinFactory<FAttrCategoryPinFactory>();
}


void FAttributesEditorModule::RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate)
{
	check(PropertyTypeName != NAME_None);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, PropertyTypeLayoutDelegate);
}

template<class T>
void FAttributesEditorModule::RegisterCustomPinFactory()
{
	TSharedPtr<T> PinFactory = MakeShareable(new T());
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
	CreatedPinFactories.Add(PinFactory);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FAttributesEditorModule, AttributesEditor);