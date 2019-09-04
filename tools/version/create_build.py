"""Script to create a new versioned build of lwpa.

Updates the etcpal/version.h file
Prompts the user to review before committing and tagging

This script is meant to be run by an lwpa developer on a development workstation. It assumes that
git is already configured globally and git commands can be run without further configuration.
"""
import argparse
import os
import sys
import datetime

try:
    import git
except ImportError as ie:
    print('ImportError: {}'.format(ie))
    print('Try "pip install gitpython" before retrying.')
    sys.exit(1)


VERSION_H_IN_FILE_REL_PATH = os.path.join('tools', 'version', 'templates', 'version.h.in')
VERSION_H_OUT_FILE_REL_PATH = os.path.join('include', 'lwpa', 'version.h')
CURRENT_VERSION_TXT_FILE_REL_PATH = os.path.join('tools', 'version', 'current_version.txt')
COMMIT_MSG_TEMPLATE = 'Update version files for lwpa build {}'
TAG_MSG_TEMPLATE = 'lwpa version {}'
RELEASE_TAG_MSG_TEMPLATE = 'lwpa release version {}'


def parse_version(vers_string):
    """Parse a version string into its component parts. Returns a list of ints representing the
    version, or None on parse error."""
    vers_string = vers_string.strip()
    vers_parts = vers_string.split('.')
    if len(vers_parts) != 4:
        return None

    try:
        major = int(vers_parts[0])
        minor = int(vers_parts[1])
        patch = int(vers_parts[2])
        build = int(vers_parts[3])
    except ValueError:
        return None

    return [major, minor, patch, build]


def update_version_files(repo_root, version):
    """Update the version header etcpal/version.h and the current_version.txt file with .he new
    version information. Returns True on success, false otherwise."""

    version_h_in_file_path = os.path.join(repo_root, VERSION_H_IN_FILE_REL_PATH)
    version_h_out_file_path = os.path.join(repo_root, VERSION_H_OUT_FILE_REL_PATH)
    current_version_txt_file_path = os.path.join(repo_root, CURRENT_VERSION_TXT_FILE_REL_PATH)

    # Copy the template file to the output file, replacing the flagged values.
    try:
        version_h_in_file = open(version_h_in_file_path, mode='r', encoding='utf8')
        version_h_out_file = open(version_h_out_file_path, mode='w', encoding='utf8')
        current_version_txt_file = open(current_version_txt_file_path, mode='w', encoding='utf8')
    except OSError:
        print('Failed to open the version header.')
        return False

    today = datetime.date.today()
    version_str = '{}.{}.{}.{}'.format(
        version[0], version[1], version[2], version[3])

    for line in version_h_in_file.readlines():
        line = line.replace('@LWPA_VERSION_MAJOR@', str(version[0]))
        line = line.replace('@LWPA_VERSION_MINOR@', str(version[1]))
        line = line.replace('@LWPA_VERSION_PATCH@', str(version[2]))
        line = line.replace('@LWPA_VERSION_BUILD@', str(version[3]))
        line = line.replace('@LWPA_VERSION_STRING@', version_str)
        line = line.replace('@LWPA_VERSION_DATESTR@', today.strftime('%d.%b.%Y'))
        line = line.replace('@LWPA_VERSION_COPYRIGHT@', 'Copyright ' + str(today.year) + ' ETC Inc.')

        version_h_out_file.write(line)

    current_version_txt_file.write(version_str + '\n')

    version_h_in_file.close()
    version_h_out_file.close()
    current_version_txt_file.close()
    return True


def prompt_to_continue():
    """Prompt the user for whether they would like to continue with the commit and tag operation."""
    print("Check the updated version files, then press 'y' to commit and tag. " +
          "Press any other key to abort.")
    choice = input().strip().lower()
    return True if choice == 'y' else False


def commit_and_tag(repo, new_version, release_build):
    """Commit the updated version files and tag the version."""
    index = repo.index
    index.add([os.path.join(repo.working_tree_dir, VERSION_H_OUT_FILE_REL_PATH),
               os.path.join(repo.working_tree_dir, CURRENT_VERSION_TXT_FILE_REL_PATH)])

    vers_string_long = '{}.{}.{}.{}'.format(new_version[0], new_version[1], new_version[2], new_version[3])
    vers_string_short = '{}.{}.{}'.format(new_version[0], new_version[1], new_version[2])

    index.commit(COMMIT_MSG_TEMPLATE.format(vers_string_long))

    repo.create_tag('v' + vers_string_long, message=TAG_MSG_TEMPLATE.format(vers_string_long))

    if release_build:
        repo.create_tag('v' + vers_string_short, message=RELEASE_TAG_MSG_TEMPLATE.format(vers_string_short))


def main():
    """The script entry point."""

    # Parse the command-line arguments.
    parser = argparse.ArgumentParser(description='Create a new versioned build of lwpa')
    parser.add_argument('new_version', help='New version number (format M.m.p.b)')
    parser.add_argument('-r', '--release', action='store_true', help='Tag a release build of lwpa.')
    args = parser.parse_args()

    new_version = parse_version(args.new_version)
    if not new_version:
        parser.print_usage()
        print('new_version: Format error.')
        sys.exit(1)

    repo_root = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
    lwpa_repo = git.Repo(repo_root)

    if not update_version_files(repo_root, new_version):
        sys.exit(1)

    if not prompt_to_continue():
        sys.exit(0)

    commit_and_tag(lwpa_repo, new_version, args.release)

    print("Done - now push using 'git push origin [branch] --tags'.")


if __name__ == '__main__':
    main()
