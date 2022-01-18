#include <string.h>
#include <regex.h>
#include <malloc.h>
#include "def.h"

#define ERROR_CODE -3

/*static const char *patterns[] = {"'id':[[:space:]]([[:digit:]]+)",
                                 "'name':[[:space:]]'([[:space:][:digit:][:alpha:]-]*)'",
                                 "ssh_url_to_repo':[[:space:]]'(.*)',[[:space:]]'http_url_to_repo",
                                 "last_activity_at':[[:space:]]'([0-9T:.-]*)"};*/

Error *__get_regex_error(Regex *regex, int err_code);
void  *__regex_init__();
Match *__match_init__();
void   __regex_del__(Regex *regex);

/*int main() {
    str test_string2[5] = {
            "#  {'id': 332, 'description': '', 'name': 'canvert2015', 'name_with_namespace': 'Andreas Drexel / canvert2015', 'path': 'canvert2015', 'path_with_namespace': 'andreas.drexel/canvert2015', 'created_at': '2015-04-03T09:49:05.000Z', 'default_branch': 'master', 'tag_list': [], 'topics': [], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/andreas.drexel/canvert2015.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/andreas.drexel/canvert2015.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/andreas.drexel/canvert2015', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2015-07-22T16:17:09.000Z', 'namespace': {'id': 77, 'name': 'Andreas Drexel', 'path': 'andreas.drexel', 'kind': 'user', 'full_path': 'andreas.drexel', 'parent_id': None, 'avatar_url': 'https://seccdn.libravatar.org/avatar/6f37c1ba244e25e821cb3596e66e806e?s=80&d=identicon', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/andreas.drexel'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/332/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 69, 'name': 'Andreas Drexel', 'username': 'andreas.drexel', 'state': 'active', 'avatar_url': 'https://seccdn.libravatar.org/avatar/6f37c1ba244e25e821cb3596e66e806e?s=80&d=identicon', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/andreas.drexel'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': True, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'enabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 69, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
            "#  {'id': 365, 'description': 'Description Project 365', 'name': 'Project 365', 'name_with_namespace': 'Michael Brandl / pi-lfs', 'path': 'pi-lfs', 'path_with_namespace': 'michael.brandl/pi-lfs', 'created_at': '2015-04-17T12:22:08.000Z', 'default_branch': 'master', 'tag_list': ['LFS', 'RPi'], 'topics': ['LFS', 'RPi'], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/michael.brandl/pi-lfs.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2021-04-06T17:05:22.864Z', 'namespace': {'id': 122, 'name': 'Michael Brandl', 'path': 'michael.brandl', 'kind': 'user', 'full_path': 'michael.brandl', 'parent_id': None, 'avatar_url': '/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 111, 'name': 'Michael Brandl', 'username': 'michael.brandl', 'state': 'active', 'avatar_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': False, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'disabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 111, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
            "#  {'id': 150, 'description': 'Description Project 150', 'name': 'Project 150', 'name_with_namespace': 'Michael Brandl / pi-lfs', 'path': 'pi-lfs', 'path_with_namespace': 'michael.brandl/pi-lfs', 'created_at': '2015-04-17T12:22:08.000Z', 'default_branch': 'master', 'tag_list': ['LFS', 'RPi'], 'topics': ['LFS', 'RPi'], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/michael.brandl/pi-lfs.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2021-04-06T17:05:22.864Z', 'namespace': {'id': 122, 'name': 'Michael Brandl', 'path': 'michael.brandl', 'kind': 'user', 'full_path': 'michael.brandl', 'parent_id': None, 'avatar_url': '/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 111, 'name': 'Michael Brandl', 'username': 'michael.brandl', 'state': 'active', 'avatar_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': False, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'disabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 111, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
            "#  {'id': 1, 'description': 'Description Project 1', 'name': 'Project 1', 'name_with_namespace': 'Michael Brandl / pi-lfs', 'path': 'pi-lfs', 'path_with_namespace': 'michael.brandl/pi-lfs', 'created_at': '2015-04-17T12:22:08.000Z', 'default_branch': 'master', 'tag_list': ['LFS', 'RPi'], 'topics': ['LFS', 'RPi'], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/michael.brandl/pi-lfs.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2021-04-06T17:05:22.864Z', 'namespace': {'id': 122, 'name': 'Michael Brandl', 'path': 'michael.brandl', 'kind': 'user', 'full_path': 'michael.brandl', 'parent_id': None, 'avatar_url': '/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 111, 'name': 'Michael Brandl', 'username': 'michael.brandl', 'state': 'active', 'avatar_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': False, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'disabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 111, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
            "#  {'id': 512, 'description': 'Description Project 512', 'name': 'Project 512', 'name_with_namespace': 'Michael Brandl / pi-lfs', 'path': 'pi-lfs', 'path_with_namespace': 'michael.brandl/pi-lfs', 'created_at': '2015-04-17T12:22:08.000Z', 'default_branch': 'master', 'tag_list': ['LFS', 'RPi'], 'topics': ['LFS', 'RPi'], 'ssh_url_to_repo': 'ssh://git@r-n-d.informatik.hs-augsburg.de:2222/michael.brandl/pi-lfs.git', 'http_url_to_repo': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs.git', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl/pi-lfs', 'readme_url': None, 'avatar_url': None, 'forks_count': 0, 'star_count': 0, 'last_activity_at': '2021-04-06T17:05:22.864Z', 'namespace': {'id': 122, 'name': 'Michael Brandl', 'path': 'michael.brandl', 'kind': 'user', 'full_path': 'michael.brandl', 'parent_id': None, 'avatar_url': '/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, '_links': {'self': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365', 'issues': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/issues', 'merge_requests': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/merge_requests', 'repo_branches': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/repository/branches', 'labels': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/labels', 'events': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/events', 'members': 'https://r-n-d.informatik.hs-augsburg.de:8080/api/v4/projects/365/members'}, 'packages_enabled': None, 'empty_repo': False, 'archived': False, 'visibility': 'internal', 'owner': {'id': 111, 'name': 'Michael Brandl', 'username': 'michael.brandl', 'state': 'active', 'avatar_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/uploads/-/system/user/avatar/111/mbrandl.jpg', 'web_url': 'https://r-n-d.informatik.hs-augsburg.de:8080/michael.brandl'}, 'resolve_outdated_diff_discussions': None, 'issues_enabled': True, 'merge_requests_enabled': True, 'wiki_enabled': True, 'jobs_enabled': False, 'snippets_enabled': False, 'container_registry_enabled': False, 'service_desk_enabled': False, 'service_desk_address': None, 'can_create_merge_request_in': True, 'issues_access_level': 'enabled', 'repository_access_level': 'enabled', 'merge_requests_access_level': 'enabled', 'forking_access_level': 'enabled', 'wiki_access_level': 'enabled', 'builds_access_level': 'disabled', 'snippets_access_level': 'disabled', 'pages_access_level': 'public', 'operations_access_level': 'enabled', 'analytics_access_level': 'enabled', 'container_registry_access_level': 'disabled', 'emails_disabled': None, 'shared_runners_enabled': True, 'lfs_enabled': True, 'creator_id': 111, 'import_status': 'none', 'open_issues_count': 0, 'ci_default_git_depth': None, 'ci_forward_deployment_enabled': None, 'ci_job_token_scope_enabled': False, 'public_jobs': True, 'build_timeout': 3600, 'auto_cancel_pending_pipelines': 'enabled', 'build_coverage_regex': None, 'ci_config_path': None, 'shared_with_groups': [], 'only_allow_merge_if_pipeline_succeeds': False, 'allow_merge_on_skipped_pipeline': None, 'restrict_user_defined_variables': False, 'request_access_enabled': True, 'only_allow_merge_if_all_discussions_are_resolved': None, 'remove_source_branch_after_merge': None, 'printing_merge_request_link_enabled': True, 'merge_method': 'merge', 'squash_option': 'default_off', 'suggestion_commit_message': None, 'auto_devops_enabled': False, 'auto_devops_deploy_strategy': 'continuous', 'autoclose_referenced_issues': True, 'keep_latest_artifact': True, 'permissions': {'project_access': None, 'group_access': None}}",
    };
    Regex *ssh_regex = regex_compile(patterns[1]);
    if (!ssh_regex) {
        fprintf(stdout, "%s", "Error occurred during Regex allocation!");
    } else if (ssh_regex->err) {
        fprintf(stdout, "%s", ssh_regex->err->err_msg);
    }

    regex_search(ssh_regex, test_string2[3]);
    if(ssh_regex->err){
        fprintf(stdout, "%s", ssh_regex->err->err_msg);
    }else if(ssh_regex->match->group){
        fprintf(stdout, "%s", "Found: ");
        fprintf(stdout, "%s\n", ssh_regex->match->group);
        fprintf(stdout, "%s", "In: ");
        //fprintf(stdout, "%s", test_string2[3]);
    }


    regex_del(ssh_regex);
    return 0;
}*/

