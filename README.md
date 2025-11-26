# OneClickInstall

- [OneClickInstall](#oneclickinstall)
  - [Building From Source](#building-from-source)
  - [Development Build](#development-build)
  - [Environments](#environments)
    - [Production Environment](#production-environment)
    - [Development Environment](#development-environment)

## Building From Source
Check [docs/BUILD.md](docs/BUILD.md) for more information

## Development Build
Check [docs/DEVBUILD.md](docs/DEVBUILD.md) for more information

## Environments

### Production Environment
| Environment Name | Description                                                                                                                   |
| ---------------- | ----------------------------------------------------------------------------------------------------------------------------- |
| `MYSQL_URI`      | Sets the MySQL [Database Connection URI](https://dev.mysql.com/doc/connector-j/en/connector-j-reference-jdbc-url-format.html) |

### Development Environment
This environment supports all the environment of the production environment and the following environments:
| Environment Name     | Description                                                                                          |
| -------------------- | ---------------------------------------------------------------------------------------------------- |
| `DEBUG`              | Enables the Webserver with Debugging Mode On                                                         |
| `COMPILE_ONLY`       | Set this if you want to compile the Executable                                                       |
| `RELEASE_BUILD_ONLY` | Set this if you want to compile the Executable as Release. Make sure that `COMPILE_ONLY` is also set |