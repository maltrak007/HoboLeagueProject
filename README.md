# Hobo League

> *Stand up against sadistic machines through the pain of the trials and earn your freedom.*

**Hobo League** is a solo third-person **multiplayer** game by [Goro7Games](https://github.com/maltrak007), built in **Unreal Engine 5.6 and C++**. It is a deliberate deep dive into scalable architecture: the Gameplay Ability System is implemented from scratch with network replication, and every other system is built around it.

**Status:** 🚧 In progress

| | |
|---|---|
| **Engine** | Unreal Engine 5.6 |
| **Language** | C++ with Blueprint extension points |
| **Core tech** | Gameplay Ability System (GAS), network replication, World Partition, Chaos Destruction, Animation Warping |
| **Planning** | Kanban (Miro), UML diagrams |

## Goals

Deepen architectural skills and multiplayer best practice: event-driven design, SOLID principles and explicit C++ memory management, with a clean seam so designers can iterate in Blueprints.

## Features

- **Custom GAS framework** — `UHAbilitySystemComponent`, a replicated `UHAttributeSet` (Health, Stamina, Overdose, ChargeBar, MovementSpeed and their maximums, clamped in `PreAttributeChange`) and a shared `UHBaseGameplayAbility`.
- **Player abilities** — `GA_Attack`, `GA_Sprint`, `GA_Interact`, `GA_ChargeAction`, `GA_SwapItem`, `GA_UseConsumable` and `GA_RegenerateStamina` (driven by a `GE_StaminaRegeneration` effect).
- **Hold-and-release minigame** — a custom ability task (`HAbilityTask_HoldReleaseMinigame`) with a tiered UI (`HHoldReleaseMinigame`, `HTierBox`) for skill-based charge actions.
- **Inventory and items** — `UHInventoryComponent` handles add / remove / equip / unequip with per-type limits and auto-replacement. Items are data-driven (`HBaseItemDataAsset`), split into weapons, consumables and non-player scenario items.
- **Overdrive system** — consuming the right combination of consumables triggers an Overdrive, resolved by `UHOverdriveComponent` against a combination data asset and gameplay-tag count changes.
- **Interaction** — `HInteractionComponent` plus an item-interactable interface; scenario elements include item generators and barrier checkers.
- **Status handling** — `HStatusHandlerComponent` for status effects on characters.
- **AI characters** — `BaseAICharacter` as the foundation for the machine opponents.
- **UI via widget controllers** — `HoboWidgetController` and `OverlayWidgetController` decouple HUD widgets from gameplay data.
- **Multiplayer sessions** — bundled `MultiplayerSessions` plugin (menu + online session subsystem) for hosting and joining games.
- **Animation hooks** — anim notifies that send gameplay events, trace item collisions and send target groups, keeping animation and ability logic in sync.

## Project layout

```
Source/HoboLeagueProject/
  Character/        BaseCharacter, PlayerCharacter, AI characters, anim instance, notifies
  Component/        Inventory, Interaction, Overdrive, StatusHandler
  GAS/              AbilitySystemComponent, AttributeSet, abilities, effects, ability tasks, tags
  Item/             Base items, data assets, consumables, weapons, scenario elements
  UI/               Widgets, widget controllers, minigame
Plugins/MultiplayerSessions/   Session creation / join subsystem and menu
Content/HoboLeague/            GAS, Game, Gyms, Item, Player, Scenario, UI assets
```

## Getting started

1. Install **Unreal Engine 5.6** with the C++ workload (Visual Studio 2022).
2. Clone the repository (assets are stored in the repo, so expect a large download).
3. Right-click `HoboLeagueProject.uproject` → *Generate Visual Studio project files*.
4. Build the `HoboLeagueProject` target (Development Editor) and open the project.
5. To test multiplayer use *Play → Number of Players: 2+* with *Net Mode: Play as Listen Server*.

## Roadmap

- Expand the trial/arena content and the machine opponents.
- Polish Overdrive combinations and consumable balance.
- Wider use of Chaos Destruction in dynamic environments.

## Credits

Project by **Gonzalo Liñán Aldana** ([Goro7Games](https://github.com/maltrak007)) · [LinkedIn](https://www.linkedin.com/in/gonzalo-liñan-aldana-934283221/). The workspace also includes marketplace/starter content (Lyra Starter essentials, Megascans presets, UE mannequins), which remains under its original licenses.
