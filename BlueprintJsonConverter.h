// Copyright 2020 by Sunspear Games

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "BlueprintJsonConverter.generated.h"

/**
 * Allows serialization and deserialization of Custom structs to/from Json in blueprints, by using custom thunks.
 * CustomThunk tells UTH to not create a default Thunk for the specified UFUNCTION, because a custom one will be provided.
 * CustomStructureParam tells the blueprint VM that the specified params are Wildcards, so their pins take any type
 */
UCLASS()
class IMMORTAL_API UBlueprintJsonConverter : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * @brief
     * @param OutJsonString Resulting Json string after serialization
     * @param Struct The struct to serialize
     * @return Returns true if serialization was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Json", CustomThunk, meta = (CustomStructureParam = "Struct", DisplayName = "StructToJsonString"))
    static bool K2_StructToJsonString(FString& OutJsonString, const UStruct* Struct);

    /**
     * @brief Just a wrapper for FJsonObjectConverter::UStructToJsonObjectString
     * @param OutJsonString Resulting Json string after serialization
     * @param StructDefinition The type of the struct to serialize (typically a UClass reference)
     * @param Struct The pointer to the actual struct being serialized
     * @return Returns true if serialization was successful
     */
    static bool StructToJsonString(FString& OutJsonString, const UStruct* StructDefinition, const void* Struct);

    // rewrite exec
    DECLARE_FUNCTION(execK2_StructToJsonString)
    {
        // Grab the reference to the Json string inside the Thunk. This advanced one step on the Stack
        P_GET_PROPERTY_REF(FStrProperty, OutJsonString);

        Stack.StepCompiledIn<FStructProperty>(nullptr);
        void* InStruct = Stack.MostRecentPropertyAddress;

        // End the process of reading the params
        P_FINISH;
        bool bSuccess = false;

        FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
        if(StructProp && InStruct)
        {
            UScriptStruct* StructType = StructProp->Struct;

            // P_NATIVE_BEGIN switches control from the Blueprint VM to C++ source code, so we can use members not exposed to the blueprint VM
            P_NATIVE_BEGIN;
                bSuccess = StructToJsonString(OutJsonString, StructType, InStruct);
            P_NATIVE_END;
        }
        *static_cast<bool*>(RESULT_PARAM) = bSuccess;
    }


    /**
     * @brief Because StructToFill is a pointer instead of reference type, blueprints put the pin on the left
     * TODO: Investigate if StructToFill can be turned into a reference instead so it shows up as a return type
     * @param JsonString The json string to deserialize
     * @param StructToFill Resulting struct after deserialization
     * @return Returns true if deserialization was successful
     */
    UFUNCTION(BlueprintCallable, Category = "Json", CustomThunk, meta = (CustomStructureParam = "StructToFill", DisplayName = "JsonStringToStruct"))
    static bool K2_JsonStringToStruct(const FString& JsonString, const UStruct* StructToFill);

    /**
     * @brief Makes use of FJsonObjectConverter::JsonObjectToUStruct
     * @param JsonString The json string to deserialize
     * @param StructDefinition The type of the struct to deserialize (typically a UClass reference)
     * @param StructPtr The pointer to the actual struct that will be filled with the data from the Json string
     * @return Returns true if deserialization was successful
     */
    static bool JsonStringToStruct(const FString& JsonString, UStruct* StructDefinition, void* StructPtr);

    DECLARE_FUNCTION(execK2_JsonStringToStruct)
    {
        // Grab the reference to the Json string inside the Thunk. This advanced one step on the Stack
        P_GET_PROPERTY_REF(FStrProperty, JsonString);

        // Grab both Struct definition and type from the second param StructToFill. ORDER OF OPERATIONS IS KEY here.
        // This works only because of the previous call to P_GET_PROPERTY_REF which left the stack on top of the struct
        Stack.StepCompiledIn<FStructProperty>(NULL);
        void* StructPtr                 = Stack.MostRecentPropertyAddress;
        FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);

        // End the process of reading the params
        P_FINISH;

        UStruct* StructDefinition = StructProperty->Struct;
        bool bSuccess;

        // P_NATIVE_BEGIN switches control from the Blueprint VM to C++ source code, so we can use members not exposed to the blueprint VM
        P_NATIVE_BEGIN
            bSuccess = JsonStringToStruct(JsonString, StructDefinition, StructPtr);
        P_NATIVE_END

        *static_cast<bool*>(RESULT_PARAM) = bSuccess;
    }
};
