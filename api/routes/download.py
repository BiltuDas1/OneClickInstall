import fastapi
from fastapi.responses import JSONResponse, Response
from ..services import gen_exe


def createDownloadRouter(app: fastapi.FastAPI):
  @app.get("/download/{tokenID}")
  async def download_exe(tokenID: str):
    """
    Patches the executable with a custom token, and then let the user to download the exe
    """
    if len(tokenID) == 0:
      return JSONResponse(
        content={"error": "no tokenID"},
        status_code=400
      )

    file = gen_exe.get_exe(tokenID)

    if isinstance(file, str):
      return JSONResponse(
        content={"error": file},
        status_code=400
      )

    return Response(
      content=bytes(file),
      media_type="application/octet-stream",
      headers={
        'Content-Disposition': f'attachment; filename="OneClickInstall.exe"'
      }
    )