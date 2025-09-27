from ..utils import patcher
import os

# Getting build path
if os.environ.get("DOCKER"):
  FILE_PATH = os.path.join("OneClickInstall.exe")
else:
  FILE_PATH = os.path.join("build", "OneClickInstall.exe")

# Variable that stores the Binary Cache
BINARY_CACHE: bytearray | None = None


def get_exe(token: str) -> str | bytearray:
  """
  Replace the Token in the Executable
  """
  global BINARY_CACHE

  # Check if Cache Exist, if not then load Cache
  if BINARY_CACHE is None:
    if os.path.exists(FILE_PATH):
      with open(FILE_PATH, "rb") as f:
        BINARY_CACHE = bytearray(f.read())
    else:
      return bytearray()
    
  return patcher.patch_executable(BINARY_CACHE, token)

