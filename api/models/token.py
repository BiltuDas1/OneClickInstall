import pydantic


class Token(pydantic.BaseModel):
  tokenID: str
class createToken(pydantic.BaseModel):
  appIds: list[int] | None = None