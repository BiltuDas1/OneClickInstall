import fastapi
from ..models import token, apps


def createAppsAPI(app: fastapi.FastAPI):
  @app.post("/v1/fetch")
  async def fetch_apps(request: token.Token):
    """
    Gets the list of application which is allocated with the specific tokenID
    """
    raise NotImplementedError()

  @app.post("/v1/getFiles")
  async def get_files(request: apps.Apps):
    """
    Get the download link of the specific application/scripts
    """
    raise NotImplementedError()

  @app.get("/v1/script/{scriptID}")
  async def download_script(scriptID: str):
    """
    Serve scripts so that clients can download it
    """
    raise NotImplementedError()