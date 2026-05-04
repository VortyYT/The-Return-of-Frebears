# Fredbear's Family Diner

A free-roam first-person horror game inspired by Five Nights at Freddy's, set in **Fredbear's Family Diner**. Built with **Unreal Engine 5.3** and **C++**.

## Overview

You are the night security guard at Fredbear's Family Diner. Survive from **12 AM to 6 AM** while three animatronics roam the diner with advanced AI. Explore the diner freely, manage limited power and resources, and uncover the dark lore hidden within.

### Animatronics

| Animatronic | Behavior | Unique Mechanic |
|---|---|---|
| **Fredbear** | Stalker — watches from shadows, ambushes from dark corners | Stays on stage until a timer expires; watching him on cameras delays activation |
| **Spring Bonnie** | Flanker — uses vents and hallways, hit-and-run tactics | Can enter the vent system to appear near the player; retreats when spotted |
| **Golden Foxy** | Rusher — dormant in storage, then explosive sprint attacks | Rush attacks down hallways; blocked by closed doors; more aggressive as power drops |

### Key Features

- **Free-roam exploration** of a fully modeled diner (main dining area, stage, kitchen, storage rooms, security office, hallways, vents, secret rooms)
- **Advanced animatronic AI** with state machines (Idle → Patrolling → Stalking → Hunting → Aggressive), perception (sight + hearing), and pathfinding with fallback behaviors
- **Power management system** — flashlight, cameras, and security doors all drain power
- **Security camera system** with render-target feeds and static interference near animatronics
- **Stamina system** for sprinting with exhaustion and recovery
- **Flashlight** with battery drain and low-battery flickering
- **Interactive doors** (regular and security) that block animatronic movement
- **Dynamic lighting** — flickering lights, darkness zones, random environmental events
- **3D spatial audio** — footsteps, whispers, metal sounds, ambient horror soundscape
- **Randomized events** — doors slamming, power surges, vent rattling, camera static
- **Lore collectibles** — posters, notes, recordings, photos, hidden items
- **Save system** with progress tracking, statistics, and unlockable extras
- **Random seed system** for unique playthroughs
- **Multiple difficulty levels** (Easy, Normal, Hard, Nightmare)
- **Minimal HUD** — time, power bar, stamina, flashlight battery, interaction prompts
- **Full menu system** — Play, Settings (Graphics/Audio/Keybinds), Extras (Lore/Stats/Profiles)
- **Debug subsystem** with console commands for testing

---

## Project Structure

```
FredbearsFamilyDiner/
├── FredbearsFamilyDiner.uproject      # UE5 project file
├── Config/                             # Engine/game/input configuration
│   ├── DefaultEngine.ini
│   ├── DefaultGame.ini
│   ├── DefaultInput.ini
│   └── DefaultEditor.ini
├── Content/                            # Asset directories (add your assets here)
│   ├── Maps/                           # Level maps
│   ├── Blueprints/                     # Blueprint assets
│   ├── Materials/                      # Materials and shaders
│   ├── Textures/                       # Texture assets
│   ├── Audio/                          # Sound effects and music
│   ├── UI/                             # Widget blueprints
│   ├── Animations/                     # Animation assets
│   └── Meshes/                         # 3D model assets
├── Source/
│   ├── FredbearsFamilyDiner.Target.cs
│   ├── FredbearsFamilyDinerEditor.Target.cs
│   └── FredbearsFamilyDiner/
│       ├── FredbearsFamilyDiner.Build.cs
│       ├── FredbearsFamilyDiner.h      # Global constants, enums, log categories
│       ├── FredbearsFamilyDiner.cpp
│       ├── Core/                       # Game framework classes
│       │   ├── FFDGameMode             # Night management, win/loss conditions
│       │   ├── FFDGameState            # Power, threat, collectible tracking
│       │   ├── FFDGameInstance         # Save/load, settings persistence
│       │   ├── FFDSaveGame             # Serializable save data
│       │   └── FFDPlayerController     # Enhanced Input bindings
│       ├── Player/                     # Player character and components
│       │   ├── FFDPlayerCharacter      # First-person character, noise system
│       │   ├── FFDFlashlightComponent  # Flashlight with battery and flickering
│       │   ├── FFDStaminaComponent     # Sprint stamina with exhaustion
│       │   └── FFDInteractionComponent # Ray-cast interaction system
│       ├── Animatronics/               # AI characters
│       │   ├── AnimatronicBase         # Base class: state machine, perception, combat
│       │   ├── Fredbear               # Stalker with stage/camera mechanics
│       │   ├── SpringBonnie           # Vent-using flanker
│       │   └── GoldenFoxy             # Rush attacker
│       ├── Systems/                    # Game-wide systems
│       │   ├── PowerSystem             # Power drain management
│       │   ├── CameraSystem            # Security camera switching
│       │   ├── AudioManager            # Ambient/tension audio layers
│       │   └── RandomEventSystem       # Environmental horror events
│       ├── Environment/                # World actors
│       │   ├── FFDInteractableInterface # Interaction interface
│       │   ├── InteractableDoor        # Openable/closable doors
│       │   ├── SecurityCamera          # Scene capture cameras
│       │   ├── FlickerLight            # Atmospheric flickering lights
│       │   └── LoreCollectible         # Story collectibles
│       ├── UI/                         # UMG widgets
│       │   ├── FFDMainMenuWidget       # Main menu with night/difficulty select
│       │   ├── FFDHUDWidget            # In-game HUD
│       │   ├── FFDSettingsWidget       # Graphics/Audio/Keybind settings
│       │   ├── FFDCameraWidget         # Camera feed overlay with static
│       │   └── FFDExtrasWidget         # Lore/Stats/Profiles viewer
│       └── Debug/
│           └── FFDDebugSubsystem       # Development console commands
```

