import pydantic


class Apps(pydantic.BaseModel):
  name: str
  appid: str
