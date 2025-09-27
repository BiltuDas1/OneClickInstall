#ifndef METADATA_H
#define METADATA_H

#include "toml/toml.hpp"
#include <string>
#include <QFile>
#include <QByteArray>
#include <string_view>

// Stores MetaDatas about the Project, from pyproject.toml
class MetaData {
  private:
    toml::table metadatas;

  public:
    explicit MetaData() {
      QFile tomlFile(":/pyproject.toml");

      // Throw an exception if the resource can't be found or opened
      if (!tomlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Fatal: Could not open :/pyproject.toml from resources.");
      }

      QByteArray fileContent = tomlFile.readAll();

      try {
        metadatas = toml::parse(fileContent.toStdString(), std::string_view{"pyproject.toml"});
      } catch (const toml::parse_error& err) {
        throw std::runtime_error("Error parsing pyproject.toml: " + std::string(err.description()));
      }
    }

    // Returns the Repository URL
    std::string getRepository() {
      // Navigate safely: check if each nested table exists before accessing it.
      if (auto project = metadatas["project"].as_table()) {
        if (auto urls = (*project)["urls"].as_table()) {
          return (*urls)["repository"].value_or<std::string>("");
        }
      }

      return "";
    }
};

#endif // METADATA_H