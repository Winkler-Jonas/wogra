#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "def.h"

#define ERROR_CODE 0

#define CHUNK_SIZE 2
#define raise(err_msg) error_new(err_msg, ERROR_CODE)

#define START_PYTHON_CODE //
#define END_PYTHON_CODE //

#define __python_init__(void) Py_Initialize()
#define __python_des__(void) Py_Finalize()
#define __python_code_start__(state) ({*state=PyGILState_Ensure();})
#define __python_code_end__(state) ({PyGILState_Release(*state);})


typedef struct {
    StringArray *input;
    StringArray *search_pattern;
    List        *output;
    Error       *error;
} ThreadData;

static const char *patterns[] = {"'id':[[:space:]]([[:digit:]]+)",
                                 "'name':[[:space:]]'([[:space:][:digit:][:alpha:]-]*)'",
                                 "ssh_url_to_repo':[[:space:]]'(.*)',[[:space:]]'http_url_to_repo",
                                 "last_activity_at':[[:space:]]'([0-9T:.-]*)"};

static const char *test_string[5] = {
        "#  {'id': 332, 'description': '', 'name': 'Project 332', 'name_with_namespace': 'Andreas Drexel / canvert2015', 'path': 'canvert2015', 'path_with_namespace': 'andreas.drexel/canvert2015', 'created_at': '2015-04-03T09:49:05.000Z', 'default_branch': 'master', 'tag_list': [], 'topics': [], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/andreas.drexel/canvert2015.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/andreas.drexel/canvert2015.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/andreas.drexel/canvert2015', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2015-07-22T16:17:09.000Z', 'namespace': {'id': 77, 'name': 'Andreas Drexel', 'path': 'andreas.drexel', 'kind': 'user', 'full_path': 'andreas.drexel', 'parent_id': None, 'avatar_url': 'https://seccdn.libravatar.org/avatar/6f37c1ba244e25e821cb3596e66e806e?s=80&d=identicon', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/andreas.drexel'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 69, 'name': 'Andreas Drexel', 'username': 'andreas.drexel', 'state': 'active', 'avatar_url': 'https://seccdn.libravatar.org/avatar/6f37c1ba244e25e821cb3596e66e806e?s=80&d=identicon', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/andreas.drexel'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': True, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'enabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 69, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
        "#  {'id': 365, 'description': 'Description Project 365', 'name': 'Project 365', 'name_with_namespace': 'Michael Brandl / pi-lfs', 'path': 'pi-lfs', 'path_with_namespace': 'michael.brandl/pi-lfs', 'created_at': '2015-04-17T12:22:08.000Z', 'default_branch': 'master', 'tag_list': ['LFS', 'RPi'], 'topics': ['LFS', 'RPi'], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/michael.brandl/pi-lfs.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2021-04-06T17:05:22.864Z', 'namespace': {'id': 122, 'name': 'Michael Brandl', 'path': 'michael.brandl', 'kind': 'user', 'full_path': 'michael.brandl', 'parent_id': None, 'avatar_url': '/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 111, 'name': 'Michael Brandl', 'username': 'michael.brandl', 'state': 'active', 'avatar_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': False, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'disabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 111, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
        "#  {'id': 150, 'description': 'Description Project 150', 'name': 'Project 150', 'name_with_namespace': 'Michael Brandl / pi-lfs', 'path': 'pi-lfs', 'path_with_namespace': 'michael.brandl/pi-lfs', 'created_at': '2015-04-17T12:22:08.000Z', 'default_branch': 'master', 'tag_list': ['LFS', 'RPi'], 'topics': ['LFS', 'RPi'], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/michael.brandl/pi-lfs.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2021-04-06T17:05:22.864Z', 'namespace': {'id': 122, 'name': 'Michael Brandl', 'path': 'michael.brandl', 'kind': 'user', 'full_path': 'michael.brandl', 'parent_id': None, 'avatar_url': '/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 111, 'name': 'Michael Brandl', 'username': 'michael.brandl', 'state': 'active', 'avatar_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': False, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'disabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 111, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
        "#  {'id': 1, 'description': 'Description Project 1', 'name': 'Project 1', 'name_with_namespace': 'Michael Brandl / pi-lfs', 'path': 'pi-lfs', 'path_with_namespace': 'michael.brandl/pi-lfs', 'created_at': '2015-04-17T12:22:08.000Z', 'default_branch': 'master', 'tag_list': ['LFS', 'RPi'], 'topics': ['LFS', 'RPi'], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/michael.brandl/pi-lfs.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2021-04-06T17:05:22.864Z', 'namespace': {'id': 122, 'name': 'Michael Brandl', 'path': 'michael.brandl', 'kind': 'user', 'full_path': 'michael.brandl', 'parent_id': None, 'avatar_url': '/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 111, 'name': 'Michael Brandl', 'username': 'michael.brandl', 'state': 'active', 'avatar_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': False, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'disabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 111, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
        "#  {'id': 512, 'description': 'Description Project 512', 'name': 'Project 512', 'name_with_namespace': 'Michael Brandl / pi-lfs', 'path': 'pi-lfs', 'path_with_namespace': 'michael.brandl/pi-lfs', 'created_at': '2015-04-17T12:22:08.000Z', 'default_branch': 'master', 'tag_list': ['LFS', 'RPi'], 'topics': ['LFS', 'RPi'], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/michael.brandl/pi-lfs.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2021-04-06T17:05:22.864Z', 'namespace': {'id': 122, 'name': 'Michael Brandl', 'path': 'michael.brandl', 'kind': 'user', 'full_path': 'michael.brandl', 'parent_id': None, 'avatar_url': '/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 111, 'name': 'Michael Brandl', 'username': 'michael.brandl', 'state': 'active', 'avatar_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': False, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'disabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 111, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
};

