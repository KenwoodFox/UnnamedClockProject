import subprocess

from datetime import date


today = date.today()

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
