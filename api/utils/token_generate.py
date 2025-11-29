import secrets

def generate_token(bytes_length=32) -> str:
    return secrets.token_hex(bytes_length)   # 32 bytes → 64 hex characters