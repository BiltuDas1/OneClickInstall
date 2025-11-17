import sys

# The placeholder used in the C++ code
PLACEHOLDER = b"/*__PLACEHOLDER_TOKEN_MUST_BE_REPLACED_BY_SERVER_SCRIPT_64_B__*/"
BUFFER_SIZE = 64

def patch_executable(data: bytearray, new_token: str) -> str | bytearray:
  """
  Finds the placeholder in the executable and replaces it with the new token.
  """
  new_token_bytes = new_token.encode('utf-8')
  if len(new_token_bytes) >= BUFFER_SIZE:
    return f"New token is too long. Maximum length is {BUFFER_SIZE - 1} characters."

  # Pad the new token with null bytes to match the buffer size
  padded_token = new_token_bytes.ljust(BUFFER_SIZE, b'\0')

  try:
    content = bytearray(data)
    offset = content.find(PLACEHOLDER) # Find the offset of the placeholder
    if offset == -1:
      return "Placeholder not found in the executable."

    # Replace the placeholder with the new padded token
    content[offset:offset + BUFFER_SIZE] = padded_token
    return content
  
  except IOError as e:
    return str(e)
