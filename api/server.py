from fastapi import FastAPI, Request
from fastapi.templating import Jinja2Templates
from fastapi.staticfiles import StaticFiles
from fastapi.responses import HTMLResponse
from fastapi.middleware import cors
from .routes import download, app_v1, status
import tomllib
import os


app = FastAPI()
with open("pyproject.toml", "rb") as f:
  PROJECT_CONFIG = tomllib.load(f)

HOSTNAME = os.getenv("HOST_ADDRESS", PROJECT_CONFIG["tool"]["oneclickinstall"]["api"]["base_url"])

# Adding CORS
app.add_middleware(
  cors.CORSMiddleware,
  allow_origins=[HOSTNAME],
  allow_credentials=True,
  allow_methods=["*"],
  allow_headers=["*"]
)

# Mount the assets folder at root URL
app.mount("/css", StaticFiles(directory="assets/css/", html=True), name="css")
app.mount("/js", StaticFiles(directory="assets/js/", html=True), name="js")
templates = Jinja2Templates(directory="assets/")

# Serve homepage
@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
  return templates.TemplateResponse("index.html", {"request": request, "title": "Homepage"})


# Routes
download.createDownloadRouter(app)
app_v1.createAppsAPI(app, PROJECT_CONFIG)
status.createHealthCheckAPI(app)