void *__regex_init__() {
    /*
     * Function: __regex_init__
     * ------------------------
     * Allocate memory for new Re structure / object
     * Check for Errors!
     *
     * returns: If Regex-Block cannot be allocated NULL is returned
     *          In any other case a regex-struct pointer is returned.
     *          Regex struct may contain Error if any other allocation was not successful!
     */
    Regex *regex = malloc(sizeof(Regex));
    if (!regex) {
        return NULL;
    }
    regex->match = __match_init__();
    if (!regex->match) {
        regex->err = error_new("RegexMatch-Block could not be allocated", ERROR_CODE);
    }
    return regex;
}

Match *__match_init__() {
    /*
     * Function: __match_init__
     * ------------------------
     * Create new Match with associated memory block
     *
     * returns: Match-Structure
     * rtype:   &Match
     */
    Match *match = malloc(sizeof(Match));
    if (match == NULL)
        return NULL;
    match->span = malloc(sizeof(Span));
    return (!match->span) ? NULL : match;
}

void __regex_del__(Regex *regex) {
    /*
     * Function: __regex_del__
     * -----------------------
     * Deallocate memory allocated by Regex-Structure
     *
     * regex:       Reference to the Regex-struct
     * type regex:  &Regex
     */
    if (regex) {
        free(regex->match->span);
        if (regex->match->group) {
            free(regex->match->group);
        }
        free(regex->match);
        if (regex->pattern) {
            free(regex->pattern);
        }
        regfree(&regex->obj);
        if (regex->err) {
            error_del(regex->err);
        }
        free(regex);
    }
}

