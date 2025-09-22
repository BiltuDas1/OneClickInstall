import asyncio
from pathlib import Path

class AsyncFileSemaphore:
  def __init__(self, path: str, poll_interval: float = 0.1):
    self.path = Path(path)
    self._poll_interval = poll_interval
    self._waiters = []

  async def wait(self):
    """Wait until the file exists."""
    if self.path.exists():
      return  # Already available

    fut = asyncio.get_event_loop().create_future()
    self._waiters.append(fut)

    try:
      while not self.path.exists():
        await asyncio.sleep(self._poll_interval)
      fut.set_result(True)
    finally:
      self._waiters.remove(fut)

  def set(self):
    """Manually create the file (signal the semaphore)."""
    self.path.touch(exist_ok=True)
    # Wake up any waiters
    for fut in self._waiters:
      if not fut.done():
        fut.set_result(True)

  def clear(self):
    """Delete the file (reset the semaphore)."""
    if self.path.exists():
      self.path.unlink()

  def is_set(self):
    return self.path.exists()