import fastapi
from ..db import mysql, error


def createHealthCheckAPI(app: fastapi.FastAPI):
  @app.get("/status")
  async def status():
    """
    Status Page for Health Check
    """
    resp = {
      "status": "OK",
      "dependencies": {}
    }

    # Check MySQL Service
    try:
      with mysql.MySQL() as DB:
        result = DB.Query("SELECT 1")
        resp["dependencies"]["mysql"] = "OK"
        if len(result) == 0:
          resp["status"] = "ERROR"
          resp["dependencies"]["mysql"] = "NOT WORKING"
    except error.DatabaseError:
      resp["status"] = "ERROR"
      resp["dependencies"]["mysql"] = "DISCONNECTED"
    
    return resp