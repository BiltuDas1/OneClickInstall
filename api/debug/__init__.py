from . import watcher
import asyncio


def reload():
  asyncio.run(watcher.watch())