static const char *test_wa[5] = {
        "Erstes Element",
        "Zweites Element",
        "Drittes Element",
        "Viertes Element",
        "Fünftes Element"
};


PyObject *crw_get_info(PyObject *self, PyObject *args);

void *strArrToPyLst(StringArray *stringArray, PyObject *pyList);

str *get_info(str string, size_t amount_pattern);

void *pyLstToStrArr(PyObject *list, StringArray **stringArray);

void *mp_search(void *args);

void *__prep_regex_pattern(StringArray *patternArray, Regex **re_patterns);

void *receiving_function(PyObject *stringList, PyObject *patternList);

int main(void) {
    //crw_get_info(NULL, NULL);

    START_PYTHON_CODE
    __python_init__();
    PyGILState_STATE *gil_state = malloc(sizeof(PyGILState_STATE));
    __python_code_start__(gil_state);

    StringArray *stringArray = stringArray_new(5);
    PyObject *pyList = PyList_New((long) stringArray->max_length);
    Error *error;

    for (size_t i = 0; i < 5; i++) {
        if (NULL != (error = stringArray_add(stringArray, (str) test_string[i]))) {
            fprintf(stderr, "%s\n", error->err_msg);
            exit(-1);
        }
    }

    StringArray *patternList = stringArray_new(4);
    PyObject *pyPattern = PyList_New((long) patternList->max_length);
    for (size_t i = 0; i < 4; i++) {
        if (NULL != (error = stringArray_add(patternList, (str) patterns[i]))) {
            fprintf(stderr, "%s\n", error->err_msg);
            exit(-1);
        }
    }
    // todo error handling missing
    strArrToPyLst(patternList, pyPattern);

    if (NULL != (error = strArrToPyLst(stringArray, pyList))) {
        fprintf(stderr, "%s", error->err_msg);
        exit(-1);
    } else {
        // PyList-Object was successfully created ... proceed!

        // pylist is argument for function coming from python code
        if (NULL != (error = receiving_function(pyList, pyPattern))) {
            fprintf(stderr, "%s", error->err_msg);
        }

//        StringArray *stringArray_one;
//        if(NULL != (error = pyLstToStrArr(pyList, &stringArray_one))){
//            fprintf(stderr, "%s\n", error->err_msg);
//        }else{
//            // PyList successfully converted to StringArray ... proceed!
//            stringArray_print(stringArray_one, stdout);
//            stringArray_del(stringArray_one);
//        }
    }

    // free allocated
    stringArray_del(stringArray);
    Py_CLEAR(pyList);

    END_PYTHON_CODE
    __python_code_end__(gil_state);
    __python_des__();
    if (error) {
        error_del(error);
    }
    return 0;
}

void *strArrToPyLst(StringArray *stringArray, PyObject *pyList) {
    /* Function: strArrToPyLst
     * ---------------------------
     * Function converts an array of strings to a python list
     *
     * str_arr: The array of strings to be converted
     *
     * returns: A python list object containing provided strings
     */
    if (!pyList) {
        return error_new("Provided PyList-Object not valid", ERROR_CODE);
    } else if (!stringArray) {
        return error_new("Provided StringArray not valid", ERROR_CODE);
    }
    for (long idx = 0; idx < stringArray->length; idx++) {
        PyList_SetItem(pyList, idx, PyBytes_FromString(stringArray->array[idx]));
    }
    return NULL;
}

