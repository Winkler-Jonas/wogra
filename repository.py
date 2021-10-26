from collections import defaultdict

import regex as re
from datetime import datetime
from typing import List, Dict


VDATE_PATTERN = re.compile("""
                    (?P<date>
                        \d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}
                    )
                    \s\+.*?update.*?to\s
                    (?P<new_ver>
                        [v\.\d\w]+
                    )
                    """, re.M | re.S | re.VERBOSE)


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

        def list_duplicates(seq):
            tally = defaultdict(list)
            for i, item in enumerate(seq):
                tally[item].append(i)
            return ((key, locs) for key, locs in tally.items()
                    if len(locs) > 1)
        index_list: List[str] = []
        # todo somehow there are more matches then lines...
        # fix asap, also can't delete by index due to multiprocessing
        for commit in new_commit_lst:
            if match := VDATE_PATTERN.search(commit):
                index_list.append(match.group('new_ver'))
        for key, lst_idx in list_duplicates(index_list):
            for idx in lst_idx[1:]:
                del new_commit_lst[idx]
        self.__commits = new_commit_lst

    def get_pretty_commits(self) -> Dict[datetime, str]:
        ret_dict: Dict[datetime, str] = {}
        for commit in self.__commits:
            if match := VDATE_PATTERN.search(commit):
                date_str, version_str = match.group('date', 'new_ver')
                date: datetime = datetime.strptime(date_str, '%Y-%m-%d %H:%M:%S')
                ret_dict[date] = version_str
        return ret_dict

    def __str__(self) -> str:
        return f"ID: {self.__repo_id}\tNAME: {self.__name}\tDATE_EDITED: {self.__date_edited}"
