import pydantic


class Token(pydantic.BaseModel):
  tokenID: str