void *pyLstToStrArr(PyObject *list, StringArray **stringArray) {
    /*Function: pyLstToStrArr
    * ---------------------------
    * Function converts a python list to an array of strings
    *
    * list: The list to be converted
    *
    * returns: Array of strings if successful, else NULL
    */
    register int len = PyObject_Length(list);
    Error *error;

    if (len <= 0 || NULL != (*stringArray = stringArray_new(len))) {
        for (size_t i = 0; i < len; i++) {
            PyObject *item;
            item = PyList_GetItem(list, (long) i);
            if (item) {
                if (NULL != (error = stringArray_add(*stringArray, PyBytes_AsString(item)))) {
                    return error;
                }
            }
        }
        return NULL;
    }
    return error_new("Error creating StringArray from Python List-Object", ERROR_CODE);
}

void *chunk_py_list(PyObject *list, List **param_list) {
    /*
     * Function: re..
     * ------------
     * Convert pylist-object to stringArray
     * Split stringArray into equal parts
     * create list containing parts
     * Deletes list reference
     */
    Error *error;
    StringArray *tmp_stringArray;
    if (NULL != (error = pyLstToStrArr(list, &tmp_stringArray))) {
        return error;
    }
    Py_CLEAR(list);
    *param_list = list_new();
    if (!*param_list) {
        return error_new("Error trying to create List-Object!", ERROR_CODE);
    }
    if (NULL != (error = stringArray_split(tmp_stringArray, CHUNK_SIZE, *param_list))) {
        return error;
    }
    stringArray_del(tmp_stringArray);
    return NULL;
}

void *__prep_regex_pattern(StringArray *patternArray, Regex **re_patterns) {
    /*
     * Function: __prep_regex_pattern
     * ----------------------------
     * Function turns provided patternList into global accessible List containing
     * provided patterns as posix-compiled pattern
     * PyObject is being destructed in process
     *
     * global PATTERNS is allocated!
     *
     * patternList:         List with POSIX-Regex pattern
     * type patternList:    &PyObject
     *
     * returns:             NULL if successful else Error
     * rtype:               Optional[NULL, Error]
     */
    for (size_t i = 0; i < patternArray->length; i++) {
        Regex *regex = regex_compile(patternArray->array[i]);
        if (regex == NULL) {
            return raise("Unknown Error occurred during compilation of pattern");
        }else if (regex->err){
            return raise(regex->err->err_msg);
        }
        re_patterns[i] = regex;
    }
    return NULL;
}

