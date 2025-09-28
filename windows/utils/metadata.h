#ifndef METADATA_H
#define METADATA_H

#include "toml/toml.hpp"
#include <string>
#include <QFile>
#include <QByteArray>
#include <string_view>
#include <map>

// Stores the tokenID of the Application
inline volatile const char tokenID[64] = "/*_MANDATORY_STEP_POPULATE_THIS_FIELD_VIA_SCRIPT_BEFORE_RUN_ */";

// Stores MetaDatas about the Project, from pyproject.toml
class MetaData {
  private:
    toml::table metadatas;
    std::string baseUrl;
    std::string apiVersion;
    std::string repositoryUrl;
    std::map<std::string, std::string> endpoints;

    // Loads the api settings
    void loadAPISettings() {
      auto tool = metadatas["tool"].as_table();
      auto oneclickinstall = (*tool)["oneclickinstall"].as_table();
      auto api = (*oneclickinstall)["api"].as_table();
      auto endpointsArr = (*api)["endpoints"].as_array();

      if ((!tool) || (!oneclickinstall) || (!api) || (!endpointsArr)) {
        throw std::runtime_error("Missing [tool.oneclickinstall.api.endpoints]");
      }

      // Loading repository url
      if (auto project = metadatas["project"].as_table()) {
        if (auto urls = (*project)["urls"].as_table()) {
          this->repositoryUrl = (*urls)["repository"].value_or<std::string>("");
        }
      }

      // Loading base_url
      this->baseUrl = (*api)["base_url"].value_or<std::string>("");

      // Loading api_version
      int version = (*api)["api_version"].value_or<int>(1);
      this->apiVersion = "v" + std::to_string(version);

      // Loading Endpoints
      for (auto&& ep : *endpointsArr) {
        auto tbl = ep.as_table();

        if (!tbl) {
          continue;
        }

        for (auto&& [key, val] : *tbl) {
          if (auto v = val.value<std::string>()) {
            this->endpoints.emplace(key.str(), *v);
          }
        }
      }

    }

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

      // Storing endpoints
      this->loadAPISettings();
    }

    // Returns the Repository URL
    std::string getRepository() const {
      return this->repositoryUrl;
    }

    // Returns the tokenID of the Current Application
    std::string getTokenID() const {
      return const_cast<const char*>(tokenID);
    }

    // Returns the base url
    std::string getBaseUrl() const {
      return this->baseUrl;
    }

    // Returns the api Version
    int getApiVersion() const {
      if (apiVersion.size() > 1 && apiVersion[0] == 'v') {
        return std::stoi(apiVersion.substr(1));
      }
      return 0;
    }

    // Returns the endpoint where the apps list will come
    std::string getAppsEndpoint() {
      return this->baseUrl + "/" + this->apiVersion + this->endpoints["listApps"];
    }

    // Returns the endpoint where the scripts and executable of the application will resides
    std::string getDownloadLinksEndpoint() {
      return this->baseUrl + "/" + this->apiVersion + this->endpoints["getDownloadLinks"];
    }
};

#endif // METADATA_H