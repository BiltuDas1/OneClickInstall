import logging

# ANSI color codes
RESET = "\033[0m"
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
CYAN = "\033[36m"
DEEP_SKY_BLUE = "\033[38;5;39m"

class ColorFormatter(logging.Formatter):
  LEVEL_COLORS = {
    "DEBUG": CYAN,
    "INFO": DEEP_SKY_BLUE,
    "WARNING": YELLOW,
    "ERROR": RED,
    "CRITICAL": RED,
  }

  def format(self, record):
    color = self.LEVEL_COLORS.get(record.levelname, RESET)
    record.levelname = f"{color}[{record.levelname}]{RESET}"
    return super().format(record)
