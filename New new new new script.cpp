#include "natives.h" 
#include <string>
#include <cmath>

// Forward declaration of scriptWait so the loader loop compiles cleanly
void scriptWait(DWORD time);

namespace VoidEngine {
    class VoidMenu {
    private:
        std::string currentMenu = "mainmenu";
        int currentOption = 0;
        int maxOptions = 0;
        bool optionPressed = false;

        // Custom Space Purple Color Theme Values
        int hR = 48, hG = 15, hB = 71, hA = 240;
        int bgR = 15, bgG = 6, bgB = 26, bgA = 220;
        int hiR = 168, hiG = 85, hiB = 247, hiA = 255;
        int txR = 243, txG = 232, txB = 255, txA = 255;

    public:
        std::string CurrentMenu() { return currentMenu; }
        
        void Title(const char* title) {
            maxOptions = 0;
            optionPressed = false;
            
            // Core keyboard controls mapping (Numpad 8 Up, Numpad 2 Down, Numpad 5 Select)
            if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 172)) currentOption = (currentOption > 0) ? currentOption - 1 : maxOptions - 1;
            if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 173)) currentOption = (currentOption < maxOptions - 1) ? currentOption + 1 : 0;
            if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 176)) optionPressed = true;

            // Fixed: Removed the 9th argument (false) to match your 8-argument SDK signature
            GRAPHICS::DRAW_RECT(0.15f, 0.12f, 0.22f, 0.08f, hR, hG, hB, hA);
            
            HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
            HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(title);
            HUD::SET_TEXT_FONT(1);
            HUD::SET_TEXT_SCALE(0.0f, 0.7f);
            HUD::SET_TEXT_COLOUR(txR, txG, txB, txA);
            HUD::SET_TEXT_CENTRE(true);
            HUD::END_TEXT_COMMAND_DISPLAY_TEXT(0.15f, 0.10f, 0);
        }

        bool MenuOption(const char* option, const char* targetMenu) {
            maxOptions++;
            float yPos = 0.16f + (static_cast<float>(maxOptions) * 0.04f);
            bool isHighlighted = (currentOption == maxOptions - 1);

            // Fixed: Removed the 9th argument (false) to match your 8-argument SDK signature
            GRAPHICS::DRAW_RECT(0.15f, yPos + 0.015f, 0.22f, 0.04f, isHighlighted ? hiR : bgR, isHighlighted ? hiG : bgG, isHighlighted ? hiB : bgB, isHighlighted ? hiA : bgA);
            
            HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
            HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(option);
            HUD::SET_TEXT_FONT(0);
            HUD::SET_TEXT_SCALE(0.0f, 0.35f);
            HUD::SET_TEXT_COLOUR(txR, txG, txB, txA);
            HUD::END_TEXT_COMMAND_DISPLAY_TEXT(0.05f, yPos, 0);

            if (isHighlighted && optionPressed) {
                currentMenu = targetMenu;
                currentOption = 0;
                return true;
            }
            return false;
        }

        bool Button(const char* option) {
            maxOptions++;
            float yPos = 0.16f + (static_cast<float>(maxOptions) * 0.04f);
            bool isHighlighted = (currentOption == maxOptions - 1);

            // Fixed: Removed the 9th argument (false) to match your 8-argument SDK signature
            GRAPHICS::DRAW_RECT(0.15f, yPos + 0.015f, 0.22f, 0.04f, isHighlighted ? hiR : bgR, isHighlighted ? hiG : bgG, isHighlighted ? hiB : bgB, isHighlighted ? hiA : bgA);
            
            HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
            HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(option);
            HUD::SET_TEXT_FONT(0);
            HUD::SET_TEXT_SCALE(0.0f, 0.35f);
            HUD::SET_TEXT_COLOUR(txR, txG, txB, txA);
            HUD::END_TEXT_COMMAND_DISPLAY_TEXT(0.05f, yPos, 0);

            return (isHighlighted && optionPressed);
        }

        bool BoolOption(const char* option, bool* toggle) {
            bool pressed = Button(option);
            float yPos = 0.16f + (static_cast<float>(maxOptions) * 0.04f);

            HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
            HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(*toggle ? "[ON]" : "[OFF]");
            HUD::SET_TEXT_FONT(0);
            HUD::SET_TEXT_SCALE(0.0f, 0.35f);
            HUD::SET_TEXT_COLOUR(*toggle ? 0 : 255, *toggle ? 255 : 0, 0, 255);
            HUD::END_TEXT_COMMAND_DISPLAY_TEXT(0.23f, yPos, 0);

            if (pressed) {
                *toggle = !(*toggle);
                return true;
            }
            return false;
        }
    };
}

