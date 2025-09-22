import asyncio
from watchfiles import awatch
from pathlib import Path
from .semaphore import AsyncFileSemaphore
import os
import logging
from .colorlog import ColorFormatter


# Logging
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)
stream = logging.StreamHandler()
stream.setFormatter(ColorFormatter("%(levelname)s %(message)s"))
logger.addHandler(stream)


async def run_server():
  """
  Run Uvicorn Server
  """
  env = os.environ.copy()
  env.update({"DEBUG": "1"})

  # Start Uvicorn as a subprocess
  process = await asyncio.create_subprocess_exec(
    "uvicorn", "api.debug.middleware:app", "--host", "0.0.0.0", "--port", "8000", "--use-colors",
    env=env
  )
  return process


async def watch():
  """
  Watches files for changes and restarts automatically.
  """
  paths_to_watch = ["api/", "assets/"]
  started = True
  reload_lock = AsyncFileSemaphore("reload.lock")

  while started:
    process = await run_server()

    logger.info("Server started... watching for changes")
    reload_lock.set() # Telling all the clients to reload

    assetsPath = str(Path(str(Path.cwd()) + "/assets").absolute().resolve())
    apiPath = str(Path(str(Path.cwd()) + "/api").absolute().resolve())

    # Watch for changes
    try:
      async for changes in awatch(*paths_to_watch):
        isAssetsChanged = False
        isApiChanged = False

        for item in changes:
          filePath = str(Path(item[1]).absolute().resolve())
          isAssetsChanged = filePath.startswith(assetsPath) or isAssetsChanged
          isApiChanged = filePath.startswith(apiPath) or isApiChanged

        if isAssetsChanged:
          logger.info(f"Changes detected, reloading frontend...")
          reload_lock.set()
          continue

        if isApiChanged:
          logger.info(f"Changes detected, restarting server...")
          process.terminate()
          await process.wait()
          break
      
    except asyncio.exceptions.CancelledError:
      # If User Pressed Ctrl + C, then Terminate Process
      process.terminate()
      await process.wait()
      logger.info("Process Stopped")
      started = False
