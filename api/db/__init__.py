from . import mysql, error
import os

if (URI := os.getenv("MYSQL_URI")) is not None:
  URI = URI.replace("mysql://", "mysql+pymysql://")
  mysql.MySQL(URI)
else:
  raise error.DatabaseError("MYSQL_URI environment is not set")
