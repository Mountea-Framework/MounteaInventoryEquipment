# Project-defined item attributes

The inventory can hold two copies of the same sword. One might have rolled 12 damage and the other 17. The template describes the possible attributes and their starting values. Each inventory item needs to retain its own roll when traded, saved, replicated, or equipped.

## Options

| Model | What the designer authors on a template | What each item owns | Project cost |
| --- | --- | --- | --- |
| GAS-defined attributes | Choose one or more project Attribute Set classes, select reflected numeric attributes, and set defaults. | An attribute identifier and a numeric value for each roll or override. | The project defines its own Attribute Set classes. Designers can configure existing attributes without adding plugin classes. |
| Typed JSON | Choose an existing `FMounteaJsonObjectDefinition`, compose definitions using includes, and enter validated field defaults. | Its own typed values for the selected schema. | Supports numbers, strings and booleans. The project supplies calculations and gameplay effects. |
| Typed data with GAS bindings | Author typed fields and bind selected numeric fields to project attributes. | Persistent item data plus the bindings needed when it is equipped or used. | Supports descriptive fields and gameplay modifiers together, but requires conversion rules and an equip lifecycle. |

## Preferred direction for numeric attributes

GAS lets a project reuse the attributes it already defines. An `FGameplayAttribute` identifies a reflected attribute on a `UAttributeSet`. A live set registered with the actor's Ability System Component participates in effect evaluation. `FGameplayAttributeData` distinguishes the base value from the current value after active effects. Selecting a set class on a template alone does not give each item an independent live set. [Epic's Gameplay Attributes and Attribute Sets](https://dev.epicgames.com/documentation/unreal-engine/gameplay-attributes-and-attribute-sets-for-the-gameplay-ability-system-in-unreal-engine?application_version=5.6)

For the two swords, the template could default to 10 damage and each `FMounteaInventoryItem` could store its roll. Equipping the 17-damage sword would contribute its value to the owner. Unequipping it would remove that contribution. A temporary buff on the owner would end without changing either sword's saved roll.

## Questions for implementation

- **Persistence and replication.** Include item values in the save format, item snapshot, and custom `NetSerialize` path. Verify with two clients that changing one item leaves the other copy independent.
- **Stack identity.** Decide which attributes affect stack compatibility. Splitting a stack must retain those values. Merging distinct rolls must preserve both or reject the merge.
- **Template evolution.** Define what happens to existing items when a project changes defaults, renames an attribute, or removes a field. Distinguish an explicit zero roll from an absent value.
- **Ownership of effects.** If an equipment integration applies a gameplay effect, track its handle per equipped item so replacing one sword removes only its contribution.

These are design options. No attribute storage format or GAS dependency has been selected for implementation.