---

## Prerequisites

- **Unreal Engine 5.3+** (download from [Epic Games Launcher](https://www.unrealengine.com/download))
- **Visual Studio 2022** (Windows) or **Xcode 14+** (macOS) with C++ game development workload
- **Git** for version control

---

## Build & Run Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/VortyYT/FredbearsFamilyDiner.git
cd FredbearsFamilyDiner
```

### 2. Generate Project Files

**Windows:**
Right-click `FredbearsFamilyDiner.uproject` → "Generate Visual Studio project files"

Or from command line:
```bash
"C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="<full_path>/FredbearsFamilyDiner.uproject" -game -engine
```

**macOS/Linux:**
```bash
<UE5_Install_Path>/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh "<full_path>/FredbearsFamilyDiner.uproject"
```

### 3. Open in Unreal Editor

Double-click `FredbearsFamilyDiner.uproject` to open in Unreal Editor. The engine will compile all C++ source files automatically.

### 4. Create Required Assets

The C++ code references assets that you need to create in the Content Browser:

1. **Maps:**
   - `Content/Maps/MainMenu` — Main menu level
   - `Content/Maps/DinerMap` — The playable diner level

2. **Widget Blueprints** (create from the C++ widget classes):
   - `WBP_MainMenu` (parent: `FFDMainMenuWidget`)
   - `WBP_HUD` (parent: `FFDHUDWidget`)
   - `WBP_Settings` (parent: `FFDSettingsWidget`)
   - `WBP_CameraView` (parent: `FFDCameraWidget`)
   - `WBP_Extras` (parent: `FFDExtrasWidget`)

3. **Animatronic Blueprints:**
   - `BP_Fredbear` (parent: `Fredbear`)
   - `BP_SpringBonnie` (parent: `SpringBonnie`)
   - `BP_GoldenFoxy` (parent: `GoldenFoxy`)

4. **Audio Assets** (place in `Content/Audio/`):
   - Ambient sounds, tension layers, music tracks
   - Footstep sounds, metal clangs, whispers
   - Door open/close/slam sounds
   - Jumpscare sounds
   - Flashlight click

5. **3D Models** (place in `Content/Meshes/`):
   - Animatronic models with skeletal meshes
   - Diner environment (walls, floor, ceiling, furniture)
   - Door meshes, camera props

### 5. Set Up the Diner Level

In `DinerMap`:
1. Build the diner layout using BSP or static meshes
2. Place a `NavMesh Bounds Volume` covering the walkable area
3. Place animatronic Blueprints at their starting positions
4. Place `SecurityCamera` actors at desired viewpoints
5. Place `InteractableDoor` actors at door locations (set `bIsSecurityDoor` for security doors)
6. Place `FlickerLight` actors for atmospheric lighting
7. Place `LoreCollectible` actors with lore text
8. Place patrol waypoint actors and assign them to animatronics
9. Place a `PowerSystem` actor
10. Place a `CameraSystem` actor and assign cameras
11. Place an `AudioManager` actor
12. Place a `RandomEventSystem` actor
13. Set the Player Start position in the security office

### 6. Build and Package

**Development Build:**
```
Build → Build Solution (Ctrl+Shift+B in Visual Studio)
```

**Package for Distribution:**
In Unreal Editor: File → Package Project → Windows/Mac/Linux

---

## Controls

| Key | Action |
|---|---|
| WASD | Move |
| Mouse | Look |
| Left Shift | Sprint |
| C / Left Ctrl | Crouch |
| E | Interact |
| F | Toggle Flashlight |
| Tab | Toggle Security Cameras |
| Escape | Pause Menu |

---

## Debug Commands

In non-shipping builds, use the debug subsystem (accessible via Blueprints or console):

- `ToggleDebugHUD` — Show AI states, power, system info overlay
- `SetPowerLevel(float)` — Set power directly
- `SkipToMorning` — Instantly win the night
- `TeleportAnimatronicToPlayer(FName)` — Teleport an animatronic to you
- `SetAnimatronicState(FName, EAnimatronicState)` — Force animatronic state
- `DeactivateAllAnimatronics` — Turn off all animatronics
- `ToggleGodMode` — Invulnerability
- `ToggleInfiniteStamina` — Unlimited sprint
- `ToggleInfiniteBattery` — Unlimited flashlight
- `DumpGameState` — Print all system states to log

---

## Architecture Notes

### AI State Machine

Each animatronic uses a state-based AI system:

```
Deactivated → Idle → Patrolling ↔ Stalking → Hunting → Aggressive → [Attack]
                                                    ↓
                                               Retreating → Idle
```

- **Aggro system**: Proximity, line-of-sight, and player noise build aggression
- **Pathfinding fallback**: After 5 failed pathfinding attempts, the animatronic teleports to the nearest waypoint
- **Difficulty scaling**: Movement speed and aggression scale with difficulty level and night number

### Power System

- Base drain rate: 0.08/sec
- Each active consumer (flashlight, cameras, doors) adds to drain
- Usage level (1-5) shown on HUD
- When power hits 0: all systems shut off, animatronics become more active

### Random Seed System

Each playthrough generates a random seed that affects:
- Animatronic patrol patterns
- Random event timing
- Environmental sound placement

---

## License

This is a fan-made project. Five Nights at Freddy's and all related characters are the property of Scott Cawthon. This project is not affiliated with or endorsed by Scott Cawthon.
