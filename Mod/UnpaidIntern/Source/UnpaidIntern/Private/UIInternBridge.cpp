// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIInternBridge.h"
#include "UIInternSubsystem.h"
#include "UIObservationBuilder.h"
#include "UIInternSettings.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void UUIInternBridge::Start(UUIInternSubsystem* Owner, int32 Port)
{
	Subsystem = Owner;
	ListenPort = Port;
	// TODO(M1): Bind TCP listener to 127.0.0.1:Port only — never 0.0.0.0.
	bRunning = true;
	UE_LOG(LogTemp, Log, TEXT("UnpaidIntern bridge stub started on 127.0.0.1:%d (no real socket yet)"), Port);
}

void UUIInternBridge::Stop()
{
	bRunning = false;
	UE_LOG(LogTemp, Log, TEXT("UnpaidIntern bridge stub stopped"));
}

FString UUIInternBridge::BuildObservationJson() const
{
	UUIInternSubsystem* Sys = Subsystem.Get();
	if (Sys && Sys->GetObservationBuilder())
	{
		return Sys->GetObservationBuilder()->BuildJson(Sys);
	}
	return TEXT("{\"tick\":0,\"session\":{\"topology\":\"second_licence\"},\"intern\":{},\"nearby_nodes\":[],\"chat_inbox\":[]}");
}

FString UUIInternBridge::HandleActionJson(const FString& Body)
{
	TSharedPtr<FJsonObject> Parsed;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
	if (!FJsonSerializer::Deserialize(Reader, Parsed) || !Parsed.IsValid())
	{
		return TEXT("{\"ok\":false,\"error\":\"bad_json\"}");
	}

	const FString Name = Parsed->GetStringField(TEXT("name"));
	UUIInternSubsystem* Sys = Subsystem.Get();
	if (!Sys)
	{
		return TEXT("{\"ok\":false,\"error\":\"no_subsystem\"}");
	}

	const bool bOk = Sys->DispatchAction(Name, Parsed->HasField(TEXT("arguments")) ? Parsed->GetObjectField(TEXT("arguments")) : nullptr);
	return bOk ? TEXT("{\"ok\":true}") : TEXT("{\"ok\":false,\"error\":\"action_failed\"}");
}
