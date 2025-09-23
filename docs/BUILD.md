# Building From Source
- [Building From Source](#building-from-source)
  - [Build using Docker (Recommended)](#build-using-docker-recommended)
  - [Building on Windows](#building-on-windows)
    - [Windows Application](#windows-application)
    - [Web Application](#web-application)

## Build using Docker (Recommended)

- [Download and Install Docker](https://www.docker.com/)
- Clone this repository
- Build the Docker Image
```
docker buildx build -f Dockerfile.build -t biltudas1/oneclickinstall:latest .
```

## Building on Windows

### Windows Application

- Install [Qt Online Installer](https://www.qt.io/download-qt-installer-oss), It will install all the required Packages
- Now Clone this repo and Open this repo in Qt Creator
- Build the Project  
> Note: By default Qt Online Installer provides Shared Qt, which doesn't link libraries with the final binary. If you are looking for Standalone Binary then you need to build Static Qt from the offical source or you can use [Engima Virtualbox](https://enigmaprotector.com/en/aboutvb.html).

### Web Application

- Install [Python](https://www.python.org/) and [Poetry](https://python-poetry.org/)
- Install all the packages
```
poetry install --without dev
```
- Run The Web Server
```
poetry run uvicorn api.server:app
```