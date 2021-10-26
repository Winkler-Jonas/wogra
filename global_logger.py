import os
import logging
from pathlib import Path


def setup_logger(name):
    formatter = logging.Formatter(fmt='%(asctime)s - %(levelname)s - %(module)s - %(message)s',
                                  datefmt='%d-%b-%y %H:%M:%S')
    handler = logging.FileHandler(f'{Path(os.path.dirname(os.path.realpath(__file__))) / "logs" / "crash.log"}', "w")
    handler.setFormatter(formatter)
    logger = logging.getLogger(name)
    logger.setLevel(logging.WARNING)
    logger.addHandler(handler)
    return logger
