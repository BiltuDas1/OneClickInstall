from . import watcher
import asyncio
import signal

# Telling all the async tasks to stop
async def shutdown(loop):
  for task in asyncio.all_tasks(loop):
    task.cancel()

def reload():
  loop = asyncio.get_event_loop()

  # Catch SIGTERM & SIGINT (Ctrl + C)
  for sig in [signal.SIGTERM, signal.SIGINT]:
    loop.add_signal_handler(sig, lambda: asyncio.create_task(shutdown(loop)))

  loop.run_until_complete(watcher.watch())