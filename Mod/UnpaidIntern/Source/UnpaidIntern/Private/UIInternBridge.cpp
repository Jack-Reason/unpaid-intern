// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIInternBridge.h"
#include "UIInternSubsystem.h"
#include "UIInternCharacter.h"
#include "UIInternSettings.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void UUIInternBridge::Start(UUIInternSubsystem* Owner, int32 Port)
{
	Subsystem = Owner;
	ListenPort = Port;
	// TODO(M1): Bind TCP listener to 127.0.0.1:Port only — never 0.0.0.0.
	// Prefer an existing community lib (ALWebsocketLib) or UE HttpServer module if available in Starter Project.
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
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetNumberField(TEXT("tick"), Sys ? Sys->GetWorldTick() : 0);

	TSharedRef<FJsonObject> Session = MakeShared<FJsonObject>();
	Session->SetStringField(TEXT("mode"), TEXT("listen_host"));
	Session->SetStringField(TEXT("bridge"), TEXT("stub"));
	Root->SetObjectField(TEXT("session"), Session);

	TSharedRef<FJsonObject> Intern = MakeShared<FJsonObject>();
	if (Sys && Sys->GetIntern())
	{
		AUIInternCharacter* Char = Sys->GetIntern();
		const FVector Loc = Char->GetActorLocation();
		TSharedRef<FJsonObject> Location = MakeShared<FJsonObject>();
		Location->SetNumberField(TEXT("x"), Loc.X);
		Location->SetNumberField(TEXT("y"), Loc.Y);
		Location->SetNumberField(TEXT("z"), Loc.Z);
		Intern->SetObjectField(TEXT("location"), Location);
		Intern->SetStringField(TEXT("job"), Char->CurrentJob);
		Intern->SetStringField(TEXT("id"), TEXT("unpaid-intern-1"));
	}
	Root->SetObjectField(TEXT("intern"), Intern);

	// TODO(M1): Fill host pose, nearby_nodes, chat_inbox, inventory from game subsystems.
	Root->SetArrayField(TEXT("nearby_nodes"), TArray<TSharedPtr<FJsonValue>>());
	Root->SetArrayField(TEXT("chat_inbox"), TArray<TSharedPtr<FJsonValue>>());

	FString Out;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
	FJsonSerializer::Serialize(Root, Writer);
	return Out;
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

	const bool bOk = Sys->DispatchAction(Name, Parsed->GetObjectField(TEXT("arguments")));
	return bOk ? TEXT("{\"ok\":true}") : TEXT("{\"ok\":false,\"error\":\"action_failed\"}");
}
