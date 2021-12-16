from urllib.parse import urlencode, urlunparse
from utils.parse_pattern import SearchPattern
from multiprocessing import Process, Queue
from repository import Repository
from datetime import datetime
import multiprocessing as mp
from itertools import repeat
from regex import Pattern
from typing import List, Dict, Optional, Callable
from math import ceil
import requests
import inspect
import ast
import os
import time

url_hsa: str = 'https://r-n-d.informatik.hs-augsburg.de:8080/'
p_token_hsa: str = 'bsrg4w9xqTxotvxzVLGD'
url_wogra: str = 'https://gitlab.wogra.com'
p_token_wogra: str = 'Lq1z1hMxG_yKeyTLaAXD'

CPU_COUNT: int = mp.cpu_count()

protocol: str = 'https'
netloc_hs: str = 'r-n-d.informatik.hs-augsburg.de:8080/'


class LabScrap:

    def __init__(self, schema: str, netloc: str, rest_hook: str, auth_token: str):
        self.__schema: str = schema
        self.__netloc: str = netloc
        self.__rest_hook: str = rest_hook
        self.__auth_token: str = auth_token
        self.__project_info: List[str] = self.__get_projects_string()

        project_pattern: List[Pattern] = SearchPattern.proj_patterns()

        split_project_info = [self.__project_info[i:i + int(ceil((len(self.__project_info) / CPU_COUNT)))] for i in
                              range(0, len(self.__project_info), int(ceil((len(self.__project_info) / CPU_COUNT))))]
        queue = Queue()
        processes = [Process(target=self.__clean_proj_info,
                             args=(str_list, queue, project_pattern)) for str_list in split_project_info]

        # Regex: 0.11 - 0.04288 - 0.106353 - 0.040888
        # Normal: 0.195449 - 0.26670885 - 0.18945121 - 0.1880488395690918 - 0.2531683444
        start_time = time.time()
        results: List[Repository] = []
        for p in processes:
            p.start()
        for p in processes:
            results.extend(queue.get())
            p.join()
        end_time = time.time()
        print(f'Multiprocess normal: {end_time-start_time}')



        # 0.30947 - 0.33707 - 0.561203 - 0.339907 - 0.3208727
        # start = time.time()
        # self.__clean_proj_info(self.__project_info)
        # end = time.time()
        # print(f'It took about: {end-start}')


    def __clean_proj_info(self, string_lst: List[str], queue: Optional[Queue] = None, pattern_lst: Optional[List[Pattern]] = None) -> List[Repository]:
        """
        Collects all relevant data from project strings

        :param string_lst: List of strings containing project information (must be gitlab-json format)
        :type string_lst: List[str]
        :param pattern_lst:
        :type pattern_lst:
        :param queue:
        :type queue:
        :return:
        :rtype:
        """
        # project_lst: List[Repository] = []
        # for string in string_lst:
        #     project_dict: Dict[str, str] = ast.literal_eval(string)
        #     project_lst.append(Repository(id=project_dict.get('id'),
        #                                   name=project_dict.get('name'),
        #                                   url=project_dict.get('ssh_url_to_repo'),
        #                                   date_edited=datetime.strptime(project_dict.get('last_activity_at'),
        #                                                                 '%Y-%m-%dT%H:%M:%S.%f%z')))
        # if queue:
        #     queue.put(project_lst)
        # return project_lst

        #get_key: Callable[[List[str], str], str] = lambda key_list, key_part: [key_list[idx] for idx, ele in enumerate(key_list) if key_part.lower() in ele.lower()][0]

        def __get_key(key_list: List[str], key_part) -> Optional[str]:
            """Function retrieves key from a dictionary that has a keyword similar to the parma key_part
               Example: The key is 'Example_string' and the key_part provided is 'str', this func returns
               the actual key 'Example_string'.

               If Multiple keys match the key_part, the first hit is returned
            """
            for idx, ele in enumerate(key_list):
                if key_part.lower() in ele.lower():
                    return key_list[idx]
            else:
                return None
        repo_lst: List[Repository] = []
        for string in string_lst:
            tmp_param_lst: Dict[str, str] = {}
            for pattern in pattern_lst:
                if match := pattern.search(string):
                    tmp_param_lst.update(match.groupdict())
            param_lst: Dict[str, str] = {}
            for key_part in inspect.signature(Repository.__init__).parameters:
                tmp_key = __get_key(key_list=list(tmp_param_lst.keys()), key_part=key_part)

                param_lst[key_part] = tmp_param_lst.get(tmp_key)
            repo_lst.append(Repository(id=param_lst.get('id'),
                                       name=param_lst.get('name'),
                                       url=param_lst.get('url'),
                                       date_edited=datetime.strptime(param_lst.get('date_edited'), '%Y-%m-%d')))
        queue.put(repo_lst)

    def __get_projects_string(self):
        """
        Scraps all project information of a gitlab-server

        """
        idx: int = 0
        projects: List[str] = []
        while True:
            query: str = urlencode({'per_page': f'100', 'page': f'{idx}'})
            url: str = urlunparse((protocol, netloc_hs, f'{self.__rest_hook}projects/', '', query, ''))
            header = {'PRIVATE-TOKEN': f'{p_token_hsa}'}
            response = requests.get(url, headers=header)
            data: List[str] = [str(line) for line in response.json()]
            if not data:
                break
            projects.extend(data)
            idx += 1
        return projects

    # Time format: YYYY-MM-DDTHH:MM:SSZ
    def __get_commit_messages(self, project_id: int = 11, since: datetime = None):
        query: str = urlencode({'all': 'True'})
        path: str = f'api/v4/projects/{project_id}/repository/commits'
        url: str = urlunparse((protocol, netloc_hs, path, '', query, ''))
        header = {'PRIVATE-TOKEN': f'{p_token_hsa}'}
        response = requests.get(url, headers=header)
        return response.json()


if __name__ == '__main__':
    LabScrap('https', netloc_hs, 'api/v4/', p_token_hsa)
