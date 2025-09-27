from fastapi import FastAPI, Request
from fastapi.templating import Jinja2Templates
from fastapi.staticfiles import StaticFiles
from fastapi.responses import HTMLResponse
from .routes import download


app = FastAPI()

# Mount the assets folder at root URL
app.mount("/css", StaticFiles(directory="assets/css/", html=True), name="css")
app.mount("/js", StaticFiles(directory="assets/js/", html=True), name="js")
templates = Jinja2Templates(directory="assets/")

# Serve homepage
@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
  return templates.TemplateResponse("index.html", {"request": request, "title": "Homepage"})

# Download functions
download.createDownloadRouter(app)