int splashTimer = 240; 
bool menuOpen = false;
VoidEngine::VoidMenu menu;

bool tenfoldBullets = false;
bool hundredfoldBullets = false;
bool infAmmo = false;
bool noReload = false;
bool ultimateSpeedShoot = false;
bool speedyRpg = false;
bool godmode = false;
bool moneyDrop = false;
bool noMissionFailure = false;

std::string vehicleInputName = "";

void KeyboardInputHelper() {
    MISC::DISPLAY_ONSCREEN_KEYBOARD(1, "FMMC_KEY_TIP8", "", "", "", "", "", 32);
    while (MISC::UPDATE_ONSCREEN_KEYBOARD() == 0) {
        scriptWait(0); 
    }
    if (MISC::UPDATE_ONSCREEN_KEYBOARD() == 1) {
        vehicleInputName = MISC::GET_ONSCREEN_KEYBOARD_RESULT();
    }
}

void DrawMenuText(const char* text, int font, float x, float y, float scaleX, float scaleY, int r, int g, int b, int a, bool center) {
    HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
    HUD::SET_TEXT_FONT(font);
    HUD::SET_TEXT_SCALE(scaleX, scaleY);
    HUD::SET_TEXT_COLOUR(r, g, b, a);
    if (center) HUD::SET_TEXT_CENTRE(true);
    HUD::END_TEXT_COMMAND_DISPLAY_TEXT(x, y, 0);
}

