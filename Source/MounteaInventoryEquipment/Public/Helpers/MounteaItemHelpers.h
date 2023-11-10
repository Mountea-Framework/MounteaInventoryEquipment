// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once


UENUM(BlueprintType)
enum class EItemDataSource : uint8
{
	EIDS_SourceTable				UMETA(DisplayName="Source Table"),
	EIDS_SourceItem				UMETA(DisplayName="Source Item"),

	Default									UMETA(hidden)
};

