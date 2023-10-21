import subprocess, pathlib

from datetime import date


today = date.today()

# Apply any patches
subprocess.run(
    [
        "patch",
        "-N",
        f"{pathlib.Path.home()}/.platformio/packages/framework-arduinorenesas-uno/libraries/Arduino_FreeRTOS/src/FreeRTOSConfig.h",
        "patches/stackWatermark.patch",
    ],
    stdout=subprocess.DEVNULL,
)


# Set revision
revision = (
    subprocess.check_output(
        [
            "git",
            "describe",
            "--abbrev=8",
            "--dirty",
            "--always",
            "--tags",
        ]
    )
    .strip()
    .decode("utf-8")
)

host = (
    subprocess.check_output(
        [
            "hostname",
        ]
    )
    .strip()
    .decode("utf-8")
)

username = (
    subprocess.check_output(
        [
            "id",
            "-u",
            "-n",
        ]
    )
    .strip()
    .decode("utf-8")
)

# Cleanup CI
if username == "root":
    username = "gitlab"
    host = "gitlab"

print(f"-DREVISION='\"{revision}\"'")

# This is for the boardConfig
print("-I include/v2_Board")
