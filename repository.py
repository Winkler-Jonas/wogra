from datetime import datetime
from typing import List


class Repository:

    def __init__(self, repo_id: str, name: str, url: str, date_edited: datetime):
        self.__repo_id: str = repo_id
        self.__url: str = url
        self.__name: str = name
        self.__date_edited: datetime = date_edited
        self.__commits: List[str] = []

    @property
    def repo_id(self) -> str:
        return self.__repo_id

    @property
    def url(self) -> str:
        return self.__url

    @property
    def name(self) -> str:
        return self.__name

    @property
    def date_edited(self) -> datetime:
        return self.__date_edited

    @property
    def commits(self) -> List[str]:
        return self.__commits

    @commits.setter
    def commits(self, new_commit_lst: List[str]):
        self.__commits = new_commit_lst

    def __str__(self) -> str:
        return f"ID: {self.__repo_id}\tNAME: {self.__name}\tDATE_EDITED: {self.__date_edited}\tCOMMITS: {''.join(self.__commits)}"
