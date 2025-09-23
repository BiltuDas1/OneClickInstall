from fastapi import Request, Response
from fastapi.responses import StreamingResponse
from starlette.middleware.base import _StreamingResponse
from ..server import app
from .semaphore import AsyncFileSemaphore
import signal
import os


RELOAD_SNIPPET = "<script>new EventSource('/events').onmessage = e => e.data === 'reload' && window.location.reload(true);</script>\n"

# Events which tells when to reload
@app.get("/events")
async def events():
  async def event_generator():
    reload_lock = AsyncFileSemaphore("reload.lock")
    while True:
      await reload_lock.wait()
      yield "data: reload\n\n"
      reload_lock.clear()
  return StreamingResponse(event_generator(), media_type="text/event-stream")


# Middleware to Reload on Browser Side
# Source: https://stackoverflow.com/a/71883126
@app.middleware("http")
async def inject_reload(request: Request, call_next):
  resp: _StreamingResponse = await call_next(request)

  # Only Inject Script in HTML
  if resp.headers.get("content-type", "").startswith("text/html"):
    chunks = []
    async for chunk in resp.body_iterator:
      chunks.append(chunk)
    response_body = b''.join(chunks)
    
    # Adding RELOAD_SNIPPET after <body>
    resp_body_str = response_body.decode()
    body_end = resp_body_str.find("</body>")
    new_resp = resp_body_str[:body_end] + RELOAD_SNIPPET + resp_body_str[body_end:]
    response_body = new_resp.encode()
    headers = dict(resp.headers)
    if (content_length := headers.get('content-length')) is not None:
      headers['content-length'] = str(int(content_length) + len(RELOAD_SNIPPET))

    return Response(content=response_body, status_code=resp.status_code, 
          headers=headers, media_type=resp.media_type)
  return resp


# Terminate the Process Forcefully (When Exit Signal Received)
def force_exit(sig, frame):
  os._exit(0)

signal.signal(signal.SIGINT, force_exit)
signal.signal(signal.SIGTERM, force_exit)