void regex_del(Regex *regex) {
    /*
     * Function: regex_del
     * -------------------
     * Deconstruct Regex structure
     *
     * regex:       The Regex-Reference to be deleted
     * type regex:  &Regex
     */
    __regex_del__(regex);
}

Error *__get_regex_error(Regex *regex, int err_code) {
    /*
     * Function: __get_regex_error
     * ---------------------------
     * Retrieve error msg from regex module
     * Parse error msg to reusable error struct and returns said obj
     *
     * regex:           The regex object causing the error
     * type regex:      &Regex
     * err_code:        The error code retrieved from regex-module
     * type err_code:   int
     * returns:         Error-struct containing error-code and msg
     * rtype:           &Error
     */
    size_t length = regerror(err_code, &regex->obj, NULL, 0);
    char buffer[length];
    (void) regerror(err_code, &regex->obj, buffer, length);
    return error_new(buffer, err_code);
}

Regex *regex_compile(const char *pattern) {
    /*
     * Function: compile
     * -----------------
     * Regex implementation to compile a regex_search pattern
     *
     * regex_obj: A regex_t variable
     * pattern: The pattern to regex_compile
     *
     * returns: Re-struct. If error occurred while compilation it is stored in err_code
     *          If Memory could not be allocated NULL value is returned
     *
     * !! Re-memory must to be freed !!
     */
    Regex *regex = __regex_init__();
    if (!regex) {
        return NULL;            // RETURN NULL: if Regex could not be allocated
    } else if (regex->err) {
        return regex;           // RETURN Regex: if Error occurred trying to create Regex
    }
    regex->pattern = strdup(pattern);
    int err_code;
    if (0 != (err_code = regcomp(&regex->obj, regex->pattern, REG_EXTENDED))) {
        regex->err = __get_regex_error(regex, err_code);
    }
    return regex;
}

void regex_search(Regex *regex, str string) {
    /*
     * Function: search
     * ----------------
     * Regex implementation to regex_search for a string in another string
     *
     * regex: A regex-struct / obj containing a pattern etc.
     * string: The string the provided pattern is used for
     *
     * :returns: Regex-object containing the match or error message
     */
    size_t n_match = 2;
    regmatch_t pmatch[n_match];

    int err_code = regexec(&regex->obj, string, n_match, pmatch, 0);
    if (err_code) {
        regex->err = __get_regex_error(regex, err_code);
        return;
    }
    regex->match->span->start = pmatch[1].rm_so + (string - string);
    regex->match->len = (pmatch[1].rm_eo - pmatch[1].rm_so);
    regex->match->span->end = regex->match->span->start + regex->match->len;
    regex->match->group = (str) malloc((regex->match->len + 1) * sizeof(char));
    if (!regex->match->group) {
        regex->err = error_new("Group-Block could not be allocated", ERROR_CODE);
        return;
    }
    slice_string(string, regex->match->group, regex->match->span->start, regex->match->span->end);
}