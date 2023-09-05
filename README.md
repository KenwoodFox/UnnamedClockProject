[![Docs](https://github.com/KenwoodFox/UnnamedClockProject/actions/workflows/docs_workflow.yml/badge.svg)](https://github.com/KenwoodFox/UnnamedClockProject/actions/workflows/docs_workflow.yml)
[![Hardware](https://github.com/KenwoodFox/UnnamedClockProject/actions/workflows/hardware_workflow.yml/badge.svg)](https://github.com/KenwoodFox/UnnamedClockProject/actions/workflows/hardware_workflow.yml)
[![Firmware](https://github.com/KenwoodFox/UnnamedClockProject/actions/workflows/firmware_workflow.yml/badge.svg)](https://github.com/KenwoodFox/UnnamedClockProject/actions/workflows/firmware_workflow.yml)


# UnnamedClockProject

![Banner](Static/Banner.png)

This repo contains all the firmware and hardware for the UnnamedClockProject.

If you're looking for the latest docs/builds, see our [Releases Page](https://github.com/KenwoodFox/UnnamedClockProject/releases).

# Getting Started

First, clone this repo (and optionally checkout a branch)

```shell
git clone https://github.com/KenwoodFox/UnnamedClockProject.git
cd UnnamedClockProject
git checkout new_board
```

# Init Submodules

Some libraries and resources are included as submodules, run the following
command to initialize them before opening the main sch

(If you get a missing library error, make sure to do this)

```shell
git submodule update --init --recursive
```
