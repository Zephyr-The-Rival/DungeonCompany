// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "UE_DungeonCompany/Public/DungeonCompanyGameInstance.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeDungeonCompanyGameInstance() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_UGameInstance();
	UE_DUNGEONCOMPANY_API UClass* Z_Construct_UClass_UDungeonCompanyGameInstance();
	UE_DUNGEONCOMPANY_API UClass* Z_Construct_UClass_UDungeonCompanyGameInstance_NoRegister();
	UPackage* Z_Construct_UPackage__Script_UE_DungeonCompany();
// End Cross Module References
	DEFINE_FUNCTION(UDungeonCompanyGameInstance::execJoinServer)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->JoinServer();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UDungeonCompanyGameInstance::execCreateServer)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->CreateServer();
		P_NATIVE_END;
	}
	void UDungeonCompanyGameInstance::StaticRegisterNativesUDungeonCompanyGameInstance()
	{
		UClass* Class = UDungeonCompanyGameInstance::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "CreateServer", &UDungeonCompanyGameInstance::execCreateServer },
			{ "JoinServer", &UDungeonCompanyGameInstance::execJoinServer },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UDungeonCompanyGameInstance_CreateServer_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UDungeonCompanyGameInstance_CreateServer_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/DungeonCompanyGameInstance.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDungeonCompanyGameInstance_CreateServer_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UDungeonCompanyGameInstance, nullptr, "CreateServer", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDungeonCompanyGameInstance_CreateServer_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDungeonCompanyGameInstance_CreateServer_Statics::Function_MetaDataParams) };
	UFunction* Z_Construct_UFunction_UDungeonCompanyGameInstance_CreateServer()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDungeonCompanyGameInstance_CreateServer_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UDungeonCompanyGameInstance_JoinServer_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UDungeonCompanyGameInstance_JoinServer_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/DungeonCompanyGameInstance.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDungeonCompanyGameInstance_JoinServer_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UDungeonCompanyGameInstance, nullptr, "JoinServer", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDungeonCompanyGameInstance_JoinServer_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDungeonCompanyGameInstance_JoinServer_Statics::Function_MetaDataParams) };
	UFunction* Z_Construct_UFunction_UDungeonCompanyGameInstance_JoinServer()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDungeonCompanyGameInstance_JoinServer_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UDungeonCompanyGameInstance);
	UClass* Z_Construct_UClass_UDungeonCompanyGameInstance_NoRegister()
	{
		return UDungeonCompanyGameInstance::StaticClass();
	}
	struct Z_Construct_UClass_UDungeonCompanyGameInstance_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameInstance,
		(UObject* (*)())Z_Construct_UPackage__Script_UE_DungeonCompany,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::DependentSingletons) < 16);
	const FClassFunctionLinkInfo Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UDungeonCompanyGameInstance_CreateServer, "CreateServer" }, // 3880729715
		{ &Z_Construct_UFunction_UDungeonCompanyGameInstance_JoinServer, "JoinServer" }, // 3463694153
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::FuncInfo) < 2048);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * \n */" },
#endif
		{ "IncludePath", "DungeonCompanyGameInstance.h" },
		{ "ModuleRelativePath", "Public/DungeonCompanyGameInstance.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDungeonCompanyGameInstance>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::ClassParams = {
		&UDungeonCompanyGameInstance::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		0,
		0,
		0x009000A8u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::Class_MetaDataParams), Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::Class_MetaDataParams)
	};
	UClass* Z_Construct_UClass_UDungeonCompanyGameInstance()
	{
		if (!Z_Registration_Info_UClass_UDungeonCompanyGameInstance.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UDungeonCompanyGameInstance.OuterSingleton, Z_Construct_UClass_UDungeonCompanyGameInstance_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UDungeonCompanyGameInstance.OuterSingleton;
	}
	template<> UE_DUNGEONCOMPANY_API UClass* StaticClass<UDungeonCompanyGameInstance>()
	{
		return UDungeonCompanyGameInstance::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UDungeonCompanyGameInstance);
	UDungeonCompanyGameInstance::~UDungeonCompanyGameInstance() {}
	struct Z_CompiledInDeferFile_FID_Users_oleor_Documents_GitHub_UniStuff_Projects_DungeonCompany_UE_DungeonCompany_Source_UE_DungeonCompany_Public_DungeonCompanyGameInstance_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_oleor_Documents_GitHub_UniStuff_Projects_DungeonCompany_UE_DungeonCompany_Source_UE_DungeonCompany_Public_DungeonCompanyGameInstance_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UDungeonCompanyGameInstance, UDungeonCompanyGameInstance::StaticClass, TEXT("UDungeonCompanyGameInstance"), &Z_Registration_Info_UClass_UDungeonCompanyGameInstance, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UDungeonCompanyGameInstance), 2726871432U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_oleor_Documents_GitHub_UniStuff_Projects_DungeonCompany_UE_DungeonCompany_Source_UE_DungeonCompany_Public_DungeonCompanyGameInstance_h_34139218(TEXT("/Script/UE_DungeonCompany"),
		Z_CompiledInDeferFile_FID_Users_oleor_Documents_GitHub_UniStuff_Projects_DungeonCompany_UE_DungeonCompany_Source_UE_DungeonCompany_Public_DungeonCompanyGameInstance_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_oleor_Documents_GitHub_UniStuff_Projects_DungeonCompany_UE_DungeonCompany_Source_UE_DungeonCompany_Public_DungeonCompanyGameInstance_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
