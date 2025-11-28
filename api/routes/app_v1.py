from api.utils.token_generate import generate_token
import fastapi
from ..models import token
from ..db import mysql, error
from fastapi.responses import JSONResponse, FileResponse
import os
from starlette.background import BackgroundTask

def transform_apps(raw_items, tokenID):
  grouped = {}

  for item in raw_items:
      app_id = item["id"]

      if app_id not in grouped:
          grouped[app_id] = {
              "name": item["app_name"],
              "main": item["main"],
              "executables": [],
              "scripts": []
          }

      # Add executable info
      grouped[app_id]["executables"].append({
          "filename": item["exe_name"],
          "url": item["exe_url"],
          "size": item["exe_size"]
      })

      # Add script info (using your /v1/script/{id} endpoint)
      grouped[app_id]["scripts"].append({
          "filename": item["main"],
          "url": f"http://localhost:8000/v1/script/{app_id}",
          "size": len(item["main"])  # OPTIONAL: replace with real script size
      })

  return {
      "status": True,
      "tokenID": tokenID,
      "result": list(grouped.values())
  }


def createAppsAPI(app: fastapi.FastAPI):
  @app.post("/v1/fetch")
  async def fetch_apps(request: token.Token):
    """
    Gets the list of applications allocated to a specific tokenID
    """

    resp = {
        "status": False,
        "status_code": 400,
        "tokenID": request.tokenID,
        "result": [],
        "message": "Failed to fetch apps",
        "error": {"message": "Invalid tokenID"}
    }

    try:
      if request.tokenID is None or request.tokenID.strip() == "":
        raise ValueError("tokenID is required")
      with mysql.MySQL() as DB:
        query = """
            SELECT 
                a.id,
                CONCAT(a.id, '.cmd') AS main,
                a.app_name,
                spt.exe_name,
                spt.exe_url,
                spt.exe_size
            FROM apps a
            INNER JOIN scripts spt 
                ON spt.app_id = a.id
            INNER JOIN user_apps ua 
                ON ua.app_id = a.id
            INNER JOIN user_tokens ut 
                ON ut.id = ua.users_id
            WHERE ut.token = :token;
        """

        params = {"token": request.tokenID}

        result = DB.Query(query, params)
        print(result)

        # Process result
        if result and len(result) > 0:
          transformed = transform_apps(result, request.tokenID)
          return JSONResponse(status_code=200,content=transformed)
        else:
          resp['status_code'] = 404
          resp["message"] = "No apps available for this token"
          resp["error"] = {"message": "No apps found"}
    except error.DatabaseError:
      resp['status_code'] = 500
      resp["status"] = False
      resp["message"] = "Internal Server Error"
      resp["error"] = {"message": "Database Error"}
    except Exception as e:
      resp['status_code'] = 400
      resp["status"] = False
      resp["message"] = "Bad Request"
      resp["error"] = {"message": str(e)}
    return JSONResponse(status_code=resp['status_code'],content=resp)

  @app.post("/v1/createToken")
  async def create_token(request: token.createToken):
    """
    Create a new token and associate it with provided app IDs"""
    def make_response(status, status_code, message, token=None, download_url=None, error_msg=None):
      return JSONResponse(
        status_code=status_code,
        content={
          "status": status,
          "status_code": status_code,
          "message": message,
          "tokenID": token,
          "downloadUrl": download_url,
          "error": {"message": error_msg} if error_msg else None
        }
      )

    try:
      #Validate Required Inputs
      if not request or request.appIds is None:
        return make_response(
          False, 400, "Invalid Request", error_msg="appIds is required"
        )
      
      #Generate Token
      new_token = generate_token(32)   # 64-char token

      with mysql.MySQL() as DB:
        # Insert token
        token_query = "INSERT INTO user_tokens (token) VALUES (:token)"
        DB.Query(token_query, {"token": new_token})

        #Fetch inserted token ID (faster than subquery each time)
        fetch_id = "SELECT id FROM user_tokens WHERE token = :token LIMIT 1"
        token_row = DB.Query(fetch_id, {"token": new_token})
        if not token_row:
          return make_response(False, 500, "Unable to fetch token ID", error_msg="Token insert failed")
        
        token_id = token_row[0]["id"]

        #Insert multiple app mappings
        if request.appIds:
          link_query = "INSERT INTO user_apps (users_id, app_id) VALUES (:uid, :app_id)"
          for app_id in request.appIds:
            DB.Query(link_query, {"uid": token_id, "app_id": app_id})

      return make_response(
        True,
        201,
        "Token created successfully",
        token=new_token,
        download_url=f"http://localhost:8000/download/{new_token}"
      )
    except error.DatabaseError as e:
      return make_response(
        False, 500, "Database Error", error_msg=str(e)
      )

    except ValueError as e:
      return make_response(
        False, 400, "Bad Request", error_msg=str(e)
      )

    except Exception as e:
      return make_response(
        False, 500, "Internal Server Error", error_msg=str(e)
      )

  @app.get("/v1/script/{scriptID}")
  async def download_script(scriptID: int):
    """
    Serve scripts so that clients can download it
    """
    
    resp = {
      "status": False,
      "status_code": 500,
      "message": "Failed to fetch script",
      "script": {},
      "error": {}
    }

    print(f"Requested ScriptID: {scriptID}")
    try:
      with mysql.MySQL() as DB:
        query = "SELECT script_txt FROM scripts WHERE app_id = :id"
        result = DB.Query(query, {"id": scriptID})
        
        if not result or len(result) != 1:
          resp["status_code"] = 404
          resp["status"] = False
          resp["script"]= None
          resp["message"]= "Script Not Found"
          resp["error"]['message'] = "Script Not Found"
        else:
          script_text = result[0]["script_txt"]
          filename = f"{scriptID}.cmd"
          file_path = f"/tmp/{filename}"

          # Create CMD file in /tmp directory
          with open(file_path, "w", encoding="utf-8") as f:
              f.write(script_text)
          # Background task: delete temp file after response is sent
          task = BackgroundTask(lambda: os.remove(file_path))

          # # Serve file as download
          return FileResponse(
            path=file_path,
            filename=filename,
            media_type="text/plain"
          )
    except error.DatabaseError:
      resp["status_code"] = 500
      resp["status"] = False
      resp["script"]= None
      resp["message"]= "Database Error"
      resp["error"]['message'] = "Database Error"
    return resp
  
  @app.get("/v1/apps")
  async def list_apps():
    """
    List all available applications
    """
    resp = {
      "status": False,
      "status_code": 500,
      "message": "Failed to fetch apps",
      "result": [],
      "error": None
    }

    try:
      with mysql.MySQL() as DB:
        query = "SELECT apps.id, apps.app_name, apps.category_id, apps.icon_id, apps.color, apps.description,ac.cat_name as category  FROM apps INNER JOIN app_categories ac ON ac.id =  apps.category_id where apps.is_active = 1"
        result = DB.Query(query)
        
      if result and len(result) > 0:
        resp["status"] = True
        resp['status_code'] = 200
        resp["result"] = result
        resp["message"] = "Apps fetched successfully"
        resp["error"] = None
      else:
        resp["status"] = False
        resp['status_code'] = 404
        resp["result"] = []
        resp["message"] = "No apps available"
        resp["error"] = None
    except error.DatabaseError:
      resp["status"] = False
      resp['status_code'] = 500
      resp["result"] = []
      resp["message"] = "Internal Server Error"
      resp["error"] = "Database Error"
    
    return JSONResponse(status_code=resp['status_code'],content=resp)
  
  @app.get("/v1/categories")
  async def list_categories():
    """
    List all available application categories
    """
    resp = {
      "status": False,
      "status_code": 500,
      "message": "Failed to fetch categories",
      "result": [],
      "error": None
    }

    try:
      with mysql.MySQL() as DB:
        query = "SELECT id, cat_name FROM app_categories"
        result = DB.Query(query)
        
      if result and len(result) > 0:
        resp["status"] = True
        resp['status_code'] = 200
        resp["result"] = result
        resp["message"] = "Apps fetched successfully"
        resp["error"] = None
      else:
        resp["status"] = False
        resp['status_code'] = 404
        resp["result"] = []
        resp["message"] = "No apps available"
        resp["error"] = None
    except error.DatabaseError:
      resp["status"] = False
      resp['status_code'] = 500
      resp["result"] = []
      resp["message"] = "Internal Server Error"
      resp["error"] = "Database Error"
    
    return JSONResponse(status_code=resp['status_code'],content=resp)
  