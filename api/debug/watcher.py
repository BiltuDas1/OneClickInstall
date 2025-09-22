import asyncio
from watchfiles import awatch
from pathlib import Path
from .semaphore import AsyncFileSemaphore


async def watch():
  """
  Watches files for changes and restarts automatically.
  """
  paths_to_watch = ["api/", "assets/"]
  started = True
  reload_lock = AsyncFileSemaphore("reload.lock")

  while started:
    # Start Uvicorn as a subprocess
    process = await asyncio.create_subprocess_exec(
      "uvicorn", "api.debug.middleware:app"
    )

    print("Server started... watching for changes")
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
          print(f"Changes detected, reloading frontend...")
          reload_lock.set()
          continue

        if isApiChanged:
          print(f"Changes detected, restarting server...")
          process.terminate()
          await process.wait()
          break
      
    except asyncio.exceptions.CancelledError:
      # If User Pressed Ctrl + C, then Terminate Process
      process.terminate()
      await process.wait()
      print("Process Stopped")
      started = False
