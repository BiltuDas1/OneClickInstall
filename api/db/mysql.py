from sqlalchemy import create_engine, text, Connection
from typing import Any, List, Dict, Optional
import threading
from . import error

class MySQL:
  """
  Class represents the MySQL Object
  """
  _engine = None
  _engine_lock = threading.Lock()

  def __init__(self, uri: str | None = None):
    if MySQL._engine is None:
      if uri is None:
        raise ValueError("Database not initialized! You must pass a URI for the first connection.")
        
      with MySQL._engine_lock:
        if MySQL._engine is None:
          MySQL._engine = create_engine(uri, pool_pre_ping=True)
    
    self.__engine = MySQL._engine
    self.__conn = None

  def Connect(self):
    """
    Connects to the MySQL
    """
    try:
      if self.__conn is not None and not self.__conn.closed and not self.__conn.invalidated:
        return

      if self.__conn is not None:
        try:
          self.__conn.close()
        except:
          pass

      self.__conn = self.__engine.connect()
    except Exception as e:
      raise error.DatabaseError("Failed to establish connection. " + str(e))

  def Disconnect(self):
    """
    Disconnect the MySQL connection
    """
    if self.__conn is None:
      return
    self.__conn.close()
    self.__conn = None

  def Query(self, query: str, params: Optional[Dict[str, Any]] = None) -> List[Dict[str, Any]]:
    """
    Executes the Query and returns the output in a 2D List
    
    :param query: The Query to execute
    :type query: str
    :param params: The parameters which will be passed to the query
    :type params: Optional[Dict[str, Any]]

    Example:  
    ```python
    result = MySQL.Query(
      "SELECT FirstName, LastName FROM clients WHERE ID < :user_id", 
      {"user_id": 3}
    )
    print(result) # [{'FirstName': 'Gord', 'LastName': 'Thompson'}, {'FirstName': 'Bob', 'LastName': 'Loblaw'}]
    ```
    """
    self.Connect()

    if self.__conn is None:
      raise ConnectionError("Database auto connection failed")

    if params is None:
      params = {}
    
    try:
      result = self.__conn.execute(text(query), params)
      
      if result.returns_rows:
        return [dict(row) for row in result.mappings()]
      
      return []
    except Exception as e:
      print(f"Query Failed: {e}")
      raise e
  
  def Save(self):
    """
    Commit the changes to Remote side
    """
    if self.__conn is None:
      return
    if self.__conn is None or self.__conn.closed:
      return
    try:
      self.__conn.commit()
    except Exception as e:
      raise error.DatabaseError("Failed to commit. " + str(e))

  def Discard(self):
    """
    Rollback the write changes
    """
    if self.__conn is None:
      return
    if self.__conn is None or self.__conn.closed:
      return
    try:
      self.__conn.rollback()
    except Exception as e:
      raise error.DatabaseError("Failed to rollback. " + str(e))

  # For using it using With Statment
  def __enter__(self):
    self.Connect()
    return self

  # Auto Rollback on error, and Auto commit on Save
  def __exit__(self, exc_type, exc_val, exc_tb):
    if exc_type:
      self.Discard()
    else:
      self.Save()
    self.Disconnect()