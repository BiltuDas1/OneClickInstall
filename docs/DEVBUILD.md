# Development Build

- [Development Build](#development-build)
  - [Auto Build (Using Docker Compose)](#auto-build-using-docker-compose)
  - [Manual Build](#manual-build)
    - [Build Windows Binary](#build-windows-binary)
    - [Run Web Application](#run-web-application)

## Auto Build (Using Docker Compose)

- Build the docker image
```bash
docker buildx build -f Dockerfile.dev -t biltudas1/oneclickinstall:latest .
```
- [Now create `docker-compose.yml` at the root of repository and then configure it accordingly.](DOCKER.md)
- Run the Docker Compose using:
```bash
docker compose up
```

## Manual Build

### Build Windows Binary

- Make sure [Qt](BUILD.md#windows-application) is Installed
- Configure CMake using the following Command
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```
- Now Build the binary
```bash
cmake --build build
```
> Note: CMake and Ninja comes with Qt if you install the Qt in Official way

### Run Web Application

- Install [Python](https://www.python.org/) and [Poetry](https://python-poetry.org/)
- Install all the packages
```bash
poetry install
```
- Run the web server
```bash
poetry run debug
```