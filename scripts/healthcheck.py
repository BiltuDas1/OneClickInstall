import sys
import urllib.request

url = "http://127.0.0.1:8000/status"
timeout_seconds = 2

try:
  with urllib.request.urlopen(url, timeout=timeout_seconds) as response:
    if response.status == 200:
      print(f"Health check passed (Status: {response.status})")
      sys.exit(0)
    else:
      print(f"Health check failed (Status: {response.status})")
      sys.exit(1)

except Exception as e:
  print(f"Health check failed with exception: {e}")
  sys.exit(1)