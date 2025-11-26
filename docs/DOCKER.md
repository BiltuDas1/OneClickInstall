# Configure `docker-compose.yml`

If you want to run the Application then use the following `docker-compose.yml`
```yml
services:
  oneclickinstall:
    image: biltudas1/oneclickinstall:latest
    container_name: oneclickinstall
    environment:
      DEBUG: yes
    volumes:
      - ".:/mnt" # Mounts the current directory inside Docker Image
    ports:
      - "8000:8000" # Expose 8000 Port
```

For Auto compile only just set the Environment `COMPILE_ONLY`:
```yml
services:
  oneclickinstall:
    image: biltudas1/oneclickinstall:latest
    container_name: oneclickinstall
    environment:
      COMPILE_ONLY: yes
      DEBUG: yes
    volumes:
      - ".:/mnt"
    ports:
      - "8000:8000"
```

For more environments check [README.md](/README.md#environments)