void *receiving_function(PyObject *stringList, PyObject *patternList) {
// todo Uncomment for actual program

//    START_PYTHON_CODE
//    __python_init__();
//    PyGILState_STATE *gil_state = malloc(sizeof(PyGILState_STATE));
//    __python_code_start__(gil_state);
    Error *error;
    if (stringList == NULL || patternList == NULL ||
        0 == PyObject_Length(patternList) || 0 == PyObject_Length(stringList)) {
        error = raise("Illegal Argument!\nList Arguments must not be NULL or emtpy!");
        goto finalize;
    }
    register size_t amount_pattern = PyObject_Length(patternList);

    StringArray *patternArray;
    if (NULL != (error = pyLstToStrArr(patternList, &patternArray))) {
        goto finalize;
    }

    List *chunked_list;

    if (NULL != (error = chunk_py_list(stringList, &chunked_list))) {
        raise(error->err_msg);
    }

    size_t      thread;
    pthread_t  *thread_handles;
    List       *thread_values = list_new(); //todo free
    size_t      amount_chunks = chunked_list->length;
    thread_handles = malloc(sizeof(pthread_t) * amount_chunks);

    // Init Mutext
    if(pthread_mutex_init(&mutex, NULL) != 0){
        // todo error occurred initializing mutex
    }

    ThreadData **args = malloc(amount_chunks * sizeof(ThreadData));

    for (size_t i = 0; i < amount_chunks; i++){
        ThreadData *arg = (ThreadData *) malloc(sizeof(ThreadData));
        arg->input = &chunked_list->list[i];
        arg->search_pattern = patternArray;
        arg->output = thread_values;
        args[i] = arg;
    }

    // Start threads
    for (thread = 0; thread < amount_chunks; thread++) {
        pthread_create(&thread_handles[thread], NULL, mp_search, (void *) args[thread]);
    }
    // Exit threads | retrieve collected data
    for (thread = 0; thread < amount_chunks; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    // Free up Thread memory
    free(thread_handles);
    pthread_mutex_destroy(&mutex);

    for (size_t i = 0; i < amount_chunks; i++){
        free(args[i]);
    }
    free(args);

    list_print(thread_values, stdout);

    finalize:
    if (patternArray) stringArray_del(patternArray);
    if (chunked_list) list_del(chunked_list);
    //if (thread_values) list_del(thread_values);

    return NULL;
}

void *mp_search(void *args) {
    /* Function: mp_search
     * -------------------
     * Execute regex-search on strings of provided input list
     *
     * args:        Structure containing two lists with stringArrays 1. Input 2. Return/Output
     * type args:   {&List, &List}
     *
     *
     */
    Error *error = NULL;
    ThreadData *thread_data = (ThreadData *) args;

    if (NULL == thread_data->input || NULL == thread_data->search_pattern){
        thread_data->error = raise("Illegal Argument! Parameter cannot be NULL!\n");
        goto KILL_THREAD;
    }
    if (NULL == thread_data->input){
        fprintf(stderr, "I'm just proceeding even though I should've stopped long ago...\n");
    }
    register size_t amount_pattern = thread_data->search_pattern->length;
    Regex **re_patterns = malloc(amount_pattern * sizeof(Regex*));
    if (NULL != (error = __prep_regex_pattern(thread_data->search_pattern, re_patterns))) {
        thread_data->error = raise(error->err_msg);
        goto KILL_THREAD;
    }
    if (NULL == thread_data->input){
        fprintf(stderr, "I'm just proceeding even though I should've stopped long ago... after __prep_regex\n");
    }
//    fprintf(stdout, "\nStarting search function\n");
//    stringArray_print(thread_data->input, stdout);
//    fprintf(stdout, "\nEnding search function\n");

    for(size_t i = 0; i < thread_data->input->length; i++){
        StringArray *new_stringArray = stringArray_new(amount_pattern);
        for(size_t x = 0; x < amount_pattern; x++){
            if (NULL == thread_data->input){
                fprintf(stderr, "I'm just proceeding even though I should've stopped long ago... right before it\n");
            }
            regex_search(re_patterns[x], thread_data->input->array[i]);
            if(re_patterns[x]->err){
                thread_data->error = raise(re_patterns[x]->err->err_msg);
                goto KILL_THREAD;
            }else{
                stringArray_add(new_stringArray, re_patterns[x]->match->group);
            }
            //fprintf(stdout, "Found String: %s\n", re_patterns[x]->match->group);
        }
        list_add_str_array(thread_data->output, new_stringArray);
    }

    for (size_t i = 0; i < amount_pattern; i++){
        regex_del(re_patterns[i]);
    }
    KILL_THREAD:
    pthread_exit(NULL);
}

/*
void *mp_search(void *args) {
     * Function: mp_search (Multiprocess-Search)
     * -------------------
     * Optimised function to regex_search multiple for multiple
     * pattern in optionally multiple strings
     *
     * strings: array of strings
     * chunk_size: The amount of strings provided
     *
     * raises: NULL if error occurred trying to match/compiling pattern
     * returns: Array of String arrays containing matches
     *
     *                                     [match_pattern_one]
     *          --> [abstract][pattern_one][match_pattern_two]
     *                                     [match_pattern_...]


    // Just for clarification (void cast)
    ThreadData *thread_data = (ThreadData *) args;

    for (size_t outer_idx = 0; outer_idx < thread_data->length; outer_idx++) {
        thread_data->result[outer_idx] = malloc(len(patterns) * sizeof(char*));
        for (size_t inner_idx = 0; inner_idx < len(patterns); inner_idx++) {
            Re *regex = regex_search(regex_compile(patterns[inner_idx]), thread_data->cur_str_ptr[outer_idx]);

            // If regex compilation NOT successful
            // -or- if pattern was not found in string
            // Concludes to ERROR !! -> cleanup -> exit
            if (regex->err_code) {
                write_regex_error(regex);
                fprintf(stderr, "ERROR: Regex failed state! -> %s\n"
                                "Pattern tried: %s\n"
                                "String: %s", regex->err_msg, regex->pattern, thread_data->cur_str_ptr[outer_idx]);
                free_regex(regex);
                thread_data = NULL;
                goto KILL_THREAD;
            }

            // If pattern was found

            thread_data->result[outer_idx][inner_idx] = "Hello";
            char *test = strdup(regex->match->group);
            //thread_data->result[outer_idx][inner_idx] = strdup(regex->match->group);
            printf("Inside Loop: %s\n", thread_data->result[outer_idx][inner_idx]);
            free_regex(regex);
        }
    }
    KILL_THREAD:
    pthread_exit(NULL);
}


PyObject *crw_get_info(PyObject *self, PyObject *args) {
     * Function: crw_get_info
     * ----------------------
     * Retrieve Python-List containing findings of regex_search-pattern in provided string
     *
     * string: The string to be searched
     *
     * returns: Python-List-Object
     *          NULL if error occurred



    START_PYTHON_CODE
    // Python-GIL status
    PyGILState_STATE *gil_state = malloc(sizeof(PyGILState_STATE));
    // Allow Python code
    init_python();
    // Secure GIL
    start_python_code(gil_state);



    // todo Remove this immediately
    StringArray *str_array = malloc(sizeof(StringArray));
    str_array->string_array = malloc(5 * sizeof(char *));
    str_array->string_array = (str *) test_string;
    str_array->length = 5;
    PyObject *list_one = strArrToPyLst(str_array);


    // Get Python-list from args
    // And convert list to array
    {
        PyObject *list = list_one; // todo should be empty see following if clause
        StringArray *string_array;

        //if(!PyArg_ParseTuple(args, "0", &list))
        //    return NULL;
        if ((string_array = pyLstToStrArr(list))) {
            // End python Code release GIL
            exit_python_code(gil_state);
            {
                END_PYTHON_CODE
                Py_BEGIN_ALLOW_THREADS
                    // Split the array into equal pieces
                    size_t amount_chunks = string_array->length / CHUNK_SIZE;
                    size_t left_over = string_array->length % CHUNK_SIZE;
                    if (left_over) amount_chunks++;
                    size_t last_element = amount_chunks -1;

                    char ***ret_value = malloc(amount_chunks*sizeof(char**));

                    size_t thread;
                    pthread_t *thread_handles;
                    thread_handles = malloc(sizeof(pthread_t) * amount_chunks);

                    for (thread = 0; thread < amount_chunks; thread++) {
                        ThreadData arg = {string_array->string_array+(thread*CHUNK_SIZE),
                                          (thread != last_element || left_over == 0) ? CHUNK_SIZE : left_over,
                                          ret_value+(thread*CHUNK_SIZE)};

                        pthread_create(&thread_handles[thread], NULL, mp_search, (void *) &arg);
                    }
                    for (thread = 0; thread < amount_chunks; thread++) {
                        pthread_join(thread_handles[thread], NULL);
                    }
                    free(thread_handles);

                    for(thread = 0; thread < amount_chunks; thread++){
                        for(size_t idx = 0; idx < (thread != last_element || left_over == 0) ? CHUNK_SIZE : left_over; idx++){
                            for(size_t i = 0; i < len(patterns); i++){
                                printf("Found: %s, Index: %lu", ret_value[thread][idx] ? ret_value[thread][idx] : i);
                            }
                        }
                    }


                Py_END_ALLOW_THREADS
            }
        }
        START_PYTHON_CODE
        start_python_code(gil_state);   // Lock GIL
        Py_DecRef(list);                // Free Python-List Object
    }
    exit_python_code(gil_state);        // Release GIL (No use of Threads allowed however!)
    END_PYTHON_CODE
    destruct_python();
}

inline str *get_info(const str string, size_t amount_pattern) {
     * Function: get_info
     * ------------------
     * Function to retrieve matching substrings found with
     * provided patterns.
     *
     * string: The string to regex_search
     * amount_pattern: The max amount of patterns initially created
     *
     * returns: An array of strings containing the results
     *          If an error occurred while compiling or searching for substring
     *          the error message is stored in the last position of the array!


    str *ret_value = malloc((amount_pattern + 1) * sizeof(str));
    for (size_t i = 0; i < amount_pattern; i++) {
        Re *regex = regex_search(regex_compile(patterns[i]), (str) string);
        if (regex->err_code) {
            write_regex_error(regex);
            ret_value[amount_pattern] = strdup(regex->err_msg);
            free_regex(regex);
            return ret_value;
        }
        ret_value[i] = strdup(regex->match->group);
        free_regex(regex);
    }
    return ret_value;
}
*/
