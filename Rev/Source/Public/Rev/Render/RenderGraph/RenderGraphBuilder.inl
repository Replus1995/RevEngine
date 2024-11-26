#pragma once

namespace Rev
{

FORCEINLINE void* FRGBuilder::Alloc(uint64 SizeInBytes, uint32 AlignInBytes)
{
	return Allocator.Alloc(SizeInBytes, AlignInBytes);
}

template<typename PODType>
FORCEINLINE PODType* FRGBuilder::AllocPOD()
{
	return Allocator.AllocUninitialized<PODType>();
}

template<typename PODType>
FORCEINLINE PODType* FRGBuilder::AllocPODArray(uint32 Count)
{
	return Allocator.AllocUninitialized<PODType>(Count);
}

template<typename ObjectType, typename ...TArgs>
FORCEINLINE ObjectType* FRGBuilder::AllocObject(TArgs && ...Args)
{
	return Allocator.Alloc<ObjectType>(std::forward<TArgs&&>(Args)...);
}

template<typename ParameterStructType>
FORCEINLINE ParameterStructType* FRGBuilder::AllocParameters()
{
	return Allocator.Alloc<ParameterStructType>();
}

template<typename ParameterStructType>
FORCEINLINE ParameterStructType* FRGBuilder::AllocParameters(ParameterStructType* StructToCopy)
{
	ParameterStructType* Struct = Allocator.Alloc<ParameterStructType>();
	*Struct = *StructToCopy;
	return Struct;
}

template<typename ParameterStructType>
inline FRGPassRef FRGBuilder::AddPass(FRGName&& InName, const ParameterStructType* ParameterStruct, ERGPassFlags Flags, PrepareLambdaFunc<ParameterStructType>&& PrepareLambda, ExecuteLambdaFunc<ParameterStructType>&& ExecuteLambda)
{
	using LocalLambdaPassType = TRGLambdaPass<ParameterStructType>;
	return PassRegistry.Allocate<LocalLambdaPassType>(Allocator, std::forward<FRGName&&>(InName), ParameterStruct, Flags, PrepareLambda, ExecuteLambda);
}

}