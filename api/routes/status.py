import fastapi


def createHealthCheckAPI(app: fastapi.FastAPI):
  @app.get("/status")
  async def status():
    """
    Status Page for Health Check
    """
    return {
      "status": "OK"
    }