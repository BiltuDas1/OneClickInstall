from watchfiles import awatch
import asyncio
import subprocess
from api.debug import colorlog
import logging


# Logging
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)
stream = logging.StreamHandler()
stream.setFormatter(colorlog.ColorFormatter("%(levelname)s %(message)s"))
logger.addHandler(stream)
BUILD_SCRIPT = "./scripts/build.sh"


async def watch():
  subprocess.run(["bash", BUILD_SCRIPT])
  logger.info("Build Done")

  paths_to_watch = ["windows/"]
  logger.info("Autobuilding Process Started... watching for changes")

  try:
    async for _ in awatch(*paths_to_watch):
      logger.info("Changes detected, recompiling binary...")
      subprocess.run(["bash", BUILD_SCRIPT])
      logger.info("Process Complete")
  except asyncio.exceptions.CancelledError:
    pass

if __name__ == "__main__":
  asyncio.run(watch())