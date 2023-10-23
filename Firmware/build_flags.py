import subprocess

from datetime import date


today = date.today()

try:
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
except:
    revision = "Unknown"

try:
    host = (
        subprocess.check_output(
            [
                "hostname",
            ]
        )
        .strip()
        .decode("utf-8")
    )
except:
    host = "Unknown"

try:
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
except:
    username = "Unknown"

# Cleanup CI
if username == "root":
    username = "gitlab"
    host = "gitlab"

print(f"-DREVISION='\"{revision}\"'")

# This is for the boardConfig
print("-I include/v2_Board")
