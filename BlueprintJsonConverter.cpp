// Copyright 2020 by Sunspear Games

#include "BlueprintJsonConverter.h"

#include "JsonObjectConverter.h"


bool UBlueprintJsonConverter::K2_StructToJsonString(FString& OutJsonString, const UStruct* Struct)
{
    // We should never hit this!  stubs to avoid NoExport on the class.
    check(0);
    return false;
}


bool UBlueprintJsonConverter::StructToJsonString(FString& OutJsonString, const UStruct* StructDefinition, const void* Struct)
{
    FJsonObjectConverter::UStructToJsonObjectString(StructDefinition, Struct, OutJsonString, 0, 0);

    return !OutJsonString.IsEmpty();
}


bool UBlueprintJsonConverter::K2_JsonStringToStruct(const FString& JsonString, const UStruct* StructToFill)
{
    // We should never hit this!  stubs to avoid NoExport on the class.
    check(0);
    return false;
}


bool UBlueprintJsonConverter::JsonStringToStruct(const FString& JsonString, UStruct* StructDefinition, void* StructPtr)
{
    // First deserialize the Json string into a Json Object
    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
    if(!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogJson, Warning, TEXT("JsonObjectStringToUStruct - Unable to parse json=[%s]"), *JsonString);
        return false;
    }

    // If the above worked, now turn the Json Object into a UStruct of the same type as the second argument
    if(!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructDefinition, StructPtr, 0, 0))
    {
        UE_LOG(LogJson, Warning, TEXT("JsonObjectStringToUStruct - Unable to deserialize. json=[%s]"), *JsonString);
        return false;
    }

    return true;
}
