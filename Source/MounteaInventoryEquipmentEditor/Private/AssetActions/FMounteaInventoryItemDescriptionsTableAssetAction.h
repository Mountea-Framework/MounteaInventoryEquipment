// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "FMounteaInventoryTableAssetAction_Base.h"

class FMounteaInventoryTableAssetAction_ItemDescriptions : public FMounteaInventoryTableAssetAction_Base
{
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
};