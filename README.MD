# 🕵️ Coup++

A full-featured modern C++ version of the classic bluffing and deduction game, **Coup** — now with a GUI and unique role mechanics.

---

## 🚀 Features

* Full gameplay logic with 6 custom roles
* Interactive GUI built using **wxWidgets**
* Role-based abilities and action blocking
* Sound-ready (commented for future implementation)
* Clean architecture for easy extension

---

## 🧾 Game Rules

### 🎯 Objective

Be the last player standing by eliminating all others through strategic moves and deception.

### 🎭 Roles (max 2 per role)

| Role     | Ability Description                                                   |
| -------- | --------------------------------------------------------------------- |
| Spy      | Can observe or interfere with another player's ability (smoke-themed) |
| Baron    | Influences tax income or redistributes coins                          |
| General  | Can block certain actions (like coups)                                |
| Governor | Gains bonus from specific actions                                     |
| Judge    | Can impose sanctions, possibly blocking or penalizing                 |
| Merchant | Gains passive coin bonus over time                                    |

### 🌀 Actions

| Action   | Description                                                           |
| -------- | --------------------------------------------------------------------- |
| Tax      | Collect extra coins (usually 3) if role allows                        |
| Bribe    | Pay coins to take an extra turn                                       |
| Arrest   | Steal coins from another player                                       |
| Sanction | Impose a temporary penalty on a player’s action/coins                 |
| Coup     | Pay 7 coins to eliminate a player (mandatory if player has 10+ coins) |

* Some actions can be **challenged** or **blocked** based on roles.
* Failed blocks or false claims may result in loss of a role.

### 💰 Coins & Forced Coup

* Players earn 1–3 coins per turn based on action.
* 10 or more coins? You **must** perform a coup.

---

## 🖥️ GUI Overview

The GUI is implemented using **wxWidgets**, featuring:

* Menu and game windows (`MenuFrame`, `GameFrame`)
* Interactive panels (`MenuPanel`, `GamePanel`)
* Image-based buttons and custom background rendering
* Hover effects, click zones for actions, and modal prompts

### 🛠 Dependencies

To build and run the GUI version:

#### Windows

* Install [wxWidgets](https://www.wxwidgets.org/downloads/)
* Use `MSYS2` or `WSL` with `g++` or `clang++`

```bash
pacman -S mingw-w64-x86_64-wxWidgets
```

#### Linux/WSL

```bash
sudo apt update
sudo apt install libwxgtk3.0-gtk3-dev
```

#### Build Command (example)

```bash
g++ -std=c++20 App.cpp GameFrame.cpp GamePanel.cpp MenuFrame.cpp MenuPanel.cpp \
    ../game/Game.cpp ../game/player/Player.cpp -o coup_gui `wx-config --cxxflags --libs`
```

---

## 🧠 Project Structure

| Folder/File          | Purpose                           |
| -------------------- | --------------------------------- |
| `game/`              | Game logic and role mechanics     |
| `player/`            | Role-specific player classes      |
| `App.h/.cpp`         | wxWidgets app entry point         |
| `GameFrame`, `Panel` | GUI windows and interaction zones |
| `MenuFrame`, `Panel` | Main menu interface               |

---

## 🤘 Final Notes

> "There's someone reaching for the fight when the thunder calls!"

Yes, we’re **holding out for a hero** — and this game might just make you feel like one.

---

## 📩 Contributing

PRs welcome. Smoke bombs optional. Just don’t forget to `#include <fun>`.