void scriptTick() {
    // Check for F8 Keyboard assignment natively
    if (PAD::IS_CONTROL_JUST_PRESSED(2, 344)) { 
        menuOpen = !menuOpen;
    }

    if (splashTimer > 0) {
        splashTimer--;
        // Fixed: Removed the 9th argument (false) to match your 8-argument SDK signature
        GRAPHICS::DRAW_RECT(0.5f, 0.5f, 1.0f, 1.0f, 10, 5, 20, 255);
        GRAPHICS::DRAW_RECT(0.5f, 0.05f, 1.0f, 0.01f, 147, 51, 234, 200); 
        GRAPHICS::DRAW_RECT(0.5f, 0.95f, 1.0f, 0.01f, 88, 28, 135, 200);  
        
        int alphaPulse = static_cast<int>(155.0f + (100.0f * sinf(static_cast<float>(splashTimer) * 0.05f)));
        DrawMenuText("WELCOME TO VOID MENU", 1, 0.5f, 0.40f, 1.2f, 1.2f, 168, 85, 247, alphaPulse, true);
        DrawMenuText("made by ccgvr1 on TikTok", 4, 0.5f, 0.52f, 0.6f, 0.6f, 255, 255, 255, 230, true);
        DrawMenuText("Loading system files...", 0, 0.5f, 0.85f, 0.35f, 0.35f, 156, 163, 175, 150, true);
        return; 
    }

    if (menuOpen) {
        if (menu.CurrentMenu() == "mainmenu") {
            menu.Title("Void Menu");
            if (menu.MenuOption("Player Options", "playersub")) {}
            if (menu.MenuOption("Weapon Options", "weaponsub")) {}
            if (menu.MenuOption("Vehicle Spawner", "vehiclesub")) {}
        }

        if (menu.CurrentMenu() == "playersub") {
            menu.Title("Player Options");
            if (menu.BoolOption("God Mode", &godmode)) {}
            if (menu.BoolOption("Money Drop", &moneyDrop)) {}
            if (menu.BoolOption("No Mission Failure", &noMissionFailure)) {}
        }

        if (menu.CurrentMenu() == "weaponsub") {
            menu.Title("Weapon Options");
            if (menu.BoolOption("Tenfold Bullets (x10)", &tenfoldBullets)) {}
            if (menu.BoolOption("Hundredfold Bullets (x100)", &hundredfoldBullets)) {}
            if (menu.BoolOption("Infinite Ammo", &infAmmo)) {}
            if (menu.BoolOption("No Reloading", &noReload)) {}
            if (menu.BoolOption("Ultimate Speed Shoot", &ultimateSpeedShoot)) {}
            if (menu.BoolOption("Speedy RPG", &speedyRpg)) {}
            
            if (menu.Button("Give All Weapons")) {
                Ped p = PLAYER::PLAYER_PED_ID();
                WEAPON::GIVE_DELAYED_WEAPON_TO_PED(p, MISC::GET_HASH_KEY("WEAPON_CARBINERIFLE"), 9999, true);
                WEAPON::GIVE_DELAYED_WEAPON_TO_PED(p, MISC::GET_HASH_KEY("WEAPON_RPG"), 9999, true);
            }
            if (menu.Button("Remove All Weapons")) {
                WEAPON::REMOVE_ALL_PED_WEAPONS(PLAYER::PLAYER_PED_ID(), true);
            }
        }

        if (menu.CurrentMenu() == "vehiclesub") {
            menu.Title("Vehicle Spawner");
            if (menu.Button("Type Vehicle Name to Spawn")) {
                KeyboardInputHelper(); 
                if (!vehicleInputName.empty()) {
                    Hash modelHash = MISC::GET_HASH_KEY(vehicleInputName.c_str());
                    if (STREAMING::IS_MODEL_IN_CDIMAGE(modelHash)) {
                        STREAMING::REQUEST_MODEL(modelHash);
                        while (!STREAMING::HAS_MODEL_LOADED(modelHash)) { scriptWait(0); }
                        Ped p = PLAYER::PLAYER_PED_ID();
                        Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(p, 0.0f, 5.0f, 0.0f);
                        Vehicle veh = VEHICLE::CREATE_VEHICLE(modelHash, coords.x, coords.y, coords.z, 0.0f, true, false, false);
                        PED::SET_PED_INTO_VEHICLE(p, veh, -1);
                        STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
                    }
                    vehicleInputName = ""; 
                }
            }
        }
    }

    // --- FEATURE BACKGROUND LOOP EXECUTION ---
    Ped playerPed = PLAYER::PLAYER_PED_ID();
    Player player = PLAYER::PLAYER_ID();

    if (godmode) {
        PLAYER::SET_PLAYER_INVINCIBLE(player, true);
        ENTITY::SET_ENTITY_PROOFS(playerPed, true, true, true, true, true, true, true, true);
    } else {
        PLAYER::SET_PLAYER_INVINCIBLE(player, false);
    }

    if (moneyDrop) {
        Vector3 c = ENTITY::GET_ENTITY_COORDS(playerPed, true);
        OBJECT::CREATE_AMBIENT_PICKUP(MISC::GET_HASH_KEY("PICKUP_MONEY_CASE"), c.x, c.y, c.z + 0.5f, 0, 25000, 0, false, true);
    }

    if (noMissionFailure) {
        MISC::SET_MISSION_FLAG(true);
    } if (infAmmo) {
WEAPON::SET_PED_INFINITE_AMMO_CLIP(playerPed, true);
} else {
WEAPON::SET_PED_INFINITE_AMMO_CLIP(playerPed, false);
}
if (noReload) {
Hash cur;
if (WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &cur, true)) {
WEAPON::SET_AMMO_IN_CLIP(playerPed, cur, WEAPON::GET_MAX_AMMO_IN_CLIP(playerPed, cur, true));
}
}
}
